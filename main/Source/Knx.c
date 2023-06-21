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
#include "TP_DataLinkLayer.h"

#define TX_TPUART2    (GPIO_NUM_17)
#define RX_TPUART2    (GPIO_NUM_18)

#define RESET_TPUART (GPIO_NUM_42)
#define SAVE_TPUART  (GPIO_NUM_41)

#define WS2812_LED   (GPIO_NUM_3)

#define KNXNETIP_USE_WIFI_INTERFACE

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
    .parity = UART_PARITY_EVEN,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

const Knx_UartCfgType KNXTPUART_CFG = 
{
    UART_NUM_1, /* Uart Port Number */
    &TPUART2_CFG, /* Uart Configuration */
    0x11FAU, /* Physical Address */
    TX_TPUART2, /* Txd Pin */ /* DevKit GPIO_NUM_4 */
    RX_TPUART2, /* Rxd Pin */ /* DevKit GPIO_NUM_5 */
    100, /* Rx Timeout */
    200, /* Tx Timeout */
    3072, /* Rx Buffer Size */
};

static void tpuart_rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "TPUART_RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(3073);
    uint8_t layer2Service;
    uint8_t dataStartOffset;
    PduInfoType lpdu;

    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, 3072, 20/portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            
#ifdef KNXNETIP_DEBUG_LOGGING
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes", rxBytes);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
#endif /* KNXNETIP_DEBUG_LOGGING */

            lpdu.SduDataPtr = &data[0];
            dataStartOffset = 0U;

            layer2Service = data[0] & TPUART2_LAYER2_SERVICE_MASK;

#ifdef TPUART2_STATEINDICATION_ENABLED
            if ( TPUART2_STATEINDICATION == (data[0] & TPUART2_STATE_INDICATION_MASK))
            {
                if ( 1 < rxBytes)
                {
                    /* A received L-Data Frame followed by the State indication as noted in */
                    /* Siemens KNX EIB TP-UART 2-IC Technical Manual, section 3.2.3.1.12.3, */
                    /* title TP-UART-State.indication/response Service.                     */
                    /* That means we have the data at offset 1, so set the offset to 1.     */
                    dataStartOffset = 1U;

                    ESP_LOGW("TP-UART2+", "Received prior Status Code, applying offset to data.");

                    /* Update Layer-2 Service */
                    layer2Service = data[dataStartOffset] & TPUART2_LAYER2_SERVICE_MASK;
                }

                if (TPUART2_STATE_SLAVE_COLLISION == (TPUART2_STATE_SLAVE_COLLISION & data[0]))
                {
                    ESP_LOGW("TP-UART2+", "SC:Slave Collision");
                }
                if (TPUART2_STATE_RECEIVE_ERROR == (TPUART2_STATE_RECEIVE_ERROR & data[0]))
                {
                    ESP_LOGW("TP-UART2+", "RE:Receive Error");
                }
                if (TPUART2_STATE_TRANSMIT_ERROR == (TPUART2_STATE_TRANSMIT_ERROR & data[0]))
                {
                    ESP_LOGW("TP-UART2+", "TE:Transmit Error");
                }
                if (TPUART2_STATE_PROTOCOL_ERROR == (TPUART2_STATE_PROTOCOL_ERROR & data[0]))
                {
                    ESP_LOGW("TP-UART2+", "PE:Protocol Error");
                }
                if (TPUART2_STATE_TEMP_WARNING == (TPUART2_STATE_TEMP_WARNING & data[0]))
                {
                    ESP_LOGW("TP-UART2+", "TW: Temperature Warning");
                }
            }
#endif /* TPUART2_STATEINDICATION_ENABLED */

            if (((TPUART2_LAYER2_L_DATA_REQ == layer2Service) ||
                 (TPUART2_LAYER2_L_EXT_DATA_REQ == layer2Service)))
            {
                if(true == TpUart2_DetectEOP(&data[dataStartOffset], (rxBytes - dataStartOffset), &lpdu.SduLength))
                {
                    lpdu.SduDataPtr = &data[dataStartOffset];

                    /* Call L_Data_Ind to inform TP DataLinkLayer */
                    TpUart2_L_Data_Ind(&lpdu);

                    vTaskDelay(5/portTICK_PERIOD_MS);

                    /* Check for data confirmation byte */
                    /* The L_DATA.confirm service is transmitted to the host if an */
                    /* acknowledge was received or if the last repetition is       */
                    /* transmitted and no acknowledge was received.                */
                    /* 0x8B = The transmission of the L_DATA-frame was successful  */
                    /* 0x0B = The transmission of the L_DATA-frame was not successful */
                    if ((lpdu.SduLength < rxBytes) && (0x8BU == data[lpdu.SduLength + 1]))
                    {
                        /* Send ACK */
                        TP_GW_L_Data_Ind_ACK(&lpdu);
                    }
                }
                else
                {
                    /* Invalid data packet from TpUart2+ */
                    ESP_LOGW(RX_TASK_TAG, "Invalid data packet received from TpUart2+");
                }
            }
        }
    }
    free(data);
}

static void got_network_connection()
{
    ESP_LOGI(TAG, "successfully established network connection!");
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

#ifdef KNXNETIP_USE_WIFI_INTERFACE
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
#endif /* KNXNETIP_USE_WIFI_INTERFACE */

    KNXnetIP_TunnellingInit();

#ifdef KNXNETIP_USE_ETH_INTERFACE
    /* Initialize Ethernet */
    lanw5500_init(got_network_connection, NULL, NULL);
#endif /* KNXNETIP_USE_ETH_INTERFACE */

    xTaskCreate(&udp_mcast_task, "udp_mcast", 4096, NULL, 5, NULL);
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET, 5, NULL);
    xTaskCreate(tpuart_rx_task, "tpuart_rx_task", 3072, NULL, configMAX_PRIORITIES - 1, NULL);
}
