/**
 * \file KnxTpUart2.c
 * 
 * \brief KnxTpUart2 Main
 * 
 * This file contains the implementation of KnxTpUart2 Main
 * 
 * \version 1.0.0
 * 
 * \author Ibrahim Ozturk
 * 
 * Copyright 2023 Ibrahim Ozturk
 * All rights exclusively reserved for Ibrahim Ozturk,
 * unless expressly agreed to otherwise.
*/

/*==================[inclusions]============================================*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "sys/time.h"
#include "driver/gpio.h"

#include "Knx_Dpt.h"
#include "KnxComObject.h"
#include "KnxTelegram.h"
#include "KnxTpUart2_Services.h"
#include "KnxTpUart2.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

KnxTpUart2_ObjectType KnxTpUart2_Create(KnxTpUart2_CfgType * cfgPtr);
uint8_t KnxTpUart2_Reset(KnxTpUart2_CfgType * cfgPtr);
uint8_t KnxTpUart2_Init(KnxTpUart2_CfgType * cfgPtr);
uint8_t KnxTpUart2_SendTelegram(KnxTpUart2_ObjectType * objCfg, KnxTelegram_Type * telegram);
void KnxTpUart2_Rx(KnxTpUart2_CfgType * cfgPtr);
void KnxTpUart2_Tx(KnxTpUart2_CfgType * cfgPtr);
bool KnxTpUart2_GetMonitoringData(KnxTpUart2_CfgType * cfgPtr, KnxTpUart2_MonitorDataType * data);
bool KnxTpUart2_IsAddressAssigned(KnxTpUart2_ObjectType * objCfg, uint16_t addr, uint8_t * index);
void KnxTpUart2_Main(void);

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/* Create a new KnxTpUart2 Object */
//KnxTpUart2_ObjectType KnxTpUart2 = KnxTpUart2_Create(KnxTpUart2_Cfg);

