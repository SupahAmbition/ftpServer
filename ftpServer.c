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
#define BACKLOG 3


/* PROGRAM OVERVIEW  
 *
 * 1. Listen for a Connection 
 * 2. Read in a command from the connection. 
 * 3. Execute the command 
 * 4. Otherwise reply with an error code. 
 * 5. Repeat.
 *
 */


/* Sends the specifed file to the specified socket. */
int sendFile( int new_socket, char* filePath )
{
	FILE* inFile; 

	//open the file for reading. 
	inFile = fopen(filePath, "r"); 

	if( !inFile )
	{
		fprintf(stderr, "server.sendFile: Was not able to open file (%s) for reading\n", filePath); 
		exit(1); 
		//send back an error code? 
	}

	//get the length of the file. 
	fseek(inFile, 0, SEEK_END); 
	int	fileLength = (int) ftell(inFile);  
	rewind(inFile);


	char buff[fileLength + 1];  // +1 for null terminated. 
	fread(buff, sizeof(char), fileLength, inFile); 


	int sendResult = send( new_socket, buff, fileLength, 0); 
	if( sendResult == -1)
	{
		perror("sendFile"); 
	}

	printf("Sent %d bytes\n", sendResult); 
	fclose(inFile); 
	close(new_socket); 
	exit(0); 
}


/* This function will create a socket and bind it
 * to the specified port on the host machine. 
 * it will return a the socketfd that was successfully created  
 * successfully binded. */
int createConnection(char* port )
{

	int socketfd;
	struct addrinfo hints, *servinfo, *p; 

	memset(&hints, 0, sizeof(hints) ); 
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP
	hints.ai_flags = AI_PASSIVE; //use own address 


	int status = getaddrinfo( NULL, port,  &hints, &servinfo);

	if( status != 0)
	{
		fprintf(stderr, "server: getaddrinfo: %s\n", gai_strerror(status) ); 
		exit(1); 
	}


	//loop through the list of endpoints we recieved from getaddrinfo. 
	for( p = servinfo; p != NULL; p = p->ai_next)
	{
		//attempt to create the socket - IPv4 and TCP
		socketfd = socket(p->ai_family, p->ai_socktype,  p->ai_protocol); 

		if(socketfd == -1)
		{
			perror("createConnection");
			continue; 
		}

		//attempt to bind the socket
		if( bind(socketfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(socketfd);
			perror("Failed to bind to specifed address.");  
			continue; 
		}

		break; 
		//if both of those functions complete successfully then 
		//we successfully created the connection. 

	}

	freeaddrinfo(servinfo); 
 	if (p == NULL)  
	{
        fprintf(stderr, "server: failed to bind to port %s\n", port );
        exit(1);
    }
	else 
	{
		return socketfd; 
	}
}

int executeCommand( int socketfd, char* command, char** args)
{

	//identify the command and then call it's associated function 
	if( strcmp( command, "RETR" ) == 0)
	{
		char filePath[100]; 
		int bytesRead;
		//recieve the name of the file that the client wants us to send. 

		bytesRead = recv( socketfd, filePath, 100, 0); 

		//call send File with the socket we are using, and the file the client wants. 
		sendFile(socketfd, filePath);  
	}
	else if( strcmp( command, "PWD" ) == 0 ) 
	{
		
	}
	else if( strcmp( command, "CWD") == 0 )
	{

	}
	else if( strcmp( command, "LST") == 0 )
	{

	}
	else if( strcmp( command, "SIZE") == 0 )
	{

	}	
	else if( strcmp( command, "FSZE" ) == 0)
	{

	}	
	else if( strcmp( command, "MKD" ) == 0) 
	{

	}
	else if( strcmp( command, "RNTO") == 0)
	{
		//rename to 
	}
	else
	{
		printf("Recieved a command that was not recognized %s\n", command); 
		//command was not recognized. 
	}
}



int main( int argc, char* argv[] )
{
	FILE* file; 

	/* GNU GETOPT */
	int opt;
	while ( (opt = getopt (argc, argv, "f:") ) != -1)
	{
		printf("Option is: %c \n", opt); 
		printf("Optarg is: %s \n", optarg); 
		switch (opt)
		{

			case 'f': 
				file = fopen(optarg, "r");
				if( !file )
				{
					fprintf( stderr, "Failed to open the file %s. \n", optarg); 
					exit(1); 
				}
				break; 

			default: 
				printf("%c is not an option", opt); 
				break; 
		}
	}
	
	int socketfd, new_socket; 
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size; 

	socketfd = createConnection(PORT); 
	printf("Successfully connected\n"); 

	//Listen for connections
	int listenResult = listen( socketfd, BACKLOG ); 

	if( listenResult < 0)
	{
		perror("listen"); 
		exit(1); 
	}

	//listen for connections. 
	while( 1 )
	{
		sin_size = sizeof their_addr; 
		new_socket = accept( socketfd, (struct sockaddr*) &their_addr, &sin_size); 

		if(new_socket == -1)
		{
			perror("server.main"); 
			continue;
		}

		if( !fork() ) // For the child process. 
		{

			close(socketfd); 
		
			//client / server interaction 
			while(1) 
			{
				//recieve the command. 	
				char command[4]; 
				int bytesRead = 0; 
		
				bytesRead = recv(new_socket, command, 4, 0);
				//maybe check how many bytes are recieved. 

				if (bytesRead > 0)
				{
					executeCommand(new_socket, command); 
				}
			}

		}
		close(new_socket); 

	}
	fclose(file);

	return 0; 

}


