#include <immintrin.h>  // 必须包含以使用 AVX 指令集
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

int avx_strcmp(const char* s1, const char* s2) {
    while (1) {
        // 加载 32 个字节到 YMM 寄存器
        __m256i chunk1 = _mm256_loadu_si256((__m256i*)s1);
        __m256i chunk2 = _mm256_loadu_si256((__m256i*)s2);

        // 比较
        __m256i result = _mm256_cmpeq_epi8(chunk1, chunk2);

        // 检查比较结果
        int mask = _mm256_movemask_epi8(result);

        if (mask != -1) {
            // 如果 mask 不是全 1，说明字符串不完全匹配
            return 0;
        }

        // 检查是否到达了字符串的结尾
        if (memchr(s1, 0, 32) || memchr(s2, 0, 32)) {
            break;
        }

        s1 += 32;
        s2 += 32;
    }

    return 1;
}

int
STRCMP (const char *p1, const char *p2)
{
  const unsigned char *s1 = (const unsigned char *) p1;
  const unsigned char *s2 = (const unsigned char *) p2;
  unsigned char c1, c2;
  do
    {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0')
	return c1 - c2;
    }
  while (c1 == c2);
  return c1 - c2;
}

#define NUM_ITERATIONS 100000

int main() {
    const char *str1 = "Hello, this is a test string for AVX comparison.";
    const char *str2 = "Hello, this is a test string for AVX comparison.";

    struct timeval start, end;
    long seconds, useconds;
    double total_time;
    int result;

    // 测试 avx_strcmp
    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        result = STRCMP(str1, str2);
    }
    gettimeofday(&end, NULL);
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    total_time = (seconds * 1000000 + useconds) / 1000000.0;
    printf("AVX strcmp used: %f useconds\n", total_time);

    // 测试 strcmp
    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        result = strcmp(str1, str2);
    }
    gettimeofday(&end, NULL);
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    total_time = (seconds * 1000000 + useconds) / 1000000.0;
    printf("Standard strcmp used: %f seconds\n", total_time);

    return 0;
}
