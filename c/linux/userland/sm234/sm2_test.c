#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

void generate_sm2_keypair_and_crypto()
{
    EVP_PKEY_CTX *ctx;
    EVP_PKEY *pkey;

    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    EVP_PKEY_paramgen_init(ctx);
    EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_sm2);
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_keygen(ctx, &pkey);
    EVP_PKEY_CTX_free(ctx);

    // print keypair to stdout
    // get private key
    EC_KEY *ec_key = EVP_PKEY_get0_EC_KEY(pkey);
    const BIGNUM *priv_key = EC_KEY_get0_private_key(ec_key);
    char *priv_key_hex = BN_bn2hex(priv_key);
    printf("private key: %s\n", priv_key_hex);

    // get public key
    const EC_POINT *pub_key = EC_KEY_get0_public_key(ec_key);
    char *pub_key_hex = EC_POINT_point2hex(EC_KEY_get0_group(ec_key), pub_key, POINT_CONVERSION_UNCOMPRESSED, NULL);
    printf("public key: %s\n", pub_key_hex);

    // free
    OPENSSL_free(priv_key_hex);
    OPENSSL_free(pub_key_hex);

    // crypto
    // https://github.com/supermanc88/SM2-encrypt-and-decrypt


    // char *plaintext = "hello world11111";
    // int in_len = strlen(plaintext);
    // unsigned char *out = NULL;
    // size_t out_len = 0;

    // // use public key to encrypt
    // EVP_PKEY_CTX *ctx_enc = EVP_PKEY_CTX_new(pkey, NULL);
    // EVP_PKEY_encrypt_init(ctx_enc);
    // EVP_PKEY_encrypt(ctx_enc, NULL, &out_len, plaintext, in_len);
    // out = OPENSSL_malloc(out_len);
    // EVP_PKEY_encrypt(ctx_enc, out, &out_len, plaintext, in_len);
    // EVP_PKEY_CTX_free(ctx_enc);
    // printf("encrypt: %s\n", out);

    // // use private key to decrypt
    // EVP_PKEY_CTX *ctx_dec = EVP_PKEY_CTX_new(pkey, NULL);
    // EVP_PKEY_decrypt_init(ctx_dec);
    // EVP_PKEY_decrypt(ctx_dec, NULL, &out_len, out, out_len);
    // unsigned char *plaintext2 = OPENSSL_malloc(out_len);
    // EVP_PKEY_decrypt(ctx_dec, plaintext2, &out_len, out, out_len);
    // EVP_PKEY_CTX_free(ctx_dec);

    // printf("plaintext: %s\n", plaintext2);

    EVP_PKEY_free(pkey);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    generate_sm2_keypair_and_crypto();
    return ret;
}