#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "Pdu.h"
#include "KnxWiFi.h"
#include "KNXnetIP.h"
#include "IP_DataLinkLayer.h"

static const char *TAG = "KNXnetIP_UdpServer";
static const char *V4TAG = "mcast-ipv4";

int KNXnetIP_MulticastSocket = -1;

/* Add a socket to the IPV4 multicast group */
static int socket_add_ipv4_multicast_group(int sock, bool assign_source_if)
{
    struct ip_mreq imreq = { 0 };
    struct in_addr iaddr = { 0 };
    int err = 0;

    /* Configure source interface */
    esp_netif_ip_info_t ip_info = { 0 };
    err = esp_netif_get_ip_info(wifi_get_netif(), &ip_info);
    if (err != ESP_OK) {
        ESP_LOGE(V4TAG, "Failed to get IP address info. Error 0x%x", err);
        goto err;
    }
    inet_addr_from_ip4addr(&iaddr, &ip_info.ip);

    /* Configure multicast address to listen to */
    err = inet_aton(MULTICAST_IPV4_ADDR, &imreq.imr_multiaddr.s_addr);
    if (err != 1) {
        ESP_LOGE(V4TAG, "Configured IPV4 multicast address '%s' is invalid.", MULTICAST_IPV4_ADDR);
        /* Errors in the return value have to be negative */ 
        err = -1;
        goto err;
    }
    ESP_LOGI(TAG, "Configured IPV4 Multicast address %s", inet_ntoa(imreq.imr_multiaddr.s_addr));
    if (!IP_MULTICAST(ntohl(imreq.imr_multiaddr.s_addr))) {
        ESP_LOGW(V4TAG, "Configured IPV4 multicast address '%s' is not a valid multicast address. This will probably not work.", MULTICAST_IPV4_ADDR);
    }

    if (assign_source_if) {
        /* Assign the IPv4 multicast source interface, via its IP */
        /* (only necessary if this socket is IPV4 only) */
        err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iaddr,
                         sizeof(struct in_addr));
        if (err < 0) {
            ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_IF. Error %d", errno);
            goto err;
        }
    }

    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         &imreq, sizeof(struct ip_mreq));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to set IP_ADD_MEMBERSHIP. Error %d", errno);
        goto err;
    }

 err:
    return err;
}

static int create_multicast_ipv4_socket(void)
{
    struct sockaddr_in saddr = { 0 };
    int sock = -1;
    int err = 0;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0)
    {
        ESP_LOGE(V4TAG, "Failed to create socket. Error %d", errno);
        return -1;
    }

    /* Bind the socket to any address */
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(UDP_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to bind socket. Error %d", errno);
        goto err;
    }

    /* Assign multicast TTL (set separately from normal interface TTL) */
    uint8_t ttl = MULTICAST_TTL;
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(uint8_t));
    if (err < 0) {
        ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_TTL. Error %d", errno);
        goto err;
    }

    /* Add socket to the multicast group for listening */
    err = socket_add_ipv4_multicast_group(sock, true);
    if (err < 0) {
        goto err;
    }

    /* All set, socket is configured for sending and receiving */
    return sock;

err:
    close(sock);
    return -1;
}

int create_unicast_ipv4_socket(uint32_t ipAddr, uint16_t port)
{
    struct sockaddr_in saddr = { 0 };
    int sock = -1;
    int err = 0;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (sock < 0)
    {
        ESP_LOGE(V4TAG, "Failed to create unicast udp socket. Error %d", errno);
    }
    else
    {
        ESP_LOGI(TAG, "Unicast udp socket created");

        /* Bind the socket to ip address */
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr.s_addr = ipAddr;
        err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
        if (err < 0) {
            ESP_LOGE(V4TAG, "Failed to bind udp socket. Error %d", errno);
        }
        else
        {
            ESP_LOGI(V4TAG, "Unicast udp socket bound, addr %lX", saddr.sin_addr.s_addr);
            ESP_LOGI(V4TAG, "Unicast udp socket bound, port %d", saddr.sin_port);

            /* Add socket to the multicast group for listening */
            err = socket_add_ipv4_multicast_group(sock, true);
            if (err < 0) {
                close(sock);
                return -1;
            }
        }
    }

    return sock;
}

