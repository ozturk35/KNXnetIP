

#include "Pdu.h"
#include "Knx_Types.h"
#include "Knx_DataLinkLayer.h"
#include "Knx_TransportLayer.h"

#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

#define NETWORK_LAYER_HOP_COUNT (0x05U)
#define NETWORK_LAYER_SOURCE_ADDRESS (0x11FAU)

static uint8_t N_RxBuffer[64];
static uint8_t N_TxBuffer[64];

/* N_Data_xx_Req */
void N_Data_Individual_Req(void);
void N_Data_Group_Req(AckType ack, uint16_t destAddr, HopCountType hopCount, uint8_t octetCount, PriorityType priority, PduInfoType * pduInfoPtr);
void N_Data_Broadcast_Req(void);
void N_Data_SystemBroadcast_Req(void);

/* N_Data_xx_Con */
void N_Data_Individual_Con(void);
void N_Data_Group_Con(AckType ack, uint16_t destAddr, HopCountType hopCount, uint8_t octetCount, PriorityType priority, PduInfoType * pduInfoPtr, bool status);
void N_Data_Broadcast_Con(void);
void N_Data_SystemBroadcast_Con(void);

/* N_Data_xx_Ind */
void N_Data_Individual_Ind(void);
void N_Data_Group_Ind(uint16_t destAddr, HopCountType hopCount, uint16_t octetCount, PriorityType priority, PduInfoType * pduInfoPtr);
void N_Data_Broadcast_Ind(void);
void N_Data_SystemBroadcast_Ind(void);

void N_Data_Individual_Req(void)
{

}

void N_Data_Group_Req(AckType ack, uint16_t destAddr, HopCountType hopCount, uint8_t octetCount, PriorityType priority, PduInfoType * pduInfoPtr)
{
    PduInfoType npdu;
    FrameFormatType frameFormat = StandardFrame;

    /* Set Network Control field (NPCI) */
    if (UnlimitedRouting == hopCount)
    {
        N_TxBuffer[0] = ((7U << LENGTH_FIELD_NPCI_OFFSET) & LENGTH_FIELD_NPCI_MASK);
    }
    else
    {
        /* Set Network Layer Parameter */
        N_TxBuffer[0] = ((NETWORK_LAYER_HOP_COUNT << LENGTH_FIELD_NPCI_OFFSET) & LENGTH_FIELD_NPCI_MASK);
    }

    /* Copy TPDU into local buffer */
    memcpy(&N_TxBuffer[TPDU_NPDU_OFFSET], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));

    npdu.SduDataPtr = &N_TxBuffer[0];
    npdu.SduLength = (uint8_t)(pduInfoPtr->SduLength) + TPDU_NPDU_OFFSET;

    /* Check if APDU length is bigger than 15 */
    if (15U < octetCount)
    {
        frameFormat = ExtendedFrame;
    }

    L_Data_Req(ack, GroupAddress, destAddr, frameFormat, &npdu, octetCount, priority, NETWORK_LAYER_SOURCE_ADDRESS);

}

void N_Data_Broadcast_Req(void)
{

}

void N_Data_SystemBroadcast_Req(void)
{

}

void N_Data_Individual_Con(void)
{

}

void N_Data_Group_Con(AckType ack, uint16_t destAddr, HopCountType hopCount, uint8_t octetCount, PriorityType priority, PduInfoType * pduInfoPtr, bool status)
{
    (void)ack;
    (void)destAddr;
    (void)hopCount;
    (void)octetCount;
    (void)priority;
}

void N_Data_Broadcast_Con(void)
{

}

void N_Data_SystemBroadcast_Con(void)
{

}

void N_Data_Group_Ind(uint16_t destAddr, HopCountType hopCount, uint16_t octetCount, PriorityType priority, PduInfoType * pduInfoPtr)
{
    if (NULL == pduInfoPtr)
    {
        ESP_LOGI("NetworkLayer","N_Data_Group_Ind: ERR_NULL_PTR");
    }
    else if (64U < (uint8_t)(pduInfoPtr->SduLength))
    {
        ESP_LOGI("NetworkLayer","N_Data_Group_Ind: ERR_BUFF_OVFL");
    }
    else
    {
        PduInfoType tpdu;
        uint16_t sourceAddr = 0x0001U;
        uint16_t tsap = 0U;

        /* Copy N-PDU into Rx buffer */
        memcpy(&N_RxBuffer[0], pduInfoPtr->SduDataPtr, (uint8_t)(pduInfoPtr->SduLength));

        for (uint8_t index = 0; index < (uint8_t)(pduInfoPtr->SduLength); index++)
        {
            ESP_LOGI("NetworkLayer", "NPDU byte[%d]: 0x%X", index, N_RxBuffer[index]);
        }

        tpdu.SduDataPtr = &N_RxBuffer[TPDU_NPDU_OFFSET];
        tpdu.SduLength = (uint8_t)(pduInfoPtr->SduLength) - TPDU_NPDU_OFFSET;

        T_Data_Group_Ind(hopCount, octetCount, priority, sourceAddr, tsap, &tpdu);
    }
}

void N_Data_Broadcast_Ind(void)
{

}

void N_Data_Individual_Ind(void)
{
    
}
