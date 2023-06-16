#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/time.h>


#include "sm4_core.h"



#define SM4_PLAIN_TEXT1 "FEDCBA98765432100123456789ABCDEF"
#define SM4_TEST_KEY "0123456789ABCDEFFEDCBA9876543210"

#define SM4_PLAIN_TEXT2 "FEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEF"
#define SM4_CIPHER_TEXT2_ECB "F0A2B07E64DD2C2590F93E4EDD90FBB4F0A2B07E64DD2C2590F93E4EDD90FBB4"

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

typedef __uint8_t uint8_t;
typedef __uint16_t uint16_t;
typedef __uint32_t uint32_t;
typedef __uint64_t uint64_t;

/**
 * \brief          SM4 context structure
 */
typedef struct
{
	u32 key[4];               /*!<  SM4 key value    */
    u32 rk[32];               /*!<  SM4 round keys    */
	int		is_bigend;
}
mbedtls_sm4_context;

#define GET_UINT32_LE(n,b,i)                            \
{                                                       \
    (n) = ( (u32) (b)[(i)    ]       )             \
        | ( (u32) (b)[(i) + 1] <<  8 )             \
        | ( (u32) (b)[(i) + 2] << 16 )             \
        | ( (u32) (b)[(i) + 3] << 24 );            \
}

#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (u32) (b)[(i)    ] << 24 )             \
        | ( (u32) (b)[(i) + 1] << 16 )             \
        | ( (u32) (b)[(i) + 2] <<  8 )             \
        | ( (u32) (b)[(i) + 3]       );            \
}

#define SMS4_ROUND     32 
#define ROL(x,y)       (((x) << (y)) | ((x) >> (32 - (y)))) 

static const uint32_t FK[4] = 
{
	0xA3B1BAC6ul, 0x56AA3350ul, 0x677D9197ul, 0xB27022DCul
};

static const uint32_t CK[SMS4_ROUND] = 
{
	0x00070e15ul, 0x1c232a31ul, 0x383f464dul, 0x545b6269ul,
	0x70777e85ul, 0x8c939aa1ul, 0xa8afb6bdul, 0xc4cbd2d9ul,
	0xe0e7eef5ul, 0xfc030a11ul, 0x181f262dul, 0x343b4249ul,
	0x50575e65ul, 0x6c737a81ul, 0x888f969dul, 0xa4abb2b9ul,
	0xc0c7ced5ul, 0xdce3eaf1ul, 0xf8ff060dul, 0x141b2229ul,
	0x30373e45ul, 0x4c535a61ul, 0x686f767dul, 0x848b9299ul,
	0xa0a7aeb5ul, 0xbcc3cad1ul, 0xd8dfe6edul, 0xf4fb0209ul,
	0x10171e25ul, 0x2c333a41ul, 0x484f565dul, 0x646b7279ul
};

static const uint8_t S_Data[256] =
{
	0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05,
	0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99,
	0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62,
	0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6,
	0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8,
	0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35,
	0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87,
	0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e,
	0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1,
	0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3,
	0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f,
	0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51,
	0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8,
	0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0,
	0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84,
	0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48
};

 static uint8_t Sbox(uint8_t input)
{
	uint8_t i,j,ret;

	i = input & 0x0f;
	j = (input) >> 4;
	ret = S_Data[i + (j << 4)];
	return ret;
}

 static uint32_t T_Transform(uint32_t input) 
{
	uint8_t  a[4];
	uint8_t  b[4];
	uint32_t B; 

	B = 0;

	a[0]=(uint8_t)(input & 0x000000FF);
	a[1]=(uint8_t)((input & 0x0000FF00) >> 8);
	a[2]=(uint8_t)((input & 0x00FF0000) >> 16);
	a[3]=(uint8_t)((input & 0xFF000000) >> 24); 

	b[0]=Sbox(a[0]);
	b[1]=Sbox(a[1]);
	b[2]=Sbox(a[2]);
	b[3]=Sbox(a[3]);

	B = ((uint32_t)b[0])|(((uint32_t)b[1])<<8)|(((uint32_t)b[2])<<16)|(((uint32_t)b[3])<<24); 

	return B;
}

 static uint32_t T2_Transform(uint32_t A) 
{
	uint32_t B = 0;
	uint32_t C = 0;

	B = T_Transform(A);   

	C = B ^ ROL(B,13) ^ ROL(B,23);

	return C;
}
static void SMS4_KeyExpansion(uint32_t *MK, uint32_t *rk)
{
	uint32_t K[36];
	uint8_t  i;    

	K[0] = MK[0] ^ FK[0];
	K[1] = MK[1] ^ FK[1];
	K[2] = MK[2] ^ FK[2];
	K[3] = MK[3] ^ FK[3];


	for(i = 0; i < SMS4_ROUND; i++)
	{      
		K[i + 4] = K[i] ^ T2_Transform(K[i+1] ^ K[i+2] ^ K[i+3] ^ CK[i]);
		rk[i] = K[i + 4];
	}
	return;
}


