#include <unistd.h>
#include <stdio.h>


// parse the command line arguments by getopt

// getopt 中可能会使用到的全局变量
// extern char *optarg; 代表当前参数的参数值
// extern int optind, opterr, optopt; 代表下一个索引的位置；是否将错误信息输出到stderr，为0时不输出；不在选项中的字符串中的参数

// getopt会改变argv[]中参数的顺序。这点需要注意
// 注意 参数 : 格式为 -c 1123
// 参数 :: 格式为 -b123   两个冒号表示可以有参数也可以没有参数，但有参数的情况下，参数必须紧跟后面，没有空格

int main(int argc, char *argv[])
{
    int c;
    while ((c = getopt(argc, argv, "ab::c:d")) != -1) {
        switch (c) {
        case 'a':
            printf("option a\n");
            break;
        case 'b':
            printf("option b\n");
            break;
        case 'c':
            printf("option c with value %s\n", optarg);
            break;
        case 'd':
            printf("option d\n");
            break;
        // 不识别的参数getopt会返回一个?,参数会保存在optopt中
        case '?':
            printf("unknown option "%c"\n", (char)optopt);
            break;
        }
    }
    return 0;
}