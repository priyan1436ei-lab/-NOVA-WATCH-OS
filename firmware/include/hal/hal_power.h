#ifndef HAL_POWER_H
#define HAL_POWER_H

#include "hal/hal_common.h"
#include "nova_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_POWER_STATE_ACTIVE       = 0,
    HAL_POWER_STATE_BALANCED     = 1,
    HAL_POWER_STATE_SAVER        = 2,
    HAL_POWER_STATE_ULTRA_SAVER  = 3,
    HAL_POWER_STATE_SLEEP        = 4,
    HAL_POWER_STATE_OFF          = 5
} hal_power_state_t;

typedef enum {
    HAL_CHARGE_STATE_DISCHARGING = 0,
    HAL_CHARGE_STATE_CHARGING    = 1,
    HAL_CHARGE_STATE_FULL        = 2,
    HAL_CHARGE_STATE_FAULT       = 3
} hal_charge_state_t;

typedef struct {
    uint16_t voltage_mv;
    int16_t  current_ma;
    uint8_t  state_of_charge_pct;
    hal_charge_state_t charge_state;
    int16_t  battery_temp_centi_c;
} hal_battery_status_t;

typedef void (*hal_power_event_cb_t)(hal_charge_state_t state, void *user_data);

typedef struct {
    nova_status_t (*init)(hal_power_event_cb_t cb, void *user_data);
    nova_status_t (*get_battery_status)(hal_battery_status_t *status);
    nova_status_t (*set_power_state)(hal_power_state_t state);
    nova_status_t (*reboot)(void);
    nova_status_t (*shutdown)(void);
    bool          (*is_ready)(void);
} hal_power_driver_t;

nova_status_t hal_power_init(hal_power_event_cb_t cb, void *user_data);
nova_status_t hal_power_get_battery_status(hal_battery_status_t *status);
nova_status_t hal_power_set_power_state(hal_power_state_t state);
nova_status_t hal_power_reboot(void);
nova_status_t hal_power_shutdown(void);
bool          hal_power_is_ready(void);

void hal_power_register_driver(const hal_power_driver_t *driver);
const hal_power_driver_t* hal_power_get_mock_driver(void);
void hal_power_mock_set_battery(uint16_t voltage_mv, uint8_t soc_pct, hal_charge_state_t state);

#ifdef __cplusplus
}
#endif

#endif /* HAL_POWER_H */
