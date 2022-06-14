#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <server_ip> <port>\n", argv[0]);
		exit(1);
	}
	char *server_ip = argv[1];
	int server_port = atoi(argv[2]);

	printf("server_ip: %s\n", server_ip);
	printf("server_port: %d\n", server_port);

	int ret = 0;
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("socket error\n");
		exit(1);
	}

	ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
		printf("bind error\n");
		exit(1);
	}

	ret = listen(sockfd, 5);
	if (ret < 0) {
		printf("listen error\n");
		exit(1);
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (connfd < 0) {
		printf("accept error\n");
		exit(1);
	}

	char buf[1024] = {0};
	fd_set readfds;
	FD_ZERO(&readfds);
	fd_set exceptfds;
	FD_ZERO(&exceptfds);

	while (1) {
		FD_SET(connfd, &readfds);
		FD_SET(connfd, &exceptfds);
		ret = select(connfd + 1, &readfds, NULL, &exceptfds, NULL);
		if (ret < 0) {
			printf("select error\n");
			exit(1);
		}

		if (FD_ISSET(connfd, &readfds)) {
			ret = recv(connfd, buf, sizeof(buf), 0);
			if (ret < 0) {
				printf("recv error\n");
				exit(1);
			}
			if (ret == 0) {
				printf("client close\n");
				break;
			}
			printf("recv: %s\n", buf);
			memset(buf, 0, sizeof(buf));
		}
	}

	close(connfd);
	close(sockfd);

	return 0;
}

