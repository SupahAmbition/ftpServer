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















/* Recieve a file from the server, 
 * reads in only 1024 bytes at a time */
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


/* This function will create a socket and bind it
 * to the specified address on port 20.  
 * it will return a the socketfd that was successfully created   */
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
		printf("Connected to server!\n"); 
		return socketfd;
	}

}


int main( int argc, char* argv[] )
{
	FILE* outFile; 
	char* address; 
	int socketfd; 
	
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

	//iteractive command line loop
	while( 1 ) 
	{
		char command[100]; 
		char* fgetsResult; 
		
		
		printf("<ftpClient>"); 
		fgetsResult = fgets(command, 100, stdin); 
		command[strcspn(command, "\r\n")] = 0;
		
		//printf("Command was %s\n", command); 
		
		
		/* 		identify and execute the command. 		*/
		if (strcmp( command, "help" ) == 0)
		{
			//print the list of commands. 
		}
		else if( strcmp( command, "connect" ) == 0 )
		{
			//connect to the server 
			printf("IPv4 Address to connect to: "); 
	
			char address[100]; 
			fgets(address, 100, stdin); 
			address[strlen(address) - 1 ] = 0; 

			socketfd = createConnection( address, PORT);	
		}
		else if( strcmp( command, "quit" ) == 0 )
		{
			//disconnect from the server
			if( socketfd != -1)
			{
				close(socketfd); 
			}
			exit(1); 
			

		}
		else if( strcmp( command, "pwd"  ) == 0 )
		{
			//print the current working directory. 
			sendCommand(socketfd, "PWD"); 
			
		}
		else if( strcmp( command, "cd" ) == 0 )
		{
			//change the current directory.
			sendCommand(socketfd, "CWD"); 
		}
		else if( strcmp( command, "ls" ) == 0 )
		{
			//the the contents of the current directory. 
			sendCommand(socketfd, "LST "); 
		}
		else if( strcmp( command, "recieve" ) == 0 ) 
		{
			//recieve the specified file from the server. 
			char recieveFile[100]; 	
			char filePath[100]; 

			sendCommand(socketfd, "RETR"); 
		
			printf("Path of the file to get from the server:"); 
			fgets(recieveFile, 100, stdin ); 
			recieveFile[strlen(recieveFile) - 1 ] = 0; 
			
			if( socketfd != -1)
			{
				int bytesSent;
				bytesSent = send(socketfd, recieveFile, strlen(recieveFile), 0); 

				printf("Where to save file?"); 
			
				fgets(filePath, 100, stdin); 
				filePath[strlen(filePath) - 1 ] = 0; 
	
				recvFile(socketfd, filePath); 
			}

		}
		else if( strcmp( command, "space available") == 0 )
		{
			//get the disk space available on the server. 
			
			sendCommand( socketfd, "SIZE"); 
		}
		else if( strcmp( command, "clear" ) == 0 )
		{
			//clear the screen 
			//system("cls");  <--- windows 
			system("clear");  // <--- Unix 
		}
		else if( strcmp( command, "mkdir" ) == 0 ) 
		{
			//create a directory. 
		}
		else if( strcmp( command, "undo" ) == 0 )
		{
			// undo the last command 
		}
		else if( strcmp( command, "rename" ) == 0) 
		{
			//rename the specifed file or directory 
		}
		else if( fgetsResult == NULL) // ctrl + d
		{
			printf("Quiting now\n"); 

			if( socketfd != -1)
			{
				close(socketfd); 
			}
			exit(1); 
		}

	}

	if( socketfd != -1)
	{
		close(socketfd); 
	}

	return 0;

}

