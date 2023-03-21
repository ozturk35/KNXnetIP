/**
 * \file KNXnetIP_Cfg.h
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
#include "Std_Types.h"

/*==================[macros]================================================*/
// Definition of com obj indicator values
// See "knx.org" for com obj indicators specification
// INDICATOR field : B7  B6  B5  B4  B3  B2  B1  B0
//                   xx  xx   C   R   W   T   U   I  
#define KNX_OBJ_C_INDICATOR	0x20 // Comuunication (C)
#define KNX_OBJ_R_INDICATOR	0x10 // Read (R)
#define KNX_OBJ_W_INDICATOR	0x08 // Write (W)
#define KNX_OBJ_T_INDICATOR	0x04 // Transmit (T)
#define KNX_OBJ_U_INDICATOR	0x02 // Update (U)
#define KNX_OBJ_I_INDICATOR	0x01 // Init Read (I)

/*==================[type definitions]======================================*/
//KNX Network Layer PDU
typedef struct {
    uint8_t control_field_1; 
    uint8_t control_field_2; 
    uint8_t source_address[2]; 
    uint8_t dest_address[2];
    uint8_t data_length; 
    uint8_t tpci; 
    uint8_t apci;
    uint8_t data[];
} Knx_NPduType;

//KNX Transport Layer PDU
typedef struct {
    uint8_t control_field; 
    uint8_t extended_control_field_1; 
    uint8_t extended_control_field_2; 
    uint8_t hop_count; 
    uint8_t ncd[2];
    uint8_t apdu[]; 
}Knx_TPduType;

// KNX Datalink Layer PDU
typedef struct
{
    // header
    uint8_t  ControlField;
    uint16_t SourceAddress;
    uint16_t DestinationAddress;
    uint8_t data_length_field;
    
    // payload
    uint8_t payload[16];
    
    // checksum
    uint8_t checksum;
    
} Knx_LDataStandardFrameType;

typedef struct {
    // header
    uint8_t  ControlField;
    uint16_t SourceAddress;
    uint16_t DestinationAddress;
    uint8_t data_length_field;
    
    // payload
    uint8_t payload[16];
    
    // checksum
    uint8_t checksum;
} Knx_LDataExtendedFrameType;

//KNX Transport Layer PDU 
typedef struct {
    unsigned short header;
    unsigned short length;
    unsigned short apci;
    unsigned char  data[];
} Knx_TPduType;

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