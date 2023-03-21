/**
 * \file KNXnetIP_Server.c
 * 
 * \brief KNX IP Network Server
 * 
 * This file contains the implementation of KNX module IP Network Server.
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
#include "KNXnetIP_Api.h"
#include "KNXnetIP_Cfg.h"

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/
typedef struct {
    unsigned short IndividualAddress;
    unsigned char * Name;
    unsigned char * FriendlyName;
    unsigned char IsRunning;
    unsigned char InShutdown;
    unsigned char Discovery;


} KNXnetIP_ServerType;

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

void KNXnetIP_Server_Main()
{
/*
    • If a server receives a data packet with an unsupported version field, it shall reply with a negative
      confirmation frame indicating in the status field E_VERSION_NOT_SUPPORTED.

    • If an invalid data packet is received, the implementation shall ignore the data packet without taking
      any further action.

    • If a connection is established, all data packets shall be sent with the same protocol version.

    • If a connection is established and the protocol version changes within the received data packets, the
    server shall shut down the connection.

    • A KNXnet/IP Server shall at least be able to process KNXnet/IP frames with a total length of 
    508 octets. Clause 8.4.3.3 specifies how a server shall handle the reception of KNXnet/IP frames
    longer than the supported frame length.
*/



}

/*==================[end of file]===========================================*/
