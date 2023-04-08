#include "Pdu.h"
#include "Knx_Types.h"
#include "Knx_NetworkLayer.h"
#include "Knx_DataLinkLayer.h"
#include "TpUart2_DataLinkLayer.h"

#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

static uint8_t L_TxBuffer[64];
static uint8_t L_RxBuffer[64];

void L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr);
void L_Data_Con(AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr, PduInfoType * pduInfoPtr, StatusType status);
void L_Data_Ind(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr);

static uint8_t L_Data_CalculateFCS(uint8_t * l_data, uint8_t length);

void L_Data_Req(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr)
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
    L_TxBuffer[index] = L_Data_CalculateFCS(&L_TxBuffer[0], index);
    index++;

    lpdu.SduDataPtr = &L_TxBuffer[0];
    lpdu.SduLength = index;

    TpUart2_L_Data_Req(CTRL_FIELD_NON_REPEATED_FRAME, destAddr, addrType, priority, &lpdu);

#if 0
    for (index = 0; index < NPDU_LPDU_OFFSET + pduInfoPtr->SduLength; index++)
        ESP_LOGI("DataLinkLayer","L_Data[%d]::0x%X", index, L_TxBuffer[index]);
#endif

}

void L_Data_Ind(AckType ack, AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, PduInfoType * pduInfoPtr, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("DataLinkLayer","L_Data_Ind: ERR_NULL_PTR");
    }
    else if (64U < (uint8_t)(pduInfoPtr->SduLength))
    {
        ESP_LOGI("DataLinkLayer","L_Data_Ind: ERR_BUFF_OVFL");
    }
    else if (frameFormat & CTRL_FIELD_FRAME_FORMAT_MASK)
    {
        ESP_LOGI("DataLinkLayer","L_Data_Ind: ERR_UNKNOWN_FRAME_FORMAT");
    }
    else
    {
        /* Copy L-PDU into Rx buffer */
        memcpy(&L_RxBuffer[0], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));

        for (uint8_t index = 0; index < (uint8_t)(pduInfoPtr->SduLength); index++)
        {
            ESP_LOGI("DataLinkLayer", "LPDU byte[%d]: 0x%X", index, L_RxBuffer[index]);
        }

#ifdef FCS_CHECK_ENABLED
        /* Data Validation */
        uint8_t fcs = L_Data_CalculateFCS(&L_RxBuffer[0], ((uint8_t)(pduInfoPtr->SduLength) - FCS_FIELD_SIZE));

        if (L_RxBuffer[(uint8_t)(pduInfoPtr->SduLength - FCS_FIELD_SIZE)] != fcs)
        {
            ESP_LOGI("DataLinkLayer","L_Data_Ind: ERR_FCS_MISMATCH");
            ESP_LOGI("DataLinkLayer","Calculated: 0x%X", fcs);
            ESP_LOGI("DataLinkLayer","Actual: 0x%X", L_RxBuffer[pduInfoPtr->SduLength - FCS_FIELD_SIZE]);
        }
        else
        {
#endif
            PduInfoType npdu;
            npdu.SduDataPtr = &L_RxBuffer[NPDU_LPDU_OFFSET];
            npdu.SduLength = (uint8_t)(pduInfoPtr->SduLength) - (NPDU_LPDU_OFFSET + FCS_FIELD_SIZE);

            if (GroupAddress == addrType)
            {
                if (0U == destAddr)
                {
                    /* Broadcast */
                    N_Data_Broadcast_Ind();
                }
                else
                {
                    /* Group */
                    N_Data_Group_Ind(destAddr, UnlimitedRouting, octetCount, priority, &npdu);
                }
            }
            else
            {
                /* Individual */
                N_Data_Individual_Ind();
            }
#ifdef FCS_CHECK_ENABLED
        }
#endif
    }
}

void L_Data_Con(AddressType addrType, uint16_t destAddr, FrameFormatType frameFormat, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr, PduInfoType * pduInfoPtr, StatusType status)
{
    (void)addrType;
    (void)destAddr;
    (void)frameFormat;
    (void)octetCount;
    (void)priority;
    (void)sourceAddr;
    (void)status;
}

static uint8_t L_Data_CalculateFCS(uint8_t * l_data, uint8_t length)
{
    uint8_t fcs = 0xFFU; /* Start with 0xFF */

    for (uint16_t i = 0; i < length; i++)
    {
        fcs ^= l_data[i];
    }

    /* Return XOR Checksum*/
    return (fcs);
}