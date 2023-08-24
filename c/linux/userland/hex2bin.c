#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

// 查找表，将16进制字符转换为整数值
static const uint8_t hex_char_to_bin[256] = {
    ['0'] = 0x0, ['1'] = 0x1, ['2'] = 0x2, ['3'] = 0x3,
    ['4'] = 0x4, ['5'] = 0x5, ['6'] = 0x6, ['7'] = 0x7,
    ['8'] = 0x8, ['9'] = 0x9, ['A'] = 0xa, ['B'] = 0xb,
    ['C'] = 0xc, ['D'] = 0xd, ['E'] = 0xe, ['F'] = 0xf,
    ['a'] = 0xa, ['b'] = 0xb, ['c'] = 0xc, ['d'] = 0xd,
    ['e'] = 0xe, ['f'] = 0xf
};

// 查找表，将整数值转换为16进制字符
static const char bin_to_hex_char[16] = {
    [0] = '0', [1] = '1', [2] = '2', [3] = '3',
    [4] = '4', [5] = '5', [6] = '6', [7] = '7',
    [8] = '8', [9] = '9', [10] = 'a', [11] = 'b',
    [12] = 'c', [13] = 'd', [14] = 'e', [15] = 'f'
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

// 将二进制数据转换为16进制字符串
// bin: 输入的二进制数据数组
// bin_size: 输入数组的大小
// hex: 输出的16进制字符串
// hex_size: 输出数组的大小，应该是输入数组长度的2倍
// 返回值: 成功转换的字节数
size_t bin_to_hex(const uint8_t *bin, size_t bin_size, char *hex, size_t hex_size) {
    if (hex_size < bin_size * 2) {
        return 0;
    }

    size_t i = 0, j = 0;
    for (i = 0, j = 0; i < bin_size; i++, j += 2) {
        uint8_t high = bin[i] >> 4;
        uint8_t low = bin[i] & 0x0f;
        hex[j] = bin_to_hex_char[high];
        hex[j+1] = bin_to_hex_char[low];
    }
    return bin_size * 2;
}

int hex2bin(char *hex, unsigned char *bin, int bin_len)
{
    int i;
    int hex_len = strlen(hex);
    if (hex_len > bin_len * 2) {
        return -1;
    }
    for (i = 0; i < hex_len; i += 2) {
        char tmp[3] = {0};
        memcpy(tmp, hex + i, 2);
        bin[i / 2] = strtol(tmp, NULL, 16);
    }
    return 0;
}

int sc_binary2hex(const unsigned char* bin_data, unsigned int bin_len, char * hex_str, unsigned int * hex_len, int is_upper)
{
 unsigned int i = 0;
 if((* hex_len) < bin_len*2+1)
 {
  return -1;
 }
 *hex_len = bin_len*2;
 if(is_upper)
 {
  for(i = 0; i < bin_len; i++)
  {
   sprintf((char*)&hex_str[i*2], "%02X", bin_data[i]);
  }
 }else
 {
  for(i = 0; i < bin_len; i++)
  {
   sprintf((char*)&hex_str[i*2], "%02x", bin_data[i]);
  }
 }

 return 0;
}


static unsigned char _get_hex_c(char h)
{
 if( h >= '0' && h <= '9' ) return  (unsigned char )(h - '0');
 if( h >= 'a' && h <= 'f' ) return  (unsigned char )(h - 'a'+10);
 if( h >= 'A' && h <= 'F' ) return  (unsigned char )(h - 'A'+10);

 return 0x0F;
}
int sc_hex2binary(const char * hex_data, unsigned int hex_len, unsigned char *bin_data, unsigned int *bin_len)
{
 unsigned int i = 0;
 const char * offset = hex_data;
 if((hex_len+1) /2 > (*bin_len) )
 {
  return -1;
 }

 for(i = 0; i < hex_len/2; i ++)
 {
  bin_data[i] = (_get_hex_c(offset[i*2])<<4) + _get_hex_c(offset[i*2+1]);
 }
 if(hex_len %2)
 {
  bin_data[i] = (_get_hex_c(offset[i*2])<<4) + 0x00;
  i++;
 }
 *bin_len = i;
 return 0;
}


int main() {
    const char *hex_str = "1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d1a2f3c4d";
    uint8_t bin_data[64];

    // 计算两个函数的性能
    // 每个函数执行100000次

    struct timeval start, end;
    int i = 0;
    gettimeofday(&start, NULL);
    for (i = 0; i < 100000; i++) {
        hex_to_bin(hex_str, bin_data, sizeof(bin_data));
    }
    gettimeofday(&end, NULL);
    printf("hex_to_bin: %ld us\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

    gettimeofday(&start, NULL);
    for (i = 0; i < 100000; i++) {
        hex2bin(hex_str, bin_data, sizeof(bin_data));
    }
    gettimeofday(&end, NULL);
    printf("hex2bin: %ld us\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

    gettimeofday(&start, NULL);
    int bin_len = sizeof(bin_data);
    for (i = 0; i < 100000; i++) {
        sc_hex2binary(hex_str, strlen(hex_str), bin_data, &bin_len);
    }
    gettimeofday(&end, NULL);
    printf("sc_hex2binary: %ld us\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

    char hex_str2[129];
    unsigned int hex_len = 128;
    gettimeofday(&start, NULL);
    for (i = 0; i < 100000; i++) {
        sc_binary2hex(bin_data, sizeof(bin_data), hex_str2, &hex_len, 0);
    }
    gettimeofday(&end, NULL);
    printf("sc_binary2hex: %ld us\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

    printf("hex_str: %s\n", hex_str);

    gettimeofday(&start, NULL);
    for (i = 0; i < 100000; i++) {
        bin_to_hex(bin_data, sizeof(bin_data), hex_str2, sizeof(hex_str2));
    }
    gettimeofday(&end, NULL);
    printf("bin_to_hex: %ld us\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

    printf("hex_str2: %s\n", hex_str2);


    // size_t converted = hex_to_bin(hex_str, bin_data, sizeof(bin_data));
    
    // printf("Converted %zu bytes:\n", converted);
    // size_t i = 0;
    // for (i = 0; i < converted; i++) {
    //     printf("%02x ", bin_data[i]);
    // }
    printf("\n");

    return 0;
}