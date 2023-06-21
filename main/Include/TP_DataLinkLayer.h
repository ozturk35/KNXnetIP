
#ifndef TP_DATALINKLAYER_H
#define TP_DATALINKLAYER_H

#include "Pdu.h"
#include "Knx_Types.h"

extern void TP_GW_L_Data_Req(uint8_t * bufferPtr, uint8_t rxLength);
extern void TP_GW_L_Data_Ind(PduInfoType * pduInfoPtr);
extern void TP_GW_L_Data_Ind_ACK(PduInfoType * pduInfoPtr);

#endif /* #ifndef TP_DATALINKLAYER_H */
