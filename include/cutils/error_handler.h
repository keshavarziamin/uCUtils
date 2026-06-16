#ifndef CUTILS_ERROR_HANDLER_H
#define CUTILS_ERROR_HANDLER_H

/*
 * ==========================================================
 *  Error Handling Utilities
 *  ----------------------------------------------------------
 *  Linux-kernel style failure handling helpers for
 *  embedded / system-level C projects.
 *
 *  Design goals:
 *    - Explicit control flow
 *    - No hidden allocations
 *    - Statement-safe macros
 *    - Minimal runtime overhead
 * ==========================================================
 */

#include <stdlib.h>
#include <stdint.h>

#include "logging.h"

/* ==========================================================
 *  Return status abstraction
 * ----------------------------------------------------------
 *  Explicit binary return model:
 *      STATUS_SUCCESS = 0
 *      STATUS_FAILURE = 1
 *
 *  NOTE:
 *      0 == success aligns with POSIX / Linux convention.
 * ========================================================== */
typedef enum {
    STATUS_SUCCESS = 0, /* no error */
    STATUS_FAILURE = 1,
} status_e;

typedef struct {
    status_e status;
    int32_t  value; /* payload on success, error code on failure */
} return_t;

#define return_is_success(ret) ((ret).status == STATUS_SUCCESS)
#define return_is_failure(ret) ((ret).status == STATUS_FAILURE)

/* ==========================================================
 *  Standardized return helpers
 * ----------------------------------------------------------
 *  These macros:
 *    - Log function context
 *    - Return immediately
 *    - Are statement-safe
 * ========================================================== */

#define return_(_status, _value)                                    \
    do {                                                            \
        log_debug("%s: return %s with value = %d",                  \
                  __func__, #_status, (int)(_value));               \
        return_t _ret = {                                           \
            .status = (_status),                                    \
            .value  = (_value),                                     \
        };                                                          \
        return _ret;                                                \
    } while (0)

#define return_failure(_error_code)                                 \
    do {                                                            \
        log_fail("%s: execution failed", __func__);                 \
        return_(STATUS_FAILURE, _error_code);                       \
    } while (0)

#define return_success()                                          \
    do {                                                            \
        log_success("%s: completed successfully", __func__);      \
        return_(STATUS_SUCCESS, 0);                                 \
    } while (0)

#define return_value(_value)                                      \
    do {                                                            \
        log_success("%s: completed successfully", __func__);      \
        return_(STATUS_SUCCESS, _value);                            \
    } while (0)

/* ==========================================================
 *  Panic handling
 * ----------------------------------------------------------
 *  Intended for unrecoverable conditions:
 *      - Memory corruption
 *      - Invariant violation
 *      - Fatal hardware fault
 *
 *  Default behavior:
 *      exit(EXIT_FAILURE)
 *
 *  For bare-metal MCU, redefine PANIC_ACT to:
 *      - NVIC_SystemReset()
 *      - while(1)
 *      - watchdog reset
 * ========================================================== */

#ifndef PANIC_ACT
#define PANIC_ACT exit(EXIT_FAILURE)
#endif

__attribute__((noreturn)) static inline void panic_impl(const char *file,
                                                        const char *func,
                                                        int line,
                                                        const char *reason)
{
    log_error("PANIC at %s:%d (%s) | %s", file, line, func, reason);
    PANIC_ACT;
    while (1) {
    }
}

#define panic(reason) \
    panic_impl(__FILE__, __func__, __LINE__, (reason))

#define panic_if(cond)              \
    do {                            \
        if (cond) {                 \
            panic(#cond);           \
        }                           \
    } while (0)

/* ==========================================================
 *  Conditional return helpers
 * ========================================================== */

#define return_fail_if(_cond, _error)                               \
    do {                                                            \
        if (_cond) {                                                \
            log_error("Condition failed: %s", #_cond);              \
            return_failure(_error);                                 \
        }                                                           \
    } while (0)

#define return_on_fail(expr)                                        \
    do {                                                            \
        return_t _ret = (expr);                                     \
        if (return_is_failure(_ret)) {                              \
            log_error("Failure in: %s", #expr);                     \
            return _ret;                                            \
        }                                                           \
    } while (0)

#define exit_on_fail(expr)                                          \
    do {                                                            \
        return_t _ret = (expr);                                     \
        if (return_is_failure(_ret)) {                              \
            log_error("Failure in: %s", #expr);                     \
            exit(EXIT_FAILURE);                                     \
        }                                                           \
    } while (0)

#define return_null_if(cond)                                        \
    do {                                                            \
        if (cond) {                                                 \
            log_error("Condition failed: %s", #cond);               \
            return NULL;                                            \
        }                                                           \
    } while (0)

#define goto_cleanup_if(cond)                                       \
    do {                                                            \
        if (cond) {                                                 \
            log_error("Condition failed: %s", #cond);               \
            goto cleanup;                                           \
        }                                                           \
    } while (0)

#endif /* CUTILS_ERROR_HANDLER_H */
