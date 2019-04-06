CC=gcc
CPP=g++
CFLAGS= -pipe -g

all: client server


client: cli_client.o ftpClient.o
	$(CC) $(CLFAGS) ftpClient.o cli_client.o -o client -lpthread  -g 

server: ftpServer.o
	$(CC) $(CLFAGS) ftpServer.o -o server -lpthread -g

clean: 
	rm -rf *.o
	rm -rf server
	rm -rf client
	rm -rf ./recieve/test1.txt
