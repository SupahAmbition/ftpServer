#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 

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

FILE* file; 


int main( int argc, char* argv[] )
{


	/* GNU GETOPT */
	int opt;
	while ( (opt = getopt (argc, argv, "f:") ) != -1)
	{
		printf("Option is: %c \n", opt); 
		printf("Optarg is: %s \n", optarg); 
		switch (opt)
		{
			case 'f': 
				file = fopen(optarg, "rb");
				break; 

			default: 
				printf("%c is not an option", opt); 
				break; 
		}
	}
	
	int socketfd, new_socket, valueRead; 
	struct sockaddr_in address; 
	int addrlen = sizeof(address); 
	char buffer[1024]; 


	//create the socket - IPv4 and TCP
	socketfd = socket(AF_INET, SOCK_STREAM,  0); 

	if(socketfd == 0)
	{
		perror("Failed to create the socket.");
		exit(1); 
	}

	//fill in the address struct. 
	address.sin_family = AF_INET;
	address.sin_port = htons(20); 
	inet_pton(AF_INET, "10.0.0.1", &address.sin_addr); 


	//bind the socket to an address
	int bindResult = bind(socketfd, (struct sockaddr*) &address, (socklen_t)addrlen );   
	
	if(bindResult > 0)
	{
		perror("Failed to bind to specified address."); 
		exit(1); 
	}


	//Listen for connections
	int listenResult = listen( socketfd, 3 ); 

	if( listenResult < 0)
	{
		perror("Failed to listen for a new connetion"); 
		exit(1); 
	}

	//accept connections 
	new_socket = accept( socketfd, (struct sockaddr*) &address, (socklen_t*)&addrlen ); 

	if( new_socket < 0 )
	{
		perror("Failed to accept connection.");
		exit(1); 
	}
	else
	{
		printf("Client Connected.\n"); 
	}


	return 0; 

}
