#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// Traverse all files with the specified suffix in the specified folder
// the file name separated by "|" charactor in file_list
// return the count of files in file_list

// Note: 
// In the ANSI version of this function, the name is limited to MAX_PATH characters. 
// To extend this limit to 32,767 wide characters, call the Unicode version of the function and prepend "\\?\" to the path. 
int traverse_folders(const char *folder, const char *suffix, char *file_list)
{
    char szDir[MAX_PATH] = {0};
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    int count = 0;
    // Get the full path of the directory
    if (GetFullPathName(folder, MAX_PATH, szDir, NULL) == 0)
    {
        return 0;
    }
    if (szDir[strlen(szDir) - 1] != '\\')
    {
        szDir[strlen(szDir)] = '\\';
    }

    strcat(szDir, "*.");
    strcat(szDir, suffix);
    // Find the first file in the directory
    hFind = FindFirstFile(szDir, &ffd);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        return 0;
    }
    // Find the rest of the files in the directory
    while (FindNextFile(hFind, &ffd) != 0)
    {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            strcat(file_list, "|");
            strcat(file_list, ffd.cFileName);
            count++;
        }
    }
    FindClose(hFind);
    return count;
}

int main(int argc, char *argv[])
{
    char file_list[MAX_PATH] = {0};
    traverse_folders("E:\\awesome-codebase\\cpp\\windows\\userland", "cpp", file_list);

    printf("%s\n", file_list);
    return 0;
}