#ifndef TPUART2_DATALINKLAYER_H
#define TPUART2_DATALINKLAYER_H

#include "Pdu.h"
#include "Knx_Types.h"

extern void TpUart2_L_Data_Req(bool repeatFlag, uint16_t destAddr, AddressType addrType, PriorityType priority, PduInfoType * pduInfoPtr);
extern void TpUart2_L_Data_Con(void);
extern void TpUart2_L_Data_Ind(bool repeatFlag, uint16_t sourceAddr, uint16_t destAddr, AddressType addrType, PriorityType priority, PduInfoType * pduInfoPtr);

#endif /* #ifndef TPUART2_DATALINKLAYER_H */ 
