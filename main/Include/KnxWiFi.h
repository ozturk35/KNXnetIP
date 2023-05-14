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

#define ESP_WIFI_SSID      "SUPERONLINE_WiFi_6637"
#define ESP_WIFI_PASS      "9MMUPEW3FJCL"
#define ESP_MAXIMUM_RETRY  3

#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define EXAMPLE_H2E_IDENTIFIER ""

#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK

#define ESP_WIFI_CHANNEL   149U
#define MAX_STA_CONN       1U

/*==================[type definitions]======================================*/


/*==================[external function declarations]========================*/
void wifi_init_sta(void);
esp_netif_t * wifi_get_netif(void);

/*==================[internal function declarations]========================*/

/*==================[external constants]====================================*/

/*------------------[version constants definition]--------------------------*/

/*==================[internal constants]====================================*/

/*==================[external data]=========================================*/
extern uint32_t KnxIPInterface_IpAddr;

/*==================[internal data]=========================================*/

/*==================[external function definitions]=========================*/

/*==================[internal function definitions]=========================*/

/*==================[end of file]===========================================*/

#endif /* ifndef KNXWIFI_H */
