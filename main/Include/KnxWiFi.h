/**
 * \file KnxWiFi.h
 * 
 * \brief Knx WiFi
 * 
 * This file contains the implementation of Knx WiFi
 * 
 * \version 1.0.0
 * 
 * \author Ibrahim Ozturk
 * 
 * Copyright 2023 Ibrahim Ozturk
 * All rights exclusively reserved for Ibrahim Ozturk,
 * unless expressly agreed to otherwise.
*/

#ifndef KNXWIFI_H
#define KNXWIFI_H

/*==================[inclusions]============================================*/

/*==================[macros]================================================*/

#define ESP_WIFI_SSID      "KNXnetIP Interface"
#define ESP_WIFI_PASS      "040ADA47778E24C"
#define ESP_WIFI_CHANNEL   11U
#define MAX_STA_CONN       1U

/*==================[type definitions]======================================*/


/*==================[external function declarations]========================*/
extern void wifi_init_softap(void);

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/

#endif /* ifndef KNXWIFI_H */
