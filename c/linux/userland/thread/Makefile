CC=gcc
CFLAGS=-Wall -Werror -g -std=gnu99
LDFLAGS=-lpthread

all: sem_test1 sem_test2

sem_test1: semaphore_test1.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

sem_test2: semaphore_test2.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f sem_test1 sem_test2