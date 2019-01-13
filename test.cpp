#include <iostream>
#include <fstream> 

int main()
{
	std::ofstream o("./recieve/Hello.txt");
	o << "Hello, World\n" << std::endl;

	return 0; 
}
