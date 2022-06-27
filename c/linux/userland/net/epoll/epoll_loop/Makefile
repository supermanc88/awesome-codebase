all: epoll_loop.o
	gcc -o epoll_loop epoll_loop.o

CFLAGS = -g
epoll_loop.o : epoll_loop.c
	$(CC) $(CFLAGS) -c epoll_loop.c

.PHONY: clean
clean:
	rm -f epoll_loop
	rm -f epoll_loop.o