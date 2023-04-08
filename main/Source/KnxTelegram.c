/**
 * \file KnxTelegram.c
 * 
 * \brief KNX Telegram
 * 
 * This file contains the implementation of Knx Telegram module
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

#include "KnxTelegram.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

void KnxTelegram_Clear(KnxTelegram_Type * telegram)
{
// clear telegram with default values :
// std FF, no repeat, normal prio, empty payload
// multicast, routing counter = 6, payload length = 1
  for (uint8_t i =0; i < KNX_TELEGRAM_MAX_SIZE; i++) 
  {
    telegram->bytes[i] = 0;
  }

  telegram->l_data.controlField = CONTROL_FIELD_DEFAULT_VALUE;
  telegram->l_data.length       = LENGTH_FIELD_DEFAULT_VALUE;
}

void KnxTelegram_SetLongPayload(KnxTelegram_Type * telegram, uint8_t * src, uint8_t length) 
{
  if (length > KNX_TELEGRAM_PAYLOAD_MAX_SIZE - 2)
  {
    length = KNX_TELEGRAM_PAYLOAD_MAX_SIZE - 2;
  }

  for(uint8_t i = 0; i < length; i++)
  {
    telegram->l_data.payloadChecksum[i] = src[i];
  }
}

void KnxTelegram_ClearLongPayload(KnxTelegram_Type * telegram)
{
  for(uint8_t i = 0; i < KNX_TELEGRAM_PAYLOAD_MAX_SIZE - 1; i++)
  {
    telegram->l_data.payloadChecksum[i] = 0;
  }
}

void KnxTelegram_GetLongPayload(KnxTelegram_Type * telegram, uint8_t * dest, uint8_t length)
{
  if (length > KNX_TELEGRAM_PAYLOAD_MAX_SIZE - 2)
  {
    length = KNX_TELEGRAM_PAYLOAD_MAX_SIZE - 2;
  }

  for(uint8_t i = 0; i < length; i++)
  {
    dest[i] = telegram->l_data.payloadChecksum[i];
  }
}

uint8_t KnxTelegram_GetChecksum(KnxTelegram_Type * telegram)
{
  return telegram->l_data.payloadChecksum[(telegram->l_data.length & LENGTH_FIELD_PAYLOAD_LENGTH_MASK) - 1];
}

uint8_t KnxTelegram_CalculateChecksum(KnxTelegram_Type * telegram)
{
  uint8_t indexChecksum, xorSum = 0;

  indexChecksum = KNX_TELEGRAM_HEADER_SIZE + (telegram->l_data.length & LENGTH_FIELD_PAYLOAD_LENGTH_MASK) + 1;

  for (uint8_t i = 0; i < indexChecksum ; i++)
  {
    xorSum ^= telegram->bytes[i]; // XOR Sum of all the databytes
  }

  return (uint8_t)(~xorSum); // Checksum equals 1's complement of databytes XOR sum
}

void KnxTelegram_UpdateChecksum(KnxTelegram_Type * telegram)
{
  uint8_t indexChecksum, xorSum=0; 

  indexChecksum = KNX_TELEGRAM_HEADER_SIZE + (telegram->l_data.length & LENGTH_FIELD_PAYLOAD_LENGTH_MASK) + 1;

  for (uint8_t i = 0; i < indexChecksum ; i++)
  {
    xorSum ^= telegram->bytes[i]; // XOR Sum of all the databytes
  }

  telegram->bytes[indexChecksum] = ~xorSum; // Checksum equals 1's complement of databytes XOR sum
}

void KnxTelegram_Copy(KnxTelegram_Type * dest, KnxTelegram_Type * src)
{
  uint8_t length = KNX_TELEGRAM_LENGTH_OFFSET + (src->l_data.length & LENGTH_FIELD_PAYLOAD_LENGTH_MASK);

  for (uint8_t i=0; i<length ; i++)
  {
    dest->bytes[i] = src->bytes[i];
  }
}

void KnxTelegram_CopyHeader(KnxTelegram_Type * dest, KnxTelegram_Type * src)
{
  for(uint8_t i=0; i < KNX_TELEGRAM_HEADER_SIZE; i++)
  {
    dest->bytes[i] = src->bytes[i];
  }
}

KnxTelegram_ValidityType KnxTelegram_GetValidity(KnxTelegram_Type * telegram)
{
  KnxTelegram_ValidityType retVal = KNX_TELEGRAM_VALID;

  if (CONTROL_FIELD_VALID_PATTERN != (telegram->l_data.controlField & CONTROL_FIELD_PATTERN_MASK))
  {
    retVal = KNX_TELEGRAM_INVALID_CONTROL_FIELD; 
  }
  else if (CONTROL_FIELD_STANDARD_FRAME_FORMAT != (telegram->l_data.controlField & CONTROL_FIELD_FRAME_FORMAT_MASK))
  {
    retVal = KNX_TELEGRAM_UNSUPPORTED_FRAME_FORMAT;
  }
  else if (!(telegram->l_data.length & LENGTH_FIELD_PAYLOAD_LENGTH_MASK))
  {
    retVal = KNX_TELEGRAM_INCORRECT_PAYLOAD_LENGTH;
  }
  else if (COMMAND_FIELD_VALID_PATTERN != (((telegram->l_data.controlField & 0xFF00U) >> 8) & COMMAND_FIELD_PATTERN_MASK))
  {
    retVal = KNX_TELEGRAM_INVALID_COMMAND_FIELD;
  }
  else if (KnxTelegram_GetChecksum(telegram) != KnxTelegram_CalculateChecksum(telegram))
  {
    retVal = KNX_TELEGRAM_INCORRECT_CHECKSUM;
  }
  else
  {
    uint8_t cmd = telegram->l_data.cmd;

    if ((KNX_COMMAND_VALUE_READ != cmd) && (KNX_COMMAND_VALUE_RESPONSE != cmd) 
        && (KNX_COMMAND_VALUE_WRITE != cmd) && (KNX_COMMAND_MEMORY_WRITE != cmd))
    {
      retVal = KNX_TELEGRAM_UNKNOWN_COMMAND;
    }
  }

  return retVal;
}

/*==================[end of file]===========================================*/
