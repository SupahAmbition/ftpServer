CC=gcc
CPP=g++
CFLAGS= -std=c++11 

all: client server


client: client.o
	$(CPP) $(CLFAGS) client.o -o client -lpthread

server: server.o
	$(CPP) $(CLFAGS) server.o -o server -lpthread


clean: 
	rm -rf *.o
	rm -rf server
	rm -rf client
