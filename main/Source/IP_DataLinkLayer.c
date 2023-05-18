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
#define KNXNETIP_DEBUG_LOGGING

bool KNXnetIP_Connected = false;
uint16_t KNXnetIP_ConnectionPort = 0;

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/
uint8_t IP_TxBuffer[256];
uint8_t IP_RxBuffer[128];

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
            uint16_t txLength = 0;

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
#ifdef KNXNETIP_DEBUG_LOGGING
                ESP_LOGE("IP", "L_Data_Ind::E_HOST_PROTOCOL_TYPE::0x%X", cemiFrame.ServiceType);
#endif
            }
            else
            {
                cemiFrame.HeaderSize = HEADER_SIZE_10;
                cemiFrame.ProtocolVersion = KNXNETIP_VERSION_10;
                KNXnetIP_FeatureIdentifierType featureIdentifer;

                switch (cemiFrame.ServiceType)
                {
                    case SEARCH_REQUEST:
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP", "L_Data_Ind::SEARCH_REQUEST");
#endif
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
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP", "L_Data_Ind::SEARCH_REQUEST_EXTENDED");
#endif
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
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP", "L_Data_Ind::DESCRIPTION_REQUEST");
#endif
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
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP", "L_Data_Ind::CONNECT_REQUEST");
#endif
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

                        KNXnetIP_Connected = true;

                        break;

                    case CONNECTIONSTATE_REQUEST:
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP", "L_Data_Ind::CONNECTIONSTATE_REQUEST");
#endif
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
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP", "L_Data_Ind::DISCONNECT_REQUEST");
#endif
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

                        KNXnetIP_Connected = false;

                        break;

                    case DISCONNECT_RESPONSE:
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP", "L_Data_Ind::DISCONNECT_RESPONSE");
#endif
                        break;

                    case TUNNELLING_REQUEST:
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP","L_Data_Ind::TUNNELLING_REQUEST");
#endif
                        /* Copy L-PDU into Rx Buffer */
                        memcpy(&IP_RxBuffer[0], &pduInfoPtr->SduDataPtr[HEADER_SIZE_10 + CONNECTION_HEADER_SIZE], pduInfoPtr->SduLength - (HEADER_SIZE_10+CONNECTION_HEADER_SIZE));

#ifdef KNXNETIP_DEBUG_LOGGING
                        for (uint8_t rxIndex = 0; rxIndex < pduInfoPtr->SduLength - (HEADER_SIZE_10+CONNECTION_HEADER_SIZE); rxIndex++)
                        {
                            if (IP_RxBuffer[rxIndex] < 0x10U)
                            {
                                printf("0%X ", IP_RxBuffer[rxIndex]);
                            }
                            else
                            {
                                printf("%X ", IP_RxBuffer[rxIndex]);
                            }
                        }
                        printf("\n ");
#endif

                        /* Gateway to TP-UART2 Interface */
                        KNXnetIP_TunnelIP2TP(&IP_RxBuffer[0], pduInfoPtr->SduLength - (HEADER_SIZE_10 + CONNECTION_HEADER_SIZE));

                        if (IPV4_UDP == dataIndHpai.HostProtocolCode)
                        {
                            KNXnetIP_TunnellingAck(&IP_TxBuffer[HEADER_SIZE_10], &txLength);

                            /* Construct ack frame header */
                            cemiFrame.ServiceType = TUNNELLING_ACK;
                            cemiFrame.TotalLength = HEADER_SIZE_10 + txLength;

                            IP_TxBuffer[0] = cemiFrame.HeaderSize;
                            IP_TxBuffer[1] = cemiFrame.ProtocolVersion;
                            IP_TxBuffer[2] = (uint8_t)((cemiFrame.ServiceType & 0xFF00) >> 8);
                            IP_TxBuffer[3] = cemiFrame.ServiceType & 0xFFU;
                            IP_TxBuffer[4] = (uint8_t)((cemiFrame.TotalLength & 0xFF00) >> 8);
                            IP_TxBuffer[5] = cemiFrame.TotalLength & 0xFFU;
                        }
                        else
                        {
                            /* For TCP communication, no TUNNELLING_ACK shall be sent. */
                        }

                        break;

                    case TUNNELLING_FEATURE_GET:
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP","L_Data_Ind::TUNNELLING_FEATURE_GET");
#endif
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
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP","L_Data_Ind::TUNNELLING_FEATURE_SET");
#endif
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
#ifdef KNXNETIP_DEBUG_LOGGING
                        ESP_LOGI("IP", "L_Data_Ind::UNSUPPORTED_SERVICE");
                        ESP_LOGI("IP", "Service: 0x%X",cemiFrame.ServiceType);
#endif
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
