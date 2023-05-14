/**
 * \file Knx_Types.h
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

#ifndef KNX_TYPES_H
#define KNX_TYPES_H

/*==================[inclusions]============================================*/
#include "esp_system.h"
#include "Pdu.h"

/*==================[macros]================================================*/
/* INDICATOR field : B7  B6  B5  B4  B3  B2  B1  B0 */
/*                   xx  xx   C   R   W   T   U   I */
#define KNX_OBJ_C_INDICATOR	(0x20U) /* Communication (C) */
#define KNX_OBJ_R_INDICATOR	(0x10U) /* Read (R) */
#define KNX_OBJ_W_INDICATOR	(0x08U) /* Write (W) */
#define KNX_OBJ_T_INDICATOR	(0x04U) /* Transmit (T) */
#define KNX_OBJ_U_INDICATOR	(0x02U) /* Update (U) */
#define KNX_OBJ_I_INDICATOR	(0x01U) /* Init Read (I) */

#define GROUP_ADDRESS_3L(main, mid, sub) ((const uint16_t)(((main & 0x1FU) << 11U) + ((mid & 0x07U) << 8U) + sub))
#define GROUP_ADDRESS_2L(main, sub) ((const uint16_t)((main & 0x1FU) << 11) + sub)
#define INDV_ADDRESS_3L(main, mid, sub) GROUP_ADDRESS_3L(main, mid, sub)
#define INDV_ADDRESS_2L(main, sub) GROUP_ADDRESS_2L(main, sub)
#define PHYSICAL_ADDRESS(area, line, busdevice) ((const uint16_t)(((area & 0x0FU) << 12) + ((line & 0x0FU) << 8) + busdevice))

/* Identifier for KNXnet/IP protocol version 1.0 */
#define KNXNETIP_VERSION_10 (0x10U)

/* Constant size of KNXnet/IP header as defined in protocol version 1.0 */
#define HEADER_SIZE_10      (0x06U)

/* Tunnelling CONNECT_ACK error codes */
#define E_TUNNELLING_LAYER (0x29U) /* The KNXnet/IP Server device does not support the requested KNXnet/IP Tunnelling layer. */

/* Timeout constants (seconds) */
#define CONNECT_REQUEST_TIMEOUT               (10U) /* KNXnet/IP Client shall wait for 10 seconds for a CONNECT_RESPONSE frame from KNXnet/IP Server. */
#define CONNECTIONSTATE_REQUEST_TIMEOUT       (10U) /* KNXnet/IP Client shall wait for 10 seconds for a CONNECTIONSTATE_RESPONSE frame from KNXnet/IP Server. */
#define DEVICE_CONFIGURATION_REQUEST_TIMEOUT  (10U) /* KNXnet/IP Client shall wait for 10 seconds for a DEVICE_CONFIGURATION_RESPONSE frame from KNXnet/IP Server. */
#define TUNNELLING_REQUEST_TIMEOUT              (1U) /* KNXnet/IP Client shall wait for 1 second for a TUNNELLING_ACK response on a TUNNELLING_REQUEST frame from KNXnet/IP Server */
#define CONNECTION_ALIVE_TIME                (120U) /* If the KNXnet/IP Server does not receive a heartbeat request within 120 seconds of the last correctly received message frame, the server shall terminate the connection by sending a DISCONNECT_REQUEST to the client’s control endpoint. */

#define KNX_TUNNELLING_SLOT_USABLE_MASK     (0x04U)
#define KNX_TUNNELLING_SLOT_AUTHORIZED_MASK (0x02U)
#define KNX_TUNNELLING_SLOT_FREE_MASK       (0x01U)

#define KNX_MANUFACTURER_CODE_MDT (0x0083U)
/*==================[type definitions]======================================*/
typedef enum {
  ExtendedFrame,
  StandardFrame,
} FrameFormatType;

typedef enum {
  SystemPriority,
  NormalPriority,
  UrgentPriority,
  LowPriority,
} PriorityType;

typedef enum {
  IndividualAddress,
  GroupAddress,
} AddressType;

typedef enum {
  AckDontCare = 0x00U,
  AckRequested = 0x02U,
} AckType;

typedef enum {
    UnlimitedRouting,
    NetworkLayerParameter,
} HopCountType;

typedef enum {
  SystemBroadcast = 0x00U,
  Broadcast = 0x10U,
} BroadcastType;

typedef enum {
  /* L_Data */
  L_data_req = 0x11U,
  L_data_con = 0x2EU,
  L_data_ind = 0x29U,

  /* Data Property */
  M_PropRead_req = 0xFCU,
  M_PropRead_con = 0xFBU,
  M_PropWrite_req = 0xF6U,
  M_PropWrite_con = 0xF5U,
  M_PropInfo_ind = 0xF7U,

  /* Function Property */
  M_FuncPropCommand_req = 0xF8U,
  M_FuncPropCommand_con = 0xFAU,
  M_FuncPropStateRead_req = 0xF9U,
  M_FuncPropStateRead_con = 0xFAU,

  /* Other */
  M_Reset_ind = 0xF0U,
  M_Reset_req = 0xF1U,

} MessageCodeType;

