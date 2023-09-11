#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

void generate_sm2_keypair_and_export(unsigned char pri_key[], unsigned char pub_key[])
{
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *pkey = NULL;

    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (!ctx) {
        printf("EVP_PKEY_CTX_new_id failed\n");
        return;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        printf("EVP_PKEY_keygen_init failed\n");
        return;
    }

    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_sm2) <= 0) {
        printf("EVP_PKEY_CTX_set_ec_paramgen_curve_nid failed\n");
        return;
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        printf("EVP_PKEY_keygen failed\n");
        return;
    }


    EC_KEY *ec_key = EVP_PKEY_get1_EC_KEY(pkey);
    if (!ec_key) {
        printf("EVP_PKEY_get1_EC_KEY failed\n");
        return;
    }

    // get private key
    const BIGNUM *priv_key = EC_KEY_get0_private_key(ec_key);
    if (!priv_key) {
        printf("EC_KEY_get0_private_key failed\n");
        return;
    }
    // private key to bin
    int priv_key_len = BN_bn2bin(priv_key, pri_key);

    // get public key
    const EC_POINT *pub_key_point = EC_KEY_get0_public_key(ec_key);
    if (!pub_key_point) {
        printf("EC_KEY_get0_public_key failed\n");
        return;
    }
    // public key to bin
    int pub_key_len = EC_POINT_point2oct(EC_KEY_get0_group(ec_key), pub_key_point, POINT_CONVERSION_UNCOMPRESSED, pub_key, 65, NULL);

    // get public key 64 bytes
    if (pub_key[0] == 0x04) {
        memmove(pub_key, pub_key + 1, 64);
    }


    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    EC_KEY_free(ec_key);
}

int evp_use_pri_key_sign_hash(unsigned char hash[], unsigned char pri_key[], unsigned char r[], unsigned char s[])
{
    EVP_PKEY_CTX *pctx = NULL;
    EVP_PKEY *pkey = NULL;

    pkey = EVP_PKEY_new();
    if (!pkey) {
        printf("EVP_PKEY_new failed\n");
        return -1;
    }

    EC_KEY *ec_key = EC_KEY_new_by_curve_name(NID_sm2);
    if (!ec_key) {
        printf("EC_KEY_new_by_curve_name failed\n");
        return -1;
    }

    // bin to private key
    BIGNUM *priv_key = BN_bin2bn(pri_key, 32, NULL);
    if (!priv_key) {
        printf("BN_bin2bn failed\n");
        return -1;
    }
    if (EC_KEY_set_private_key(ec_key, priv_key) != 1) {
        printf("EC_KEY_set_private_key failed\n");
        return -1;
    }

    if (EVP_PKEY_set1_EC_KEY(pkey, ec_key) != 1) {
        printf("EVP_PKEY_set1_EC_KEY failed\n");
        return -1;
    }

    pctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!pctx) {
        printf("EVP_PKEY_CTX_new failed\n");
        return -1;
    }

    if (EVP_PKEY_sign_init(pctx) <= 0) {
        printf("EVP_PKEY_sign_init failed\n");
        return -1;
    }

    size_t sig_len = 72;
    unsigned char sig[72] = {0};
    if (EVP_PKEY_sign(pctx, sig, &sig_len, hash, 32) != 1) {
        printf("EVP_PKEY_sign failed\n");
        return -1;
    }

    // der to r and s
    ECDSA_SIG *ecdsa_sig = ECDSA_SIG_new();
    if (!ecdsa_sig) {
        printf("ECDSA_SIG_new failed\n");
        return -1;
    }
    unsigned char *sig_ptr = sig;
    if (d2i_ECDSA_SIG(&ecdsa_sig, (const unsigned char **)&sig_ptr, sig_len) == NULL) {
        printf("d2i_ECDSA_SIG failed\n");
        return -1;
    }
    const BIGNUM *r_bn = NULL;
    const BIGNUM *s_bn = NULL;
    ECDSA_SIG_get0(ecdsa_sig, &r_bn, &s_bn);
    BN_bn2bin(r_bn, r);
    BN_bn2bin(s_bn, s);

    // free memory
    BN_free(priv_key);

    EVP_PKEY_CTX_free(pctx);
    EVP_PKEY_free(pkey);
    EC_KEY_free(ec_key);
    ECDSA_SIG_free(ecdsa_sig);

    return 0;
}

