TARGET=strcmp_opt

CC=gcc
CFLAGS=-g -O2

# 支持avx指令集
CFLAGS+=-mavx -mavx2

SRCS=strcmp_opt.c
INCLUDES=-I.
LIBS=-L.

.PHONY: all clean

all: $(TARGET)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET)
