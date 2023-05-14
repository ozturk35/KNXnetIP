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

void KNXnetIP_TunnellingAck(KNXnetIP_TunnellingConnectionHeaderType connectionHeader, KNXnetIP_TunnelingCemiFrameType cemiFrame, uint8_t * txBuffer, uint16_t * txLength)
{
    KNXnetIP_ServiceInformationType serviceInformation;
    (void)serviceInformation;

    switch(cemiFrame.MessageCode)
    {
        case L_data_req:
            /* Construct TpUart2 Frame */
            bool repeatFlag = 0U;
            uint16_t destAddr = 0U;
            AddressType addrType = 0U;
            PriorityType priority = 0U;
//            PduInfoType * pduInfoPtr;

            /* Set function parameters for TpUart2 Message */
            // repeatFlag = 0;

            /* Call TpUart2 L_Data_Req */
//            TpUart2_L_Data_Req(repeatFlag, destAddr, addrType, priority, pduInfoPtr);

            break;

        case M_PropRead_req:
            break;

        case M_PropWrite_req:
            break;

        case M_FuncPropCommand_req:
            break;

        case M_FuncPropStateRead_req:
            break;

        case M_Reset_req:
            break;

        default:
            /* Other message code types shall not be received. */
            break;
    }

    /* Create Tunnelling Ack frame */
    uint8_t txBytes = 0;
    KNXnetIP_ErrorCodeType errorCode = E_NO_ERROR;

    txBuffer[txBytes++] = connectionHeader.StructureLength;
    txBuffer[txBytes++] = connectionHeader.CommunicationChannelId;
    txBuffer[txBytes++] = connectionHeader.SequenceCounter;
    txBuffer[txBytes++] = errorCode;

    /* Update Tx Length */
    *txLength = txBytes;
}

void KNXnetIP_TunnelIP2TP(PduInfoType * pduInfoPtr)
{
//    TP_L_Data_Req();
}

void KNXnetIP_TunnelTP2IP(PduInfoType * pduInfoPtr)
{
//    IP_L_Data_Ind();
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

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/
