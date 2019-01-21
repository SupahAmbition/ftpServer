#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "20"



/* Recieve a file from the server, 
 * read in only 1024 bytes at a time */
int recieveFile( int socketfd, FILE* outFile )
{
	if(outFile == NULL)
	{
		outFile = fopen("./recieve/test1.txt", "w"); 
	}


	char buff[1024]; 
	int bytesRead = 1; 
	while(bytesRead > 0)
	{
		//read from the server into a buffer. 
		bytesRead = recv(socketfd, buff, 1024, 0); 
		fwrite(buff, sizeof(char), bytesRead, outFile); 

		printf("writing: %d bytes\n", bytesRead); 
		memset(buff, '0', 1024);

	}

	fclose(outFile); 
	return 0; 

}


/* This function will create a socket and bind it
 * to the specified port on the host machine. 
 * it will return a the socketfd that was successfully created  
 * successfully binded. */
int createConnection( char* address,  char* port )
{

	int socketfd; 
	struct addrinfo hints,  *servinfo, *p;

	memset(&hints, 0, sizeof(hints) ); 
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP
	int status = getaddrinfo( address, port, &hints, &servinfo);

	if(status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status) ); 
		exit(1); 
	}


	for( p = servinfo; p != NULL; p = p->ai_next)
	{
		socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if( socketfd == -1) 
		{
			perror("client: socket");
			continue;
		}


		//conect to the server 	
		int connectionResult = connect(socketfd, p->ai_addr, p->ai_addrlen);
		if ( connectionResult == -1) 
		{
			close(socketfd);
			perror("client: connect");
			continue;
		}

		// if the socket passes those two tests then
		// we successfully created the socket. 
		break; 
	}

	freeaddrinfo(servinfo); 
	if(p == NULL)
	{
		fprintf(stderr, "Failed to connect to the server\n"); 
		exit(1); 
	}
	else 
	{
		return socketfd;
	}

}


int main( int argc, char* argv[] )
{
	FILE* outFile; 
	char* address; 

	address = argv[1]; 

	/* GNU GETOPT USEAGE  */ 
	/* 	./client address [flags] */ 
	int opt;
	while ( (opt = getopt(argc, argv, "o:") ) != -1)
	{
		printf("Option is: %c \n", opt); 
		printf("Optarg is: %s \n", optarg); 
		printf("argv[1] is: %s \n", argv[1]); 
		switch (opt)
		{
			case 'o': 

				outFile = fopen(optarg, "w"); 

				if( !outFile )
				{
					fprintf( stderr, "Failed to create the file %s. \n", optarg); 
					exit(1); 
				}
				break; 

		 	default: 
				 printf("%c is not an option", opt); 
				 break; 
		}
	}

	int socketfd; 
	
	socketfd = createConnection( address, PORT);	
	printf("Connected to server!\n"); 

	recieveFile(socketfd, outFile); 

	close(socketfd); 

	return 0;

}

