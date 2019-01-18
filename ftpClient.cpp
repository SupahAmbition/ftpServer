#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <string.h>

FILE* outFile; 


int main( int argc, char* argv[] )
{

	/* GNU GETOPT */ 
	int opt;
	while ( (opt = getopt(argc, argv, "") ) != -1)
	{
		printf("Option is: %c \n", opt); 
		printf("Optarg is: %s \n", optarg); 
		switch (opt)
		{

		 	default: 
				 printf("%c is not an option", opt); 
				 break; 
		}
	}
	
	int socketfd, valueRead;
	struct sockaddr_in address; 
	int addrlen = sizeof(address); 


	socketfd = socket(AF_INET, SOCK_STREAM, 0); 

	if(socketfd == 0)
	{
		perror("Failed to create the socket\n"); 
		exit(1); 
	}


	//fill out the address struct. 
	address.sin_family = AF_INET; 
	address.sin_port = htons(20); 
	inet_pton(AF_INET, argv[1], &address); 

	//connect to the server
	int connectResult = connect(socketfd, (struct sockaddr*) &address, (socklen_t)addrlen);

	if( connectResult < 0)
	{
		perror("Falled to connect to specified address\n");
		exit(1); 
	}
	else 
	{
		printf("Connected to server!\n"); 
		char buff[1024]; 

		outFile = fopen("./recieve/test1.txt", "w");

		int bytesRead = 1; 
		while(bytesRead > 0)
		{
			//read from the server into a buffer. 
			bytesRead = read(socketfd, buff, 1024); 
			fwrite(buff, sizeof(char), 1024, outFile); 
		
			printf("writing: %s\n", buff); 
			memset(buff, '0', 1024);

		}

		fclose(outFile); 


	}

	close(socketfd); 
	


	return 0;

}










