#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/types.h>


unsigned char key_128[16] = {0x01, 0x23, 0x45, 0x67, 
                                0x89, 0xab, 0xcd, 0xef, 
                                0xfe, 0xdc, 0xba, 0x98, 
                                0x76, 0x54, 0x32, 0x10};

unsigned char key_192[24] = {0x01, 0x23, 0x45, 0x67, 
                                0x89, 0xab, 0xcd, 0xef, 
                                0xfe, 0xdc, 0xba, 0x98, 
                                0x76, 0x54, 0x32, 0x10,
                                0x01, 0x23, 0x45, 0x67, 
                                0x89, 0xab, 0xcd, 0xef};

unsigned char key_256[32] = {0x01, 0x23, 0x45, 0x67, 
                                0x89, 0xab, 0xcd, 0xef, 
                                0xfe, 0xdc, 0xba, 0x98, 
                                0x76, 0x54, 0x32, 0x10,
                                0x01, 0x23, 0x45, 0x67, 
                                0x89, 0xab, 0xcd, 0xef,
                                0xfe, 0xdc, 0xba, 0x98, 
                                0x76, 0x54, 0x32, 0x10};

unsigned char iv[16] = {0x11, 0x22, 0x33, 0x44, 
                        0x55, 0x66, 0x77, 0x88, 
                        0x99, 0xaa, 0xbb, 0xcc, 
                        0xdd, 0xee, 0xff, 0x00};

unsigned char plain[32] = {0xfe, 0xdc, 0xba, 0x98, 
                            0x76, 0x54, 0x32, 0x10, 
                            0x01, 0x23, 0x45, 0x67, 
                            0x89, 0xab, 0xcd, 0xef, 
                            0xfe, 0xdc, 0xba, 0x98, 
                            0x76, 0x54, 0x32, 0x10, 
                            0x01, 0x23, 0x45, 0x67, 
                            0x89, 0xab, 0xcd, 0xef};



void test_128()
{

    EVP_CIPHER_CTX *ctx128 = EVP_CIPHER_CTX_new();

    const EVP_CIPHER *cipher_128 = EVP_aes_128_ecb();

    EVP_CipherInit_ex(ctx128, cipher_128, NULL, key_128, iv, 1);

    EVP_CIPHER_CTX_set_padding(ctx128, 0);

    // encrypt
    int len = 0;
    unsigned char cipher_text[32] = {0};
    EVP_CipherUpdate(ctx128, cipher_text, &len, plain, 32);

    // final
    EVP_CipherFinal(ctx128, cipher_text + len, &len);

    // print cipher text
    int i = 0;
    for (i = 0; i < 32; i++)
    {
        printf("%02x ", cipher_text[i]);
        if (i % 16 == 15)
            printf("\n");
    }

    printf("\n");

    // decrypt
    EVP_CipherInit_ex(ctx128, cipher_128, NULL, key_128, iv, 0);

    // set no padding
    EVP_CIPHER_CTX_set_padding(ctx128, 0);

    // decrypt
    unsigned char plain_text[32] = {0};
    EVP_CipherUpdate(ctx128, plain_text, &len, cipher_text, 32);
    printf("len = %d\n", len);

    // final
    EVP_CipherFinal(ctx128, plain_text + len, &len);

    // print plain text
    for (i = 0; i < 32; i++)
    {
        printf("%02x ", plain_text[i]);
        if (i % 16 == 15)
            printf("\n");
    }

    printf("\n");


    // free
    EVP_CIPHER_CTX_free(ctx128);
}

void test_192()
{

    EVP_CIPHER_CTX *ctx192 = EVP_CIPHER_CTX_new();

    const EVP_CIPHER *cipher_192 = EVP_aes_192_ofb();

    EVP_CipherInit_ex(ctx192, cipher_192, NULL, key_192, iv, 1);

    EVP_CIPHER_CTX_set_padding(ctx192, 0);

    // encrypt
    int len = 0;
    unsigned char cipher_text[32] = {0};
    EVP_CipherUpdate(ctx192, cipher_text, &len, plain, 32);

    // final
    EVP_CipherFinal(ctx192, cipher_text + len, &len);

    // print cipher text
    int i = 0;
    for (i = 0; i < 32; i++)
    {
        printf("%02x ", cipher_text[i]);
        if (i % 16 == 15)
            printf("\n");
    }

    printf("\n");

    // decrypt
    EVP_CipherInit_ex(ctx192, cipher_192, NULL, key_192, iv, 0);

    // set no padding
    EVP_CIPHER_CTX_set_padding(ctx192, 0);

    // decrypt
    unsigned char plain_text[32] = {0};
    EVP_CipherUpdate(ctx192, plain_text, &len, cipher_text, 32);
    printf("len = %d\n", len);

    // final
    EVP_CipherFinal(ctx192, plain_text + len, &len);

    // print plain text
    for (i = 0; i < 32; i++)
    {
        printf("%02x ", plain_text[i]);
        if (i % 16 == 15)
            printf("\n");
    }

    printf("\n");


    // free
    EVP_CIPHER_CTX_free(ctx192);
}

void test_256()
{
    EVP_CIPHER_CTX *ctx256 = EVP_CIPHER_CTX_new();

    const EVP_CIPHER *cipher_256 = EVP_aes_256_cfb();

    EVP_CipherInit_ex(ctx256, cipher_256, NULL, key_256, iv, 1);

    EVP_CIPHER_CTX_set_padding(ctx256, 0);

    // encrypt
    int len = 0;
    unsigned char cipher_text[32] = {0};
    EVP_CipherUpdate(ctx256, cipher_text, &len, plain, 32);

    // final
    EVP_CipherFinal(ctx256, cipher_text + len, &len);

    // print cipher text
    int i = 0;
    for (i = 0; i < 32; i++)
    {
        printf("%02x ", cipher_text[i]);
        if (i % 16 == 15)
            printf("\n");
    }

    printf("\n");

    // decrypt
    EVP_CipherInit_ex(ctx256, cipher_256, NULL, key_256, iv, 0);

    // set no padding
    EVP_CIPHER_CTX_set_padding(ctx256, 0);

    // decrypt
    unsigned char plain_text[32] = {0};
    EVP_CipherUpdate(ctx256, plain_text, &len, cipher_text, 32);
    printf("len = %d\n", len);

    // final
    EVP_CipherFinal(ctx256, plain_text + len, &len);

    // print plain text
    for (i = 0; i < 32; i++)
    {
        printf("%02x ", plain_text[i]);
        if (i % 16 == 15)
            printf("\n");
    }

    printf("\n");


    // free
    EVP_CIPHER_CTX_free(ctx256);
}

int main(int argc, char *argv[])
{
    int ret = 0;

    printf("test 128\n");
    test_128();
    printf("============================================\n");
    
    printf("test 192\n");
    test_192();
    printf("============================================\n");

    printf("test 256\n");
    test_256();
    printf("============================================\n");


    return ret;
}