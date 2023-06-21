#ifndef TPUART2_DATALINKLAYER_H
#define TPUART2_DATALINKLAYER_H

#include "Pdu.h"
#include "Knx_Types.h"

extern void TpUart2_L_Data_Req(bool repeatFlag, uint16_t destAddr, AddressType addrType, PriorityType priority, PduInfoType * pduInfoPtr);
extern void TpUart2_L_Data_Con(void);
extern void TpUart2_L_Data_Ind(PduInfoType * pduInfoPtr);
extern bool TpUart2_DetectEOP(uint8_t * dataPtr, uint8_t rxLength, uint8_t * validDataLength);

#endif /* #ifndef TPUART2_DATALINKLAYER_H */ 
