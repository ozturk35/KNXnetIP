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
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "KnxTpUart2_Services.h"

/*==================[macros]================================================*/

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
UartReq_ReturnType KnxTpUart2_U_L_DataStart(uint8_t eibCtrl);
UartReq_ReturnType KnxTpUart2_U_L_DataContinue(uint8_t index, uint8_t eibData);
UartReq_ReturnType KnxTpUart2_U_L_DataEnd(uint8_t length, uint8_t chksum);
UartReq_ReturnType KnxTpUart2_U_MxRstCnt(uint8_t busyCnt, uint8_t nackCnt);
UartReq_ReturnType KnxTpUart2_U_ActivateCRC(void);
UartReq_ReturnType KnxTpUart2_U_PollingState(uint8_t slotnumber, uint16_t pollAddr, uint8_t pollState);

int64_t KnxTpUart2_GetTimeMs();

/*==================[internal function declarations]========================*/
UartReq_ReturnType KnxTpUart2_Transmit(const char * serviceName, const char * data);

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/
UartReq_ReturnType KnxTpUart2_U_ResetRequest(void)
{
    uint8_t cmd[] = {
        TPUART2_U_RESET_REQUEST,
    };

    return KnxTpUart2_Transmit("TPUART2_U_RESET_REQUEST", &cmd);
}

UartReq_ReturnType KnxTpUart2_U_StateRequest(void)
{
    uint8_t cmd[] = {
        TPUART2_U_STATE_REQUEST,
    };

    return KnxTpUart2_Transmit("TPUART2_U_STATE_REQUEST", &cmd);
}

UartReq_ReturnType KnxTpUart2_U_ActiveBusmon(void)
{
    uint8_t cmd[] = {
        TPUART2_U_ACTIVEBUSMON,
    };

    return KnxTpUart2_Transmit("TPUART2_U_ACTIVEBUSMON", &cmd);
}

UartReq_ReturnType KnxTpUart2_U_ProductIdRequest(void)
{
    uint8_t cmd[] = {
        TPUART2_U_PRODUCTID_REQUSET,
    };

    return KnxTpUart2_Transmit("TPUART2_U_PRODUCTID_REQUSET", &cmd);
}

UartReq_ReturnType KnxTpUart2_U_ActivateBusyMode(void)
{
    uint8_t cmd[] = {
        TPUART2_U_ACTIVATEBUSYMODE,
    };

    return KnxTpUart2_Transmit("TPUART2_U_ACTIVATEBUSYMODE", &cmd);
}

UartReq_ReturnType KnxTpUart2_U_ResetBusyMode(void)
{
    uint8_t cmd[] = {
        TPUART2_U_RESETBUSYMODE,
    };

    return KnxTpUart2_Transmit("TPUART2_U_RESETBUSYMODE", &cmd);
}

UartReq_ReturnType KnxTpUart2_U_SetAddress(uint16_t physicalAddr)
{
    uint8_t cmd[] = {
        TPUART2_U_SETADDRESS,
        (physicalAddr >> 8) & 0xFFU,
        (physicalAddr & 0xFFU)
    };

    return KnxTpUart2_Transmit("TPUART2_U_SETADDRESS", cmd);
}

UartReq_ReturnType KnxTpUart2_U_AckInformation(uint8_t nack, uint8_t busy, uint8_t addr)
{
    uint8_t cmd[] = {
        TPUART2_U_ACKINFORMATION |
        ((nack & 0x01) << 2) |
        ((busy & 0x01) << 1) |
        (addr & 0x01)
    };

    return KnxTpUart2_Transmit("TPUART2_U_ACKINFORMATION", cmd);
}

UartReq_ReturnType KnxTpUart2_U_L_DataStart(uint8_t eibCtrl)
{
    uint8_t cmd = {
        TPUART2_U_L_DATASTART,
        eibCtrl
    };

    return KnxTpUart2_Transmit("TPUART2_U_L_DATASTART", cmd);
}

UartReq_ReturnType KnxTpUart2_U_L_DataContinue(uint8_t index, uint8_t eibData)
{
    uint8_t cmd[] = {
        TPUART2_U_L_DATACONTINUE | (index & 0x3FU),
        eibData
    };

    return KnxTpUart2_Transmit("TPUART2_U_L_DATACONTINUE", cmd);
}

UartReq_ReturnType KnxTpUart2_U_L_DataEnd(uint8_t length, uint8_t chksum)
{
    uint8_t cmd[] = {
        TPUART2_U_L_DATAEND | (length & 0x3FU),
        chksum
    };

    return KnxTpUart2_Transmit("TPUART2_U_L_DATAEND", cmd);
}

UartReq_ReturnType KnxTpUart2_U_MxRstCnt(uint8_t busyCnt, uint8_t nackCnt)
{
    uint8_t cmd[] = {
        TPUART2_U_MXRSTCNT,
        ((busyCnt & 0x07U) << 5) | (nackCnt & 0x07U)
    };

    return KnxTpUart2_Transmit("TPUART2_U_MXRSTCNT", cmd);
}

UartReq_ReturnType KnxTpUart2_U_ActivateCRC(void)
{
    uint8_t cmd[] = {
        TPUART2_U_ACTIVATECRC
    };

    return KnxTpUart2_Transmit("TPUART2_U_ACTIVATECRC", &cmd);
}

UartReq_ReturnType KnxTpUart2_U_PollingState(uint8_t slotnumber, uint16_t pollAddr, uint8_t pollState)
{
    uint8_t cmd[] = {
        TPUART2_U_POLLINGSTATE | (slotnumber & 0x0FU),
        ((pollAddr & 0xFF00U) >> 8),
        (pollAddr & 0x00FFU),
        pollState
    };

    return KnxTpUart2_Transmit("TPUART2_U_POLLINGSTATE", cmd);
}

int64_t KnxTpUart2_GetTimeMs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}
/*==================[internal function definitions]=========================*/
static UartReq_ReturnType KnxTpUart2_Transmit(const char * serviceName, const char * data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);

    ESP_LOGI(serviceName, "Wrote %d bytes", txBytes);

    return txBytes;
}

/*==================[end of file]===========================================*/
