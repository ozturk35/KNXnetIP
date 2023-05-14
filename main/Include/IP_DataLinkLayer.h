#ifndef IP_DATALINKLAYER_H
#define IP_DATALINKLAYER_H

#include "Pdu.h"
#include "Knx_Types.h"

void IP_L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint16_t octetCount, PriorityType priority, uint16_t sourceAddr);
void IP_L_Data_Ind(PduInfoType * pduInfoPtr,  uint32_t ipAddr, uint16_t port);

extern uint8_t IP_TxBuffer[256];

#endif /* #ifndef IP_DATALINKLAYER_H */ 
