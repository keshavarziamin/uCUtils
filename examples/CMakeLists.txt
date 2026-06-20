cmake_minimum_required(VERSION 3.16)

project(ucutils_examples)

add_executable(${PROJECT_NAME}_error_handler error_handler_exmpale.c)
target_compile_features(${PROJECT_NAME}_error_handler PRIVATE c_std_11)
target_link_libraries(${PROJECT_NAME}_error_handler PRIVATE UCUtils::ucutils)

target_compile_options(${PROJECT_NAME}_error_handler PRIVATE -Wall -Wextra -Werror)

target_compile_definitions(${PROJECT_NAME}_error_handler PRIVATE
    LOG_LEVEL=LOG_LEVEL_STATUS
    LOG_MODULE_TAG=\"ERROR_HANDLER\"
)
add_test(NAME ${PROJECT_NAME}_error_handler COMMAND ${PROJECT_NAME}_error_handler)
