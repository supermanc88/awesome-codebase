#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

// 查找表，将16进制字符转换为整数值
static const uint8_t hex_char_to_bin[256] = {
    ['0'] = 0x0, ['1'] = 0x1, ['2'] = 0x2, ['3'] = 0x3,
    ['4'] = 0x4, ['5'] = 0x5, ['6'] = 0x6, ['7'] = 0x7,
    ['8'] = 0x8, ['9'] = 0x9, ['A'] = 0xa, ['B'] = 0xb,
    ['C'] = 0xc, ['D'] = 0xd, ['E'] = 0xe, ['F'] = 0xf,
    ['a'] = 0xa, ['b'] = 0xb, ['c'] = 0xc, ['d'] = 0xd,
    ['e'] = 0xe, ['f'] = 0xf
};

// 将16进制字符串转换为二进制数据
// hex: 输入的16进制字符串
// bin: 输出的二进制数据数组
// bin_size: 输出数组的大小，应该是输入字符串长度的一半
// 返回值: 成功转换的字节数
size_t hex_to_bin(const char *hex, uint8_t *bin, size_t bin_size) {
    size_t hex_len = strlen(hex);
    if (hex_len % 2 != 0 || bin_size < hex_len / 2) {
        return 0;
    }

    size_t i = 0, j = 0;
    for (i = 0, j = 0; i < hex_len; i += 2, j++) {
        uint8_t high = hex_char_to_bin[(unsigned char)hex[i]];
        uint8_t low = hex_char_to_bin[(unsigned char)hex[i+1]];
        bin[j] = (high << 4) | low;
    }
    return hex_len / 2;
}


int generate_rsa_test_use_old()
{
    RSA *rsa = NULL;
    int ret = 0;

    rsa = RSA_generate_key(1024, 65537, NULL, NULL);
    if (rsa == NULL) {
        printf("RSA_generate_key failed\n");
        ret = -1;
        goto out;
    }

    BIGNUM *n = NULL;
    BIGNUM *e = NULL;
    BIGNUM *d = NULL;
    BIGNUM *p = NULL;
    BIGNUM *q = NULL;
    BIGNUM *dmp1 = NULL;
    BIGNUM *dmq1 = NULL;
    BIGNUM *iqmp = NULL;

    RSA_get0_key(rsa, (const BIGNUM **)&n, (const BIGNUM **)&e, (const BIGNUM **)&d);
    RSA_get0_factors(rsa, (const BIGNUM **)&p, (const BIGNUM **)&q);
    RSA_get0_crt_params(rsa, (const BIGNUM **)&dmp1, (const BIGNUM **)&dmq1, (const BIGNUM **)&iqmp);

    char *n_hex = BN_bn2hex(n);
    char *e_hex = BN_bn2hex(e);
    char *d_hex = BN_bn2hex(d);
    char *p_hex = BN_bn2hex(p);
    char *q_hex = BN_bn2hex(q);
    char *dmp1_hex = BN_bn2hex(dmp1);
    char *dmq1_hex = BN_bn2hex(dmq1);
    char *iqmp_hex = BN_bn2hex(iqmp);

    printf("n: %s\n", n_hex);
    printf("e: %s\n", e_hex);
    printf("d: %s\n", d_hex);
    printf("p: %s\n", p_hex);
    printf("q: %s\n", q_hex);
    printf("dmp1: %s\n", dmp1_hex);
    printf("dmq1: %s\n", dmq1_hex);
    printf("iqmp: %s\n", iqmp_hex);

    OPENSSL_free(n_hex);
    OPENSSL_free(e_hex);
    OPENSSL_free(d_hex);
    OPENSSL_free(p_hex);
    OPENSSL_free(q_hex);
    OPENSSL_free(dmp1_hex);
    OPENSSL_free(dmq1_hex);
    OPENSSL_free(iqmp_hex);

    RSA_free(rsa);

out:
    return ret;
}


