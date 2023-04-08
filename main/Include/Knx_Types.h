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
#include "esp_system.h"

/*==================[macros]================================================*/
/* INDICATOR field : B7  B6  B5  B4  B3  B2  B1  B0 */
/*                   xx  xx   C   R   W   T   U   I */
#define KNX_OBJ_C_INDICATOR	(0x20U) /* Communication (C) */
#define KNX_OBJ_R_INDICATOR	(0x10U) /* Read (R) */
#define KNX_OBJ_W_INDICATOR	(0x08U) /* Write (W) */
#define KNX_OBJ_T_INDICATOR	(0x04U) /* Transmit (T) */
#define KNX_OBJ_U_INDICATOR	(0x02U) /* Update (U) */
#define KNX_OBJ_I_INDICATOR	(0x01U) /* Init Read (I) */

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
