
#ifndef KNXDEVICE_TYPES_H
#define KNXDEVICE_TYPES_H

/*==================[inclusions]============================================*/

#include "Std_Types.h"
#include "KnxTelegram.h"
#include "KnxTpUart2.h"

typedef enum {
  KNX_DEVICE_OK = 0U,
  KNX_DEVICE_NOT_IMPLEMENTED = 254U,
  KNX_DEVICE_ERROR = 255U
} KnxDevice_StatusType;

typedef enum {
  Knx_TP1 = 0x00U,
  Knx_PL110 = 0x01U,
  Knx_RF = 0x02U,
  Knx_IP = 0x05,
} DptMediumType;

typedef enum {
  INIT,
  IDLE,
  TX_ONGOING,
} KnxDevice_StateType;

typedef enum  {
  EIB_READ_REQUEST,
  EIB_WRITE_REQUEST,
  EIB_RESPONSE_REQUEST
} KnxDevice_CommandType;

typedef struct {
  KnxDevice_CommandType command;
  uint8_t index;
  union { // Value
    // Field used in case of short value (value width <= 1 byte)
    struct {
      uint8_t byteValue;
      uint8_t notUsed;
    };
    uint8_t * valuePtr; // Field used in case of long value (width > 1 byte), space is allocated dynamically
  } value;
} Knx_TxActionType;

/* Knx Device Struct */
typedef struct {
    KnxComObject_Type * comObjectList;
    uint8_t comObjectsNb;
    KnxDevice_StateType state;
    KnxTpUart2_CfgType * cfg;
    Knx_TxActionType * txCmdList;
    uint8_t txCmdLastIndex;
    bool initCompleted;
    uint8_t initIndex;
    uint16_t lastInitTimeMs;
    uint16_t lastRxTimeUs;
    uint16_t lastTxTimeUs;
    KnxTelegram_Type txTelegram;
    KnxTelegram_Type rxTelegram;
} KnxDeviceType;

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

#endif /* #ifndef KNXDEVICE_TYPES_H */