/*==================[external function definitions]=========================*/
KnxTpUart2_ObjectType KnxTpUart2_Create(KnxTpUart2_CfgType * cfgPtr) {
    KnxTpUart2_ObjectType tpUartObj;

    // We won't use a buffer for sending data.
    uart_driver_install(cfgPtr->uartCfg->uartPort, cfgPtr->uartCfg->rxBufferSize * 2, 0, 0, NULL, 0);
    uart_param_config(cfgPtr->uartCfg->uartPort, cfgPtr->uartCfg->uartConfig);
    uart_set_pin(cfgPtr->uartCfg->uartPort, cfgPtr->uartCfg->txdPin, cfgPtr->uartCfg->rxdPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    tpUartObj.rx.state  = RX_STATE_RESET;
    tpUartObj.rx.addressedPduId = 0;
    tpUartObj.tx.state = TX_STATE_RESET;
    tpUartObj.tx.ackCbkFctPtr = NULL;
    tpUartObj.tx.nbRemainingBytes = 0;
    tpUartObj.tx.byteIndex = 0;
    tpUartObj.stateIndication = 0;
    tpUartObj.eventCbkFctPtr = NULL;
    tpUartObj.comObjectList = NULL;
    tpUartObj.assignedComObjNb = 0;
    tpUartObj.orderedIndexTable = NULL;
    tpUartObj.stateIndication = 0;

    return tpUartObj;
}

uint8_t KnxTpUart2_Init(KnxTpUart2_CfgType * cfgPtr)
{
    uint8_t retVal = KNXTPUART2_ERROR_NOT_INIT_STATE;

    if (NULL == cfgPtr)
    {
        retVal = KNXTPUART2_ERROR_NULL_CFG_PTR;
    }
    else if ((RX_STATE_INIT != cfgPtr->objCfg->rx.state)
       || (TX_STATE_INIT != cfgPtr->objCfg->tx.state))
    {
        /* Do nothing. Default value KNXTPUART2_UNITIALIZED will be returned. */
    }
    else
    {
        if (MODE_BUSMONITORING != cfgPtr->objCfg->mode)
        {
            KnxTpUart2_U_ActiveBusmon();
            retVal = KNXTPUART2_OK;
        }
        else
        {
            if (NULL == cfgPtr->objCfg->eventCbkFctPtr)
            {
                retVal = KNXTPUART2_ERROR_NULL_EVT_CBK_FC;
            }
            else if (NULL == cfgPtr->objCfg->tx.ackCbkFctPtr)
            {
                retVal = KNXTPUART2_ERROR_NULL_ACK_CBK_FC;
            }
            else
            {
                /* Send request TPUART2_U_SETADDRESS (0x28U) */
                KnxTpUart2_U_SetAddress(cfgPtr->objCfg->physicalAddr);
                
                /* Send request TPUART2_U_STATE_REQUEST (0x02U) */
                KnxTpUart2_U_StateRequest();

                cfgPtr->objCfg->rx.state = RX_STATE_IDLE;
                cfgPtr->objCfg->tx.state = TX_STATE_IDLE;

                retVal = KNXTPUART2_OK;
            }
        }
    }

    return retVal;
}

uint8_t KnxTpUart2_Reset(KnxTpUart2_CfgType * cfgPtr)
{
    uint16_t startTime, nowTime;
    uint8_t attempts;
    uint8_t retVal = KNXTPUART2_ERROR;

    if ((NULL == cfgPtr) || (NULL == cfgPtr->objCfg) || (NULL == cfgPtr->uartCfg) )
    {
        /* NULL pointer detected */
        retVal = KNXTPUART2_ERROR_NULL_CFG_PTR;
    }
    else
    {
        if ((RX_STATE_RESET < cfgPtr->objCfg->rx.state) || (TX_STATE_RESET < cfgPtr->objCfg->tx.state))
        {
            uart_driver_delete(cfgPtr->uartCfg->uartPort);
            cfgPtr->objCfg->rx.state = RX_STATE_RESET;
            cfgPtr->objCfg->tx.state = TX_STATE_RESET;
        }

        uart_driver_install(cfgPtr->uartCfg->uartPort, cfgPtr->uartCfg->rxBufferSize * 2, 0, 0, NULL, 0);
        uart_param_config(cfgPtr->uartCfg->uartPort, cfgPtr->uartCfg->uartConfig);
        uart_set_pin(cfgPtr->uartCfg->uartPort, cfgPtr->uartCfg->txdPin, cfgPtr->uartCfg->txdPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

        for (attempts = 0; attempts < 9; attempts++)
        {
            KnxTpUart2_U_ResetRequest();

            nowTime = KnxTpUart2_GetTimeMs();
            startTime = nowTime;

            while ((nowTime - startTime) < 1000)
            {
                uint8_t rxByte;
                (void)uart_read_bytes(cfgPtr->uartCfg->uartPort, &rxByte, 1, 100);

                if (TPUART2_RESETINDICATION == rxByte)
                {
                    cfgPtr->objCfg->rx.state = RX_STATE_INIT;
                    cfgPtr->objCfg->tx.state = TX_STATE_INIT;

                    retVal = KNXTPUART2_OK;
                    attempts = 9;
                    break;
                }
            };
        }
        uart_driver_delete(cfgPtr->uartCfg->uartPort);
    }

    return retVal;
}

uint8_t KnxTpUart2_SendTelegram(KnxTpUart2_ObjectType * objCfg, KnxTelegram_Type * telegram)
{
    uint8_t retVal = KNXTPUART2_ERROR;

    if ((NULL == objCfg) || (NULL == telegram))
    {
        /* NULL pointer error */
    }
    else if (TX_STATE_IDLE != objCfg->tx.state)
    {
        /* Tx initialization error.  */
    }
    else
    {
        if (telegram->l_data.sourceAddr != objCfg->physicalAddr)
        {
            telegram->l_data.sourceAddr = objCfg->physicalAddr;
            KnxTelegram_UpdateChecksum(telegram);
        }

        objCfg->tx.telegram = telegram;
        objCfg->tx.nbRemainingBytes = telegram->l_data.length & 0x0FU;
        objCfg->tx.byteIndex = 0;
        objCfg->tx.state = TX_STATE_TRANSMITTING;

        retVal = KNXTPUART2_OK;
    }

    return retVal;
}

void KnxTpUart2_Rx(KnxTpUart2_CfgType * cfgPtr)
{
    uint8_t rxByte;
    uint16_t nowTime;

    static uint8_t receivedBytes;
    static KnxTelegram_Type telegram;
    static uint8_t addressedComObjId;
    static uint16_t lastByteRxTimeUs;

    if (RX_STATE_EIB_TELEGRAM_RECEPTION_START == cfgPtr->objCfg->rx.state)
    {
        nowTime = esp_timer_get_time();

        /* Check if time delta above 2ms */
        if ((nowTime - lastByteRxTimeUs) > 2000)
        {
            switch(cfgPtr->objCfg->rx.state)
            {
                case RX_STATE_EIB_TELEGRAM_RECEPTION_START:
                case RX_STATE_EIB_TELEGRAM_RECECPTION_LENGTH_INVALID:
                    cfgPtr->objCfg->eventCbkFctPtr(EVENT_EIB_TELEGRAM_RECEPTION_ERR);
                    break;
                
                case RX_STATE_EIB_TELEGRAM_RECEPTION_ADDRESSED:
                    if (telegram.l_data.payloadChecksum[KNX_TELEGRAM_PAYLOAD_MAX_SIZE-2] == KnxTelegram_CalculateChecksum(&telegram))
                    {
                        KnxTelegram_Copy(cfgPtr->objCfg->rx.telegram, &telegram);
                        cfgPtr->objCfg->rx.addressedPduId = addressedComObjId;
                        cfgPtr->objCfg->eventCbkFctPtr(EVENT_EIB_TELEGRAM_RECEIVE);
                    }
                    else
                    {
                        cfgPtr->objCfg->eventCbkFctPtr(EVENT_EIB_TELEGRAM_RECEPTION_ERR);
                    }

                    break;

                default:
                    break;
            }

            cfgPtr->objCfg->rx.state = RX_STATE_IDLE;
        }
    }

    if( 1 /* uart is available */ )
    {
        lastByteRxTimeUs = esp_timer_get_time();
        (void)uart_read_bytes(cfgPtr->uartCfg->uartPort, &rxByte, 1, 100);

        switch (cfgPtr->objCfg->rx.state)
        {
            case RX_STATE_IDLE:
                if (CONTROL_FIELD_VALID_PATTERN == (rxByte & CONTROL_FIELD_PATTERN_MASK))
                {
                    cfgPtr->objCfg->rx.state = RX_STATE_EIB_TELEGRAM_RECEPTION_START;
                    receivedBytes = 1;

                    telegram.bytes[0] = rxByte;
                }
                else if (TPUART2_DATACONFIRMSUCCESS == rxByte)
                {
                    if (TX_STATE_WAITING_ACK == cfgPtr->objCfg->tx.state)
                    {
                        cfgPtr->objCfg->tx.ackCbkFctPtr(TX_RESPONSE_ACK);
                        cfgPtr->objCfg->tx.state = TX_STATE_IDLE;
                    }
                }
                else if (TPUART2_RESETINDICATION == rxByte)
                {
                    if ((TX_STATE_TRANSMITTING == cfgPtr->objCfg->tx.state)
                       || (TX_STATE_WAITING_ACK == cfgPtr->objCfg->tx.state))
                    {
                        cfgPtr->objCfg->tx.ackCbkFctPtr(TX_RESPONSE_RESET);
                    }

                    cfgPtr->objCfg->tx.state = TX_STATE_STOPPED;
                    cfgPtr->objCfg->rx.state = RX_STATE_STOPPED;
                    cfgPtr->objCfg->eventCbkFctPtr(EVENT_RESET);
                }
                else if (TPUART2_STATEINDICATION == (rxByte & TPUART2_STATE_INDICATION_MASK) )
                {
                    cfgPtr->objCfg->eventCbkFctPtr(EVENT_STATEINDICATION);
                    cfgPtr->objCfg->stateIndication = rxByte;
                }
                else if (TPUART2_DATACONFIRMFAIL == rxByte)
                {
                    if (cfgPtr->objCfg->tx.state == TX_STATE_WAITING_ACK)
                    {
                        cfgPtr->objCfg->tx.ackCbkFctPtr(TX_RESPONSE_NACK);
                        cfgPtr->objCfg->tx.state = TX_STATE_IDLE; 
                    }
                }
            break;

            case RX_STATE_EIB_TELEGRAM_RECEPTION_START:
                telegram.bytes[receivedBytes] = rxByte;
                receivedBytes++;

                if (3U == receivedBytes) /* Source Address byte received */
                {
                    if (cfgPtr->objCfg->physicalAddr == telegram.l_data.sourceAddr)
                    {
                        cfgPtr->objCfg->rx.state = RX_STATE_EIB_TELEGRAM_RECEPTION_NOT_ADDRESSED;
                    }
                }
                else if (6U == receivedBytes) /* Length byte received (also address type and routing) */
                {
                    if (KnxTpUart2_IsAddressAssigned(cfgPtr->objCfg, telegram.l_data.destAddr, &addressedComObjId))
                    {
                        cfgPtr->objCfg->rx.state = RX_STATE_EIB_TELEGRAM_RECEPTION_ADDRESSED;
                        KnxTpUart2_U_AckInformation(0, 0, 1); /* Addressed */
                    }
                    else
                    {
                        cfgPtr->objCfg->rx.state = RX_STATE_EIB_TELEGRAM_RECEPTION_NOT_ADDRESSED;
                        KnxTpUart2_U_AckInformation(0, 0, 0); /* Ack */
                    }
                }

                break;

            case RX_STATE_EIB_TELEGRAM_RECEPTION_ADDRESSED:
                if (KNX_TELEGRAM_MAX_SIZE == receivedBytes)
                {
                    cfgPtr->objCfg->rx.state = RX_STATE_EIB_TELEGRAM_RECECPTION_LENGTH_INVALID;
                }
                else
                {
                    telegram.bytes[receivedBytes] = rxByte;
                    receivedBytes++;
                }

                break;

            default:
                break;
        }
    }
}

void KnxTpUart2_Tx(KnxTpUart2_CfgType * cfgPtr)
{
    uint16_t nowTime;
    static uint16_t sentMessageTimeMs;

    switch(cfgPtr->objCfg->tx.state)
    {
        case TX_STATE_WAITING_ACK:
            nowTime = esp_timer_get_time();

            if ((nowTime - sentMessageTimeMs) > 500)
            {
                cfgPtr->objCfg->tx.ackCbkFctPtr(NO_ANSWER_TIMEOUT);
                cfgPtr->objCfg->tx.state = TX_STATE_IDLE;
            }
            break;
        
        case TX_STATE_TRANSMITTING:
            if (RX_STATE_EIB_TELEGRAM_RECEPTION_START != cfgPtr->objCfg->rx.state)
            {
                if (1 == cfgPtr->objCfg->tx.nbRemainingBytes)
                {
                    uint8_t crc = KnxTelegram_CalculateChecksum(cfgPtr->objCfg->tx.telegram);
                    (void) KnxTpUart2_U_L_DataEnd(cfgPtr->objCfg->tx.byteIndex, crc);

                    sentMessageTimeMs = esp_timer_get_time();
                    cfgPtr->objCfg->tx.state = TX_STATE_WAITING_ACK;
                }
                else
                {
                    KnxTpUart2_U_L_DataContinue(cfgPtr->objCfg->tx.byteIndex, cfgPtr->objCfg->tx.telegram->bytes[cfgPtr->objCfg->tx.byteIndex]);

                    cfgPtr->objCfg->tx.byteIndex++;
                    cfgPtr->objCfg->tx.nbRemainingBytes--;
                }
            }

            break;

        default:
            break;
    }
}

bool KnxTpUart2_GetMonitoringData(KnxTpUart2_CfgType * cfgPtr, KnxTpUart2_MonitorDataType * data)
{
    uint8_t retVal = false;

    uint16_t nowTime;
    KnxTpUart2_MonitorDataType currentData = {true, 0};
    static uint16_t lastByteRxTimeUs;

    if (false != currentData.isEOP)
    {
        nowTime = (uint16_t)esp_timer_get_time();

        if ((nowTime - lastByteRxTimeUs) > 2000)
        {
            currentData.isEOP = true;
            currentData.dataByte = 0;
            memcpy(data, &currentData, sizeof(KnxTpUart2_MonitorDataType));
            return true;
        }
    }

    if (1 /* uart is available */)
    {
        int len = uart_read_bytes(cfgPtr->uartCfg->uartPort, &(currentData.dataByte), 1, 100);
        if (len > 0)
        {
            currentData.isEOP = false;
            data = &currentData;
            lastByteRxTimeUs = esp_timer_get_time();
            retVal = true;
        }
    }

    return retVal;
}

bool KnxTpUart2_IsAddressAssigned(KnxTpUart2_ObjectType * objCfg, uint16_t addr, uint8_t * index)
{
    uint8_t retVal = false;
    uint8_t divCounter = 0;
    uint8_t i, startIndex, stopIndex, range;

    if (false == objCfg->assignedComObjNb)
    {
        /* Empty com object list. do nothing */
    }
    else
    {
        for (i = 4; objCfg->assignedComObjNb >> i; i++)
        {
            divCounter++;
        }

        startIndex = 0;
        stopIndex = objCfg->assignedComObjNb - 1;
        range = objCfg->assignedComObjNb;

        while (divCounter)
        {
            range = range >> 1;

            if (addr >= objCfg->comObjectList[objCfg->orderedIndexTable[startIndex + range]].addr)
            {
                startIndex += range;
            }
            else
            {
                stopIndex -= range;
            }

            divCounter--;
        };

        for (i = startIndex; (objCfg->comObjectList[objCfg->orderedIndexTable[i]].addr != (uint16_t)(addr && (i <= stopIndex))); i++ );

        if (i > stopIndex)
        {
            retVal = false;
        }
        else
        {
            memcpy(index, &(objCfg->orderedIndexTable[i]), 1);
            retVal = true;
        }
    }

    return retVal;
}

void KnxTpUart2_Main(void)
{
    uint8_t eibCtrl = 0xFF;
    uint8_t eibData = 0xFF;
    KnxTpUart2_U_L_DataStart(eibCtrl);
    KnxTpUart2_U_L_DataContinue(0x01, eibData);
}

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/
