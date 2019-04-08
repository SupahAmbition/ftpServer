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


/*
 *
 * Sends a header block, followed by the accomponing 
 * Data blocks.
 *
 * Returns -1 on failue. 
 *
 */
int sendCommand(int socketfd, int command_code , char** args)
{

	uint32_t header; 
	char* data = args[0];
	
	if(socketfd == -1)
	{
		fprintf(stderr, "Was not able to send command because socket was invalid\n"); 
		return -1;
	}
	if( args[0] == NULL )
	{
		fprintf(stderr, "client.sendCommand(): command string missing.\n"); 
		return -1; 
	}

	switch(command_code)
	{
		// pwd
		case 4: 
			header = packHeader( 3, 4 ); 
			break;
		
		// cd (path) 
		case 5: 
			strcat( data, args[1] ); 
			header = packHeader( 3, strlen(data) );  
			break; 
		
		// ls
		case 6: 
			header = packHeader(3, 4); 
			break;
		
		// retr (file) [path]
		case 7: 
			strcat( data, args[1] ); 
			
			if( args[2] != NULL )
			{
				strcat(data, args[2] ); 
			}
			
			header = packHeader( 3, strlen( data ) ); 
			break; 
	
		// space
		case 8:
			header = packHeader(3, 4); 
			break; 
	
		// mkdir (path) 
		case 9: 
			strcat( data, args[1] ); 
			packHeader(3, strlen( data ) ); 
			break;
		
		// undo
		case 10: 
			header = packHeader(3, 4); 
			break;
		
		// help 
		case 12: 
			header = packHeader(3, 4); 
			break;
		
		default: 
			fprintf(stderr, "Did not recognize command %s.\n", args[0]); 
			return -1; 
	}
			

	//send header
	int headerResult = send( socketfd, header, sizeof(header), 0 ); 
	if( headerResult > 0 )
	{
		int bytesSent = send( socketfd, data, strlen(data) * sizeof(char), 0); 
	}

	//read a response. 
	if( bytesSent > 0 )
	{
		uint32_t status; 
		int bytesRead = recv( socketfd, status, sizeof(status), 0 ); 
	}

	if( bytesSent == -1 || bytesRead == -1 )
	{
		perror("sendCommand"); 
	}
	
	free(data);

	return status;
}	


/*
 * Packs the header block for an exchange between server and client. 
 * Can account for max 2**24 data bytes at a time. 
 *
 * packHeader(3, 4) - creates the header for 
 * 	a command data block with no arguments. 
 *
 * Descriptor Codes: 
 * 0000 = Null
 * 0001 = Status Request 
 * 0010 = Status Response
 * 0011 = Command Data 
 * 0100 = File Data
 */
uint32_t packHeader( unsigned short descriptor, int dataLength )
{
	if( dataLength > (1 << 24)  )
	{
		fprintf(stderr, "Header Creation Error: Send only < 2**24 bytes at time.\n"); 
		return -1; 
	}

	uint32_t result = descriptor; 

	//move descriptor to the 8 most significant bits. 
	result = result << 24; 
	result = result | dataLength; 

	return htonl(result); 
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
		char* defaultPath = "./recieve/default.txt";  

		printf("client.recvFile: Was not able to open file (%s). \n", filePath);
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
	free(buff); 
	return 0; 
}

// 1 for valid IP. 
int validateIpAddr( char* ipAddr )
{
	struct sockaddr_in sa; 
	int result = inet_pton( AF_INET, ipAddr, &(sa.sin_addr) ); 
	return result;
}


/* 
 * This function will create a socket and bind it
 * to the specified address on port 20.  
 * it will return a the socketfd that was successfully created   
 */
int createConnection( char* address,  char* port )
{

	if( validateIpAddr( address ) < 1 )
	{
		fprintf(stderr, "Address: %s is not a correctly formated IP address\n", address ); 
		return -1; 
	}

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

