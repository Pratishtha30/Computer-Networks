#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include "PacketUtil.h"

void MakeDataPacket(unsigned char clientid, unsigned char segmentno, unsigned char length, unsigned char payload[255], DataPacket *Data)
{
  Data->ClientID=clientid;
  Data->Length=length;
  Data->SegmentNo=segmentno;
  //if(payload): check the length is correct
  strncpy(Data->PayLoad, payload, length);
}

void MakeAckPacket(unsigned char clientid, unsigned char receivedsegmentno, AckPacket *Ack)
{
  Ack->ClientID=clientid;
  Ack->ReceivedSegmentNo=receivedsegmentno;
 
}

void MakeRejectPacket(unsigned char clientid, enum Reject_Subcode rejectsubcode, unsigned char receivedsegmentno, RejectPacket *Reject)
{
  Reject->ClientID=clientid;
  Reject->RejectSubcode=rejectsubcode;
  Reject->ReceivedSegmentNo=receivedsegmentno;
}


void PrintDataPacket(DataPacket datapkt)
{
  printf("PrintDataPacket(human readable): ClientID %u  SegmentNo %u  Length %u Payload %s\n ", datapkt.ClientID, datapkt.SegmentNo, datapkt.Length, datapkt.PayLoad);
  printf("PrintDataPacket           (hex): ClientID %X  SegmentNo %X  Length %X Payload %s\n ", datapkt.ClientID, datapkt.SegmentNo, datapkt.Length, datapkt.PayLoad);
}

void PrintAckPacket(AckPacket ackpacket)
{
  printf("PrintAckPacket                 :ClientID %u ReceivedSegmentNo %u\n ", ackpacket.ClientID, ackpacket.ReceivedSegmentNo);
}

void PrintRejectPacket(RejectPacket rejectpacket)
{
  printf("PrintRejectPacket(human readable): ClientID %u RejectSubcode %u ReceivedSegmentNo %u\n ", rejectpacket.ClientID, (unsigned int)rejectpacket.RejectSubcode, rejectpacket.ReceivedSegmentNo);
  printf("PrintRejectPacket             HEX: ClientID %x RejectSubcode %x ReceivedSegmentNo %x\n ", rejectpacket.ClientID, rejectpacket.RejectSubcode, rejectpacket.ReceivedSegmentNo);
}


void SerializeDataPacket(DataPacket datapkt, unsigned char serializedoutput[264])
{
serializedoutput[0]=0xFF;// populate start of Packet Identifier

serializedoutput[1]=0xFF;
serializedoutput[2]=datapkt.ClientID;
serializedoutput[3]=0xFF; //popuating DATA
serializedoutput[4]=0xF1;
serializedoutput[5]=datapkt.SegmentNo;
serializedoutput[6]=datapkt.Length;
strncpy(&serializedoutput[7], datapkt.PayLoad, datapkt.Length);
serializedoutput[262]=0xFF;
serializedoutput[263]=0xFF;
}

void ParseSerializedDataPacket(unsigned char serialized[264], DataPacket *datapkt)
{
if((serialized[0] != 0xFF) || (serialized[1]!=0xFF)){
printf("Start of Packet Identifier corrupted %02X %02X\n", (unsigned char) serialized[0], (unsigned char) serialized[1]);
}

datapkt->ClientID=serialized[2];
if(serialized[3]!=0xFF || serialized[4]!=0xF1){
printf("Corrupted DATA %x %x\n", serialized[7], serialized[4]);
}
datapkt->SegmentNo=serialized[5];
datapkt->Length= serialized[6];
strncpy(datapkt->PayLoad, &serialized[7], datapkt->Length);

if((serialized[262] != 0xFF)|| (serialized[263]!=0xFF)){
  printf("Corrupted end of Packet Identifier %X %X\n", serialized[262],serialized[263]);
  }
}

void SerializeAckPacket(AckPacket ackpkt, unsigned char serializedoutput[264])
{
serializedoutput[0]=0xFF;// populate start of Packet Identifier
serializedoutput[1]=0xFF;
serializedoutput[2]=ackpkt.ClientID;
serializedoutput[3]=0xFF; //popuating ACK
serializedoutput[4]=0xF2;
serializedoutput[5]=ackpkt.ReceivedSegmentNo;
serializedoutput[262]=0xFF;
serializedoutput[263]=0xFF;
}

void ParseSerializedAckPacket(unsigned char serialized[264], AckPacket *ackpkt)
{
if((serialized[0] != 0xFF) && (serialized[1]!=0xFF)){
printf("Start of Packet Identifier corrupted %02X %02X\n", (unsigned char) serialized[0], (unsigned char) serialized[1]);
}
ackpkt->ClientID=serialized[2];
if(serialized[3]!=0xFF && serialized[4]!=0xF2){
printf("Corrupted ACK %x %x\n", serialized[3], serialized[4]);
}
ackpkt->ReceivedSegmentNo=serialized[5];
if((serialized[262] != 0xFF)&& (serialized[263]!=0xFF)){
  printf("Corrupted end of Packet Identifier %X %X\n", serialized[262],serialized[263]);
  }
}

void SerializeRejectPacket(RejectPacket rejectpkt, unsigned char serializedoutput[264])
{
serializedoutput[0]=0xFF;// populate start of Packet Identifier
serializedoutput[1]=0xFF;
serializedoutput[2]=rejectpkt.ClientID;
serializedoutput[3]=0xFF; //popuating REJECT
serializedoutput[4]=0xF3;
serializedoutput[5]=0XFF;
switch (rejectpkt.RejectSubcode) {
 case out_of_sequence:
   serializedoutput[6]=0XF4;
   break;
 case lenth_mismatching:
   serializedoutput[6]=0XF5;
   break;
 case end_of_packet_missing:
   serializedoutput[6]=0XF6;
   break;
 case duplicate_packet:
   serializedoutput[6]=0XF7;
   break;
 default:
   printf("Invalid enum value:%u\n",(unsigned int)rejectpkt.RejectSubcode);
   exit(1);
}
serializedoutput[7]=rejectpkt.ReceivedSegmentNo;
serializedoutput[262]=0xFF;
serializedoutput[263]=0xFF;

}

void ParseSerializedRejectPacket(unsigned char serialized[264], RejectPacket *rejectpkt)
{

if((serialized[0] != 0xFF) || (serialized[1]!=0xFF)){
printf("Start of Packet Identifier corrupted %02X %02X\n", (unsigned char) serialized[0], (unsigned char) serialized[1]);
}
rejectpkt->ClientID=serialized[2];
if(serialized[3]!=0xFF && serialized[4]!=0xF3){
printf("Corrupted REJECT %02X %02X\n", serialized[3], serialized[4]);
}
if(serialized[5]!=0XFF){
 printf("Corrupted First Byte of Reject Sub code %02X\n", serialized[5]);
}

if(serialized[6]==0XF4){
rejectpkt->RejectSubcode=out_of_sequence;
} else
if(serialized[6]==0XF5){
rejectpkt->RejectSubcode=lenth_mismatching;
 }else
if(serialized[6]==0XF6){
rejectpkt->RejectSubcode=end_of_packet_missing;
 }else
if(serialized[6]==0XF7){
rejectpkt->RejectSubcode=duplicate_packet;
 }else{
printf("Corrupted Second Byte of Reject Sub code %02X\n", serialized[6]);
}

rejectpkt->ReceivedSegmentNo= serialized[7];
if((serialized[262] != 0xFF)&& (serialized[263]!=0xFF)){
  printf("Corrupted end of Packet Identifier %X %X\n", serialized[262],serialized[263]);
  }
}


