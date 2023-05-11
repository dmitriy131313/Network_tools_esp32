idf_component_register(
    SRCS "WebSocketsServer.cpp"
         "WebSocketsClient.cpp"
         "WebSockets.cpp"
         "SocketIOclient.cpp"
    INCLUDE_DIRS "." "./libsha1" "./libb64"
    REQUIRES ""
    PRIV_REQUIRES Utilites
                  AsyncTCP_SSL
                  FreeRTOS_usr
)

target_compile_options(${COMPONENT_TARGET} PRIVATE -fno-rtti)