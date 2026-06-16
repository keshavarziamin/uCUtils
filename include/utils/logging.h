#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/* log levels */
#define LOG_LEVEL_NONE 0  // no logging
#define LOG_LEVEL_ERROR 1 // error level logging
#define LOG_LEVEL_INFO 2  // info level logging (most detailed) error, info
#define LOG_LEVEL_DEBUG                                                        \
  3 // debug level logging (most detailed) error, info, debug
#define LOG_LEVEL_STATUS                                                       \
  4 // status level logging (most detailed) error, info, debug, success and fail
    // status
#define LOG_LEVEL_TRACE                                                        \
  5 // trace level logging (most detailed) error, info, debug, success and fail
    // status trace

#ifndef LOG_LEVEL
#error "LOG_LEVEL must be defined (e.g. -DLOG_LEVEL=LOG_LEVEL_STATUS)"
#endif

#ifndef LOG_MODULE_TAG
#error "LOG_MODULE_TAG must be defined (e.g. -DLOG_MODULE_TAG=\"MODULE_NAME\")"
#endif

/* low-level print helpers */
#define log_print(...) printf(__VA_ARGS__)
#define log_println(...)                                                       \
  do {                                                                         \
    printf(__VA_ARGS__);                                                       \
    printf("\r\n");                                                            \
  } while (0)

#define println(...)                                                           \
  do {                                                                         \
    printf(__VA_ARGS__);                                                       \
    printf("\r\n");                                                            \
  } while (0)

/* common log backend */
#define __log_detail(tag, level, fmt, ...)                                     \
  do {                                                                         \
    log_println("[%s][%s] %s:%d %s(): " fmt, tag, level, __FILE__, __LINE__,   \
                __func__, ##__VA_ARGS__);                                      \
  } while (0)

#define __log_print(tag, level, fmt, ...)                                      \
  do {                                                                         \
    log_println("[%s][%s] " fmt, tag, level, ##__VA_ARGS__);                   \
  } while (0)

/* level-controlled frontends */
#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define log_error(fmt, ...)                                                    \
  __log_detail(LOG_MODULE_TAG, "ERROR", fmt, ##__VA_ARGS__)
#else
#define log_error(tag, fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define log_info(fmt, ...)                                                     \
  __log_print(LOG_MODULE_TAG, "INFO", fmt, ##__VA_ARGS__)
#define log_success(fmt, ...)                                                  \
  __log_print(LOG_MODULE_TAG, "SUCCESS", fmt, ##__VA_ARGS__)
#define log_fail(fmt, ...)                                                     \
  __log_print(LOG_MODULE_TAG, "FAIL", fmt, ##__VA_ARGS__)
#else
#define log_info(fmt, ...)
#define log_success(fmt, ...)
#define log_fail(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define log_debug(fmt, ...)                                                    \
  __log_print(LOG_MODULE_TAG, "DEBUG", fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_TRACE
#define log_trace(fmt, ...)                                                    \
  __log_print(LOG_MODULE_TAG, "TRACE", fmt, ##__VA_ARGS__)
#else
#define log_trace(fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* UTILS_LOGGER_H */