int generate_rsa_test_use_evp2()
{
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;

    // 1. 初始化 OpenSSL
    OpenSSL_add_all_algorithms();

    // 2. 设置 RSA 密钥长度
    int rsa_keylen = 1024;

    // 3. 使用 EVP 接口生成 RSA 密钥对
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        perror("EVP_PKEY_CTX_new_id failed");
        exit(1);
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        perror("EVP_PKEY_keygen_init failed");
        exit(1);
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, rsa_keylen) <= 0) {
        perror("EVP_PKEY_CTX_set_rsa_keygen_bits failed");
        exit(1);
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        perror("EVP_PKEY_keygen failed");
        exit(1);
    }

    // gen private key
    EVP_PKEY_generate(ctx, &pkey);

    BIGNUM *n = NULL;
    BIGNUM *e = NULL;
    BIGNUM *d = NULL;
    BIGNUM *p = NULL;
    BIGNUM *q = NULL;
    BIGNUM *dmp1 = NULL;
    BIGNUM *dmq1 = NULL;
    BIGNUM *iqmp = NULL;

    RSA *rsa = NULL;
    rsa = EVP_PKEY_get1_RSA(pkey);
    if (rsa == NULL) {
        printf("EVP_PKEY_get1_RSA failed\n");
        return -1;
    }

    RSA_get0_key(rsa, (const BIGNUM **)&n, (const BIGNUM **)&e, (const BIGNUM **)&d);
    RSA_get0_factors(rsa, (const BIGNUM **)&p, (const BIGNUM **)&q);
    RSA_get0_crt_params(rsa, (const BIGNUM **)&dmp1, (const BIGNUM **)&dmq1, (const BIGNUM **)&iqmp);

    char *n_hex = BN_bn2hex(n);
    char *e_hex = BN_bn2hex(e);
    char *d_hex = BN_bn2hex(d);
    char *p_hex = BN_bn2hex(p);
    char *q_hex = BN_bn2hex(q);
    char *dmp1_hex = BN_bn2hex(dmp1);
    char *dmq1_hex = BN_bn2hex(dmq1);
    char *iqmp_hex = BN_bn2hex(iqmp);

    // printf("n: %s\n", n_hex);
    // printf("e: %s\n", e_hex);
    // printf("d: %s\n", d_hex);
    // printf("p: %s\n", p_hex);
    // printf("q: %s\n", q_hex);
    // printf("dmp1: %s\n", dmp1_hex);
    // printf("dmq1: %s\n", dmq1_hex);
    // printf("iqmp: %s\n", iqmp_hex);

    OPENSSL_free(n_hex);
    OPENSSL_free(e_hex);
    OPENSSL_free(d_hex);
    OPENSSL_free(p_hex);
    OPENSSL_free(q_hex);
    OPENSSL_free(dmp1_hex);
    OPENSSL_free(dmq1_hex);
    OPENSSL_free(iqmp_hex);

    RSA_free(rsa);

    // 4. 将生成的密钥对保存到文件
    // FILE *pkey_file = fopen("private_key.pem", "wb");
    // PEM_write_PrivateKey(pkey_file, pkey, NULL, NULL, 0, NULL, NULL);
    // fclose(pkey_file);

    // FILE *pubkey_file = fopen("public_key.pem", "wb");
    // PEM_write_PUBKEY(pubkey_file, pkey);
    // fclose(pubkey_file);

    // 5. 清理并释放所有分配的资源
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    EVP_cleanup();

    return 0;
}

