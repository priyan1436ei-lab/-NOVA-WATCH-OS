#ifndef HAL_HAPTIC_H
#define HAL_HAPTIC_H

#include "hal/hal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_HAPTIC_EFFECT_NONE       = 0,
    HAL_HAPTIC_EFFECT_CLICK      = 1,
    HAL_HAPTIC_EFFECT_DOUBLE_CLICK = 2,
    HAL_HAPTIC_EFFECT_TICK       = 3,
    HAL_HAPTIC_EFFECT_ALERT      = 4,
    HAL_HAPTIC_EFFECT_ALARM      = 5,
    HAL_HAPTIC_EFFECT_SUCCESS    = 6,
    HAL_HAPTIC_EFFECT_ERROR      = 7
} hal_haptic_effect_t;

typedef struct {
    nova_status_t (*init)(void);
    nova_status_t (*play_effect)(hal_haptic_effect_t effect);
    nova_status_t (*buzz)(uint16_t duration_ms, uint8_t intensity_pct);
    nova_status_t (*stop)(void);
    bool          (*is_ready)(void);
} hal_haptic_driver_t;

nova_status_t hal_haptic_init(void);
nova_status_t hal_haptic_play_effect(hal_haptic_effect_t effect);
nova_status_t hal_haptic_buzz(uint16_t duration_ms, uint8_t intensity_pct);
nova_status_t hal_haptic_stop(void);
bool          hal_haptic_is_ready(void);

void hal_haptic_register_driver(const hal_haptic_driver_t *driver);
const hal_haptic_driver_t* hal_haptic_get_mock_driver(void);
hal_haptic_effect_t hal_haptic_mock_get_last_effect(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HAPTIC_H */
