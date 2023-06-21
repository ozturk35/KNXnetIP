/**
 * \file KnxTpUart2_Services.h
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

#ifndef KNXTPUART2_SERVICES_H
#define KNXTPUART2_SERVICES_H

/*==================[inclusions]============================================*/
#include "esp_system.h"

/*==================[macros]================================================*/
/* Services to UART */
#define TPUART2_U_RESET_REQUEST        (0x01U)
#define TPUART2_U_STATE_REQUEST        (0x02U)
#define TPUART2_U_ACTIVEBUSMON         (0x05U)
#define TPUART2_U_ACKINFORMATION       (0x10U) /* Nack: +0x04, Busy: +0x02, Addressed: +0x01 */

#define TPUART2_U_PRODUCTID_REQUSET    (0x20U)
#define TPUART2_U_ACTIVATEBUSYMODE     (0x21U)
#define TPUART2_U_RESETBUSYMODE        (0x22U)
#define TPUART2_U_MXRSTCNT             (0x24U)
#define TPUART2_U_ACTIVATECRC          (0x25U)
#define TPUART2_U_SETADDRESS           (0x28U)

#define TPUART2_U_L_DATASTART          (0x80U)
#define TPUART2_U_L_DATACONTINUE       (0x80U) /* +index [1..62] */
#define TPUART2_U_L_DATAEND            (0x40U) /* +length [7..63] */
#define TPUART2_U_POLLINGSTATE         (0xE0U) /* +Slotnumber [0..14] */

/* Services from UART */
#define TPUART2_ACKNOWLEDGEFRAME       (0xCCU)
#define TPUART2_NOTACKNOWLEDGEFRAME    (0x0CU)
#define TPUART2_BUSYFRAME              (0xC0U)

#define TPUART2_RESETINDICATION        (0x03U)
#define TPUART2_STATEINDICATION        (0x07U)
#define TPUART2_DATACONFIRMSUCCESS     (0x8BU)
#define TPUART2_DATACONFIRMFAIL        (0x0BU)
#define TPUART2_STATE_INDICATION_MASK  (0x07U)

/* Layer-2 Services */
#define TPUART2_LAYER2_SERVICE_MASK   (0x90U)
#define TPUART2_LAYER2_L_DATA_REQ     (0x90U)
#define TPUART2_LAYER2_L_EXT_DATA_REQ (0x10U)
#define TPUART2_LAYER2_L_POLLDATA_REQ (0xF0U)

/* TP-UART-Control-Services */
#define TPUART2_U_RESET_IND (0x03U)

/* TP-UART-State.indication/response Service */
#define TPUART2_STATE_SLAVE_COLLISION (0x80U)
#define TPUART2_STATE_RECEIVE_ERROR   (0x40U)
#define TPUART2_STATE_TRANSMIT_ERROR  (0x20U)
#define TPUART2_STATE_PROTOCOL_ERROR  (0x10U)
#define TPUART2_STATE_TEMP_WARNING    (0x08U)

/*==================[type definitions]======================================*/
typedef int UartReq_ReturnType;

/*==================[external function declarations]========================*/
extern UartReq_ReturnType KnxTpUart2_U_ResetRequest(void);
extern UartReq_ReturnType KnxTpUart2_U_StateRequest(void);
extern UartReq_ReturnType KnxTpUart2_U_ActiveBusmon(void);
extern UartReq_ReturnType KnxTpUart2_U_ProductIdRequest(void);
extern UartReq_ReturnType KnxTpUart2_U_ActivateBusyMode(void);
extern UartReq_ReturnType KnxTpUart2_U_ResetBusyMode(void);
extern UartReq_ReturnType KnxTpUart2_U_SetAddress(uint16_t physicalAddr);
extern UartReq_ReturnType KnxTpUart2_U_AckInformation(uint8_t nack, uint8_t busy, uint8_t addr);
extern UartReq_ReturnType KnxTpUart2_U_L_DataStart(uint8_t eibCtrl);
extern UartReq_ReturnType KnxTpUart2_U_L_DataContinue(uint8_t index, uint8_t eibData);
extern UartReq_ReturnType KnxTpUart2_U_L_DataEnd(uint8_t length, uint8_t chksum);
extern UartReq_ReturnType KnxTpUart2_U_MxRstCnt(uint8_t busyCnt, uint8_t nackCnt);
extern UartReq_ReturnType KnxTpUart2_U_ActivateCRC(void);
extern UartReq_ReturnType KnxTpUart2_U_PollingState(uint8_t slotnumber, uint16_t pollAddr, uint8_t pollState);

extern int64_t KnxTpUart2_GetTimeMs();
extern int64_t KnxTpUart2_GetTimeUs();

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

#endif /* #ifndef KNXTPUART2_SERVICES_H */

/*==================[end of file]===========================================*/
