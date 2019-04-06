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

void cli( int socketfd,  char** args )
{
	//printf("Command was %s\n", command); 

	char* command = args[0]; 


	// Identify and execute the command. 
	if (command == NULL || strcmp( command, "quit" ) == 0  )
	{

	 	printf("Quiting now\n"); 
		
		//disconnect from the server
		if( socketfd != -1)
		{
			close(socketfd); 
		}
		exit(1); 
	}

	// connect (address)
	else if( strcmp( command, "connect" ) == 0 )
	{
		//connect to the server 
		
		char* address  = args[1]; 
		socketfd = createConnection( address, PORT);	
	}
	// quit or ctrl+d
	else if( strcmp( command, "help" ) == 0 )
	{
		//print the list of commands. 
	}
	// pwd 
	else if( strcmp( command, "pwd"  ) == 0 )
	{
		//print the current working directory. 
		sendCommand(socketfd, "PWD"); 
		
	}
	// cd (new directory) 
	else if( strcmp( command, "cd" ) == 0 )
	{
		//change the current directory.
		sendCommand(socketfd, "CWD"); 
	}
	// ls
	else if( strcmp( command, "ls" ) == 0 )
	{
		//the the contents of the current directory. 
		sendCommand(socketfd, "LST "); 
	}
	
	// recieve (path of file to recieve) (location to save file) 
	else if( strcmp( command, "recieve" ) == 0 ) 
	{
		//recieve the specified file from the server. 
		char* recieveFile = args[1]; 	
		char* filePath = args[2]; 

		sendCommand(socketfd, "RETR"); 
		
		if( socketfd != -1)
		{
			int bytesSent;
			bytesSent = send(socketfd, recieveFile, strlen(recieveFile), 0); 
			
			recvFile(socketfd, filePath); 
		}

		free( recieveFile ); 
		free( filePath ); 
	}
	// space 
	else if( strcmp( command, "space") == 0 )
	{
		//get the disk space available on the server. 
		
		sendCommand( socketfd, "SIZE"); 
	}
	// clear 
	else if( strcmp( command, "clear" ) == 0 )
	{
		//clear the screen 
		//system("cls");  <--- windows 
		system("clear");  // <--- Unix 
	}
	
	//mkdir
	else if( strcmp( command, "mkdir" ) == 0 ) 
	{
		//create a directory. 
	}

	//undo 
	else if( strcmp( command, "undo" ) == 0 )
	{
		// undo the last command  ?
	}

	//rename
	else if( strcmp( command, "rename" ) == 0) 
	{
		//rename the specifed file or directory 
	}

}

#define CLI_BUFFSIZE 1024 
char* readline()
{
	int buffsize = CLI_BUFFSIZE;  
	int position = 0; 
	char* buffer = malloc( sizeof(char) * buffsize ); 
	int c; 


	if (!buffer) 
	{
    	fprintf(stderr, "ftpclient_cli: allocation error\n");
    	exit(1);
	}
	
	while(1) 
	{
		c = getchar(); 
		if( c == EOF || c == '\n' )
		{
			buffer[position] = '\0';
			return buffer;
		}
		else{ buffer[position] = c;  }

		position++; 

		//check if we need to reallocate 
		if( position >= buffsize ) 
		{
			buffsize = buffsize + CLI_BUFFSIZE; 
			buffer = realloc(buffer, buffsize); 

			if( !buffer )
			{
				fprintf(stderr, "ftpclient_cli: allocation error\n");
				exit(1);
			}
		}
	}
}


#define CLI_TOK_BUFFSIZE 64
#define CLI_TOK_DELIM " \t\r\n\a"
char** splitline(char* line)
{
	int buffsize = CLI_BUFFSIZE; 
	int position = 0; 
	char** tokens = malloc( buffsize * sizeof(char*) );
	char* token; 

	token = strtok( line, CLI_TOK_DELIM ); 
	while( token != NULL) 
	{
		tokens[position] = token; 
		position++; 

		if( position >= buffsize )
		{
			buffsize = buffsize + CLI_TOK_BUFFSIZE; 
			tokens = realloc( tokens, buffsize * sizeof(char*) ); 
			if( !tokens )
			{
				fprintf(stderr, "ftpclient_cli: allocation error\n");
				exit(1);
			}
		}
		token = strtok(NULL, CLI_TOK_DELIM); 
	}
	tokens[position] = NULL; 
	return tokens; 
}	



int main( int argc, char* argv[] )
{
	FILE* outFile; 
	char* address; 
	int socketfd; 

	address = argv[1]; 
	socketfd = createConnection( address, PORT); 

	//iteractive command line loop
	while(1)
	{
		char* line; 
		char** args; 
		
		printf("<ftpClient>"); 
		line = readline(); 
		args = splitline(line); 

		//execute the given command. 
		cli(socketfd, args); 

		free(line); 
		free(args); 
	} 
	return 0;
}

