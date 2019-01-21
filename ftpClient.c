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
	
	int socketfd, new_socket, valueRead; 
	struct addrinfo hints,  *servinfo, *p;

	memset(&hints, 0, sizeof(hints) ); 
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP

	int status = getaddrinfo( argv[1], "20", &hints, &servinfo);

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
		break; 
	}


	if(p == NULL)
	{
		fprintf(stderr, "Failed to connect to the server\n"); 
		exit(1); 
	}
	else
	{

		printf("Connected to server!\n"); 
		freeaddrinfo(servinfo); 
		char buff[1024]; 

		outFile = fopen("./recieve/test1.txt", "w");

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
	}

	close(socketfd); 

	return 0;

}










