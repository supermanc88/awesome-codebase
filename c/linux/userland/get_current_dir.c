#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <string.h>
#include <sys/types.h>

// 获取程序所在绝对路径
int get_current_dir(char *path)
{
    int ret = 0;
    char str[256] = {0};
    char proc_path[1024] = {0};
    pid_t pid = getpid();

    snprintf(str, sizeof(str), "/proc/%d/exe", pid);

    int len = readlink(str, proc_path, sizeof(proc_path));

    if (len < 0) {
        ret = -1;
    } else {
        char *p = strrchr(proc_path, '/');
        if (p) {
            *(p+1) = '\0';
        }
        strcpy(path, proc_path);
    }

    return ret;
}


int main(int argc, char *argv[])
{
    int ret = 0;
    char path[1024];

    ret = get_current_dir(path);
    if (ret == 0) {
        printf("%s\n", path);
    }

    return ret;
}