#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

/* log levels */
#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_DEBUG 2
#define LOG_LEVEL_TRACE 3

#ifndef LOG_LEVEL
#error ""
#endif
#ifndef LOG_MODULE_TAG
#error ""
#endif

/* low-level print helpers */
#define log_print(...) printf(__VA_ARGS__)
#define log_println(...)     \
    do                       \
    {                        \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)

#define println(...)         \
    do                       \
    {                        \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)

/* common log backend */
#define __log_detail(tag, level, fmt, ...)                    \
    do                                                        \
    {                                                         \
        log_println("[%s][%s] %s:%d %s(): " fmt,              \
                    tag, level, __FILE__, __LINE__, __func__, \
                    ##__VA_ARGS__);                           \
    } while (0)

#define __log_print(tag, level, fmt, ...)                        \
    do                                                           \
    {                                                            \
        log_println("[%s][%s] " fmt, tag, level, ##__VA_ARGS__); \
    } while (0)

/* level-controlled frontends */
#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define log_error(fmt, ...) \
    __log_detail(LOG_MODULE_TAG, "ERROR", fmt, ##__VA_ARGS__)
#else
#define log_error(tag, fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define log_info(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "INFO", fmt, ##__VA_ARGS__)
#define log_success(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "SUCCESS", fmt, ##__VA_ARGS__)
#define log_fail(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "FAIL", fmt, ##__VA_ARGS__)
#else
#define log_info(fmt, ...)
#define log_success(fmt, ...)
#define log_fail(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define log_debug(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "DEBUG", fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_TRACE
#define log_trace(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "TRACE", fmt, ##__VA_ARGS__)
#else
#define log_trace(fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* UTILS_LOGGER_H */
