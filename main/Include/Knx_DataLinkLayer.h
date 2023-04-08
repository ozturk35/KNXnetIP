
#ifndef KNX_DATALINKLAYER_H
#define KNX_DATALINKLAYER_H

#include "Pdu.h"
#include "Knx_Types.h"

extern void L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr);
extern void L_Data_Con(AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr, PduInfoType * pduInfoPtr, StatusType status);
extern void L_Data_Ind(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr);

#endif /* #ifndef KNX_DATALINKLAYER_H */