int generate_rsa_key_write_file()
{
    int ret = 0;

    RSA *rsa = NULL;

    rsa = RSA_generate_key(1024, 65537, NULL, NULL);
    if (rsa == NULL) {
        printf("RSA_generate_key failed\n");
        ret = -1;
        goto out;
    }

    // write private key
    FILE *pkey_file = fopen("private_key.pem", "wb");
    PEM_write_RSAPrivateKey(pkey_file, rsa, NULL, NULL, 0, NULL, NULL);
    fclose(pkey_file);

    // write public key
    FILE *pubkey_file = fopen("public_key.pem", "wb");
    PEM_write_RSAPublicKey(pubkey_file, rsa);
    fclose(pubkey_file);


    // BIGNUM *n = NULL;
    // BIGNUM *e = NULL;
    // BIGNUM *d = NULL;
    // BIGNUM *p = NULL;
    // BIGNUM *q = NULL;
    // BIGNUM *dmp1 = NULL;
    // BIGNUM *dmq1 = NULL;
    // BIGNUM *iqmp = NULL;

    // RSA_get0_key(rsa, (const BIGNUM **)&n, (const BIGNUM **)&e, (const BIGNUM **)&d);
    // RSA_get0_factors(rsa, (const BIGNUM **)&p, (const BIGNUM **)&q);
    // RSA_get0_crt_params(rsa, (const BIGNUM **)&dmp1, (const BIGNUM **)&dmq1, (const BIGNUM **)&iqmp);

    // char *n_hex = BN_bn2hex(n);
    // char *e_hex = BN_bn2hex(e);
    // char *d_hex = BN_bn2hex(d);
    // char *p_hex = BN_bn2hex(p);
    // char *q_hex = BN_bn2hex(q);
    // char *dmp1_hex = BN_bn2hex(dmp1);
    // char *dmq1_hex = BN_bn2hex(dmq1);
    // char *iqmp_hex = BN_bn2hex(iqmp);

    // // printf("n: %s\n", n_hex);
    // // printf("e: %s\n", e_hex);
    // // printf("d: %s\n", d_hex);
    // // printf("p: %s\n", p_hex);
    // // printf("q: %s\n", q_hex);
    // // printf("dmp1: %s\n", dmp1_hex);
    // // printf("dmq1: %s\n", dmq1_hex);
    // // printf("iqmp: %s\n", iqmp_hex);

    // OPENSSL_free(n_hex);
    // OPENSSL_free(e_hex);
    // OPENSSL_free(d_hex);
    // OPENSSL_free(p_hex);
    // OPENSSL_free(q_hex);
    // OPENSSL_free(dmp1_hex);
    // OPENSSL_free(dmq1_hex);
    // OPENSSL_free(iqmp_hex);

    RSA_free(rsa);

out:
    return ret;
}

