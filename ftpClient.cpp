#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


int main( int argc, char* argv[] )
{

	/* GNU GETOPT */ 
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
	
	
	try
    {
	    boost::asio::io_context io_context;

		//Resolve the address given from input. 
		tcp::resolver resolver(io_context);
	    tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "20");

		//create the socket and connect to the endpoint. 
	    tcp::socket socket(io_context);
	    boost::asio::connect(socket, endpoints);

		while(true)
		{
			
		    boost::array<char, 128> buf;
		    boost::system::error_code error;

		    size_t len = socket.read_some(boost::asio::buffer(buf), error);

		    if (error == boost::asio::error::eof)
		      break; // Connection closed cleanly by peer.
		    else if (error)
		      throw boost::system::system_error(error); // Some other error.

		  	printf("%s", buf.data());

		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl; 
	}
	return 0;

}
