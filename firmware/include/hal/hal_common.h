#ifndef HAL_COMMON_H
#define HAL_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "nova_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t hal_time_ms_t;

typedef enum {
    HAL_DRIVER_MODE_PHYSICAL = 0,
    HAL_DRIVER_MODE_MOCK     = 1
} hal_driver_mode_t;

typedef void (*hal_callback_t)(void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* HAL_COMMON_H */

