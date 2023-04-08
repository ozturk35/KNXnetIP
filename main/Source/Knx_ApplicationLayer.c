

#include "Pdu.h"
#include "Knx_TransportLayer.h"

#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

static uint8_t A_RxBuffer[64];
static uint8_t A_TxBuffer[64];

void A_GroupValue_Read_Req(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom);
void A_GroupValue_Read_Lcon(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, StatusType status);
void A_GroupValue_Read_Ind(uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom);
void A_GroupValue_Read_Res(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr);
void A_GroupValue_Read_Acon(uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr);

void A_GroupValue_Write_Req(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr);
void A_GroupValue_Write_Lcon(AckType ack, uint16_t asap, PriorityType priorit, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr);

void A_GroupValue_Read_Req(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom)
{
    PduInfoType apdu;
    uint8_t index = 0;
    uint8_t tsap;

    /* Set Application Control field (APCI) */
    /* Following statements may commented out as it is ineffective. A_GroupValue_Read is 0x00 */
    A_TxBuffer[index] = (A_GroupValue_Read >> 2) & 0x3U; /* 2 bits in octet 6 */
    index++;

    A_TxBuffer[index] = (A_GroupValue_Read << 6) & 0xC0U; /* 2 bits in octet 7 */
    index++;

    apdu.SduDataPtr = &A_TxBuffer[0];
    apdu.SduLength = index; /* Common field for APCI & TPCI */

    tsap = asap; /* Will do a mapping */

    /* Pass data to Transport Layer */
    T_Data_Group_Req(ack, hopCount, (apdu.SduLength - 1), priority, tsap, &apdu);

    (void)secCom; /* Not used in the application layer */
}

void A_GroupValue_Read_Lcon(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, StatusType status)
{
    (void)ack;
    (void)asap;
    (void)priority;
    (void)hopCount;
    (void)secCom;
    (void)status;
}

void A_GroupValue_Read_Ind(uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom)
{
    (void)asap;
    (void)priority;
    (void)hopCount;
    (void)secCom;
}

void A_GroupValue_Read_Res(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr)
{
    (void)ack;
    (void)asap;
    (void)priority;
    (void)hopCount;
    (void)secCom;
    (void)APduPtr;
}

void A_GroupValue_Read_Acon(uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr)
{
    if (NULL == APduPtr)
    {
        ESP_LOGI("ApplicationLayer","A_GroupValue_Read_Acon: ERR_NULL_PTR");
    } 
    else if (64U < APduPtr->SduLength)
    {
        ESP_LOGI("ApplicationLayer","A_GroupValue_Read_Acon: ERR_BUFF_OVFL");
    }
    else
    {
        /* Copy A-PDU into Rx buffer */
        memcpy(&A_RxBuffer[0], APduPtr->SduDataPtr, (uint8_t)(APduPtr->SduLength));

        ESP_LOGI("ApplicationLayer","A_GroupValue_Read_Acon: Rx bytes: %d", (uint8_t)(APduPtr->SduLength));

        for (uint8_t index = 0; index < (uint8_t)(APduPtr->SduLength); index++)
        {
            ESP_LOGI("ApplicationLayer", "APDU byte[%d]: 0x%X", index, A_RxBuffer[index]);
        }
    }
    (void)asap; 
    (void)priority;
    (void)hopCount;
    (void)secCom;
    (void)APduPtr;
}

void A_GroupValue_Read_Rcon(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr)
{
    (void)asap;
    (void)priority;
    (void)hopCount;
    (void)secCom;
    (void)APduPtr;
}

void A_GroupValue_Write_Req(AckType ack, uint16_t asap, PriorityType priority, HopCountType hopCount, SecComType secCom, PduInfoType * APduPtr)
{
    if (NULL == APduPtr)
    {
        ESP_LOGI("ApplicationLayer","A_GroupValue_Write_Req: ERR_NULL_PTR");
    }
    else
    {
        PduInfoType apdu;
        uint16_t tsap = asap; /* Will do a mapping */
        uint8_t index = 0;

        /* Set Application Control field (APCI) */
        A_TxBuffer[index] = (A_GroupValue_Write >> 2) & 0x3U; /* 2 bits in octet 6 */
        index++;

        A_TxBuffer[index] = (A_GroupValue_Write << 6) & 0xC0U; /* 2 bits in octet 7 */

        /* Values that only consist of 6 bits or less have the optimized A_GroupValue_Write-PDU format: */
        if ((APduPtr->SduLength <= 1U) & (0 == (APduPtr->SduDataPtr[0] & 0xC0U)))
        {
            A_TxBuffer[index] = (APduPtr->SduDataPtr[0]) & 0x3FU;
            index++;
        }
        else
        {
            index++;
            /* Copy data into Tx buffer */
            memcpy(&A_TxBuffer[index], APduPtr->SduDataPtr, (uint8_t)(APduPtr->SduLength));
        }

        apdu.SduDataPtr = &A_TxBuffer[0];
        apdu.SduLength = (uint8_t)(APduPtr->SduLength) + index;

        T_Data_Group_Req(ack, hopCount, (APduPtr->SduLength - 1), priority, tsap, &apdu);
    }

    (void)secCom; /* Not used in the application layer */
}
