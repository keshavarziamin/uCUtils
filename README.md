# UCUtils

[![Version](https://img.shields.io/badge/version-0.1.0-blue)](CMakeLists.txt)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![Standard](https://img.shields.io/badge/C-C11-blue)](CMakeLists.txt)

Header-only C utilities for embedded and system-level projects. Provides structured error handling, compile-time logging, and fatal panic helpers with minimal runtime overhead.


## Release v0.1.0

First public release. Establishes the core diagnostics foundation: logging macros, structured returns, and panic handling — all header-only with zero heap usage.

### Included modules

| Module | Header | Status |
|--------|--------|--------|
| Logging | `include/ucutils/logging.h` | Stable |
| Error handling | `include/ucutils/error_handler.h` | Stable |

### Features in v0.1.0

**Logging (`logging.h`)**
- Six compile-time log levels: `NONE`, `ERROR`, `INFO`, `DEBUG`, `STATUS`, `TRACE`
- Tagged output with module name, level, file, line, and function (`log_error`)
- Level-gated macros stripped at compile time when disabled
- Pluggable output backend via `LOG_WRITE` (default: `printf`)
- Configurable line ending via `ENDL` (default: `\r\n`)
- Raw helpers: `log_print`, `log_println`, `println`
- C++ compatible (`extern "C"`)

**Error handling (`error_handler.h`)**
- Portable `return_t` struct: `{ status_e status; int32_t value; }`
- Binary status model: `STATUS_SUCCESS` (0) / `STATUS_FAILURE` (1)
- Return macros: `return_success`, `return_value`, `return_failure`, `return_`
- Conditional helpers: `return_fail_if`, `return_on_fail`, `exit_on_fail`
- Cleanup pattern: `return_null_if`, `goto_cleanup_if`
- Status checks: `return_is_success`, `return_is_failure`
- Fatal panic: `panic`, `panic_if` with overridable `PANIC_ACT`
- Integrated logging on success, failure, and error paths

**Build & integration**
- CMake INTERFACE library: `UCUtils::ucutils`
- `find_package(UCUtils)` with install support
- Optional compile tests (`UCUTILS_BUILD_TESTS`, default ON)
- Optional examples (`UCUTILS_BUILD_EXAMPLES`, default OFF)
- C11 required; no `.c` source files to link

### Not in v0.2.0 (planned)

| Feature | Target |
|---------|--------|
| Lightweight formatter (replace `printf`) | v0.2.x |
| Ring buffer for async logging | v0.3.0 |
| Timestamps in log lines | v0.3.0 |
| Assert / fault handler module | v0.3.0 |

### New in v0.2.0

- **`install.sh`** — configure, build, test, and install for host and ARM Cortex-M presets
- **Generic ARM toolchain** — [`cmake/gcc-arm-none-eabi.cmake`](cmake/gcc-arm-none-eabi.cmake) (consumer sets CPU/FPU/linker script)
- **Log sinks** — `LOG_SINK_STDIO`, `LOG_SINK_UART`, `LOG_SINK_ITM` for STM32 + GCC
- **UART handle required** — `UCUTILS_UART_HANDLE` must be set in your project CMake (no silent default)

## What

**UCUtils** (micro C utilities) is a small, macro-based utility library:

| Header | Purpose |
|--------|---------|
| `include/ucutils/logging.h` | Compile-time leveled logging (`log_error`, `log_info`, `log_debug`, …) |
| `include/ucutils/error_handler.h` | Structured returns (`return_t`), failure propagation, panic helpers |

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
- CMake 3.16+ (recommended for integration, install, and tests)
- Optional: `arm-none-eabi-gcc` for cross-compiled firmware installs

## Integration tutorial

UCUtils is header-only and architecture-agnostic. Pick the integration path that fits your project.

| Method | Best for |
|--------|----------|
| [Install + `find_package`](#1-install--find_package-host-or-arm) | Separate firmware/app repos, multiple targets |
| [`add_subdirectory`](#2-add_subdirectory--git-submodule) | Monorepo, vendored copy, rapid iteration |
| [Manual `-I` / `-D`](#3-manual-compiler-flags-no-cmake) | Bare-metal without CMake, quick experiments |

**Important:** install prefix must match your target architecture:

| Target | Install command | Prefix |
|--------|-----------------|--------|
| Host (Linux/PC) | `./install.sh` | `~/.local/ucutils_host` |
| Cortex-M4 firmware | `./install.sh --arch cortex-m4` | `~/.local/ucutils_arm_m4` |
| Cortex-M7 firmware | `./install.sh --arch cortex-m7` | `~/.local/ucutils_arm_m7` |

---

### 1. Install + `find_package` (host or ARM)

#### Step 1 — Install UCUtils

```bash
git clone https://github.com/your-org/uCUtils.git
cd uCUtils

# Host (runs tests during install)
./install.sh

# ARM firmware (skips host tests; installs headers + CMake package)
./install.sh --arch cortex-m4
./install.sh --arch cortex-m0plus
./install.sh --arch cortex-m7 --build-type Release
```

Install options:

```bash
./install.sh --help
./install.sh --prefix /opt/ucutils_arm_m4 --arch cortex-m4
./install.sh --examples          # also build examples
./install.sh --no-tests          # skip ctest on host
```

#### Step 2 — Use in your project `CMakeLists.txt`

**Host application (Linux/PC):**

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_host_app C)

list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local/ucutils_host")
find_package(UCUtils REQUIRED)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE UCUtils::ucutils)

target_compile_definitions(my_app PRIVATE
    LOG_LEVEL=LOG_LEVEL_STATUS
    LOG_MODULE_TAG=\"MY_APP\"
)
```

```bash
cmake -S . -B build
cmake --build build
./build/my_app
```

**STM32 firmware (UART logging):**

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_firmware C)

list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local/ucutils_arm_m4")
find_package(UCUtils REQUIRED)

add_executable(my_firmware main.c startup.s)
target_link_libraries(my_firmware PRIVATE UCUtils::ucutils)

# UART handle comes from YOUR project (CubeMX/HAL), not from UCUtils defaults
target_compile_definitions(my_firmware PRIVATE
    UCUTILS_PROJECT_TARGET=PROJECT_TARGET_STM32
    UCUTILS_LOG_SINK=LOG_SINK_UART
    UCUTILS_UART_HANDLE=huart2
    LOG_LEVEL=LOG_LEVEL_DEBUG
    LOG_MODULE_TAG=\"FIRMWARE\"
)
```

Configure with your ARM toolchain:

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/your/toolchain.cmake \
  -DCMAKE_PREFIX_PATH="$HOME/.local/ucutils_arm_m4"

cmake --build build
```

In `main.c`, include HAL **before** UCUtils when using UART or ITM:

```c
#include "main.h"              /* your STM32 HAL / CubeMX generated headers */
#include <ucutils/error_handler.h>
```

**STM32 firmware (ITM / SWO logging):**

```cmake
target_compile_definitions(my_firmware PRIVATE
    UCUTILS_PROJECT_TARGET=PROJECT_TARGET_STM32
    UCUTILS_LOG_SINK=LOG_SINK_ITM
    LOG_LEVEL=LOG_LEVEL_DEBUG
    LOG_MODULE_TAG=\"FIRMWARE\"
)
```

Include CMSIS core header before UCUtils (e.g. `core_cm4.h` from your MCU pack).

---

### 2. `add_subdirectory` / git submodule

Add UCUtils inside your repo (no install step):

```bash
git submodule add https://github.com/your-org/uCUtils.git third_party/uCUtils
```

**`CMakeLists.txt`:**

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_project C)

add_subdirectory(third_party/uCUtils)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE UCUtils::ucutils)

target_compile_definitions(my_app PRIVATE
    LOG_LEVEL=LOG_LEVEL_STATUS
    LOG_MODULE_TAG=\"MY_APP\"
)
```

For STM32 UART inside a monorepo, set the UART handle on **your** target:

```cmake
target_compile_definitions(my_app PRIVATE
    UCUTILS_PROJECT_TARGET=PROJECT_TARGET_STM32
    UCUTILS_LOG_SINK=LOG_SINK_UART
    UCUTILS_UART_HANDLE=huart1
    LOG_LEVEL=LOG_LEVEL_DEBUG
    LOG_MODULE_TAG=\"MY_APP\"
)
```

Or pass options when configuring UCUtils itself:

```bash
cmake -S . -B build \
  -DUCUTILS_LOG_SINK=LOG_SINK_UART \
  -DUCUTILS_UART_HANDLE=huart1
```

CMake will fail at configure time if `LOG_SINK_UART` is selected without `UCUTILS_UART_HANDLE`.

---

### 3. Manual compiler flags (no CMake)

```bash
arm-none-eabi-gcc -std=c11 \
    -I/path/to/uCUtils/include \
    -I/path/to/STM32_HAL/Inc \
    -DUCUTILS_PROJECT_TARGET=PROJECT_TARGET_STM32 \
    -DUCUTILS_LOG_SINK=LOG_SINK_UART \
    -DUCUTILS_UART_HANDLE=huart1 \
    -DLOG_LEVEL=LOG_LEVEL_STATUS \
    -DLOG_MODULE_TAG=\"MY_APP\" \
    main.c -o my_firmware.elf
```

---

### 4. ARM toolchain file (consumer project)

UCUtils ships a **generic** [`cmake/gcc-arm-none-eabi.cmake`](cmake/gcc-arm-none-eabi.cmake). It does not hardcode an MCU — your project supplies CPU and linker script:

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/uCUtils/cmake/gcc-arm-none-eabi.cmake \
  -DARM_CPU=cortex-m4 \
  -DARM_FPU=fpv4-sp-d16 \
  -DARM_FLOAT_ABI=hard \
  -DARM_LINKER_SCRIPT=/path/to/your/linker.ld \
  -DCMAKE_PREFIX_PATH="$HOME/.local/ucutils_arm_m4"
```

Supported `install.sh --arch` presets:

| `--arch` | ARM_CPU | FPU | Float ABI |
|----------|---------|-----|-----------|
| `cortex-m0` | cortex-m0 | — | soft |
| `cortex-m0plus` | cortex-m0plus | — | soft |
| `cortex-m3` | cortex-m3 | — | soft |
| `cortex-m4` | cortex-m4 | fpv4-sp-d16 | hard |
| `cortex-m7` | cortex-m7 | fpv5-sp-d16 | hard |

---

### Minimal application example

```c
#include <ucutils/error_handler.h>

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

## Quick start

### CMake (subdirectory)

```cmake
add_subdirectory(path/to/uCUtils)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE UCUtils::ucutils)

target_compile_definitions(my_app PRIVATE
    LOG_LEVEL=LOG_LEVEL_STATUS
    LOG_MODULE_TAG=\"MY_APP\"
)
```

```bash
cmake -S . -B build
cmake --build build
```

## Configuration

Define these **before** including `logging.h` (directly or via `error_handler.h`).

### Required

| Macro | Description | Example |
|-------|-------------|---------|
| `LOG_LEVEL` | Maximum log verbosity (see levels below) | `-DLOG_LEVEL=LOG_LEVEL_DEBUG` |
| `LOG_MODULE_TAG` | Short tag printed in every log line | `-DLOG_MODULE_TAG=\"SENSOR\"` |

### Platform and log sink (optional)

| Macro | Default | Description |
|-------|---------|-------------|
| `UCUTILS_PROJECT_TARGET` | `PROJECT_TARGET_HOST` | `PROJECT_TARGET_HOST` or `PROJECT_TARGET_STM32` |
| `UCUTILS_LOG_SINK` | `LOG_SINK_STDIO` | `LOG_SINK_STDIO`, `LOG_SINK_UART`, or `LOG_SINK_ITM` |
| `UCUTILS_UART_HANDLE` | *(none)* | **Required** when `UCUTILS_LOG_SINK=LOG_SINK_UART` (e.g. `huart1`, `huart2`) |

When `UCUTILS_LOG_SINK=LOG_SINK_UART` is used without `UCUTILS_UART_HANDLE`, CMake configure and compilation both fail with an explicit error.

### Other optional macros

| Macro | Default | Description |
|-------|---------|-------------|
| `LOG_WRITE(...)` | `printf(__VA_ARGS__)` | Low-level output backend. Override for custom sinks |
| `ENDL` | `LOG_WRITE("\r\n")` | Line ending appended by `log_println` |
| `PANIC_ACT` | `exit(EXIT_FAILURE)` | Action taken after a panic. On MCU, use reset or watchdog |

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

### Example: STM32 UART log sink

Set the UART handle in your **project root** `CMakeLists.txt`:

```cmake
target_compile_definitions(my_firmware PRIVATE
    UCUTILS_PROJECT_TARGET=PROJECT_TARGET_STM32
    UCUTILS_LOG_SINK=LOG_SINK_UART
    UCUTILS_UART_HANDLE=huart2
    LOG_LEVEL=LOG_LEVEL_INFO
    LOG_MODULE_TAG=\"MOTOR\"
)
```

```c
#include "main.h"   /* STM32 HAL must come first */
#include <ucutils/error_handler.h>
```

### Example: custom log backend (embedded)

```c
#define LOG_WRITE(...) my_custom_write(__VA_ARGS__)
#define LOG_LEVEL       LOG_LEVEL_INFO
#define LOG_MODULE_TAG  "MOTOR"

#include <ucutils/error_handler.h>
```

### Example: MCU panic handler

```c
#define PANIC_ACT NVIC_SystemReset()

#include <ucutils/error_handler.h>
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
UCUtils/
├── include/ucutils/
│   ├── logging.h
│   └── error_handler.h
├── cmake/
│   ├── gcc-arm-none-eabi.cmake   # generic ARM toolchain (consumer sets CPU/linker)
│   └── ucutilsConfig.cmake.in
├── examples/
│   ├── CMakeLists.txt
│   └── error_handler_exmpale.c
├── tests/
│   ├── CMakeLists.txt
│   └── compile_test.c
├── install.sh                    # build, test, install (host + ARM presets)
├── CMakeLists.txt
├── LICENSE
└── README.md
```

## Building UCUtils (development)

### Using `install.sh` (recommended)

```bash
./install.sh                          # host → ~/.local/ucutils_host
./install.sh --arch cortex-m4         # ARM  → ~/.local/ucutils_arm_m4
./install.sh --examples --build-type Release
```

### Manual CMake

```bash
cmake -S . -B build -DUCUTILS_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Disable tests when embedding the library:

```bash
cmake -S . -B build -DUCUTILS_BUILD_TESTS=OFF
```

### Examples

```bash
cmake -S . -B build -DUCUTILS_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Or via install script:

```bash
./install.sh --examples
```

## Install

```bash
# Quick install (host, with tests)
./install.sh

# Custom prefix
./install.sh --arch cortex-m4 --prefix /opt/ucutils_arm_m4

# Manual equivalent
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
cmake --install build --prefix ~/.local/ucutils_host
```

Consumer CMake project after install:

```cmake
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local/ucutils_host")
find_package(UCUtils REQUIRED)
target_link_libraries(my_app PRIVATE UCUtils::ucutils)
```

## Design notes

- **Header-only** — no `.c` files to link; include and define macros
- **`return_t` vs `NULL`** — use `return_t` for operational results; use `return_null_if` only in pointer-returning APIs
- **Logging on success paths** — `return_success` / `return_value` emit `log_success` when `LOG_LEVEL >= LOG_LEVEL_STATUS`
- **C++** — headers use `extern "C"` guards and can be included from C++

## Changelog

### v0.2.0

- Add `install.sh` with host and Cortex-M architecture presets
- Add generic `cmake/gcc-arm-none-eabi.cmake` toolchain file
- Add STM32 log sinks: `LOG_SINK_UART`, `LOG_SINK_ITM` (GCC, `_write` retarget)
- Require explicit `UCUTILS_UART_HANDLE` for UART sink (set in consumer project CMake)
- Expand README with integration tutorials for host, ARM, submodule, and manual use

### v0.1.0

- Initial release as **UCUtils** (micro C utilities)
- Add `logging.h` with compile-time level stripping and `LOG_WRITE` backend hook
- Add `error_handler.h` with `return_t`, propagation macros, and panic helpers
- Add CMake INTERFACE target `UCUtils::ucutils` with install and `find_package` support
- Add compile test and error-handler example
- MIT license

## License

MIT License — see [LICENSE](LICENSE).
