add_executable(cutils_compile_test compile_test.c)

target_link_libraries(cutils_compile_test PRIVATE CUtils::cutils)

target_compile_definitions(cutils_compile_test PRIVATE
    LOG_LEVEL=LOG_LEVEL_STATUS
    LOG_MODULE_TAG=\"TEST\"
)

add_test(NAME cutils_compile_test COMMAND cutils_compile_test)
