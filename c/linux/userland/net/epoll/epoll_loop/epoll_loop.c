// epoll 基于非阻塞IO事件驱动模型

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define MAX_EVENTS 1024         // 监听的最大事件数
#define MAX_BUFFER_SIZE 1024    // 每个连接的最大缓冲区大小
#define SERV_PORT 9877          // 服务器端口

struct my_events {
    int fd;                                                 // 客户端连接的文件描述符
    int events;                                             // epoll事件
    void *arg;                                              // 用户数据
    void (*call_back)(int fd, int events, void *arg);       // 回调函数
    int status;                                             // 是否在epoll中注册
    char buffer[MAX_BUFFER_SIZE];                           // 缓冲区
    int buffer_size;                                        // 缓冲区大小
    long last_active;                                       // 记录加入epoll的时间
};

int g_efd;                                                  // 全局epoll文件描述符
struct my_events g_events[MAX_EVENTS + 1];                  // 全局事件集合,+1 listen fd

void event_set(struct my_events *ev, int fd, void (*call_back)(int fd, int events, void *arg), void *arg);
void event_add(int efd, struct my_events *ev, int events);
void event_del(int efd, struct my_events *ev);
void accept_conn_callback(int fd, int events, void *arg);
void recv_data_callback(int fd, int events, void *arg);
void send_data_callback(int fd, int events, void *arg);

// event set 函数
void event_set(struct my_events *ev, int fd, void (*call_back)(int fd, int events, void *arg), void *arg)
{
    ev->fd = fd;
    ev->events = 0;
    ev->arg = arg;
    ev->call_back = call_back;
    ev->status = 0;
    ev->last_active = time(NULL);
}

// event add 函数
void event_add(int efd, struct my_events *ev, int events)
{
    struct epoll_event epv = {0};
    ev->events = epv.events = events;
    epv.data.ptr = ev;
    int op;
    
    if (ev->status == 1) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }
    if (epoll_ctl(efd, op, ev->fd, &epv) < 0) {
        perror("epoll_ctl");
        exit(1);
    } else {
        printf("epoll_ctl op: %d, fd: %d, events: %d\n", op, ev->fd, ev->events);
    }
}

// event del 函数
void event_del(int efd, struct my_events *ev)
{
    struct epoll_event epv = {0};
    if (ev->status != 1) {
        return;
    }
    epv.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
}

void send_data_callback(int fd, int events, void *arg)
{
    struct my_events *ev = (struct my_events *)arg;
    int len;
    len = send(fd, ev->buffer, ev->buffer_size, 0);     // 直接写回数据，未处理
    printf("send data: %s, len = %d\n", ev->buffer, len);

    if (len > 0) {
        printf("send data: %s\n", ev->buffer);
        ev->buffer_size = 0;
        event_del(g_efd, ev);                   // 把send event从epoll中删除
        event_set(ev, fd, recv_data_callback, ev); // 把recv event加入epoll中
        event_add(g_efd, ev, EPOLLIN);  // 把recv event加入epoll中
    } else {
        printf("send data error: %s\n", strerror(errno));
        event_del(g_efd, ev);
        close(fd);
    }
}

void recv_data_callback(int fd, int events, void *arg)
{
    struct my_events *ev = (struct my_events *)arg;
    int nread;

    nread = recv(fd, ev->buffer, MAX_BUFFER_SIZE, 0);
    // 从树上把读事件删除，再把写事件加入epoll中
    event_del(g_efd, ev);

    if (nread > 0) {
        ev->buffer_size = nread;
        ev->buffer[nread] = '\0';
        printf("recv data: %s\n", ev->buffer);
        event_set(ev, fd, send_data_callback, ev);
        event_add(g_efd, ev, EPOLLOUT);
    } else if (nread == 0) {
        printf("client close\n");
        close(fd);
    } else {
        printf("recv data error: %s\n", strerror(errno));
        close(fd);
    }
}

void accept_conn_callback(int fd, int events, void *arg)
{
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen = sizeof(cliaddr);
    int i = 0;
    int connfd = accept(fd, (struct sockaddr *)&cliaddr, &cliaddrlen);
    if (connfd < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // 将新连接的文件描述符加入epoll事件集合
    do {
        for (i = 0; i < MAX_EVENTS; i++) {
            if (g_events[i].fd == 0) {
                break;
            }
        }

        if (i == MAX_EVENTS) {
            printf("too many connections\n");
            close(connfd);
            break;
        }

        // 设置connfd为非阻塞模式
        int flags = fcntl(connfd, F_GETFL, 0);
        fcntl(connfd, F_SETFL, flags | O_NONBLOCK);
        
        // 将新连接的文件描述符加入epoll事件集合
        event_set(&g_events[i], connfd, recv_data_callback, &g_events[i]);
        event_add(g_efd, &g_events[i], EPOLLIN);

    } while (0);

    printf("accept a new connection from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
}

// 创建listen socket,并将其加入epoll事件集合
void init_listen_socket(int efd, short port)
{
    int lfd;
    struct sockaddr_in servaddr;
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // port reuse
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if (bind(lfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(lfd, 20) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 设置非阻塞模式
    int flags = fcntl(lfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(lfd, F_SETFL, flags);

    // 将listen socket加入epoll事件集合, 添加到最后一个位置
    printf("listen on port %d\n", port);
    event_set(&g_events[MAX_EVENTS], lfd, accept_conn_callback, &g_events[MAX_EVENTS]);
    event_add(efd, &g_events[MAX_EVENTS], EPOLLIN);
    printf("add listen socket to epoll\n");
}


int main(int argc, char *argv[])
{
    // 创建epoll文件描述符
    g_efd = epoll_create(MAX_EVENTS + 1);
    if (g_efd == -1) {
        perror("epoll_create");
        exit(1);
    }

    // 初始化socket
    init_listen_socket(g_efd, SERV_PORT);

    struct epoll_event events[MAX_EVENTS + 1];

    while (1) {
        int nfds = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        printf("epoll_wait return %d\n", nfds);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(1);
        }
        int i;
        for (i = 0; i < nfds; i++) {
            struct epoll_event *ev = &events[i];
            struct my_events *myev = (struct my_events *)ev->data.ptr;
            if ( (ev->events & EPOLLIN) && (myev->events & EPOLLIN) ) {
                myev->call_back(myev->fd, ev->events, myev->arg);
            } else if ( (ev->events & EPOLLOUT) && (myev->events & EPOLLOUT) ) {
                myev->call_back(myev->fd, ev->events, myev->arg);
            } else {
                printf("epoll error\n");
            }
        }
    }

    // release
    close(g_efd);

    return 0;
}