#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

#include <openssl/evp.h>
#include <openssl/aes.h>

#define _TEST_AES_128_CBC_ 1
#define _TEST_AES_192_ECB_ 1
#define _TEST_AES_256_CFB_ 1
#define _TEST_AES_256_OFB_ 1
#define _TEST_AES_256_CTR_ 1

unsigned char iv[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 
                        0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
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


#if _TEST_AES_128_CBC_
unsigned char key_128[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 
                            0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

int enc_aes_128_cbc(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int *ciphertext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_128_cbc();

    if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, ciphertext_len, plaintext, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + *ciphertext_len, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *ciphertext_len += plaintext_len;
    

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}

int dec_aes_128_cbc(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext, int *plaintext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_128_cbc();

    if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext, plaintext_len, ciphertext, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + *plaintext_len, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *plaintext_len += ciphertext_len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}
#endif // _TEST_AES_128_CBC_


#if _TEST_AES_192_ECB_
unsigned char key_192[24] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 
                            0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 
                            0xc, 0xd, 0xe, 0xf, 0x0, 0x1, 
                            0x2, 0x3, 0x4, 0x5, 0x6, 0x7};

int enc_aes_192_ecb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext, int *ciphertext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_192_ecb();

    if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, NULL)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, ciphertext_len, plaintext, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + *ciphertext_len, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *ciphertext_len += plaintext_len;
free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}

int dec_aes_192_ecb(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *plaintext, int *plaintext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_192_ecb();

    if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, NULL)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext, plaintext_len, ciphertext, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + *plaintext_len, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *plaintext_len += ciphertext_len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}
#endif // _TEST_AES_192_ECB_


#if _TEST_AES_256_CFB_

unsigned char key_256[32] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 
                            0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 
                            0xc, 0xd, 0xe, 0xf, 0x0, 0x1, 
                            0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                            0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 
                            0xe, 0xf};

int enc_aes_256_cfb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int *ciphertext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_256_cfb();

    if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, ciphertext_len, plaintext, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + *ciphertext_len, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *ciphertext_len += plaintext_len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}

int dec_aes_256_cfb(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext, int *plaintext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_256_cfb();

    if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext, plaintext_len, ciphertext, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + *plaintext_len, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *plaintext_len += ciphertext_len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}

#endif // _TEST_AES_256_CFB_


#if _TEST_AES_256_OFB_

// unsigned char key_256[32] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 
//                             0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 
//                             0xc, 0xd, 0xe, 0xf, 0x0, 0x1, 
//                             0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
//                             0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 
//                             0xe, 0xf};

int enc_aes_256_ofb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int *ciphertext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_256_ofb();

    if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, ciphertext_len, plaintext, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + *ciphertext_len, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *ciphertext_len += plaintext_len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}

int dec_aes_256_ofb(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext, int *plaintext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_256_ofb();

    if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext, plaintext_len, ciphertext, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + *plaintext_len, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *plaintext_len += ciphertext_len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}
#endif // _TEST_AES_256_OFB_


#if _TEST_AES_256_CTR_

// unsigned char key_256[32] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5,
//                             0x6, 0x7, 0x8, 0x9, 0xa, 0xb,
//                             0xc, 0xd, 0xe, 0xf, 0x0, 0x1,
//                             0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
//                             0x8, 0x9, 0xa, 0xb, 0xc, 0xd,
//                             0xe, 0xf};

int enc_aes_256_ctr(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int *ciphertext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_256_ctr();

    if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, ciphertext_len, plaintext, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + *ciphertext_len, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *ciphertext_len += plaintext_len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}

int dec_aes_256_ctr(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext, int *plaintext_len)
{
    int ret = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        ret = -1;
        goto out;
    }

    const EVP_CIPHER *cipher = EVP_aes_256_ctr();

    if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv)) {
        ret = -1;
        goto free_ctx;
    }

    // set no padding
    if (1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext, plaintext_len, ciphertext, ciphertext_len)) {
        ret = -1;
        goto free_ctx;
    }

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + *plaintext_len, plaintext_len)) {
        ret = -1;
        goto free_ctx;
    }

    *plaintext_len += ciphertext_len;

