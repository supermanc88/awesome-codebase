#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>



int main(int argc, char *argv[])
{
    // get environment variables
    char *env_var = getenv("PATH");
    if (env_var) {
        printf("PATH: %s\n", env_var);
    }


    // set environment variables
    int ret = setenv("test_path", "/home/test", 1);
    if (ret == 0) {
        printf("setenv success\n");
    } else {
        printf("setenv failed\n");
    }

    env_var = getenv("test_path");
    if (env_var) {
        printf("test_path: %s\n", env_var);
    }

    // put environment variables
    ret = putenv("test_path=11111111");
    if (ret == 0) {
        printf("putenv success\n");
    } else {
        printf("putenv failed\n");
    }

    // 子进程也可以获取父进程的环境变量
    char cmd[256] = "echo $test_path";
    FILE *f = popen(cmd, "r");
    if (f) {
        char buf[256] = {0};
        fgets(buf, 256, f);
        printf("popen test_path: %s\n", buf);
        pclose(f);
    }

    env_var = getenv("test_path");
    if (env_var) {
        printf("test_path: %s\n", env_var);
    }

    // remove environment variables
    ret = unsetenv("test_path");
    if (ret == 0) {
        printf("unsetenv success\n");
    } else {
        printf("unsetenv failed\n");
    }

    return 0;
}