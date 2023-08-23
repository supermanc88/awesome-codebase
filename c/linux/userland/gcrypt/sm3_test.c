#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

#include <gcrypt.h>


int sm3_test(char *str)
{
    int ret = 0;
    gcry_md_hd_t hd;
    unsigned char *digest;
    unsigned char *buf;
    size_t len;
    size_t i;

    len = strlen(str);
    buf = (unsigned char *)malloc(len);
    if (buf == NULL) {
        printf("malloc failed\n");
        return -1;
    }
    memcpy(buf, str, len);

    digest = (unsigned char *)malloc(32);
    if (digest == NULL) {
        printf("malloc failed\n");
        return -1;
    }

    gcry_md_open(&hd, GCRY_MD_SM3, 0);
    gcry_md_write(hd, buf, len);
    gcry_md_final(hd);
    memcpy(digest, gcry_md_read(hd, GCRY_MD_SM3), 32);
    gcry_md_close(hd);

    for (i = 0; i < 32; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");

    free(buf);
    free(digest);

    return ret;
}

int sm3_long_input_test() 
{
    gcry_md_hd_t hd;
    unsigned char digest[32];
    unsigned char buf[1] = { 'a' }; 
    int i;
    
    gcry_md_open(&hd, GCRY_MD_SM3, 0);

    for (int i = 0; i < 1000000; i++)
        gcry_md_write(hd, buf, 1);  

    gcry_md_final(hd);
    memcpy(digest, gcry_md_read(hd, GCRY_MD_SM3), 32);

    for (i = 0; i < 32; i++) {
        printf("%02x", digest[i]);
    }

    printf("\n");

    gcry_md_close(hd);

  return 0;
}


int main(int argc, char *argv[])
{
    int ret = 0;

    sm3_test("a");
    sm3_test("abc");
    sm3_long_input_test();

    return ret;
}