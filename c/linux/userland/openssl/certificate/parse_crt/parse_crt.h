#ifndef PARSE_CRT_PARSE_CRT
#define PARSE_CRT_PARSE_CRT

/**
 * @brief 解析PEM格式的x509证书文件
 * 
 * @param pem_file 
 * @return int 0: 成功; -1: 失败
 */
int parse_pem(const char *pem_file);

/**
 * @brief 解析DER格式的x509证书文件
 * 
 * @param der_file 
 * @return int  0: 成功; -1: 失败
 */
int parse_der(const char *der_file);

/**
 * @brief 将PEM格式的x509证书文件转换为DER格式
 * 
 * @param pem_file 
 * @param der_file 
 * @return int 0: 成功; -1: 失败
 */
int pem2der(const char *pem_file, const char *der_file);

/**
 * @brief 将DER格式的x509证书文件转换为PEM格式
 * 
 * @param der_file 
 * @param pem_file 
 * @return int 0: 成功; -1: 失败
 */
int der2pem(const char *der_file, const char *pem_file);

#endif /* PARSE_CRT_PARSE_CRT */
