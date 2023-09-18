#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>


#include <openssl/evp.h>

unsigned char key[24] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5,
                         0x6, 0x7, 0x8, 0x9, 0xa, 0xb,
                         0xc, 0xd, 0xe, 0xf, 0x0, 0x1,
                         0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
unsigned char iv[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
unsigned char plaintext[64] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                               0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 
                               0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 
                               0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 
                               0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 
                               0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 
                               0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 
                               0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
unsigned char ciphertext[1024] = {0};


int dump_hex(unsigned char *buf, int len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
    return 0;
}

int enc_3des_cbc(unsigned char *plaintext, int plaintext_len, unsigned char *key, int key_len,
                    unsigned char *iv, int iv_len, unsigned char *ciphertext, int *ciphertext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        printf("EVP_CIPHER_CTX_new failed\n");
        ret = -1;
        goto out;
    }    

    const EVP_CIPHER *cipher = EVP_des_ede3_cbc();

    if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv)) {
        printf("EVP_EncryptInit_ex failed\n");
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, ciphertext_len, plaintext, plaintext_len)) {
        printf("EVP_EncryptUpdate failed\n");
        ret = -1;
        goto free_ctx;
    }

    int len = 0;
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + *ciphertext_len, &len)) {
        printf("EVP_EncryptFinal_ex failed\n");
        ret = -1;
        goto free_ctx;
    }
    *ciphertext_len += len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}

int dec_3des_cbc(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, int key_len,
                    unsigned char *iv, int iv_len, unsigned char *plaintext, int *plaintext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        printf("EVP_CIPHER_CTX_new failed\n");
        ret = -1;
        goto out;
    }    

    const EVP_CIPHER *cipher = EVP_des_ede3_cbc();

    if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv)) {
        printf("EVP_DecryptInit_ex failed\n");
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext, plaintext_len, ciphertext, ciphertext_len)) {
        printf("EVP_DecryptUpdate failed\n");
        ret = -1;
        goto free_ctx;
    }

    int len = 0;
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + *plaintext_len, &len)) {
        printf("EVP_DecryptFinal_ex failed\n");
        ret = -1;
        goto free_ctx;
    }
    *plaintext_len += len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}


int des3_crypto(const EVP_CIPHER *cipher, int encrypt, unsigned char *in, int in_len, unsigned char *key, int key_len,
                    unsigned char *iv, int iv_len, unsigned char *out, int *out_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        printf("EVP_CIPHER_CTX_new failed\n");
        ret = -1;
        goto out;
    }    

    // const EVP_CIPHER *cipher = EVP_des_ede3_cbc();

    if (1 == encrypt) {
        if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv)) {
            printf("EVP_EncryptInit_ex failed\n");
            ret = -1;
            goto free_ctx;
        }
    } else {
        if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv)) {
            printf("EVP_DecryptInit_ex failed\n");
            ret = -1;
            goto free_ctx;
        }
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    int len = 0;
    if (1 == encrypt) {
        if (1 != EVP_EncryptUpdate(ctx, out, out_len, in, in_len)) {
            printf("EVP_EncryptUpdate failed\n");
            ret = -1;
            goto free_ctx;
        }
        if (1 != EVP_EncryptFinal_ex(ctx, out + *out_len, &len)) {
            printf("EVP_EncryptFinal_ex failed\n");
            ret = -1;
            goto free_ctx;
        }
    } else {
        if (1 != EVP_DecryptUpdate(ctx, out, out_len, in, in_len)) {
            printf("EVP_DecryptUpdate failed\n");
            ret = -1;
            goto free_ctx;
        }
        if (1 != EVP_DecryptFinal_ex(ctx, out + *out_len, &len)) {
            printf("EVP_DecryptFinal_ex failed\n");
            ret = -1;
            goto free_ctx;
        }
    }
    *out_len += len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}



int main(int argc, char *argv[])
{
    int ret = 0;

    printf("\n*********************3des cbc test*********************\n");
    int ciphertext_len = 0;
    int plaintext_len = 0;

    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    ret = enc_3des_cbc(plaintext, sizeof(plaintext), key, sizeof(key), iv, sizeof(iv), ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("enc_3des_cbc failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", ciphertext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = dec_3des_cbc(ciphertext, ciphertext_len, key, sizeof(key), iv, sizeof(iv), plaintext, &plaintext_len);
    if (ret != 0) {
        printf("dec_3des_cbc failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

    printf("\n*********************3des crypto cbc test*********************\n");
    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    const EVP_CIPHER *cipher = EVP_des_ede3_cbc();
    ret = des3_crypto(cipher, 1, plaintext, sizeof(plaintext), key, sizeof(key), iv, sizeof(iv), ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("des3_crypto failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", plaintext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = des3_crypto(cipher, 0, ciphertext, ciphertext_len, key, sizeof(key), iv, sizeof(iv), plaintext, &plaintext_len);
    if (ret != 0) {
        printf("des3_crypto failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

    printf("\n*********************3des crypto ecb test*********************\n");
    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    cipher = EVP_des_ede3_ecb();
    ret = des3_crypto(cipher, 1, plaintext, sizeof(plaintext), key, sizeof(key), NULL, 0, ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("des3_crypto failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", plaintext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = des3_crypto(cipher, 0, ciphertext, ciphertext_len, key, sizeof(key), NULL, 0, plaintext, &plaintext_len);
    if (ret != 0) {
        printf("des3_crypto failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

    printf("\n*********************3des crypto cfb test*********************\n");
    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    cipher = EVP_des_ede3_cfb();
    ret = des3_crypto(cipher, 1, plaintext, sizeof(plaintext), key, sizeof(key), iv, sizeof(iv), ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("des3_crypto failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", plaintext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = des3_crypto(cipher, 0, ciphertext, ciphertext_len, key, sizeof(key), iv, sizeof(iv), plaintext, &plaintext_len);
    if (ret != 0) {
        printf("des3_crypto failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

    printf("\n*********************3des crypto ofb test*********************\n");
    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    cipher = EVP_des_ede3_ofb();
    ret = des3_crypto(cipher, 1, plaintext, sizeof(plaintext), key, sizeof(key), iv, sizeof(iv), ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("des3_crypto failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", plaintext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = des3_crypto(cipher, 0, ciphertext, ciphertext_len, key, sizeof(key), iv, sizeof(iv), plaintext, &plaintext_len);
    if (ret != 0) {
        printf("des3_crypto failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

out:
    return ret;
}