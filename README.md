# socket-programming


c. What have I done in this assignment:
	I have finished all the requirement as described in the Project requirement. 
	All five servers will up and run. 
	In Phases1:
		For the client, it will get the inputs from command line and send all of them to AWS using TCP connection. 
		For serverA and serverB, they read data from database_a.csv and database_b.csv file, respectively. 
		For the AWS, it will listen client and it also send to monitor using TCP and to serverA and serverB separately using UDP connection. 
	In Phases2:
		If the link-id exist, aws will send the data to serverC using UDP.
		serverC calculate the results needed and sent them back to aws
	In Phases3:
		the AWS will send the end2end time back to client and sent and all details to monitor. 

d. what each file does:
	client.c: it will get the inputs and send all of them to AWS using TCP connection. It also receive the result from aws.
	aws.c:    received the data from client using TCP, sent to and receive from serverA and serverB using UDP. If link id exists, it send data to serverC and receive from C using UDP. 
				send input data and calculation results to monitor using TCP
	serverA.c: After it get data from AWS using UDP, look up the table it has.
	serverB.c: After it get data from AWS using UDP, look up the table it has.
	serverC.c: After it get data from AWS using UDP, it will make the calculation and then send back the results to aws.

e. how to run the programs:
	1. Frist you should open six different terminal. And using one of them to type"make all" to compile all the file. 
	2. typing "make serverA" using one terminal to run serverA.
	3. typing "make serverB" using one terminal to run serverB.
	4. typing "make serverC" using one terminal to run serverc.
	5. typing "make aws" using one terminal to run AWS. 
	6. typing "./client <LINK_ID> <SIZE> <POWER> " to run the client.
	7. typing "make monitor" to run the monitor.

f. An example of the format of all the messages exchanged (command line for client: ./client 88 50000 -30):
   1. client:
	The client is up and running. 
	The client has sent the  ID= 88, size=50000, and power=-30 to AWS.
	The delay for link 88 is 66.10 ms

   2. aws:
	The AWS is up and running. 
	The AWS received  link ID=88, size=50000, and power=-30 from the client using TCP over port 1171 
	The AWS sent link ID=88, size=50000, and power=-30 to the monitor using TCP over port 50357 
	The AWS sent link ID=88 to Backend-Server A using UDP over port 21616. 
	The AWS received 1 matches from Backend-Server A using UDP over port 21616. 
	The AWS sent link ID=88 to Backend-Server B using UDP over port 22616. 
	The AWS received 0 matches from Backend-Server B using UDP over port 22616. 
	The AWS sent link ID=88, size=50000, power=-30, and link information to Backend-Server C using UDP over port 23616 
	The AWS received outputs from Backend-Server C using UDP over port 23616. 
	The AWS sent delay=66.10 ms to the client using TCP over port 1171 
	The AWS sent detailed results to the monitor using TCP over port 50357.

   3. serverA:
	The Server A is up and running using UDP on port 21616.
	The Server A received input 88. 
	The server A has found 1 match. 
	The Server A finished sending the output to AWS.

   4. serverB:
	The Server B is up and running using UDP on port 22616.
	The Server B received input 88. 
	The server B has found 0 match. 
	The Server B finished sending the output to AWS. 

   5. serverC:
	The Server C is up and running using UDP on port 23616.
	The Server C received link information of link 88, file size 50000, and signal power -30 
	The server C finished the calculation for link 88. 
	The Server C finished sending the output to AWS.

   6. monitor:
	The monitor is up and running. 
	The monitor received link ID=88, size=50000, and power=-30 from the AWS 
	The result for link 88: Tt = 0.00 ms, Tp = 66.10 ms, Delay = 66.10 ms 

g. the idiosyncrasy:
	To run the project, must obey the order described in "how to run" part. 

h. reuse of the code:
	Some code block for setting TCP and UDP are from "Beej's Guide to Network Programming -- Using Internet Sockets". They are marked in the program. 
