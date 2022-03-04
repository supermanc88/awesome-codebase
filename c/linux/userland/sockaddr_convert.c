#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>


void get_ipv4_addr(void *src, char *addr_str)
{
    struct sockaddr_in *sin = (struct sockaddr_in *)src;

    inet_ntop(AF_INET, &sin->sin_addr, addr_str, INET_ADDRSTRLEN);

    printf("addr_str = %s\n", addr_str);
}


void get_ipv6_addr(void *src, char *addr_str)
{
    struct sockaddr_in6 *sin = (struct sockaddr_in *)src;

    inet_ntop(AF_INET6, &sin->sin6_addr, addr_str, INET6_ADDRSTRLEN);

    printf("addr_str = %s\n", addr_str);
}


void ipv4_addr_str_to_sockaddr(char *addr_str, void *sockaddr)
{
    struct sockaddr_in *sin = (struct sockaddr_in *)sockaddr;

    sin->sin_family = AF_INET;
    inet_pton(AF_INET, addr_str, &sin->sin_addr);
}


void ipv6_addr_str_to_sockaddr(char *addr_str, void *sockaddr)
{
    struct sockaddr_in6 *sin = (struct sockaddr_in *)sockaddr;

    sin->sin6_family = AF_INET6;
    inet_pton(AF_INET6, addr_str, &sin->sin6_addr);
}


void sockaddr_set_port(struct sockaddr *sockaddr, int port)
{
    if (sockaddr->sa_family == AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in *)sockaddr;
        sin->sin_port = htons(port);
    } else if (sockaddr->sa_family == AF_INET6) {
        struct sockaddr_in6 *sin = (struct sockaddr_in *)sockaddr;
        sin->sin6_port = htons(port);
    }
}

short sockaddr_get_port(struct sockaddr *sockaddr)
{
    if (sockaddr->sa_family == AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in *)sockaddr;
        return ntohs(sin->sin_port);
    } else if (sockaddr->sa_family == AF_INET6) {
        struct sockaddr_in6 *sin = (struct sockaddr_in *)sockaddr;
        return ntohs(sin->sin6_port);
    }

    return -1;
}