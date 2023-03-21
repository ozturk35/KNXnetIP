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
void KNXnetIP_ConnectRequest()
{

}

void KNXnetIP_ConnectResponse()
{

}

void KNXnetIP_ConnectionStateRequest()
{

}

void KNXnetIP_ConnectionStateResponse()
{

}

void KNXnetIP_DisconnectRequest()
{

}

void KNXnetIP_DisconnectResponse()
{

}

void KNXnetIP_TunnellingRequest()
{

}

void KNXnetIP_TunnellingAck()
{

}

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

static KNXnetIP_GroupAddressType KNXnetIP_GetGroupAddressFromStr(uint8_t * str)
{
    KNXnetIP_GroupAddressType l_groupAddress;

    /* Data is expected to be stirng in format M/M/S e.g. 1/0/4 */
    /* */

    uint8_t * token;
    token = strtok(str,"/");

    l_groupAddress.MainGroup   = 1;
    l_groupAddress.MiddleGroup = 2;
    l_groupAddress.SubGroup    = 4;

    return l_groupAddress;
}

static KNXnetIP_GroupAddressType KNXnetIP_GetGroupAddressFromFullAddress(uint8_t * fullAddress)
{
    KNXnetIP_GroupAddressType l_groupAddress;

    /* Data is expected to be stirng in format M/M/S e.g. 1.2.5@2.3.4 */
    /* */

    /* Check if address is in correct format */

    /* return 2nd part of the FullAddress */

    return l_groupAddress;

}

static KNXnetIP_GroupAddressType KNXnetIP_GetGroupAddressFromData(uint8_t * bytes)
{
    KNXnetIP_GroupAddressType l_groupAddress;

    l_groupAddress.MainGroup = bytes[0] >> 3;
    l_groupAddress.MiddleGroup = bytes[0] & 0x07;
    l_groupAddress.SubGroup = bytes[1];

    return l_groupAddress;
}

static KNXnetIP_IndividualAddrType KNXnetIP_GetIndividualAddressFromStr(uint8_t * str)
{
    KNXnetIP_IndividualAddrType l_individualAddr;

    /* Data is expected to be string in format 3.0.6 */
    /* Split the dots. */

    l_individualAddr.SubnetworkAddr  = str[0] << 4; /* Area */
    l_individualAddr.SubnetworkAddr |= str[1];      /* Line */
    l_individualAddr.DeviceAddr      = str[2];      /* Bus Device */

    return l_individualAddr;
}

static KNXnetIP_HPAIFrameType KNXnetIP_GetHPAIFromData(string ipAddress, uint16_t port, uint8_t length, uint8_t hostProtocolCode)
{
    KNXnetIP_HPAIFrameType l_hpaiFrame;
    /* ipAddress is expected to be in format IPv4 string */
    /* port is expected to be an integer */
    l_hpaiFrame.StructureLength  = length;
    l_hpaiFrame.HostProtocolCode = hostProtocolCode;
    l_hpaiFrame.HostProtocolDataPtr = (void *) 0;

    return l_hpaiFrame;
}


static void KNXnetIP_IP2TPFrame(KNXnetIP_FrameType * ipFrame, unsigned char * tpFrame)
{
    /* Set-up KNX TP Header */
    tpFrame[0] = 0x06; /* Control Field 1 */
    tpFrame[1] = 0x10; /* Control Field 2 */
    tpFrame[2] = 0x04; /* Standard Frame Format */

    /* Set-up KNX TP Address */
    tpFrame[3] = ipFrame[4]; /* TP Address byte 1 */
    tpFrame[4] = ipFrame[5]; /* TP Address byte 2 */

    /* Set-up KNX TP Data */
    tpFrame[5] = ipFrame[7]; /* TP Data Byte 1 */
    tpFrame[6] = ipFrame[8]; /* TP Data Byte 2 */

    /* Set-up KNX TP Checksum */
    tpFrame[7] = 0x00; /* TP Checksum */
}

static void KNXnetIP_DecodeFrame(KNXnetIP_FrameType * data)
{
    
}

/*==================[end of file]===========================================*/
