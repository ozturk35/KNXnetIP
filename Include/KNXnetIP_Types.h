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

/* Constant size of KNXnet/IP header as defined in protocol version 1.0 */
#define HEADER_SIZE_10      0x06U

/* Identifier for KNXnet/IP protocol version 1.0 */
#define KNXNETIP_VERSION_10 0x10U

/* Error codes */
/* Common error codes */
#define E_NO_ERROR              (0x00U) /* Operation successful */
#define E_HOST_PROTOCOL_TYPE    (0x01U) /* The requested host protocol is not supported by the KNXnet/IP device. */
#define E_VERSION_NOT_SUPPORTED (0x02U) /* The requested protocol version is not supported by the KNXnet/IP device. */
#define E_SEQUENCE_NUMBER       (0x04U) /* The received sequence number is out of order. */

/* CONNECT RESPONSE status codes */
#define E_CONNECTION_TYPE     (0x22U) /* The KNXnet/IP Server device does not support the requested connection type. */
#define E_CONNECTION_OPTION   (0x23U) /* The KNXnet/IP Server device does not support one or more requested connection options. */
#define E_NO_MORE_CONNECTIONS (0x24U) /* The KNXnet/IP Server device cannot accept the new data connection because its maximum amount of concurrent connections is already used. */

/* CONNECTIONSTATE_RESPONSE status codes */
#define E_CONNECTION_ID   (0x21U) /* The KNXnet/IP Server device cannot find an active data connection with the specified ID. */
#define E_DATA_CONNECTION (0x26U) /* The KNXnet/IP Server device detects an error concerning the data connection with the specified ID. */
#define E_KNX_CONNECTION  (0x27U) /* The KNXnet/IP Server device detects an error concerning the KNX connection with the specified ID. */

/* Tunnelling CONNECT_ACK error codes */
#define E_TUNNELING_LAYER (0x29U) /* The KNXnet/IP Server device does not support the requested KNXnet/IP Tunnelling layer. */

/* Timeout constants (seconds) */
#define CONNECT_REQUEST_TIMEOUT               (10U) /* KNXnet/IP Client shall wait for 10 seconds for a CONNECT_RESPONSE frame from KNXnet/IP Server. */
#define CONNECTIONSTATE_REQUEST_TIMEOUT       (10U) /* KNXnet/IP Client shall wait for 10 seconds for a CONNECTIONSTATE_RESPONSE frame from KNXnet/IP Server. */
#define DEVICE_CONFIGURATION_REQUEST_TIMEOUT  (10U) /* KNXnet/IP Client shall wait for 10 seconds for a DEVICE_CONFIGURATION_RESPONSE frame from KNXnet/IP Server. */
#define TUNNELING_REQUEST_TIMEOUT              (1U) /* KNXnet/IP Client shall wait for 1 second for a TUNNELING_ACK response on a TUNNELING_REQUEST frame from KNXnet/IP Server */
#define CONNECTION_ALIVE_TIME                (120U) /* If the KNXnet/IP Server does not receive a heartbeat request within 120 seconds of the last correctly received message frame, the server shall terminate the connection by sending a DISCONNECT_REQUEST to the client’s control endpoint. */

/* Internet Protocol constants */
#define KNXNETIP_PORT_NUMBER       (3671U)
#define IpAddress(ipaddr)
#define KNXNETIP_MULTICAST_ADDRESS IpAddress("224.0.23.12")

/*==================[type definitions]======================================*/

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
    KNX_SRV_ROUTING_BUSY         = 0x0532U,

    /* KNXnet/IP Remote Logging */
    KNX_SRV_REMOTE_LOG_START = 0x0600U,
    KNX_SRV_REMOTE_LOG_END   = 0x06FFU,

    /* KNXnet/IP Remote Configuration and Diagnosis */
    KNX_SRV_REMOTE_DIAG_REQUEST         = 0x0740U,
    KNX_SRV_REMOTE_DIAG_RESPONSE        = 0x0741U,
    KNX_SRV_REMOTE_BASIC_CONFIG_REQUEST = 0x0742U,
    KNX_SRV_REMOTE_RESET_REQUEST        = 0x0743U,

    /* KNXnet/IP Object Server */

} KNXnetIP_ServiceType;

typedef enum {
    KNX_DEVICE_MGMT_CONNECTION = 0x03U,
    KNX_TUNNEL_CONNECTION      = 0x04U,
    KNX_REMLOG_CONNECTION      = 0x06U,
    KNX_REMCONF_CONNECTION     = 0x07U,
    KNX_OBJSVR_CONNECTION      = 0x08U,
} KNXnetIP_ConnectionType;

