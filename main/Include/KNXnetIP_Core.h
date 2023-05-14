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

#define IP_ADDRESS(x,y,z,t) (uint32_t)((((uint32_t)t << 24) & 0xFF000000) | \
                                       (((uint32_t)z << 16) & 0xFF0000) | \
                                       (((uint32_t)y << 8) & 0xFF00) | \
                                       ((uint32_t)x & 0xFF))

#define CHANNEL_1 (1U)
#define CHANNEL_2 (2U)
#define CHANNEL_3 (3U)
#define CHANNEL_4 (4U)

#define KNX_CHANNEL_NUM (4U)

#define KNX_INDIVIDUAL_ADDR  (0x1101U)
#define KNX_SUPPORTED_SERVICE_NUM (4U)
#define KNX_INDIVIDUAL_ADDR_NUM (2U)
#define KNX_TUNNELLING_SLOT_NUM (1U)

#define KNX_TUNNELLING_SLOT_STATUS_FREE       (0x01U)
#define KNX_TUNNELLING_SLOT_STATUS_AUTHORIZED (0x02U)
#define KNX_TUNNELLING_SLOT_STATUS_USABLE     (0x04U)
#endif /* #ifndef KNXNETIP_CORE_H */ 
