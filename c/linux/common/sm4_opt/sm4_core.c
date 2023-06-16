#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#include "sm4_core.h"


#define asmlinkage __attribute__((regparm(0)))

asmlinkage void sm4_aesni_avx_crypt4(const u32 *rk, u8 *dst,
				const u8 *src, int nblocks);
asmlinkage void sm4_aesni_avx_crypt8(const u32 *rk, u8 *dst,
				const u8 *src, int nblocks);
asmlinkage void sm4_aesni_avx_ctr_enc_blk8(const u32 *rk, u8 *dst,
				const u8 *src, u8 *iv);
asmlinkage void sm4_aesni_avx_cbc_dec_blk8(const u32 *rk, u8 *dst,
				const u8 *src, u8 *iv);
asmlinkage void sm4_aesni_avx_cfb_dec_blk8(const u32 *rk, u8 *dst,
				const u8 *src, u8 *iv);


static const u32 fk[4] = {
	0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc
};

static const u32 ____cacheline_aligned ck[32] = {
	0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
	0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
	0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
	0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
	0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
	0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
	0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
	0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279
};

static const u8 ____cacheline_aligned sbox[256] = {
	0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7,
	0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
	0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3,
	0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
	0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a,
	0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
	0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95,
	0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
	0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba,
	0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
	0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b,
	0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
	0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2,
	0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
	0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52,
	0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
	0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5,
	0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
	0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55,
	0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
	0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60,
	0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
	0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f,
	0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
	0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f,
	0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
	0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd,
	0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
	0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e,
	0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
	0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20,
	0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48
};

#define min(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);	\
	_x < _y ? _x : _y; })

#define max(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);	\
	_x > _y ? _x : _y; })

/**
 * rol32 - rotate a 32-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline u32 rol32(u32 word, unsigned int shift)
{
	return (word << (shift & 31)) | (word >> ((-shift) & 31));
}

static inline u32 sm4_t_non_lin_sub(u32 x)
{
	u32 out;

	out  = (u32)sbox[x & 0xff];
	out |= (u32)sbox[(x >> 8) & 0xff] << 8;
	out |= (u32)sbox[(x >> 16) & 0xff] << 16;
	out |= (u32)sbox[(x >> 24) & 0xff] << 24;

	return out;
}

static inline u32 sm4_key_lin_sub(u32 x)
{
	return x ^ rol32(x, 13) ^ rol32(x, 23);
}

static inline u32 sm4_enc_lin_sub(u32 x)
{
	return x ^ rol32(x, 2) ^ rol32(x, 10) ^ rol32(x, 18) ^ rol32(x, 24);
}

static inline u32 sm4_key_sub(u32 x)
{
	return sm4_key_lin_sub(sm4_t_non_lin_sub(x));
}

static inline u32 sm4_enc_sub(u32 x)
{
	return sm4_enc_lin_sub(sm4_t_non_lin_sub(x));
}

static inline u32 sm4_round(u32 x0, u32 x1, u32 x2, u32 x3, u32 rk)
{
	return x0 ^ sm4_enc_sub(x1 ^ x2 ^ x3 ^ rk);
}

static inline void crypto_xor_cpy(u8 *dst, const u8 *src1, const u8 *src2,
				  unsigned int size)
{
	if ((size % sizeof(unsigned long)) == 0) {
		unsigned long *d = (unsigned long *)dst;
		unsigned long *s1 = (unsigned long *)src1;
		unsigned long *s2 = (unsigned long *)src2;

		while (size > 0) {
			*d++ = *s1++ ^ *s2++;
			size -= sizeof(unsigned long);
		}
	} else {
		while (size > 0) {
            *dst++ = *src1++ ^ *src2++;
            size--;
        }
	}
}
static inline u16 swab16(u16 x)
{
	return  ((x & (u16)0x00ffU) << 8) |
		((x & (u16)0xff00U) >> 8);
}

static inline u32 swab32(u32 x)
{
	return  ((x & (u32)0x000000ffUL) << 24) |
		((x & (u32)0x0000ff00UL) <<  8) |
		((x & (u32)0x00ff0000UL) >>  8) |
		((x & (u32)0xff000000UL) >> 24);
}

static inline u64 swab64(u64 x)
{
	return  (u64)((x & (u64)0x00000000000000ffULL) << 56) |
		(u64)((x & (u64)0x000000000000ff00ULL) << 40) |
		(u64)((x & (u64)0x0000000000ff0000ULL) << 24) |
		(u64)((x & (u64)0x00000000ff000000ULL) <<  8) |
		(u64)((x & (u64)0x000000ff00000000ULL) >>  8) |
		(u64)((x & (u64)0x0000ff0000000000ULL) >> 24) |
		(u64)((x & (u64)0x00ff000000000000ULL) >> 40) |
		(u64)((x & (u64)0xff00000000000000ULL) >> 56);
}

#define cpu_to_be16(x) swab16(x)
#define be16_to_cpu(x) swab16(x)
#define cpu_to_be32(x) swab32(x)
#define be32_to_cpu(x) swab32(x)
#define cpu_to_be64(x) swab64(x)
#define be64_to_cpu(x) swab64(x)

#define __packed __attribute__((__packed__))

#define __get_unaligned_t(type, ptr) ({						\
	const struct { type x; } __packed *__pptr = (typeof(__pptr))(ptr);	\
	__pptr->x;								\
})

static inline u32 get_unaligned_be32(const void *p)
{
	return be32_to_cpu(__get_unaligned_t(u32, p));
}


/**
 * @brief 设置密钥
 * 
 * @param ctx context
 * @param key key
 * @param key_len key length, must be 16 
 * @return int 
 */
