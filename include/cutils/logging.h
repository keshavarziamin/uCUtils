#ifndef CUTILS_LOGGING_H
#define CUTILS_LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/* ==========================================================
 *  Log levels (ascending verbosity)
 * ========================================================== */
#define LOG_LEVEL_NONE   0
#define LOG_LEVEL_ERROR  1
#define LOG_LEVEL_INFO   2
#define LOG_LEVEL_DEBUG  3
#define LOG_LEVEL_STATUS 4
#define LOG_LEVEL_TRACE  5

#ifndef LOG_LEVEL
#error "LOG_LEVEL must be defined (e.g. -DLOG_LEVEL=LOG_LEVEL_STATUS)"
#endif

#ifndef LOG_MODULE_TAG
#error "LOG_MODULE_TAG must be defined (e.g. -DLOG_MODULE_TAG=\"MODULE\")"
#endif

#ifndef LOG_WRITE
#define LOG_WRITE(...) printf(__VA_ARGS__)
#endif

#ifndef ENDL
#define ENDL LOG_WRITE("\r\n")
#endif

/* ==========================================================
 *  Low-level print helpers
 * ========================================================== */
#define log_print(...)   LOG_WRITE(__VA_ARGS__)

#define log_println(...)              \
    do {                              \
        LOG_WRITE(__VA_ARGS__);       \
        ENDL;                         \
    } while (0)

#define println(...) log_println(__VA_ARGS__)

/* ==========================================================
 *  Internal backends
 * ========================================================== */
#define __log_detail(tag, level, fmt, ...)                          \
    do {                                                            \
        log_println("[%s][%s] %s:%d %s(): " fmt,                    \
                    tag, level, __FILE__, __LINE__, __func__,       \
                    ##__VA_ARGS__);                                 \
    } while (0)

#define __log_print(tag, level, fmt, ...)                           \
    do {                                                            \
        log_println("[%s][%s] " fmt, tag, level, ##__VA_ARGS__);    \
    } while (0)

/* ==========================================================
 *  Level-controlled frontends
 * ========================================================== */
#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define log_error(fmt, ...) \
    __log_detail(LOG_MODULE_TAG, "ERROR", fmt, ##__VA_ARGS__)
#else
#define log_error(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define log_info(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "INFO", fmt, ##__VA_ARGS__)
#else
#define log_info(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_STATUS
#define log_success(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "SUCCESS", fmt, ##__VA_ARGS__)
#define log_fail(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "FAIL", fmt, ##__VA_ARGS__)
#else
#define log_success(fmt, ...) ((void)0)
#define log_fail(fmt, ...)    ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define log_debug(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "DEBUG", fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_TRACE
#define log_trace(fmt, ...) \
    __log_print(LOG_MODULE_TAG, "TRACE", fmt, ##__VA_ARGS__)
#else
#define log_trace(fmt, ...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* CUTILS_LOGGING_H */
