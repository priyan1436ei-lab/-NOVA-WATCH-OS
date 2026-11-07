#ifndef HAL_BLE_H
#define HAL_BLE_H

#include "hal/hal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_BLE_STATE_IDLE         = 0,
    HAL_BLE_STATE_ADVERTISING  = 1,
    HAL_BLE_STATE_CONNECTED    = 2,
    HAL_BLE_STATE_DISCONNECTING= 3
} hal_ble_state_t;

typedef void (*hal_ble_state_cb_t)(hal_ble_state_t new_state, void *user_data);
typedef void (*hal_ble_rx_cb_t)(uint16_t char_uuid, const uint8_t *data, size_t length, void *user_data);

typedef struct {
    nova_status_t (*init)(hal_ble_state_cb_t state_cb, hal_ble_rx_cb_t rx_cb, void *user_data);
    nova_status_t (*start_advertising)(const char *device_name);
    nova_status_t (*stop_advertising)(void);
    nova_status_t (*send_notification)(uint16_t char_uuid, const uint8_t *data, size_t length);
    nova_status_t (*disconnect)(void);
    hal_ble_state_t (*get_state)(void);
    bool          (*is_ready)(void);
} hal_ble_driver_t;

nova_status_t hal_ble_init(hal_ble_state_cb_t state_cb, hal_ble_rx_cb_t rx_cb, void *user_data);
nova_status_t hal_ble_start_advertising(const char *device_name);
nova_status_t hal_ble_stop_advertising(void);
nova_status_t hal_ble_send_notification(uint16_t char_uuid, const uint8_t *data, size_t length);
nova_status_t hal_ble_disconnect(void);
hal_ble_state_t hal_ble_get_state(void);
bool          hal_ble_is_ready(void);

void hal_ble_register_driver(const hal_ble_driver_t *driver);
const hal_ble_driver_t* hal_ble_get_mock_driver(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_BLE_H */

