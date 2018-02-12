/*
Programming Assignment 2 Client Model
Objective:  Client Using customized protocol on top of UDP protocol for requesting identification from server for access permission to network..
Author: Pratishtha Deep
*/

#include<stdio.h>//printf
#include<string.h>//memset
#include<stdlib.h>//exit(0)
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>

#define SERVER "127.0.0.1"
#define PORT 8888 //The port on which to listen for incoming data
#define ResultLength  264

void die(char *s)
{
    perror(s);
    exit(1);
}


int main(void)
{
 struct sockaddr_in si_other;
 
 int s, i, slen=sizeof(si_other);
 char Result[ResultLength];
//Making UDP socket 
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
// Timer logic for retransmitting data
struct timeval read_timeout={3,0};

setsockopt(s,SOL_SOCKET,SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));

int Counter=1;
unsigned char DataPacket[264];
char input[255];
char Hex[8];
char sHex[4];
int Segment=1;
long Quotient=0;
int Remainder;
int j=0;
int Technology;

// Creating the common Packet Segment
DataPacket[0]=0xFF;
DataPacket[1]=0xFF;
DataPacket[2]=0xFF;
DataPacket[3]=0xFF;
DataPacket[4]=0xF8;
DataPacket[5]=Segment+'0';
DataPacket[6]=14+'0';
DataPacket[12]=0xFF;
DataPacket[13]=0xFF;
DataPacket[14]='\0';

//Counter for 4 time verifying the number. The number is converted in ascii format and then it is send to server
while(Counter<=4)
  {
   printf("Enter the user mobile number \n");
   scanf("%ld",&Quotient);

   printf("Enter Technology \n");
   scanf("%d",&Technology);

//Converting 10 digit decimal to 8 bit ascii
  j=0;
  while (Quotient != 0)
  {
   Remainder = Quotient % 16;
 
  if (Remainder < 10)
    Hex[j++] = 48 + Remainder;

  else
    Hex[j++] = 55 + Remainder;
    Quotient = Quotient / 16;
  }
    Hex[j++]='\0';

    DataPacket[7]=Technology+'0';
// Convertig 8 bit ascii to 4 bytes
    DataPacket[8]= Hex[0]*10+Hex[1]+'0';
    DataPacket[9]= Hex[2]*10+Hex[3]+'0';
    DataPacket[10]= Hex[4]*10+Hex[5]+'0';
    DataPacket[11]= Hex[6]*10+Hex[7]+'0';
// Sending the data Packet

int ResultStatus=sendto(s, DataPacket, 14 , 0 , (struct sockaddr *) &si_other, slen);

memset(Result,'\0', ResultLength);
// Trying 3 times ,in case packet is not recieved by server
int Count=4;

while(Count!=0)
 {
  ResultStatus=recvfrom(s, Result, ResultLength, 0, (struct sockaddr *) &si_other, &slen);
 
  if(ResultStatus==0|ResultStatus==-1)
   {
    printf("retransmitting attempt no: %d\n",4-Count);
    ResultStatus=sendto(s, DataPacket, DataPacket[6] , 0 , (struct sockaddr *) &si_other, slen);
    Count=Count-1;
   }
   else{
        break;
       }
   }
// Message for when Packet is sent and processed by server
   if(Count!=0 && ResultStatus > 0)
   {
    printf("Success in sending message \n");
   }
   else{
    printf("Server doesn't respond \n");
   }
   if(Result[4]==(char)0xF9)
   {
    printf("Subscriber has not paid \n");
   }
    else if(Result[4]==(char)0xFA)
   {
    printf("Subscriber does not exist in database \n");
   }
   else if(Result[4]==(char)0xFB)
   {
    printf("Subscriber permitted. Enjoy \n");
   }
    Segment=Segment+1;
    Counter=Counter+1;
   }

}


