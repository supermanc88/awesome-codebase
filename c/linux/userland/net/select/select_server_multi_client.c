#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, char *argv[])
{
	int ret = 0;
	int server_sockfd = 0;
	int client_sockfd = 0;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	fd_set readfds;
	fd_set testfds;

	int server_len = sizeof(server_addr);
	int client_len = sizeof(client_addr);

	if (argc != 3) {
		printf("Usage: %s <server_addr> <server_port>\n", argv[0]);
		exit(1);
	}

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sockfd < 0) {
		printf("socket error\n");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	server_addr.sin_port = htons(atoi(argv[2]));

	ret = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
		printf("bind error\n");
		exit(1);
	}

	ret = listen(server_sockfd, 5);
	if (ret < 0) {
		printf("listen error\n");
		exit(1);
	}

	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);

	while (1) {
		int fd;
		testfds = readfds;
		printf("server waiting\n");

		ret = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
		if (ret < 0) {
			printf("select error\n");
			exit(1);
		}

		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if (FD_ISSET(fd, &testfds)) {
				if (fd == server_sockfd) {
					client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);
					if (client_sockfd < 0) {
						printf("accept error\n");
						exit(1);
					}
					FD_SET(client_sockfd, &readfds);
					printf("server accept a client\n");
				} else {
					char buf[1024];
					int len = recv(fd, buf, sizeof(buf), 0);
					if (len > 0) {
						buf[len] = '\0';
						printf("server receive: %s\n", buf);
						send(fd, buf, len, 0);
					} else {
						printf("server close a client\n");
						FD_CLR(fd, &readfds);
						close(fd);
					}
				}
			}
		}


	}


	return ret;
}
