
#ifndef KNX_APPLICATIONLAYER_H
#define KNX_APPLICATIONLAYER_H

#include "Pdu.h"
#include "Knx_Types.h"

extern void A_GroupValue_Read_Req(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom);
extern void A_GroupValue_Read_Lcon(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, StatusType status);
extern void A_GroupValue_Read_Ind(uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom);
extern void A_GroupValue_Read_Res(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr);
extern void A_GroupValue_Read_Acon(uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr);

#endif /* #ifndef KNX_APPLICATIONLAYER_H */
