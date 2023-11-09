/* Rijndael (AES) for GnuPG
 * Copyright (C) 2000, 2001, 2002, 2003, 2007,
 *               2008, 2011, 2012 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 *******************************************************************
 * The code here is based on the optimized implementation taken from
 * http://www.esat.kuleuven.ac.be/~rijmen/rijndael/ on Oct 2, 2000,
 * which carries this notice:
 *------------------------------------------
 * rijndael-alg-fst.c   v2.3   April '2000
 *
 * Optimised ANSI C code
 *
 * authors: v1.0: Antoon Bosselaers
 *          v2.0: Vincent Rijmen
 *          v2.3: Paulo Barreto
 *
 * This code is placed in the public domain.
 *------------------------------------------
 *
 * The SP800-38a document is available at:
 *   http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for memcmp() */

#include "types.h"  /* for byte and u32 typedefs */
#include "bufhelp.h"
#include "rijndael-internal.h"



#ifdef USE_AESNI
/* AES-NI (AMD64 & i386) accelerated implementations of AES */
extern void _gcry_aes_aesni_do_setkey(RIJNDAEL_context *ctx, const byte *key);
extern void _gcry_aes_aesni_prepare_decryption(RIJNDAEL_context *ctx);

extern unsigned int _gcry_aes_aesni_encrypt (const RIJNDAEL_context *ctx,
                                             unsigned char *dst,
                                             const unsigned char *src);
extern unsigned int _gcry_aes_aesni_decrypt (const RIJNDAEL_context *ctx,
                                             unsigned char *dst,
                                             const unsigned char *src);
extern void _gcry_aes_aesni_cfb_enc (void *context, unsigned char *iv,
                                     void *outbuf_arg, const void *inbuf_arg,
                                     size_t nblocks);
extern void _gcry_aes_aesni_cbc_enc (void *context, unsigned char *iv,
                                     void *outbuf_arg, const void *inbuf_arg,
                                     size_t nblocks, int cbc_mac);
extern void _gcry_aes_aesni_ctr_enc (void *context, unsigned char *ctr,
                                     void *outbuf_arg, const void *inbuf_arg,
                                     size_t nblocks);
extern void _gcry_aes_aesni_ctr32le_enc (void *context, unsigned char *ctr,
					 void *outbuf_arg,
					 const void *inbuf_arg, size_t nblocks);
extern void _gcry_aes_aesni_cfb_dec (void *context, unsigned char *iv,
                                     void *outbuf_arg, const void *inbuf_arg,
                                     size_t nblocks);
extern void _gcry_aes_aesni_cbc_dec (void *context, unsigned char *iv,
                                     void *outbuf_arg, const void *inbuf_arg,
                                     size_t nblocks);
extern void _gcry_aes_aesni_xts_crypt (void *context, unsigned char *tweak,
                                       void *outbuf_arg, const void *inbuf_arg,
                                       size_t nblocks, int encrypt);
#endif


/* All the numbers.  */
#include "rijndael-tables.h"



/* Function prototypes.  */
static const char *selftest(void);
static void prepare_decryption(RIJNDAEL_context *ctx);


/* Prefetching for encryption/decryption tables. */
static inline void prefetch_table(const volatile byte *tab, size_t len)
{
    size_t i;

    for (i = 0; len - i >= 8 * 32; i += 8 * 32) {
        (void)tab[i + 0 * 32];
        (void)tab[i + 1 * 32];
        (void)tab[i + 2 * 32];
        (void)tab[i + 3 * 32];
        (void)tab[i + 4 * 32];
        (void)tab[i + 5 * 32];
        (void)tab[i + 6 * 32];
        (void)tab[i + 7 * 32];
    }
    for (; i < len; i += 32) {
        (void)tab[i];
    }

    (void)tab[len - 1];
}

static void prefetch_enc(void)
{
    /* Modify counters to trigger copy-on-write and unsharing if physical pages
    * of look-up table are shared between processes.  Modifying counters also
    * causes checksums for pages to change and hint same-page merging algorithm
    * that these pages are frequently changing.  */
    enc_tables.counter_head++;
    enc_tables.counter_tail++;

    /* Prefetch look-up tables to cache.  */
    prefetch_table((const void *)&enc_tables, sizeof(enc_tables));
}

