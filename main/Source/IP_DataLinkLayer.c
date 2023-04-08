#include "Pdu.h"
#include "KNXnetIP_Core.h"
#include "Knx_Types.h"
#include "KNXnetIP_Types.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "esp_log.h"


static uint8_t IP_TxBuffer[512];
static uint8_t IP_RxBuffer[512];

void IP_L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint16_t octetCount, PriorityType priority, uint16_t sourceAddr);
void IP_L_Data_Ind(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint16_t octetCount, PriorityType priority, uint16_t sourceAddr);

void IP_L_Data_Ind(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint16_t octetCount, PriorityType priority, uint16_t sourceAddr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("IP","L_Data_Ind: ERR_NULL_PTR");
    }
    else
    {
        KNXnetIP_FrameType cemiFrame;
        KNXnetIP_ErrorCodeType errorCode = E_NO_ERROR;

        /* Decode KNXnet/IP header */
        cemiFrame.HeaderSize = pduInfoPtr->SduDataPtr[0];
        cemiFrame.ProtocolVersion = pduInfoPtr->SduDataPtr[1];
        cemiFrame.ServiceType = ((uint16_t)(pduInfoPtr->SduDataPtr[2]) << 8U) & 0xFF00U;
        cemiFrame.ServiceType |= pduInfoPtr->SduDataPtr[3] & 0xFFU;
        cemiFrame.TotalLength = ((uint16_t)(pduInfoPtr->SduDataPtr[4]) << 8U) & 0xFF00U;
        cemiFrame.TotalLength |= pduInfoPtr->SduDataPtr[5] & 0xFFU;
        cemiFrame.Data = &pduInfoPtr->SduDataPtr[6];

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

            /* Decode HPAI from SEARCH_REQUEST frame */
            dataIndHpai.StructureLength = pduInfoPtr->SduDataPtr[6];
            dataIndHpai.HostProtocolCode = pduInfoPtr->SduDataPtr[7];
            /* Decode IP address */
            dataIndHpai.ipAddress  = (uint32_t)(pduInfoPtr->SduDataPtr[8]) << 24U;
            dataIndHpai.ipAddress |= (uint32_t)(pduInfoPtr->SduDataPtr[9]) << 16U;
            dataIndHpai.ipAddress |= (uint32_t)(pduInfoPtr->SduDataPtr[10]) << 8U;
            dataIndHpai.ipAddress |= (uint32_t)(pduInfoPtr->SduDataPtr[11]);
            /* Decode port number */
            dataIndHpai.portNumber  = (uint16_t)(pduInfoPtr->SduDataPtr[12]) << 8U;
            dataIndHpai.portNumber |= (uint16_t)(pduInfoPtr->SduDataPtr[13]);
            

            if (IPV4_UDP != dataIndHpai.HostProtocolCode)
            {
                errorCode = E_HOST_PROTOCOL_TYPE;
            }
            else
            {
                cemiFrame.HeaderSize = HEADER_SIZE_10;
                cemiFrame.ProtocolVersion = KNXNETIP_VERSION_10;

                switch (cemiFrame.ServiceType)
                {
                    case SEARCH_REQUEST:
                        KNXnetIP_SearchResponse(dataIndHpai, &IP_TxBuffer, &txLength);

                        /* Construct frame header */

                        cemiFrame.ServiceType = SEARCH_RESPONSE;
                        cemiFrame.TotalLength = txLength;

                        KNXnetIP_UnicastSend(dataIndHpai.ipAddress, dataIndHpai.portNumber, &IP_TxBuffer, HEADER_SIZE_10 + cemiFrame.TotalLength);
                        break;

                    case DESCRIPTION_REQUEST:
                        KNXnetIP_DescriptionResponse(dataIndHpai, &IP_TxBuffer, &txLength);

                        /* Construct frame header */
                        cemiFrame.HeaderSize = HEADER_SIZE_10;
                        cemiFrame.ProtocolVersion = KNXNETIP_VERSION_10;
                        cemiFrame.ServiceType = DESCRIPTION_RESPONSE;
                        cemiFrame.TotalLength = txLength;

                        KNXnetIP_UnicastSend(dataIndHpai.ipAddress, dataIndHpai.portNumber, &IP_TxBuffer, HEADER_SIZE_10 + cemiFrame.TotalLength);
                        break;
                    
                    case CONNECT_REQUEST:
                        KNXnetIP_ConnectResponse(dataIndHpai, &IP_TxBuffer, &txLength);

                        break;

                    case CONNECTIONSTATE_REQUEST:

                        break;
                    
                    case DISCONNECT_REQUEST:
                        break;
                    
                    case DISCONNECT_RESPONSE:
                        break;
                    
                    default:
                        break;
                }
            }
        }
    }

    (void)ack;
    (void)addrType;
    (void)destAddr;
    (void)frameFormat;
    (void)octetCount;
    (void)priority;
    (void)sourceAddr;
}

void IP_L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint16_t octetCount, PriorityType priority, uint16_t sourceAddr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("IP","L_Data_Ind: ERR_NULL_PTR");
    }
    else
    {
        KNXnetIP_FrameType cemiFrame;
        KNXnetIP_ErrorCodeType errorCode = E_NO_ERROR;

    }
    (void)ack;
    (void)addrType;
    (void)destAddr;
    (void)frameFormat;
    (void)octetCount;
    (void)priority;
    (void)sourceAddr;
}

void IP_DataLinkLayer_Main()
{
    KNXnetIP_FrameType cemiFrame;
    KNXnetIP_ErrorCodeType errorCode = E_NO_ERROR;

    uint16_t rxBytes = ReadBytesMulticast(&IP_RxBuffer, 512);

    /* Decode KNXnet/IP header */
    cemiFrame.HeaderSize = IP_RxBuffer[0];
    cemiFrame.ProtocolVersion = IP_RxBuffer[1];
    cemiFrame.ServiceType = ((uint16_t)(IP_RxBuffer[2]) << 8U) & 0xFF00U;
    cemiFrame.ServiceType |= IP_RxBuffer[3] & 0xFFU;
    cemiFrame.TotalLength = ((uint16_t)(IP_RxBuffer[4]) << 8U) & 0xFF00U;
    cemiFrame.TotalLength |= IP_RxBuffer[5] & 0xFFU;
    cemiFrame.Data = &IP_RxBuffer[6];

    if (rxBytes <= 0)
    {
        ESP_LOGI("IP","IP_DataLinkLayerMain: ERR_EMPTY_RXBYTES");
    }
    else if (HEADER_SIZE_10 != IP_RxBuffer[0])
    {
        errorCode = E_ERROR;
        ESP_LOGI("IP","IP_DataLinkLayerMain: ERR_INVALID_HEADER_SIZE");
    }
    else if (KNXNETIP_VERSION_10 != IP_RxBuffer[1])
    {
        errorCode = E_VERSION_NOT_SUPPORTED;
        ESP_LOGI("IP","IP_DataLinkLayerMain: ERR_INVALID_HEADER_SIZE");
    }
    else
    {
        switch (cemiFrame.ServiceType)
        {
            case SEARCH_REQUEST:
                break;

            case DESCRIPTION_REQUEST:
                break;

            case CONNECT_REQUEST:
                break;

            case CONNECTIONSTATE_REQUEST:
                break;

            case DISCONNECT_REQUEST:
                break;

            case DISCONNECT_RESPONSE:
                break;

            default:
                break;
        }
    }
}