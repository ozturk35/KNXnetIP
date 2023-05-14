set(srcs "./Source/KnxEthernet.c"
         "./Source/KnxWiFi.c"
         "./Source/KNXnetIP_UdpServer.c"
         "./Source/KNXnetIP_TcpServer.c"
         "./Source/KNXnetIP_Core.c"
         "./Source/KNXnetIP_Tunnelling.c"
         "./Source/TpUart2_DataLinkLayer.c"
         "./Source/KnxTpUart2_Services.c"
         "./Source/TP_DataLinkLayer.c"
         "./Source/IP_DataLinkLayer.c"
         "./Source/Knx.c"
         )

idf_component_register(SRCS "${srcs}"
INCLUDE_DIRS  "./Include")