static void prefetch_dec(void)
{
    /* Modify counters to trigger copy-on-write and unsharing if physical pages
    * of look-up table are shared between processes.  Modifying counters also
    * causes checksums for pages to change and hint same-page merging algorithm
    * that these pages are frequently changing.  */
    dec_tables.counter_head++;
    dec_tables.counter_tail++;

    /* Prefetch look-up tables to cache.  */
    prefetch_table((const void *)&dec_tables, sizeof(dec_tables));
}


/* Perform the key setup.  */
static int
do_setkey (RIJNDAEL_context *ctx, const byte *key, const unsigned keylen)
{
    static int initialized = 0;
    static const char *selftest_failed = 0;
    void (*hw_setkey)(RIJNDAEL_context * ctx, const byte *key) = NULL;
    int rounds;
    int i, j, r, t, rconpointer = 0;
    int KC;
    unsigned int hwfeatures;

    /* The on-the-fly self tests are only run in non-fips mode. In fips
        mode explicit self-tests are required.  Actually the on-the-fly
        self-tests are not fully thread-safe and it might happen that a
        failed self-test won't get noticed in another thread.

        FIXME: We might want to have a central registry of succeeded
        self-tests. */

    if (keylen == 128 / 8) {
        rounds = 10;
        KC = 4;
    } else if (keylen == 192 / 8) {
        rounds = 12;
        KC = 6;
    } else if (keylen == 256 / 8) {
        rounds = 14;
        KC = 8;
    } else
        return -1;

    ctx->rounds = rounds;

    ctx->decryption_prepared = 0;

    (void)hwfeatures;

    if (0) {
        ;
    }
    #ifdef USE_AESNI
    else if (1) {
        hw_setkey = _gcry_aes_aesni_do_setkey;
        ctx->encrypt_fn = _gcry_aes_aesni_encrypt;
        ctx->decrypt_fn = _gcry_aes_aesni_decrypt;
        ctx->prefetch_enc_fn = NULL;
        ctx->prefetch_dec_fn = NULL;
        ctx->prepare_decryption = _gcry_aes_aesni_prepare_decryption;
        ctx->use_avx = 1;
        ctx->use_avx2 = 0;
    }
    #endif

    /* NB: We don't yet support Padlock hardware key generation.  */

    if (hw_setkey) {
        hw_setkey(ctx, key);
    }

    return 0;
}


static int
rijndael_setkey (void *context, const byte *key, const unsigned keylen)
{
    RIJNDAEL_context *ctx = context;
    return do_setkey(ctx, key, keylen);
}

static unsigned int
rijndael_encrypt (void *context, byte *b, const byte *a, size_t nblock)
{
    RIJNDAEL_context *ctx = context;

    if (ctx->prefetch_enc_fn)
        ctx->prefetch_enc_fn();

    byte *dst = b;
    byte *src = a;

    int i = 0;
    for (i = 0; i < nblock; i++) {
        ctx->encrypt_fn(ctx, dst, src);
        dst += BLOCKSIZE;
        src += BLOCKSIZE;
    }

    return 0;
}


static inline void
check_decryption_preparation (RIJNDAEL_context *ctx)
{
    if (!ctx->decryption_prepared) {
        ctx->prepare_decryption(ctx);
        ctx->decryption_prepared = 1;
    }
}


static unsigned int
rijndael_decrypt (void *context, byte *b, const byte *a,  size_t nblock)
{
    RIJNDAEL_context *ctx = context;

    check_decryption_preparation(ctx);

    if (ctx->prefetch_dec_fn)
        ctx->prefetch_dec_fn();

    byte *dst = b;
    byte *src = a;

    int i = 0;

    for (i = 0; i < nblock; i++) {
        ctx->decrypt_fn(ctx, dst, src);
        dst += BLOCKSIZE;
        src += BLOCKSIZE;
    }

    return 0;
}


#if 0


#endif

