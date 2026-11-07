#include "test_runner.h"
#include "modules/power_manager.h"
#include "modules/hal_manager.h"

static nova_power_mode_t s_last_old_mode = POWER_MODE_ACTIVE;
static nova_power_mode_t s_last_new_mode = POWER_MODE_ACTIVE;
static void on_mode_change(nova_power_mode_t old_m, nova_power_mode_t new_m, void *user_data) {
    (void)user_data;
    s_last_old_mode = old_m;
    s_last_new_mode = new_m;
}

static battery_alert_level_t s_last_alert = BATTERY_ALERT_NONE;
static uint8_t s_last_soc = 0;
static void on_alert(battery_alert_level_t level, uint8_t soc, void *user_data) {
    (void)user_data;
    s_last_alert = level;
    s_last_soc = soc;
}

static void test_power_manager_modes(void) {
    hal_manager_init(HAL_DRIVER_MODE_MOCK);
    power_manager_init(on_mode_change, on_alert, NULL);

    TEST_ASSERT_EQUAL(POWER_MODE_ACTIVE, power_manager_get_mode(), "Initial mode ACTIVE");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, power_manager_set_mode(POWER_MODE_BALANCED), "Switch to BALANCED");
    TEST_ASSERT_EQUAL(POWER_MODE_BALANCED, power_manager_get_mode(), "Mode should be BALANCED");
    TEST_ASSERT_EQUAL(POWER_MODE_ACTIVE, s_last_old_mode, "Old mode was ACTIVE");
    TEST_ASSERT_EQUAL(POWER_MODE_BALANCED, s_last_new_mode, "New mode is BALANCED");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, power_manager_set_mode(POWER_MODE_BATTERY_SAVER), "Switch to SAVER");
    TEST_ASSERT_EQUAL(POWER_MODE_BATTERY_SAVER, power_manager_get_mode(), "Mode should be SAVER");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, power_manager_set_mode(POWER_MODE_ULTRA_SAVER), "Switch to ULTRA");
    TEST_ASSERT_EQUAL(POWER_MODE_ULTRA_SAVER, power_manager_get_mode(), "Mode should be ULTRA");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, power_manager_set_mode(POWER_MODE_SLEEP), "Switch to SLEEP");
    TEST_ASSERT_EQUAL(POWER_MODE_SLEEP, power_manager_get_mode(), "Mode should be SLEEP");

    /* Restore active */
    power_manager_set_mode(POWER_MODE_ACTIVE);
}

static void test_power_voltage_to_soc_curve(void) {
    TEST_ASSERT_EQUAL(100, power_manager_voltage_to_soc(4200), "4200mV -> 100%");
    TEST_ASSERT_EQUAL(100, power_manager_voltage_to_soc(4350), ">4200mV clamped to 100%");
    TEST_ASSERT_EQUAL(90, power_manager_voltage_to_soc(4050), "4050mV -> 90%");
    TEST_ASSERT_EQUAL(80, power_manager_voltage_to_soc(3920), "3920mV -> 80%");
    TEST_ASSERT_EQUAL(50, power_manager_voltage_to_soc(3720), "3720mV -> 50%");
    TEST_ASSERT_EQUAL(20, power_manager_voltage_to_soc(3550), "3550mV -> 20%");
    TEST_ASSERT_EQUAL(10, power_manager_voltage_to_soc(3450), "3450mV -> 10%");
    TEST_ASSERT_EQUAL(0, power_manager_voltage_to_soc(3000), "3000mV -> 0%");
    TEST_ASSERT_EQUAL(0, power_manager_voltage_to_soc(2800), "<3000mV clamped to 0%");
}

static void test_power_battery_alerts(void) {
    hal_manager_init(HAL_DRIVER_MODE_MOCK);
    s_last_alert = BATTERY_ALERT_NONE;
    power_manager_init(on_mode_change, on_alert, NULL);

    /* Normal battery (85%) */
    hal_power_mock_set_battery(3950, 85, HAL_CHARGE_STATE_DISCHARGING);
    power_manager_update();
    TEST_ASSERT_EQUAL(BATTERY_ALERT_NONE, power_manager_get_alert_level(), "85% should produce no alert");

    /* Low battery alert (18% < 20%) */
    hal_power_mock_set_battery(3520, 18, HAL_CHARGE_STATE_DISCHARGING);
    power_manager_update();
    TEST_ASSERT_EQUAL(BATTERY_ALERT_LOW, power_manager_get_alert_level(), "18% should trigger LOW alert");
    TEST_ASSERT_EQUAL(BATTERY_ALERT_LOW, s_last_alert, "Alert callback received LOW");
    TEST_ASSERT_EQUAL(18, s_last_soc, "Alert callback SoC 18%");

    /* Critical battery alert (8% < 10%) */
    hal_power_mock_set_battery(3400, 8, HAL_CHARGE_STATE_DISCHARGING);
    power_manager_update();
    TEST_ASSERT_EQUAL(BATTERY_ALERT_CRITICAL, power_manager_get_alert_level(), "8% should trigger CRITICAL alert");
    TEST_ASSERT_EQUAL(BATTERY_ALERT_CRITICAL, s_last_alert, "Alert callback received CRITICAL");

    /* Shutdown alert (2% < 3%) */
    hal_power_mock_set_battery(3080, 2, HAL_CHARGE_STATE_DISCHARGING);
    power_manager_update();
    TEST_ASSERT_EQUAL(BATTERY_ALERT_SHUTDOWN, power_manager_get_alert_level(), "2% should trigger SHUTDOWN alert");
    TEST_ASSERT_EQUAL(BATTERY_ALERT_SHUTDOWN, s_last_alert, "Alert callback received SHUTDOWN");
}

void run_power_manager_tests(void) {
    RUN_TEST(test_power_manager_modes);
    RUN_TEST(test_power_voltage_to_soc_curve);
    RUN_TEST(test_power_battery_alerts);
}
