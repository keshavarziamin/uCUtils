# CUtils

Header-only C utilities for embedded and system-level projects. Provides structured error handling, compile-time logging, and fatal panic helpers with minimal runtime overhead.

Part of [MCUBoost](https://github.com/MCUBoost).

## What

**CUtils** is a small, macro-based utility library:

| Header | Purpose |
|--------|---------|
| `include/cutils/logging.h` | Compile-time leveled logging (`log_error`, `log_info`, `log_debug`, …) |
| `include/cutils/error_handler.h` | Structured returns (`return_t`), failure propagation, panic helpers |

Functions return a `return_t` value instead of mixing `int`, `NULL`, and ad-hoc error codes:

```c
typedef struct {
    status_e status;  /* STATUS_SUCCESS or STATUS_FAILURE */
    int32_t  value;   /* payload on success, error code on failure */
} return_t;
```

Logging and return macros are **statement-safe** (`do { ... } while (0)`) and strip out at compile time when the log level is too low.

## Why

Embedded and firmware code often suffers from inconsistent error handling:

- Some functions return `0` / `-1`, others return `NULL`, others use custom enums
- `printf` debug output is left in production builds
- Fatal errors are handled differently on host vs MCU

This library standardizes those patterns:

- **Explicit control flow** — failures are visible and propagated deliberately
- **No hidden allocations** — macros only, no heap use
- **Compile-time log stripping** — reduce flash usage and runtime cost
- **Portable panic hook** — `exit()` on host, reset/watchdog on bare metal
- **POSIX-aligned success** — `STATUS_SUCCESS == 0`

## Requirements

- C11 compiler
- CMake 3.16+ (optional, for integration and tests)

## Quick start

### CMake (recommended)

```cmake
# Add as a subdirectory or fetch via FetchContent / install
add_subdirectory(path/to/CUtils)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE CUtils::cutils)

target_compile_definitions(my_app PRIVATE
    LOG_LEVEL=LOG_LEVEL_STATUS
    LOG_MODULE_TAG=\"MY_APP\"
)
```

```bash
cmake -S . -B build
cmake --build build
```

### Manual integration

Add `include/` to your compiler include path and define the required macros **before** including the headers:

```bash
gcc -std=c11 -Ipath/to/CUtils/include \
    -DLOG_LEVEL=LOG_LEVEL_STATUS \
    -DLOG_MODULE_TAG=\"MY_APP\" \
    main.c -o my_app
```

```c
#include <cutils/error_handler.h>

static return_t init_sensor(void)
{
    return_fail_if(sensor_hw_ready() == 0, -1);
    return_success();
}

static return_t app_run(void)
{
    return_on_fail(init_sensor());
    return_value(42);
}

int main(void)
{
    return_t result = app_run();

    if (return_is_failure(result)) {
        return (int)result.value;
    }

    return 0;
}
```

## Configuration

Define these **before** including `logging.h` (directly or via `error_handler.h`).

### Required

| Macro | Description | Example |
|-------|-------------|---------|
| `LOG_LEVEL` | Maximum log verbosity (see levels below) | `-DLOG_LEVEL=LOG_LEVEL_DEBUG` |
| `LOG_MODULE_TAG` | Short tag printed in every log line | `-DLOG_MODULE_TAG=\"SENSOR\"` |

### Optional

| Macro | Default | Description |
|-------|---------|-------------|
| `LOG_WRITE(...)` | `printf(__VA_ARGS__)` | Low-level output backend. Override for UART, RTT, semihosting, etc. |
| `ENDL` | `LOG_WRITE("\r\n")` | Line ending appended by `log_println` |
| `PANIC_ACT` | `exit(EXIT_FAILURE)` | Action taken after a panic. On MCU, use reset or watchdog. |

### Log levels

Levels are ordered from least to most verbose:

| Level | Value | Enabled macros |
|-------|-------|----------------|
| `LOG_LEVEL_NONE` | 0 | (all logging disabled) |
| `LOG_LEVEL_ERROR` | 1 | `log_error` |
| `LOG_LEVEL_INFO` | 2 | + `log_info` |
| `LOG_LEVEL_DEBUG` | 3 | + `log_debug` |
| `LOG_LEVEL_STATUS` | 4 | + `log_success`, `log_fail` |
| `LOG_LEVEL_TRACE` | 5 | + `log_trace` |

**Suggested defaults:**

| Build | `LOG_LEVEL` |
|-------|-------------|
| Release / production firmware | `LOG_LEVEL_ERROR` or `LOG_LEVEL_NONE` |
| Bring-up / development | `LOG_LEVEL_STATUS` or `LOG_LEVEL_DEBUG` |
| Deep debugging | `LOG_LEVEL_TRACE` |

### Example: custom log backend (embedded)

```c
#define LOG_WRITE(...) uart_write(__VA_ARGS__)
#define LOG_LEVEL       LOG_LEVEL_INFO
#define LOG_MODULE_TAG  "MOTOR"

#include <cutils/error_handler.h>
```

### Example: MCU panic handler

```c
#define PANIC_ACT NVIC_SystemReset()

#include <cutils/error_handler.h>
```

## API reference

### Return helpers

| Macro | Description |
|-------|-------------|
| `return_(status, value)` | Return a `return_t` with given status and value |
| `return_success()` | Return `STATUS_SUCCESS` with value `0` |
| `return_value(v)` | Return `STATUS_SUCCESS` with payload `v` |
| `return_failure(code)` | Log failure and return `STATUS_FAILURE` with error code |
| `return_fail_if(cond, code)` | Return failure when `cond` is true |
| `return_on_fail(expr)` | Call `expr`; if it fails, propagate the same `return_t` |
| `exit_on_fail(expr)` | Call `expr`; exit process on failure (for `main` / startup) |
| `return_null_if(cond)` | Return `NULL` when `cond` is true (pointer-returning functions) |
| `goto_cleanup_if(cond)` | `goto cleanup` when `cond` is true |

### Status helpers

| Macro | Description |
|-------|-------------|
| `return_is_success(ret)` | True when `ret.status == STATUS_SUCCESS` |
| `return_is_failure(ret)` | True when `ret.status == STATUS_FAILURE` |

### Panic helpers

| Macro | Description |
|-------|-------------|
| `panic(reason)` | Log fatal error with file/line/function and run `PANIC_ACT` |
| `panic_if(cond)` | Panic when `cond` is true |

Use panics only for unrecoverable faults (corruption, invariant violations, fatal hardware errors). Recoverable errors should use `return_failure` / `return_on_fail`.

### Logging macros

| Macro | Typical use |
|-------|-------------|
| `log_error(fmt, ...)` | Errors with file, line, and function |
| `log_info(fmt, ...)` | General information |
| `log_success(fmt, ...)` | Successful operation (used by return macros) |
| `log_fail(fmt, ...)` | Failed operation (used by return macros) |
| `log_debug(fmt, ...)` | Debug detail |
| `log_trace(fmt, ...)` | Verbose trace |
| `log_print(...)` | Raw output, no tag |
| `log_println(...)` | Raw output with line ending |
| `println(...)` | Alias for `log_println` |

## Project layout

```
CUtils/
├── include/cutils/
│   ├── logging.h
│   └── error_handler.h
├── cmake/
│   └── cutilsConfig.cmake.in
├── tests/
│   ├── CMakeLists.txt
│   └── compile_test.c
├── CMakeLists.txt
├── LICENSE
└── README.md
```

## Building tests

```bash
cmake -S . -B build -DCUTILS_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Disable tests when embedding the library:

```bash
cmake -S . -B build -DCUTILS_BUILD_TESTS=OFF
```

## Install

```bash
cmake -S . -B build
cmake --install build --prefix /path/to/install
```

Consumer CMake project:

```cmake
find_package(CUtils REQUIRED)
target_link_libraries(my_app PRIVATE CUtils::cutils)
```

## Design notes

- **Header-only** — no `.c` files to link; include and define macros
- **`return_t` vs `NULL`** — use `return_t` for operational results; use `return_null_if` only in pointer-returning APIs
- **Logging on success paths** — `return_success` / `return_value` emit `log_success` when `LOG_LEVEL >= LOG_LEVEL_STATUS`
- **C++** — headers use `extern "C"` guards and can be included from C++

## License

MIT License — see [LICENSE](LICENSE).
