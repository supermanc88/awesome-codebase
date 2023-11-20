TARGET=parse_crt

CC=gcc
CFLAGS=-g 

SRCS=parse_crt.c

INC_DIR=/mnt/e/Linux/openssl/openssl-3.1.1/include

LIB_DIR=/mnt/e/Linux/openssl/openssl-3.1.1/
LIBS=-lssl -lcrypto

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -I$(INC_DIR) -o $@ $^ -L$(LIB_DIR) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)