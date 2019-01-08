#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

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


	/* GNU GETOPT */
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

	try
 	{
		boost::asio::io_context; 

		//accept connections on port 20 
		tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 20) );

		printf("Listening for connections\n"); 
		while(true)
		{
			//accept connections on the socket. 
			tcp::socket socket(io_context);
			acceptor.accept(socket); 

			printf("Client Connected\n"); 
			

			//create the message to send. 
			/*
			boost::system::error_code ignored_error;
			boost::asio::buffer buffer( message )
			boost::asio::write(socket, buffer, ignored_error);
			*/
		}

	}
    catch (std::exception& e)
    {
    	std::cerr << e.what() << std::endl;
    }
	return 0; 

}
