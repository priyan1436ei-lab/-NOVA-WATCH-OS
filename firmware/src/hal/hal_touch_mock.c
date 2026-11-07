#include "hal/hal_touch.h"
#include <string.h>

static const hal_touch_driver_t *s_active_driver = NULL;
static hal_touch_irq_cb_t s_touch_cb = NULL;
static void *s_touch_user_data = NULL;
static bool s_mock_ready = false;
static bool s_mock_sleep = false;
static hal_touch_data_t s_mock_current_data = {0};

static nova_status_t mock_touch_init(hal_touch_irq_cb_t cb, void *user_data) {
    s_touch_cb = cb;
    s_touch_user_data = user_data;
    s_mock_ready = true;
    s_mock_sleep = false;
    memset(&s_mock_current_data, 0, sizeof(s_mock_current_data));
    return NOVA_SUCCESS;
}

static nova_status_t mock_touch_read(hal_touch_data_t *out_data) {
    if (!s_mock_ready) return NOVA_ERROR_NOT_INITIALIZED;
    if (!out_data) return NOVA_ERROR_INVALID_PARAM;
    *out_data = s_mock_current_data;
    return NOVA_SUCCESS;
}

static nova_status_t mock_touch_set_sleep(bool sleep) {
    s_mock_sleep = sleep;
    return NOVA_SUCCESS;
}

static bool mock_touch_is_ready(void) {
    return s_mock_ready && !s_mock_sleep;
}

static const hal_touch_driver_t s_mock_driver = {
    .init = mock_touch_init,
    .read = mock_touch_read,
    .set_sleep = mock_touch_set_sleep,
    .is_ready = mock_touch_is_ready
};

const hal_touch_driver_t* hal_touch_get_mock_driver(void) {
    return &s_mock_driver;
}

void hal_touch_register_driver(const hal_touch_driver_t *driver) {
    s_active_driver = driver;
}

void hal_touch_mock_inject(const hal_touch_data_t *data) {
    if (data) {
        s_mock_current_data = *data;
        if (s_touch_cb) {
            s_touch_cb(&s_mock_current_data, s_touch_user_data);
        }
    }
}

nova_status_t hal_touch_init(hal_touch_irq_cb_t cb, void *user_data) {
    if (!s_active_driver) {
        s_active_driver = &s_mock_driver;
    }
    return s_active_driver->init(cb, user_data);
}

nova_status_t hal_touch_read(hal_touch_data_t *out_data) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->read(out_data);
}

nova_status_t hal_touch_set_sleep(bool sleep) {
    if (!s_active_driver) return NOVA_ERROR_NOT_INITIALIZED;
    return s_active_driver->set_sleep(sleep);
}

bool hal_touch_is_ready(void) {
    if (!s_active_driver) return false;
    return s_active_driver->is_ready();
}

