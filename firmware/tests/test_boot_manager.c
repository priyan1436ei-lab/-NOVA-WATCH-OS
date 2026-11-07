#include "test_runner.h"
#include "modules/boot_manager.h"
#include "nova_config.h"

static void test_boot_manager_init_normal(void) {
    nova_status_t status = boot_manager_init(RESET_REASON_POWER_ON);
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, status, "Boot manager init should succeed");
    TEST_ASSERT_EQUAL(RESET_REASON_POWER_ON, boot_manager_get_reset_reason(), "Reset reason should match");
    TEST_ASSERT_EQUAL(BOOT_STAGE_PRE_INIT, boot_manager_get_current_stage(), "Initial stage should be PRE_INIT");
    TEST_ASSERT(!boot_manager_is_safe_mode(), "Safe mode should not be active on normal boot");
}

static void test_boot_stage_advancement(void) {
    boot_manager_init(RESET_REASON_POWER_ON);

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, boot_manager_advance_stage(BOOT_STAGE_EARLY_HAL), "Advance to EARLY_HAL");
    TEST_ASSERT_EQUAL(BOOT_STAGE_EARLY_HAL, boot_manager_get_current_stage(), "Stage should be EARLY_HAL");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, boot_manager_advance_stage(BOOT_STAGE_STORAGE), "Advance to STORAGE");
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, boot_manager_advance_stage(BOOT_STAGE_DIAGNOSTIC), "Advance to DIAGNOSTIC");
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, boot_manager_advance_stage(BOOT_STAGE_SERVICES), "Advance to SERVICES");
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, boot_manager_advance_stage(BOOT_STAGE_UI), "Advance to UI");
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, boot_manager_record_successful_boot(), "Advance to RUNNING");
    TEST_ASSERT_EQUAL(BOOT_STAGE_RUNNING, boot_manager_get_current_stage(), "Stage should be RUNNING");
}

static void test_crash_loop_safe_mode_trigger(void) {
    /* Simulate consecutive watchdog crash boots */
    boot_manager_init(RESET_REASON_WATCHDOG);
    TEST_ASSERT(!boot_manager_is_safe_mode(), "Crash 1 should not trigger safe mode");

    boot_manager_init(RESET_REASON_WATCHDOG);
    TEST_ASSERT(!boot_manager_is_safe_mode(), "Crash 2 should not trigger safe mode");

    nova_status_t status = boot_manager_init(RESET_REASON_WATCHDOG);
    TEST_ASSERT_EQUAL(NOVA_ERROR_BOOT_SAFE_MODE, status, "Crash 3 should return BOOT_SAFE_MODE");
    TEST_ASSERT(boot_manager_is_safe_mode(), "Safe mode should now be active");
    TEST_ASSERT_EQUAL(BOOT_STAGE_SAFE_MODE, boot_manager_get_current_stage(), "Stage should be locked to SAFE_MODE");

    /* Attempting to advance beyond safe mode should fail */
    status = boot_manager_advance_stage(BOOT_STAGE_UI);
    TEST_ASSERT_EQUAL(NOVA_ERROR_INVALID_STATE, status, "Advancing in safe mode must fail");

    /* Successful boot resets crash counter */
    boot_manager_record_successful_boot();
    boot_status_t stat;
    boot_manager_get_status(&stat);
    TEST_ASSERT_EQUAL(0, stat.crash_count, "Crash count must reset after successful boot");
}

static void test_watchdog_feeding(void) {
    nova_status_t status = boot_manager_feed_watchdog();
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, status, "Watchdog feed should succeed");
}

void run_boot_manager_tests(void) {
    RUN_TEST(test_boot_manager_init_normal);
    RUN_TEST(test_boot_stage_advancement);
    RUN_TEST(test_crash_loop_safe_mode_trigger);
    RUN_TEST(test_watchdog_feeding);
}

