#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


int main( int argc, char* argv[] )
{

	/* GNU GETOPT
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
	}*/
	try
    {
    	if (argc != 2)
	    {
	      	std::cerr << "Incorrect Usage, Use: ./client <host>" << std::endl;
	      	return 1;
	    }

	    boost::asio::io_context io_context;

	    tcp::resolver resolver(io_context);
	    tcp::resolver::results_type endpoints =
	      resolver.resolve(argv[1], "daytime");

	    tcp::socket socket(io_context);
	    boost::asio::connect(socket, endpoints);

	    for (;;)
	    {
		    boost::array<char, 128> buf;
		    boost::system::error_code error;

		    size_t len = socket.read_some(boost::asio::buffer(buf), error);

		    if (error == boost::asio::error::eof)
		      break; // Connection closed cleanly by peer.
		    else if (error)
		      throw boost::system::system_error(error); // Some other error.

		  	printf("NEW MESSAGE FROM SERVER: %s", buf.data());
		    //std::cout.write(buf.data(), len);
	    }
	}
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
	return 0;

}
