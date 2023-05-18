#ifndef KNXNETIP_TUNNELLING_H
#define KNXNETIP_TUNNELLING_H

#include "Pdu.h"
#include "Knx_Types.h"

void KNXnetIP_TunnellingAck(uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_TunnellingInit(void);
void KNXnetIP_TunnellingFeatureGet(KNXnetIP_FeatureIdentifierType featureIdentifier, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_TunnellingFeatureSet(KNXnetIP_FeatureIdentifierType featureIdentifier, uint16_t value, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_TunnellingRequest(uint8_t * bufferPtr, uint16_t length);

extern void KNXnetIP_TunnelIP2TP(uint8_t * rxBufferPtr, uint8_t rxLength);

#endif /* #ifndef KNXNETIP_TUNNELLING_H */ 
