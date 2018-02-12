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
#include "PacketUtil.h" 

#define PORT 8888   //The port on which to listen for incoming data
 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(void)
{
 struct sockaddr_in si_me, si_other;
     
 int s, i, slen = sizeof(si_other) , recv_len;
 unsigned char serialized_packet[264];
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
  
int lastSeqNo=0;
/*
In following code, data is received from the client  in serialized_packed array.
This array is parsed and then payload is check and according ACK or REJECT is send.
*/  
while(1)
 {
  printf("--------------------------------------------------------------------------------------------------------------------\n");
  printf("Waiting for data...");//Listening for data from the client
  fflush(stdout);

memset(serialized_packet,'\0', 264); 
//try to receive some data, this is a blocking call
if ((recv_len = recvfrom(s, serialized_packet, 264, 0, (struct sockaddr *) &si_other, &slen)) == -1)
  {
   die("recvfrom()");
  }
//Print the received packet source and length
printf("Received packet from %s:%d and recv_len=%d\n\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), recv_len);

/*
The data came from client is in an array.
This array is parsed and data is stores in datapacket.
This data packet is checked for error handling
*/
struct DataPacket datapkt;
ParseSerializedDataPacket(serialized_packet, &datapkt);
PrintDataPacket(datapkt);
/*
Length of the data packet is checked.
It it doesnot match then make a reject packet for length mismatch.
Serialized the packet and send it to client in array.
*/ 
if(datapkt.Length != strlen(datapkt.PayLoad)) {
   printf("\n\nLength Mismatch. datapkt.Length=%u  and  strlen(datapkt.PayLoad)=%zu\n\n",datapkt.Length,strlen(datapkt.PayLoad));
   struct RejectPacket rejectpkt;
   MakeRejectPacket(datapkt.ClientID, lenth_mismatching, datapkt.SegmentNo, &rejectpkt);
   SerializeRejectPacket(rejectpkt, serialized_packet);

/*
Segment no of data packet is checked.
If it is in same sequence as that of previous make a reject packet for duplicate packet.
Serialized the packet and send it to client in array.
*/
} else if(datapkt.SegmentNo == lastSeqNo){
   printf("\n\nDuplicate packet. Last seq no %d Segment no %d i.e why out of sequence\n", lastSeqNo, datapkt.SegmentNo);
   struct RejectPacket rejectpkt;
   MakeRejectPacket(datapkt.ClientID, duplicate_packet, datapkt.SegmentNo, &rejectpkt);
   SerializeRejectPacket(rejectpkt, serialized_packet);

/*
Last byte is checked.
If it is not 0XFF, then end byte of packet is corrupted.
Make a reject packet for end of packet missing.
Serialized the packet and send it to client in array.
*/
} else if(serialized_packet[262]!= 0XFF || serialized_packet[263] != 0XFF) {
 printf("\n\nEnd of Packet missing\n\n");
 struct RejectPacket rejectpkt;
 MakeRejectPacket(datapkt.ClientID, end_of_packet_missing, datapkt.SegmentNo, &rejectpkt);
 SerializeRejectPacket(rejectpkt, serialized_packet);

/*
Segment no of data packet is checked.
If it is not in sequence make a reject packet for out of sequence.
Serialized the packet and send it to client in array.
*/
} else if(datapkt.SegmentNo != lastSeqNo+1){
 printf("\n\nOut of sequence. Expected segment %u, received segment %u\n\n",lastSeqNo+1,datapkt.SegmentNo);
 struct RejectPacket rejectpkt;
 MakeRejectPacket(datapkt.ClientID, out_of_sequence, datapkt.SegmentNo, &rejectpkt);
 SerializeRejectPacket(rejectpkt, serialized_packet);

//If correct packet, send ACK to client.
} else{
  struct AckPacket ackpkt;
  MakeAckPacket(datapkt.ClientID, datapkt.SegmentNo, &ackpkt);
  SerializeAckPacket(ackpkt, serialized_packet);
  lastSeqNo = datapkt.SegmentNo;
} 
 
//Send ACK or REJECT packet to client
if (sendto(s, serialized_packet, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
  {
   die("sendto()");
  }

}
close(s);
return 0;
}
