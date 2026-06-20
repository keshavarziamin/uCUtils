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

if(UCUTILS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(UCUTILS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