int sm4_skcipher_setkey(struct sm4_context *ctx, const unsigned char *key, unsigned int key_len)
{
    u32 rk[4];
	int i;
	u32 *inkey = (u32 *)key;

	if (key_len != SM4_KEY_SIZE)
		return -EINVAL;

	rk[0] = get_unaligned_be32(&inkey[0]) ^ fk[0];
	rk[1] = get_unaligned_be32(&inkey[1]) ^ fk[1];
	rk[2] = get_unaligned_be32(&inkey[2]) ^ fk[2];
	rk[3] = get_unaligned_be32(&inkey[3]) ^ fk[3];

	for (i = 0; i < 32; i += 4) {
		rk[0] ^= sm4_key_sub(rk[1] ^ rk[2] ^ rk[3] ^ ck[i + 0]);
		rk[1] ^= sm4_key_sub(rk[2] ^ rk[3] ^ rk[0] ^ ck[i + 1]);
		rk[2] ^= sm4_key_sub(rk[3] ^ rk[0] ^ rk[1] ^ ck[i + 2]);
		rk[3] ^= sm4_key_sub(rk[0] ^ rk[1] ^ rk[2] ^ ck[i + 3]);

		ctx->rkey_enc[i + 0] = rk[0];
		ctx->rkey_enc[i + 1] = rk[1];
		ctx->rkey_enc[i + 2] = rk[2];
		ctx->rkey_enc[i + 3] = rk[3];
		ctx->rkey_dec[31 - 0 - i] = rk[0];
		ctx->rkey_dec[31 - 1 - i] = rk[1];
		ctx->rkey_dec[31 - 2 - i] = rk[2];
		ctx->rkey_dec[31 - 3 - i] = rk[3];
	}

	return 0;
}

/**
 * @brief 一个block运算，加密或解密，供实现其它模式使用
 * 
 * @param rkey 轮密钥
 * @param out 
 * @param in 
 * @return int 
 */
int sm4_crypt_block(const unsigned int *rkey, unsigned char *out, unsigned char *in)
{
    sm4_aesni_avx_crypt4((u32 *)rkey, out, in, 1);
    return 0;
}

/**
 * @brief ecb模式加密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @return int 
 */
int sm4_avx_ecb_encrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len)
{
    unsigned int nbytes;
	nbytes = in_len;
    while (nbytes > 0) {
		const u8 *src = in;
		u8 *dst = out;

		while (nbytes >= SM4_CRYPT8_BLOCK_SIZE) {
			sm4_aesni_avx_crypt8(ctx->rkey_enc, dst, src, 8);
			dst += SM4_CRYPT8_BLOCK_SIZE;
			src += SM4_CRYPT8_BLOCK_SIZE;
			nbytes -= SM4_CRYPT8_BLOCK_SIZE;
		}
		while (nbytes >= SM4_BLOCK_SIZE) {
			unsigned int nblocks = min(nbytes >> 4, 4u);
			sm4_aesni_avx_crypt4(ctx->rkey_enc, dst, src, nblocks);
			dst += nblocks * SM4_BLOCK_SIZE;
			src += nblocks * SM4_BLOCK_SIZE;
			nbytes -= nblocks * SM4_BLOCK_SIZE;
		}
	}

    return 0;
}

/**
 * @brief ecb模式解密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @return int 
 */
