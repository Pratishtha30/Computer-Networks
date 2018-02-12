/*
Programming Assignment 1 Server Model
Objective:  Client Using customized protocol on top of UDP protocol for requesting identification from server for access permission to network
Author: Pratishtha Deep
*/

#include<stdio.h>//printf
#include<string.h>//memset
#include<stdlib.h>//exit(0)
#include<arpa/inet.h>
#include<sys/socket.h>

#define PacketLen 512
#define PORT 8888//The port on which to listen for incoming data

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
 struct sockaddr_in si_me, si_other;

 int s, i, slen = sizeof(si_other) , rlen;
 int seq=1;
  //create a UDP socket
 if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
   die("socket");
  }

 memset((char *) &si_me, 0, sizeof(si_me));

 si_me.sin_family = AF_INET;
 si_me.sin_port = htons(PORT);
 si_me.sin_addr.s_addr = htonl(INADDR_ANY);
 //bind socket to port
 if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
   {
    die("bind");
   }

while(1)
 {
  printf("Waiting for data... \n");
  fflush(stdout);
  int sendlen=0;
// Recieving the pAcket
  unsigned char Packet[PacketLen];

if ((rlen = recvfrom(s, Packet, PacketLen, 0, (struct sockaddr *) &si_other,&slen)) == -1)
  {
   die("recvfrom()");
  }

unsigned char Ack[14];
int Count=-1, technology=0, paid=0, choice=0, j=0;
long Phone_No;
int Remainder;
char Hex_number[8];
unsigned char Temp[4];
char *line = NULL;
ssize_t read, len=0;
//Reading from File
FILE *fp = fopen("Verification_DataBase.txt", "r");

if(fp==NULL)
{
 printf("\nCannot Open File\n");
 exit(EXIT_FAILURE);
}
/*
In following code, Subscriber number, technology and paid status is read.
The subscriber number is converted into hexa decimal and stored in a Temp array
AckPacket is made accordingly
*/
while((read = getline(&line, &len, fp)) != -1)
 {
  if(Count == -1)
   {
    Count++;
    continue;
   }else{
      sscanf(line, "%ld %d %d", &Phone_No, &technology, &paid);
      // Converting 10 digit number to ascii
      j=0;
      //printf ("phone %ld \n",Phone_No);
while (Phone_No != 0)
{
 Remainder =Phone_No % 16;
   if (Remainder < 10)
   Hex_number[j++] = 48 + Remainder;
   else
   Hex_number[j++] = 55 + Remainder;
   Phone_No =Phone_No / 16;
}
  Hex_number[j++]='\0';
// Converting ascii 4 bytes
  Temp[0]= Hex_number[0]*10+Hex_number[1]+'0';
  Temp[1]= Hex_number[2]*10+Hex_number[3]+'0';
  Temp[2]= Hex_number[4]*10+Hex_number[5]+'0';
  Temp[3]= Hex_number[6]*10+Hex_number[7]+'0';
// Compare logic
if (Packet[8]==Temp[0] && Packet[9]==Temp[1] && Packet[10]==Temp[2] && Packet[11]==Temp[3] && Packet[7]==technology+'0' )
  {
   if (paid==1)
     {
      choice=1;
      printf("phone no is registered with technologynology and is paid \n");
     }else{
      choice=2;
      printf("phone no is registered with technologynology but has not paid\n");
     }
     break;
   }
 }
}
//Crafting common packet segments
Ack[0]=0xFF;
Ack[1]=0xFF;
Ack[2]=0xFF;
Ack[5]=seq+'0';
Ack[6]=14;
Ack[7]=Packet[7];
Ack[8]=Packet[8];
Ack[9]=Packet[9];
Ack[10]=Packet[10];
Ack[11]=Packet[11];
Ack[12]=0xFF;
Ack[13]=0xFF;
//crafting rest based on user
if (choice==1)
{
 Ack[3]=0xFF;
 Ack[4]=0xFB;
}
else if (choice==2)
{
 Ack[3]=0xFF;
 Ack[4]=0xF9;
}
else
{
 Ack[3]=0xFF;
 Ack[4]=0xFA;
printf("phone no is not registered with technologynology \n");
}
//Sending Response
if (sendto(s, Ack, 14, 0, (struct sockaddr*) &si_other, slen) == -1)
{
 die("sendto()");
}
seq=seq+1;
}
 close(s);
 return 0;
}

