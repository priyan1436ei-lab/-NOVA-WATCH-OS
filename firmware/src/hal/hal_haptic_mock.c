#include "hal/hal_haptic.h"

static const hal_haptic_driver_t *s_active_driver = NULL;
static bool s_mock_ready = false;
static hal_haptic_effect_t s_last_effect = HAL_HAPTIC_EFFECT_NONE;
static uint16_t s_last_buzz_duration_ms = 0;
static uint8_t  s_last_buzz_intensity_pct = 0;

static nova_status_t mock_haptic_init(void) {
    s_mock_ready = true;
    s_last_effect = HAL_HAPTIC_EFFECT_NONE;
    return NOVA_SUCCESS;
}

static nova_status_t mock_haptic_play_effect(hal_haptic_effect_t effect) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    s_last_effect = effect;
    return NOVA_SUCCESS;
}

static nova_status_t mock_haptic_buzz(uint16_t duration_ms, uint8_t intensity_pct) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    s_last_buzz_duration_ms = duration_ms;
    s_last_buzz_intensity_pct = intensity_pct;
    return NOVA_SUCCESS;
}

static nova_status_t mock_haptic_stop(void) {
    s_last_effect = HAL_HAPTIC_EFFECT_NONE;
    s_last_buzz_duration_ms = 0;
    return NOVA_SUCCESS;
}

static bool mock_haptic_is_ready(void) {
    return s_mock_ready;
}

static const hal_haptic_driver_t s_mock_driver = {
    .init = mock_haptic_init,
    .play_effect = mock_haptic_play_effect,
    .buzz = mock_haptic_buzz,
    .stop = mock_haptic_stop,
    .is_ready = mock_haptic_is_ready
};

const hal_haptic_driver_t* hal_haptic_get_mock_driver(void) {
    return &s_mock_driver;
}

void hal_haptic_register_driver(const hal_haptic_driver_t *driver) {
    s_active_driver = driver;
}

hal_haptic_effect_t hal_haptic_mock_get_last_effect(void) {
    return s_last_effect;
}

nova_status_t hal_haptic_init(void) {
    if (!s_active_driver) {
        s_active_driver = &s_mock_driver;
    }
    return s_active_driver->init();
}

nova_status_t hal_haptic_play_effect(hal_haptic_effect_t effect) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->play_effect(effect);
}

nova_status_t hal_haptic_buzz(uint16_t duration_ms, uint8_t intensity_pct) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->buzz(duration_ms, intensity_pct);
}

nova_status_t hal_haptic_stop(void) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->stop();
}

bool hal_haptic_is_ready(void) {
    if (!s_active_driver) return false;
    return s_active_driver->is_ready();
}