void KNXnetIP_UDPSend(uint32_t ipAddr, uint16_t port, uint8_t * txBuffer, uint16_t txLength)
{
    if (KNXnetIP_MulticastSocket < 0)
    {
        ESP_LOGE(TAG, "Failed to get IPv4 socket");
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    else
    {
        struct sockaddr_in sdestv4 = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr.s_addr = htonl(ipAddr)
        };

        int err = 0;

        do
        {
            vTaskDelay(5 / portTICK_PERIOD_MS);
            err = sendto(KNXnetIP_MulticastSocket, txBuffer, txLength, 0, (struct sockaddr *)&sdestv4, sizeof(struct sockaddr_in));

            if (err < 0)
            {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            }
            else
            {
//                ESP_LOGI(TAG, "Message sent");
            }
        } while (-1 == err);
    }
}

void udp_mcast_task(void *pvParameters)
{
    while (1)
    {
        KNXnetIP_MulticastSocket = create_multicast_ipv4_socket();

        if (KNXnetIP_MulticastSocket < 0)
        {
            ESP_LOGE(TAG, "Failed to create IPv4 udp multicast socket");
            vTaskDelay(5 / portTICK_PERIOD_MS);
            continue;
        }

        /* Set destination multicast addresses for sending from these sockets */
        struct sockaddr_in sdestv4 = {
            .sin_family = PF_INET,
            .sin_port = htons(UDP_PORT),
        };

        inet_aton(MULTICAST_IPV4_ADDR, &sdestv4.sin_addr.s_addr);

        /* Loop waiting for UDP received */
        int err = 1;
        while (err > 0)
        {
            struct timeval tv = {
                .tv_sec = 2,
                .tv_usec = 0,
            };
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(KNXnetIP_MulticastSocket, &rfds);

            int s = select(KNXnetIP_MulticastSocket + 1, &rfds, NULL, NULL, &tv);
            if (s < 0)
            {
                ESP_LOGE(TAG, "Select failed: errno %d", errno);
                err = -1;
                break;
            }
            else if (s > 0)
            {
                if (FD_ISSET(KNXnetIP_MulticastSocket, &rfds))
                {
                    /* Incoming datagram received */
                    char recvbuf[512];
                    char raddr_name[32] = { 0 };
                    uint32_t ipAddr;
                    uint16_t port;

                    struct sockaddr_storage raddr;
                    socklen_t socklen = sizeof(raddr);
                    int len = recvfrom(KNXnetIP_MulticastSocket, recvbuf, sizeof(recvbuf)-1, 0,
                                       (struct sockaddr *)&raddr, &socklen);
                    if (len < 0)
                    {
                        ESP_LOGE(TAG, "multicast recvfrom failed: errno %d", errno);
                        err = -1;
                        break;
                    }

                    /* Get the sender's address as a string */
                    if (raddr.ss_family == PF_INET)
                    {
                        inet_ntoa_r(((struct sockaddr_in *)&raddr)->sin_addr,
                                    raddr_name, sizeof(raddr_name)-1);
                    }

                    ipAddr = htonl(((struct sockaddr_in *)&raddr)->sin_addr.s_addr);
                    port = htons(((struct sockaddr_in *)&raddr)->sin_port);

//                    ESP_LOGI(TAG, "received %d bytes from %s:%d", len, raddr_name, port);

                    PduInfoType lpdu;
                    lpdu.SduDataPtr = (uint8_t *)&recvbuf;
                    lpdu.SduLength = (uint8_t)len;

                    /* Call L_Data_Ind to inform IP DataLinkLayer */
                    IP_L_Data_Ind(&lpdu, ipAddr, port);
                }
            }
            else
            { 
                /* Socket idle. */
            }
        }

        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(KNXnetIP_MulticastSocket, 0);
        close(KNXnetIP_MulticastSocket);
    }
}
