/**
 * \file KNXnetIP_UdpServer.h
 * 
 * \brief KNXnet/IP Udp Server
 * 
 * This file contains the implementation of KNXnet/IP Udp Server
 * 
 * \version 1.0.0
 * 
 * \author Ibrahim Ozturk
 * 
 * Copyright 2023 Ibrahim Ozturk
 * All rights exclusively reserved for Ibrahim Ozturk,
 * unless expressly agreed to otherwise.
*/

#ifndef KNXNETIP_UDPSERVER_H
#define KNXNETIP_UDPSERVER_H

/*==================[inclusions]============================================*/

/*==================[macros]================================================*/

#define PORT (3671U)

/*==================[type definitions]======================================*/


/*==================[external function declarations]========================*/
extern void udp_server_task(void *pvParameters);

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/

#endif /* ifndef KNXNETIP_UDPSERVER_H */
