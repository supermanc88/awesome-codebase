#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>

pthread_t tid1, tid2;

void *thread_fun(void *arg)
{
    while (1) {
        struct hostent *host = gethostbyname2((const char *)arg, AF_INET);
        if (host == NULL) {
            printf("gethostbyname2 failed, errno: %d\n", errno);
            return NULL;
        }
        printf("gethostbyname2 %s %p success\n", (const char *)arg, host->h_addr_list[0]);

        if (host->h_addr_list[0] == NULL) {
            printf("gethostbyname2 %s failed\n", (const char *)arg);
            exit(-1);
            return NULL;
        }
    }
    return NULL;
}

int multi_thread_test(const char *hostname, char *ip)
{
    int ret = 0;
    

    pthread_create(&tid1, NULL, thread_fun, (void *)ip);
    pthread_create(&tid2, NULL, thread_fun, (void *)hostname);


    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    if (argc != 3) {
        printf("Usage: %s <hostname> <ip>\n", argv[0]);
        return -1;
    }

    ret = multi_thread_test(argv[1], argv[2]);


    // wait all threads exit
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return ret;
}