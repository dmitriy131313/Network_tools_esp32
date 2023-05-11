#pragma once
#include <esp_log.h>

#define DEBUG_SSL_LOG_LEVEL_OFF 0
#define DEBUG_SSL_LOG_LEVEL_INFO 1
#define DEBUG_SSL_LOG_LEVEL_WARNING 2
#define DEBUG_SSL_LOG_LEVEL_ERROR 3

//#define DEBUG_SSL_LOG_TASK_STACK

#define DEBUG_SSL_LOG_LEVEL DEBUG_SSL_LOG_LEVEL_INFO

#if DEBUG_SSL_LOG_LEVEL == DEBUG_SSL_LOG_LEVEL_OFF
    #define DEBUG_SSL_LOG_I(tag, format, ...) void(0)
    #define DEBUG_SSL_LOG_W(tag, format, ...) void(0)
    #define DEBUG_SSL_LOG_E(tag, format, ...) void(0)
#elif DEBUG_SSL_LOG_LEVEL == DEBUG_SSL_LOG_LEVEL_INFO
    #define DEBUG_SSL_LOG_I(tag, format, ...) ESP_LOGI( tag, format, ##__VA_ARGS__ )
    #define DEBUG_SSL_LOG_W(tag, format, ...) ESP_LOGW( tag, format, ##__VA_ARGS__ )
    #define DEBUG_SSL_LOG_E(tag, format, ...) ESP_LOGE( tag, format, ##__VA_ARGS__ )
#elif DEBUG_SSL_LOG_LEVEL == DEBUG_SSL_LOG_LEVEL_WARNING
    #define DEBUG_SSL_LOG_I(tag, format, ...) void(0)
    #define DEBUG_SSL_LOG_W(tag, format, ...) ESP_LOGW( tag, format, ##__VA_ARGS__ )
    #define DEBUG_SSL_LOG_E(tag, format, ...) ESP_LOGE( tag, format, ##__VA_ARGS__ )
#elif DEBUG_SSL_LOG_LEVEL == DEBUG_SSL_LOG_LEVEL_ERROR
    #define DEBUG_SSL_LOG_I(tag, format, ...) void(0)
    #define DEBUG_SSL_LOG_W(tag, format, ...) void(0)
    #define DEBUG_SSL_LOG_E(tag, format, ...) ESP_LOGE( tag, format, ##__VA_ARGS__ )
#endif