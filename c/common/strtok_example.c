#include <stdio.h>
#include <string.h>


/**
* \require <string.h>
* \brief main 使用strtok函数对字符串进行分割
* \param argc
* \param argv
* \return
*/
int main(int argc, char **argv)
{
	char str[] = "-this, a sample string";
	char *pch;
	pch  = strtok(str, " ");
	while (pch != NULL) {
		printf("%s\n", pch);
		pch = strtok(NULL, " ");
	}
	return 0;
}