typedef enum {
    None,
    Auth,
    AuthConf,
} SecComType;

typedef struct {
    uint8_t SubnetworkAddr; /* Area Address (4-bit) | Line Address (4-bit) */
    uint8_t DeviceAddr;
} KNXnetIP_IndividualAddrType;

typedef struct {
    uint8_t MainGroup;
    uint8_t MiddleGroup;
    uint8_t SubGroup;
} KNXnetIP_GroupAddressType;

typedef struct {
    uint16_t address;
    uint8_t dataPointId;
} Knx_ObjectType;

typedef unsigned short KNXnetIP_TotalLengthType;

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
    SEARCH_REQUEST_EXTENDED  = 0x020BU,
    SEARCH_RESPONSE_EXTENDED = 0X020CU,

    /* KNXnet/IP Device Management Services */
    DEVICE_CONFIGURATION_REQUEST = 0x0310U,
    DEVICE_CONFIGURATION_ACK     = 0x0311U,

    /* KNXnet/IP Tunneling Services */
    TUNNELLING_REQUEST          = 0x0420U,
    TUNNELLING_ACK              = 0x0421U,
    TUNNELLING_FEATURE_GET      = 0x0422U,
    TUNNELLING_FEATURE_RESPONSE = 0x0423U,
    TUNNELLING_FEATURE_SET      = 0x0424U,
    TUNNELLING_FEATURE_INFO     = 0x0425U,

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

typedef enum {
    CH_FREE,
    CH_CONNECTED,
} KNXnetIP_ChannelStatusType;

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
    uint8_t ChannelId;
    KNXnetIP_ChannelStatusType ChannelStatus;
} KNXnetIP_ChannelType;

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
    uint8_t CommunicationChannelId;
    uint8_t SequenceCounter;
    uint8_t reserved;
} KNXnetIP_TunnellingConnectionHeaderType;

typedef struct {
    uint8_t CtrlField1;
    uint8_t CtrlField2;
    uint16_t SourceAddr;
    uint16_t DestAddr;
    uint8_t DataLength;
    TPCIType Tpci;
    APCIType Apci_data;
    uint8_t * Data;
} KNXnetIP_ServiceInformationType;

typedef struct {
    MessageCodeType MessageCode;
    uint8_t AdditionalInfoLength;
    uint8_t * AdditionalInformation;
    KNXnetIP_ServiceInformationType * ServiceInformation;
} KNXnetIP_TunnelingCemiFrameType;

typedef struct {
    KNXnetIP_ServiceFamilyIdType ServiceFamilyId;
    uint8_t ServiceFamilyVersion;
} KNXnetIP_ServiceFamilyType;

typedef enum {
    TX_IDLE,
    TX_QUEUED,
    TX_OVFL,
    TX_SOCK_FAIL,
    TX_ERROR,
} KNXnetIP_SendStatusType;

typedef struct {
    uint32_t ipAddress;
    uint32_t port;
    KNXnetIP_SendStatusType status;
    uint16_t txLength;
} KNXnetIP_SendQueueType;

typedef struct {
    uint16_t IndvAddr;
    uint16_t SlotStatus;
} KNXnetIP_TunnelingSlotType;

typedef enum {
    SUPPORTED_EMI_TYPE = 0x01U,
    HOST_DEVICE_DESC_TYPE_0,
    BUS_CONNECTION_STATUS,
    KNX_MANUFACTURER_CODE,
    ACTIVE_EMI_TYPE,
    INFO_SERVICE_EN = 0x08U,
} KNXnetIP_FeatureIdentifierType;

typedef enum {
    EMI1 = 0x01U,
    EMI2,
    CEMI,
} KNX_EMIType;

typedef enum {
    NOT_SUPPORTED,
    EMI1_ONLY,
    EMI2_ONLY,
    EMI1_EMI2,
    CEMI_ONLY,
    CEMI_EMI1,
    CEMI_EMI2,
    CEMI_EMI1_EMI2,
} Knx_SupportedEMIType;

typedef enum {
    KNXNETIP_SYS7 = 0x5705,
} Knx_DeviceDescriptorType0;

typedef struct {
    Knx_SupportedEMIType SupportedEMIType;
    Knx_DeviceDescriptorType0 DeviceDescType0;
    bool BusStatus;
    uint16_t ManufacturerCode;
    uint16_t ActiveEMIType;
    bool InfoServiceEnable;
} KNXnetIP_TunnellingFeatureType;

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

#endif /* #ifndef KNX_TYPES_H */
