#include <windows.h>


// 从路径中获取文件名
// 返回值：
//		成功：文件名
//		失败：NULL
char* GetFileNameFromPath(char* pPath)
{
	char* pFileName = NULL;
	int nLen = strlen(pPath);
	int nIndex = nLen - 1;
	for (; nIndex >= 0; nIndex--)
	{
		if (pPath[nIndex] == '\\' || pPath[nIndex] == '/')
		{
			break;
		}
	}
	if (nIndex >= 0)
	{
		pFileName = pPath + nIndex + 1;
	}
	return pFileName;
}

// 从路径中获取无后缀的文件名
// 注意：此函数会修改输入的路径
// 返回值：
//		成功：文件名
//		失败：NULL
char* GetFileNameNoSuffixFromPath(char* pPath)
{
	char* pFileName = NULL;
	int nLen = strlen(pPath);
	int nIndex = nLen - 1;
	for (; nIndex >= 0; nIndex--)
	{
		if (pPath[nIndex] == '\\' || pPath[nIndex] == '/')
		{
			break;
		}
	}
	if (nIndex >= 0)
	{
		pFileName = pPath + nIndex + 1;
		int nSuffixIndex = strlen(pFileName) - 1;
		for (; nSuffixIndex >= 0; nSuffixIndex--)
		{
			if (pFileName[nSuffixIndex] == '.')
			{
				pFileName[nSuffixIndex] = '\0';
				break;
			}
		}
	}
	return pFileName;
}