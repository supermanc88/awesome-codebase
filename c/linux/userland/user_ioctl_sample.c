#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>


#define DEVICE_NAME "/dev/kernel_ioctl_sample_dev"

#define IOCTL_MAGIC 'k'
#define IOCTL_GET_INT          _IOR(IOCTL_MAGIC, 0, int)
#define IOCTL_SET_INT          _IOW(IOCTL_MAGIC, 1, int)
#define IOCTL_GET_STRING       _IOR(IOCTL_MAGIC, 2, char *)
#define IOCTL_SET_STRING       _IOW(IOCTL_MAGIC, 3, char *)
#define IOCTL_MSG              _IO(IOCTL_MAGIC, 4)

int main(int argc, char *argv[])
{
    int fd;
    char buf[100] = "aaaaaaa";
    int v = 100;


    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    printf("IOCTL_SET_INT\n");
    if (ioctl(fd, IOCTL_SET_INT, &v) < 0) {
        perror("ioctl");
        exit(1);
    }
    printf("v = %d\n", v);

    printf("IOCTL_GET_INT\n");
    if (ioctl(fd, IOCTL_GET_INT, &v) < 0) {
        perror("ioctl");
        exit(1);
    }
    printf("v = %d\n", v);

    printf("IOCTL_SET_STRING\n");
    if (ioctl(fd, IOCTL_SET_STRING, buf) < 0) {
        perror("ioctl");
        exit(1);
    }
    printf("buf = %s\n", buf);

    printf("IOCTL_GET_STRING\n");
    if (ioctl(fd, IOCTL_GET_STRING, buf) < 0) {
        perror("ioctl");
        exit(1);
    }
    printf("buf = %s\n", buf);

    printf("IOCTL_MSG\n");
    if (ioctl(fd, IOCTL_MSG, NULL) < 0) {
        perror("ioctl");
        exit(1);
    }

    close(fd);
    return 0;
}