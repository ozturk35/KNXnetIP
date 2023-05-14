#ifndef KNXETHERNET_H
#define KNXETHERNET_H
#pragma once

#include "esp_err.h"

typedef struct {

    char ip[16];
    char netmask[16];
    char gw[16];
    char mac[18];

} connection_info_t;

/**
 * @brief initialize lanw5500 ethernet
 * 
 * @param eth_got_ip_cb function callback which executes 
 * when IP address is obtained
 * @param eth_disconnected_cb function callback which executes 
 * when Ethernet cable is disconnected
 * @param eth_timeout_cb function callback which executes if no ip address could be obtained. Set to NULL to disable the timeout timer.
 * @return esp_err_t ESP_OK on success
 */
esp_err_t lanw5500_init(void (*eth_got_ip_cb)(void), void (*eth_disconnected_cb)(void), void (*eth_timeout_cb)(void));

/**
 * @brief deinitialize lanw5500 ethernet * 
 */
void lanw5500_deinit();

/**
 * @brief get connection information (mac, ip, netmask, gw)
 * 
 * @return char* json string
 */
char *lanw5500_get_status();

/**
 * @brief get connection information (mac, ip, netmask, gw)
 * 
 * @return const pointer to connection_info_t struct
 */
const connection_info_t *lanw5500_conn_info();

#endif /* #ifndef KNXETHERNET_H */ 
