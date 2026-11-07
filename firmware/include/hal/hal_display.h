#ifndef HAL_DISPLAY_H
#define HAL_DISPLAY_H

#include "hal/hal_common.h"
#include "nova_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
} hal_display_area_t;

typedef void (*hal_display_flush_cb_t)(void);

typedef struct {
    nova_status_t (*init)(void);
    nova_status_t (*set_power)(bool on);
    nova_status_t (*set_brightness)(uint8_t percentage);
    nova_status_t (*flush)(const hal_display_area_t *area, const uint16_t *color_p, hal_display_flush_cb_t cb);
    nova_status_t (*wait_tearing_effect)(uint32_t timeout_ms);
    bool          (*is_ready)(void);
} hal_display_driver_t;

nova_status_t hal_display_init(void);
nova_status_t hal_display_set_power(bool on);
nova_status_t hal_display_set_brightness(uint8_t percentage);
nova_status_t hal_display_flush(const hal_display_area_t *area, const uint16_t *color_p, hal_display_flush_cb_t cb);
nova_status_t hal_display_wait_tearing_effect(uint32_t timeout_ms);
bool          hal_display_is_ready(void);

void hal_display_register_driver(const hal_display_driver_t *driver);
const hal_display_driver_t* hal_display_get_mock_driver(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_DISPLAY_H */

