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

#define PORT 20 
#define BACKLOG 3


/* PROGRAM OVERVIEW  
 *
 * 1. Listen for a Connection 
 * 2. Read in a command from the connection. 
 * 3. Execute the command 
 * 		a. PWD = send the name of the working directory back to the connection 
 * 		b. CD  = change the the working directory. reply with name of new directory (?)
 * 		c. LS  = reply with a list of the contents of the working directory. 
 * 
 * 4. Otherwise reply with an error code. 
 * 5. Repeat.
 *
 */


/* Sends the specifed file to the specified socket. */
int sendFile( int new_socket, FILE* inFile)
{
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
	close(new_socket); 
	exit(0); 

	free(buff); 
}


/* This function will create a socket and bind it
 * to the specified port on the host machine. 
 * it will return a pointer to the socket that was 
 * successfully binded. */
int createConnection(int portNum)
{

	int socketfd;
	struct addrinfo hints, *servinfo, *p; 
	char port[10]; 

	memset(&hints, 0, sizeof(hints) ); 
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP
	hints.ai_flags = AI_PASSIVE; //use own address 


	sprintf( port, "%d", portNum ); 
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
        fprintf(stderr, "server: failed to bind to port %d\n", portNum);
        exit(1);
    }
	else 
	{
		return socketfd; 
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

	while( 1 )
	{
		sin_size = sizeof their_addr; 
		new_socket = accept( socketfd, (struct sockaddr*) &their_addr, &sin_size); 

		if(new_socket == -1)
		{
			perror("server: was not able to accept connetion.\n"); 
			continue;
		}

		if( !fork() ) // For the child process. 
		{
			sendFile(new_socket, file);
			close(socketfd); 
		}
		close(new_socket); 

	}
	fclose(file);

	return 0; 

}


