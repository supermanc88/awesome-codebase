#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define one_printf(arg...) printf(arg)
#define two_printf(arg...) one_printf(arg)

int main(int argc, char *argv[])
{
    int ret = 0;

    two_printf("hello world\n");

    two_printf("hello world %d\n", 1);

    return ret;
}