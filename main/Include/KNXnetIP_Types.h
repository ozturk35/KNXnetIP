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

/* Internet Protocol Constants */
#define PORT_NUMBER (3671U)
#define SYSTEM_SETUP_MULTICAST_ADDRESS ("224.0.23.12") /* or Hex: 0xE000170C */

/*==================[type definitions]======================================*/
typedef unsigned short KNXnetIP_TotalLengthType;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

/* Service IDs */
typedef enum {
    /* KNXnet/IP Core Services */
    SEARCH_REQUEST           = 0x0201U,
    SEARCH_RESPONSE          = 0x0202U,
    DESCRIPTION_REQUEST      = 0x0203U,
    DESCRIPTION_RESPONSE     = 0x0204U,
    CONNECT_REQUEST          = 0x0205U,
    CONNECT_RESPONSE         = 0x0206U,
    CONNECTIONSTATE_REQUEST  = 0x0207U,
    CONNECTIONSTATE_RESPONSE = 0x0208U,
    DISCONNECT_REQUEST       = 0x0209U,
    DISCONNECT_RESPONSE      = 0x020AU,

    /* KNXnet/IP Device Management Services */
    DEVICE_CONFIGURATION_REQUEST = 0x0310U,
    DEVICE_CONFIGURATION_ACK     = 0x0311U,

    /* KNXnet/IP Tunneling Services */
    TUNNELING_REQUEST = 0x0420U,
    TUNNELING_ACK     = 0x0421U,

    /* KNXnet/IP Secure Services */
    SECURE_WRAPPER       = 0x0950U,
    SESSION_REQUEST      = 0x0951U,
    SESSION_RESPONSE     = 0x0952U,
    SESSION_AUTHENTICATE = 0x0953U,
    SESSION_STATUS       = 0x0954U,
    TIMER_NOTIFY         = 0x0955U,
} KNXnetIP_ServiceType;

typedef enum {
    DEVICE_MGMT_CONNECTION = 0x03U,
    TUNNEL_CONNECTION      = 0x04U,
    REMLOG_CONNECTION      = 0x06U,
    REMCONF_CONNECTION     = 0x07U,
    OBJSVR_CONNECTION      = 0x08U,
} KNXnetIP_ConnectionType;

typedef enum {
    IPV4_UDP = 0x01U,
    IPV4_TCP = 0x02U,
} KNXnetIP_HostProtocolCodeTpe;

typedef enum {
    KNXNETIP_CORE = 0x02U,
    KNXNETIP_DEVICEMGMT,
    KNXNETIP_TUNNELLING,
    KNXNETIP_ROUTING,
    KNXNETIP_REMOTELOGGING,
    KNXNETIP_REMOTECONFIGDIAG,
    KNXNETIP_OBJECTSERVER,
    KNXNETIP_SECURE
} KNXnetIP_ServiceFamilyIdType;

typedef enum {
    E_NO_ERROR              = 0x00U,
    E_HOST_PROTOCOL_TYPE    = 0x01U,
    E_VERSION_NOT_SUPPORTED = 0x02U,
    E_SEQUENCE_NUMBER       = 0x04U,
    E_ERROR                 = 0x0FU,
    E_CONNECTION_ID         = 0x21U,
    E_CONNECTION_TYPE       = 0x22U,
    E_CONNECTION_OPTION     = 0x23U,
    E_NO_MORE_CONNECTIONS   = 0x24U,
    E_DATA_CONNECTION       = 0x26U,
    E_KNX_CONNECTION        = 0x27U,
    E_AUTHORISATION_ERROR   = 0x28U,
    E_TUNNELNIG_LAYER       = 0x29U,
    E_NO_TUNNELLING_ADDRESS = 0x2DU,
    E_CONNECTION_IN_USE     = 0x2EU,
} KNXnetIP_ErrorCodeType;

typedef enum {
    DEVICE_INFO = 0x01U,
    SUPP_SVC_FAMILIES,
    IP_CONFIG,
    IP_CUR_CONFIG,
    KNX_ADDRESSES,
    SECURED_SERVICES,
    TUNNELLING_INFO,
    EXTENDED_DEVICE_INFO,
    MFR_DATA = 0xFEU,
} KNXnetIP_DIBType;

typedef enum {
    KNX_TP1   = 0x02U,
    KNX_PL110 = 0x04U,
    KNX_RF    = 0x10U,
    KNX_IP    = 0x20U,
} KNXnetIP_MediumType;

/**
  * KNXnet/IP header binary format

    +-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+-7-+-6-+-5-+-4-+-3-+-2-+-1-+-0-+
    | Header Size                   | Protocol Version              |
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
    uint8_t  HeaderSize;
    uint8_t  ProtocolVersion;
    KNXnetIP_ServiceType ServiceType;
    uint16_t TotalLength;
    uint8_t * Data;
} KNXnetIP_FrameType;

typedef struct {
    uint8_t StructureLength;
    uint8_t HostProtocolCode;
    uint32_t ipAddress;
    uint16_t portNumber;
} KNXnetIP_HPAIType;

typedef struct {
    uint8_t StructureLength;
    KNXnetIP_ConnectionType ConnectionTypeCode;
    uint8_t * HostProtocolDataPtr;
} KNXnetIP_CRIType;

typedef struct {
    uint8_t StructureLength;
    KNXnetIP_ConnectionType ConnectionTypeCode;
    uint8_t * HostProtocolIndepDataPtr;
    uint8_t * HostProtocolDepDataPtr;
} KNXnetIP_CRDType;

typedef struct {
    uint8_t StructureLength;
    KNXnetIP_DIBType DescriptionTypeCode;
    KNXnetIP_MediumType Medium;
    uint8_t DeviceStatus;
    uint16_t IndividualAddr;
    uint16_t ProjectInstallationId;
    uint8_t SerialNumber[6];
    uint32_t RoutingMulticastAddr;
    uint8_t MACAddr[6];
    uint8_t FriendlyName[30];
} KNXnetIP_DIBDeviceInfoType;

typedef struct {
    KNXnetIP_ServiceFamilyIdType ServiceFamilyId;
    uint8_t ServiceFamilyVersion;
} KNXnetIP_ServiceFamilyType;

typedef struct {
    uint8_t StructureLength;
    KNXnetIP_DIBType DescriptionTypeCode;
    KNXnetIP_ServiceFamilyType * SupportedServiceFamilies;
} KNXnetIP_DIBSupportedServicesType;

typedef struct {
    KNXnetIP_HPAIType hpai;
} KNXnetIP_SearchRequestFrameType;

typedef struct {
    KNXnetIP_HPAIType hpai;
    KNXnetIP_DIBDeviceInfoType dibDevInfo;
    KNXnetIP_DIBSupportedServicesType dibSuppSrv;
} KNXnetIP_SearchResponseFrameType;

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
