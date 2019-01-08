#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main( int argc, char* argv[] )
{

	/* GNU GETOPT  */ 
	int opt;
	while ( (opt = getopt(argc, argv, "") ) != -1)
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
