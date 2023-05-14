#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/timers.h"
#include "esp_mac.h"
#include "sdkconfig.h"
#include "Port_Cfg.h"
#include "KnxEthernet.h"

#define ETH_SPI_HOST                (1U)
#define ETH_SPI_PHY_ADDR            (1U)
#define ETH_SPI_CLOCK_MHZ           (40U)
#define ETH_CONNECTION_TIMEOUT      (10U)

#define SPI_ETHERNETS_NUM           (1U)
#define INTERNAL_ETHERNETS_NUM      (0U)

static const char* TAG = "lanw5500";
static void (*_eth_got_ip_cb)(void);
static void (*_eth_disconnected_cb)(void);

static TimerHandle_t timer = NULL;
static esp_netif_t *eth_netif_spi = NULL;
static esp_eth_handle_t eth_handle_spi = NULL;
static spi_device_handle_t spi_handle = NULL;

typedef struct {

    uint8_t spi_cs_gpio;
    uint8_t int_gpio;
    int8_t phy_reset_gpio;
    uint8_t phy_addr;

} spi_eth_module_config_t;

connection_info_t conn_info;

static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    if (timer) xTimerStop(timer, 0);
    switch (event_id)
    {
        case ETHERNET_EVENT_CONNECTED:
            esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
            sprintf(conn_info.mac, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            ESP_LOGI(TAG, "Ethernet Link Up");
            ESP_LOGI(TAG, "Ethernet HW Addr: %s", conn_info.mac);
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Ethernet Link Down");
            if (_eth_disconnected_cb) _eth_disconnected_cb();
            break;
        case ETHERNET_EVENT_START:
            ESP_LOGI(TAG, "Ethernet Started");
            if (timer) xTimerStart(timer, 0);
            break;
        case ETHERNET_EVENT_STOP:
            ESP_LOGI(TAG, "Ethernet Stopped");
            break;
        default:
            break;
    }
}

void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    sprintf(conn_info.ip, IPSTR, IP2STR(&ip_info->ip));
    sprintf(conn_info.netmask, IPSTR, IP2STR(&ip_info->netmask));
    sprintf(conn_info.gw, IPSTR, IP2STR(&ip_info->gw));

    ESP_LOGI(TAG, "Ethernet Got IP Address:");
    ESP_LOGI(TAG, " - IP:   %s", conn_info.ip);
    ESP_LOGI(TAG, " - MASK: %s", conn_info.netmask);
    ESP_LOGI(TAG, " - GW:   %s\n", conn_info.gw);
    
    // execute callback
    if (_eth_got_ip_cb) _eth_got_ip_cb();
}

void lanw5500_deinit()
{
    if (timer) xTimerDelete(timer, 0);
    esp_eth_stop(eth_handle_spi);
    esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler);
    //gpio_uninstall_isr_service(); do not uninstall as isr service is needed by other gpios
    //TODO!
    //esp_eth_driver_uninstall(eth_handle_spi);
    spi_bus_remove_device(spi_handle);
    spi_bus_free(ETH_SPI_HOST);
    esp_netif_destroy(eth_netif_spi);
    esp_event_loop_delete_default();
    esp_netif_deinit();
    timer = NULL;
    eth_netif_spi = NULL;
    eth_handle_spi = NULL;
    spi_handle = NULL;
}