/**
n: BB3B4D2E7839ED9F592C2E017C8AA53ACB74634DF5A6EB761FD200C47BA1C406FAD56BFDA708ACE5156B0776484647EA42A88E3410B32721F5E253680DB3D4853921C00E8AD94DF5DC13511316E87D4C73D6CD3562769BAAC9E6ED1DFDDECF56C5005DE456CCFD74AFD3035A6DB9A9DB2F8B203273CCD6A1EE292F96D966C7E7
e: 010001
d: 40D35876C573B11D61FF94F238CFBC43AE1BDF2227F2A245E4E08E197980D9E15CC9B6393DB31841BCF7B0BEB869750EDC038630EAB34CD24A531173CE82BC0AC88E6D79692001BFB3910D84325AE7073395B491CF33654268FEEEC79DB520F990422AD0F0974BC55210A32ACFB31FABCA7578B1EB9FC6EA42BEC4E9ECE4F549
p: E82BA24096A2A510890E9E6A55CDC7390140B287BCAECB3F637208045F26D1DEEA6200D0549117AFED54C18051C002E4DFE8BD19E58775EBD1C4947AA965F38D
q: CE72E23FA5B283D956D8815D841CD9B9D6C86B93659E10C637AD74433614BB5626E91B2518ABD561526DF3BDD96F4632858C4E74EAFD6D24CA8D903CD9E0B243
dmp1: C9712A8B7DE7CAEE88ECBD38A8102E9E605EE14872C6FAA49A45537D6F8DC024F91A65785D17B304BB1881F83376354C9E96529508993B108C66532A0696E5F5
dmq1: 5E3033E0EDEC00138C77C2ADB3B4A5F10F78E8FB9A84FF0A3024C6C2534E2A1A282E79DFF6DD8EC09426EDE953E65C9478318729F87C1EEB862E07FC7B3AF499
iqmp: 5FDCE96F56880B67505A39C67EC672A7E62BC55DDEA9041FDF505FA2911A213DDA5E5A97E4631088BFFB04F052A9FF71E290169229676F0D3C14662F3123556C
*/
char *g_n_hex = "BB3B4D2E7839ED9F592C2E017C8AA53ACB74634DF5A6EB761FD200C47BA1C406FAD56BFDA708ACE5156B0776484647EA42A88E3410B32721F5E253680DB3D4853921C00E8AD94DF5DC13511316E87D4C73D6CD3562769BAAC9E6ED1DFDDECF56C5005DE456CCFD74AFD3035A6DB9A9DB2F8B203273CCD6A1EE292F96D966C7E7";
char *g_e_hex = "010001";
char *g_d_hex = "40D35876C573B11D61FF94F238CFBC43AE1BDF2227F2A245E4E08E197980D9E15CC9B6393DB31841BCF7B0BEB869750EDC038630EAB34CD24A531173CE82BC0AC88E6D79692001BFB3910D84325AE7073395B491CF33654268FEEEC79DB520F990422AD0F0974BC55210A32ACFB31FABCA7578B1EB9FC6EA42BEC4E9ECE4F549";
char *g_p_hex = "E82BA24096A2A510890E9E6A55CDC7390140B287BCAECB3F637208045F26D1DEEA6200D0549117AFED54C18051C002E4DFE8BD19E58775EBD1C4947AA965F38D";
char *g_q_hex = "CE72E23FA5B283D956D8815D841CD9B9D6C86B93659E10C637AD74433614BB5626E91B2518ABD561526DF3BDD96F4632858C4E74EAFD6D24CA8D903CD9E0B243";
char *g_dmp1_hex = "C9712A8B7DE7CAEE88ECBD38A8102E9E605EE14872C6FAA49A45537D6F8DC024F91A65785D17B304BB1881F83376354C9E96529508993B108C66532A0696E5F5";
char *g_dmq1_hex = "5E3033E0EDEC00138C77C2ADB3B4A5F10F78E8FB9A84FF0A3024C6C2534E2A1A282E79DFF6DD8EC09426EDE953E65C9478318729F87C1EEB862E07FC7B3AF499";
char *g_iqmp_hex = "5FDCE96F56880B67505A39C67EC672A7E62BC55DDEA9041FDF505FA2911A213DDA5E5A97E4631088BFFB04F052A9FF71E290169229676F0D3C14662F3123556C";

char *g_sign_hex = "6d3b35fd779af1c437d4f9795a814caf17a882c27e3ec77cdd67b7a64d60433592b44c54339a8b57342e20b7d3d471a7d7917dfda0c819bd8a807dcc34f4cff8807e5434e6380fcc50fcbae257aebab1e0ae2b346ca364742215d8c76b0bc1f6e0774cfd3daac5eb6bc3754c94182d12d1c2856d0ddd4619635dbe0ee851a452";

char *g_encrypt_hex = "3d959d20b310f5152d4ddc1ff55f1e56fa02fcd36a0c131762a9915375e1a5a2b2b1e4eccf84dddd4512611efb9cb2813a697843311e9901b488cd21d4e3885edee59e091dff540b1bab378bea81f3ba75dc48dcbaba1ed96dcfd4578460f43365dc30b6e2e9598b83b02c662d3a527884ccec0d5fcc13725ecf3fb3606fff92";

