/**
 * \file KnxTpUart2.h
 * 
 * \brief KnxTpUart2
 * 
 * This file contains the implementation of KnxTpUart2
 * 
 * \version 1.0.0
 * 
 * \author Ibrahim Ozturk
 * 
 * Copyright 2023 Ibrahim Ozturk
 * All rights exclusively reserved for Ibrahim Ozturk,
 * unless expressly agreed to otherwise.
*/

#ifndef KNXTPUART2_H
#define KNXTPUART2_H

/*==================[inclusions]============================================*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

/*==================[macros]================================================*/

#define TPUART2_TELEGRAM_POLL_SIZE (8U)
#define TPUART2_TELEGRAM_SPLITTER_STACK_SIZE (128U)
#define TPUART2_TELEGRAM_DISPATCHER_STACK_SIZE (128U)

#define KNXTPUART2_OK                    (0x00U)
#define KNXTPUART2_ERROR                 (0xFFU)
#define KNXTPUART2_ERROR_NOT_INIT_STATE  (0xFEU)
#define KNXTPUART2_ERROR_NULL_EVT_CBK_FC (0xFDU)
#define KNXTPUART2_ERROR_NULL_ACK_CBK_FC (0xFCU)
#define KNXTPUART2_ERROR_NULL_CFG_PTR    (0xFBU)

/*==================[type definitions]======================================*/
typedef enum {
    MODE_NORMAL, /* Normal Mode */
    MODE_BUSMONITORING /* Bus Monitoring Mode */
} KnxTpUart2_ModeType;

typedef enum {
    EVENT_RESET = 0,
    EVENT_EIB_TELEGRAM_RECEIVE,
    EVENT_EIB_TELEGRAM_RECEPTION_ERR,
    EVENT_STATEINDICATION
} KnxTpUart2_EventType;

typedef void (*KnxTpUart2_EventCbkFctPtrType) (KnxTpUart2_EventType);

typedef enum {
    RX_STATE_RESET = 0,
    RX_STATE_STOPPED,
    RX_STATE_INIT,
    RX_STATE_IDLE, /* Wait for a Ctrl Field */
    RX_STATE_EIB_TELEGRAM_RECEPTION_START, /* Reception started, no address eval. yet */
    RX_STATE_EIB_TELEGRAM_RECEPTION_ADDRESSED, /* Address eval. done */
    RX_STATE_EIB_TELEGRAM_RECECPTION_LENGTH_INVALID,
    RX_STATE_EIB_TELEGRAM_RECEPTION_NOT_ADDRESSED /* Address eval. not done */
} KnxTpUart2_RxStateType;

typedef struct {
    KnxTpUart2_RxStateType state;
    KnxTelegram_Type * telegram;
    uint8_t addressedPduId;
} KnxTpUart2_RxType;

typedef enum {
    TX_STATE_RESET = 0,
    TX_STATE_STOPPED,
    TX_STATE_INIT,
    TX_STATE_IDLE,
    TX_STATE_TRANSMITTING,
    TX_STATE_WAITING_ACK
} KnxTpUart2_TxStateType;

typedef enum {
    TX_RESPONSE_ACK = 0,
    TX_RESPONSE_NACK,
    TX_RESPONSE_TIMEOUT,
    TX_RESPONSE_RESET /* TPUART2 reset before Tx response reception */
} KnxTpUart2_TxResponseType;

typedef void (*KnxTpUart2_AckCbkFctPtr) (KnxTpUart2_TxResponseType);

typedef struct {
    KnxTpUart2_TxStateType state;
    KnxTelegram_Type * telegram;
    KnxTpUart2_AckCbkFctPtr ackCbkFctPtr;
    uint8_t nbRemainingBytes;
    uint8_t byteIndex;
} KnxTpUart2_TxType;

typedef struct {
    uint8_t isEOP;
    uint8_t dataByte;
} KnxTpUart2_MonitorDataType;

typedef struct {
    uint16_t addr;
    uint8_t dptId;
    uint8_t indicator;
    uint8_t length;
    boolean validity;
    uint8_t * value
} KnxTpUart2_ComObjectTpye;

typedef struct {
    uint16_t physicalAddr;
    KnxTpUart2_ModeType mode;
    KnxTpUart2_RxType rx;
    KnxTpUart2_TxType tx;
    KnxTpUart2_EventCbkFctPtrType eventCbkFctPtr;
    KnxTpUart2_ComObjectTpye * comObjectList;
    uint8_t assignedComObjNb;
    uint8_t * orderedIndexTable;
    uint8_t stateIndication;
} KnxTpUart2_ObjectType;

typedef struct {
    uart_port_t uartPort;
    uart_config_t * uartConfig;
    uint16_t physicalAddr;
    uint8_t txdPin;
    uint8_t rxdPin;
    uint16_t rxTimeout;
    uint16_t txTimeout;
    uint16_t rxBufferSize;
} KnxTpUart2_UartCfgType;

typedef struct {
    KnxTpUart2_UartCfgType * uartCfg;
    KnxTpUart2_ObjectType * objCfg;
} KnxTpUart2_CfgType;

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/

#endif /* #ifndef KNXTPUART2_H */