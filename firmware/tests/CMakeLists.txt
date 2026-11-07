add_executable(nova_tests
    test_runner.c
    test_boot_manager.c
    test_hal.c
    test_diagnostics.c
    test_power_manager.c
)

target_include_directories(nova_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../include)
target_link_libraries(nova_tests PRIVATE nova_core)

add_test(NAME nova_unit_tests COMMAND nova_tests)