int sm4_avx_ecb_decrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len)
{
    unsigned int nbytes;
	nbytes = in_len;
    while (nbytes > 0) {
		const u8 *src = in;
		u8 *dst = out;

		while (nbytes >= SM4_CRYPT8_BLOCK_SIZE) {
			sm4_aesni_avx_crypt8(ctx->rkey_dec, dst, src, 8);
			dst += SM4_CRYPT8_BLOCK_SIZE;
			src += SM4_CRYPT8_BLOCK_SIZE;
			nbytes -= SM4_CRYPT8_BLOCK_SIZE;
		}
		while (nbytes >= SM4_BLOCK_SIZE) {
			unsigned int nblocks = min(nbytes >> 4, 4u);
			sm4_aesni_avx_crypt4(ctx->rkey_dec, dst, src, nblocks);
			dst += nblocks * SM4_BLOCK_SIZE;
			src += nblocks * SM4_BLOCK_SIZE;
			nbytes -= nblocks * SM4_BLOCK_SIZE;
		}
	}

    return 0;
}

/**
 * @brief cbc模式加密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_cbc_encrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len)
{
	unsigned int nbytes;
	int err = 0;

	nbytes = in_len;

	while (nbytes > 0) {
		const u8 *walk_iv = iv;
		const u8 *src = in;
		u8 *dst = out;

		while (nbytes >= SM4_BLOCK_SIZE) {
			crypto_xor_cpy(dst, src, walk_iv, SM4_BLOCK_SIZE);
			sm4_crypt_block(ctx->rkey_enc, dst, dst);
			walk_iv = dst;
			src += SM4_BLOCK_SIZE;
			dst += SM4_BLOCK_SIZE;
			nbytes -= SM4_BLOCK_SIZE;
		}
		if (iv != walk_iv)
			memcpy(iv, walk_iv, SM4_BLOCK_SIZE);
	}

	return err;    
}

/**
 * @brief cbc模式解密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_cbc_decrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len)
{
	unsigned int nbytes;
	int err = 0;

	nbytes = in_len;

	while (nbytes > 0) {
		const u8 *src = in;
		u8 *dst = out;
		u8 *walk_iv = iv;

		while (nbytes >= SM4_CRYPT8_BLOCK_SIZE) {
			sm4_aesni_avx_cbc_dec_blk8(ctx->rkey_dec, dst, src, walk_iv);
			dst += SM4_CRYPT8_BLOCK_SIZE;
			src += SM4_CRYPT8_BLOCK_SIZE;
			nbytes -= SM4_CRYPT8_BLOCK_SIZE;
		}

		while (nbytes >= SM4_BLOCK_SIZE) {
			u8 keystream[SM4_BLOCK_SIZE * 8];
			u8 tmp_iv[SM4_BLOCK_SIZE];
			unsigned int nblocks = min(nbytes >> 4, 8u);
			int i;

			sm4_aesni_avx_crypt8(ctx->rkey_dec, keystream,
						src, nblocks);

			src += ((int)nblocks - 2) * SM4_BLOCK_SIZE;
			dst += (nblocks - 1) * SM4_BLOCK_SIZE;
			memcpy(tmp_iv, src + SM4_BLOCK_SIZE, SM4_BLOCK_SIZE);

			for (i = nblocks - 1; i > 0; i--) {
				crypto_xor_cpy(dst, src,
					&keystream[i * SM4_BLOCK_SIZE],
					SM4_BLOCK_SIZE);
				src -= SM4_BLOCK_SIZE;
				dst -= SM4_BLOCK_SIZE;
			}
			crypto_xor_cpy(dst, walk_iv, keystream, SM4_BLOCK_SIZE);
			memcpy(walk_iv, tmp_iv, SM4_BLOCK_SIZE);
			dst += nblocks * SM4_BLOCK_SIZE;
			src += (nblocks + 1) * SM4_BLOCK_SIZE;
			nbytes -= nblocks * SM4_BLOCK_SIZE;
		}
	}

	return err;    
}

/**
 * @brief cfb模式加密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_cfb_encrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len)
{
	unsigned int nbytes;
	int err = 0;

	nbytes = in_len;

	while (nbytes > 0) {
		u8 keystream[SM4_BLOCK_SIZE];
		const u8 *walk_iv = iv;
		const u8 *src = in;
		u8 *dst = out;

		while (nbytes >= SM4_BLOCK_SIZE) {
			sm4_crypt_block(ctx->rkey_enc, keystream, walk_iv);
			crypto_xor_cpy(dst, src, keystream, SM4_BLOCK_SIZE);
			walk_iv = dst;
			src += SM4_BLOCK_SIZE;
			dst += SM4_BLOCK_SIZE;
			nbytes -= SM4_BLOCK_SIZE;
		}
		if (iv != walk_iv)
			memcpy(iv, walk_iv, SM4_BLOCK_SIZE);

		/* tail */
		if (nbytes > 0) {
			sm4_crypt_block(ctx->rkey_enc, keystream, walk_iv);
			crypto_xor_cpy(dst, src, keystream, nbytes);
			nbytes = 0;
		}
	}

	return err;
}

