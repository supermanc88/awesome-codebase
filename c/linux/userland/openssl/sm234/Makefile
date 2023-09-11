CC = gcc
CFLAGS = -Wall -g
LIBS = -lcrypto -lssl

all: sm4test sm3test sm2test

sm4test: sm4_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

sm3test: sm3_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

sm2test: sm2_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -f *.o sm4test sm3test sm2test