esp_err_t lanw5500_init(void (*eth_got_ip_cb)(void), void (*eth_disconnected_cb)(void), void (*eth_timeout_cb)())
{
    esp_err_t ret = ESP_OK;

    // Assign callback
    _eth_got_ip_cb = eth_got_ip_cb;
    _eth_disconnected_cb = eth_disconnected_cb;

    // Initialize TCP/IP network interface (should be called only once in application)
    ret |= esp_netif_init();
    if (ret != ESP_OK) return ESP_FAIL;

    // Create default event loop that running in background
    ret |= esp_event_loop_create_default();
    if (ret != ESP_OK)
    {
        esp_netif_deinit();
        return ESP_FAIL;
    }

    // Create instance of esp-netif for SPI Ethernet
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    esp_netif_config_t cfg_spi = {
        .base = &esp_netif_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };
    esp_netif_config.if_key = "ETH_DEF"; //! IMPORTANT: need to be named like this, mdns component uses this key to get netif handle
    esp_netif_config.if_desc = "eth0";
    esp_netif_config.route_prio = 30;
    eth_netif_spi = esp_netif_new(&cfg_spi);
    
    // Init MAC and PHY configs to default
    eth_mac_config_t mac_config_spi = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config_spi = ETH_PHY_DEFAULT_CONFIG();

    // Install GPIO ISR handler to be able to service SPI Eth modlues interrupts
    gpio_install_isr_service(0);

    // Init SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num = ETH_SPI_MISO_GPIO,
        .mosi_io_num = ETH_SPI_MOSI_GPIO,
        .sclk_io_num = ETH_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ret |= spi_bus_initialize(ETH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK)
    {
        esp_netif_destroy(eth_netif_spi);
        esp_event_loop_delete_default();
        esp_netif_deinit();
        return ESP_FAIL;
    }

    // Init specific SPI Ethernet module configuration from Kconfig (CS GPIO, Interrupt GPIO, etc.)
    spi_eth_module_config_t spi_eth_module_config = {
        ETH_SPI_CS_GPIO,
        ETH_SPI_INT_GPIO,
        ETH_SPI_PHY_RST_GPIO,
        ETH_SPI_PHY_ADDR
    };

    // Configure SPI interface and Ethernet driver for specific SPI module
    esp_eth_mac_t *mac_spi;
    esp_eth_phy_t *phy_spi;

    spi_device_interface_config_t devcfg = {
        .command_bits = 16, // Actually it's the address phase in W5500 SPI frame
        .address_bits = 8,  // Actually it's the control phase in W5500 SPI frame
        .mode = 0,
        .clock_speed_hz = ETH_SPI_CLOCK_MHZ * 1000 * 1000,
        .queue_size = 20
    };

    // Set SPI module Chip Select GPIO
    devcfg.spics_io_num = spi_eth_module_config.spi_cs_gpio;

    // w5500 ethernet driver is based on spi driver
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(ETH_SPI_HOST, &devcfg);

    // Set remaining GPIO numbers and configuration used by the SPI module
    w5500_config.int_gpio_num = spi_eth_module_config.int_gpio;
    phy_config_spi.phy_addr = spi_eth_module_config.phy_addr;
    phy_config_spi.reset_gpio_num = spi_eth_module_config.phy_reset_gpio;

    mac_spi = esp_eth_mac_new_w5500(&w5500_config, &mac_config_spi);
    phy_spi = esp_eth_phy_new_w5500(&phy_config_spi);

    esp_eth_config_t eth_config_spi = ETH_DEFAULT_CONFIG(mac_spi, phy_spi);
    ret |= esp_eth_driver_install(&eth_config_spi, &eth_handle_spi);

    if (ret != ESP_OK)
    {
        spi_bus_remove_device(spi_handle);
        spi_bus_free(ETH_SPI_HOST);
        esp_netif_destroy(eth_netif_spi);
        esp_event_loop_delete_default();
        esp_netif_deinit();
        return ESP_FAIL;
    }

    // set mac address
    uint8_t mac_eth[6];
    esp_read_mac(mac_eth, ESP_MAC_WIFI_STA);
    mac_eth[5] += 2;
    ret |= esp_eth_ioctl(eth_handle_spi, ETH_CMD_S_MAC_ADDR, mac_eth);
    if (ret != ESP_OK)
    {
        //esp_eth_driver_uninstall(eth_handle_spi);
        spi_bus_remove_device(spi_handle);
        spi_bus_free(ETH_SPI_HOST);
        esp_netif_destroy(eth_netif_spi);
        esp_event_loop_delete_default();
        esp_netif_deinit();
        return ESP_FAIL;
    }

    // attach Ethernet driver to TCP/IP stack
    ret |= esp_netif_attach(eth_netif_spi, esp_eth_new_netif_glue(eth_handle_spi));
    if (ret != ESP_OK)
    {
        //esp_eth_driver_uninstall(eth_handle_spi);
        spi_bus_remove_device(spi_handle);
        spi_bus_free(ETH_SPI_HOST);
        esp_netif_destroy(eth_netif_spi);
        esp_event_loop_delete_default();
        esp_netif_deinit();
        return ESP_FAIL;        
    }

    // Register user defined event handers
    ret |= esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL);
    ret |= esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL);

    // Register timer
    if (eth_timeout_cb && ETH_CONNECTION_TIMEOUT)
    {
        timer = xTimerCreate("EthConnectTimoutTimer", pdMS_TO_TICKS(ETH_CONNECTION_TIMEOUT*1000), false, NULL, eth_timeout_cb);
        if (!timer) ESP_LOGE(TAG, "Unable to initialize connection timeout timer");
    }

    // start Ethernet driver state machine
    ret |= esp_eth_start(eth_handle_spi);
    if (ret != ESP_OK)
    {
        esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler);
        esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler);
        //esp_eth_driver_uninstall(eth_handle_spi);
        spi_bus_remove_device(spi_handle);
        spi_bus_free(ETH_SPI_HOST);
        esp_netif_destroy(eth_netif_spi);
        esp_event_loop_delete_default();
        esp_netif_deinit();
        return ESP_FAIL;        
    }

    return ret;
}