/**
 * @brief cfb模式解密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_cfb_decrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len)
{
	unsigned int nbytes;
	int err = 0;


	nbytes = in_len;

	while (nbytes > 0) {
		const u8 *src = in;
		u8 *dst = out;

		while (nbytes >= SM4_CRYPT8_BLOCK_SIZE) {
			sm4_aesni_avx_cfb_dec_blk8(ctx->rkey_enc, dst, src, iv);
			dst += SM4_CRYPT8_BLOCK_SIZE;
			src += SM4_CRYPT8_BLOCK_SIZE;
			nbytes -= SM4_CRYPT8_BLOCK_SIZE;
		}

		while (nbytes >= SM4_BLOCK_SIZE) {
			u8 keystream[SM4_BLOCK_SIZE * 8];
			unsigned int nblocks = min(nbytes >> 4, 8u);

			memcpy(keystream, iv, SM4_BLOCK_SIZE);
			if (nblocks > 1)
				memcpy(&keystream[SM4_BLOCK_SIZE], src,
					(nblocks - 1) * SM4_BLOCK_SIZE);
			memcpy(iv, src + (nblocks - 1) * SM4_BLOCK_SIZE,
				SM4_BLOCK_SIZE);

			sm4_aesni_avx_crypt8(ctx->rkey_enc, keystream,
						keystream, nblocks);

			crypto_xor_cpy(dst, src, keystream,
					nblocks * SM4_BLOCK_SIZE);
			dst += nblocks * SM4_BLOCK_SIZE;
			src += nblocks * SM4_BLOCK_SIZE;
			nbytes -= nblocks * SM4_BLOCK_SIZE;
		}

		/* tail */
		if (nbytes > 0) {
			u8 keystream[SM4_BLOCK_SIZE];

			sm4_crypt_block(ctx->rkey_enc, keystream, iv);
			crypto_xor_cpy(dst, src, keystream, nbytes);
			nbytes = 0;
		}

	}

	return err;
}

/**
 * @brief ofb模式加密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_ofb_encrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len)
{
	unsigned int nbytes;
	int err = 0;

	nbytes = in_len;

	while (nbytes > 0) {
		u8 keystream[SM4_BLOCK_SIZE];
		const u8 *walk_iv = iv;
		const u8 *src = in;
		u8 *dst = out;

		while (nbytes >= SM4_BLOCK_SIZE) {
			sm4_crypt_block(ctx->rkey_enc, keystream, walk_iv);
#if 0
			int i = 0;
			for (i = 0; i < SM4_BLOCK_SIZE; i++) {
				printf("%02x ", keystream[i]);
			}
			printf("\n");
#endif
			crypto_xor_cpy(dst, src, keystream, SM4_BLOCK_SIZE);
			walk_iv = keystream;
			src += SM4_BLOCK_SIZE;
			dst += SM4_BLOCK_SIZE;
			nbytes -= SM4_BLOCK_SIZE;
		}
		if (iv != walk_iv)
			memcpy(iv, walk_iv, SM4_BLOCK_SIZE);

		/* tail */
		if (nbytes > 0) {
			sm4_crypt_block(ctx->rkey_enc, keystream, walk_iv);
			crypto_xor_cpy(dst, src, keystream, nbytes);
			nbytes = 0;
		}
	}

	return err;
}

/**
 * @brief ofb模式解密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_ofb_decrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len)
{
	unsigned int nbytes;
	int err = 0;

	nbytes = in_len;

	while (nbytes > 0) {
		u8 keystream[SM4_BLOCK_SIZE];
		const u8 *walk_iv = iv;
		const u8 *src = in;
		u8 *dst = out;

		while (nbytes >= SM4_BLOCK_SIZE) {
			sm4_crypt_block(ctx->rkey_enc, keystream, walk_iv);
#if 0
			int i = 0;
			for (i = 0; i < SM4_BLOCK_SIZE; i++) {
				printf("%02x ", keystream[i]);
			}
			printf("\n");
#endif
			crypto_xor_cpy(dst, src, keystream, SM4_BLOCK_SIZE);
			walk_iv = keystream;
			src += SM4_BLOCK_SIZE;
			dst += SM4_BLOCK_SIZE;
			nbytes -= SM4_BLOCK_SIZE;
		}
		if (iv != walk_iv)
			memcpy(iv, walk_iv, SM4_BLOCK_SIZE);

		/* tail */
		if (nbytes > 0) {
			sm4_crypt_block(ctx->rkey_enc, keystream, walk_iv);
			crypto_xor_cpy(dst, src, keystream, nbytes);
			nbytes = 0;
		}
	}

	return err;
}