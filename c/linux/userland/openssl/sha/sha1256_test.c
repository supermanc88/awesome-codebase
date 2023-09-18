#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>

#include <openssl/evp.h>

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

char *data = "1234567890 hello world!";

int sha1_test(unsigned char md[], unsigned int *md_len)
{
    int ret = 0;

    // unsigned char md[EVP_MAX_MD_SIZE] = {0};

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        printf("EVP_MD_CTX_new failed\n");
        ret = -1;
        goto out;
    }

    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL)) {
        printf("EVP_DigestInit_ex failed\n");
        ret = -1;
        goto free_ctx;
    }

    int data_len = strlen(data);
    if (1 != EVP_DigestUpdate(mdctx, data, data_len)) {
        printf("EVP_DigestUpdate failed\n");
        ret = -1;
        goto free_ctx;
    }

    // unsigned int md_len = 0;
    if (1 != EVP_DigestFinal_ex(mdctx, md, md_len)) {
        printf("EVP_DigestFinal_ex failed\n");
        ret = -1;
        goto free_ctx;
    }

free_ctx:
    EVP_MD_CTX_free(mdctx);
out:
    return ret;
}

int sha256_test(unsigned char md[], unsigned int *md_len)
{
    int ret = 0;

    // unsigned char md[EVP_MAX_MD_SIZE] = {0};

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        printf("EVP_MD_CTX_new failed\n");
        ret = -1;
        goto out;
    }

    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
        printf("EVP_DigestInit_ex failed\n");
        ret = -1;
        goto free_ctx;
    }

    int data_len = strlen(data);
    if (1 != EVP_DigestUpdate(mdctx, data, data_len)) {
        printf("EVP_DigestUpdate failed\n");
        ret = -1;
        goto free_ctx;
    }

    // unsigned int md_len = 0;
    if (1 != EVP_DigestFinal_ex(mdctx, md, md_len)) {
        printf("EVP_DigestFinal_ex failed\n");
        ret = -1;
        goto free_ctx;
    }

free_ctx:
    EVP_MD_CTX_free(mdctx);
out:
    return ret;
}



int main(int argc, char *argv[])
{
    int ret = 0;

    unsigned char md[EVP_MAX_MD_SIZE] = {0};
    unsigned int md_len = 0;

    printf("\n****************** sha1_test ******************\n");
    ret = sha1_test(md, &md_len);
    if (ret != 0) {
        printf("sha1_test failed\n");
        goto out;
    }

    dump_hex(md, md_len);

    memset(md, 0, sizeof(md));

    printf("\n****************** sha256_test ******************\n");
    ret = sha256_test(md, &md_len);
    if (ret != 0) {
        printf("sha256_test failed\n");
        goto out;
    }

    dump_hex(md, md_len);

out:
    return ret;
}