int use_rsa_priv_sign()
{
    int ret = 0;

    BIGNUM *n = NULL;
    BIGNUM *e = NULL;
    BIGNUM *d = NULL;
    BIGNUM *p = NULL;
    BIGNUM *q = NULL;
    BIGNUM *dmp1 = NULL;
    BIGNUM *dmq1 = NULL;
    BIGNUM *iqmp = NULL;

    RSA *rsa = NULL;
    rsa = RSA_new();
    if (rsa == NULL) {
        printf("RSA_new failed\n");
        return -1;
    }

    n = BN_new();
    e = BN_new();
    d = BN_new();
    p = BN_new();
    q = BN_new();
    dmp1 = BN_new();
    dmq1 = BN_new();
    iqmp = BN_new();

    BN_hex2bn(&n, g_n_hex);
    BN_hex2bn(&e, g_e_hex);
    BN_hex2bn(&d, g_d_hex);
    BN_hex2bn(&p, g_p_hex);
    BN_hex2bn(&q, g_q_hex);
    BN_hex2bn(&dmp1, g_dmp1_hex);
    BN_hex2bn(&dmq1, g_dmq1_hex);
    BN_hex2bn(&iqmp, g_iqmp_hex);

    RSA_set0_key(rsa, n, e, d);
    RSA_set0_factors(rsa, p, q);
    RSA_set0_crt_params(rsa, dmp1, dmq1, iqmp);

    char *n_hex = BN_bn2hex(n);
    char *e_hex = BN_bn2hex(e);
    char *d_hex = BN_bn2hex(d);
    char *p_hex = BN_bn2hex(p);
    char *q_hex = BN_bn2hex(q);
    char *dmp1_hex = BN_bn2hex(dmp1);
    char *dmq1_hex = BN_bn2hex(dmq1);
    char *iqmp_hex = BN_bn2hex(iqmp);

    // compare with g_n_hex
    printf("nhex compare: %d\n", strcmp(n_hex, g_n_hex));
    printf("ehex compare: %d\n", strcmp(e_hex, g_e_hex));
    printf("dhex compare: %d\n", strcmp(d_hex, g_d_hex));
    printf("phex compare: %d\n", strcmp(p_hex, g_p_hex));
    printf("qhex compare: %d\n", strcmp(q_hex, g_q_hex));
    printf("dmp1hex compare: %d\n", strcmp(dmp1_hex, g_dmp1_hex));
    printf("dmq1hex compare: %d\n", strcmp(dmq1_hex, g_dmq1_hex));
    printf("iqmphex compare: %d\n", strcmp(iqmp_hex, g_iqmp_hex));

    OPENSSL_free(n_hex);
    OPENSSL_free(e_hex);
    OPENSSL_free(d_hex);
    OPENSSL_free(p_hex);
    OPENSSL_free(q_hex);
    OPENSSL_free(dmp1_hex);
    OPENSSL_free(dmq1_hex);
    OPENSSL_free(iqmp_hex);

    // rsa private key sign
    unsigned char *sign = NULL;
    unsigned int sign_len = 0;
    sign = (unsigned char *)malloc(RSA_size(rsa));
    memset(sign, 0, RSA_size(rsa));

    char *data = "hello world";
    int data_len = strlen(data);

    ret = RSA_sign(NID_sha1, (unsigned char *)data, data_len, sign, &sign_len, rsa);
    // print sign hex string
    int i = 0;
    for (i = 0; i < sign_len; i++) {
        printf("%02x", sign[i]);
    }
    printf("\n");

    printf("sign_len: %d\n", sign_len);

    // rsa public key verify
    ret = RSA_verify(NID_sha1, (unsigned char *)data, data_len, sign, sign_len, rsa);
    printf("verify ret: %d\n", ret);
    free(sign);

    // free memory
    // BN_free(n);
    // BN_free(e);
    // BN_free(d);
    // BN_free(p);
    // BN_free(q);
    // BN_free(dmp1);
    // BN_free(dmq1);
    // BN_free(iqmp);

    RSA_free(rsa);



    return ret;
}

