#include "openssl/asn1.h"
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <sys/time.h>

#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/bio.h>
#include <openssl/bn.h>

#include "parse_crt.h"

int parse_pem(const char *pem_file)
{
    int ret = 0;

    FILE *fp = NULL;
    X509 *x509 = NULL;

    fp = fopen(pem_file, "r");
    if (fp == NULL) {
        printf("fopen %s failed\n", pem_file);
        ret = -1;
        goto out;
    }

    // read pem file
    x509 = PEM_read_X509(fp, NULL, NULL, NULL);
    if (x509 == NULL) {
        printf("PEM_read_X509 failed\n");
        ret = -1;
        goto out2;
    }

    // get cert attributes

    // get version
    long version = X509_get_version(x509);
    printf("version: %ld\n", version);

    // get serial number
    ASN1_INTEGER *serial_number = X509_get_serialNumber(x509);
    BIGNUM *bn = ASN1_INTEGER_to_BN(serial_number, NULL);
    char *serial_number_str = BN_bn2hex(bn);
    printf("serial number: %s\n", serial_number_str);
    OPENSSL_free(serial_number_str);
    BN_free(bn);

    // get issuer
    X509_NAME *issuer = X509_get_issuer_name(x509);
    char *issuer_str = X509_NAME_oneline(issuer, NULL, 0);
    printf("issuer: %s\n", issuer_str);
    OPENSSL_free(issuer_str);

    // get subject
    X509_NAME *subject = X509_get_subject_name(x509);
    char *subject_str = X509_NAME_oneline(subject, NULL, 0);
    printf("subject: %s\n", subject_str);
    OPENSSL_free(subject_str);

    // get not before
    ASN1_TIME *not_before = X509_get_notBefore(x509);
    // print not before via bio
    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        printf("BIO_new failed\n");
        ret = -1;
        goto out3;
    }
    ASN1_TIME_print(bio, not_before);
    BUF_MEM *bptr = NULL;
    BIO_get_mem_ptr(bio, &bptr);
    printf("not before:\n%s\n", bptr->data);
    BIO_free(bio);

    // print not before in number format
    struct tm tm;
    ASN1_TIME_to_tm(not_before, &tm);
    time_t not_before_time = timegm(&tm);
    printf("not before: %ld\n", not_before_time);

    // get not after
    ASN1_TIME *not_after = X509_get_notAfter(x509);
    // print not after via bio
    bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        printf("BIO_new failed\n");
        ret = -1;
        goto out3;
    }
    ASN1_TIME_print(bio, not_after);
    BIO_get_mem_ptr(bio, &bptr);
    printf("not after:\n%s\n", bptr->data);
    BIO_free(bio);

    // print not after in number format
    ASN1_TIME_to_tm(not_after, &tm);
    time_t not_after_time = timegm(&tm);
    printf("not after: %ld\n", not_after_time);

    // get public key
    EVP_PKEY *public_key = X509_get_pubkey(x509);
    if (public_key == NULL) {
        printf("X509_get_pubkey failed\n");
        ret = -1;
        goto out3;
    }

    {
        // get public key in bin format
        int public_key_len = i2d_PUBKEY(public_key, NULL);
        if (public_key_len <= 0) {
            printf("i2d_PUBKEY failed\n");
            ret = -1;
            goto out3;
        }
        unsigned char *public_key_bin = malloc(public_key_len);
        if (public_key_bin == NULL) {
            printf("malloc failed\n");
            ret = -1;
            goto out3;
        }
        unsigned char *p = public_key_bin;
        public_key_len = i2d_PUBKEY(public_key, &p);
        if (public_key_len <= 0) {
            printf("i2d_PUBKEY failed\n");
            ret = -1;
            goto out3;
        }
        // print public key in hex
        int i = 0;
        for (i = 0; i < public_key_len; i++) {
            printf("%02x", public_key_bin[i]);
        }
        printf("\n");

        free(public_key_bin);
    }

    // print public key hex via bio
    bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        printf("BIO_new failed\n");
        ret = -1;
        goto out3;
    }

    PEM_write_bio_PUBKEY(bio, public_key);
    bptr = NULL;
    BIO_get_mem_ptr(bio, &bptr);
    printf("public key:\n%s\n", bptr->data);
    BIO_free(bio);

    // get signature and algorithm
    ASN1_BIT_STRING *signature = NULL;
    X509_ALGOR *signature_algorithm = NULL;
    X509_get0_signature(&signature, &signature_algorithm, x509);
    if (signature == NULL || signature_algorithm == NULL) {
        printf("X509_get0_signature failed\n");
        ret = -1;
        goto out3;
    }

    // print algorithm
    int pkey_type = OBJ_obj2nid(signature_algorithm->algorithm);
    const char *pkey_name = OBJ_nid2ln(pkey_type);
    printf("signature algorithm: %s\n", pkey_name);

    // print signature
    bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        printf("BIO_new failed\n");
        ret = -1;
        goto out3;
    }
    ASN1_STRING_print_ex(bio, signature, ASN1_STRFLGS_DUMP_ALL);
    bptr = NULL;
    BIO_get_mem_ptr(bio, &bptr);
    printf("signature:\n%s\n", bptr->data);
    BIO_free(bio);

    // print signature in hex
    int i = 0; 
    for (i = 0; i < signature->length; i++) {
        printf("%02x", signature->data[i]);
    }
    printf("\n");

