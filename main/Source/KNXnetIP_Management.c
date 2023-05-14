/**
 * \file KNXnetIP_Management.c
 * 
 * \brief KNXnet/IP Management Services
 * 
 * This file contains the implementation of KNXnet/IP Management Services
 * 
 * \version 1.0.0
 * 
 * \author Ibrahim Ozturk
 * 
 * Copyright 2023 Ibrahim Ozturk
 * All rights exclusively reserved for Ibrahim Ozturk,
 * unless expressly agreed to otherwise.
*/

/*==================[inclusions]============================================*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

#include "Knx_Types.h"


/*==================[macros]================================================*/

/*==================[type definitions]======================================*/
typedef struct {
    uint8_t a;
} KNXnetIP_ServiceContainerType;
/*==================[external function declarations]========================*/

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/
KNXnetIP_ServiceContainerType KNXnetIP_ServiceContainer[1];

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/
void KNXnetIP_DeviceConfigurationResponse(KNXnetIP_FrameType cemiFrame, uint8_t * txBuffer, uint16_t * txLength)
{
    uint8_t txBytes = 0;

    /* Structure Length */
    txBuffer[txBytes++] = 0x02U;

    /* Communication Channel ID */
    txBuffer[txBytes++] = KNXnetIP_Channel[0].ChannelId;

    /* Sequence Counter */
    txBuffer[txBytes++] = 0x00U;

    /* reserved */
    txBuffer[txBytes++] = 0x00U;

    /* cEMI frame - Message Code */
    txBuffer[txBytes++] = 0x00U;

    /* cEMI frame - Service Information */
    txBuffer[txBytes++] = 0x00U;

    /*
        DEVICE_CONFIGURATION_REQUEST shall carry a cEMI frame with the configuration message.
        The configuration message shall contain a local device management service

        KNXnet/IP Server KNXnet/IP Client
        - M_PropRead.con
        - M_PropWrite.con
        - M_PropInfo.ind
        - M_FuncPropStateResponse.con
        - cEMI T_Data_Individual.ind
        - cEMI T_Data_Connected.ind
    */

    /* Update Tx Length */
    *txLength = txBytes;
}

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/
