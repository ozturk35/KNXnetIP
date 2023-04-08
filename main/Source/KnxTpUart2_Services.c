/**
 * \file KnxTpUart2_Services.c
 * 
 * \brief KnxTpUart2 Services
 * 
 * This file contains the implementation of KnxTpUart2 Services
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
#include "esp_system.h"
#include "esp_log.h"
#include "sys/time.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "KnxTpUart2_Services.h"

/*==================[macros]================================================*/
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/
UartReq_ReturnType KnxTpUart2_U_ResetRequest(void);
UartReq_ReturnType KnxTpUart2_U_StateRequest(void);
UartReq_ReturnType KnxTpUart2_U_ActiveBusmon(void);
UartReq_ReturnType KnxTpUart2_U_ProductIdRequest(void);
UartReq_ReturnType KnxTpUart2_U_ActivateBusyMode(void);
UartReq_ReturnType KnxTpUart2_U_ResetBusyMode(void);
UartReq_ReturnType KnxTpUart2_U_SetAddress(uint16_t physicalAddr);
UartReq_ReturnType KnxTpUart2_U_AckInformation(uint8_t nack, uint8_t busy, uint8_t addr);
UartReq_ReturnType KnxTpUart2_U_L_DataStart(uint8_t eibCtrl);
UartReq_ReturnType KnxTpUart2_U_L_DataContinue(uint8_t index, uint8_t eibData);
UartReq_ReturnType KnxTpUart2_U_L_DataEnd(uint8_t length, uint8_t chksum);
UartReq_ReturnType KnxTpUart2_U_MxRstCnt(uint8_t busyCnt, uint8_t nackCnt);
UartReq_ReturnType KnxTpUart2_U_ActivateCRC(void);
UartReq_ReturnType KnxTpUart2_U_PollingState(uint8_t slotnumber, uint16_t pollAddr, uint8_t pollState);

int64_t KnxTpUart2_GetTimeMs();
int64_t KnxTpUart2_GetTimeUs();

/*==================[internal function declarations]========================*/
static UartReq_ReturnType KnxTpUart2_Transmit(const char * serviceName, const void * data, size_t size);

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/
UartReq_ReturnType KnxTpUart2_U_ResetRequest(void)
{
    const char cmd[] = {
        TPUART2_U_RESET_REQUEST,
    };

    return KnxTpUart2_Transmit("TPUART2_U_RESET_REQUEST", cmd, 1U);
}

UartReq_ReturnType KnxTpUart2_U_StateRequest(void)
{
    const char cmd[] = {
        TPUART2_U_STATE_REQUEST,
    };

    return KnxTpUart2_Transmit("TPUART2_U_STATE_REQUEST", cmd, 1U);
}

UartReq_ReturnType KnxTpUart2_U_ActiveBusmon(void)
{
    const char cmd[] = {
        TPUART2_U_ACTIVEBUSMON,
    };

    return KnxTpUart2_Transmit("TPUART2_U_ACTIVEBUSMON", cmd, 1U);
}

UartReq_ReturnType KnxTpUart2_U_ProductIdRequest(void)
{
    const char cmd[] = {
        TPUART2_U_PRODUCTID_REQUSET,
    };

    return KnxTpUart2_Transmit("TPUART2_U_PRODUCTID_REQUSET", cmd, 1U);
}

UartReq_ReturnType KnxTpUart2_U_ActivateBusyMode(void)
{
    const char cmd[] = {
        TPUART2_U_ACTIVATEBUSYMODE,
    };

    return KnxTpUart2_Transmit("TPUART2_U_ACTIVATEBUSYMODE", cmd, 1U);
}

UartReq_ReturnType KnxTpUart2_U_ResetBusyMode(void)
{
    const char cmd[] = {
        TPUART2_U_RESETBUSYMODE,
    };

    return KnxTpUart2_Transmit("TPUART2_U_RESETBUSYMODE", cmd, 1U);
}

UartReq_ReturnType KnxTpUart2_U_SetAddress(uint16_t physicalAddr)
{
    const char cmd[] = {
        TPUART2_U_SETADDRESS,
        (physicalAddr >> 8) & 0xFFU,
        (physicalAddr & 0xFFU)
    };

    return KnxTpUart2_Transmit("TPUART2_U_SETADDRESS", cmd, 3U);
}

UartReq_ReturnType KnxTpUart2_U_AckInformation(uint8_t nack, uint8_t busy, uint8_t addr)
{
    const char cmd[] = {
        TPUART2_U_ACKINFORMATION |
        ((nack & 0x01) << 2) |
        ((busy & 0x01) << 1) |
        (addr & 0x01)
    };

    return KnxTpUart2_Transmit("TPUART2_U_ACKINFORMATION", cmd, 1U);
}

UartReq_ReturnType KnxTpUart2_U_L_DataStart(uint8_t eibCtrl)
{
    const char cmd[2] = {
        (char)TPUART2_U_L_DATASTART,
        (char)eibCtrl
    };

    return KnxTpUart2_Transmit("TPUART2_U_L_DATASTART", cmd, 2U);
}

UartReq_ReturnType KnxTpUart2_U_L_DataContinue(uint8_t index, uint8_t eibData)
{
    const char cmd[2] = {
        (char)TPUART2_U_L_DATACONTINUE | (index & 0x3FU),
        (char)eibData
    };

    return KnxTpUart2_Transmit("TPUART2_U_L_DATACONTINUE", cmd, 2U);
}

UartReq_ReturnType KnxTpUart2_U_L_DataEnd(uint8_t length, uint8_t chksum)
{
    const char cmd[2] = {
        (char)TPUART2_U_L_DATAEND | (length & 0x3FU),
        (char)chksum
    };

    return KnxTpUart2_Transmit("TPUART2_U_L_DATAEND", cmd, 2U);
}

UartReq_ReturnType KnxTpUart2_U_MxRstCnt(uint8_t busyCnt, uint8_t nackCnt)
{
    const char cmd[] = {
        TPUART2_U_MXRSTCNT,
        ((busyCnt & 0x07U) << 5) | (nackCnt & 0x07U)
    };

    return KnxTpUart2_Transmit("TPUART2_U_MXRSTCNT", cmd, 2U);
}

UartReq_ReturnType KnxTpUart2_U_ActivateCRC(void)
{
    const char cmd[] = {
        TPUART2_U_ACTIVATECRC
    };

    return KnxTpUart2_Transmit("TPUART2_U_ACTIVATECRC", cmd, 1U);
}

UartReq_ReturnType KnxTpUart2_U_PollingState(uint8_t slotnumber, uint16_t pollAddr, uint8_t pollState)
{
    const char cmd[] = {
        TPUART2_U_POLLINGSTATE | (slotnumber & 0x0FU),
        ((pollAddr & 0xFF00U) >> 8),
        (pollAddr & 0x00FFU),
        pollState
    };

    return KnxTpUart2_Transmit("TPUART2_U_POLLINGSTATE", cmd, 4U);
}

int64_t KnxTpUart2_GetTimeMs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

int64_t KnxTpUart2_GetTimeUs()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1e6) + tv.tv_usec);
}

/*==================[internal function definitions]=========================*/
static UartReq_ReturnType KnxTpUart2_Transmit(const char * serviceName, const void * data, size_t size)
{
    int txBytes = uart_write_bytes(UART_NUM_1, data, size);
    uart_wait_tx_done(UART_NUM_1, portMAX_DELAY);

    ESP_LOGI(serviceName, "Wrote %d bytes", txBytes);

    return txBytes;
}

/*==================[end of file]===========================================*/