/* Description type codes */
typedef enum {
    KNX_DESC_DEVICE_INFO       = 0x01U,
    KNX_DESC_SUPP_SVC_FAMILIES = 0x02U,
    KNX_DESC_IP_CONFIG         = 0x03U,
    KNX_DESC_IP_CUR_CONFIG     = 0x04U,
    KNX_DESC_KNX_ADDRESSES     = 0x05U,
    KNX_DESC_RESERVED          = 0x06, /* 0x06 - 0x0FD */
    KNX_DESC_MFR_DATA          = 0xFEU,
    KNX_DESC_NOT_USED          = 0xFFU,
} KNXnetIP_DescType;

/* KNX medium codes */
typedef enum {
    KNX_MEDIUM_RESERVED  = 0x01U,
    KNX_MEDIUM_TP1       = 0x02U,
    KNX_MEDIUM_PL110     = 0x04U,
    KNX_NEDIUM_RESERVED2 = 0x08U,
    KNX_MEDIUM_RF        = 0x10U,
    KNX_MEDIUM_IP        = 0x20U,
} KNXnetIP_MediumType;

/* Host protocol codes */
typedef enum {
    KNX_HOSTPROTOCOL_IPV4_UDP = 0x01U,
    KNX_HOSTPROTOCOL_IPV6_TCP = 0x02U,
} KNXnetIP_HostProtocolType;

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

typedef uint8_t KNXnetIP_HeaderLengthType;
typedef uint8_t KNXnetIP_ProtocolVersionType;
typedef uint8_t KNXnetIP_HeaderLenghtType;
typedef uint8_t KNXnetIP_HeaderLenghtType;

/**
  * KNXnet/IP header binary format

    +-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+
    | Header Length                 | Protocol Version              |
    | (1 Octet)                     | (1 Octet)                     |
    +-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+
    | Service Type Identifier                                       |
    | (2 Octet)                                                     |
    +-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+
    | Total Length                                                  |
    | (2 Octet)                                                     |
    +-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+
*/
typedef struct {
    KNXnetIP_HeaderLengthType    KNXnetIP_HeaderLength;
    KNXnetIP_ProtocolVersionType KNXnetIP_ProtocolVersion;
    KNXnetIP_ServiceType         KNXnetIP_Service;
    unsigned int                 KNXnetIP_TotalLenght;
} KNXnetIP_HeaderType;

typedef unsigned char KNXnetIP_BodyType;

typedef struct 
{
    KNXnetIP_HeaderType KNXnetIP_Header;
    KNXnetIP_BodyType * KNXnetIP_Body;

} KNXnetIP_FrameType;


typedef struct {
    uint8_t StructureLength;
    uint8_t HostProtocolCode;
    uint8_t * HostProtocolDataPtr;
} KNXnetIP_HPAIFrameType;

typedef struct {
    uint8_t StructureLength;
    uint8_t ConnectionTypeCode;
    uint8_t * HostProtocolDataPtr;
} KNXnetIP_CRIFrameType;

typedef struct {
    uint8_t StructureLength;
    uint8_t ConnectionTypeCode;
    uint8_t * HostProtocolIndepDataPtr;
    uint8_t * HostProtocolDepDataPtr;
} KNXnetIP_CRDFrameType;

typedef struct {
    uint8_t StructureLength;
    uint8_t DescriptionTypeCode;
    KNXnetIP_MediumType KNXnetIP_Medium;
    uint8_t KNXnetIP_DeviceStatus;
    uint16_t KNXnetIP_IndividualAddr;
    uint16_t KNXnetIP_ProjectInstallationId;
    uint8_t KNXnetIP_SerialNumber[6];
    uint8_t KNXnetIP_RoutingMulticastAddr[6];
    uint8_t KNXnetIP_MACAddr[6];
    uint8_t KNXnetIP_FriendlyName[30];
} KNXnetIP_DeviceInfoIBFrameType;

typedef struct {
    uint8_t ServiceFamilyId;
    uint8_t ServiceFamilyVersion;
} KNXnetIP_ServiceFamilyType;

typedef struct {
    uint8_t StructureLength;
    uint8_t DescriptionTypeCode;
    KNXnetIP_ServiceFamilyType * SupportedServiceFamilies;
} KNXnetIP_SupportedServiceFamilyType;

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
