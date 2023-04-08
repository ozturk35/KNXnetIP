/**
 * \file KnxDevice.h
 * 
 * \brief KNX Device
 * 
 * This file contains the implementation of Knx Device module
 * 
 * \version 1.0.0
 * 
 * \author Ibrahim Ozturk
 * 
 * Copyright 2023 Ibrahim Ozturk
 * All rights exclusively reserved for Ibrahim Ozturk,
 * unless expressly agreed to otherwise.
*/

#ifndef KNX_DEVICE_H
#define KNX_DEVICE_H

/*==================[inclusions]============================================*/

#include "KnxDevice_Types.h"
#include "KnxTelegram.h"
#include "KnxTpUart2.h"

/*==================[macros]================================================*/
#define ACTIONS_BUFFER_SIZE (16U)


/*==================[external function definitions]=========================*/
/* KNX device execution task */
// This function shall be called in the loop
extern void KnxDevice_Task(KnxDeviceType * device);

/*==================[internal function definitions]=========================*/

// static void KnxDevice_GetTpUartEvents(KnxTpUart2_EventType event);

// static void KnxDevice_TxTelegramAck(KnxTpUart2_TxAckType txAck);

#endif /* #ifndef KNX_DEVICE_H */

/*==================[end of file]===========================================*/
