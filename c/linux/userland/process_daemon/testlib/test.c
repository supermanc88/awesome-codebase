#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

extern int g_signal;

int libfunc()
{
    int ret = 0;
    printf("Hello, World! single = %d, pid = %d\n", g_signal, getpid());

    while (1) {
        sleep(1);
    }
    return ret;
}