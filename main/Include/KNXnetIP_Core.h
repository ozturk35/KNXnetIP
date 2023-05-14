#ifndef KNXNETIP_CORE_H
#define KNXNETIP_CORE_H

#include "Knx_Types.h"
#include "KNXnetIP_Types.h"

void KNXnetIP_SearchResponse(uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_SearchResponseExtended(uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_DescriptionResponse(uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_ConnectResponse(KNXnetIP_ErrorCodeType errorCode, KNXnetIP_HPAIType * connectRequestHpai, KNXnetIP_CRIType * cri, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_ConnectionStateResponse(KNXnetIP_ErrorCodeType errorCode, uint8_t * txBuffer, uint16_t * txLength);
//void KNXnetIP_DisconnectRequest(void);
void KNXnetIP_DisconnectResponse(uint8_t * txBuffer, uint16_t * txLength);

#endif /* #ifndef KNXNETIP_CORE_H */ 
