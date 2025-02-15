/**
 * \file KNXnetIP.h
 * 
 * \brief KNXnet/IP
 * 
 * This file contains the implementation of KNXmodule KNXnet/IP
 * 
 * \version 1.0.0
 * 
 * \author Ibrahim Ozturk
 * 
 * Copyright 2023 Ibrahim Ozturk
 * All rights exclusively reserved for Ibrahim Ozturk,
 * unless expressly agreed to otherwise.
*/

#ifndef KNXNETIP_H
#define KNXNETIP_H

/*==================[inclusions]============================================*/
#include "Knx_Types.h"
#include "KNXnetIP_Core.h"
#include "KNXnetIP_Tunnelling.h"
#include "KNXnetIP_UdpServer.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/
extern KNXnetIP_ChannelType KNXnetIP_Channel[KNX_CHANNEL_NUM];
extern bool KNXnetIP_TcpTxTunnelReqPending;
extern bool KNXnetIP_Connected;
extern uint32_t KNXnetIP_TcpIpAddr;
extern int KNXnetIP_TcpSock;

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/

#endif /* #ifndef KNXNETIP_H */
