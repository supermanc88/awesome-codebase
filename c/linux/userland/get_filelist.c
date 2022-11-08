#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>

/**
 * @brief 获取指定目录下的文件，不包括子目录
 * 
 * @param dirpath 
 * @param filelist 
 * @return int 
 */
int get_filelist(char *dirpath, char *filelist)
{
    int ret = 0;

    // 获取指定目录下的所有文件
    DIR *dir = opendir(dirpath);
    if (dir == NULL)
    {
        perror("opendir error");
        return -1;
    }

    // 逐个读取目录下的文件
    struct dirent *ptr = NULL;
    while ((ptr = readdir(dir)) != NULL)
    {
        // 跳过.和..这两个目录
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;

        // 如果是目录，递归调用get_filelist
        if (ptr->d_type == DT_DIR) {
            // char subdir[1024] = {0};
            // sprintf(subdir, "%s/%s", dirpath, ptr->d_name);
            // get_filelist(subdir, filelist);
            continue;
        } else {
            // char filepath[1024] = {0};
            // sprintf(filepath, "%s/%s", dirpath, ptr->d_name);

            // 将文件路径拼接到filelist中
            strcat(filelist, "|");
            strcat(filelist, ptr->d_name);
        }
    }

    return ret;
}


int main(int argc, char *argv[])
{
    int ret = 0;

    char filelist[1024] = {0};

    ret = get_filelist("/root/test/", filelist);
    if (ret != 0) {
        printf("get_filelist failed, ret = %d\n", ret);
        return ret;
    }

    printf("filelist = %s\n", filelist);
    return ret;
}