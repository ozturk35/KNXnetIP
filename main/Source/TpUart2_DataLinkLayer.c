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

static uint8_t TpUart2_TxBuffer[512];
//static uint8_t TpUart2_RxBuffer[512];

void TpUart2_L_Data_Req(bool repeatFlag, uint16_t destAddr, AddressType addrType, PriorityType priority, PduInfoType * pduInfoPtr);
void TpUart2_L_Data_Con(void);
void TpUart2_L_Data_Ind(PduInfoType * pduInfoPtr);
bool TpUart2_DetectEOP(uint8_t * dataPtr, uint8_t rxLength, uint8_t * validDataLength);

void TpUart2_L_Data_Req(bool repeatFlag, uint16_t destAddr, AddressType addrType, PriorityType priority, PduInfoType * pduInfoPtr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("TpUart2","TpUart2_L_Data_Req: ERR_NULL_PTR");
    }
    else
    {
        /* Copy frame into Tx buffer */
        memcpy(&TpUart2_TxBuffer[0], pduInfoPtr->SduDataPtr, (uint16_t)(pduInfoPtr->SduLength));

        for (uint8_t i = 0; i < pduInfoPtr->SduLength; i++)
        {
            if (0U == i)
            {
                KnxTpUart2_U_L_DataStart(TpUart2_TxBuffer[i]);
            }
            else if ((pduInfoPtr->SduLength) - 1 == i)
            {
                KnxTpUart2_U_L_DataEnd((pduInfoPtr->SduLength) - 1, TpUart2_TxBuffer[i]);
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
        /* UART Controlfield */
        uint8_t layer2Service = pduInfoPtr->SduDataPtr[0];

        if (TPUART2_LAYER2_L_POLLDATA_REQ == layer2Service)
        {
            KnxTpUart2_U_PollingState(0, 0x11FAU, 0);
        }
        else
        {
            layer2Service = layer2Service & TPUART2_LAYER2_SERVICE_MASK;

            switch (layer2Service)
            {
                case TPUART2_LAYER2_L_DATA_REQ:
                case TPUART2_LAYER2_L_EXT_DATA_REQ:
                    if (true == KNXnetIP_Connected)
                    {
                        ESP_LOGI("TpUart2_DataLinkLayer","TpUart2_L_Data_Ind: TUNNEL TO IP");
                        /* Tunnel to IP */
                        TP_GW_L_Data_Ind(pduInfoPtr);
                        vTaskDelay(50/portTICK_PERIOD_MS);
                    }
                    break;
                
                default:
                    break;
            }
        }
    }
}

bool TpUart2_DetectEOP(uint8_t * dataPtr, uint8_t rxLength, uint8_t * validDataLength)
{
    uint8_t fcs = 0xFFU; /* Start with 0xFF */
    bool validPacketFound = false;
 
    for (uint8_t index = 0; index < rxLength; index++)
    {
        fcs ^= dataPtr[index];
        
        if ((rxLength >= index + 1) && (dataPtr[index + 1] == fcs))
        {
            *validDataLength = index + 1;
            validPacketFound = true;
            ESP_LOGI("TpUart2 EOP","Length: %d, Checksum: 0x%X\n",index+1, fcs);
            break;
        }
    }

    return validPacketFound;
}
