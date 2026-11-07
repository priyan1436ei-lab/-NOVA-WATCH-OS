#include "test_runner.h"
#include "modules/hal_manager.h"
#include <string.h>

static bool s_flush_called = false;
static void dummy_flush_cb(void) {
    s_flush_called = true;
}

static void test_hal_display(void) {
    hal_manager_init(HAL_DRIVER_MODE_MOCK);
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_display_init(), "Display init");
    TEST_ASSERT(hal_display_is_ready(), "Display should be ready");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_display_set_brightness(80), "Set brightness 80%");
    TEST_ASSERT_EQUAL(NOVA_ERROR_INVALID_PARAM, hal_display_set_brightness(120), "Set brightness 120% should fail");

    hal_display_area_t area = {0, 0, 100, 100};
    uint16_t buffer[100];
    s_flush_called = false;
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_display_flush(&area, buffer, dummy_flush_cb), "Flush display");
    TEST_ASSERT(s_flush_called, "Flush callback should be invoked");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_display_set_power(false), "Display power off");
    TEST_ASSERT(!hal_display_is_ready(), "Display should not be ready when power off");
    TEST_ASSERT_EQUAL(NOVA_ERROR_INVALID_STATE, hal_display_flush(&area, buffer, NULL), "Flush when power off should fail");

    hal_display_set_power(true);
}

static bool s_touch_invoked = false;
static hal_touch_data_t s_received_touch;
static void touch_callback(const hal_touch_data_t *data, void *user_data) {
    (void)user_data;
    if (data) {
        s_received_touch = *data;
        s_touch_invoked = true;
    }
}

static void test_hal_touch(void) {
    s_touch_invoked = false;
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_touch_init(touch_callback, NULL), "Touch init");
    TEST_ASSERT(hal_touch_is_ready(), "Touch ready");

    hal_touch_data_t inject = {
        .point_count = 1,
        .points[0] = { .x = 180, .y = 180, .pressure = 100, .event = HAL_TOUCH_EVENT_DOWN },
        .gesture = HAL_GESTURE_TAP,
        .timestamp_ms = 1234
    };

    hal_touch_mock_inject(&inject);
    TEST_ASSERT(s_touch_invoked, "Touch callback triggered");
    TEST_ASSERT_EQUAL(180, s_received_touch.points[0].x, "X coord match");
    TEST_ASSERT_EQUAL(180, s_received_touch.points[0].y, "Y coord match");
    TEST_ASSERT_EQUAL(HAL_GESTURE_TAP, s_received_touch.gesture, "Gesture match");

    hal_touch_data_t read_out;
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_touch_read(&read_out), "Touch read");
    TEST_ASSERT_EQUAL(180, read_out.points[0].x, "Readout X match");

    hal_touch_set_sleep(true);
    TEST_ASSERT(!hal_touch_is_ready(), "Touch should not be ready when in sleep");
    hal_touch_set_sleep(false);
}

static void test_hal_sensor(void) {
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_sensor_init(), "Sensor init");
    TEST_ASSERT(hal_sensor_is_ready(), "Sensor ready");

    hal_imu_data_t imu;
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_sensor_imu_read(&imu), "IMU read");
    TEST_ASSERT_EQUAL(1000, imu.accel_z_mg, "Default gravity 1000mg");
    TEST_ASSERT_EQUAL(1420, imu.step_count, "Step count match");

    hal_ppg_data_t ppg;
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_sensor_ppg_read(&ppg), "PPG read");
    TEST_ASSERT_EQUAL(72, ppg.heart_rate_bpm, "Heart rate 72 bpm");
    TEST_ASSERT_EQUAL(98, ppg.spo2_percent, "SpO2 98%");
    TEST_ASSERT(ppg.finger_detected, "Finger detected");

    hal_temp_data_t temp;
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_sensor_temp_read(&temp), "Temp read");
    TEST_ASSERT_EQUAL(3650, temp.skin_temp_centi_c, "Skin temp 36.5C");
}

static void test_hal_storage(void) {
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_storage_init(), "Storage init");
    TEST_ASSERT(hal_storage_is_ready(), "Storage ready");

    /* Partition not mounted - read/write should fail */
    uint8_t buf[64];
    size_t bytes_read = 0;
    TEST_ASSERT_EQUAL(NOVA_ERROR_INVALID_STATE, hal_storage_write_file(HAL_STORAGE_PARTITION_CONFIG, "config.bin", "data", 4), "Unmounted write fail");

    /* Mount partition */
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_storage_mount(HAL_STORAGE_PARTITION_CONFIG), "Mount config partition");

    /* Write file */
    const char *test_str = "NOVA_SETTINGS_V1";
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_storage_write_file(HAL_STORAGE_PARTITION_CONFIG, "config.bin", test_str, strlen(test_str)), "Write file");

    /* Read file back */
    memset(buf, 0, sizeof(buf));
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_storage_read_file(HAL_STORAGE_PARTITION_CONFIG, "config.bin", buf, sizeof(buf), &bytes_read), "Read file");
    TEST_ASSERT_EQUAL(strlen(test_str), bytes_read, "Bytes read match");
    TEST_ASSERT_EQUAL(0, memcmp(buf, test_str, strlen(test_str)), "Content match");

    /* Delete file */
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_storage_delete_file(HAL_STORAGE_PARTITION_CONFIG, "config.bin"), "Delete file");
    TEST_ASSERT_EQUAL(NOVA_ERROR_NOT_FOUND, hal_storage_read_file(HAL_STORAGE_PARTITION_CONFIG, "config.bin", buf, sizeof(buf), &bytes_read), "Deleted file not found");
}

static void test_hal_haptic(void) {
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_haptic_init(), "Haptic init");
    TEST_ASSERT(hal_haptic_is_ready(), "Haptic ready");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_haptic_play_effect(HAL_HAPTIC_EFFECT_CLICK), "Play click effect");
    TEST_ASSERT_EQUAL(HAL_HAPTIC_EFFECT_CLICK, hal_haptic_mock_get_last_effect(), "Last effect click");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_haptic_buzz(150, 90), "Buzz haptic");
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_haptic_stop(), "Stop haptic");
    TEST_ASSERT_EQUAL(HAL_HAPTIC_EFFECT_NONE, hal_haptic_mock_get_last_effect(), "Last effect cleared");
}

static void test_hal_ble(void) {
    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_ble_init(NULL, NULL, NULL), "BLE init");
    TEST_ASSERT(hal_ble_is_ready(), "BLE ready");
    TEST_ASSERT_EQUAL(HAL_BLE_STATE_IDLE, hal_ble_get_state(), "BLE initial state IDLE");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_ble_start_advertising("NOVA Watch"), "Start advertising");
    TEST_ASSERT_EQUAL(HAL_BLE_STATE_ADVERTISING, hal_ble_get_state(), "BLE state ADVERTISING");

    TEST_ASSERT_EQUAL(NOVA_SUCCESS, hal_ble_stop_advertising(), "Stop advertising");
    TEST_ASSERT_EQUAL(HAL_BLE_STATE_IDLE, hal_ble_get_state(), "BLE state IDLE");
}

void run_hal_tests(void) {
    RUN_TEST(test_hal_display);
    RUN_TEST(test_hal_touch);
    RUN_TEST(test_hal_sensor);
    RUN_TEST(test_hal_storage);
    RUN_TEST(test_hal_haptic);
    RUN_TEST(test_hal_ble);
}

