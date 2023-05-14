/**
 * \file IP_DataLinkLayer.c
 * 
 * \brief KNXnet/IP Data Link Layer
 * 
 * This file contains the implementation of KNXnet/IP Data Link Layer
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

#include "Pdu.h"
#include "KNXnetIP.h"

#include "TP_DataLinkLayer.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/
uint8_t IP_TxBuffer[256];
// static uint8_t IP_RxBuffer[256];

// static uint8_t KNXnetIP_SequenceNumber = 0U;

/*==================[external function definitions]=========================*/

void IP_L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint16_t octetCount, PriorityType priority, uint16_t sourceAddr);
void IP_L_Data_Ind(PduInfoType * pduInfoPtr, uint32_t ipAddr, uint16_t port);

void IP_L_Data_Ind(PduInfoType * pduInfoPtr, uint32_t ipAddr, uint16_t port)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("IP","L_Data_Ind: ERR_NULL_PTR");
    }
    else
    {
        KNXnetIP_FrameType cemiFrame;
        KNXnetIP_ErrorCodeType errorCode = E_NO_ERROR;
        uint8_t rxIndex = HEADER_SIZE_10;

        /* Decode KNXnet/IP header */
        cemiFrame.HeaderSize = pduInfoPtr->SduDataPtr[0];
        cemiFrame.ProtocolVersion = pduInfoPtr->SduDataPtr[1];
        cemiFrame.ServiceType = ((uint16_t)(pduInfoPtr->SduDataPtr[2]) << 8U) & 0xFF00U;
        cemiFrame.ServiceType |= pduInfoPtr->SduDataPtr[3] & 0xFFU;

        if (HEADER_SIZE_10 != cemiFrame.HeaderSize)
        {
            errorCode = E_ERROR;
        }
        else if (KNXNETIP_VERSION_10 != cemiFrame.ProtocolVersion)
        {
            errorCode = E_VERSION_NOT_SUPPORTED;
        }
        else
        {
            KNXnetIP_HPAIType dataIndHpai;
            uint16_t txLength;

            /* Decode Control Endpoint HPAI  */
//            dataIndHpai.StructureLength = pduInfoPtr->SduDataPtr[6];

            if ((DISCONNECT_REQUEST == cemiFrame.ServiceType) || (CONNECTIONSTATE_REQUEST == cemiFrame.ServiceType))
            {
                rxIndex += 3;
            }
            else
            {
                rxIndex++;
            }

            dataIndHpai.HostProtocolCode = pduInfoPtr->SduDataPtr[rxIndex++];
            dataIndHpai.ipAddress  = (uint32_t)(pduInfoPtr->SduDataPtr[rxIndex++]) << 24U;
            dataIndHpai.ipAddress |= (uint32_t)(pduInfoPtr->SduDataPtr[rxIndex++]) << 16U;
            dataIndHpai.ipAddress |= (uint32_t)(pduInfoPtr->SduDataPtr[rxIndex++]) << 8U;
            dataIndHpai.ipAddress |= (uint32_t)(pduInfoPtr->SduDataPtr[rxIndex++]);
            dataIndHpai.portNumber = (uint32_t)(pduInfoPtr->SduDataPtr[rxIndex++]) << 8U;
            dataIndHpai.portNumber |= (uint32_t)(pduInfoPtr->SduDataPtr[rxIndex++]);

            if ((IPV4_UDP != dataIndHpai.HostProtocolCode) && 
                (IPV4_TCP != dataIndHpai.HostProtocolCode))
            {
                errorCode = E_HOST_PROTOCOL_TYPE;
                ESP_LOGE("IP", "L_Data_Ind::E_HOST_PROTOCOL_TYPE::0x%X", cemiFrame.ServiceType);
            }
            else
            {
                cemiFrame.HeaderSize = HEADER_SIZE_10;
                cemiFrame.ProtocolVersion = KNXNETIP_VERSION_10;
                KNXnetIP_FeatureIdentifierType featureIdentifer;

                switch (cemiFrame.ServiceType)
                {
                    case SEARCH_REQUEST:
//                        ESP_LOGI("IP", "L_Data_Ind::SEARCH_REQUEST");

                        KNXnetIP_SearchResponse(&IP_TxBuffer[HEADER_SIZE_10], &txLength);

                        /* Construct frame header */
                        cemiFrame.ServiceType = SEARCH_RESPONSE;
                        cemiFrame.TotalLength = HEADER_SIZE_10 + txLength;

                        IP_TxBuffer[0] = cemiFrame.HeaderSize;
                        IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                        IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                        IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                        IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                        IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;

                        break;

                    case SEARCH_REQUEST_EXTENDED:
//                        ESP_LOGI("IP", "L_Data_Ind::SEARCH_REQUEST_EXTENDED");

                        KNXnetIP_SearchResponseExtended(&IP_TxBuffer[HEADER_SIZE_10], &txLength);

                         /* Construct frame header */
                        cemiFrame.ServiceType = SEARCH_RESPONSE_EXTENDED;
                        cemiFrame.TotalLength = HEADER_SIZE_10 + txLength;

                        IP_TxBuffer[0] = cemiFrame.HeaderSize;
                        IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                        IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                        IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                        IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                        IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;

                        break;
                    
                    case DESCRIPTION_REQUEST:
                        ESP_LOGI("IP", "L_Data_Ind::DESCRIPTION_REQUEST");

                        KNXnetIP_DescriptionResponse(&IP_TxBuffer[HEADER_SIZE_10], &txLength);

                        /* Construct frame header */
                        cemiFrame.ServiceType = DESCRIPTION_RESPONSE;
                        cemiFrame.TotalLength = HEADER_SIZE_10 + txLength;

                        IP_TxBuffer[0] = cemiFrame.HeaderSize;
                        IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                        IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                        IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                        IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                        IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;

                        break;

                    case CONNECT_REQUEST:
                        ESP_LOGI("IP", "L_Data_Ind::CONNECT_REQUEST");

                        KNXnetIP_CRIType cri;

                        cri.ConnectionTypeCode = pduInfoPtr->SduDataPtr[23];
                        KNXnetIP_ConnectResponse(errorCode, &dataIndHpai, &cri, &IP_TxBuffer[HEADER_SIZE_10], &txLength);

                        /* Construct frame header */
                        cemiFrame.ServiceType = CONNECT_RESPONSE;
                        cemiFrame.TotalLength = HEADER_SIZE_10 + txLength;

                        IP_TxBuffer[0] = cemiFrame.HeaderSize;
                        IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                        IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                        IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                        IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                        IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;

                        break;

                    case CONNECTIONSTATE_REQUEST:
                        ESP_LOGI("IP", "L_Data_Ind::CONNECTIONSTATE_REQUEST");

                        KNXnetIP_ConnectionStateResponse(errorCode, &IP_TxBuffer[0], &txLength);

                        /* Construct frame header */
                        cemiFrame.ServiceType = CONNECTIONSTATE_RESPONSE;
                        cemiFrame.TotalLength = HEADER_SIZE_10 + txLength;

                        IP_TxBuffer[0] = cemiFrame.HeaderSize;
                        IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                        IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                        IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                        IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                        IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;

                        break;

                    case DISCONNECT_REQUEST:
                        ESP_LOGI("IP", "L_Data_Ind::DISCONNECT_REQUEST");

                        KNXnetIP_DisconnectResponse(&IP_TxBuffer[HEADER_SIZE_10], &txLength);

                        /* Construct frame header */
                        cemiFrame.ServiceType = DISCONNECT_RESPONSE;
                        cemiFrame.TotalLength = HEADER_SIZE_10 + txLength;

                        IP_TxBuffer[0] = cemiFrame.HeaderSize;
                        IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                        IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                        IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                        IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                        IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;

                        break;

                    case DISCONNECT_RESPONSE:
                        break;

                    case TUNNELLING_REQUEST:
                        ESP_LOGI("IP","L_Data_Ind::TUNNELLING_REQUEST");

                        // KNXnetIP_TunnellingConnectionHeaderType connectionHeader;
                        // KNXnetIP_TunnelingCemiFrameType cemiFrame;

                        // connectionHeader.StructureLength        = pduInfoPtr->SduDataPtr[6];
                        // connectionHeader.CommunicationChannelId = pduInfoPtr->SduDataPtr[7];
                        // connectionHeader.SequenceCounter        = pduInfoPtr->SduDataPtr[8];

                        // if ((KNXnetIP_SequenceNumber == connectionHeader.SequenceCounter) ||
                        //     (((KNXnetIP_SequenceNumber - 1) & 0xFU) == connectionHeader.SequenceCounter))
                        // {

                        // }
                        // else
                        // {

                        // }

                        // cemiFrame.MessageCode           = pduInfoPtr->SduDataPtr[10];
                        // cemiFrame.AdditionalInfoLength  = pduInfoPtr->SduDataPtr[11];

                        // /* Copy L-PDU into Rx buffer */
                        // memcpy(&IP_RxBuffer[0], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));

                        // /* Check additional information */
                        // if (0U == cemiFrame.AdditionalInfoLength)
                        // {
                        //     cemiFrame.ServiceInformation = &(pduInfoPtr->SduDataPtr[12]);
                        // }
                        // else
                        // {
                        //     cemiFrame.AdditionalInformation = &(pduInfoPtr->SduDataPtr[12]);
                        //     cemiFrame.ServiceInformation    = &(pduInfoPtr->SduDataPtr[13]);
                        // }

                //         /* Gateway to TP-UART2 Interface */
                //         errorCode = KNXnetIP_TunnelIP2TP(&IP_RxBuffer[0], rxLength);

                //         /* Construct ack frame header */
                //         cemiFrame.ServiceType = TUNNELLING_ACK;
                //         cemiFrame.TotalLength = txLength;

                //         IP_TxBuffer[0] = cemiFrame.HeaderSize;
                //         IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                //         IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                //         IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                //         IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                //         IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;

                //         KNXnetIP_UDPSend(dataIndHpai.ipAddress, UDP_PORT, &IP_TxBuffer[0], cemiFrame.TotalLength);

                        break;

                    case TUNNELLING_FEATURE_GET:
                        ESP_LOGI("IP","L_Data_Ind::TUNNELLING_FEATURE_GET");
                        featureIdentifer = pduInfoPtr->SduDataPtr[10];

                        KNXnetIP_TunnellingFeatureGet(featureIdentifer, &IP_TxBuffer[HEADER_SIZE_10], &txLength);

                        /* Construct frame header */
                        cemiFrame.ServiceType = TUNNELLING_FEATURE_RESPONSE;
                        cemiFrame.TotalLength = HEADER_SIZE_10 + txLength;

                        IP_TxBuffer[0] = cemiFrame.HeaderSize;
                        IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                        IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                        IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                        IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                        IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;

                        break;

                    case TUNNELLING_FEATURE_SET:
                        ESP_LOGI("IP","L_Data_Ind::TUNNELLING_FEATURE_SET");
                        featureIdentifer = pduInfoPtr->SduDataPtr[10];
                        uint16_t value = pduInfoPtr->SduDataPtr[12];

                        KNXnetIP_TunnellingFeatureSet(featureIdentifer, value, &IP_TxBuffer[HEADER_SIZE_10], &txLength);

                        /* Construct frame header */
                        cemiFrame.ServiceType = TUNNELLING_FEATURE_RESPONSE;
                        cemiFrame.TotalLength = HEADER_SIZE_10 + txLength;

                        IP_TxBuffer[0] = cemiFrame.HeaderSize;
                        IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                        IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                        IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                        IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                        IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;

                        break;

                    default:
                    /* Unsupported service */
                        ESP_LOGI("IP", "L_Data_Ind::UNKNOWN_SERVICE");
                        ESP_LOGI("IP", "Service: 0x%X",cemiFrame.ServiceType);

                        break;
                }

                if (txLength > 0)
                {
                    if (IPV4_UDP == dataIndHpai.HostProtocolCode)
                    {
                        KNXnetIP_UDPSend(ipAddr, port, &IP_TxBuffer[0], cemiFrame.TotalLength);
                    }
                    else
                    {
                        KNXnetIP_TcpUpdateTxBuffer(&IP_TxBuffer[0], cemiFrame.TotalLength);
                    }
                }
                else
                {
                    /* No data to be sent! */
                }
            }
        }
    }

//    (void)ack;
//    (void)addrType;
//    (void)destAddr;
//    (void)frameFormat;
//    (void)octetCount;
//    (void)priority;
//    (void)sourceAddr;
}

void IP_L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint16_t octetCount, PriorityType priority, uint16_t sourceAddr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("IP","L_Data_Ind: ERR_NULL_PTR");
    }
    else
    {

    }
//    (void)ack;
//    (void)addrType;
//    (void)destAddr;
//    (void)frameFormat;
//    (void)octetCount;
//    (void)priority;
//    (void)sourceAddr;
}
