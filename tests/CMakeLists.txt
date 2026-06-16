add_executable(utils_compile_test compile_test.c)

target_link_libraries(utils_compile_test PRIVATE utils::utils)

target_compile_definitions(utils_compile_test PRIVATE
    LOG_LEVEL=LOG_LEVEL_STATUS
    LOG_MODULE_TAG=\"TEST\"
)

add_test(NAME utils_compile_test COMMAND utils_compile_test)
