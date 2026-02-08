cmake_minimum_required(VERSION 4.2)
project(utils
LANGUAGES C ASM
VERSION 0.1.0)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(${PROJECT_NAME}
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)