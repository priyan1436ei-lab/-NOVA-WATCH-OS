#ifndef HAL_TOUCH_H
#define HAL_TOUCH_H

#include "hal/hal_common.h"
#include "nova_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_TOUCH_EVENT_NONE      = 0,
    HAL_TOUCH_EVENT_DOWN      = 1,
    HAL_TOUCH_EVENT_UP        = 2,
    HAL_TOUCH_EVENT_CONTACT   = 3
} hal_touch_event_t;

typedef enum {
    HAL_GESTURE_NONE          = 0,
    HAL_GESTURE_TAP           = 1,
    HAL_GESTURE_DOUBLE_TAP    = 2,
    HAL_GESTURE_LONG_PRESS    = 3,
    HAL_GESTURE_SWIPE_UP      = 4,
    HAL_GESTURE_SWIPE_DOWN    = 5,
    HAL_GESTURE_SWIPE_LEFT    = 6,
    HAL_GESTURE_SWIPE_RIGHT   = 7
} hal_gesture_type_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t  pressure;
    hal_touch_event_t event;
} hal_touch_point_t;

typedef struct {
    uint8_t point_count;
    hal_touch_point_t points[NOVA_TOUCH_MAX_POINTS];
    hal_gesture_type_t gesture;
    uint32_t timestamp_ms;
} hal_touch_data_t;

typedef void (*hal_touch_irq_cb_t)(const hal_touch_data_t *data, void *user_data);

typedef struct {
    nova_status_t (*init)(hal_touch_irq_cb_t cb, void *user_data);
    nova_status_t (*read)(hal_touch_data_t *out_data);
    nova_status_t (*set_sleep)(bool sleep);
    bool          (*is_ready)(void);
} hal_touch_driver_t;

nova_status_t hal_touch_init(hal_touch_irq_cb_t cb, void *user_data);
nova_status_t hal_touch_read(hal_touch_data_t *out_data);
nova_status_t hal_touch_set_sleep(bool sleep);
bool          hal_touch_is_ready(void);

void hal_touch_register_driver(const hal_touch_driver_t *driver);
const hal_touch_driver_t* hal_touch_get_mock_driver(void);
void hal_touch_mock_inject(const hal_touch_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* HAL_TOUCH_H */