int mbedtls_sm4_setkey_ex( mbedtls_sm4_context *ctx, const unsigned char *key, unsigned int keybits, int is_bigend )
{
	if(!ctx || !key || keybits != 128)
	{
		return -1;
	}
	if(0 == is_bigend)
	{
		GET_UINT32_LE( ctx->key[0], key,  0 );
		GET_UINT32_LE( ctx->key[1], key,  4 );
		GET_UINT32_LE( ctx->key[2], key,  8 );
		GET_UINT32_LE( ctx->key[3], key, 12 );
	}else
	{
		GET_UINT32_BE( ctx->key[0], key,  0 );
		GET_UINT32_BE( ctx->key[1], key,  4 );
		GET_UINT32_BE( ctx->key[2], key,  8 );
		GET_UINT32_BE( ctx->key[3], key, 12 );
	}

	ctx->is_bigend = is_bigend;

	SMS4_KeyExpansion(ctx->key, ctx->rk);
	return (0);
}

int mbedtls_sm4_setkey( mbedtls_sm4_context *ctx, const unsigned char *key, unsigned int keybits )
{
	return mbedtls_sm4_setkey_ex(ctx, key, keybits, 1);
}


int main(int argc, char *argv[])
{
    int ret = 0;

    int a = 16;

    printf("a >> 4 = %d\n", a >> 4);
    unsigned char sm4_key[MBEDTLS_SM4_KEY_LENGTH];
    _str2bin(SM4_TEST_KEY, strlen(SM4_TEST_KEY), sm4_key);
	struct sm4_context ctx = {0};
	sm4_skcipher_setkey(&ctx, sm4_key, 16);

#if 1
	{



    unsigned char test_data_t1[5][MBEDTLS_SM4_BLOCK_LENGTH];

    _str2bin(SM4_PLAIN_TEXT1, strlen(SM4_PLAIN_TEXT1), test_data_t1[0]);

    unsigned char dst[5][MBEDTLS_SM4_BLOCK_LENGTH] = {0};

	// mbedtls_sm4_context ctx = {0};

	// mbedtls_sm4_setkey(&ctx, sm4_key, 128);


    int i = 0;

	for (i = 0; i < 32; i++)
	{
		printf("%08x ", ctx.rkey_enc[i]);
	}

	printf("\n");


	printf("ecb 1 block encrypt:\n");

	struct timeval start, end;


	gettimeofday(&start, NULL);

	for (i = 0; i < 1000000; i++) {

    	// sm4_aesni_avx_crypt4((u32 *)ctx.rk, dst[0], test_data_t1[0], 1);
		sm4_avx_ecb_encrypt(&ctx, dst[0], test_data_t1[0], 16);
	}

	gettimeofday(&end, NULL);

	printf("time = %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));


	printf("key:\n");
	for (i = 0; i < 16; i++)
	{
		printf("%02x ", sm4_key[i]);
	}

	printf("\n");

	printf("plain text:\n");
	for (i = 0; i < 16; i++)
	{
		printf("%02x ", test_data_t1[0][i]);
	}

	printf("\n");

	printf("cipher text:\n");

    for (i = 0; i < 16; i++)
    {
        printf("%02x ", dst[0][i]);
    }

	printf("\n");

	printf("src dst common memory test:\n");
	unsigned char common_mem[MBEDTLS_SM4_BLOCK_LENGTH] = {0};
	memcpy(common_mem, dst[0], 16);

	printf("common_mem, before dec:\n");
    for (i = 0; i < 16; i++)
    {
        printf("%02x ", common_mem[i]);
    }
	printf("\n");

	// sm4_aesni_avx_crypt4((u32 *)ctx.rk, dst[0], dst[0], 1);
	sm4_crypt_block(ctx.rkey_dec, common_mem, common_mem);

	printf("common_mem, after dec:\n");
    for (i = 0; i < 16; i++)
    {
        printf("%02x ", common_mem[i]);
    }
	printf("\n");


	unsigned char dst1[5][MBEDTLS_SM4_BLOCK_LENGTH] = {0};
    // sm4_aesni_avx_crypt4((u32 *)rk_dec, dst1[0], dst[0], 1);
	sm4_avx_ecb_decrypt(&ctx, dst1[0], dst[0], 16);

	printf("plain text:\n");
    for (i = 0; i < 16; i++)
    {
        printf("%02x ", dst1[0][i]);
    }

	printf("\n");

	}
#endif
#if 1
	printf("=========================================================================\n");
	{
		printf("ecb 2 block encrypt:\n");

		unsigned char sm4_key[MBEDTLS_SM4_KEY_LENGTH * 2];

		unsigned char test_data_t1[5][MBEDTLS_SM4_BLOCK_LENGTH * 2];

		_str2bin(SM4_TEST_KEY, strlen(SM4_TEST_KEY), sm4_key);
		_str2bin(SM4_PLAIN_TEXT2, strlen(SM4_PLAIN_TEXT2), test_data_t1[0]);

		unsigned char dst[5][MBEDTLS_SM4_BLOCK_LENGTH * 2] = {0};

		struct timeval start, end;

		int i = 0;

		gettimeofday(&start, NULL);

		for (i = 0; i < 1000000; i++) {

			// sm4_aesni_avx_crypt4((u32 *)ctx.rk, dst[0], test_data_t1[0], 2);
			sm4_avx_ecb_encrypt(&ctx, dst[0], test_data_t1[0], 16 * 2);
		}

		gettimeofday(&end, NULL);

		printf("time = %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));


		printf("key:\n");
		for (i = 0; i < 16; i++)
		{
			printf("%02x ", sm4_key[i]);
		}

		printf("\n");

		printf("plain text:\n");
		for (i = 0; i < 32; i++)
		{
			printf("%02x ", test_data_t1[0][i]);
		}

		printf("\n");

		printf("cipher text:\n");

		for (i = 0; i < 32; i++)
		{
			printf("%02x ", dst[0][i]);
		}

		printf("\n");


		unsigned char dst1[5][MBEDTLS_SM4_BLOCK_LENGTH * 2] = {0};
		// sm4_aesni_avx_crypt4((u32 *)rk_dec, dst1[0], dst[0], 2);
		sm4_avx_ecb_decrypt(&ctx, dst1[0], dst[0], 16 * 2);

		printf("plain text:\n");
		for (i = 0; i < 32; i++)
		{
			printf("%02x ", dst1[0][i]);
		}

		printf("\n");
	}

#endif

#if 1
	
	printf("=========================================================================\n");
	{
		printf("ecb 8 block encrypt:\n");

		#define SM4_PLAIN_TEXT3 "FEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEF"

		#define SM4_CIPHER_TEXT3_ECB "F0A2B07E64DD2C2590F93E4EDD90FBB4F0A2B07E64DD2C2590F93E4EDD90FBB4F0A2B07E64DD2C2590F93E4EDD90FBB4F0A2B07E64DD2C2590F93E4EDD90FBB4F0A2B07E64DD2C2590F93E4EDD90FBB4F0A2B07E64DD2C2590F93E4EDD90FBB4F0A2B07E64DD2C2590F93E4EDD90FBB4F0A2B07E64DD2C2590F93E4EDD90FBB4"

		unsigned char sm4_key[MBEDTLS_SM4_KEY_LENGTH * 8];

		unsigned char test_data_t1[5][MBEDTLS_SM4_BLOCK_LENGTH * 8];

		_str2bin(SM4_TEST_KEY, strlen(SM4_TEST_KEY), sm4_key);
		_str2bin(SM4_PLAIN_TEXT3, strlen(SM4_PLAIN_TEXT3), test_data_t1[0]);

		unsigned char dst[5][MBEDTLS_SM4_BLOCK_LENGTH * 8] = {0};


		struct timeval start, end;

		int i = 0;

		gettimeofday(&start, NULL);

		for (i = 0; i < 100000000; i++) {

			// sm4_aesni_avx_crypt8((u32 *)ctx.rk, dst[0], test_data_t1[0], 8);
			sm4_avx_ecb_encrypt(&ctx, dst[0], test_data_t1[0], 16 * 8);
		}

		gettimeofday(&end, NULL);

		printf("time = %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));

		// 计算每秒加密的数据量
		printf("speed = %f MB/s\n", 100000000.0 / ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0 ) * 128 / 1024 / 1024);


		printf("key:\n");
		for (i = 0; i < 16; i++)
		{
			printf("%02x ", sm4_key[i]);
		}

		printf("\n");

		printf("plain text:\n");
		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", test_data_t1[0][i]);
		}

		printf("\n");

		printf("cipher text:\n");

		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", dst[0][i]);
		}

		printf("\n");

		unsigned char dst1[5][MBEDTLS_SM4_BLOCK_LENGTH * 8] = {0};
		// sm4_aesni_avx_crypt8((u32 *)rk_dec, dst1[0], dst[0], 8);
		sm4_avx_ecb_decrypt(&ctx, dst1[0], dst[0], 16 * 8);

		printf("plain text:\n");
		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", dst1[0][i]);
		}

		printf("\n");
	}
