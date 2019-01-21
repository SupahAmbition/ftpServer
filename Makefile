CC=gcc
CPP=g++
CFLAGS= -pipe -g

all: client server


client: ftpClient.o
	$(CC) $(CLFAGS) ftpClient.o -o client -lpthread

server: ftpServer.o
	$(CC) $(CLFAGS) ftpServer.o -o server -lpthread


clean: 
	rm -rf *.o
	rm -rf server
	rm -rf client
	rm -rf ./recieve/test1.txt
