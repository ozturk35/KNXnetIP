/**
 * \file KNXnetIP_Core.c
 * 
 * \brief KNXnet/IP Core Services
 * 
 * This file contains the implementation of KNXnet/IP Core Services
 * 
 * \version 1.0.0
 * 
 * \author Ibrahim Ozturk
 * 
 * Copyright 2023 Ibrahim Ozturk
 * All rights exclusively reserved for Ibrahim Ozturk,
 * unless expressly agreed to otherwise.
*/

/*==================[inclusions]============================================*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

#include "KNXnetIP.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/
extern uint32_t KnxIPInterface_IpAddr;

KNXnetIP_ChannelType KNXnetIP_Channel[KNX_CHANNEL_NUM] = {
    {CHANNEL_1, CH_FREE},
    {CHANNEL_2, CH_FREE},
    {CHANNEL_3, CH_FREE},
    {CHANNEL_4, CH_FREE},
};

/*==================[internal data]=========================================*/
static const KNXnetIP_ServiceFamilyType KnxSupportedServiceFamilies[KNX_SUPPORTED_SERVICE_NUM] = {
    {KNXNETIP_CORE,       0x02U},
    {KNXNETIP_DEVICEMGMT, 0x02U},
    {KNXNETIP_TUNNELLING, 0x02U},
    {KNXNETIP_SECURE,     0x01U},
};

static const uint16_t KNXnetIP_KnxIndividualAddresses[KNX_INDIVIDUAL_ADDR_NUM] =
{
    KNX_INDIVIDUAL_ADDR,
    0x11FAU,
};

static KNXnetIP_TunnelingSlotType KNXnetIP_TunnelingSlot[KNX_TUNNELLING_SLOT_NUM] = 
{
    {0x11FAU, KNX_TUNNELLING_SLOT_STATUS_FREE},
};

static const uint8_t KnxDeviceFriendlyName[30] = {'A','T','I','O','S',' ','K','N','X',' ','B','R','I','D','G','E'};
//static const uint8_t KnxDeviceSerialNumber[6] = {0x00, 0xEF, 0x26, 0x50, 0x06, 0x5C};
static const uint8_t KnxDeviceSerialNumber[6] = {0x00U, 0x83U, 0x78U, 0x40U, 0x13U, 0x89U};

static const uint8_t KnxDeviceMACAddress[6] = {0xCCU, 0x1BU, 0xE0, 0x80U, 0x96U, 0x1AU};
static const uint32_t KnxDeviceMulticastAddr = IP_ADDRESS(224, 0, 23, 12);
static const uint32_t KnxDeviceZeroAddr = IP_ADDRESS(0, 0, 0, 0);

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