#endif

#if 1
	
	printf("=========================================================================\n");
	{
		printf("cbc 8 block encrypt:\n");

		#define SM4_PLAIN_TEXT3 "FEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEF"
		#define SM4_TEST_IV  "112233445566778899AABBCCDDEEFF00"

		unsigned char sm4_key[MBEDTLS_SM4_KEY_LENGTH * 8];

		unsigned char sm4_iv[MBEDTLS_SM4_BLOCK_LENGTH * 8];

		unsigned char test_data_t1[5][MBEDTLS_SM4_BLOCK_LENGTH * 8];

		_str2bin(SM4_TEST_KEY, strlen(SM4_TEST_KEY), sm4_key);
		_str2bin(SM4_TEST_IV, strlen(SM4_TEST_IV), sm4_iv);
		_str2bin(SM4_PLAIN_TEXT3, strlen(SM4_PLAIN_TEXT3), test_data_t1[0]);

		unsigned char dst[5][MBEDTLS_SM4_BLOCK_LENGTH * 8] = {0};

		struct timeval start, end;

		int i = 0;

		gettimeofday(&start, NULL);

		for (i = 0; i < 1000000; i++) {

			_str2bin(SM4_TEST_IV, strlen(SM4_TEST_IV), sm4_iv);
			// sm4_aesni_avx_crypt8((u32 *)ctx.rk, dst[0], test_data_t1[0], 8);
			// sm4_avx_ecb_encrypt(&ctx, dst[0], test_data_t1[0], 16 * 8);
			sm4_avx_cbc_encrypt(&ctx, dst[0], test_data_t1[0], 16 * 8, sm4_iv, 16);
		}

		gettimeofday(&end, NULL);

		printf("time = %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));


		printf("key:\n");
		for (i = 0; i < 16; i++)
		{
			printf("%02x ", sm4_key[i]);
		}

		printf("\n");

		printf("plain text:\n");
		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", test_data_t1[0][i]);
		}

		printf("\n");

		printf("cipher text:\n");

		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", dst[0][i]);
		}

		printf("\n");
		gettimeofday(&start, NULL);

		unsigned char dst1[5][MBEDTLS_SM4_BLOCK_LENGTH * 8] = {0};
		for (i = 0; i < 1000000; i++) {
			_str2bin(SM4_TEST_IV, strlen(SM4_TEST_IV), sm4_iv);
			sm4_avx_cbc_decrypt(&ctx, dst1[0], dst[0], 16 * 8, sm4_iv, 16);
		}

		gettimeofday(&end, NULL);

		printf("time = %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));

		printf("plain text:\n");
		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", dst1[0][i]);
		}

		printf("\n");
	}

 #endif

 #if 1
	printf("=========================================================================\n");
	{
		printf("cfb 8 block encrypt:\n");

		#define SM4_PLAIN_TEXT3 "FEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEF"
		#define SM4_TEST_IV  "112233445566778899AABBCCDDEEFF00"

		unsigned char sm4_key[MBEDTLS_SM4_KEY_LENGTH * 8];

		unsigned char sm4_iv[MBEDTLS_SM4_BLOCK_LENGTH * 8];

		unsigned char test_data_t1[5][MBEDTLS_SM4_BLOCK_LENGTH * 8];

		_str2bin(SM4_TEST_KEY, strlen(SM4_TEST_KEY), sm4_key);
		_str2bin(SM4_TEST_IV, strlen(SM4_TEST_IV), sm4_iv);
		_str2bin(SM4_PLAIN_TEXT3, strlen(SM4_PLAIN_TEXT3), test_data_t1[0]);

		unsigned char dst[5][MBEDTLS_SM4_BLOCK_LENGTH * 8] = {0};

		struct timeval start, end;

		int i = 0;

		gettimeofday(&start, NULL);

		for (i = 0; i < 1000000; i++) {

			_str2bin(SM4_TEST_IV, strlen(SM4_TEST_IV), sm4_iv);
			// sm4_aesni_avx_crypt8((u32 *)ctx.rk, dst[0], test_data_t1[0], 8);
			// sm4_avx_ecb_encrypt(&ctx, dst[0], test_data_t1[0], 16 * 8);
			sm4_avx_cfb_encrypt(&ctx, dst[0], test_data_t1[0], 16 * 8, sm4_iv, 16);
		}

		gettimeofday(&end, NULL);

		printf("time = %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));


		printf("key:\n");
		for (i = 0; i < 16; i++)
		{
			printf("%02x ", sm4_key[i]);
		}

		printf("\n");

		printf("plain text:\n");
		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", test_data_t1[0][i]);
		}

		printf("\n");

		printf("cipher text:\n");

		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", dst[0][i]);
		}

		printf("\n");
		gettimeofday(&start, NULL);

		unsigned char dst1[5][MBEDTLS_SM4_BLOCK_LENGTH * 8] = {0};
		for (i = 0; i < 1000000; i++) {
			_str2bin(SM4_TEST_IV, strlen(SM4_TEST_IV), sm4_iv);
			sm4_avx_cfb_decrypt(&ctx, dst1[0], dst[0], 16 * 8, sm4_iv, 16);
		}

		gettimeofday(&end, NULL);

		printf("time = %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));

		printf("plain text:\n");
		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", dst1[0][i]);
		}

		printf("\n");
	}

