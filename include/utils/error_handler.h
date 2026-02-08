#pragma once
/*----------------------------------------------------------
 * panic / STATUS_ERROR-handling utilities (Linux kernel style)
 *----------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "logging.h"

/*----------------------------------------------------------
 * STATUS_ERROR codes
 *----------------------------------------------------------*/
typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = 1,
    STATUS_TIMEOUT = 2,
    STAUTS_BUSY = 3,
} status_e;

#define PANIC_ACT exit(EXIT_FAILURE)

/*----------------------------------------------------------
 * Panic implementation (never returns)
 *----------------------------------------------------------*/
__attribute__((noreturn))
static inline void panic_impl()
{
    log_println("Panic occurred! Halting execution.");
    PANIC_ACT;
}

/*----------------------------------------------------------
 * Macros (statement-safe, kernel-style naming)
 *----------------------------------------------------------*/
#define panic_if(cond)                                     \
    do {                                                   \
        if (cond){                                         \
            log_error("TAG",#cond);\
            panic_impl();\
        }                                  \
    } while (0)

#define return_error_if(cond)                              \
    do {                                                   \
        if (cond) {                                       \
            log_error("TAG",#cond);         \
            return STATUS_ERROR;                           \
        }                                                  \
    } while (0)

#define return_null_if(cond)                               \
    do {                                                   \
        if (cond) {                                       \
            log_error("TAG",#cond);         \
            return NULL;                                   \
        }                                                  \
    } while (0)

#define goto_cleanup_if(cond)                              \
    do {                                                   \
        if (cond) {                                       \
           log_error("TAG",#cond);         \
            printf("goto cleanup to free allocated memory.\r\n");             \
            goto cleanup;                                 \
        }                                                  \
    } while (0)
