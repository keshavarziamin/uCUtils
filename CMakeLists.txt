cmake_minimum_required(VERSION 3.16)

project(CUtils
    VERSION 0.1.0
    LANGUAGES C ASM
)

add_library(cutils INTERFACE)
add_library(CUtils::cutils ALIAS cutils)

target_include_directories(cutils
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(cutils INTERFACE c_std_11)

install(TARGETS cutils EXPORT cutilsTargets)
install(DIRECTORY include/ DESTINATION include)

install(EXPORT cutilsTargets
    FILE cutilsTargets.cmake
    NAMESPACE CUtils::
    DESTINATION lib/cmake/CUtils
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CUtilsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cutilsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CUtilsConfig.cmake"
    INSTALL_DESTINATION lib/cmake/CUtils
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CUtilsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CUtilsConfigVersion.cmake"
    DESTINATION lib/cmake/CUtils
)

option(CUTILS_BUILD_TESTS "Build CUtils compile tests" ON)

if(CUTILS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