#endif

#if 1
	printf("=========================================================================\n");
	{
		printf("ofb 8 block encrypt:\n");

		#define SM4_PLAIN_TEXT3 "FEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEFFEDCBA98765432100123456789ABCDEF"
		#define SM4_TEST_IV  "112233445566778899AABBCCDDEEFF00"

		unsigned char sm4_key[MBEDTLS_SM4_KEY_LENGTH * 8];

		unsigned char sm4_iv[MBEDTLS_SM4_BLOCK_LENGTH * 8];

		unsigned char test_data_t1[5][MBEDTLS_SM4_BLOCK_LENGTH * 8];

		_str2bin(SM4_TEST_KEY, strlen(SM4_TEST_KEY), sm4_key);
		_str2bin(SM4_TEST_IV, strlen(SM4_TEST_IV), sm4_iv);
		_str2bin(SM4_PLAIN_TEXT3, strlen(SM4_PLAIN_TEXT3), test_data_t1[0]);

		unsigned char dst[5][MBEDTLS_SM4_BLOCK_LENGTH * 8] = {0};

		struct timeval start, end;

		int i = 0;

		gettimeofday(&start, NULL);

		for (i = 0; i < 1000000; i++) {

			_str2bin(SM4_TEST_IV, strlen(SM4_TEST_IV), sm4_iv);
			sm4_avx_ofb_encrypt(&ctx, dst[0], test_data_t1[0], 16 * 8, sm4_iv, 16);
		}

		gettimeofday(&end, NULL);

		printf("time = %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));


		printf("key:\n");
		for (i = 0; i < 16; i++)
		{
			printf("%02x ", sm4_key[i]);
		}

		printf("\n");

		printf("plain text:\n");
		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", test_data_t1[0][i]);
		}

		printf("\n");

		printf("cipher text:\n");

		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", dst[0][i]);
		}

		printf("\n");
		gettimeofday(&start, NULL);

		unsigned char dst1[5][MBEDTLS_SM4_BLOCK_LENGTH * 8] = {0};
		for (i = 0; i < 1000000; i++) {
			_str2bin(SM4_TEST_IV, strlen(SM4_TEST_IV), sm4_iv);
			sm4_avx_ofb_decrypt(&ctx, dst1[0], dst[0], 16 * 8, sm4_iv, 16);
		}

		gettimeofday(&end, NULL);

		printf("time = %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));

		printf("plain text:\n");
		for (i = 0; i < 16 * 8; i++)
		{
			printf("%02x ", dst1[0][i]);
		}

		printf("\n");
	}

#endif
    return ret;
}