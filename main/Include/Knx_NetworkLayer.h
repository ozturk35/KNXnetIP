
#ifndef KNX_NETWORKLAYER_H
#define KNX_NETWORKLAYER_H

#include "Pdu.h"
#include "Knx_Types.h"

extern void N_Data_Group_Req(AckType ack, uint16_t destAddr, HopCountType hopCount, uint8_t octetCount, PriorityType priority, PduInfoType * pduInfoPtr);
extern void N_Data_Group_Con(AckType ack, uint16_t destAddr, HopCountType hopCount, uint8_t octetCount, PriorityType priority, PduInfoType * pduInfoPtr, bool status);
extern void N_Data_Group_Ind(uint16_t destAddr, HopCountType hopCount, uint16_t octetCount, PriorityType priority, PduInfoType * pduInfoPtr);
extern void N_Data_Broadcast_Ind(void);
extern void N_Data_Individual_Ind(void);

#endif /* #ifndef KNX_NETWORKLAYER_H */
