# Tyler's File Transfer Protocol Server 

This is a project maintained by Tyler Beverley (SupahAmbition) and Tyler Reiland (tylerreiland).   
Currently the program is seperated into two, a server and a client application. In the future we will combine these two to be one program.   


## FILES 
* ftpclient.c	->  Run this program on the machine you want to recieve the file. 
* ftpserver.c 	->  Run this program on the machine you want to host/send the files from. 
* ./tests/ 		->  The directory that holds all the files we will be testing with. 


## MAKEFILE RULES 
* make 		-> compile both programs into executables. 
* client 	-> compile just the client. 
* server 	-> compile jus the server. 
* clean 	-> removes all compiled, and object files. 


## TESTCASES 
* test1 		-> What it tests/ and associated expected file hash to ensure file is intact. 