int use_rsa_pub_verify()
{
    int ret = 0;

    BIGNUM *n = NULL;
    BIGNUM *e = NULL;

    RSA *rsa = NULL;
    rsa = RSA_new();
    if (rsa == NULL) {
        printf("RSA_new failed\n");
        return -1;
    }

    n = BN_new();
    e = BN_new();

    BN_hex2bn(&n, g_n_hex);
    BN_hex2bn(&e, g_e_hex);

    RSA_set0_key(rsa, n, e, NULL);

    char *n_hex = BN_bn2hex(n);
    char *e_hex = BN_bn2hex(e);

    // compare with g_n_hex
    printf("nhex compare: %d\n", strcmp(n_hex, g_n_hex));
    printf("ehex compare: %d\n", strcmp(e_hex, g_e_hex));

    OPENSSL_free(n_hex);
    OPENSSL_free(e_hex);

    // rsa public key verify
    unsigned char *sign = NULL;
    unsigned int sign_len = 0;
    sign = (unsigned char *)malloc(RSA_size(rsa));
    memset(sign, 0, RSA_size(rsa));

    char *data = "hello world";
    int data_len = strlen(data);

    sign_len = hex_to_bin(g_sign_hex, sign, strlen(g_sign_hex));

    ret = RSA_verify(NID_sha1, (unsigned char *)data, data_len, sign, sign_len, rsa);
    printf("verify ret: %d\n", ret);

    free(sign);

    // free memory
    // BN_free(n);
    // BN_free(e);

    RSA_free(rsa);


    return ret;
}

int use_rsa_pub_encrypt()
{
    int ret = 0;

    BIGNUM *n = NULL;
    BIGNUM *e = NULL;

    RSA *rsa = NULL;
    rsa = RSA_new();
    if (rsa == NULL) {
        printf("RSA_new failed\n");
        return -1;
    }

    n = BN_new();
    e = BN_new();

    BN_hex2bn(&n, g_n_hex);
    BN_hex2bn(&e, g_e_hex);

    RSA_set0_key(rsa, n, e, NULL);

    char *n_hex = BN_bn2hex(n);
    char *e_hex = BN_bn2hex(e);

    // compare with g_n_hex
    printf("nhex compare: %d\n", strcmp(n_hex, g_n_hex));
    printf("ehex compare: %d\n", strcmp(e_hex, g_e_hex));

    OPENSSL_free(n_hex);
    OPENSSL_free(e_hex);

    // rsa public key encrypt
    unsigned char *encrypt = NULL;
    unsigned int encrypt_len = 0;
    encrypt = (unsigned char *)malloc(RSA_size(rsa));
    memset(encrypt, 0, RSA_size(rsa));

    char *data = "hello world";
    int data_len = strlen(data);

    ret = RSA_public_encrypt(data_len, (unsigned char *)data, encrypt, rsa, RSA_PKCS1_PADDING);
    // print encrypt hex string
    int i = 0;
    for (i = 0; i < ret; i++) {
        printf("%02x", encrypt[i]);
    }
    printf("\n");

    // free memory

    RSA_free(rsa);

    return ret;
}

