
#ifndef KNX_TRANSPORTLAYER_H
#define KNX_TRANSPORTLAYER_H

#include "Pdu.h"
#include "Knx_Types.h"

extern const uint16_t GroupAddressTable[];

extern void T_Data_Group_Req(AckType ack, HopCountType hopCount, uint8_t octetCount, PriorityType priority, uint16_t tsap, PduInfoType * pduInfoPtr);
extern void T_Data_Group_Ind(HopCountType hopCount, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr, uint16_t tsap, PduInfoType * pduInfoPtr);

#endif /* #ifndef KNX_TRANSPORTLAYER_H */
