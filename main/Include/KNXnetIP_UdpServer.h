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

#define UDP_PORT (3671U)

#define MULTICAST_TTL IP_MULTICAST_TTL
#define MULTICAST_IPV4_ADDR "224.0.23.12"

/*==================[type definitions]======================================*/


/*==================[external function declarations]========================*/
void udp_mcast_task(void *pvParameters);
void tcp_server_task(void *pvParameters);

void KNXnetIP_UDPSend(uint32_t ipAddr, uint16_t port, uint8_t * txBuffer, uint16_t txLength);
void KNXnetIP_TcpUpdateTxBuffer(uint8_t * txBuffer, uint16_t txLength);

extern int create_unicast_ipv4_socket(uint32_t ipAddr, uint16_t port);
extern void tcp_transmitPendingTunnelReq(const int sock, uint32_t ipAddr, uint16_t port);

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
