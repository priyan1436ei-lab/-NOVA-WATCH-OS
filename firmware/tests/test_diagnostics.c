#include "test_runner.h"
#include "modules/diagnostics_manager.h"

static bool s_mock_bist_pass = true;
static nova_status_t mock_bist_test(void) {
    return s_mock_bist_pass ? NOVA_SUCCESS : NOVA_ERROR_IO;
}

static void test_diagnostics_subsystem_registration(void) {
    diagnostics_manager_init();
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, diagnostics_register_subsystem(SUBSYSTEM_HAL_DISPLAY, "AMOLED Display", mock_bist_test), "Register display");
    TEST_ASSERT_EQUAL(HEALTH_STATE_UNKNOWN, diagnostics_get_subsystem_state(SUBSYSTEM_HAL_DISPLAY), "Initial state UNKNOWN");
}

static void test_diagnostics_bist_execution(void) {
    diagnostics_manager_init();
    diagnostics_register_subsystem(SUBSYSTEM_HAL_DISPLAY, "AMOLED Display", mock_bist_test);

    /* Test PASS */
    s_mock_bist_pass = true;
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, diagnostics_run_bist(SUBSYSTEM_HAL_DISPLAY), "Run BIST pass");
    TEST_ASSERT_EQUAL(HEALTH_STATE_OK, diagnostics_get_subsystem_state(SUBSYSTEM_HAL_DISPLAY), "State should be OK");

    /* Test FAIL */
    s_mock_bist_pass = false;
    TEST_ASSERT_EQUAL(NOVA_ERROR_IO, diagnostics_run_bist(SUBSYSTEM_HAL_DISPLAY), "Run BIST fail");
    TEST_ASSERT_EQUAL(HEALTH_STATE_FAILED, diagnostics_get_subsystem_state(SUBSYSTEM_HAL_DISPLAY), "State should be FAILED");

    system_telemetry_t telem;
    diagnostics_get_telemetry(&telem);
    TEST_ASSERT_EQUAL(1, telem.total_bist_passed, "1 BIST passed");
    TEST_ASSERT_EQUAL(1, telem.total_bist_failed, "1 BIST failed");
}

static void test_diagnostics_telemetry(void) {
    diagnostics_manager_init();
    diagnostics_update_heap_stats(112 * 1024, 80 * 1024);

    system_telemetry_t telem;
    diagnostics_get_telemetry(&telem);
    TEST_ASSERT_EQUAL(112 * 1024, telem.total_heap_bytes, "Total heap match");
    TEST_ASSERT_EQUAL(80 * 1024, telem.free_heap_bytes, "Free heap match");
    TEST_ASSERT_EQUAL(80 * 1024, telem.min_free_heap_bytes, "Min free heap match");

    diagnostics_update_heap_stats(112 * 1024, 50 * 1024);
    diagnostics_get_telemetry(&telem);
    TEST_ASSERT_EQUAL(50 * 1024, telem.min_free_heap_bytes, "Min free heap tracked lower water mark");
}

void run_diagnostics_tests(void) {
    RUN_TEST(test_diagnostics_subsystem_registration);
    RUN_TEST(test_diagnostics_bist_execution);
    RUN_TEST(test_diagnostics_telemetry);
}
