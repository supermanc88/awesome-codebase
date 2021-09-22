#include <dlfnc.h>

// get so absolute path by func addr
int get_so_self_path(void *func_addr, char *path, int path_len)
{
	Dl_info info;
	if(dladdr(func_addr, &info) == 0)
	{
		return -1;
	}

    strcpy_s(path, path_len, info.dli_fname);

    return 0;
}