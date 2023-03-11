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

#include <KNXnetIP_Cfg.h>

/*==================[macros]================================================*/

/*==================[type definitions]======================================*/

typedef enum {
    KNXIPNET_SERVICE_SEARCH_REQ           = 0x0201,
    KNXIPNET_SERVICE_SEARCH_RESP          = 0x0202,
    KNXIPNET_SERVICE_DESCRIPTION_REQ      = 0x0203,
    KNXIPNET_SERVICE_DESCRIPTION_RESP     = 0x0204,
    KNXIPNET_SERVICE_CONNECT_REQ          = 0x0205,
    KNXIPNET_SERVICE_CONNECT_RESP         = 0x0206,
    KNXIPNET_SERVICE_CONNECTIONSTATE_REQ  = 0x0207,
    KNXIPNET_SERVICE_CONNECTIONSTATE_RESP = 0x0208,
    KNXIPNET_SERVICE_DISCONNECT_REQ       = 0x0209,
    KNXIPNET_SERVICE_DISCONNECT_RESP      = 0x020A,
    

    KNXIPNET_SERVICE_DEVICE_CONFIG_REQ  = 0x0310,
    KNXIPNET_SERVICE_DEVICE_CONFIG_RESP = 0x0311,

    /* KNXnet/IP Tunneling Services */
    KNXIPNET_SERVICE_TUNNELING_REQ  = 0x0420,
    KNXIPNET_SERVICE_TUNNELING_RESP = 0x0421,

    /* KNXnet/IP Routing Services */
    KNXIPNET_SERVICE_ROUTING_INDICATION   = 0x0530,
    KNXIPNET_SERVICE_ROUTING_LOST_MESSAGE = 0x0531,
    KNXIPNET_SERVICE_ROUTING_BUSY         = 0x0532,

    KNX_SERVICE_RLOG_START = 0x0600,
    KNX_SERVICE_RLOG_END   = 0x06FF,

    KNX_SERVICE_REMOTE_DIAG_REQ         = 0x0740,
    KNX_SERVICE_REMOTE_DIAG_RESP        = 0x0741,
    KNX_SERVICE_REMOTE_BASIC_CONFIG_REQ = 0x0742,
    KNX_SERVICE_REMOTE_RESET_REQ        = 0x0743,
} KNXnetIP_ServiceType;


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
} KNXnetIP_CommandType



typedef enum {
    KNX_CEMI_MSG_DATA_REQ = 0x11,
    KNX_CEMI_MSG_DATA_IND = 0x12,
    KNX_CEMI_MSG_DATA_CON = 0x13,
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
