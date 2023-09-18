#include "openssl/bn.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>

#include <openssl/rand.h>
#include <openssl/bn.h>

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

int generate_random_test(unsigned char ran[], int ran_len)
{
    int ret = 0;

    // 生成随机字节数组
    RAND_bytes(ran, ran_len);
    dump_hex(ran, ran_len);

    // 生成伪随机字节数组
    RAND_pseudo_bytes(ran, ran_len);
    dump_hex(ran, ran_len);

    // 生成随机数
    unsigned int ran_num = 0;
    RAND_bytes((unsigned char *)&ran_num, sizeof(ran_num));
    printf("ran_num: %d\n", ran_num);

    // 生成随机长整数
    unsigned long ran_long = 0;
    RAND_bytes((unsigned char *)&ran_long, sizeof(ran_long));
    printf("ran_long: %ld\n", ran_long);

    // 生成随机大数
    BIGNUM *ran_bn = BN_new();
    BN_rand(ran_bn, 1024, 0, 0);
    char *ran_bn_str = BN_bn2hex(ran_bn);
    printf("ran_bn: %s\n", ran_bn_str);
    OPENSSL_free(ran_bn_str);
    BN_free(ran_bn);


    return ret;
}


int main(int argc, char *argv[])
{
    int ret = 0;


    unsigned char ran[1024] = {0};
    generate_random_test(ran, 32);


    return ret;
}