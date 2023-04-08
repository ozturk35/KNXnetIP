

#include "Pdu.h"
#include "Knx_NetworkLayer.h"
#include "Knx_TransportLayer.h"
#include "Knx_ApplicationLayer.h"

#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

static uint8_t T_RxBuffer[64];
static uint8_t T_TxBuffer[64];

void T_Data_Group_Req(AckType ack, HopCountType hopCount, uint8_t octetCount, PriorityType priority, uint16_t tsap, PduInfoType * pduInfoPtr);
void T_Data_Group_Con(void);
void T_Data_Group_Ind(HopCountType hopCount, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr, uint16_t tsap, PduInfoType * pduInfoPtr);

void T_Data_Group_Req(AckType ack, HopCountType hopCount, uint8_t octetCount, PriorityType priority, uint16_t tsap, PduInfoType * pduInfoPtr)
{
    PduInfoType tpdu;

    /* Check if input parameter is not NULL. */
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("Knx TransportLayer", "T_Data_Group_Req : ERR_NULL_PTR");
    }
    else if (0U == pduInfoPtr->SduLength)
    {
        ESP_LOGI("Knx TransportLayer", "T_Data_Group_Req : ERR_ZERO_LENGTH_APDU");
    }
    else
    {
        /* Get group address from table */
        const uint16_t groupAddr = GroupAddressTable[tsap];

        /* Copy APDU into local buffer */
        memcpy(&T_TxBuffer[APDU_TPDU_OFFSET], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));

        /* Set Transport Control field (TPCI) */
        T_TxBuffer[0] = (pduInfoPtr->SduDataPtr[0]) & TPDU_FIELD_APCI_MASK;

        /* Following statement may commented out as it is ineffective. T_Data_Group is 0x00 */
        T_TxBuffer[0] |= (T_Data_Group & TPDU_FIELD_TPCI_MASK);

        tpdu.SduDataPtr = &T_TxBuffer[0];
        tpdu.SduLength = (uint8_t)(pduInfoPtr->SduLength);

        N_Data_Group_Req(ack, groupAddr, hopCount, octetCount, priority, &tpdu);

    }

    (void)tsap;
}

void T_Data_Group_Con(void)
{

}

void T_Data_Group_Ind(HopCountType hopCount, uint8_t octetCount, PriorityType priority, uint16_t sourceAddr, uint16_t tsap, PduInfoType * pduInfoPtr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("TransportLayer","T_Data_Group_Ind: ERR_NULL_PTR");
    }
    else if (64U < (uint8_t)(pduInfoPtr->SduLength))
    {
        ESP_LOGI("TransportLayer","T_Data_Group_Ind: ERR_BUFF_OVFL: %d",(uint8_t)(pduInfoPtr->SduLength));
    }
    else
    {
        PduInfoType apdu;
        uint16_t asap = tsap;
        APCIType apci;

        /* Copy T-PDU into Rx buffer */
        memcpy(&T_RxBuffer[0], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));

        for (uint8_t index = 0; index < (uint8_t)(pduInfoPtr->SduLength); index++)
        {
            ESP_LOGI("TransportLayer", "TPDU byte[%d]: 0x%X", index, T_RxBuffer[index]);
        }

        apdu.SduDataPtr = &T_RxBuffer[APDU_TPDU_OFFSET];
        apdu.SduLength = (uint8_t)(pduInfoPtr->SduLength) - APDU_TPDU_OFFSET;

        apci = (apdu.SduDataPtr[0] & APDU_FIELD_APCI_MASK) << 2U;
        apci |= (apdu.SduDataPtr[1] & ASDU_FIELD_APCI_4BIT_MASK) >> 6U;

        if (A_GroupValue_Response == apci)
        {
            A_GroupValue_Read_Acon(asap, priority, hopCount, None, &apdu);
        }
        else
        {
            ESP_LOGI("TransportLayer", "ERR_UNKNOWN_APCI_VALUE");
        }
    }
}
