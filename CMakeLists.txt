cmake_minimum_required(VERSION 3.16)

project(utils
    VERSION 0.1.0
    LANGUAGES C ASM
)

add_library(utils INTERFACE)
add_library(utils::utils ALIAS utils)

target_include_directories(utils
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(utils INTERFACE c_std_11)

install(TARGETS utils EXPORT utilsTargets)
install(DIRECTORY include/ DESTINATION include)

install(EXPORT utilsTargets
    FILE utilsTargets.cmake
    NAMESPACE utils::
    DESTINATION lib/cmake/utils
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/utilsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/utilsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/utilsConfig.cmake"
    INSTALL_DESTINATION lib/cmake/utils
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/utilsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/utilsConfigVersion.cmake"
    DESTINATION lib/cmake/utils
)

option(UTILS_BUILD_TESTS "Build utils compile tests" ON)

if(UTILS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
