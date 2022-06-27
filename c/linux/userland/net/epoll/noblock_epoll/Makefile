all: client server

client : client.o
	gcc -o client client.o
server : server.o
	gcc -o server server.o

.PHONY: clean
clean:
	rm -f client server *.o