set(srcs "./Source/KNXnetIP_UdpServer.c"
         "./Source/KnxWiFi.c"
         "./Source/Knx_ApplicationLayer.c"
         "./Source/Knx_TransportLayer.c"
         "./Source/Knx_NetworkLayer.c"
         "./Source/Knx_DataLinkLayer.c"
         "./Source/TpUart2_DataLinkLayer.c"
         "./Source/KnxTpUart2.c"
         "./Source/KnxTpUart2_Services.c"
         "./Source/Knx.c"
         )

idf_component_register(SRCS "${srcs}"
INCLUDE_DIRS  "./Include")