int use_rsa_priv_decrypt()
{
    int ret = 0;

    BIGNUM *n = NULL;
    BIGNUM *e = NULL;
    BIGNUM *d = NULL;
    BIGNUM *p = NULL;
    BIGNUM *q = NULL;
    BIGNUM *dmp1 = NULL;
    BIGNUM *dmq1 = NULL;
    BIGNUM *iqmp = NULL;

    RSA *rsa = NULL;
    rsa = RSA_new();
    if (rsa == NULL) {
        printf("RSA_new failed\n");
        return -1;
    }

    n = BN_new();
    e = BN_new();
    d = BN_new();
    p = BN_new();
    q = BN_new();
    dmp1 = BN_new();
    dmq1 = BN_new();
    iqmp = BN_new();

    BN_hex2bn(&n, g_n_hex);
    BN_hex2bn(&e, g_e_hex);
    BN_hex2bn(&d, g_d_hex);
    BN_hex2bn(&p, g_p_hex);
    BN_hex2bn(&q, g_q_hex);
    BN_hex2bn(&dmp1, g_dmp1_hex);
    BN_hex2bn(&dmq1, g_dmq1_hex);
    BN_hex2bn(&iqmp, g_iqmp_hex);

    RSA_set0_key(rsa, n, e, d);
    RSA_set0_factors(rsa, p, q);
    RSA_set0_crt_params(rsa, dmp1, dmq1, iqmp);

    char *n_hex = BN_bn2hex(n);
    char *e_hex = BN_bn2hex(e);
    char *d_hex = BN_bn2hex(d);
    char *p_hex = BN_bn2hex(p);
    char *q_hex = BN_bn2hex(q);
    char *dmp1_hex = BN_bn2hex(dmp1);
    char *dmq1_hex = BN_bn2hex(dmq1);
    char *iqmp_hex = BN_bn2hex(iqmp);

    // compare with g_n_hex
    printf("nhex compare: %d\n", strcmp(n_hex, g_n_hex));
    printf("ehex compare: %d\n", strcmp(e_hex, g_e_hex));
    printf("dhex compare: %d\n", strcmp(d_hex, g_d_hex));
    printf("phex compare: %d\n", strcmp(p_hex, g_p_hex));
    printf("qhex compare: %d\n", strcmp(q_hex, g_q_hex));
    printf("dmp1hex compare: %d\n", strcmp(dmp1_hex, g_dmp1_hex));
    printf("dmq1hex compare: %d\n", strcmp(dmq1_hex, g_dmq1_hex));
    printf("iqmphex compare: %d\n", strcmp(iqmp_hex, g_iqmp_hex));

    OPENSSL_free(n_hex);
    OPENSSL_free(e_hex);
    OPENSSL_free(d_hex);
    OPENSSL_free(p_hex);
    OPENSSL_free(q_hex);
    OPENSSL_free(dmp1_hex);
    OPENSSL_free(dmq1_hex);
    OPENSSL_free(iqmp_hex);

    // rsa private key decrypt
    unsigned char *decrypt = NULL;
    unsigned int decrypt_len = 0;
    decrypt = (unsigned char *)malloc(RSA_size(rsa));
    memset(decrypt, 0, RSA_size(rsa));

    char *encrypt_data = NULL;
    int encrypt_data_len = 0;
    encrypt_data = (char *)malloc(RSA_size(rsa));
    memset(encrypt_data, 0, RSA_size(rsa));

    encrypt_data_len = hex_to_bin(g_encrypt_hex, encrypt_data, strlen(g_encrypt_hex));

    ret = RSA_private_decrypt(encrypt_data_len, (unsigned char *)encrypt_data, decrypt, rsa, RSA_PKCS1_PADDING);

    printf("decrypt: %s\n", decrypt);

    RSA_free(rsa);

    return ret;
}

int use_rsa_priv_sign_no_print()
{
    int ret = 0;

    BIGNUM *n = NULL;
    BIGNUM *e = NULL;
    BIGNUM *d = NULL;
    BIGNUM *p = NULL;
    BIGNUM *q = NULL;
    BIGNUM *dmp1 = NULL;
    BIGNUM *dmq1 = NULL;
    BIGNUM *iqmp = NULL;

    RSA *rsa = NULL;
    rsa = RSA_new();
    if (rsa == NULL) {
        printf("RSA_new failed\n");
        return -1;
    }

    n = BN_new();
    e = BN_new();
    d = BN_new();
    p = BN_new();
    q = BN_new();
    dmp1 = BN_new();
    dmq1 = BN_new();
    iqmp = BN_new();

    BN_hex2bn(&n, g_n_hex);
    BN_hex2bn(&e, g_e_hex);
    BN_hex2bn(&d, g_d_hex);
    BN_hex2bn(&p, g_p_hex);
    BN_hex2bn(&q, g_q_hex);
    BN_hex2bn(&dmp1, g_dmp1_hex);
    BN_hex2bn(&dmq1, g_dmq1_hex);
    BN_hex2bn(&iqmp, g_iqmp_hex);

    RSA_set0_key(rsa, n, e, d);
    RSA_set0_factors(rsa, p, q);
    RSA_set0_crt_params(rsa, dmp1, dmq1, iqmp);

    // rsa private key sign
    unsigned char *sign = NULL;
    unsigned int sign_len = 0;
    sign = (unsigned char *)malloc(RSA_size(rsa));
    memset(sign, 0, RSA_size(rsa));

    char *data = "hello world";
    int data_len = strlen(data);

    ret = RSA_sign(NID_sha1, (unsigned char *)data, data_len, sign, &sign_len, rsa);

    RSA_free(rsa);


    return ret;
}

