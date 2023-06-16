#ifndef _SM4_CORE_H_
#define _SM4_CORE_H_

#define SM4_RKEY_WORDS 32
#define SM4_BLOCK_SIZE 16
#define SM4_KEY_SIZE 16
#define SM4_CRYPT8_BLOCK_SIZE	(SM4_BLOCK_SIZE * 8)




#define EINVAL          22      /* Invalid argument */

struct sm4_context {
    unsigned int rkey_enc[32];
    unsigned int rkey_dec[32];
};

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
typedef signed char		s8;
typedef short			s16;
typedef int			s32;
typedef long long		s64;

#define ____cacheline_aligned

/**
 * @brief 设置密钥
 * 
 * @param ctx context
 * @param key key
 * @param key_len key length, must be 16 
 * @return int 
 */
int sm4_skcipher_setkey(struct sm4_context *ctx, const unsigned char *key, unsigned int key_len);

/**
 * @brief 一个block运算，加密或解密，供实现其它模式使用
 * 
 * @param rkey 轮密钥
 * @param out 
 * @param in 
 * @return int 
 */
int sm4_crypt_block(const unsigned int *rkey, unsigned char *out, unsigned char *in);

/**
 * @brief ecb模式加密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @return int 
 */
int sm4_avx_ecb_encrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len);

/**
 * @brief ecb模式解密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @return int 
 */
int sm4_avx_ecb_decrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len);

/**
 * @brief cbc模式加密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_cbc_encrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len);

/**
 * @brief cbc模式解密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_cbc_decrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len);

/**
 * @brief cfb模式加密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_cfb_encrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len);

/**
 * @brief cfb模式解密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_cfb_decrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len);

/**
 * @brief ofb模式加密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_ofb_encrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len);

/**
 * @brief ofb模式解密
 * 
 * @param ctx 
 * @param out 
 * @param in 
 * @param in_len 
 * @param iv 
 * @param iv_len 
 * @return int 
 */
int sm4_avx_ofb_decrypt(struct sm4_context *ctx, unsigned char *out, const unsigned char *in, unsigned int in_len, unsigned char *iv, int iv_len);


#endif /* _SM4_CORE_H_ */
