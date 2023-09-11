#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <openssl/evp.h>

void do_crypto(unsigned char *in, int in_len, unsigned char *out, int *out_len,
               const unsigned char *key, const unsigned char *iv, int enc)
{
    EVP_CIPHER_CTX *ctx;
    int tmplen;

    ctx = EVP_CIPHER_CTX_new();
    EVP_CipherInit_ex(ctx, EVP_sm4_cbc(), NULL, key, iv, enc);
    EVP_CipherUpdate(ctx, out, out_len, in, in_len);
    EVP_CipherFinal_ex(ctx, out + *out_len, &tmplen);
    *out_len += tmplen;
    EVP_CIPHER_CTX_free(ctx);
}

int main(int argc, char *argv[])
{
    int ret = 0;

    // unsigned char key[] =
    //     {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
    // unsigned char iv[] = 
    //     {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

    char *key = "1234567890abcdef";
    char *iv = "1234567890abcdef";

    char *plaintext = "hello world11111";
    char ciphertext[1024] = {0};

    int out_len = 0;
    int in_len = strlen(plaintext);

    do_crypto(plaintext, in_len, ciphertext, &out_len, key, iv, 1);

    int i = 0;
    for (i = 0; i < out_len; i++) {
        printf("%02x", ciphertext[i]);
    }

    printf("\n");

    return ret;
}