CC=gcc
CFLAGS=-std=gnu99 -Wall -pedantic -g

all: clean client server

client:
	$(CC) $(CFLAGS) client.c -o client

server:
	$(CC) $(CFLAGS) server.c -o server

clean:
	rm -f client server
