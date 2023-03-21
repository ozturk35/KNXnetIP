set(srcs "./Source/KnxTelegram.c"
         "./Source/KnxTpUart2.c"
         "./Source/KnxTpUart2_Services.c")

idf_component_register(SRCS "${srcs}"
INCLUDE_DIRS  "./Include")