void selftest_128()
{
    static const unsigned char plaintext_128[32] =
    {
      0x01,0x4B,0xAF,0x22,0x78,0xA6,0x9D,0x33,
      0x1D,0x51,0x80,0x10,0x36,0x43,0xE9,0x9A,
      0x01,0x4B,0xAF,0x22,0x78,0xA6,0x9D,0x33,
      0x1D,0x51,0x80,0x10,0x36,0x43,0xE9,0x9A
    };
  static const unsigned char key_128[16] =
    {
      0xE8,0xE9,0xEA,0xEB,0xED,0xEE,0xEF,0xF0,
      0xF2,0xF3,0xF4,0xF5,0xF7,0xF8,0xF9,0xFA
    };
  static const unsigned char ciphertext_128[32] =
    {
      0x67,0x43,0xC3,0xD1,0x51,0x9A,0xB4,0xF2,
      0xCD,0x9A,0x78,0xAB,0x09,0xA5,0x11,0xBD,
      0x67,0x43,0xC3,0xD1,0x51,0x9A,0xB4,0xF2,
      0xCD,0x9A,0x78,0xAB,0x09,0xA5,0x11,0xBD
    };

    byte scratch[32] = {0};
    RIJNDAEL_context context = {0};

    do_setkey(&context, key_128, 16);

    rijndael_encrypt(&context, scratch, plaintext_128, 2);

    if (memcmp (scratch, ciphertext_128, sizeof (ciphertext_128))) {
        printf("AES-128 test encryption failed.\n");
    } else {
        printf("AES-128 test encryption success.\n");
        int i = 0;
        for (i = 0; i < 32; i++) {
            printf("%02x ", scratch[i]);
        }
        printf("\n");

        for (i = 0; i < 32; i++) {
            printf("%02x ", ciphertext_128[i]);
        }
        printf("\n");
    }

    rijndael_decrypt(&context, scratch, scratch, 2);

    if (memcmp (scratch, plaintext_128, sizeof (plaintext_128))) {
        printf("AES-128 test decryption failed.\n");
    } else {
        printf("AES-128 test decryption success.\n");
        int i = 0;
        for (i = 0; i < 32; i++) {
            printf("%02x ", scratch[i]);
        }
        printf("\n");

        for (i = 0; i < 32; i++) {
            printf("%02x ", plaintext_128[i]);
        }
        printf("\n");

    }


}

void selftest_cbc_128()
{
    static const unsigned char plaintext_128[32] =
    {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,0x77, 0x88,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,0x77, 0x88,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,0x77, 0x88,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,0x77, 0x88
    };
    static const unsigned char key_128[16] =
    {
        0x31, 0x31, 0x32, 0x32, 0x33, 0x33,0x34, 0x34,
        0x35, 0x35, 0x36, 0x36, 0x37, 0x37,0x38, 0x38
    };
    static const unsigned char ciphertext_128[32] =
    {
        0x68, 0xB5, 0xB0, 0xEF, 0xC0, 0x82, 0x95, 0xD2,  
        0xB7, 0x40, 0x51, 0xA4, 0x1A, 0x74, 0xCE, 0x2F,
        0x90, 0xA1, 0x0C, 0xE2, 0x8D, 0x70, 0xDC, 0x85,  
        0xA3, 0x1C, 0x30, 0xFC, 0x19, 0xF2, 0xE0, 0x71
    };
    unsigned char iv[16] = 
    {
        0x31, 0x31, 0x31, 0x31, 0x31, 0x31,0x31, 0x31,
        0x31, 0x31, 0x31, 0x31, 0x31, 0x31,0x31, 0x31
    };

    byte scratch[32] = {0};
    RIJNDAEL_context context = {0};

    do_setkey(&context, key_128, 16);

    _gcry_aes_aesni_cbc_enc(&context, iv, scratch, plaintext_128, 2, 0);

    if (memcmp (scratch, ciphertext_128, sizeof (ciphertext_128))) {
        printf("AES-128 test encryption failed.\n");
    } else {
        printf("AES-128 test encryption success.\n");
        int i = 0;
        for (i = 0; i < 32; i++) {
            printf("%02x ", scratch[i]);
        }
        printf("\n");

        for (i = 0; i < 32; i++) {
            printf("%02x ", ciphertext_128[i]);
        }
        printf("\n");
    }

    memset(iv, 0x31, 16);
    _gcry_aes_aesni_cbc_dec(&context, iv, scratch, scratch, 2);

    if (memcmp (scratch, plaintext_128, sizeof (plaintext_128))) {
        printf("AES-128 test decryption failed.\n");
    } else {
        printf("AES-128 test decryption success.\n");
        int i = 0;
        for (i = 0; i < 32; i++) {
            printf("%02x ", scratch[i]);
        }
        printf("\n");

        for (i = 0; i < 32; i++) {
            printf("%02x ", plaintext_128[i]);
        }
        printf("\n");
    }
}

