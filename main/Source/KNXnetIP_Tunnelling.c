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

#include "Knx_Types.h"
#include "IP_DataLinkLayer.h"
#include "TP_DataLinkLayer.h"
#include "TpUart2_DataLinkLayer.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/
void KNXnetIP_TunnellingFeatureService(KNXnetIP_FeatureIdentifierType featureIdentifier);

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
    KNXnetIP_TunnellingFeature.BusStatus = false;
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

void KNXnetIP_TunnellingFeatureService(KNXnetIP_FeatureIdentifierType featureIdentifier)
{
    uint16_t featureValue = 0;

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

        default:
            /* Unsupported feature identifier. Shouldn't get here. */
            break;
    }
}
/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/
