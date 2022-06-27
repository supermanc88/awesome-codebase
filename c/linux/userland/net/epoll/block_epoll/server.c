#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAXLINE 10
#define SERV_PORT 9877

int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    int listenfd, connfd;
    char buff[MAXLINE];
    int efd;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, 20);

    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
    printf("accept a new connection from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

    struct epoll_event ev, events[20];

    efd = epoll_create(20);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = connfd;

    epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &ev);

    while (1) {
        int nfds = epoll_wait(efd, events, 20, -1);
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == connfd) {
                int n = read(connfd, buff, MAXLINE/2);
                if (n == 0) {
                    printf("client close\n");
                    close(connfd);
                    exit(0);
                }
                printf("read: %s\n", buff);
            }
        }
    }

    close(listenfd);
    close(connfd);
    close(efd);

    return 0;
}