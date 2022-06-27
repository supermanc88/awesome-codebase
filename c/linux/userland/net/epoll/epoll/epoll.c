#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>

#define MAXLINE 10

// epoll 管道读写
int main(int argc, char *argv[])
{
    int efd;
    int i;
    int pfd[2]; // 管道
    pid_t pid;
    char buf[MAXLINE] = {0};
    char ch = 'a';

    if (pipe(pfd) == -1) {
        perror("pipe");
        exit(1);
    }

    if ((pid = fork()) == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // 子进程
        close(pfd[0]); // 关闭管道读端
        while (1) {
            for (i = 0; i < MAXLINE/2; i++) {
                buf[i] = ch;
            }
            buf[i - 1] = '\n';
            ch++;

            for ( ; i < MAXLINE; i++) {
                buf[i] = ch;
            }
            buf[i - 1] = '\n';
            ch++;
            write(pfd[1], buf, MAXLINE);
            printf("write to pipe: %s\n", buf);
            sleep(5);
        }
        close(pfd[1]);
    } else { // 父进程
        close(pfd[1]); // 关闭管道写端
        efd = epoll_create(10); // 创建 epoll 句柄
        if (efd == -1) {
            perror("epoll_create");
            exit(1);
        }
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET; // 设置 epoll 事件为 EPOLLIN, 边缘触发模式
        // ev.events = EPOLLIN; // 默认为EPOLLLT
        ev.data.fd = pfd[0]; // 设置 epoll 事件对应的文件描述符为 pfd[0]
        if (epoll_ctl(efd, EPOLL_CTL_ADD, pfd[0], &ev) == -1) {
            perror("epoll_ctl");
            exit(1);
        }
        struct epoll_event events[10];
        int n;
        while (1) {
            n = epoll_wait(efd, events, 10, -1); // 等待 epoll 事件
            if (events[0].data.fd == pfd[0]) {
                if (read(pfd[0], buf, MAXLINE/2) == -1) {
                    perror("read");
                    exit(1);
                }
                printf("read from pipe: %s\n", buf);
            }
        }

        close(efd);
        close(pfd[0]);
    }

    return 0;
}
