#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

#include "Pdu.h"
#include "KNXnetIP.h"
#include "TP_DataLinkLayer.h"
#include "TpUart2_DataLinkLayer.h"

static uint8_t L_TxBuffer[512];
static uint8_t L_RxBuffer[512];

void TP_GW_L_Data_Req(uint8_t * bufferPtr, uint8_t rxLength);
void TP_GW_L_Data_Ind(PduInfoType * pduInfoPtr);
void TP_GW_L_Data_Ind_ACK(PduInfoType * pduInfoPtr);

static uint8_t TP_L_Data_CalculateFCS(uint8_t * l_data, uint16_t length);

void TP_GW_L_Data_Req(uint8_t * bufferPtr, uint8_t rxLength)
{
    PduInfoType lpdu;
    uint8_t index = 0;

    /* Set CTRL field */
    L_TxBuffer[index++] = bufferPtr[CEMI_FRAME_CTRL1_FIELD_OFFSET];

    /* Set Source Address */
    L_TxBuffer[index++] = 0x11;
    L_TxBuffer[index++] = 0xFA;

    /* Set Destination Address */
    L_TxBuffer[index++] = bufferPtr[CEMI_FRAME_DA_HI_BYTE_OFFET];
    L_TxBuffer[index++] = bufferPtr[CEMI_FRAME_DA_LO_BYTE_OFFET];

    /* Set Length Field - AT, HC, LG */
    L_TxBuffer[index++] = bufferPtr[CEMI_FRAME_CTRL2_FIELD_OFFSET] | bufferPtr[CEMI_FRAME_LENGTH_FIELD_OFFSET];

    /* Copy Data into Tx Buffer */
    memcpy(&L_TxBuffer[index], &bufferPtr[CEMI_FRAME_TPDU_FIELD_OFFSET], bufferPtr[CEMI_FRAME_LENGTH_FIELD_OFFSET] + 1);
    index += bufferPtr[CEMI_FRAME_LENGTH_FIELD_OFFSET] + 1;

    /* Set FCS field */
    L_TxBuffer[index] = TP_L_Data_CalculateFCS(&L_TxBuffer[0], index);
    index++;

    lpdu.SduDataPtr = &L_TxBuffer[0];
    lpdu.SduLength = index;

    // ESP_LOGW("TP","IP2TP");

    TpUart2_L_Data_Req(0, 0, 0, 0, &lpdu);
}

void TP_GW_L_Data_Ind(PduInfoType * pduInfoPtr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("TP","L_Data_Ind: ERR_NULL_PTR");
    }
    else
    {
        uint8_t index = 0;

        /* Message Code */
        L_RxBuffer[index++] =  L_DATA_IND;
        L_RxBuffer[index++] =  0x00U;

        /* CTRL1 Field */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[0];

        /* CTRL2 Field - AT, HC, EFF */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[5] & LENGTH_FIELD_AT_HC_MASK;

        /* Source Address - High */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[1];

        /* Source Address - Low */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[2];

        /* Destination Address - High */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[3];

        /* Destination Address - Low */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[4];

        if ((L_RxBuffer[CEMI_FRAME_SA_HI_BYTE_OFFET] == L_RxBuffer[CEMI_FRAME_DA_HI_BYTE_OFFET]) && 
            (L_RxBuffer[CEMI_FRAME_SA_LO_BYTE_OFFET] == L_RxBuffer[CEMI_FRAME_DA_LO_BYTE_OFFET]))
        {
            /* Source and Destination Address are same! */
            L_RxBuffer[CEMI_FRAME_DA_HI_BYTE_OFFET] = 0x00U;
            L_RxBuffer[CEMI_FRAME_DA_LO_BYTE_OFFET] = 0x00U;
        }

        /* Data Length */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[5] & LENGTH_FIELD_LG_MASK;

        /* Copy Data into Rx Buffer */
        memcpy(&L_RxBuffer[index], &pduInfoPtr->SduDataPtr[EMI_FRAME_DATA_OFFSET], L_RxBuffer[CEMI_FRAME_LENGTH_FIELD_OFFSET] + 1);

        index += L_RxBuffer[CEMI_FRAME_LENGTH_FIELD_OFFSET] + 1;

        /* Tunnelling Request - Send over IP */
        KNXnetIP_TunnellingRequest(&L_RxBuffer[0], index);

        // ESP_LOGW("IP","TP2IP");
    }
}

void TP_GW_L_Data_Ind_ACK(PduInfoType * pduInfoPtr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("TP","L_Data_Ind_ACK: ERR_NULL_PTR");
    }
    else
    {
        uint8_t index = 0;

        /* Message Code */
        L_RxBuffer[index++] =  L_DATA_IND;
        L_RxBuffer[index++] =  0x00U;

        /* CTRL1 Field */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[0];

        /* CTRL2 Field - AT, HC, EFF */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[5] & LENGTH_FIELD_AT_HC_MASK;

        /* Source Address - High */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[1];

        /* Source Address - Low */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[2];

        /* Destination Address - High */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[3];

        /* Destination Address - Low */
        L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[4];

        if ((L_RxBuffer[CEMI_FRAME_SA_HI_BYTE_OFFET] == L_RxBuffer[CEMI_FRAME_DA_HI_BYTE_OFFET]) && 
            (L_RxBuffer[CEMI_FRAME_SA_LO_BYTE_OFFET] == L_RxBuffer[CEMI_FRAME_DA_LO_BYTE_OFFET]))
        {
            /* Source and Destination Address are same! */
            L_RxBuffer[CEMI_FRAME_DA_HI_BYTE_OFFET] = 0x00U;
            L_RxBuffer[CEMI_FRAME_DA_LO_BYTE_OFFET] = 0x00U;
        }

        /* Data Length */
        L_RxBuffer[index++] =  0x00U;

        /* TPCI, SeqNum = 0 : ACK      */
        /* 1... .... = Packet Type: Control (1)    */
        /* .1.. .... = Sequence Type: Numbered (1) */
        /* ..00 00.. = Sequence Number: 0          */
        /* .... ..10 = Service: ACK (0x2)          */
        L_RxBuffer[index++] = 0xC2U;

        /* Tunnelling Request - Send over IP */
        KNXnetIP_TunnellingRequest(&L_RxBuffer[0], index);

    }
}

static uint8_t TP_L_Data_CalculateFCS(uint8_t * l_data, uint16_t length)
{
    uint8_t fcs = 0xFFU; /* Start with 0xFF */

    for (uint16_t i = 0; i < length; i++)
    {
        fcs ^= l_data[i];
    }

    /* Return XOR Checksum*/
    return (fcs);
}