out3:
    X509_free(x509);
out2:
    fclose(fp);
out:
    return ret;
}

int parse_der(const char *der_file)
{
    int ret = 0;

    FILE *fp = NULL;
    X509 *x509 = NULL;

    fp = fopen(der_file, "r");
    if (fp == NULL) {
        printf("fopen %s failed\n", der_file);
        ret = -1;
        goto out;
    }

    // read der file
    x509 = d2i_X509_fp(fp, NULL);
    if (x509 == NULL) {
        printf("d2i_X509_fp failed\n");
        ret = -1;
        goto out2;
    }

    // get cert attributes

    // get version
    long version = X509_get_version(x509);
    printf("version: %ld\n", version);

    // get serial number
    ASN1_INTEGER *serial_number = X509_get_serialNumber(x509);
    BIGNUM *bn = ASN1_INTEGER_to_BN(serial_number, NULL);
    char *serial_number_str = BN_bn2hex(bn);
    printf("serial number: %s\n", serial_number_str);
    OPENSSL_free(serial_number_str);
    BN_free(bn);

    // get issuer
    X509_NAME *issuer = X509_get_issuer_name(x509);
    char *issuer_str = X509_NAME_oneline(issuer, NULL, 0);
    printf("issuer: %s\n", issuer_str);
    OPENSSL_free(issuer_str);

    // get subject
    X509_NAME *subject = X509_get_subject_name(x509);
    char *subject_str = X509_NAME_oneline(subject, NULL, 0);
    printf("subject: %s\n", subject_str);
    OPENSSL_free(subject_str);

    // get not before
    ASN1_TIME *not_before = X509_get_notBefore(x509);
    // print not before via bio
    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        printf("BIO_new failed\n");
        ret = -1;
        goto out3;
    }
    ASN1_TIME_print(bio, not_before);
    BUF_MEM *bptr = NULL;
    BIO_get_mem_ptr(bio, &bptr);
    printf("not before:\n%s\n", bptr->data);
    BIO_free(bio);

    // print not before in number format
    struct tm tm;
    ASN1_TIME_to_tm(not_before, &tm);
    time_t not_before_time = timegm(&tm);
    printf("not before: %ld\n", not_before_time);

    // get not after
    ASN1_TIME *not_after = X509_get_notAfter(x509);
    // print not after via bio
    bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        printf("BIO_new failed\n");
        ret = -1;
        goto out3;
    }
    ASN1_TIME_print(bio, not_after);
    BIO_get_mem_ptr(bio, &bptr);
    printf("not after:\n%s\n", bptr->data);
    BIO_free(bio);

    // print not after in number format
    ASN1_TIME_to_tm(not_after, &tm);
    time_t not_after_time = timegm(&tm);
    printf("not after: %ld\n", not_after_time);

    // get public key
    EVP_PKEY *public_key = X509_get_pubkey(x509);
    if (public_key == NULL) {
        printf("X509_get_pubkey failed\n");
        ret = -1;
        goto out3;
    }
    // print public key hex via bio
    bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        printf("BIO_new failed\n");
        ret = -1;
        goto out3;
    }

    PEM_write_bio_PUBKEY(bio, public_key);
    bptr = NULL;
    BIO_get_mem_ptr(bio, &bptr);
    printf("public key:\n%s\n", bptr->data);
    BIO_free(bio);

    // get signature and algorithm
    ASN1_BIT_STRING *signature = NULL;
    X509_ALGOR *signature_algorithm = NULL;
    X509_get0_signature(&signature, &signature_algorithm, x509);
    if (signature == NULL || signature_algorithm == NULL) {
        printf("X509_get0_signature failed\n");
        ret = -1;
        goto out3;
    }

    // print algorithm
    int pkey_type = OBJ_obj2nid(signature_algorithm->algorithm);
    const char *pkey_name = OBJ_nid2ln(pkey_type);
    printf("signature algorithm: %s\n", pkey_name);

    // print signature
    bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        printf("BIO_new failed\n");
        ret = -1;
        goto out3;
    }
    ASN1_STRING_print_ex(bio, signature, ASN1_STRFLGS_DUMP_ALL);
    bptr = NULL;
    BIO_get_mem_ptr(bio, &bptr);
    printf("signature:\n%s\n", bptr->data);
    BIO_free(bio);

    // print signature in hex
    int i = 0;
    for (i = 0; i < signature->length; i++) {
        printf("%02x", signature->data[i]);
    }
    printf("\n");

