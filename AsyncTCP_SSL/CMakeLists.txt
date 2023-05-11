idf_component_register(
    SRCS "AsyncTCP_RW.cpp" "ESPAsyncTCPbuffer.cpp" "AsyncTCP_SSL.cpp" "tcp_mbedtls.c"
    INCLUDE_DIRS "."
    REQUIRES "Utilites" "mbedtls" "FreeRTOS_usr"
    PRIV_REQUIRES ""
)

target_compile_options(${COMPONENT_TARGET} PRIVATE -fno-rtti)