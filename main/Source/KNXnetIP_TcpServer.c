/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "Pdu.h"
#include "KnxWiFi.h"
#include "KNXnetIP.h"
#include "IP_DataLinkLayer.h"

#define PORT                        (3671U)
#define KEEPALIVE_IDLE              (30U)
#define KEEPALIVE_INTERVAL          (30U)
#define KEEPALIVE_COUNT             (3U)

static const char *TAG = "KNXnetIP_TcpServer";

bool KNXnetIP_TcpTxTunnelReqPending = false;

uint32_t KNXnetIP_TcpIpAddr = 0;
int KNXnetIP_TcpSock = -1;

uint8_t * Tcp_TxBufferPtr;
uint16_t Tcp_TxLength = 0;

void KNXnetIP_TcpUpdateTxBuffer(uint8_t * txBuffer, uint16_t txLength)
{
    Tcp_TxBufferPtr = txBuffer;
    Tcp_TxLength = txLength;
}

static void tcp_transmit(const int sock, uint32_t ipAddr, uint16_t port)
{
    int len;
    char rx_buffer[256];

    do {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0)
        {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        }
        else if (len == 0)
        {
            ESP_LOGW(TAG, "Connection closed");
        }
        else
        {
            rx_buffer[len] = 0; /* Null-terminate whatever is received and treat it like a string */ 
//            ESP_LOGI(TAG, "Received %d bytes", len);

            PduInfoType lpdu;
            lpdu.SduDataPtr = (uint8_t *)&rx_buffer;
            lpdu.SduLength = (uint8_t)len;

            /* Call L_Data_Ind to inform IP DataLinkLayer */
            IP_L_Data_Ind(&lpdu, ipAddr, port, IPV4_TCP);

            /* IP DataLinkLayer updates Tcp_TxBufferPtr via call to KNXnetIP_TcpUpdateTxBuffer */

            if (0 == Tcp_TxLength)
            {
                ESP_LOGE(TAG, "Error occurred during getting data from link layer.");
            }
            else
            {
                // send() can return less bytes than supplied length.
                // Walk-around for robust implementation.
                int to_write = Tcp_TxLength;
                while (to_write > 0) {
                    int written = send(sock, Tcp_TxBufferPtr + (Tcp_TxLength - to_write), to_write, 0);
                    if (written < 0) {
                        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                        // Failed to retransmit, giving up
                        return;
                    }
                    to_write -= written;
                }
            }
        }
    } while (len > 0);
}

void tcp_transmitPendingTunnelReq(const int sock, uint32_t ipAddr, uint16_t port)
{
    /* IP DataLinkLayer updates Tcp_TxBufferPtr via call to KNXnetIP_TunnellingRequest */
    if (NULL == Tcp_TxBufferPtr)
    {
        ESP_LOGE(TAG, "Error occurred during getting data from link layer. NULL_PTR");
    }
    else if (0 == Tcp_TxLength)
    {
        ESP_LOGE(TAG, "Error occurred during getting data from link layer. ZERO_LENGTH");
    }
    else
    {
        // send() can return less bytes than supplied length.
        // Walk-around for robust implementation.
        int to_write = Tcp_TxLength;
        while (to_write > 0) {
            int written = send(sock, Tcp_TxBufferPtr + (Tcp_TxLength - to_write), to_write, 0);
            if (written < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                // Failed to retransmit, giving up
                return;
            }
            to_write -= written;
        }
    }
}

void tcp_server_task(void *pvParameters)
{
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;

    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;

    uint32_t ipAddr;
    uint16_t port;
    struct sockaddr_storage dest_addr;

    if (addr_family == AF_INET) {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;
    }

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1) {
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }

        ipAddr = htonl(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr);
        port = htons(((struct sockaddr_in *)&source_addr)->sin_port);

        KNXnetIP_TcpIpAddr = ipAddr;
        KNXnetIP_TcpSock = sock;

        tcp_transmit(sock, ipAddr, port);

        shutdown(sock, 0);
        close(sock);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}
