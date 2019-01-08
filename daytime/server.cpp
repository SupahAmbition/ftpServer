#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


std::string make_daytime_string( )
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}


int main( int argc, char* argv[] )
{

	try
	{
		boost::asio::io_context io_context;

	    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));

	    printf("Listening for connections\n");
	    for(;;)
	 	{
            tcp::socket socket(io_context);
	        acceptor.accept(socket);

	        printf("Client connected! Sending message...\n");
	        std::string message = make_daytime_string();  

	        boost::system::error_code ignored_error;
	        boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
       
		}
	    printf("TEST");

	}
    catch (std::exception& e)
    {
    	std::cerr << e.what() << std::endl;
    }
	return 0; 

}
