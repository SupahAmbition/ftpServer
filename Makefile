CC=gcc
CPP=g++
CFLAGS= -std=c++11

all: client server


client: ftpClient.o
	$(CPP) $(CLFAGS) ftpClient.o -o client

server: ftpServer.o
	$(CPP) $(CLFAGS) ftpServer.o -o server


clean: 
	rm -rf *.o
	rm -rf server
	rm -rf client
