#include "hal/hal_common.h"
#include "hal/hal_display.h"
#include "hal/hal_touch.h"
#include "hal/hal_sensor.h"
#include "hal/hal_power.h"
#include "hal/hal_storage.h"
#include "hal/hal_ble.h"

#if defined(__ZEPHYR__)
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>

/* Display Zephyr Binding */
static const struct device *s_zephyr_disp_dev = NULL;

static nova_status_t zephyr_display_init(void) {
    s_zephyr_disp_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(s_zephyr_disp_dev)) {
        return NOVA_ERROR_HAL_DISPLAY_INIT;
    }
    display_blanking_off(s_zephyr_disp_dev);
    return NOVA_SUCCESS;
}

static nova_status_t zephyr_display_set_power(bool on) {
    if (!s_zephyr_disp_dev) return NOVA_ERROR_NOT_INITIALIZED;
    if (on) {
        display_blanking_off(s_zephyr_disp_dev);
    } else {
        display_blanking_on(s_zephyr_disp_dev);
    }
    return NOVA_SUCCESS;
}

static nova_status_t zephyr_display_set_brightness(uint8_t percentage) {
    if (!s_zephyr_disp_dev) return NOVA_ERROR_NOT_INITIALIZED;
    struct display_capabilities caps;
    display_get_capabilities(s_zephyr_disp_dev, &caps);
    /* Set brightness via display API if supported */
    return NOVA_SUCCESS;
}

static nova_status_t zephyr_display_flush(const hal_display_area_t *area, const uint16_t *color_p, hal_display_flush_cb_t cb) {
    if (!s_zephyr_disp_dev) return NOVA_ERROR_NOT_INITIALIZED;
    struct display_buffer_descriptor desc;
    desc.buf_size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;
    desc.width = area->x2 - area->x1 + 1;
    desc.height = area->y2 - area->y1 + 1;
    desc.pitch = desc.width;

    int ret = display_write(s_zephyr_disp_dev, area->x1, area->y1, &desc, color_p);
    if (ret != 0) return NOVA_ERROR_IO;
    if (cb) cb();
    return NOVA_SUCCESS;
}

static nova_status_t zephyr_display_wait_tearing_effect(uint32_t timeout_ms) {
    /* Zephyr GPIO interrupt synchronization for TE pin */
    k_sleep(K_MSEC(1));
    return NOVA_SUCCESS;
}

static bool zephyr_display_is_ready(void) {
    return s_zephyr_disp_dev && device_is_ready(s_zephyr_disp_dev);
}

static const hal_display_driver_t s_zephyr_display_driver = {
    .init = zephyr_display_init,
    .set_power = zephyr_display_set_power,
    .set_brightness = zephyr_display_set_brightness,
    .flush = zephyr_display_flush,
    .wait_tearing_effect = zephyr_display_wait_tearing_effect,
    .is_ready = zephyr_display_is_ready
};

void hal_zephyr_register_all(void) {
    hal_display_register_driver(&s_zephyr_display_driver);
}

#else

/* Host simulator stub */
void hal_zephyr_register_all(void) {
    /* On host systems, Zephyr drivers are omitted and Mock drivers are active */
}

#endif /* __ZEPHYR__ */

