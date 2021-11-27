#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// create a directory
int create_dir(const char *dir_path)
{
    return mkdir(dir_path, 0666);
}

// create a file
int create_file(const char *file_path)
{
    int fd = open(file_path, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
        return -1;
    close(fd);
    return 0;
}

// create a directory and all its parent directories
int create_dir_recursive(const char *dir_path)
{
    struct stat st;
    if (stat(dir_path, &st) == -1) {
        if (errno == ENOENT) {
            if (strrchr(dir_path, '/') == dir_path)
                return create_dir(dir_path);
            else {
                char *parent_path = strdup(dir_path);
                char *child_path = strrchr(parent_path, '/');
                *child_path = '\0';
                int ret = create_dir_recursive(parent_path);
                free(parent_path);
                if (ret == -1)
                    return -1;
                return create_dir(dir_path);
            }
        }
        else {
            return -1;
        }
    } else {
        if (S_ISDIR(st.st_mode))
            return 0;
        else
            return -1;
    }
}

// create a file and all its parent directories
int create_file_recursive(const char *file_path)
{
    char *parent_path = strdup(file_path);
    char *child_path = strrchr(parent_path, '/');
    *child_path = '\0';
    int ret = create_dir_recursive(parent_path);
    free(parent_path);
    if (ret == -1)
        return -1;
    return create_file(file_path);
}

int main(int argc, char *argv[])
{
    create_file_recursive("/test/aaaa/1.txt");
    return 0;
}
