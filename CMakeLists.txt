cmake_minimum_required(VERSION 3.16)

project(UCUtils
    VERSION 0.1.0
    LANGUAGES C ASM
)

add_library(ucutils INTERFACE)
add_library(UCUtils::ucutils ALIAS ucutils)

target_include_directories(ucutils
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ucutils INTERFACE c_std_11)

install(TARGETS ucutils EXPORT ucutilsTargets)
install(DIRECTORY include/ DESTINATION include)

install(EXPORT ucutilsTargets
    FILE ucutilsTargets.cmake
    NAMESPACE UCUtils::
    DESTINATION lib/cmake/UCUtils
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/UCUtilsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ucutilsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/UCUtilsConfig.cmake"
    INSTALL_DESTINATION lib/cmake/UCUtils
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/UCUtilsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/UCUtilsConfigVersion.cmake"
    DESTINATION lib/cmake/UCUtils
)

option(UCUTILS_BUILD_TESTS "Build UCUtils compile tests" ON)
option(UCUTILS_BUILD_EXAMPLES "Build UCUtils examples" OFF)

set(UCUTILS_PROJECT_TARGET "PROJECT_TARGET_HOST" CACHE STRING
    "Target platform: PROJECT_TARGET_HOST or PROJECT_TARGET_STM32")
set(UCUTILS_LOG_SINK "LOG_SINK_STDIO" CACHE STRING
    "Log sink: LOG_SINK_STDIO, LOG_SINK_UART, or LOG_SINK_ITM")
set(UCUTILS_UART_HANDLE "" CACHE STRING
    "STM32 UART handle symbol for LOG_SINK_UART (e.g. huart1). Required when UCUTILS_LOG_SINK=LOG_SINK_UART")

if(UCUTILS_LOG_SINK STREQUAL "LOG_SINK_UART" AND UCUTILS_UART_HANDLE STREQUAL "")
    message(FATAL_ERROR
        "UCUTILS_LOG_SINK=LOG_SINK_UART requires UCUTILS_UART_HANDLE "
        "(e.g. -DUCUTILS_UART_HANDLE=huart1)")
endif()

target_compile_definitions(ucutils INTERFACE
    UCUTILS_PROJECT_TARGET=${UCUTILS_PROJECT_TARGET}
    UCUTILS_LOG_SINK=${UCUTILS_LOG_SINK}
)

if(NOT UCUTILS_UART_HANDLE STREQUAL "")
    target_compile_definitions(ucutils INTERFACE
        UCUTILS_UART_HANDLE=${UCUTILS_UART_HANDLE}
    )
endif()

if(UCUTILS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(UCUTILS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
