#ifndef HAL_SENSOR_H
#define HAL_SENSOR_H

#include "hal/hal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int16_t accel_x_mg;
    int16_t accel_y_mg;
    int16_t accel_z_mg;
    int16_t gyro_x_dps;
    int16_t gyro_y_dps;
    int16_t gyro_z_dps;
    uint32_t step_count;
    bool wrist_tilt_detected;
} hal_imu_data_t;

typedef struct {
    uint32_t ppg_green_raw;
    uint32_t ppg_red_raw;
    uint32_t ppg_ir_raw;
    uint8_t  heart_rate_bpm;
    uint8_t  spo2_percent;
    bool     finger_detected;
} hal_ppg_data_t;

typedef struct {
    int16_t skin_temp_centi_c;
} hal_temp_data_t;

typedef struct {
    nova_status_t (*init)(void);
    nova_status_t (*imu_read)(hal_imu_data_t *data);
    nova_status_t (*ppg_read)(hal_ppg_data_t *data);
    nova_status_t (*temp_read)(hal_temp_data_t *data);
    nova_status_t (*set_low_power)(bool enable);
    bool          (*is_ready)(void);
} hal_sensor_driver_t;

nova_status_t hal_sensor_init(void);
nova_status_t hal_sensor_imu_read(hal_imu_data_t *data);
nova_status_t hal_sensor_ppg_read(hal_ppg_data_t *data);
nova_status_t hal_sensor_temp_read(hal_temp_data_t *data);
nova_status_t hal_sensor_set_low_power(bool enable);
bool          hal_sensor_is_ready(void);

void hal_sensor_register_driver(const hal_sensor_driver_t *driver);
const hal_sensor_driver_t* hal_sensor_get_mock_driver(void);
void hal_sensor_mock_set_imu(const hal_imu_data_t *data);
void hal_sensor_mock_set_ppg(const hal_ppg_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* HAL_SENSOR_H */