free_ctx:
    EVP_CIPHER_CTX_free(ctx);
out:
    return ret;
}

#endif // _TEST_AES_256_CTR_


int main(int argc, char *argv[])
{
    int ret = 0;

#if _TEST_AES_128_CBC_
{
    printf("\n****************************aes 128 cbc test****************************\n");
    int ciphertext_len = 0;
    int plaintext_len = 0;

    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    ret = enc_aes_128_cbc(plaintext, sizeof(plaintext), key_128, iv, ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("enc_aes_128_cbc failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", ciphertext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = dec_aes_128_cbc(ciphertext, ciphertext_len, key_128, iv, plaintext, &plaintext_len);
    if (ret != 0) {
        printf("dec_aes_128_cbc failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

}
#endif // _TEST_AES_128_CBC_

#if _TEST_AES_192_ECB_
{
    printf("\n****************************aes 192 ecb test****************************\n");
    int ciphertext_len = 0;
    int plaintext_len = 0;

    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    ret = enc_aes_192_ecb(plaintext, sizeof(plaintext), key_192, ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("enc_aes_192_ecb failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", ciphertext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = dec_aes_192_ecb(ciphertext, ciphertext_len, key_192, plaintext, &plaintext_len);
    if (ret != 0) {
        printf("dec_aes_192_ecb failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

}
#endif // _TEST_AES_192_ECB_

#if _TEST_AES_256_CFB_
{
    printf("\n****************************aes 256 cfb test****************************\n");
    int ciphertext_len = 0;
    int plaintext_len = 0;

    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    ret = enc_aes_256_cfb(plaintext, sizeof(plaintext), key_256, iv, ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("enc_aes_256_cfb failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", ciphertext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = dec_aes_256_cfb(ciphertext, ciphertext_len, key_256, iv, plaintext, &plaintext_len);
    if (ret != 0) {
        printf("dec_aes_256_cfb failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

}
#endif // _TEST_AES_256_CFB_

#if _TEST_AES_256_OFB_
{
    printf("\n****************************aes 256 ofb test****************************\n");
    int ciphertext_len = 0;
    int plaintext_len = 0;

    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    ret = enc_aes_256_ofb(plaintext, sizeof(plaintext), key_256, iv, ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("enc_aes_256_ofb failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", ciphertext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = dec_aes_256_ofb(ciphertext, ciphertext_len, key_256, iv, plaintext, &plaintext_len);
    if (ret != 0) {
        printf("dec_aes_256_ofb failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

}
#endif // _TEST_AES_256_OFB_

#if _TEST_AES_256_CTR_
{
    printf("\n****************************aes 256 ctr test****************************\n");
    int ciphertext_len = 0;
    int plaintext_len = 0;

    printf("plaintext:\n");
    dump_hex(plaintext, sizeof(plaintext));

    ret = enc_aes_256_ctr(plaintext, sizeof(plaintext), key_256, iv, ciphertext, &ciphertext_len);
    if (ret != 0) {
        printf("enc_aes_256_ctr failed\n");
        goto out;
    }

    printf("ciphertext_len: %d, ciphertext:\n", ciphertext_len);
    dump_hex(ciphertext, ciphertext_len);

    ret = dec_aes_256_ctr(ciphertext, ciphertext_len, key_256, iv, plaintext, &plaintext_len);
    if (ret != 0) {
        printf("dec_aes_256_ctr failed\n");
        goto out;
    }

    printf("plaintext_len: %d, plaintext:\n", plaintext_len);
    dump_hex(plaintext, plaintext_len);

}
#endif // _TEST_AES_256_CTR_


out:
    return ret;
}