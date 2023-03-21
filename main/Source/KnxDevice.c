/**
 * \file KnxDevice.c
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
/*==================[inclusions]============================================*/

#include "KnxDevice.h"

uint8_t KnxDevice_CmdBuffer[64] = { 0 };

static inline uint16_t TimeDelta(uint16_t now, uint16_t before) { return (uint16_t)(now - before); }

/* Create a Knx Device */
KnxDeviceType KnxDevice = {
  .state = INIT,
  .tpuart = NULL,
  .txCmdList = &KnxDevice_CmdBuffer[0],
  .initCompleted = FALSE,
  .initIndex = 0,
  .rxTelegram = (uint8_t) NULL,
};
