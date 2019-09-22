# Tyler's File Transfer Protocol Server 

This is a project maintained by Tyler Beverley (SupahAmbition).
Currently the program is seperated into two, a server and a client application. In the future we will combine these two to be one program.   

Status of this Project: 
Development is on hold for now, the program is able to transfer files over the network, but other File transfer protocol features such as renaming files over the network have not been implemented. My goal was to learn how files were transfered over the network, which I believe I have achieved. 


## FILES 
* ftpclient.c	->  Run this program on the machine you want to recieve the file. 
* ftpserver.c 	->  Run this program on the machine you want to host/send the files from. 
* ./tests/ 		->  The directory that holds all the files we will be testing with. 
* ./recieve/ 	->	The directory that holds files that have been sent from the server to the client for testing. 


## MAKEFILE RULES 
* make 		-> compile both programs into executables. 
* client 	-> compile just the client. 
* server 	-> compile jus the server. 
* clean 	-> removes all compiled, and object files. 


## TESTCASES 
* test1 		-> Test basic file transfer functionallity.
						Expected MD5sum -> 708da4e0f3f43e9f14180947c17a6409 