void selftest_192()
{
    static unsigned char plaintext_192[16] =
    {
        0x76,0x77,0x74,0x75,0xF1,0xF2,0xF3,0xF4,
        0xF8,0xF9,0xE6,0xE7,0x77,0x70,0x71,0x72
    };
    static unsigned char key_192[24] =
    {
        0x04,0x05,0x06,0x07,0x09,0x0A,0x0B,0x0C,
        0x0E,0x0F,0x10,0x11,0x13,0x14,0x15,0x16,
        0x18,0x19,0x1A,0x1B,0x1D,0x1E,0x1F,0x20
    };
    static const unsigned char ciphertext_192[16] =
    {
        0x5D,0x1E,0xF2,0x0D,0xCE,0xD6,0xBC,0xBC,
        0x12,0x13,0x1A,0xC7,0xC5,0x47,0x88,0xAA
    };

    byte scratch[16] = {0};
    RIJNDAEL_context context = {0};

    do_setkey(&context, key_192, 24);

    rijndael_encrypt(&context, scratch, plaintext_192, 1);

    if (memcmp (scratch, ciphertext_192, sizeof (ciphertext_192))) {
        printf("AES-192 test encryption failed.\n");
    } else {
        printf("AES-192 test encryption success.\n");
        int i = 0;
        for (i = 0; i < 16; i++) {
            printf("%02x ", scratch[i]);
        }
        printf("\n");

        for (i = 0; i < 16; i++) {
            printf("%02x ", ciphertext_192[i]);
        }
        printf("\n");
    }

    rijndael_decrypt(&context, scratch, scratch, 1);

    if (memcmp (scratch, plaintext_192, sizeof (plaintext_192))) {
        printf("AES-192 test decryption failed.\n");
    } else {
        printf("AES-192 test decryption success.\n");
        int i = 0;
        for (i = 0; i < 16; i++) {
            printf("%02x ", scratch[i]);
        }
        printf("\n");

        for (i = 0; i < 16; i++) {
            printf("%02x ", plaintext_192[i]);
        }
        printf("\n");
    }
}

void selftest_256()
{
    static unsigned char plaintext_256[16] =
    {
        0x06,0x9A,0x00,0x7F,0xC7,0x6A,0x45,0x9F,
        0x98,0xBA,0xF9,0x17,0xFE,0xDF,0x95,0x21
    };

    static unsigned char key_256[32] =
    {
        0x08,0x09,0x0A,0x0B,0x0D,0x0E,0x0F,0x10,
        0x12,0x13,0x14,0x15,0x17,0x18,0x19,0x1A,
        0x1C,0x1D,0x1E,0x1F,0x21,0x22,0x23,0x24,
        0x26,0x27,0x28,0x29,0x2B,0x2C,0x2D,0x2E
    };

    static const unsigned char ciphertext_256[16] =
    {
        0x08,0x0E,0x95,0x17,0xEB,0x16,0x77,0x71,
        0x9A,0xCF,0x72,0x80,0x86,0x04,0x0A,0xE3
    };

    byte scratch[16] = {0};
    RIJNDAEL_context context = {0};

    do_setkey(&context, key_256, 32);

    rijndael_encrypt(&context, scratch, plaintext_256, 1);

    if (memcmp (scratch, ciphertext_256, sizeof (ciphertext_256))) {
        printf("AES-256 test encryption failed.\n");
    } else {
        printf("AES-256 test encryption success.\n");
        int i = 0;
        for (i = 0; i < 16; i++) {
            printf("%02x ", scratch[i]);
        }
        printf("\n");

        for (i = 0; i < 16; i++) {
            printf("%02x ", ciphertext_256[i]);
        }
        printf("\n");
    }

    rijndael_decrypt(&context, scratch, scratch, 1);

    if (memcmp (scratch, plaintext_256, sizeof (plaintext_256))) {
        printf("AES-256 test decryption failed.\n");
    } else {
        printf("AES-256 test decryption success.\n");
        int i = 0;
        for (i = 0; i < 16; i++) {
            printf("%02x ", scratch[i]);
        }
        printf("\n");

        for (i = 0; i < 16; i++) {
            printf("%02x ", plaintext_256[i]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;
    RIJNDAEL_context ctx = {0};
    byte key[16] = {0};

    do_setkey(&ctx, key, 16);

    int i = 0, j = 0;

    for (i = 0; i < ctx.rounds; i++) {
        for (j = 0; j < 4; j++) {
            printf("%02x ", ctx.keyschenc32[i][j]);
        }
        printf("\n");
    }

    selftest_128();

    selftest_192();

    selftest_256();


    selftest_cbc_128();


    return ret;
}