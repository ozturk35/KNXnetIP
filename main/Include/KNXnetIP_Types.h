/**
 * \file KNXnetIP_Types.h
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

#ifndef KNXNETIP_TYPES_H
#define KNXNETIP_TYPES_H

/*==================[inclusions]============================================*/

#include <KNXnetIP.h>

/*==================[macros]================================================*/

/* Identifier for KNXnet/IP protocol version 1.0 */
#define KNXNETIP_VERSION_10 (0x10U)

/* Constant size of KNXnet/IP header as defined in protocol version 1.0 */
#define HEADER_SIZE_10      (0x06U)

/* Tunnelling CONNECT_ACK error codes */
#define E_TUNNELING_LAYER (0x29U) /* The KNXnet/IP Server device does not support the requested KNXnet/IP Tunnelling layer. */

/* Timeout constants (seconds) */
#define CONNECT_REQUEST_TIMEOUT               (10U) /* KNXnet/IP Client shall wait for 10 seconds for a CONNECT_RESPONSE frame from KNXnet/IP Server. */
#define CONNECTIONSTATE_REQUEST_TIMEOUT       (10U) /* KNXnet/IP Client shall wait for 10 seconds for a CONNECTIONSTATE_RESPONSE frame from KNXnet/IP Server. */
#define DEVICE_CONFIGURATION_REQUEST_TIMEOUT  (10U) /* KNXnet/IP Client shall wait for 10 seconds for a DEVICE_CONFIGURATION_RESPONSE frame from KNXnet/IP Server. */
#define TUNNELING_REQUEST_TIMEOUT              (1U) /* KNXnet/IP Client shall wait for 1 second for a TUNNELING_ACK response on a TUNNELING_REQUEST frame from KNXnet/IP Server */
#define CONNECTION_ALIVE_TIME                (120U) /* If the KNXnet/IP Server does not receive a heartbeat request within 120 seconds of the last correctly received message frame, the server shall terminate the connection by sending a DISCONNECT_REQUEST to the client’s control endpoint. */

/*==================[type definitions]======================================*/
typedef unsigned short KNXnetIP_TotalLengthType;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

/* Service IDs */
typedef enum {
    /* KNXnet/IP Core */
    KNX_SRV_SEARCH_REQUEST           = 0x0201U,
    KNX_SRV_SEARCH_RESPONSE          = 0x0202U,
    KNX_SRV_DESCRIPTION_REQUEST      = 0x0203U,
    KNX_SRV_DESCRIPTION_RESPONSE     = 0x0204U,
    KNX_SRV_CONNECT_REQUEST          = 0x0205U,
    KNX_SRV_CONNECT_RESPONSE         = 0x0206U,
    KNX_SRV_CONNECTIONSTATE_REQUEST  = 0x0207U,
    KNX_SRV_CONNECTIONSTATE_RESPONSE = 0x0208U,
    KNX_SRV_DISCONNECT_REQUEST       = 0x0209U,
    KNX_SRV_DISCONNECT_RESPONSE      = 0x020AU,

    /* KNXnet/IP Device Management */
    KNX_SRV_DEVICE_CONFIGURATION_REQUEST = 0x0310U,
    KNX_SRV_DEVICE_CONFIGURATION_ACK     = 0x0311U,

    /* KNXnet/IP Tunneling */
    KNX_SRV_TUNNELING_REQUEST = 0x0420U,
    KNX_SRV_TUNNELING_ACK     = 0x0421U,

    /* KNXnet/IP Routing */
    KNX_SRV_ROUTING_INDICATION   = 0x0530U,
    KNX_SRV_ROUTING_LOST_MESSAGE = 0x0531U,
} KNXnetIP_ServiceType;

typedef struct {
    uint8_t HeaderLength;
    uint8_t ProtocolVersion;
    KNXnetIP_ServiceType Service;
    KNXnetIP_TotalLengthType TotalLenght;
} KNXnetIP_HeaderType;

typedef uint8_t KNXnetIP_FrameType;

typedef struct {
    uint8_t StructureLength;
    uint8_t HostProtocolCode;
    uint8_t * HostProtocolDataPtr;
} KNXnetIP_HPAIFrameType;

/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/

#endif /* ifndef KNXNETIP_TYPES_H */
