#include "hal/hal_ble.h"
#include <string.h>

static const hal_ble_driver_t *s_active_driver = NULL;
static hal_ble_state_cb_t s_state_cb = NULL;
static hal_ble_rx_cb_t s_rx_cb = NULL;
static void *s_ble_user_data = NULL;
static hal_ble_state_t s_mock_state = HAL_BLE_STATE_IDLE;
static bool s_mock_ready = false;

static nova_status_t mock_ble_init(hal_ble_state_cb_t state_cb, hal_ble_rx_cb_t rx_cb, void *user_data) {
    s_state_cb = state_cb;
    s_rx_cb = rx_cb;
    s_ble_user_data = user_data;
    s_mock_ready = true;
    s_mock_state = HAL_BLE_STATE_IDLE;
    return NOVA_SUCCESS;
}

static nova_status_t mock_ble_start_advertising(const char *device_name) {
    (void)device_name;
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    s_mock_state = HAL_BLE_STATE_ADVERTISING;
    if (s_state_cb) {
        s_state_cb(s_mock_state, s_ble_user_data);
    }
    return NOVA_SUCCESS;
}

static nova_status_t mock_ble_stop_advertising(void) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    s_mock_state = HAL_BLE_STATE_IDLE;
    if (s_state_cb) {
        s_state_cb(s_mock_state, s_ble_user_data);
    }
    return NOVA_SUCCESS;
}

static nova_status_t mock_ble_send_notification(uint16_t char_uuid, const uint8_t *data, size_t length) {
    (void)char_uuid;
    (void)data;
    (void)length;
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    if (s_mock_state != HAL_BLE_STATE_CONNECTED) return NOVA_ERROR_INVALID_STATE;
    return NOVA_SUCCESS;
}

static nova_status_t mock_ble_disconnect(void) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    s_mock_state = HAL_BLE_STATE_IDLE;
    if (s_state_cb) {
        s_state_cb(s_mock_state, s_ble_user_data);
    }
    return NOVA_SUCCESS;
}

static hal_ble_state_t mock_ble_get_state(void) {
    return s_mock_state;
}

static bool mock_ble_is_ready(void) {
    return s_mock_ready;
}

static const hal_ble_driver_t s_mock_driver = {
    .init = mock_ble_init,
    .start_advertising = mock_ble_start_advertising,
    .stop_advertising = mock_ble_stop_advertising,
    .send_notification = mock_ble_send_notification,
    .disconnect = mock_ble_disconnect,
    .get_state = mock_ble_get_state,
    .is_ready = mock_ble_is_ready
};

const hal_ble_driver_t* hal_ble_get_mock_driver(void) {
    return &s_mock_driver;
}

void hal_ble_register_driver(const hal_ble_driver_t *driver) {
    s_active_driver = driver;
}

nova_status_t hal_ble_init(hal_ble_state_cb_t state_cb, hal_ble_rx_cb_t rx_cb, void *user_data) {
    if (!s_active_driver) {
        s_active_driver = &s_mock_driver;
    }
    return s_active_driver->init(state_cb, rx_cb, user_data);
}

nova_status_t hal_ble_start_advertising(const char *device_name) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->start_advertising(device_name);
}

nova_status_t hal_ble_stop_advertising(void) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->stop_advertising();
}

nova_status_t hal_ble_send_notification(uint16_t char_uuid, const uint8_t *data, size_t length) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->send_notification(char_uuid, data, length);
}

nova_status_t hal_ble_disconnect(void) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->disconnect();
}

hal_ble_state_t hal_ble_get_state(void) {
    if (!s_active_driver) return HAL_BLE_STATE_IDLE;
    return s_active_driver->get_state();
}

bool hal_ble_is_ready(void) {
    if (!s_active_driver) return false;
    return s_active_driver->is_ready();
}
