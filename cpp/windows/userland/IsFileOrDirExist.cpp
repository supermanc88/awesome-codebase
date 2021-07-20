#include <windows.h>


// 判断文件是否存在
bool IsFileEixst(const char *filename)
{
    DWORD attr = GetFileAttributes(filename);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}


// 判断目录是否存在
bool IsDirExist(const char *dirname)
{
    DWORD attr = GetFileAttributes(dirname);
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}