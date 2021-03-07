
all: client.o server.o

client.o:
	gcc -o client client.c process.c -g -lpthread -Wall

server.o:
	gcc -o server server.c myprotocol.c -g -Wall


clean: 
	rm client server

.PHONY: all
