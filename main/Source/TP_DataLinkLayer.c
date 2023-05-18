#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

#include "Pdu.h"
#include "KNXnetIP.h"
#include "TP_DataLinkLayer.h"
#include "TpUart2_DataLinkLayer.h"

#define KNXNETIP_DEBUG_LOGGING

static uint8_t L_TxBuffer[64];
static uint8_t L_RxBuffer[64];

void TP_L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr);
void TP_L_Data_Con(AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr, PduInfoType * pduInfoPtr, StatusType status);
void TP_L_Data_Ind(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr);
void TP_GW_L_Data_Req(uint8_t * bufferPtr, uint8_t rxLength);
void TP_GW_L_Data_Ind(PduInfoType * pduInfoPtr);

static uint8_t TP_L_Data_CalculateFCS(uint8_t * l_data, uint8_t length);

void TP_L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr)
{
    PduInfoType lpdu;
    uint8_t index = 0;

    /* Set CTRL field */
    L_TxBuffer[index] = CTRL_FIELD_L_DATA_FRAME;
    L_TxBuffer[index] |= (uint8_t)(frameFormat << CTRL_FIELD_FRAME_FORMAT_OFFSET) & CTRL_FIELD_FRAME_FORMAT_MASK;
    L_TxBuffer[index] |= (CTRL_FIELD_NON_REPEATED_FRAME << CTRL_FIELD_REPEAT_FLAG_OFFSET) & CTRL_FIELD_REPEAT_FLAG_MASK;
    L_TxBuffer[index] |= (priority << CTRL_FIELD_PRIORITY_OFFSET) & CTRL_FIELD_PRIORITY_MASK;

    index++;

    if (ExtendedFrame == frameFormat)
    {
        /* Set CTRLE field */
        L_TxBuffer[index] |= (addrType << CTRLE_FIELD_ADDRESS_TYPE_OFFSET) & CTRLE_FIELD_ADDRESS_TYPE_MASK;
        L_TxBuffer[index] |= 0x0000U << CTRLE_FIELD_EFF_OFFSET;
        index++;
    }

    /* Set Source Address */
    L_TxBuffer[index] = (uint8_t)(sourceAddr >> 8) & 0xFFU;
    index++;
    L_TxBuffer[index] = (uint8_t)(sourceAddr & 0xFFU);
    index++;

    /* Set Destination Address */
    L_TxBuffer[index] = (uint8_t)(destAddr >> 8) & 0xFFU;
    index++;
    L_TxBuffer[index] = (uint8_t)(destAddr & 0xFFU);
    index++;

    /* Copy NPDU into local buffer */
    if (ExtendedFrame == frameFormat)
    {
        memcpy(&L_TxBuffer[NPDU_LPDU_OFFSET + CTRLE_FIELD_SIZE], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));
    }
    else
    {
        memcpy(&L_TxBuffer[NPDU_LPDU_OFFSET], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));
    }

    /* Set Length field - Address Type, Hop Count, Length */
    if (ExtendedFrame != frameFormat)
    {
        L_TxBuffer[index] &= LENGTH_FIELD_NPCI_MASK;
        L_TxBuffer[index] |= (addrType << LENGTH_FIELD_ADDRESS_TYPE_OFFSET) & LENGHT_FIELD_ADDRESS_TYPE_MASK;
        L_TxBuffer[index] |= octetCount;
    }
    index += (uint8_t)(pduInfoPtr->SduLength);

    /* Set FCS field */
    L_TxBuffer[index] = TP_L_Data_CalculateFCS(&L_TxBuffer[0], index);
    index++;

    lpdu.SduDataPtr = &L_TxBuffer[0];
    lpdu.SduLength = index;

    TpUart2_L_Data_Req(CTRL_FIELD_NON_REPEATED_FRAME, destAddr, addrType, priority, &lpdu);
}

void TP_L_Data_Ind(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("TP","L_Data_Ind: ERR_NULL_PTR");
    }
    else if (64U < (uint8_t)(pduInfoPtr->SduLength))
    {
        ESP_LOGI("TP","L_Data_Ind: ERR_BUFF_OVFL");
    }
    else
    {
        /* Copy L-PDU into Rx buffer */
        memcpy(&L_RxBuffer[0], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));

        for (uint8_t index = 0; index < (uint8_t)(pduInfoPtr->SduLength); index++)
        {
            ESP_LOGI("TP", "LPDU byte[%d]: 0x%X", index, L_RxBuffer[index]);
        }

