#include "hal/hal_display.h"
#include <string.h>

static const hal_display_driver_t *s_active_driver = NULL;

static bool s_mock_power = false;
static uint8_t s_mock_brightness = 70;
static bool s_mock_ready = false;
static uint32_t s_flush_count = 0;
static hal_display_area_t s_last_flush_area = {0};

static nova_status_t mock_display_init(void) {
    s_mock_power = true;
    s_mock_ready = true;
    s_flush_count = 0;
    return NOVA_SUCCESS;
}

static nova_status_t mock_display_set_power(bool on) {
    s_mock_power = on;
    return NOVA_SUCCESS;
}

static nova_status_t mock_display_set_brightness(uint8_t percentage) {
    if (percentage > 100) {
        return NOVA_ERROR_INVALID_PARAM;
    }
    s_mock_brightness = percentage;
    return NOVA_SUCCESS;
}

static nova_status_t mock_display_flush(const hal_display_area_t *area, const uint16_t *color_p, hal_display_flush_cb_t cb) {
    if (!s_mock_ready || !s_mock_power) {
        return NOVA_ERROR_INVALID_STATE;
    }
    if (!area || !color_p) {
        return NOVA_ERROR_INVALID_PARAM;
    }
    s_last_flush_area = *area;
    s_flush_count++;
    if (cb) {
        cb();
    }
    return NOVA_SUCCESS;
}

static nova_status_t mock_display_wait_tearing_effect(uint32_t timeout_ms) {
    (void)timeout_ms;
    return NOVA_SUCCESS;
}

static bool mock_display_is_ready(void) {
    return s_mock_ready && s_mock_power;
}

static const hal_display_driver_t s_mock_driver = {
    .init = mock_display_init,
    .set_power = mock_display_set_power,
    .set_brightness = mock_display_set_brightness,
    .flush = mock_display_flush,
    .wait_tearing_effect = mock_display_wait_tearing_effect,
    .is_ready = mock_display_is_ready
};

const hal_display_driver_t* hal_display_get_mock_driver(void) {
    return &s_mock_driver;
}

void hal_display_register_driver(const hal_display_driver_t *driver) {
    s_active_driver = driver;
}

nova_status_t hal_display_init(void) {
    if (!s_active_driver) {
        s_active_driver = &s_mock_driver;
    }
    return s_active_driver->init();
}

nova_status_t hal_display_set_power(bool on) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->set_power(on);
}

nova_status_t hal_display_set_brightness(uint8_t percentage) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->set_brightness(percentage);
}

nova_status_t hal_display_flush(const hal_display_area_t *area, const uint16_t *color_p, hal_display_flush_cb_t cb) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->flush(area, color_p, cb);
}

nova_status_t hal_display_wait_tearing_effect(uint32_t timeout_ms) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->wait_tearing_effect(timeout_ms);
}

bool hal_display_is_ready(void) {
    if (!s_active_driver) return false;
    return s_active_driver->is_ready();
}