int use_rsa_priv_sign_perform()
{
    int ret = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL);
    int i = 0;
    for (i = 0; i < 10000; i++) {
        ret = use_rsa_priv_sign_no_print();
    }
    gettimeofday(&end, NULL);

    // print sign count per second
    printf("use_rsa_priv_sign_perform ret: %d\n", ret);
    printf("use_rsa_priv_sign_perform time: %ld\n", (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000);
    printf("use_rsa_priv_sign_perform count per second: %f\n", 10000.0 / ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000));


    return ret;
}

int use_rsa_pub_verify_no_print(unsigned char *sign_data, unsigned int sign_data_len)
{
    int ret = 0;

    BIGNUM *n = NULL;
    BIGNUM *e = NULL;

    RSA *rsa = NULL;
    rsa = RSA_new();
    if (rsa == NULL) {
        printf("RSA_new failed\n");
        return -1;
    }

    n = BN_new();
    e = BN_new();

    BN_hex2bn(&n, g_n_hex);
    BN_hex2bn(&e, g_e_hex);

    RSA_set0_key(rsa, n, e, NULL);

    // rsa public key verify
    char *data = "hello world";
    int data_len = strlen(data);

    ret = RSA_verify(NID_sha1, (unsigned char *)data, data_len, sign_data, sign_data_len, rsa);


    RSA_free(rsa);

    return ret;
}

int use_rsa_pub_verify_perform()
{
    int ret = 0;

    unsigned char *sign = NULL;
    unsigned int sign_len = 0;
    sign = (unsigned char *)malloc(128);
    memset(sign, 0, 128);

    sign_len = hex_to_bin(g_sign_hex, sign, strlen(g_sign_hex));

    struct timeval start, end;
    gettimeofday(&start, NULL);
    int i = 0;
    for (i = 0; i < 1000000; i++) {
        ret = use_rsa_pub_verify_no_print(sign, sign_len);
    }
    gettimeofday(&end, NULL);

    // print sign count per second
    printf("use_rsa_pub_verify_perform ret: %d\n", ret);
    printf("use_rsa_pub_verify_perform time: %ld\n", (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000);
    printf("use_rsa_pub_verify_perform count per second: %f\n", 1000000.0 / ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000));

    free(sign);


    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    int i = 0;

    printf("\n******************************test generate rsa key********************************************************\n");
    // exec 1000 times, check memory leak
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (i = 0; i < 10; i++) {
        ret = generate_rsa_test_use_old();
    }
    gettimeofday(&end, NULL);
    printf("generate_rsa_test_use_old ret: %d\n", ret);
    printf("generate_rsa_test_use_old time: %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));

    // sleep(2);

    gettimeofday(&start, NULL);
    for (i = 0; i < 10; i++) {
        ret = generate_rsa_test_use_evp2();
    }
    gettimeofday(&end, NULL);
    printf("generate_rsa_test_use_evp2 ret: %d\n", ret);
    printf("generate_rsa_test_use_evp2 time: %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));

    // sleep(2);

    printf("\n******************************test save rsa key********************************************************\n");
    generate_rsa_key_write_file();

    printf("\n******************************test use rsa key to sign********************************************************\n");
    use_rsa_priv_sign();

    printf("\n******************************test use rsa key to verify********************************************************\n");
    use_rsa_pub_verify();

    printf("\n******************************test use rsa key to encrypt********************************************************\n");
    use_rsa_pub_encrypt();

    printf("\n*******************************test use rsa key to decrypt********************************************************\n");
    use_rsa_priv_decrypt();

    printf("\n*******************************test use rsa key to sign performance********************************************************\n");
    use_rsa_priv_sign_perform();

    printf("\n*******************************test use rsa key to verify performance********************************************************\n");
    use_rsa_pub_verify_perform();


    return ret;
}