out3:
    X509_free(x509);

out2:
    fclose(fp);

out:
    return ret;
}

int pem2der(const char *pem_file, const char *der_file)
{
    int ret = 0;

    FILE *fp = NULL;
    X509 *x509 = NULL;

    fp = fopen(pem_file, "r");
    if (fp == NULL) {
        printf("fopen %s failed\n", pem_file);
        ret = -1;
        goto out;
    }

    // read pem file
    x509 = PEM_read_X509(fp, NULL, NULL, NULL);
    if (x509 == NULL) {
        printf("PEM_read_X509 failed\n");
        ret = -1;
        goto out2;
    }

    // write der file
    FILE *d_fp = fopen(der_file, "w");
    if (d_fp == NULL) {
        printf("fopen %s failed\n", der_file);
        ret = -1;
        goto out3;
    }

    i2d_X509_fp(d_fp, x509);
    fclose(d_fp);

out3:
    X509_free(x509);
out2:
    fclose(fp);
out:
    return ret;
}

int der2pem(const char *der_file, const char *pem_file)
{
    int ret = 0;

    FILE *fp = NULL;
    X509 *x509 = NULL;

    fp = fopen(der_file, "r");
    if (fp == NULL) {
        printf("fopen %s failed\n", der_file);
        ret = -1;
        goto out;
    }

    // read der file
    x509 = d2i_X509_fp(fp, NULL);
    if (x509 == NULL) {
        printf("d2i_X509_fp failed\n");
        ret = -1;
        goto out2;
    }

    // write pem file
    FILE *p_fp = fopen(pem_file, "w");
    if (p_fp == NULL) {
        printf("fopen %s failed\n", pem_file);
        ret = -1;
        goto out3;
    }

    PEM_write_X509(p_fp, x509);
    fclose(p_fp);

out3:
    X509_free(x509);
out2:
    fclose(fp);
out:
    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    parse_pem("../mycert.pem");

    // pem2der("../mycert.pem", "../mycert.der1");

    // parse_der("../mycert.der1");

    // der2pem("../mycert.der1", "../mycert.pem1");

    // parse_pem("../mycert.pem1");

    return ret;
}