#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

#include "Pdu.h"
#include "Knx_Types.h"
#include "KNXnetIP.h"
#include "TP_DataLinkLayer.h"
#include "TpUart2_DataLinkLayer.h"
#include "KnxTpUart2_Services.h"

static uint8_t TpUart2_TxBuffer[128];
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
    (void)repeatFlag;
    (void)destAddr;
    (void)addrType;
    (void)priority;
}

void TpUart2_L_Data_Con()
{

}

void TpUart2_L_Data_Ind(PduInfoType * pduInfoPtr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("TpUart2_DataLinkLayer","TpUart2_L_Data_Ind: ERR_NULL_PTR");
    }
    else if (0U == (uint8_t)(pduInfoPtr->SduLength))
    {
        ESP_LOGI("TpUart2_DataLinkLayer","TpUart2_L_Data_Ind: ERR_ZERO_LENGTH_LPDU");
    }
    else
    {
        ESP_LOGI("TpUart2_DataLinkLayer","TpUart2_L_Data_Ind: TUNNEL TO IP");

        if (true == KNXnetIP_Connected)
        {
            /* Tunnel to IP */
            TP_GW_L_Data_Ind(pduInfoPtr);
        }
    }
}
