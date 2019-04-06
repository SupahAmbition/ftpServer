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

#define PORT "20"



int sendCommand(int socketfd, char* command)
{
	if(socketfd == -1)
	{
		fprintf(stderr, "Was not able to send command because socket was invalid\n"); 
		exit(1);
	}

	int length = strlen(command); 
	int sendResult = send( socketfd, command, length, 0); 

	if( sendResult == -1)
	{
		perror("sendCommand"); 
	}
	//printf("Send command %s\n", command); 
}	


// Recieve a file from the server, 
// reads in only 1024 bytes at a time 
int recvFile( int socketfd, char* filePath )
{
	FILE* outFile; 
	char buff[1024]; 
	int bytesRead = 1; 
	
	
	//open the file for writing. 
	outFile = fopen(filePath, "w"); 

	if( !outFile )
	{
		char defaultPath[] = "./recieve/default.txt";  

		printf("client.sendFile: Was not able to open file (%s) for writing\n", filePath);
		printf("Opening %s for writing instead", defaultPath );  

		outFile = fopen(defaultPath, "w"); 
	}

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


/* 
 * This function will create a socket and bind it
 * to the specified address on port 20.  
 * it will return a the socketfd that was successfully created   
 */
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
		return -1; 
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
		printf("Connected to server!\n"); 
		return socketfd;
	}

}