#ifdef FCS_CHECK_ENABLED
        /* Data Validation */
        uint8_t fcs = TP_L_Data_CalculateFCS(&L_RxBuffer[0], ((uint8_t)(pduInfoPtr->SduLength) - FCS_FIELD_SIZE));

        if (L_RxBuffer[(uint8_t)(pduInfoPtr->SduLength - FCS_FIELD_SIZE)] != fcs)
        {
            ESP_LOGI("TP","L_Data_Ind: ERR_FCS_MISMATCH");
            ESP_LOGI("TP","Calculated: 0x%X", fcs);
            ESP_LOGI("TP","Actual: 0x%X", L_RxBuffer[pduInfoPtr->SduLength - FCS_FIELD_SIZE]);
        }
        else
        {
#endif
            /* Tunnel to IP DataLinkLayer */

#ifdef FCS_CHECK_ENABLED
        }
#endif
    }
}

void TP_L_Data_Con(AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr, PduInfoType * pduInfoPtr, StatusType status)
{
    (void)addrType;
    (void)destAddr;
    (void)frameFormat;
    (void)octetCount;
    (void)priority;
    (void)sourceAddr;
    (void)status;
}

void TP_GW_L_Data_Req(uint8_t * bufferPtr, uint8_t rxLength)
{
    PduInfoType lpdu;
    uint8_t index = 0;

    /* Set CTRL field */
    L_TxBuffer[index++] = bufferPtr[CEMI_FRAME_CTRL1_FIELD_OFFSET];

    /* Set Source Address */
    L_TxBuffer[index++] = (uint8_t)((KNX_INDIVIDUAL_ADDR >> 8) & 0xFFU);
    L_TxBuffer[index++] = (uint8_t)(KNX_INDIVIDUAL_ADDR & 0xFFU);

    /* Set Destination Address */
    L_TxBuffer[index++] = bufferPtr[CEMI_FRAME_DA_HI_BYTE_OFFET];
    L_TxBuffer[index++] = bufferPtr[CEMI_FRAME_DA_LO_BYTE_OFFET];

    /* Set Length Field - AT, HC, LG */
    L_TxBuffer[index++] = bufferPtr[CEMI_FRAME_CTRL2_FIELD_OFFSET] | bufferPtr[CEMI_FRAME_LENGTH_FIELD_OFFSET];

    /* Copy Data into Tx Buffer */
    memcpy(&L_TxBuffer[index], &bufferPtr[CEMI_FRAME_TPDU_FIELD_OFFSET], (uint8_t)(rxLength - CEMI_FRAME_TPDU_FIELD_OFFSET));
    index += rxLength - CEMI_FRAME_TPDU_FIELD_OFFSET;

    /* Set FCS field */
    L_TxBuffer[index] = TP_L_Data_CalculateFCS(&L_TxBuffer[0], index);
    index++;

    lpdu.SduDataPtr = &L_TxBuffer[0];
    lpdu.SduLength = index;

    TpUart2_L_Data_Req(0, 0, 0, 0, &lpdu);
}

void TP_GW_L_Data_Ind(PduInfoType * pduInfoPtr)
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

    /* Data Length */
    L_RxBuffer[index++] =  pduInfoPtr->SduDataPtr[5] & LENGTH_FIELD_LG_MASK;

    /* Copy Data into Rx Buffer */
    memcpy(&L_RxBuffer[index], &pduInfoPtr->SduDataPtr[EMI_FRAME_DATA_OFFSET], pduInfoPtr->SduLength - (EMI_FRAME_DATA_OFFSET + FCS_FIELD_SIZE));
    index += pduInfoPtr->SduLength - (EMI_FRAME_DATA_OFFSET + FCS_FIELD_SIZE);

    /* Tunnelling Request - Send over IP */
    KNXnetIP_TunnellingRequest(&L_RxBuffer[0], index);

#ifdef KNXNETIP_DEBUG_LOGGING
    for (uint8_t rxIndex = 0; rxIndex < index; rxIndex++)
    {
        if (L_RxBuffer[rxIndex] < 0x10U)
        {
            printf("0%X ", L_RxBuffer[rxIndex]);
        }
        else
        {
            printf("%X ", L_RxBuffer[rxIndex]);
        }
    }
    printf("\n ");
#endif
}

static uint8_t TP_L_Data_CalculateFCS(uint8_t * l_data, uint8_t length)
{
    uint8_t fcs = 0xFFU; /* Start with 0xFF */

    for (uint16_t i = 0; i < length; i++)
    {
        fcs ^= l_data[i];
    }

    /* Return XOR Checksum*/
    return (fcs);
}