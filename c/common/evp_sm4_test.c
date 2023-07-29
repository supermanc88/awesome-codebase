#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/types.h>


int main(int argc, char *argv[])
{
    int ret = 0;


    // sm4 key
    unsigned char key[16] = {0x01, 0x23, 0x45, 0x67, 
                             0x89, 0xab, 0xcd, 0xef, 
                             0xfe, 0xdc, 0xba, 0x98, 
                             0x76, 0x54, 0x32, 0x10};
    
    // sm4 iv
    unsigned char iv[16] = {0x11, 0x22, 0x33, 0x44, 
                            0x55, 0x66, 0x77, 0x88, 
                            0x99, 0xaa, 0xbb, 0xcc, 
                            0xdd, 0xee, 0xff, 0x00};

    // plain text
    unsigned char plain[64] = {0xfe, 0xdc, 0xba, 0x98, 
                               0x76, 0x54, 0x32, 0x10, 
                               0x01, 0x23, 0x45, 0x67, 
                               0x89, 0xab, 0xcd, 0xef, 
                               0xfe, 0xdc, 0xba, 0x98, 
                               0x76, 0x54, 0x32, 0x10, 
                               0x01, 0x23, 0x45, 0x67, 
                               0x89, 0xab, 0xcd, 0xef, 
                               0xfe, 0xdc, 0xba, 0x98, 
                               0x76, 0x54, 0x32, 0x10, 
                               0x01, 0x23, 0x45, 0x67, 
                               0x89, 0xab, 0xcd, 0xef, 
                               0xfe, 0xdc, 0xba, 0x98, 
                               0x76, 0x54, 0x32, 0x10, 
                               0x01, 0x23, 0x45, 0x67, 
                               0x89, 0xab, 0xcd, 0xef};



    // ctx
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    // cipher
    // const EVP_CIPHER *cipher = EVP_sm4_ecb();
    // const EVP_CIPHER *cipher = EVP_sm4_cbc();
    // const EVP_CIPHER *cipher = EVP_sm4_cfb();
    const EVP_CIPHER *cipher = EVP_sm4_ofb();

    // cipher init
    EVP_CipherInit_ex(ctx, cipher, NULL, key, iv, 1);

    // set no padding
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    // encrypt
    int len = 0;
    unsigned char cipher_text[80] = {0};
    EVP_CipherUpdate(ctx, cipher_text, &len, plain, 64);
    printf("len = %d\n", len);

    // final
    EVP_CipherFinal(ctx, cipher_text + len, &len);

    // print cipher text
    int i = 0;
    for (i = 0; i < 80; i++)
    {
        printf("%02x ", cipher_text[i]);
        if (i % 16 == 15)
            printf("\n");
    }

    printf("\n");

    // decrypt
    EVP_CipherInit_ex(ctx, cipher, NULL, key, iv, 0);

    // set no padding
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    // decrypt
    unsigned char plain_text[80] = {0};
    EVP_CipherUpdate(ctx, plain_text, &len, cipher_text, 64);
    printf("len = %d\n", len);

    // final
    EVP_CipherFinal(ctx, plain_text + len, &len);

    // print plain text
    for (i = 0; i < 80; i++)
    {
        printf("%02x ", plain_text[i]);
        if (i % 16 == 15)
            printf("\n");
    }

    printf("\n");


    // free
    EVP_CIPHER_CTX_free(ctx);

    return ret;
}