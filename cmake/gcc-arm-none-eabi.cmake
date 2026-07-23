# Generic GNU Arm Embedded (arm-none-eabi) toolchain file.
#
# uCUtils stays architecture-agnostic; the consuming project selects the MCU.
# Example (Cortex-M4 with FPU):
#   cmake -S . -B build \
#     -DCMAKE_TOOLCHAIN_FILE=/path/to/uCUtils/cmake/gcc-arm-none-eabi.cmake \
#     -DARM_CPU=cortex-m4 \
#     -DARM_FPU=fpv4-sp-d16 \
#     -DARM_FLOAT_ABI=hard \
#     -DARM_LINKER_SCRIPT=/path/to/your/linker.ld
#
# Example (Cortex-M0+, no FPU):
#   cmake -S . -B build \
#     -DCMAKE_TOOLCHAIN_FILE=/path/to/uCUtils/cmake/gcc-arm-none-eabi.cmake \
#     -DARM_CPU=cortex-m0plus \
#     -DARM_FLOAT_ABI=soft \
#     -DARM_LINKER_SCRIPT=/path/to/your/linker.ld

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

set(TOOLCHAIN_PREFIX arm-none-eabi- CACHE STRING "GNU Arm Embedded toolchain prefix")

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# MCU-specific settings (provided by the consuming project)
set(ARM_CPU "" CACHE STRING "Target CPU for -mcpu= (e.g. cortex-m0plus, cortex-m4, cortex-m7)")
set(ARM_FPU "" CACHE STRING "FPU for -mfpu= (leave empty if none, e.g. fpv4-sp-d16)")
set(ARM_FLOAT_ABI "" CACHE STRING "Float ABI for -mfloat-abi= (e.g. soft, softfp, hard)")
set(ARM_LINKER_SCRIPT "" CACHE FILEPATH "Linker script for -T (optional)")
set(ARM_SPECS nano CACHE STRING "Newlib specs file suffix: nano, nosys, or empty to omit")

set(TARGET_FLAGS "")
if(ARM_CPU)
  set(TARGET_FLAGS "${TARGET_FLAGS} -mcpu=${ARM_CPU}")
else()
  message(WARNING "ARM_CPU is not set. Pass -DARM_CPU=<core> for correct code generation.")
endif()

if(ARM_FPU)
  set(TARGET_FLAGS "${TARGET_FLAGS} -mfpu=${ARM_FPU}")
endif()

if(ARM_FLOAT_ABI)
  set(TARGET_FLAGS "${TARGET_FLAGS} -mfloat-abi=${ARM_FLOAT_ABI}")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fdata-sections -ffunction-sections")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS}")

if(ARM_LINKER_SCRIPT)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${ARM_LINKER_SCRIPT}\"")
endif()

if(ARM_SPECS)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=${ARM_SPECS}.specs")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
set(TOOLCHAIN_LINK_LIBRARIES "m")
