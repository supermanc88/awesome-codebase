#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// get process absolute path
//
// @param pid - process id
// @param path - absolute path of the process
// @return 0 if successful, -1 otherwise
int GetProcessAbsolutePath(int pid, char* path)
{
    char proc_path[256];
    sprintf(proc_path, "/proc/%d/exe", pid);
    int ret = readlink(proc_path, path, 256);
    if (ret == -1) {
        return -1;
    }
    return 0;
}

