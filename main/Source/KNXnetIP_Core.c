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
#include "KNXnetIP_Types.h"

/*==================[macros]================================================*/
#define IP_ADDRESS(x,y,z,t) (uint32_t)((((uint32_t)x << 24) & 0xFF000000) | \
                                       (((uint32_t)y << 16) & 0xFF0000) | \
                                       (((uint32_t)z << 8) & 0xFF00) | \
                                       ((uint32_t)t & 0xFF))
/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/
static const KNXnetIP_ServiceFamilyType KnxSupportedServiceFamilies[] = {
    {KNXNETIP_CORE,             KNXNETIP_VERSION_10},
    {KNXNETIP_DEVICEMANAGEMENT, KNXNETIP_VERSION_10},
    {KNXNETIP_TUNNELLING,       KNXNETIP_VERSION_10}
};

static const uint8_t KnxDeviceFriendlyName[30] = {'A','T','I','O','S','-','K','N','X',' ','I','N','T','E','R','F','A','C','E','','','','','','','','','','',''};
static const uint8_t KnxDeviceSerialNumber[6] = {0x00, 0x01, 0x11, 0x11, 0x11, 0x11};
static const uint8_t KnxDeviceMACAddress[6] = {0x45, 0x49, 0x42, 0x6E, 0x65, 0x74};
/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

void KNXnetIP_SearchResponse(KNXnetIP_HPAIType * searchRequestHpai, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_DescriptionResponse(KNXnetIP_HPAIType * searchRequestHpai, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_ConnectResponse(KNXnetIP_HPAIType * searchRequestHpai, uint8_t * txBuffer, uint16_t * txLength);

//void KNXnetIP_ConnectionStateResponse(void);
//void KNXnetIP_DisconnectRequest(void);
//void KNXnetIP_DisconnectResponse(void);

void KNXnetIP_SearchResponse(KNXnetIP_HPAIType * searchRequestHpai, uint8_t * txBuffer, uint16_t * txLength)
{
    uint16_t txBytes = 0;

    /* HPAI - Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* HPAI - Host Protocol Code */
    txBuffer[txBytes++] = IPV4_UDP;

    /* HPAI - IP Address of control endpoint */
    txBuffer[txBytes++] = (uint8_t)((searchRequestHpai->ipAddress >> 24) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((searchRequestHpai->ipAddress >> 16) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)((searchRequestHpai->ipAddress >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(searchRequestHpai->ipAddress & 0xFFU);

    /* HPAI - Port Number of control endpoint */
    txBuffer[txBytes++] = (uint8_t)((searchRequestHpai->portNumber >> 8) & 0xFFU);
    txBuffer[txBytes++] = (uint8_t)(searchRequestHpai->portNumber & 0xFFU);

    /* DIB Device Hardware - Structure Length */
    txBuffer[txBytes++] = 0x36U;

    /* DIB Device Hardware - Description Type Code */
    txBuffer[txBytes++] = DEVICE_INFO;

    /* DIB Device Hardware - KNX Medium */
    txBuffer[txBytes++] = KNX_IP;

    /* DIB Device Hardware - Device Status */
    txBuffer[txBytes++] = 0x01U; /* Programming mode */

    /* DIB Device Hardware - Individual Address */
    txBuffer[txBytes++] = 0x10U;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Device Hardware - Project-InstallationID */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x11U;

    /* DIB Device Hardware - Serial Number */
    memcpy(&txBuffer[txBytes], &KnxDeviceSerialNumber[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Routing Multicast Address */
    memcpy(&txBuffer[txBytes], &IP_ADDRESS(224, 0, 23, 12), 4U);
    txBytes += 4;

    /* DIB Device Hardware - MAC Address */
    memcpy(&txBuffer[txBytes], &KnxDeviceMACAddress[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Friendly Name */
    memcpy(&txBuffer[txBytes], &KnxDeviceFriendlyName[0], 30);
    txBytes += 30;

    /* DIB Supported Service Family - Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* DIB Supported Service Family - Description Type Code */
    txBuffer[txBytes++] = SUPP_SVC_FAMILIES;

    /* DIB Supported Service Family - Supported Service Families */
    memcpy(&txBuffer[txBytes], &KnxSupportedServiceFamilies[0], 6U);
    txBytes += 6;

    *txLength = txBytes;
}

void KNXnetIP_DescriptionResponse(KNXnetIP_HPAIType * searchRequestHpai, uint8_t * txBuffer, uint16_t * txLength)
{
    uint16_t txBytes = 0;

    /* DIB Device Hardware - Structure Length */
    txBuffer[txBytes++] = 0x36U;

    /* DIB Device Hardware - Description Type Code */
    txBuffer[txBytes++] = DEVICE_INFO;

    /* DIB Device Hardware - KNX Medium */
    txBuffer[txBytes++] = KNX_IP;

    /* DIB Device Hardware - Device Status */
    txBuffer[txBytes++] = 0x01U; /* Programming mode */

    /* DIB Device Hardware - Individual Address */
    txBuffer[txBytes++] = 0x10U;
    txBuffer[txBytes++] = 0x00U;

    /* DIB Device Hardware - Project-InstallationID */
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = 0x11U;

    /* DIB Device Hardware - Serial Number */
    memcpy(&txBuffer[txBytes], &KnxDeviceSerialNumber[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Routing Multicast Address */
    memcpy(&txBuffer[txBytes], &IP_ADDRESS(224, 0, 23, 12), 4U);
    txBytes += 4;

    /* DIB Device Hardware - MAC Address */
    memcpy(&txBuffer[txBytes], &KnxDeviceMACAddress[0], 6U);
    txBytes += 6;

    /* DIB Device Hardware - Friendly Name */
    memcpy(&txBuffer[txBytes], &KnxDeviceFriendlyName[0], 30);
    txBytes += 30;

    /* DIB Supported Service Family - Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* DIB Supported Service Family - Description Type Code */
    txBuffer[txBytes++] = SUPP_SVC_FAMILIES;

    /* DIB Supported Service Family - Supported Service Families */
    memcpy(&txBuffer[txBytes], &KnxSupportedServiceFamilies[0], 6U);
    txBytes += 6;

    /* DIB Other Device Information - Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* DIB Other Device Information - Description Type Code */
    txBuffer[txBytes++] = MFR_DATA;

    /* DIB Other Device Information - KNX Manufacturer ID */
    txBuffer[txBytes++] = 0xBBU;
    txBuffer[txBytes++] = 0xEEU;

    /* DIB Other Device Information - Device Type Name */
    txBuffer[txBytes++] = 'N';
    txBuffer[txBytes++] = '1';
    txBuffer[txBytes++] = '4';
    txBuffer[txBytes++] = '6';

    *txLength = txBytes;
}

void KNXnetIP_ConnectResponse(KNXnetIP_HPAIType * searchRequestHpai, uint8_t * txBuffer, uint16_t * txLength)
{
    uint16_t txBytes = 0;

    /* Communication Channel ID */
    txBuffer[txBytes++] = 21U;

    /* Status Code */
    txBuffer[txBytes++] = E_NO_ERROR;

    /* Structure Length */
    txBuffer[txBytes++] = 0x08U;

    /* Host Protocol Code */
    txBuffer[txBytes++] = IPV4_UDP;

    /* IP Address of data endpoint */
    txBuffer[txBytes++] =

    txBuffer[txBytes++] =
    
}

/*==================[end of file]===========================================*/
