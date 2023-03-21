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

#include "Std_Types.h"
#include "KnxDevice_Types.h"
#include "KnxTelegram.h"
#include "KnxTpUart2.h"

/*==================[macros]================================================*/
#define ACTIONS_BUFFER_SIZE (16U)

inline uint16_t PHY_ADDR(uint8_t area, uint8_t line, uint8_t busdevice)
{ return (uint16_t) ( ((area&0xF)<<12) + ((line&0xF)<<8) + busdevice ); }

inline uint16_t GROUP_ADDR(uint8_t maingrp, uint8_t midgrp, uint8_t subgrp)
{ return (uint16_t) ( ((maingrp&0x1F)<<11) + ((midgrp&0x7)<<8) + subgrp ); }

inline uint16_t GROUP_ADDR(uint8_t maingrp, uint8_t subgrp)
{ return (uint16_t) ( ((maingrp&0x1F)<<11) + subgrp ); }

KnxDevice_StatusType KnxDevice_Begin(HardwareSerial& serial, word physicalAddr);

// Stop the KNX Device
void KnxDevice_End(KnxDeviceType * device);

/* KNX device execution task */
// This function shall be called in the loop
void KnxDevice_Task(KnxDeviceType * device);

KnxDevice_StatusType KnxDevice_Read(uint8_t objectIndex, uint8_t returnedValue[]);

KnxDevice_StatusType KnxDevice_Write(uint8_t objectIndex, uint8_t valuePtr[]);

void KnxDevice_Update(KnxDeviceType * device, uint8_t objectIndex);

boolean KnxDevice_IsActive(KnxDeviceType * device);

/*==================[internal function definitions]=========================*/

static void KnxDevice_GetTpUartEvents(KnxTpUart_EventType event);

static void KnxDevice_TxTelegramAck(KnxTpUart_TxAckType txAck);

#endif /* #ifndef KNX_DEVICE_H */

/*==================[end of file]===========================================*/
