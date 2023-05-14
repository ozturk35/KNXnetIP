#ifndef KNXNETIP_TUNNELLING_H
#define KNXNETIP_TUNNELLING_H

#include "Pdu.h"
#include "Knx_Types.h"

void KNXnetIP_TunnellingAck(KNXnetIP_TunnellingConnectionHeaderType connectionHeader, KNXnetIP_TunnelingCemiFrameType cemiFrame, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_TunnellingInit(void);
void KNXnetIP_TunnellingFeatureGet(KNXnetIP_FeatureIdentifierType featureIdentifier, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_TunnellingFeatureSet(KNXnetIP_FeatureIdentifierType featureIdentifier, uint16_t value, uint8_t * txBuffer, uint16_t * txLength);

extern void KNXnetIP_TunnelTP2IP(PduInfoType * pduInfoPtr);
extern void KNXnetIP_TunnelIP2TP(PduInfoType * pduInfoPtr);

#endif /* #ifndef KNXNETIP_TUNNELLING_H */ 
