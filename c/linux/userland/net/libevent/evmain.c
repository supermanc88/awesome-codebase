#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/util.h>

char buf[1024];

void libev_read_callback(struct bufferevent *bev, void *ctx)
{
    int len = bufferevent_read(bev, buf, sizeof(buf));
    buf[len] = '\0';
    if (len > 0) {
        printf("%s\n", buf);
    }
    char reply[1024] = "reply: ";
    strcat(reply, buf);
    bufferevent_write(bev, reply, strlen(reply));
}

void libev_write_callback(struct bufferevent *bev, void *ctx)
{
    printf("libev_write_callback\n");
    // send buf to client
    struct evbuffer *output = bufferevent_get_output(bev);
}

void libev_event_callback(struct bufferevent *bev, short what, void *ctx)
{
    if (what & BEV_EVENT_EOF) {
        printf("libev_event_callback: BEV_EVENT_EOF\n");
        bufferevent_free(bev);
    } else if (what & BEV_EVENT_ERROR) {
        printf("libev_event_callback: BEV_EVENT_ERROR\n");
        bufferevent_free(bev);
    }
}

void libev_listener_callback(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
    struct event_base *base = (struct event_base *)user_data;
    struct bufferevent *bev;

    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, libev_read_callback, NULL,
        libev_event_callback, NULL);

    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

int main(int argc, char *argv[])
{
    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(9999);

    base = event_base_new();
    // 初始化listen 并设置端口重用和关闭时释放
    listener = evconnlistener_new_bind(base, libev_listener_callback, (void *)base,
                                       LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                                       (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    event_base_dispatch(base);
    evconnlistener_free(listener);
    event_base_free(base);
    return 0;
}