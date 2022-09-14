#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <openssl/evp.h>

void do_sm3(unsigned char *in, int in_len, unsigned char *out, int *out_len)
{
    EVP_MD_CTX *ctx;
    ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sm3(), NULL);
    EVP_DigestUpdate(ctx, in, in_len);
    EVP_DigestFinal_ex(ctx, out, out_len);
    EVP_MD_CTX_free(ctx);
}

int main(int argc, char *argv[])
{
    int ret = 0;

    char *plaintext = "hello world11111";
    char digest[1024] = {0};

    int out_len = 0;
    int in_len = strlen(plaintext);

    do_sm3(plaintext, in_len, digest, &out_len);

    int i = 0;
    for (i = 0; i < out_len; i++) {
        printf("%02x", digest[i]);
    }

    printf("\n");

    return ret;
}