char *lanw5500_get_status()
{
	const char *conn_info_json_format = "{\"mode\":\"Ethernet\",\"mac\":\"%s\",\"ip\":\"%s\",\"netmask\":\"%s\",\"gw\":\"%s\"}\n";
    char *buf = NULL;
    asprintf(&buf, conn_info_json_format, conn_info.mac, conn_info.ip, conn_info.netmask, conn_info.gw);
    return buf;
}

const connection_info_t *lanw5500_conn_info()
{
    return (const connection_info_t*) &conn_info;
}

// void KnxEthernet_Init(void)
// {
//     // Initialize Ethernet driver
//     uint8_t eth_port_cnt = 0;
//     esp_eth_handle_t *eth_handles;
//     ESP_ERROR_CHECK(lanw5500_init(&eth_handles, &eth_port_cnt));

//     // Initialize TCP/IP network interface aka the esp-netif (should be called only once in application)
//     ESP_ERROR_CHECK(esp_netif_init());
//     // Create default event loop that running in background
//     ESP_ERROR_CHECK(esp_event_loop_create_default());

//     // Create instance(s) of esp-netif for Ethernet(s)
//     if (eth_port_cnt == 1) {
//         // Use ESP_NETIF_DEFAULT_ETH when just one Ethernet interface is used and you don't need to modify
//         // default esp-netif configuration parameters.
//         esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
//         esp_netif_t *eth_netif = esp_netif_new(&cfg);
//         // Attach Ethernet driver to TCP/IP stack
//         ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handles[0])));
//     } else {
//         // Use ESP_NETIF_INHERENT_DEFAULT_ETH when multiple Ethernet interfaces are used and so you need to modify
//         // esp-netif configuration parameters for each interface (name, priority, etc.).
//         esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
//         esp_netif_config_t cfg_spi = {
//             .base = &esp_netif_config,
//             .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
//         };
//         char if_key_str[10];
//         char if_desc_str[10];
//         char num_str[3];
//         for (int i = 0; i < eth_port_cnt; i++) {
//             itoa(i, num_str, 10);
//             strcat(strcpy(if_key_str, "ETH_"), num_str);
//             strcat(strcpy(if_desc_str, "eth"), num_str);
//             esp_netif_config.if_key = if_key_str;
//             esp_netif_config.if_desc = if_desc_str;
//             esp_netif_config.route_prio -= i*5;
//             esp_netif_t *eth_netif = esp_netif_new(&cfg_spi);

//             // Attach Ethernet driver to TCP/IP stack
//             ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handles[i])));
//         }
//     }

//     // Register user defined event handers
//     ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

//     // Start Ethernet driver state machine
//     for (int i = 0; i < eth_port_cnt; i++) {
//         ESP_ERROR_CHECK(esp_eth_start(eth_handles[i]));
//     }
// }