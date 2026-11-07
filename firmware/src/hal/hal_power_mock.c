#include "hal/hal_power.h"
#include <string.h>

static const hal_power_driver_t *s_active_driver = NULL;
static hal_power_event_cb_t s_power_cb = NULL;
static void *s_power_user_data = NULL;
static bool s_mock_ready = false;
static hal_power_state_t s_mock_power_state = HAL_POWER_STATE_ACTIVE;

static hal_battery_status_t s_mock_battery = {
    .voltage_mv = 3950,
    .current_ma = -24,
    .state_of_charge_pct = 85,
    .charge_state = HAL_CHARGE_STATE_DISCHARGING,
    .battery_temp_centi_c = 2850 /* 28.5 °C */
};

static nova_status_t mock_power_init(hal_power_event_cb_t cb, void *user_data) {
    s_power_cb = cb;
    s_power_user_data = user_data;
    s_mock_ready = true;
    s_mock_power_state = HAL_POWER_STATE_ACTIVE;
    return NOVA_SUCCESS;
}

static nova_status_t mock_power_get_battery_status(hal_battery_status_t *status) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    if (!status) return NOVA_ERROR_INVALID_PARAM;
    *status = s_mock_battery;
    return NOVA_SUCCESS;
}

static nova_status_t mock_power_set_power_state(hal_power_state_t state) {
    s_mock_power_state = state;
    return NOVA_SUCCESS;
}

static nova_status_t mock_power_reboot(void) {
    return NOVA_SUCCESS;
}

static nova_status_t mock_power_shutdown(void) {
    s_mock_power_state = HAL_POWER_STATE_OFF;
    return NOVA_SUCCESS;
}

static bool mock_power_is_ready(void) {
    return s_mock_ready;
}

static const hal_power_driver_t s_mock_driver = {
    .init = mock_power_init,
    .get_battery_status = mock_power_get_battery_status,
    .set_power_state = mock_power_set_power_state,
    .reboot = mock_power_reboot,
    .shutdown = mock_power_shutdown,
    .is_ready = mock_power_is_ready
};

const hal_power_driver_t* hal_power_get_mock_driver(void) {
    return &s_mock_driver;
}

void hal_power_register_driver(const hal_power_driver_t *driver) {
    s_active_driver = driver;
}

void hal_power_mock_set_battery(uint16_t voltage_mv, uint8_t soc_pct, hal_charge_state_t state) {
    s_mock_battery.voltage_mv = voltage_mv;
    s_mock_battery.state_of_charge_pct = soc_pct;
    s_mock_battery.charge_state = state;
    if (s_power_cb) {
        s_power_cb(state, s_power_user_data);
    }
}

nova_status_t hal_power_init(hal_power_event_cb_t cb, void *user_data) {
    if (!s_active_driver) {
        s_active_driver = &s_mock_driver;
    }
    return s_active_driver->init(cb, user_data);
}

nova_status_t hal_power_get_battery_status(hal_battery_status_t *status) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->get_battery_status(status);
}

nova_status_t hal_power_set_power_state(hal_power_state_t state) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->set_power_state(state);
}

nova_status_t hal_power_reboot(void) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->reboot();
}

nova_status_t hal_power_shutdown(void) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->shutdown();
}

bool hal_power_is_ready(void) {
    if (!s_active_driver) return false;
    return s_active_driver->is_ready();
}

