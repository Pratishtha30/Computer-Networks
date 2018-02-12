/*
Programming Assignment 1 Client Model
Objective:  Client Using customized protocol on top of UDP protocol for sending information to server.
Author: Pratishtha Deep
*/


#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include "PacketUtil.h"

#define SERVER "127.0.0.1"
#define PORT 8888   //The port on which to send data

// 
void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
 struct sockaddr_in si_other;
 
 int s, i, slen=sizeof(si_other);
 unsigned char serialized[264];

 //create a UDP Socket
 if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
 memset((char *) &si_other, 0, sizeof(si_other));

 si_other.sin_family = AF_INET;
 si_other.sin_port = htons(PORT);
     
 if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
   {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
   }

struct timeval read_timeout={3,0};
setsockopt(s,SOL_SOCKET,SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));

int segment_no=1; 
//for loop to send 5 correct packets
for(segment_no=1; segment_no<=5;segment_no++)
  {

  struct DataPacket datapkt;

  MakeDataPacket(1, segment_no, 19, "data client message", &datapkt);//Making a data packet containg message in payload
  PrintDataPacket(datapkt);//printing data packet contents in decimal and hex mode
  
  memset(serialized,'\0', 264);  
  SerializeDataPacket(datapkt, serialized);//serializing all tha data and putting into serialized array
  
//sending data to server
int ResultStatus = sendto(s, serialized, 264, 0 , (struct sockaddr *) &si_other, slen); 

memset(serialized,'\0', 264);//

//Count to send packet 3 times again in case server does not respond
int count=3;

while(count!=0)
  {
   //Checking received data status. If result status is 0 or -1 then retransmit again
 
   ResultStatus=recvfrom(s, serialized, 264, 0, (struct sockaddr *) &si_other, &slen);
   //printf("Attempting to receive: count=%d\n", count);
   if(ResultStatus==0||ResultStatus==-1)
     { 
      printf("Retransmitting Attempt Number: %d\n",4-count);
      ResultStatus=sendto(s, serialized, 264, 0 , (struct sockaddr *) &si_other, slen);
      count=count-1;
     }else{
          break;
          }
   }

//Packet is sent and processed by server print the success message and also print Acknowledgement
if(count!=0 && ResultStatus > 0)
  {
   printf("Success in sending message \n");
   printf(" Expecting ACK\n");

  //using ackpkt to check the acknowledgement recieved from serve. Using ParseSerializedAckPacket to parse each element of serialized array and put ack in ackpkt
  struct AckPacket ackpkt;
  ParseSerializedAckPacket(serialized, &ackpkt);
  PrintAckPacket(ackpkt);

  }else{
        printf("Server doesn't respond \n");
        break;
       }

memset(serialized,'\0', 264);

printf("-------------------------------------------------------------------------------------------------------------------------------\n");

}// for loop end

//In following code, Error handling is done. 4 cases have been made separately. 
int choice=0;

// Here segment_no=6 will come
while(choice!=6)
{
 printf("Please enter choice for Error Handling\n 1:Duplicate packet\n 2:Out of sequence\n 3:Length Mismatch\n 4:End packet missing\n 5:Quit\n");
 scanf("%d",&choice);

struct DataPacket datapkt;

switch(choice){
/*
Case 1: Duplicate packet. MakeDataPAcket will make a duplicate packet which will send previous packet. 
This packet will further serialized by SerializeDataPacket and then send to serialized array.
*/
case 1: 
   printf("Duplicate packet\n\n"); 
   MakeDataPacket(1, segment_no -1 /*Previous*/, 19, "data client message", &datapkt);
   PrintDataPacket(datapkt);
   memset(serialized,'\0', 264);
   SerializeDataPacket(datapkt, serialized);
   break;    
/*
Case 2: Out of Sequence. MakeDataPAcket will make a outofsequence packet that is it will send a wrong sequence of packet.
This packet will further serialized by SerializeDataPacket and then send to serialized array.
*/
case 2: 
   printf("Out of sequence\n\n");
   MakeDataPacket(1, segment_no+2/*wrong sequence is set*/, 19, "data client message", &datapkt);
   PrintDataPacket(datapkt);
   memset(serialized,'\0', 264);
   SerializeDataPacket(datapkt, serialized);
   break;
/*
Case 3: Length Mismatch. MakeDataPAcket will make a length mismatch packet that is length of the packet is set wrong in this case.
This packet will further serialized by SerializeDataPacket and then send to serialized array.
*/
case 3:
   printf ("Length Mismatch\n\n");
   MakeDataPacket(1, segment_no, 20 /* 19 actually */, "data client message", &datapkt);
   PrintDataPacket(datapkt);
   memset(serialized,'\0', 264);
   SerializeDataPacket(datapkt, serialized);
   break;
/*
Case 4: End of Packet Missing. MakeDataPAcket will make a corect packet.
This packet will further serialized by SerializeDataPacket and then send to serialized array.
Before sending to the server, the end bit of packet is chanaged to a corrupted bit.
*/
case 4: 
   printf ("End of Packet Missing\n\n");
   MakeDataPacket(1, segment_no, 19, "data client message", &datapkt);
   PrintDataPacket(datapkt);
   memset(serialized,'\0', 264); // Corrupt the end
   SerializeDataPacket(datapkt, serialized);
   serialized[263]=0XF6;
   break;

/* In case user wants to quits*/   
case 5:
   printf("Thank you!\n");
   exit(0);
}
//sending data in serialized array to server

int ResultStatus = sendto(s, serialized, 264, 0 , (struct sockaddr *) &si_other, slen);

memset(serialized,'\0', 264);//

//Count to send packet 3 times again in case server does not respond
int count=3;

while(count!=0)
  {
   //Checking received data status. If result status is 0 or -1 then retransmit again
 
   ResultStatus=recvfrom(s, serialized, 264, 0, (struct sockaddr *) &si_other, &slen);

   if(ResultStatus==0||ResultStatus==-1)
     { 
      printf("Retransmitting Attempt Number: %d\n",4-count);
      ResultStatus=sendto(s, serialized, 264, 0 , (struct sockaddr *) &si_other, slen);
      count=count-1;
     }else{
          break;
          }
   }
//Packet is sent and processed by server print the success message and also print Acknowledgement
if(count!=0 && ResultStatus > 0)
  {
   printf("Success in sending message \n");

/* 
If a reject packet comes from server. 
Parse the serialized array and check for reject packet.
Print the reject packet.
*/  
  printf("Expecting REJECT packet\n\n");
  struct RejectPacket rejectpkt;
  ParseSerializedRejectPacket(serialized, &rejectpkt);
  PrintRejectPacket(rejectpkt);
  printf("---------------------------------------------------\n");
  memset(serialized,'\0', 264); 

  }else{
        printf("Server doesn't respond \n");
        exit(1);
       }
}//while loop end
close(s);
return 0;
}//main end
