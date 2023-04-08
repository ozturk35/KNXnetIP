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

#include "KnxTpUart2.h"
#include "KnxTpUart2_Services.h"
#include "KnxDevice.h"

void KnxDevice_Task(KnxDeviceType * device)
{
  Knx_TxActionType action;
  uint16_t nowTimeMs, nowTimeUs;

  if(FALSE == device->initCompleted)
  {
    nowTimeMs = KnxTpUart2_GetTimeMs();

    if ((nowTimeMs - device->lastInitTimeMs) > 500)
    {
      while ((device->initIndex < device->comObjectsNb) && (device->comObjectList[device->initIndex].valid))
      {
        device->initIndex++;
      };

      if (device->initIndex == device->comObjectsNb)
      {
        device->initCompleted = TRUE;
      }
      else
      {
        action.ctrlField = A_GroupValue_Read;
        action.index = device->initIndex;
        device->txCmdList[device->txCmdLastIndex] = action;
        device->txCmdLastIndex++;
        device->lastInitTimeMs = KnxTpUart2_GetTimeMs();
      }
    }
  }

  nowTimeUs = KnxTpUart2_GetTimeUs();
  if (nowTimeUs - device->lastRxTimeUs > 400)
  {
    device->lastRxTimeUs = nowTimeUs;
    KnxTpUart2_Rx(device->cfg);
  }

  if ( IDLE == device->state)
  {
    if (device->txCmdLastIndex > 0)
    {
      switch (device->txCmdList->ctrlField)
      {
        case A_GroupValue_Read:
          device->txTelegram.l_data.controlField &= ~CONTROL_FIELD_PRIORITY_MASK;
          device->txTelegram.l_data.controlField |= KNX_PRIORITY_NORMAL_VALUE & CONTROL_FIELD_PRIORITY_MASK;
          device->txTelegram.l_data.sourceAddr = 
          device->txTelegram.l_data.destAddr = device->comObjectList[device->txCmdList->index].addr;
          device->txTelegram.l_data.length = device->comObjectList[device->txCmdList->index].length;
          device->txTelegram.l_data.cmd = KNX_COMMAND_VALUE_READ;
          KnxTelegram_UpdateChecksum(&(device->txTelegram));
          KnxTpUart2_SendTelegram(device->cfg->objCfg, &(device->txTelegram));
        case A_GroupValue_Response:
        case A_GroupValue_Write:
        default:
          break;
      }
    }
  }

  nowTimeUs = KnxTpUart2_GetTimeUs();
  if ((nowTimeUs - device->lastTxTimeUs) > 800)
  {
    device->lastTxTimeUs = nowTimeUs;
    KnxTpUart2_Tx(device->cfg);
  }
}