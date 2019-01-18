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
				file = fopen(optarg, "r");
				break; 

			default: 
				printf("%c is not an option", opt); 
				break; 
		}
	}
	
	int socketfd, new_socket, valueRead; 
	struct addrinfo hints, *servinfo, *p; 
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size; 

	memset(&hints, 0, sizeof(hints) ); 
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo( NULL, "20", &hints, &servinfo);

	if( status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status) ); 
		exit(1); 
	}


	//loop through the results of getaddrinfo
	for( p = servinfo; p != NULL; p = p->ai_next)
	{
		//create the socket - IPv4 and TCP
		socketfd = socket(p->ai_family, p->ai_socktype,  p->ai_protocol); 

		if(socketfd == -1)
		{
			perror("Failed to create the socket.");
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

	}

	freeaddrinfo(servinfo); 
 	if (p == NULL)  
	{
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

	//Listen for connections
	int listenResult = listen( socketfd, 3 ); 

	if( listenResult < 0)
	{
		perror("Failed to listen for a new connetion"); 
		exit(1); 
	}


	while(true)
	{
		sin_size = sizeof their_addr; 
		new_socket = accept( socketfd, (struct sockaddr*) &their_addr, &sin_size); 

		if(new_socket == -1)
		{
			perror("accept"); 
			continue;
		}

		if( !fork() ) // For the child process. 
		{
			close(socketfd); 
	
			fseek(file, 0, SEEK_END); 
			int	fileLength = (int) ftell(file);  
			rewind(file);


			char buff[fileLength + 1];  // +1 for null terminated. 
			fread(buff, sizeof(char), fileLength, file); 

			fclose(file); 

			int sendResult = send( new_socket, buff, fileLength, 0); 
			if( sendResult == -1)
			{
				perror("Send"); 
			}

			printf("Sent %d bytes\n", sendResult); 
			close(new_socket); 
			exit(0); 
		}

		close(new_socket); 

	}

	return 0; 

}












