#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

#include "Pdu.h"
#include "Knx_Types.h"
#include "KNXnetIP_Tunnelling.h"
#include "TP_DataLinkLayer.h"
#include "TpUart2_DataLinkLayer.h"
#include "KnxTpUart2_Services.h"

static uint8_t TpUart2_TxBuffer[64];
//static uint8_t TpUart2_RxBuffer[64];

void TpUart2_L_Data_Req(bool repeatFlag, uint16_t destAddr, AddressType addrType, PriorityType priority, PduInfoType * pduInfoPtr);
void TpUart2_L_Data_Con(void);
void TpUart2_L_Data_Ind(PduInfoType * pduInfoPtr);

void TpUart2_L_Data_Req(bool repeatFlag, uint16_t destAddr, AddressType addrType, PriorityType priority, PduInfoType * pduInfoPtr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("TpUart2","TpUart2_L_Data_Req: ERR_NULL_PTR");
    }
    else
    {
        /* Copy frame into Tx buffer */
        memcpy(&TpUart2_TxBuffer[0], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));

        for (uint8_t i = 0; i < pduInfoPtr->SduLength; i++)
        {
            if (0U == i)
            {
                KnxTpUart2_U_L_DataStart(TpUart2_TxBuffer[i]);
            }
            else if ((pduInfoPtr->SduLength) - 1 == i)
            {
                KnxTpUart2_U_L_DataEnd(pduInfoPtr->SduLength, TpUart2_TxBuffer[i]);
            }
            else
            {
                KnxTpUart2_U_L_DataContinue(i, TpUart2_TxBuffer[i]);
            }
        }
    }
}

void TpUart2_L_Data_Con()
{

}

void TpUart2_L_Data_Ind(PduInfoType * pduInfoPtr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("TpUart2_DataLinkLayer","TpUart2_L_Data_Con: ERR_NULL_PTR");
    }
    else if (0U == (uint8_t)(pduInfoPtr->SduLength))
    {
        ESP_LOGI("TpUart2_DataLinkLayer","TpUart2_L_Data_Con: ERR_ZERO_LENGTH_LPDU");
    }
    else
    {
        FrameFormatType frameFormat;
        PriorityType priority;
        bool repeatFlag;
        uint8_t octetCount;

        frameFormat = (FrameFormatType)(((uint8_t)(pduInfoPtr->SduDataPtr[0]) & CTRL_FIELD_FRAME_FORMAT_MASK) >> CTRL_FIELD_FRAME_FORMAT_OFFSET);
        repeatFlag = (bool)(((uint8_t)(pduInfoPtr->SduDataPtr[0]) & CTRL_FIELD_REPEAT_FLAG_MASK) >> CTRL_FIELD_REPEAT_FLAG_OFFSET);
        priority = (PriorityType)(((uint8_t)(pduInfoPtr->SduDataPtr[0]) & CTRL_FIELD_PRIORITY_MASK) >> CTRL_FIELD_PRIORITY_OFFSET);

        if (StandardFrame == frameFormat)
        {
            octetCount = (uint8_t)(pduInfoPtr->SduDataPtr[5U]) & LENGTH_FIELD_LG_MASK;
        }
        else
        {
            octetCount = (uint8_t)(pduInfoPtr->SduDataPtr[6U]);
        }

        /* Tunnel to IP */
        // KNXnetIP_TunnelTP2IP(pduInfoPtr);
    }
}
