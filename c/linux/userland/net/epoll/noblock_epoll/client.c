#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 10
#define SERV_PORT 9877

int main(int argc , char *argv[])
{
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    int sockfd;
    char ch = 'a';

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    int i = 0;
    while(1) {
        for (i = 0; i < MAXLINE/2; i++) {
            buff[i] = ch;
        }
        buff[i] = '\n';
        ch++;
        for (; i < MAXLINE; i++) {
            buff[i] = ch;
        }
        buff[i] = '\n';
        ch++;
        write(sockfd, buff, MAXLINE);
        sleep(5);
    }
    close(sockfd);

    return 0;
}