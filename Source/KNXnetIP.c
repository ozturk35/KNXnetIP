/**
 * \file KNXIPNet.c
 * 
 * \brief KNX IP Network
 * 
 * This file contains the implementation of KNX module IP Network.
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
#include "KNXnetIP_Cfg.h"
#include "KNXnetIP_Types.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

void KNXnetIP_GetLocalAddr(void);

void KNXnetIP_GetPhysAddr(void);

void KNXnetIP_GetPhysAddr(void);

void KNXnetIP_SendRequest(void);

void KNXnetIP_GetGroupAddr(void);

void KNXnetIP_SetGroupAddr(void);

void KNXnetIP_SendFrame(void);

void KNXnetIP_ReceiveFrame(void);

void KNXnetIP_Rx(void);

void KNXnetIP_Tx(void);

/*==================[internal function declarations]========================*/
static void KNXnetIP_Init();

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

static KNXnetIP_HeaderType KNXnetIP_CreateHeaderFromData(KNXnetIP_ServiceType service, KNXnetIP_TotalLengthType totalLength )
{
    KNXnetIP_HeaderType l_header;

    l_header.HeaderLength    = HEADER_SIZE_10;
    l_header.ProtocolVersion = KNXNETIP_VERSION_10;
    l_header.Service         = service;
    l_header.TotalLenght     = totalLength;

    return l_header;
}

static KNXnetIP_HeaderType KNXnetIP_CreateHeaderFromFrame(KNXnetIP_FrameType * frame)
{
    KNXnetIP_HeaderType l_header;

    l_header.HeaderLength    = frame[0];
    l_header.ProtocolVersion = frame[1];
    l_header.Service         = (KNXnetIP_ServiceType) (frame[2] << 8 | frame[3]);
    l_header.TotalLenght     = (KNXnetIP_TotalLengthType) (frame [4] << 8 | frame[5]);

    return l_header;
}

static void KNXnetIP_ConnectRequest()
{

}

static void KNXnetIP_ConnectResponse()
{

}

static void KNXnetIP_ConnectionStateRequest()
{

}

static void KNXnetIP_ConnectionStateResponse()
{

}

static void KNXnetIP_DisconnectRequest()
{

}

static void KNXnetIP_DisconnectResponse()
{

}


static void KNXnetIP_DecodeFrame(KNXnetIP_FrameType * data)
{
    
}

/*==================[end of file]===========================================*/
