typedef struct DataPacket{
                          unsigned char ClientID;
                          unsigned char SegmentNo;
                          unsigned char Length;
                          unsigned char PayLoad[255];
                         }DataPacket;

typedef struct AckPacket{
                         unsigned char ClientID;
                         unsigned char ReceivedSegmentNo;
                        }AckPacket;

enum Reject_Subcode{out_of_sequence, lenth_mismatching, end_of_packet_missing, duplicate_packet };

typedef struct RejectPacket{
                            unsigned char ClientID;
                            enum Reject_Subcode RejectSubcode;
                            unsigned char ReceivedSegmentNo;
                           }RejectPacket;


void MakeDataPacket(unsigned char clientid, unsigned char segmentno, unsigned char length, unsigned char payload[255], DataPacket *Data);
void PrintDataPacket(DataPacket datapkt);
void SerializeDataPacket(DataPacket datapkt, unsigned char serializedoutput[264]);
void ParseSerialzedDataPacket(unsigned char serialized[264], DataPacket *datapkt);

void MakeAckPacket(unsigned char clientid, unsigned char receivedsegmentno, AckPacket *Ack);
void PrintAckPacket(AckPacket ackpacket);
void SerializeAckPacket(AckPacket ackpkt, unsigned char serializedoutput[264]);
void ParseSerialzedAckPacket(unsigned char serialized[264], AckPacket *ackpkt);

void MakeRejectPacket(unsigned char clientid, enum Reject_Subcode rejectsubcode, unsigned char receivedsegmentno, RejectPacket *Reject);
void PrintRejectPacket(RejectPacket rejectpacket);
void SerializeRejectPacket(RejectPacket rejectpkt, unsigned char serializedoutput[264]);
void ParseSerialzedRejectPacket(unsigned char serialized[264], RejectPacket *rejectpkt);

