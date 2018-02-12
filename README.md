# Computer-Networks
Assignments
Assignment 1: 
PacketUtil.h and PacketUtil.c is needed to compile the program.
PacketUtil.h and PacketUtil.c are header files which give detail about how various packet are formed. 
Datapacket, Ackpacket and Rejectpacket are made as per asked by the assignment.

To run program compile server first then compile client. 

1) gcc server1.c PacketUtil.c -o server1.c
2)./server1
3) gcc client1.c PacketUtil.c -o client1.c
4)./client1

5 correct packets are send from client to server, packets are defined in client.c file.
The server.c checks the packets and send the acknowledgment.
  
For Error Handling, user has given a menu option from where he can choose which error he wanted to check.
The client will send corrupted packet according to each error.
The server will send reject packet to client accordingly.


Assignment 2:

The Verification_Database is present in the text document which has the Subscriber Number, Technology and Paid fields.
The input user should provide subscriber number without hypen.
The Identification request packet is verified against the data present in the text file and the acknowledgement is returned by the server.
To run the program, first compile server and then client programs.
