#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>

int main(int argc, char *argv[])
{
    int ret = 0;
    unsigned char ran[16];
    char ran_buf[33];
    srand(time(NULL));

    int i;
    for (i = 0; i < 16; i++) {
        ran[i] = rand() % 256;
    }

    for (i = 0; i < 16; i++) {
        sprintf(ran_buf + i * 2, "%02x", ran[i]);
    }

    printf("%s\n", ran_buf);


    return ret;
}