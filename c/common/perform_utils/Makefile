TARGET=perform_test

CC=gcc
CFLAGS=-std=gnu99 -g -O2

SRCS=test.c
INCLUDES=-I.
LIBS=-L.
LIBS+=-lpthread

.PHONY: all clean

all: $(TARGET)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET)
