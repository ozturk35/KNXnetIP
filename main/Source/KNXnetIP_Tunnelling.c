/**
 * \file KNXnetIP_Tunnelling.c
 * 
 * \brief KNXnet/IP Tunnelling Services
 * 
 * This file contains the implementation of KNXnet/IP Tunnelling Services
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

#include "IP_DataLinkLayer.h"
#include "TP_DataLinkLayer.h"
#include "TpUart2_DataLinkLayer.h"

#include "KNXnetIP.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/
void KNXnetIP_TunnellingInit(void);
void KNXnetIP_TunnellingFeatureGet(KNXnetIP_FeatureIdentifierType featureIdentifier, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_TunnellingFeatureSet(KNXnetIP_FeatureIdentifierType featureIdentifier, uint16_t value, uint8_t * txBuffer, uint16_t * txLength);
void KNXnetIP_TunnellingRequest(uint8_t * bufferPtr, uint16_t length);

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/
KNXnetIP_TunnellingFeatureType KNXnetIP_TunnellingFeature;

/*==================[external function definitions]=========================*/
void KNXnetIP_TunnellingInit(void)
{
    KNXnetIP_TunnellingFeature.SupportedEMIType = CEMI_ONLY;
    KNXnetIP_TunnellingFeature.DeviceDescType0 = KNXNETIP_SYS7;
    KNXnetIP_TunnellingFeature.BusStatus = true;
    KNXnetIP_TunnellingFeature.ManufacturerCode = KNX_MANUFACTURER_CODE_MDT;
    KNXnetIP_TunnellingFeature.ActiveEMIType = CEMI;
}

void KNXnetIP_TunnellingAck(uint8_t * txBuffer, uint16_t * txLength)
{
    uint8_t txBytes = 0;
    KNXnetIP_ErrorCodeType errorCode = E_NO_ERROR;

    txBuffer[txBytes++] = 0x04U;
    txBuffer[txBytes++] = KNXnetIP_Channel[0].ChannelId;
    txBuffer[txBytes++] = 0x00U;
    txBuffer[txBytes++] = errorCode;

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_TunnelIP2TP(uint8_t * rxBufferPtr, uint8_t rxLength)
{
    TP_GW_L_Data_Req(rxBufferPtr, rxLength);
}

void KNXnetIP_TunnellingFeatureGet(KNXnetIP_FeatureIdentifierType featureIdentifier, uint8_t * txBuffer, uint16_t * txLength)
{
    uint16_t featureValue = 0;
    uint16_t txBytes = 0;

    /* Connection Header - Structure Length */
    txBuffer[txBytes++] = 0x04U;

    /* Connection Header - Channel */
    txBuffer[txBytes++] = KNXnetIP_Channel[0].ChannelId;

    /* Connection Header - Sequence Counter */
    txBuffer[txBytes++] = 0x00U;

    /* Connection Header - reserved */
    txBuffer[txBytes++] = 0x00U;

    /* Tunneling Feature Identifier */
    txBuffer[txBytes++] = featureIdentifier;

    /* Status */
    txBuffer[txBytes++] = 0x00; /* OK */

    /* Feature Value */
    switch (featureIdentifier)
    {
        case SUPPORTED_EMI_TYPE:
            featureValue = (uint16_t)KNXnetIP_TunnellingFeature.SupportedEMIType;
            break;

        case HOST_DEVICE_DESC_TYPE_0:
            featureValue = (uint16_t)KNXnetIP_TunnellingFeature.DeviceDescType0;
            break;

        case BUS_CONNECTION_STATUS:
            featureValue = (uint16_t)KNXnetIP_TunnellingFeature.BusStatus;
            break;

        case KNX_MANUFACTURER_CODE:
            featureValue = (uint16_t)KNXnetIP_TunnellingFeature.ManufacturerCode;
            break;

        case ACTIVE_EMI_TYPE:
            featureValue = (uint16_t)KNXnetIP_TunnellingFeature.ActiveEMIType;
            break;

        case INFO_SERVICE_EN:
            featureValue = (uint16_t)KNXnetIP_TunnellingFeature.InfoServiceEnable;

        default:
            /* Unsupported feature identifier. Shouldn't get here. */
            break;
    }

    txBuffer[txBytes++] = featureValue;

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_TunnellingFeatureSet(KNXnetIP_FeatureIdentifierType featureIdentifier, uint16_t value, uint8_t * txBuffer, uint16_t * txLength)
{
    uint16_t txBytes = 0;

    /* Connection Header - Structure Length */
    txBuffer[txBytes++] = 0x04U;

    /* Connection Header - Channel */
    txBuffer[txBytes++] = KNXnetIP_Channel[0].ChannelId;

    /* Connection Header - Sequence Counter */
    txBuffer[txBytes++] = 0x00U;

    /* Connection Header - reserved */
    txBuffer[txBytes++] = 0x00U;

    /* Tunneling Feature Identifier */
    txBuffer[txBytes++] = featureIdentifier;

    /* Status */
    txBuffer[txBytes++] = 0x00; /* OK */

    /* Feature Value */
    switch (featureIdentifier)
    {
        case SUPPORTED_EMI_TYPE:
            KNXnetIP_TunnellingFeature.SupportedEMIType = value;
            break;

        case HOST_DEVICE_DESC_TYPE_0:
            KNXnetIP_TunnellingFeature.DeviceDescType0 = value;
            break;

        case BUS_CONNECTION_STATUS:
            KNXnetIP_TunnellingFeature.BusStatus = value;
            break;

        case KNX_MANUFACTURER_CODE:
            KNXnetIP_TunnellingFeature.ManufacturerCode = value;
            break;

        case ACTIVE_EMI_TYPE:
            KNXnetIP_TunnellingFeature.ActiveEMIType = value;
            break;

        case INFO_SERVICE_EN:
            KNXnetIP_TunnellingFeature.InfoServiceEnable = value;

        default:
            /* Unsupported feature identifier. Shouldn't get here. */
            break;
    }

    txBuffer[txBytes++] = value;

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_TunnellingRequest(uint8_t * bufferPtr, uint16_t length)
{
    KNXnetIP_ServiceType serviceType = TUNNELLING_REQUEST;
    uint16_t txLength = HEADER_SIZE_10 + CONNECTION_HEADER_SIZE + length;

    IP_TxBuffer[0] = HEADER_SIZE_10;
    IP_TxBuffer[1] = KNXNETIP_VERSION_10;
    IP_TxBuffer[2] = (uint8_t)((serviceType & 0xFF00) >> 8);
    IP_TxBuffer[3] = serviceType & 0xFFU;
    IP_TxBuffer[4] = (uint8_t)((txLength & 0xFF00) >> 8);
    IP_TxBuffer[5] = (txLength & 0xFFU);
    IP_TxBuffer[6] = 0x04U;
    IP_TxBuffer[7] = KNXnetIP_Channel[0].ChannelId;
    IP_TxBuffer[8] = 0x00U;
    IP_TxBuffer[9] = 0x00U;

    memcpy(&IP_TxBuffer[10], bufferPtr, length);

    KNXnetIP_TcpUpdateTxBuffer(&IP_TxBuffer[0], HEADER_SIZE_10 + CONNECTION_HEADER_SIZE + length);

    tcp_transmitPendingTunnelReq(KNXnetIP_TcpSock, KNXnetIP_TcpIpAddr, UDP_PORT);
}

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/
