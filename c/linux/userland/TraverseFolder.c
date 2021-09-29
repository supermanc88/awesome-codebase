#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

// Traverse all files with the specified suffix in the specified folder
// the file name separated by "|" charactor in file_list
// return the count of files in file_lis

int traverse_folder(char *folder, char *suffix, char *file_list)
{
    DIR *dir;
    struct dirent *ptr;
    char child_path[256];
    int file_count = 0;
    int len;
    dir = opendir(folder);
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if (ptr->d_type == 8) //file
        {
            if (strstr(ptr->d_name, suffix) != NULL)
            {
                strcpy(child_path, folder);
                strcat(child_path, "/");
                strcat(child_path, ptr->d_name);
                len = strlen(child_path);
                child_path[len] = '|';
                child_path[len + 1] = '\0';
                strcat(file_list, child_path);
                file_count++;
            }
        }
        else if (ptr->d_type == 4) //folder
        {
            strcpy(child_path, folder);
            strcat(child_path, "/");
            strcat(child_path, ptr->d_name);
            file_count += traverse_folder(child_path, suffix, file_list);
        }
    }
    closedir(dir);
    return file_count;
}

int main(int argc, char *argv[])
{
    char file_list[256] = {0};
    traverse_folder("/root/testc", "cpp", file_list);

    printf("%s\n", file_list);
    return 0;
}