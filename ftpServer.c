#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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


int main( int argc, char* argv[] )
{


	/* GNU GETOPT  */ 
	int opt;
	while ( (opt = getopt (argc, argv, "") ) != -1)
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
	return 0; 

}
