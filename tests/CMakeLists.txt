add_executable(ucutils_compile_test compile_test.c)

target_link_libraries(ucutils_compile_test PRIVATE UCUtils::ucutils)

target_compile_definitions(ucutils_compile_test PRIVATE
    LOG_LEVEL=LOG_LEVEL_STATUS
    LOG_MODULE_TAG=\"TEST\"
)

add_test(NAME ucutils_compile_test COMMAND ucutils_compile_test)
