#ifndef UTILS_ERROR_HANDLER_H
#define UTILS_ERROR_HANDLER_H

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
#include <errno.h>
#include "logging.h"

/* ==========================================================
 *  Return status abstraction
 * ----------------------------------------------------------
 *  Explicit binary return model:
 *      RETURN_SUCCESS = 0
 *      RETURN_FAIL    = 1
 *
 *  NOTE:
 *      0 == success aligns with POSIX / Linux convention.
 * ==========================================================
 */
typedef enum
{
    RETURN_SUCCESS = 0,
    RETURN_FAIL = 1
} return_e;

/* ==========================================================
 *  Standardized return helpers
 * ----------------------------------------------------------
 *  These macros:
 *    - Log function context
 *    - Return immediately
 *    - Are statement-safe
 * ==========================================================
 */

#define return_fail()                               \
    do                                              \
    {                                               \
        log_fail("%s: execution failed", __func__); \
        return RETURN_FAIL;                         \
    } while (0)

#define return_success()                                     \
    do                                                       \
    {                                                        \
        log_success("%s: completed successfully", __func__); \
        return RETURN_SUCCESS;                               \
    } while (0)

/*
 * Helper condition macro.
 * Requires a local variable named `ret` of type return_e.
 */
#define if_return_fail(ret) if ((ret) == RETURN_FAIL)

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
 * ==========================================================
 */

#ifndef PANIC_ACT
#define PANIC_ACT exit(EXIT_FAILURE)
#endif

/*
 * panic_impl()
 *
 * Non-returning fatal handler.
 * Always terminates program flow.
 */
__attribute__((noreturn)) static inline void panic_impl(const char *file,
                                                        const char *func,
                                                        int line,
                                                        const char *reason)
{
    log_error("PANIC at %s:%d (%s) | %s",
              file, line, func, reason);

    PANIC_ACT;

    /* Defensive infinite loop for embedded builds */
    while (1)
    {
    }
}

/*
 * Panic macro with file/function/line context.
 */
#define panic(reason) \
    panic_impl(__FILE__, __func__, __LINE__, (reason))

/*
 * Conditional panic.
 */
#define panic_if(cond)    \
    do                    \
    {                     \
        if (cond)         \
        {                 \
            panic(#cond); \
        }                 \
    } while (0)

/* ==========================================================
 *  Conditional Return Helpers
 * ==========================================================
 */

/*
 * Return RETURN_FAIL if condition is true.
 */
#define return_fail_if(cond)                          \
    do                                                \
    {                                                 \
        if (cond)                                     \
        {                                             \
            log_error("Condition failed: %s", #cond); \
            return RETURN_FAIL;                       \
        }                                             \
    } while (0)

/*
 * Propagate failure from another function.
 *
 * Example:
 *      return_on_fail(init_driver());
 */
#define return_on_fail(expr)                    \
    do                                          \
    {                                           \
        return_e _ret = (expr);                 \
        if (_ret == RETURN_FAIL)                \
        {                                       \
            log_error("Failure in: %s", #expr); \
            return RETURN_FAIL;                 \
        }                                       \
    } while (0)

/*
 * Return NULL on failure condition.
 * Intended for pointer-returning functions.
 */
#define return_null_if(cond)                          \
    do                                                \
    {                                                 \
        if (cond)                                     \
        {                                             \
            log_error("Condition failed: %s", #cond); \
            return NULL;                              \
        }                                             \
    } while (0)

/*
 * Jump to cleanup label if condition is true.
 * Requires a label named `cleanup:` in the function.
 */
#define goto_cleanup_if(cond)                         \
    do                                                \
    {                                                 \
        if (cond)                                     \
        {                                             \
            log_error("Condition failed: %s", #cond); \
            goto cleanup;                             \
        }                                             \
    } while (0)

#endif /* UTILS_ERROR_HANDLER_H */
