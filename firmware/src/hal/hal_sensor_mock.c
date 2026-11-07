#include "hal/hal_sensor.h"
#include <string.h>

static const hal_sensor_driver_t *s_active_driver = NULL;
static bool s_mock_ready = false;
static bool s_mock_low_power = false;

static hal_imu_data_t s_mock_imu = {
    .accel_x_mg = 0,
    .accel_y_mg = 0,
    .accel_z_mg = 1000, /* 1g downward */
    .gyro_x_dps = 0,
    .gyro_y_dps = 0,
    .gyro_z_dps = 0,
    .step_count = 1420,
    .wrist_tilt_detected = false
};

static hal_ppg_data_t s_mock_ppg = {
    .ppg_green_raw = 145000,
    .ppg_red_raw = 182000,
    .ppg_ir_raw = 191000,
    .heart_rate_bpm = 72,
    .spo2_percent = 98,
    .finger_detected = true
};

static hal_temp_data_t s_mock_temp = {
    .skin_temp_centi_c = 3650 /* 36.5 °C */
};

static nova_status_t mock_sensor_init(void) {
    s_mock_ready = true;
    s_mock_low_power = false;
    return NOVA_SUCCESS;
}

static nova_status_t mock_sensor_imu_read(hal_imu_data_t *data) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    if (!data) return NOVA_ERROR_INVALID_PARAM;
    *data = s_mock_imu;
    return NOVA_SUCCESS;
}

static nova_status_t mock_sensor_ppg_read(hal_ppg_data_t *data) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    if (!data) return NOVA_ERROR_INVALID_PARAM;
    *data = s_mock_ppg;
    return NOVA_SUCCESS;
}

static nova_status_t mock_sensor_temp_read(hal_temp_data_t *data) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    if (!data) return NOVA_ERROR_INVALID_PARAM;
    *data = s_mock_temp;
    return NOVA_SUCCESS;
}

static nova_status_t mock_sensor_set_low_power(bool enable) {
    s_mock_low_power = enable;
    return NOVA_SUCCESS;
}

static bool mock_sensor_is_ready(void) {
    return s_mock_ready;
}

static const hal_sensor_driver_t s_mock_driver = {
    .init = mock_sensor_init,
    .imu_read = mock_sensor_imu_read,
    .ppg_read = mock_sensor_ppg_read,
    .temp_read = mock_sensor_temp_read,
    .set_low_power = mock_sensor_set_low_power,
    .is_ready = mock_sensor_is_ready
};

const hal_sensor_driver_t* hal_sensor_get_mock_driver(void) {
    return &s_mock_driver;
}

void hal_sensor_register_driver(const hal_sensor_driver_t *driver) {
    s_active_driver = driver;
}

void hal_sensor_mock_set_imu(const hal_imu_data_t *data) {
    if (data) s_mock_imu = *data;
}

void hal_sensor_mock_set_ppg(const hal_ppg_data_t *data) {
    if (data) s_mock_ppg = *data;
}

nova_status_t hal_sensor_init(void) {
    if (!s_active_driver) {
        s_active_driver = &s_mock_driver;
    }
    return s_active_driver->init();
}

nova_status_t hal_sensor_imu_read(hal_imu_data_t *data) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->imu_read(data);
}

nova_status_t hal_sensor_ppg_read(hal_ppg_data_t *data) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->ppg_read(data);
}

nova_status_t hal_sensor_temp_read(hal_temp_data_t *data) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->temp_read(data);
}

nova_status_t hal_sensor_set_low_power(bool enable) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->set_low_power(enable);
}

bool hal_sensor_is_ready(void) {
    if (!s_active_driver) return false;
    return s_active_driver->is_ready();
}

