#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>


#include <gcrypt.h>

#define SM4_PLAIN_TEXT1 "FEDCBA98765432100123456789ABCDEF"
#define SM4_TEST_KEY "0123456789ABCDEFFEDCBA9876543210"

#define MBEDTLS_SM4_BLOCK_LENGTH	16

#define MBEDTLS_SM4_KEY_LENGTH		16

static unsigned char _get_digit(char c )
{
	unsigned char d = 0x00;
	if( c >= 0x30 && c <= 0x39 ) d = c - 0x30;
	if( c >= 0x41 && c <= 0x46 ) d = c - 0x37;
	if( c >= 0x61 && c <= 0x66 ) d = c - 0x57;

	return( d );
}

static int _str2bin(char* str, int strLen, unsigned char* buf)
{
	int i = 0;
	for(i = 0; i < strLen/2; i++)
	{
		buf[i] = ((_get_digit(str[i*2]))<<4)+(_get_digit(str[i*2+1]));
	}
	return i;
}

void xerr(const char* msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

int gcrypt_init()
{
    /* Version check should be the very first call because it
       makes sure that important subsystems are intialized. */
    if (!gcry_check_version (GCRYPT_VERSION))
    {
        xerr("gcrypt: library version mismatch");
    }

    gcry_error_t err = 0;

    /* We don't want to see any warnings, e.g. because we have not yet
       parsed program options which might be used to suppress such
       warnings. */
    err = gcry_control (GCRYCTL_SUSPEND_SECMEM_WARN);

    /* ... If required, other initialization goes here.  Note that the
       process might still be running with increased privileges and that
       the secure memory has not been intialized.  */

    /* Allocate a pool of 16k secure memory.  This make the secure memory
       available and also drops privileges where needed.  */
    err |= gcry_control (GCRYCTL_INIT_SECMEM, 16384, 0);

    /* It is now okay to let Libgcrypt complain when there was/is
       a problem with the secure memory. */
    err |= gcry_control (GCRYCTL_RESUME_SECMEM_WARN);

    /* ... If required, other initialization goes here.  */

    /* Tell Libgcrypt that initialization has completed. */
    err |= gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);

    if (err) {
        xerr("gcrypt: failed initialization");
    }

    return 0;
}


int main(int argc, char *argv[])
{
    int ret = 0;

    ret = gcrypt_init();

    size_t sm4_blk_len = gcry_cipher_get_algo_blklen(GCRY_CIPHER_SM4);

    printf("sm4 block len: %d\n", sm4_blk_len);

    gcry_cipher_hd_t sm4_hd;
    gcry_error_t err = 0;

    err = gcry_cipher_open(&sm4_hd, GCRY_CIPHER_SM4, GCRY_CIPHER_MODE_ECB, 0);

    if (err) {
        xerr("gcrypt: failed to open cipher");
    }

    unsigned char sm4_key[MBEDTLS_SM4_KEY_LENGTH];
	unsigned char sm4_iv[MBEDTLS_SM4_BLOCK_LENGTH];

	unsigned char test_data_t1[5][MBEDTLS_SM4_BLOCK_LENGTH];
	unsigned char test_data_t2[5][MBEDTLS_SM4_BLOCK_LENGTH*2];

	
	_str2bin(SM4_TEST_KEY, strlen(SM4_TEST_KEY), sm4_key);
	
	_str2bin(SM4_PLAIN_TEXT1, strlen(SM4_PLAIN_TEXT1), test_data_t1[0]);


    char *key = "1234567890123456";
    char *plain = "1234567890123456";
    unsigned char *cipher = malloc(sm4_blk_len);

    err = gcry_cipher_setkey(sm4_hd, sm4_key, strlen(key));

    if (err) {
        xerr("gcrypt: failed to set key");
    }

    struct timeval tv1, tv2;

    gettimeofday(&tv1, NULL);

    int count = 1000000;
    int i = 0;

    for (i = 0; i < count; i++) {
        err = gcry_cipher_encrypt(sm4_hd, cipher, sm4_blk_len, test_data_t1[0], sm4_blk_len);
    }


    gettimeofday(&tv2, NULL);

    printf("time: %ld\n", (tv2.tv_sec - tv1.tv_sec) * 1000000 + tv2.tv_usec - tv1.tv_usec);

    if (err) {
        xerr("gcrypt: failed to encrypt");
    }

    i = 0;
    for (i = 0; i < sm4_blk_len; i++) {
        printf("%02x", cipher[i]);
    }
    printf("\n");

    unsigned char plain2[sm4_blk_len];

    err = gcry_cipher_decrypt(sm4_hd, plain2, sm4_blk_len, cipher, sm4_blk_len);

    if (err) {
        xerr("gcrypt: failed to decrypt");
    }

    for (i = 0; i < sm4_blk_len; i++) {
        printf("%02x", plain2[i]);
    }
    printf("\n");

    gcry_cipher_close(sm4_hd);


    return ret;
}