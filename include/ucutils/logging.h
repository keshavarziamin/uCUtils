#ifndef UCUTILS_LOGGING_H
#define UCUTILS_LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/* ==========================================================
 *  Log levels (ascending verbosity)
 * ========================================================== */
#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_STATUS 4
#define LOG_LEVEL_TRACE 5

#ifndef LOG_LEVEL
#error "LOG_LEVEL must be defined (e.g. -DLOG_LEVEL=LOG_LEVEL_STATUS)"
#endif

#ifndef LOG_MODULE_TAG
#error "LOG_MODULE_TAG must be defined (e.g. -DLOG_MODULE_TAG=\"MODULE\")"
#endif


/* ==========================================================
 *  Target / log sink selection
 * ========================================================== */
#define PROJECT_TARGET_HOST 0
#define PROJECT_TARGET_STM32 1

#define LOG_SINK_STDIO 0
#define LOG_SINK_UART 1
#define LOG_SINK_ITM 2

#ifndef UCUTILS_PROJECT_TARGET
#define UCUTILS_PROJECT_TARGET PROJECT_TARGET_HOST
#endif

#ifndef UCUTILS_LOG_SINK
#define UCUTILS_LOG_SINK LOG_SINK_STDIO
#endif

/* ==========================================================
 *  Platform log backend
 *  STM32 + GCC: retarget printf via _write (UART or ITM)
 *  Otherwise: default printf backend below
 * ========================================================== */
#if (UCUTILS_PROJECT_TARGET == PROJECT_TARGET_STM32) && defined(__GNUC__)

#if (UCUTILS_LOG_SINK == LOG_SINK_UART)

#ifndef HAL_UART_MODULE_ENABLED
#error "STM32 UART log sink: include your STM32 HAL header before ucutils/logging.h"
#endif

#ifndef UCUTILS_UART_HANDLE
#define UCUTILS_UART_HANDLE huart1
#endif

extern UART_HandleTypeDef UCUTILS_UART_HANDLE;

static inline int ucutils_log_backend_write(char *ptr, int len) {
  HAL_UART_Transmit(&UCUTILS_UART_HANDLE, (uint8_t *)ptr, (uint16_t)len,
                    HAL_MAX_DELAY);
  return len;
}

#elif (UCUTILS_LOG_SINK == LOG_SINK_ITM)

#ifndef __CORTEX_M
#error "STM32 ITM log sink: include CMSIS core header (e.g. core_cm4.h) before ucutils/logging.h"
#endif

static inline int ucutils_log_backend_write(char *ptr, int len) {
  for (int i = 0; i < len; ++i) {
    if ((ITM->TCR & ITM_TCR_ITMENA_Msk) && (ITM->TER & 1UL)) {
      while (ITM->PORT[0].u32 == 0U) {
      }
      ITM->PORT[0].u8 = (uint8_t)ptr[i];
    }
  }
  return len;
}

#elif (UCUTILS_LOG_SINK != LOG_SINK_STDIO)

#error "Unsupported UCUTILS_LOG_SINK for STM32 target"

#endif /* UCUTILS_LOG_SINK */

#if (UCUTILS_LOG_SINK == LOG_SINK_UART) || (UCUTILS_LOG_SINK == LOG_SINK_ITM)

__attribute__((weak)) int _write(int fd, char *ptr, int len) {
  if (fd == 1 || fd == 2) {
    return ucutils_log_backend_write(ptr, len);
  }
  return -1;
}

#endif /* LOG_SINK_UART || LOG_SINK_ITM */

#elif (UCUTILS_PROJECT_TARGET == PROJECT_TARGET_STM32) && \
    ((UCUTILS_LOG_SINK == LOG_SINK_UART) || (UCUTILS_LOG_SINK == LOG_SINK_ITM))

#error "STM32 UART/ITM log sink requires GCC"

#endif /* PROJECT_TARGET_STM32 */

#ifndef LOG_WRITE
#define LOG_WRITE(...) printf(__VA_ARGS__)
#endif

#ifndef ENDL
#define ENDL LOG_WRITE("\r\n")
#endif

/* ==========================================================
 *  Low-level print helpers
 * ========================================================== */
#define log_print(...) LOG_WRITE(__VA_ARGS__)

#define log_println(...)                                                       \
  do {                                                                         \
    LOG_WRITE(__VA_ARGS__);                                                    \
    ENDL;                                                                      \
  } while (0)

#define println(...) log_println(__VA_ARGS__)

/* ==========================================================
 *  Internal backends
 * ========================================================== */
#define __log_detail(tag, level, fmt, ...)                                     \
  do {                                                                         \
    log_println("[%s][%s] %s:%d %s(): " fmt, tag, level, __FILE__, __LINE__,   \
                __func__, ##__VA_ARGS__);                                      \
  } while (0)

#define __log_print(tag, level, fmt, ...)                                      \
  do {                                                                         \
    log_println("[%s][%s] " fmt, tag, level, ##__VA_ARGS__);                   \
  } while (0)

/* ==========================================================
 *  Level-controlled frontends
 * ========================================================== */
#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define log_error(fmt, ...)                                                    \
  __log_detail(LOG_MODULE_TAG, "ERROR", fmt, ##__VA_ARGS__)
#else
#define log_error(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define log_info(fmt, ...)                                                     \
  __log_print(LOG_MODULE_TAG, "INFO", fmt, ##__VA_ARGS__)
#else
#define log_info(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_STATUS
#define log_success(fmt, ...)                                                  \
  __log_print(LOG_MODULE_TAG, "SUCCESS", fmt, ##__VA_ARGS__)
#define log_fail(fmt, ...)                                                     \
  __log_print(LOG_MODULE_TAG, "FAIL", fmt, ##__VA_ARGS__)
#else
#define log_success(fmt, ...) ((void)0)
#define log_fail(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define log_debug(fmt, ...)                                                    \
  __log_print(LOG_MODULE_TAG, "DEBUG", fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_TRACE
#define log_trace(fmt, ...)                                                    \
  __log_print(LOG_MODULE_TAG, "TRACE", fmt, ##__VA_ARGS__)
#else
#define log_trace(fmt, ...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* UCUTILS_LOGGING_H */