void KNXnetIP_SearchResponse(uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_SearchResponseExtended(uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_DescriptionResponse(uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_ConnectResponse(KNXnetIP_ErrorCodeType errorCode, KNXnetIP_HPAIType * connectRequestHpai, KNXnetIP_CRIType * cri, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_ConnectionStateResponse(KNXnetIP_ErrorCodeType errorCode, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_DisconnectRequest(KNXnetIP_HPAIType * disconnectRequestHpai, uint8_t * txBuffer, uint16_t * txLength );
void KNXnetIP_DisconnectResponse(uint8_t * txBuffer, uint16_t * txLength);

void KNXnetIP_SearchResponse(uint8_t * txBuffer, uint16_t * txLength)
{
    uint16_t txBytes = 0;

    /* HPAI Control endpoint - Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* HPAI Control endpoint - Host Protocol Code */
    txBuffer[txBytes++] = IPV4_UDP;

    /* HPAI Control endpoint - IP Address */
    txBuffer[txBytes++] = (uint8_t)(KnxIPInterface_IpAddr & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((KnxIPInterface_IpAddr >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((KnxIPInterface_IpAddr >> 16) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((KnxIPInterface_IpAddr >> 24) & 0xFFU);

    /* HPAI Control endpoint - Port Number */
    txBuffer[txBytes++] = (uint8_t)((UDP_PORT >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(UDP_PORT & 0xFFU);

    /* DIB Device Hardware - Structure Length */
    txBuffer[txBytes++] = 0x36U;

    /* DIB Device Hardware - Description Type Code */
    txBuffer[txBytes++] = DEVICE_INFO;

    /* DIB Device Hardware - KNX Medium */
    txBuffer[txBytes++] = KNX_TP1;

    /* DIB Device Hardware - Device Status */
    txBuffer[txBytes++] = 0x00U; /* Programming mode */

    /* DIB Device Hardware - Individual Address */
    txBuffer[txBytes++] = (uint8_t)((KNX_INDIVIDUAL_ADDR >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(KNX_INDIVIDUAL_ADDR & 0xFFU);

    /* DIB Device Hardware - Project-InstallationID */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Device Hardware - Serial Number */
    memcpy(&txBuffer[txBytes], &KnxDeviceSerialNumber[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Routing Multicast Address */
    memcpy(&txBuffer[txBytes], &KnxDeviceZeroAddr, 4U);
    txBytes += 4;

    /* DIB Device Hardware - MAC Address */
    memcpy(&txBuffer[txBytes], &KnxDeviceMACAddress[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Friendly Name */
    memcpy(&txBuffer[txBytes], &KnxDeviceFriendlyName[0], 30);
    txBytes += 30;

    /* DIB Supported Service Family - Structure Length */
    txBuffer[txBytes++] = 0x0AU;

    /* DIB Supported Service Family - Description Type Code */
    txBuffer[txBytes++] = SUPP_SVC_FAMILIES;

    /* DIB Supported Service Family - Supported Service Families */
    for (uint8_t index = 0; index < KNX_SUPPORTED_SERVICE_NUM; index++)
    {
        txBuffer[txBytes++] = KnxSupportedServiceFamilies[index].ServiceFamilyId;
        txBuffer[txBytes++] = KnxSupportedServiceFamilies[index].ServiceFamilyVersion;
    }

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_SearchResponseExtended(uint8_t * txBuffer, uint16_t * txLength)
{
    uint16_t txBytes = 0;
    uint8_t index = 0;

    /* HPAI Control endpoint - Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* HPAI Control endpoint - Host Protocol Code */
    txBuffer[txBytes++] = IPV4_UDP;

    /* HPAI Control endpoint - IP Address */
    txBuffer[txBytes++] = (uint8_t)(KnxIPInterface_IpAddr & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((KnxIPInterface_IpAddr >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((KnxIPInterface_IpAddr >> 16) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((KnxIPInterface_IpAddr >> 24) & 0xFFU);

    /* HPAI Control endpoint - Port Number */
    txBuffer[txBytes++] = (uint8_t)((UDP_PORT >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(UDP_PORT & 0xFFU);

    /* DIB Device Hardware - Structure Length */
    txBuffer[txBytes++] = 0x36U;

    /* DIB Device Hardware - Description Type Code */
    txBuffer[txBytes++] = DEVICE_INFO;

    /* DIB Device Hardware - KNX Medium */
    txBuffer[txBytes++] = KNX_TP1;

    /* DIB Device Hardware - Device Status */
    txBuffer[txBytes++] = 0x00U; /* Programming mode */

    /* DIB Device Hardware - Individual Address */
    txBuffer[txBytes++] = (uint8_t)((KNX_INDIVIDUAL_ADDR >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(KNX_INDIVIDUAL_ADDR & 0xFFU);

    /* DIB Device Hardware - Project-InstallationID */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Device Hardware - Serial Number */
    memcpy(&txBuffer[txBytes], &KnxDeviceSerialNumber[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Routing Multicast Address */
    memcpy(&txBuffer[txBytes], &KnxDeviceZeroAddr, 4U);
    txBytes += 4;

    /* DIB Device Hardware - MAC Address */
    memcpy(&txBuffer[txBytes], &KnxDeviceMACAddress[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Friendly Name */
    memcpy(&txBuffer[txBytes], &KnxDeviceFriendlyName[0], 30);
    txBytes += 30;

    /* DIB Supported Service Family - Structure Length */
    txBuffer[txBytes++] = 0x0AU;

    /* DIB Supported Service Family - Description Type Code */
    txBuffer[txBytes++] = SUPP_SVC_FAMILIES;

    /* DIB Supported Service Family - Supported Service Families */
    for (index = 0; index < KNX_SUPPORTED_SERVICE_NUM; index++)
    {
        txBuffer[txBytes++] = KnxSupportedServiceFamilies[index].ServiceFamilyId;
        txBuffer[txBytes++] = KnxSupportedServiceFamilies[index].ServiceFamilyVersion;
    }

    /* DIB Extended Device Information - Structure Length*/
    txBuffer[txBytes++] = 0x08U;

    /* DIB Extended Deice Information - Description Type Code */
    txBuffer[txBytes++] = EXTENDED_DEVICE_INFO;

    /* DIB Extended Device Information - Medium Status */
    /* For a KNXnet/IP Tunnelling Server on TP1,                 */
    /* this shall be the connection state of the TP1 connection. */
    txBuffer[txBytes++] = 0x00U; /* i.e. KnxTpUart2_GetConnectionState() */

    /* DIB Extended Device Information - Reserved */
    txBuffer[txBytes++] = 0x00U;

    /* DIB Extended Device Information - Maximal Local APDU length */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0xF0U;

    /* DIB Extended Device Information - Device Descriptor Type 0 */
    txBuffer[txBytes++] = (uint8_t)((KNXNETIP_SYS7 >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(KNXNETIP_SYS7 & 0xFFU);

    /* DIB Ip Config - Structure Length */
    txBuffer[txBytes++] = 0x10U;

    /* DIB Ip Config - Description Type Code */
    txBuffer[txBytes++] = IP_CONFIG;

    /* DIB Ip Config - IP Address */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Ip Config - Subnet Mask */
    txBuffer[txBytes++] = 0xFFU;
    txBuffer[txBytes++] = 0xFFU;
    txBuffer[txBytes++] = 0xFFU;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Ip Config - Default Gateway */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Ip Config - IP Capabilities  */
    txBuffer[txBytes++] = 0x01; /* BootP */

    /* DIB Ip Config - IP Assignment */
    txBuffer[txBytes++] = 0x04; /* DHCP */

    /* DIB Current Config - Structure Length */
    txBuffer[txBytes++] = 0x14U;

    /* DIB Current Config - Description Type Code */
    txBuffer[txBytes++] = IP_CUR_CONFIG;

    /* DIB Current Config - IP Address */
    txBuffer[txBytes++] = (uint8_t)(KnxIPInterface_IpAddr & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((KnxIPInterface_IpAddr >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((KnxIPInterface_IpAddr >> 16) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((KnxIPInterface_IpAddr >> 24) & 0xFFU);

    /* DIB Current Config - Subnet Mask */
    txBuffer[txBytes++] = 0xFFU;
    txBuffer[txBytes++] = 0xFFU;
    txBuffer[txBytes++] = 0xFFU;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Current Config - Default Gateway */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Current Config - DHCP Server */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Current Config - IP Assignment */
    txBuffer[txBytes++] = 0x04; /* DHCP */

    /* DIB Current Config - Reserved */
    txBuffer[txBytes++] = 0x00U;

    /* DIB Knx Address - Structure Length */
    txBuffer[txBytes++] = 0x06U;

    /* DIB Knx Address - Description Type Code */
    txBuffer[txBytes++] = KNX_ADDRESSES;

    /* DIB Knx Address - KNX Individual Address */
    for (index = 0; index < KNX_INDIVIDUAL_ADDR_NUM; index++)
    {
        txBuffer[txBytes++] = (uint8_t)((KNXnetIP_KnxIndividualAddresses[index] >> 8) & 0xFFU);
        txBuffer[txBytes++] = (uint8_t)(KNXnetIP_KnxIndividualAddresses[index] & 0xFFU);
    }

    /* DIB Tunnel Information - Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* DIB Tunnel Information - Description Type Code */
    txBuffer[txBytes++] = TUNNELLING_INFO;

    /* DIB Tunnel Information - Maximal Local APDU length */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0xF0U;

    /* DIB Tunnel Information - Tunneling Slot */
    for (index = 0; index < KNX_TUNNELLING_SLOT_NUM; index++)
    {
        txBuffer[txBytes++] = (uint8_t)((KNXnetIP_TunnelingSlot[index].IndvAddr >> 8) & 0xFFU);
        txBuffer[txBytes++] = (uint8_t)(KNXnetIP_TunnelingSlot[index].IndvAddr & 0xFFU);

        /* Reserved */
        txBuffer[txBytes++] = 0x00; 

        /* Slot Status */
        txBuffer[txBytes++] = KNXnetIP_TunnelingSlot[index].SlotStatus;
    }

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_DescriptionResponse(uint8_t * txBuffer, uint16_t * txLength)
{
    uint16_t txBytes = 0;

    /* DIB Device Hardware - Structure Length */
    txBuffer[txBytes++] = 0x36U;

    /* DIB Device Hardware - Description Type Code */
    txBuffer[txBytes++] = DEVICE_INFO;

    /* DIB Device Hardware - KNX Medium */
    txBuffer[txBytes++] = KNX_TP1;

    /* DIB Device Hardware - Device Status */
    txBuffer[txBytes++] = 0x00U; /* Programming mode */

    /* DIB Device Hardware - Individual Address */
    txBuffer[txBytes++] = 0xFFU;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Device Hardware - Project-InstallationID */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Device Hardware - Serial Number */
    memcpy(&txBuffer[txBytes], &KnxDeviceSerialNumber[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Routing Multicast Address */
    memcpy(&txBuffer[txBytes], &KnxDeviceMulticastAddr, 4U);
    txBytes += 4;

    /* DIB Device Hardware - MAC Address */
    memcpy(&txBuffer[txBytes], &KnxDeviceMACAddress[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Friendly Name */
    memcpy(&txBuffer[txBytes], &KnxDeviceFriendlyName[0], 30);
    txBytes += 30;

    /* DIB Supported Service Family - Structure Length */
    txBuffer[txBytes++] = 0x0AU;

    /* DIB Supported Service Family - Description Type Code */
    txBuffer[txBytes++] = SUPP_SVC_FAMILIES;

    /* DIB Supported Service Family - Supported Service Families */
    for (uint8_t index = 0; index < KNX_SUPPORTED_SERVICE_NUM; index++)
    {
        txBuffer[txBytes++] = KnxSupportedServiceFamilies[index].ServiceFamilyId;
        txBuffer[txBytes++] = KnxSupportedServiceFamilies[index].ServiceFamilyVersion;
    }

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_ConnectResponse(KNXnetIP_ErrorCodeType errorCode, KNXnetIP_HPAIType * connectRequestHpai, KNXnetIP_CRIType * cri, uint8_t * txBuffer, uint16_t * txLength)
{
    uint16_t txBytes = 0;

    /* Communication Channel ID */
    txBuffer[txBytes++] = KNXnetIP_Channel[0].ChannelId;

    /* Status Code */
    txBuffer[txBytes++] = errorCode;

    /* HPAI Control endpoint - Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* HPAI Control endpoint - Host Protocol Code */
    txBuffer[txBytes++] = connectRequestHpai->HostProtocolCode;

    /* HPAI Control endpoint - IP Address */
    txBuffer[txBytes++] = (uint8_t)((connectRequestHpai->ipAddress >> 24) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((connectRequestHpai->ipAddress >> 16) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((connectRequestHpai->ipAddress >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(connectRequestHpai->ipAddress & 0xFFU);

    /* HPAI Control endpoint - Port Number */
    txBuffer[txBytes++] = (uint8_t)((connectRequestHpai->portNumber >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(connectRequestHpai->portNumber & 0xFFU);

    if (TUNNEL_CONNECTION == cri->ConnectionTypeCode)
    {
        /* CRD - Structure Length */
        txBuffer[txBytes++] = 0x04;

        /* CRD - Connection Type Code */
        txBuffer[txBytes++] = TUNNEL_CONNECTION;

        /* CRD - Individual Address */
        txBuffer[txBytes++] = (uint8_t)((KNXnetIP_TunnelingSlot[0].IndvAddr >> 8) & 0xFFU);
        txBuffer[txBytes++] = (uint8_t)(KNXnetIP_TunnelingSlot[0].IndvAddr & 0xFFU);
    }
    else if (DEVICE_MGMT_CONNECTION == cri->ConnectionTypeCode)
    {
        /* CRD - Structure Length */
        txBuffer[txBytes++] = 0x02;

        /* CRD - Connection Type Code */
        txBuffer[txBytes++] = DEVICE_MGMT_CONNECTION;
    }

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_ConnectionStateResponse(KNXnetIP_ErrorCodeType errorCode, uint8_t * txBuffer, uint16_t * txLength)
{
    uint8_t txBytes = 0;

    /* Communication Channel ID */
    txBuffer[txBytes++] = KNXnetIP_Channel[0].ChannelId;

    /* Status Code */
    txBuffer[txBytes++] = errorCode;

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_DisconnectRequest(KNXnetIP_HPAIType * disconnectRequestHpai, uint8_t * txBuffer, uint16_t * txLength)
{
    uint8_t txBytes = 0;

    /* Communication Channel ID */
    txBuffer[txBytes++] = KNXnetIP_Channel[0].ChannelId;

    /* reserved */
    txBuffer[txBytes++] = 0x00U;

    /* HPAI Control endpoint - Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* HPAI Control endpoint - Host Protocol Code */
    txBuffer[txBytes++] = disconnectRequestHpai->HostProtocolCode;

    /* HPAI Control endpoint - IP Address */
    txBuffer[txBytes++] = (uint8_t)((disconnectRequestHpai->ipAddress >> 24) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((disconnectRequestHpai->ipAddress >> 16) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((disconnectRequestHpai->ipAddress >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(disconnectRequestHpai->ipAddress & 0xFFU);

    /* HPAI Control endpoint - Port Number */
    txBuffer[txBytes++] = (uint8_t)((disconnectRequestHpai->portNumber >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(disconnectRequestHpai->portNumber & 0xFFU);

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_DisconnectResponse(uint8_t * txBuffer, uint16_t * txLength)
{
    uint8_t txBytes = 0;

    /* Communication Channel ID */
    txBuffer[txBytes++] = KNXnetIP_Channel[0].ChannelId;

    /* Status Code */
    txBuffer[txBytes++] = E_NO_ERROR;

    /* Update Tx Length */
    *txLength = txBytes;
}

/*==================[end of file]===========================================*/
