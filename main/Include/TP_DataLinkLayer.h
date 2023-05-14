
#ifndef TP_DATALINKLAYER_H
#define TP_DATALINKLAYER_H

#include "Pdu.h"
#include "Knx_Types.h"

extern void TP_L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr);
extern void TP_L_Data_Con(AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr, PduInfoType * pduInfoPtr, StatusType status);
extern void TP_L_Data_Ind(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr);

#endif /* #ifndef TP_DATALINKLAYER_H */
