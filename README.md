# client-server
TCP Client-Server socket C program..

# Description
	Sample client-server program. The server is a socket server listening on port 54321, it serves
as a directory listing server which will list current directory content and send back to client. The
client use socket to connect to the server, it supports 4 commands below. User will enter command
in client and display result sent back from the server.
	• pwd: display server current directory under the session
	• ls: list files under current server directory. The display format will be:
		o <type> <filename> <date>
	• type: file or dir
	• filename: name of the file/dir
	• File creation date
	• cd <directory> : enter the directory
	• bye: close the connection session

# Compilation 
	make

# Run
For Server side:	./server

For Client side:	./client

# Input
pwd:	To print working directory.
ls:	To list dirctory structure.
cd:	To change directory.
bye:	Exit.

# Sample Output:

 Enter your choice: pwd
 From server: /home/harish 
 Enter your choice: cd
 Enter Directory path: /home/harish/Documents
 From server: Directory changed successfully 
 Enter your choice: pwd
 From server: /home/harish/Documents 
 Enter your choice: ls
 From server: ls
 Directory	..	Fri Feb 18 20:07:25 2022
 Directory	Drive	Thu Feb 17 16:21:11 2022
 Directory	.	Thu Feb 17 16:21:11 2022
 Regular File	Computer Networking A Top Down Approach.pdfThu Jan 13 12:33:54 2022

 Enter your choice: bye

