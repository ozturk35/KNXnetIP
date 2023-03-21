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

#include "Knx_Dpt.h"
#include "KnxComObject.h"
#include "KnxTelegram.h"
#include "KnxTpUart2_Services.h"
#include "KnxTpUart2.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

void KnxTpUart2_Create(KnxTpUart2_CfgType * cfgPtr);
void KnxTpUart2_Main(void);

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/
const uart_config_t TPUART2_CFG = 
{
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

KnxTpUart2_UartCfgType KnxTpUart2_UartCfg = 
{
    UART_NUM_1, /* Uart Port Number */
    &TPUART2_CFG, /* Uart Configuration */
    GPIO_NUM_4, /* Txd Pin */
    GPIO_NUM_5, /* Rxd Pin */
    200, /* Rx Timeout */
    200, /* Tx Timeout */
    1024, /* Rx Buffer Size */
};

KnxTpUart2_CfgType KnxTpUart2_Cfg = 
{
    &KnxTpUart2_UartCfg,
    &KnxTpUart2_Object
};

/* Create a new KnxTpUart2 Object */
//KnxTpUart2_ObjectType KnxTpUart2 = KnxTpUart2_Create(KnxTpUart2_Cfg);

/*==================[external function definitions]=========================*/
void KnxTpUart2_Create(KnxTpUart2_CfgType * cfgPtr) {
    KnxTpUart2_ObjectType tpUartObj;

    // We won't use a buffer for sending data.
    uart_driver_install(cfgPtr->uartPort, cfgPtr->rxBufSize * 2, 0, 0, NULL, 0);
    uart_param_config(cfgPtr->uartPort, cfgPtr->uart_config);
    uart_set_pin(cfgPtr->uartPort, cfgPtr->txdPin, cfgPtr->txdPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    tpUartObj.rx.state  = RX_STATE_RESET;
    tpUartObj.rx.addressedPduId = 0;
    tpUartObj.tx.state = TX_STATE_RESET;
    tpUartObj.tx.ackCbkFctPtr = NULL;
    tpUartObj.tx.nbRemainingBytes = 0;
    tpUartObj.tx.txByteIndex = 0;
    tpUartObj.stateIndication = 0;
    tpUartObj.eventCbkFctPtr = NULL;
    tpUartObj.comObjectList = NULL;
    tpUartObj.assignedComObjNb = 0;
    tpUartObj.orderedIndexTable = NULL;
    tpUartObj.stateIndication = 0;

    return tpUartObj;
}

uint8_t KnxTpUart2_Init(KnxTpUart2_ObjectType * objPtr)
{
    uint8_t retVal = KNXTPUART2_UNINITIALIZED;

    if ((RX_STATE_INIT != objPtr->rx.state)
       || (TX_STATE_INIT != objPtr->tx.state))
    {
        /* Do nothing. Default value KNXTPUART2_UNITIALIZED will be returned. */
    }
    else
    {
        if (MODE_BUSMONITORING != objPtr->mode)
        {
            KnxTpUart2_U_ActiveBusmon();
            retVal = KNXTPUART2_OK;
        }
        else
        {
            if (NULL == objPtr->eventCbkFctPtr)
            {
                retVal = KNXTPUART2_EVT_CALLBACK_NULL;
            }
            else if (NULL == objPtr->tx.ackCbkFctPtr)
            {
                retVal = KNXTPUART2_ACK_CALLBACK_NULL;
            }
            else
            {
                /* Send request TPUART2_U_SETADDRESS (0x28U) */
                KnxTpUart2_U_SetAddress(objPtr->physicalAddr);
                
                /* Send request TPUART2_U_STATE_REQUEST (0x02U) */
                KnxTpUart2_U_StateRequest();

                objPtr->rx.state = RX_STATE_IDLE;
                objPtr->tx.state = TX_STATE_IDLE;

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
            objPtr->rx.state = RX_STATE_RESET;
            objPtr->tx.state = TX_STATE_RESET;
        }

        uart_driver_install(cfgPtr->uartCfg->uartPort, cfgPtr->uartCfg->rxBufSize * 2, 0, 0, NULL, 0);
        uart_param_config(cfgPtr->uartCfg->uartPort, cfgPtr->uartCfg->uart_config);
        uart_set_pin(cfgPtr->uartCfg->uartPort, cfgPtr->txdPin, cfgPtr->txdPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

        for (attempts = 0; attempts < 9; attempts++)
        {
            KnxTpUart2_U_ResetRequest();

            nowTime = KnxTpUart2_GetTimeMs();
            startTime = nowTime;

            while (TimeDelta(nowTime, startTime < 1000))
            {
                uint8_t length = uart_read_bytes(cfgPtr->uartCfg->uartPort, length, 100);

                if (TPUART2_RESETINDICATION == uart_read_bytes())
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

        return retVal;
    }
}

uint8_t KnxTpUart2_SendTelegram(KnxTpUart2_ObjectType * objPtr, KnxTelegram_Type * telegram)
{
    uint8_t retVal = KNXTPUART2_E_NOT_OK;

    if (TX_STATE_IDLE != objPtr->tx.state)
    {
        /* Tx initialization error.  */
    }
    else
    {
        if (telegram->sourceAddr != objPtr->physicalAddr)
        {
            telegram->l_data.sourceAddr = objPtr->physicalAddr;
            KnxTelegram_UpdateChecksum(telegram);
        }

        objPtr->tx.telegram = telegram;
        objPtr->tx.nbRemainingBytes = telegram->l_data.length & 0x0FU;
        objPtr->tx.byteIndex = 0;
        objPtr->tx.state = TX_STATE_TRANSMITTING;

        retVal = KNXTPUART2_E_OK;
    }

    return retVal;
}

void KnxTpUart2_Rx(KnxTpUart2_ObjectType * objPtr)
{
    uint8_t rxByte;
    uint16_t nowTime;

    uint8_t receivedBytes;
    KnxTelegram_Type telegram;
    uint8_t addressedComObjId;
    uint16_t lastByteRxTimeUs;

    if (RX_STATE_EIB_TELEGRAM_RECEPTION_START == objPtr->rx.state)
    {
        nowTime = esp_timer_get_time();

        /* Check if time delta above 2ms */
        if (TimeDelta(nowTime, lastByteRxTimeUs) > 2000)
        {
            switch(objPtr->rx.state)
            {
                case RX_EIB_TELEGRAM_RECEPTION_START:
                case RX_STATE_EIB_TELEGRAM_RECECPTION_LENGTH_INVALID:
                    objPtr->eventCbkFctPtr(TPUART_EVENT_EIB_TELEGRAM_RECEPTION_ERROR);
                    break;
                
                case RX_STATE_EIB_TELEGRAM_RECEPTION_ADDRESSED:
                    if (telegram.l_data.payloadChecksum == KnxTelegram_CalculateChecksum(&telegram))
                    {
                        KnxTelegram_Copy(&objPtr->rx.telegram, &telegram);
                        objPtr->rx.addressedPduId = addressedComObjId;
                        objPtr->eventCbkFctPtr(TPUART_EVENT_RECEIVED_EIB_TELEGRAM);
                    }
                    else
                    {
                        objPtr->eventCbkFctPtr(TPUART_EVENT_EIB_TELEGRAM_RECEPTION_ERROR);
                    }

                    break;

                default:
                    break;
            }

            objPtr->rx.state = RX_STATE_IDLE;
        }
    }

    if( 1 /* uart is available */ )
    {
        rxByte = uart_read(objPtr->uart);
        lastByteRxTimeUs = esp_timer_get_time();

        switch (objPtr->rx.state)
        {
            case RX_STATE_IDLE:
                if (EIB_CTRL_FIELD_VALID_PATTERN == (rxByte & EIB_CTRL_FIELD_MASK))
                {
                    objPtr->rx.state = RX_STATE_EIB_TELEGRAM_RECEPTION_START;
                    receivedBytes = 1;

                    telegram.bytes[0] = rxByte;
                }
                else if (TPUART_DATA_CONFIRM_SUCCESS == rxByte)
                {
                    if (TX_STATE_WAITING_ACK == objPtr->tx.state)
                    {
                        objPtr->tx.ackCbkFctPtr(TX_RESPONSE_ACK);
                        objPtr->tx.state = TX_IDLE;
                    }
                }
                else if (TPUART_RESET_INDICATION == rxByte)
                {
                    if ((TX_STATE_TRANSMITTING == objPtr->tx.state)
                       || (TX_STATE_WAITING_ACK == objPtr->tx.state))
                    {
                        objPtr->tx.ackCbkFctPtr(TX_RESPONSE_RESET);
                    }
                    objPtr->tx.state = TX_STATE_STOPPED;
                    objPtr->rx.state = RX_STATE_STOPPED;
                    objPtr->eventCbkFctPtr(TPUART_EVENT_RESET);
                }
                else if (TPUART_EVENT_STATE_INDICATION == (rxByte & TPUART_STATE_INDICATION_MASK) )
                {
                    objPtr->eventCbkFctPtr(TPUART_EVENT_STATE_INDICATIION);
                    objPtr->stateIndication = rxByte;
                }
                else if (TPUART_DATA_CONFIRM_FAILED == rxByte)
                {
                    if (objPtr->tx.state == TX_WAITING_ACK)
                    {
                        objPtr->tx.ackFctPtr(TX_RESPONSE_NACK);
                        objPtr->tx.state = TX_STATE_IDLE; 
                    }
                }
            break;

            case RX_STATE_EIB_TELEGRAM_RECEPTION_START:
                telegram.bytes[receivedBytes] = rxByte;
                receivedBytes++;

                if (3 == receivedBytes) /* Source Address byte received */
                {
                    if (objPtr->physicalAddr == telegram.l_data.sourceAddr)
                    {
                        objPtr->rx.state = RX_STATE_EIB_TELEGRAM_RECEPTION_NOT_ADDRESSED
                    }
                }
                else if (6 == receivedBytes) /* Length byte received (also address type and routing) */
                {
                    if (KnxTpUart2_IsAddressAssigned(telegram.l_data.destAddr, addressedComObjId))
                    {
                        objPtr->rx.state = RX_STATE_EIB_TELEGRAM_RECEPTION_ADDRESSED;
                        KnxTpUart2_U_AckInformation(0, 0, 1); /* Addressed */
                    }
                    else
                    {
                        objPtr->rx.state = RX_STATE_EIB_TELEGRAM_RECEPTION_NOT_ADDRESSED;
                        KnxTpUart2_U_AckInformation(0, 0, 0); /* Ack */
                    }
                }

                break;
            
            case RX_STATE_EIB_TELEGRAM_RECEPTION_ADDRESSED:
                if (KNX_TELEGRAM_MAX_SIZE == receivedBytes)
                {
                    objPtr->rx.state = RX_STATE_EIB_TELEGRAM_RECECPTION_LENGTH_INVALID
                }
                else
                {
                    telegram.bytes[rxByte, receivedBytes];
                    receivedBytes++;
                }

                break;

            default:
                break;
        }
    }
}

void KnxTpUart2_Tx(KnxTpUart2_ObjectType * objPtr)
{
    uint16_t nowTime;
    uint8_t txByte[2];
    uint16_t sentMessageTimeMs;

    switch(objPtr->tx.state)
    {
        case TX_STATE_WAITING_ACK:
            nowTime = esp_timer_get_time();

            if (TimeDelta(nowTime, sentMessageTimeMs) > 500)
            {
                objPtr->tx.ackCbkFctPtr(NO_ANSWER_TIMEOUT);
                objPtr->tx.state = TX_STATE_IDLE;
            }
            break;
        
        case TX_STATE_TRANSMITTING:
            if (RX_STATE_EIB_TELEGRAM_RECEPTION_START != objPtr->rx.state)
            {
                if (1 == objPtr->tx.nbRemainingBytes)
                {
                    uint8_t crc = KnxTelegram_CalculateChecksum(objPtr->tx.telegram)
                    KnxTpUart2_U_L_DataEnd(objPtr->tx.byteIndex, crc);

                    sentMessageTimeMs = esp_timer_get_time();
                    objPtr->tx.state = TX_STATE_WAITING_ACK;
                }
                else
                {
                    KnxTpUart2_U_L_DataContinue(objPtr->tx.byteIndex, rawByte[index]);

                    objPtr->tx.byteIndex++;
                    objPtr->tx.nbRemainingBytes--;
                }
            }

            break;

        default:
            break;
    }
}

boolean KnxTpUart2_GetMonitoringData(KnxTpUart2_MonitoringDataType * data)
{
    uint8_t retVal = FALSE;

    uint16_t nowTime;
    KnxTpUart2_MonitoringDataType currentData = {true, 0};
    uint16_t lastByteRxTimeUs;

    if (FALSE != currentData.isEOP)
    {
        nowTime = (uint16_t) esp_timer_get_time();

        if (TimeDelta(nowTime, lastByteRxTimeUs) > 2000)
        {
            currentData.isEOP = true;
            currentData.dataByte = 0;
            data = currentData;
            return TRUE;
        }
    }

    if (1 /* uart is available */)
    {
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, currentData.dataByte, 1, 20 / portTICK_PERIOD_MS);
        currentData.isEOP = false;
        data = currentData;
        lastByteRxTimeUs = esp_timer_get_time();
        retVal = TRUE;
    }

    return retVal;
}

boolean KnxTpUart2_IsAddressAssigned(KnxTpUart2_ObjectType * objPtr, uint16_t addr, uint8_t * index)
{
    uint8_t retVal = FALSE;
    uint8_t divCounter = 0;
    uint8_t i, startIndex, stopIndex, range;

    if (FALSE == objPtr->assignedComObjNb)
    {
        /* Empty com object list. do nothing */
    }
    else
    {
        for (i = 4; objPtr->assignedComObjNb >> i; i++)
        {
            divisionCounter++;
        }

        startIndex = 0;
        stopIndex = objPtr->assignedComObjNb - 1;
        range = objPtr->assignedComObjNb;

        while (divisionCounter)
        {
            range >> 1;

            if (addr >= objPtr->comObjectList[objPtr->orderedIndexTable[startIndex + range]].addr)
            {
                startIndex += range;
            }
            else
            {
                stopIndex -= range;
            }

            divCounter--;
        };

        for (i = startIndex; (objPtr->comObjectList[objPtr->comObjectList[objPtr->orderedIndexTable[i]].addr] != addr && (i <= stopIndex)); i++ );

        if (i > stopIndex)
        {
            retVal = FALSE;
        }
        else
        {
            index = objPtr->orderedIndexTable[i];
            retVal = TRUE;
        }
    }

    return retVal;
}

void KnxTpUart2_Main(void)
{
    uint8_t eibCtrl[] = {

    };

    uint8_t eibData[] = {

    };

    KnxTpUart2_U_L_DataStart(eibCtrl);
    KnxTpUart2_U_L_DataContinue(0x01, eibData)
}

void KnxTpUart2_Reset(KnxTpUart2Type * uart)
{
    uint16_t startTime, nowTime;
    uint8_t maxResetAttempts = UART_MAX_RESET_ATTEMPTS;
}


/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/