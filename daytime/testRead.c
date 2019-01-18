#include <stdio.h>


FILE* file; 

int main( int argc, char* argv[] )
{
	file = fopen(argv[1], "r" ); 

	fseek(file, 0, SEEK_END); 
	int	fileLength = (int) ftell(file);  
	rewind(file);


	int bytesRead; 
	char buff[fileLength ]; 

	bytesRead = fread(buff, sizeof(char), fileLength, file); 
	fileLength = fileLength - bytesRead; 

	printf("%s\n", buff); 


	fclose(file); 
	
}
