#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "KnxEthernet.h"
#include "KnxWiFi.h"
#include "KNXnetIP.h"
#include "KnxTpUart2_Services.h"
#include "Pdu.h"

#include "TpUart2_DataLinkLayer.h"

static const char *TAG = "Knx Eth sta";

typedef struct {
    uart_port_t uartPort;
    const uart_config_t * uartConfig;
    uint16_t physicalAddr;
    uint8_t txdPin;
    uint8_t rxdPin;
    uint16_t rxTimeout;
    uint16_t txTimeout;
    uint16_t rxBufferSize;
} Knx_UartCfgType;

const uart_config_t TPUART2_CFG = 
{
    .baud_rate = 19200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

const Knx_UartCfgType KNXTPUART_CFG = 
{
    UART_NUM_1, /* Uart Port Number */
    &TPUART2_CFG, /* Uart Configuration */
    0x11FAU, /* Physical Address */
    GPIO_NUM_18, /* Txd Pin */ /* DevKit GPIO_NUM_4 */ /* Actual GPIO_NUM_18 */
    GPIO_NUM_17, /* Rxd Pin */ /* DevKit GPIO_NUM_5 */ /* Actual GPIO_NUM_17 */
    200, /* Rx Timeout */
    200, /* Tx Timeout */
    1024, /* Rx Buffer Size */
};

static void tpuart_rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "TPUART_RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(1025);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, 1024, 20 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);

            /* Call L_Data_Ind to inform TP DataLinkLayer */
            PduInfoType lpdu;
            lpdu.SduDataPtr = data;
            lpdu.SduLength = (uint8_t)rxBytes;

            TpUart2_L_Data_Ind(&lpdu);
        }
    }
    free(data);
}

void app_main(void)
{
    ESP_LOGI("KnxIpInterface", "ESP32 Knx Stack v0.1");

    // Initialize UART
    uart_driver_install(KNXTPUART_CFG.uartPort, KNXTPUART_CFG.rxBufferSize * 2, 0, 0, NULL, 0);
    uart_param_config(KNXTPUART_CFG.uartPort, KNXTPUART_CFG.uartConfig);
    uart_set_pin(KNXTPUART_CFG.uartPort, KNXTPUART_CFG.txdPin, KNXTPUART_CFG.rxdPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

/* BEGIN WIFI CODE */
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
/* END WIFI CODE */

    KNXnetIP_TunnellingInit();

    /* Initialize Ethernet */
//    lanw5500_init(&w5500_got_ip, &w5500_disconnect, &w5500_timeout);

    xTaskCreate(&udp_mcast_task, "udp_mcast", 4096, NULL, 5, NULL);
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET, 5, NULL);

    xTaskCreate(tpuart_rx_task, "tpuart_rx_task", 1024*2, NULL, configMAX_PRIORITIES - 1, NULL);
}
