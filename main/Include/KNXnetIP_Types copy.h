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

#include "KNXnetIP.h"
#include "Std_Types.h"

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

/* Device Classes */
typedef enum {
    KNX_DEV_CLASS_A, /* Configuration and System Maintenance Tools */
    KNX_DEV_CLASS_B, /* KNXnet/IP Router */
    KNX_DEV_CLASS_C  /* any other KNXnet/IP device*/
} KNXnetIP_DeviceClassType;

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

/* Connection Types */
typedef enum {
    KNX_DEVICE_MGMT_CONNECTION = 0x03U, /* Data connection used to configure a KNXnet/IP device. */
    KNX_TUNNEL_CONNECTION      = 0x04U, /* Data connection used to forward KNX telegrams between two KNXnet/IP devices. */
    KNX_REMLOG_CONNECTION      = 0x06U, /* Data connection used for configuration and data transfer with a remote logging server. */
    KNX_REMCONF_CONNECTION     = 0x07U, /* Data connection used for data transfer with a remote configuration server. */
    KNX_OBJSVR_CONNECTION      = 0x08U, /* Data connection used for configuration and data transfer with an Object Server in a KNXnet/IP device. */
} KNXnetIP_ConnectionType;

/* Secure session status codes */
typedef enum {
    STATUS_AUTHENTICATION_SUCCESS,
    STATUS_AUTHENTICATION_FAILED,
    STATUS_UNAUTHENTICATED,
    STATUS_TIMEOUT,
    STATUS_CLOSE,
    STATUS_KEEPALIVE,
} KNXnetIP_SessionStatusType;

/* Description type codes */
typedef enum {
    KNX_DESC_DEVICE_INFO          = 0x01U,
    KNX_DESC_SUPP_SVC_FAMILIES    = 0x02U,
    KNX_DESC_IP_CONFIG            = 0x03U,
    KNX_DESC_IP_CUR_CONFIG        = 0x04U,
    KNX_DESC_KNX_ADDRESSES        = 0x05U,
    KNX_DESC_SECURED_SERVICES     = 0x06U,
    KNX_DESC_TUNNELING_INFO       = 0x07U,
    KNX_DESC_EXTENDED_DEVICE_INFO = 0x08U,
    /* reserved 09h - FDh */
    KNX_DESC_MFR_DATA             = 0xFEU,
    KNX_DESC_NOT_USED             = 0xFFU,
} KNXnetIP_DescType;

typedef enum {
    KNX_COMMAND_READ            = 0x00U,
    KNX_COMMAND_ANSWER          = 0x01U,
    KNX_COMMAND_WRITE           = 0x02U,
    KNX_COMMAND_INDV_ADDR_WRTIE = 0x03U,
    KNX_COMMAND_INDV_ADDR_REQ   = 0x04U,
    KNX_COMMAND_INDV_ADDR_RESP  = 0x05U,
    KNX_COMMAND_ADC_READ        = 0x06U,
    KNX_COMMAND_ADC_ANSWER      = 0x07U,
    KNX_COMMAND_MEM_READ        = 0x08U,
    KNX_COMMAND_MEM_ANSWER      = 0x09U,
    KNX_COMMAND_MEM_WRITE       = 0x0AU,
    KNX_COMMAND_UNKNOWN         = 0x0BU,
    KNX_COMMAND_MASK_VER_READ   = 0x0CU,
    KNX_COMMAND_MASK_VER_RESP   = 0x0DU,
    KXN_COMMAND_RESTART         = 0x0EU,
    KXN_COMMAND_ESCAPE          = 0x0FU,
} KNXnetIP_CommandType;

typedef enum {
    KNX_CEMI_MSG_DATA_REQ = 0x11U,
    KNX_CEMI_MSG_DATA_IND = 0x29U,
    KNX_CEMI_MSG_DATA_CON = 0x2EU,
} KNXnetIP_CEMIMsgType;

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
