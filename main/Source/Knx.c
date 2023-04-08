#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "KNXnetIP_UdpServer.h"
#include "KnxWiFi.h"
#include "KnxTpUart2.h"
#include "KnxTpUart2_Services.h"
#include "KnxTelegram.h"
#include "KnxComObject.h"
#include "Knx_Dpt.h"
#include "Knx_Types.h"
#include "KnxDevice.h"
#include "Pdu.h"
#include "Knx_ApplicationLayer.h"

#include "TpUart2_DataLinkLayer.h"

static const char *TAG = "Knx WiFi softAP";

#define GROUP_ADDRESS_3L(main, mid, sub) ((uint16_t)(((main & 0x1FU) << 11U) + ((mid & 0x07U) << 8U) + sub))
#define GROUP_ADDRESS_2L(main, sub) ((uint16_t)((main & 0x1FU) << 11) + sub)
#define INDV_ADDRESS_3L(main, mid, sub) GROUP_ADDRESS_3L(main, mid, sub)
#define INDV_ADDRESS_2L(main, sub) GROUP_ADDRESS_2L(main, sub)
#define PHYSICAL_ADDRESS(area, line, busdevice) ((uint16_t)(((area & 0x0FU) << 12) + ((line & 0x0FU) << 8) + busdevice))

Knx_TxActionType KnxDevice_CmdBuffer[64] = { 0 };

const uart_config_t TPUART2_CFG = 
{
    .baud_rate = 19200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

const KnxTpUart2_UartCfgType KnxTpUart2_UartCfg = 
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

KnxTpUart2_ObjectType KnxTpUart2_Object;

KnxTpUart2_CfgType KnxTpUart2_Cfg = 
{
    &KnxTpUart2_UartCfg,
    &KnxTpUart2_Object
};

const KnxComObject_Type KnxObjectList[3] = 
{
   {.addr = GROUP_ADDRESS_3L(0,0,1), .dptId = KNX_DPT_1_001, .indicators = (KNXCOMOBJ_C_FLAG | KNXCOMOBJ_R_FLAG | KNXCOMOBJ_T_FLAG), .length = 8, .valid = false},
   {.addr = GROUP_ADDRESS_3L(0,0,2), .dptId = KNX_DPT_7_002, .indicators = (KNXCOMOBJ_C_FLAG | KNXCOMOBJ_R_FLAG | KNXCOMOBJ_T_FLAG), .length = 8, .valid = false},
   {.addr = GROUP_ADDRESS_3L(0,0,3), .dptId = KNX_DPT_3_008, .indicators = (KNXCOMOBJ_C_FLAG | KNXCOMOBJ_R_FLAG | KNXCOMOBJ_T_FLAG), .length = 8, .valid = false}
};

KnxTelegram_Type TxTelegram =
{
    .l_data.controlField = CONTROL_FIELD_DEFAULT_VALUE,
    .l_data.sourceAddr = 0U,
    .l_data.destAddr = 0U,
    .l_data.length = LENGTH_FIELD_DEFAULT_VALUE,
    .l_data.cmd = 0xFFFF,
};

KnxTelegram_Type RxTelegram = 
{
    .l_data.controlField = CONTROL_FIELD_DEFAULT_VALUE,
    .l_data.sourceAddr = 0U,
    .l_data.destAddr = 0U,
    .l_data.length = LENGTH_FIELD_DEFAULT_VALUE,
    .l_data.cmd = 0xFFFF,
};

/* Create a Knx Device */
KnxDeviceType KnxDevice = 
{
    .comObjectList = &KnxObjectList[0],
    .comObjectsNb = (sizeof(KnxObjectList)/sizeof(KnxComObject_Type)),
    .state = INIT,
    .cfg = &KnxTpUart2_Cfg,
    .txCmdList = &KnxDevice_CmdBuffer[0],
    .txCmdLastIndex = 0,
    .initCompleted = FALSE,
    .initIndex = 0U,
    .lastInitTimeMs = 0U,
    .lastRxTimeUs = 0U,
    .lastTxTimeUs = 0U,
}; 

int KnxDevice_Init(void)
{
    KnxTpUart2_Object = KnxTpUart2_Create(&KnxTpUart2_Cfg);

    return 0;
}

const uint16_t GroupAddressTable[] =
{
    GROUP_ADDRESS_3L(5U,0U,0U),
    GROUP_ADDRESS_3L(0u,2U,0U),
};

static void KnxDevice_Main()
{
    ESP_LOGI("KnxIpInterface", "ESP32 Knx Stack v0.1");

    //KnxDevice_Task(&KnxDevice);
}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(1025);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, 1024, 20 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }
    }
    free(data);
}

void app_main(void)
{


    KnxDevice_Init();
    KnxDevice_Main();

//    A_GroupValue_Read_Req(AckDontCare, 0, LowPriority, UnlimitedRouting, None);

//    PduInfoType lpdu;
//    uint8_t pduData[] = { 0xBCU, 0x11U, 0xFAU, 0x05U, 0x00U, 0xE1U, 0x00U, 0x00U, 0x4CU};
//    lpdu.SduDataPtr = &pduData[0];
//    lpdu.SduLength = 9;

//    TpUart2_L_Data_Req((bool)1, 0x11FAU, GroupAddress, LowPriority, &lpdu);

    /* Construct A_GroupValue_Read-Response PDU */
//    PduInfoType lpdu;
//    const char sdu[] = {0xBCU, 0x11U, 0xFAU, 0x05U, 0x00U, 0xE1U, 0x00U, 0x00U, 0x4CU};

//    lpdu.SduDataPtr = (uint8_t *)&sdu[0];
//    lpdu.SduLength = 9;
 
    /* Group Address 0.0.001 */
//    TpUart2_L_Data_Ind(0U, 0x11FA1U, 0x0500, GroupAddress, LowPriority, &lpdu);
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);

    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES - 1, NULL);
}
