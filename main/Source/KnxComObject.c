/**
 * \file KnxComObject.c
 * 
 * \brief KNX Object
 * 
 * This file contains the implementation of Knx ComObject module
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

#include "KnxComObject.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

void KnxComObject_CopyToTelegram(KnxComObject_Type * src, KnxTelegram_Type * dst)
{
    dst->priority = src->priority;
    dst->targetAddress = src->address;
    dst->payloadLength = src->length;
}

/*==================[end of file]===========================================*/