int evp_use_pub_key_sign_hash(unsigned char hash[], unsigned char pub_key[], unsigned char r[], unsigned char s[])
{
    EVP_PKEY_CTX *pctx = NULL;
    EVP_PKEY *pkey = NULL;

    pkey = EVP_PKEY_new();
    if (!pkey) {
        printf("EVP_PKEY_new failed\n");
        return -1;
    }

    EC_KEY *ec_key = EC_KEY_new_by_curve_name(NID_sm2);
    if (!ec_key) {
        printf("EC_KEY_new_by_curve_name failed\n");
        return -1;
    }

    // bin to public key
    unsigned char pub_key_0x04[65] = {0x04};
    memcpy(pub_key_0x04 + 1, pub_key, 64);
    EC_POINT *pub_key_point = EC_POINT_new(EC_KEY_get0_group(ec_key));
    if (!pub_key_point) {
        printf("EC_POINT_new failed\n");
        return -1;
    }
    if (EC_POINT_oct2point(EC_KEY_get0_group(ec_key), pub_key_point, pub_key_0x04, 65, NULL) != 1) {
        printf("EC_POINT_oct2point failed\n");
        return -1;
    }

    if (EC_KEY_set_public_key(ec_key, pub_key_point) != 1) {
        printf("EC_KEY_set_public_key failed\n");
        return -1;
    }

    if (EVP_PKEY_set1_EC_KEY(pkey, ec_key) != 1) {
        printf("EVP_PKEY_set1_EC_KEY failed\n");
        return -1;
    }

    pctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!pctx) {
        printf("EVP_PKEY_CTX_new failed\n");
        return -1;
    }

    if (EVP_PKEY_verify_init(pctx) <= 0) {
        printf("EVP_PKEY_verify_init failed\n");
        return -1;
    }

    // r and s to der
    ECDSA_SIG *ecdsa_sig = ECDSA_SIG_new();
    if (!ecdsa_sig) {
        printf("ECDSA_SIG_new failed\n");
        return -1;
    }
    BIGNUM *r_bn = BN_bin2bn(r, 32, NULL);
    BIGNUM *s_bn = BN_bin2bn(s, 32, NULL);
    if (!r_bn || !s_bn) {
        printf("BN_bin2bn failed\n");
        return -1;
    }
    if (ECDSA_SIG_set0(ecdsa_sig, r_bn, s_bn) != 1) {
        printf("ECDSA_SIG_set0 failed\n");
        return -1;
    }

    unsigned char *sig = NULL;
    int sig_len = i2d_ECDSA_SIG(ecdsa_sig, &sig);
    if (sig_len <= 0) {
        printf("i2d_ECDSA_SIG failed\n");
        return -1;
    }

    if (EVP_PKEY_verify(pctx, sig, sig_len, hash, 32) != 1) {
        printf("EVP_PKEY_verify failed\n");
        return -1;
    }

    // free memory
    OPENSSL_free(sig);
    EC_POINT_free(pub_key_point);

    EVP_PKEY_CTX_free(pctx);
    EVP_PKEY_free(pkey);
    EC_KEY_free(ec_key);
    ECDSA_SIG_free(ecdsa_sig);

    // printf("verify success\n");
    return 0;
}

#ifdef _TEST_
int main(int argc, char *argv[])
{
    int ret = 0;

    unsigned char pri_key[32] = {0};
    unsigned char pub_key[65] = {0};

    generate_sm2_keypair_and_export(pri_key, pub_key);

    int i = 0;
    printf("pri_key: ");
    for (i = 0; i < 32; i++) {
        printf("%02x", pri_key[i]);
    }
    printf("\n");

    printf("pub_key: ");
    for (i = 0; i < 65; i++) {
        printf("%02x", pub_key[i]);
    }
    printf("\n");

    // hash 0 ~ 31
    unsigned char hash[32] = {0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,
                              0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
                              0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e, 0x1f};

    unsigned char r[32] = {0};
    unsigned char s[32] = {0};

    // test 100000 times
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (i = 0; i < 100000; i++) {
        evp_use_pri_key_sign_hash(hash, pri_key, r, s);
    }
    gettimeofday(&end, NULL);

    // calc tps
    double tps = 100000.0 / (end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0);
    printf("tps: %f\n", tps);

    printf("r: ");
    for (i = 0; i < 32; i++) {
        printf("%02x", r[i]);
    }
    printf("\n");

    printf("s: ");
    for (i = 0; i < 32; i++) {
        printf("%02x", s[i]);
    }
    printf("\n");

    gettimeofday(&start, NULL);
    for (i = 0; i < 100000; i++) {
        evp_use_pub_key_sign_hash(hash, pub_key, r, s);
    }
    gettimeofday(&end, NULL);

    // calc tps
    tps = 100000.0 / (end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0);
    printf("tps: %f\n", tps);

    return ret;
}
#endif