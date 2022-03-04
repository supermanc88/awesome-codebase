#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// get current time
int get_current_time(char *time_str)
{
    struct timeval tv;
    struct tm *tm;
    int len;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    len = strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm);
    sprintf(time_str + len, ".%06ld", tv.tv_usec);
    return 0;
}


int main(int argc, char *argv[])
{
    char time_str[30];
    get_current_time(time_str);
    printf("%s\n", time_str);
    return 0;
}