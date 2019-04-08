#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "ftpClient.h" 
#define PORT "20"

void cli( int socketfd,  char** args )
{
	//printf("Command was %s\n", command); 

	char* command = args[0]; 

	// ====  Identify and execute the command.  ========
	//
	//
	// 1.  Quit
	if (command == NULL || strcmp( command, "quit" ) == 0  )
	{
	 	printf("Quiting now\n"); 
		
		//disconnect from the server
		if( socketfd != -1)
		{
			close(socketfd); 
		}
		exit(0); 
	}
	// 2. clear 
	else if( strcmp( command, "clear" ) == 0 )
	{
		//clear the screen 
		//system("cls");  <--- windows 
		system("clear");  // <--- Unix 
	}
	// 3. connect (address)
	else if( strcmp( command, "connect" ) == 0 )
	{
		char* address  = args[1]; 
		socketfd = createConnection( address, PORT);	
		if( socketfd = -1 )
		{
			printf("Was not able to connect to %s\n", address ); 
		}
	}
	// 4. pwd  
	else if( strcmp( command, "pwd"  ) == 0 )
	{
		args[0] = "PWD0"; 
		sendCommand(socketfd, 4, args); 
		
	}
	// 5. cd (new directory) 
	else if( strcmp( command, "cd" ) == 0 )
	{
		args[0] = "CWD0"; 
		sendCommand(socketfd, 5, args); 
	}
	// 6. ls
	else if( strcmp( command, "ls" ) == 0 )
	{
		args[0] = "LST0"; 
		sendCommand(socketfd, 6, args); 
	}
	
	// 7. recieve (path of file to recieve) (location to save file) 
	else if( strcmp( command, "retrieve" ) == 0 ) 
	{
		args[0] = "RETR"; 
		sendCommand(socketfd, 7, args); 
	}
	// 8. space 
	else if( strcmp( command, "space") == 0 )
	{
		args[0] = "SIZE";
		sendCommand( socketfd, 8, args); 
	}
	
	// 9. mkdir
	else if( strcmp( command, "mkdir" ) == 0 ) 
	{
		args[0] = "MKDR"; 
		sendCommand( socketfd, 9, args); 
	}

	// 10. undo 
	else if( strcmp( command, "undo" ) == 0 )
	{
		// undo the last command  ?
		args[0] = "UNDO"; 
		sendCommand( socketfd, 10, args); 
	}
	// 12. help 
	else if( strcmp( command, "help" ) == 0 )
	{
		//print the list of commands. 
		char* helpString; 

		helpString = "Usage client [Address]."  
					  "\nCOMMANDS:" 
					  "\n\t quit \t\t\t- Disconnect from the server and exit the client." 
					  "\n\t connect (address) \t- Connect to a FTP server specifed at the given addres. "
					  "\n\t pwd \t\t\t- Print the current working directory on the server. " 
					  "\n\t cd (path) \t\t- Change the directory on the server to the specified path. "
					  "\n\t ls \t\t\t- List the contents of the working directory on the server. "
					  "\n\t retrieve (file) [path]\t- Recieve a file that is hosted on the server. "
					  "\n\t\t\t\t The first argument is file you want to recieve, "
				      "\n\t\t\t\t and the second argument is an option to specify "
					  "\n\t\t\t\t where the file should be saved to. "
					  "\n\t space \t\t\t- Get the disk space available on the server. "
					  "\n\t mkdir \t\t\t- Create a directory on the server." 
					  "\n\t undo \t\t\t- Undo the last command."
					  "\n\t rename (path) (new)\t- Rename the specifed file. "
					  "\n\t help \t\t\t- displays this help message "; 

		printf( "%s\n", helpString ); 
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
	//socketfd = createConnection( address, PORT); 

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

