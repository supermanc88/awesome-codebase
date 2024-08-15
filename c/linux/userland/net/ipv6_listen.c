#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/*
用来测试ipv6的监听是否支持ipv4
测试方法：
server端：
[root@build test]# gcc -o test ipv6_listen.c 
[root@build test]# ls
ipv6_listen.c  test
[root@build test]# ./test 
Listening on [::]:12345
Accepted connection from ::ffff:10.210.20.17:47726

client端：
[root@localhost ~]# nc -nv 10.210.20.105 12345
Ncat: Version 7.70 ( https://nmap.org/ncat )
Ncat: Connected to 10.210.20.105:12345.
 */


int main(int argc, char *argv[])
{
    int ret = 0;

    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        ret = -1;
        goto out;
    }

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(12345);
    addr.sin6_addr = in6addr_any;

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        ret = -1;
        goto out;
    }

    if (listen(fd, 5) < 0) {
        perror("listen");
        ret = -1;
        goto out;
    }

    printf("Listening on [::]:12345\n");

    while (1) {
        struct sockaddr_in6 client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            perror("accept");
            ret = -1;
            goto out;
        }

        char client_addr_str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &client_addr.sin6_addr, client_addr_str, sizeof(client_addr_str));
        printf("Accepted connection from %s:%d\n", client_addr_str, ntohs(client_addr.sin6_port));

        char buf[1024];
        ssize_t n = read(client_fd, buf, sizeof(buf));
        if (n < 0) {
            perror("read");
            ret = -1;
            goto out;
        }

        write(STDOUT_FILENO, buf, n);

        close(client_fd);
    }


out:
    return ret;
}