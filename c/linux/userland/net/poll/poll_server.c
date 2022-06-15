#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>


#define PORT 8080
#define MAX_CLIENTS 100
#define OPEN_MAX 1024
#define MAXLINE 80

int main(int argc, char *argv[])
{
	int listenfd, connfd, sockfd;
	int i = 0;
	int nready = 0;
	int n = 0;
	char buf[MAXLINE];
	struct pollfd clients[MAX_CLIENTS];
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(listenfd, MAX_CLIENTS) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	clients[0].fd = listenfd;
	clients[0].events = POLLIN;

	for (i = 1; i < MAX_CLIENTS; i++) {
		clients[i].fd = -1;
	}

	int maxi = 0;

	for ( ; ; ) {
		// 阻塞监听是否有客户端连接
		nready = poll(clients, maxi + 1, -1);

		if (clients[0].revents & POLLIN) {
			clilen = sizeof(cli_addr);
			// 接受客户端连接
			connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &clilen);
			if (connfd < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}
			printf("new client\n");

			for (i = 1; i < MAX_CLIENTS; i++) {
				if (clients[i].fd < 0) {
					// 找到空闲的位置
					clients[i].fd = connfd;
					break;
				}

			}
			if (i == MAX_CLIENTS - 1) {
				printf("too many clients\n");
				close(connfd);
			}

			clients[i].events = POLLIN;
			if (i > maxi) {
				maxi = i;
				printf("new maxi: %d\n", maxi);
			}
			// 如果没有更多的就绪事件时，继续回到poll阻塞
			if (--nready <= 0) {
				printf("nready <= 0\n");
				continue;
			}
		}

		for (i = 1; i <= maxi; i++) {
			if (clients[i].fd < 0) {
				printf("clients[%d].fd < 0\n", i);
				continue;
			}

			if (clients[i].revents & POLLIN) {
				printf("client data comming\n");
				sockfd = clients[i].fd;

				if ((n = read(sockfd, buf, MAXLINE)) < 0) {
					// connection reset by client
					if (errno == ECONNRESET) {
						close(sockfd);
						clients[i].fd = -1;
					} else {
						perror("read");
						exit(EXIT_FAILURE);
					}
				} else if (n == 0) {
					// connection closed by client
					close(sockfd);
					clients[i].fd = -1;
				} else {
					printf("read %d bytes, buf = %s\n", n, buf);
					write(sockfd, buf, n);
					memset(buf, 0, MAXLINE);
				}
				if (--nready <= 0) {
					break;
				}
			}
		}

	}


	return 0;
}
