
all: server.o client.o

client.o:
	gcc -o client client.c process.c crypt.c -g -lcrypto -lssl -lpthread -Wall

server.o:
	gcc -o server server.c process.c crypt.c -g -lcrypto -lssl -lpthread -Wall


clean: 
	rm client server

.PHONY: all
