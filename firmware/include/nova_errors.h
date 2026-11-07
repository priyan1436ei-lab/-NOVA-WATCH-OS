#ifndef NOVA_ERRORS_H
#define NOVA_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NOVA_SUCCESS                    = 0,
    NOVA_ERROR_UNKNOWN             = -1,
    NOVA_ERROR_INVALID_PARAM       = -2,
    NOVA_ERROR_INVALID_STATE       = -3,
    NOVA_ERROR_NOT_INITIALIZED     = -4,
    NOVA_ERROR_ALREADY_INITIALIZED = -5,
    NOVA_ERROR_NO_MEMORY           = -6,
    NOVA_ERROR_TIMEOUT             = -7,
    NOVA_ERROR_BUSY                = -8,
    NOVA_ERROR_NOT_SUPPORTED       = -9,
    NOVA_ERROR_NOT_FOUND           = -10,
    NOVA_ERROR_BUFFER_OVERFLOW     = -11,
    NOVA_ERROR_IO                  = -12,
    NOVA_ERROR_CRC_MISMATCH        = -13,
    NOVA_ERROR_SIGNATURE_INVALID   = -14,
    NOVA_ERROR_PERMISSION_DENIED   = -15,

    /* Hardware Subsystem Errors (-100 .. -199) */
    NOVA_ERROR_HAL_DISPLAY_INIT    = -100,
    NOVA_ERROR_HAL_DISPLAY_TIMEOUT = -101,
    NOVA_ERROR_HAL_TOUCH_INIT      = -102,
    NOVA_ERROR_HAL_TOUCH_COMM      = -103,
    NOVA_ERROR_HAL_SENSOR_INIT     = -104,
    NOVA_ERROR_HAL_SENSOR_COMM     = -105,
    NOVA_ERROR_HAL_POWER_INIT      = -106,
    NOVA_ERROR_HAL_STORAGE_INIT    = -107,
    NOVA_ERROR_HAL_STORAGE_CORRUPT = -108,
    NOVA_ERROR_HAL_AUDIO_INIT      = -109,
    NOVA_ERROR_HAL_HAPTIC_INIT     = -110,
    NOVA_ERROR_HAL_BLE_INIT        = -111,

    /* System Module Errors (-200 .. -299) */
    NOVA_ERROR_BOOT_CRASH_LOOP     = -200,
    NOVA_ERROR_BOOT_SAFE_MODE      = -201,
    NOVA_ERROR_DIAG_SELFTEST_FAIL  = -202,
    NOVA_ERROR_POWER_CRITICAL_BAT  = -203,
    NOVA_ERROR_SECURITY_AUTH_FAIL  = -204,
    NOVA_ERROR_OTA_VERIFY_FAIL     = -205,
    NOVA_ERROR_OTA_SLOT_INVALID    = -206
} nova_status_t;

static inline const char* nova_status_to_string(nova_status_t status) {
    switch (status) {
        case NOVA_SUCCESS:                    return "NOVA_SUCCESS";
        case NOVA_ERROR_UNKNOWN:             return "NOVA_ERROR_UNKNOWN";
        case NOVA_ERROR_INVALID_PARAM:       return "NOVA_ERROR_INVALID_PARAM";
        case NOVA_ERROR_INVALID_STATE:       return "NOVA_ERROR_INVALID_STATE";
        case NOVA_ERROR_NOT_INITIALIZED:     return "NOVA_ERROR_NOT_INITIALIZED";
        case NOVA_ERROR_ALREADY_INITIALIZED: return "NOVA_ERROR_ALREADY_INITIALIZED";
        case NOVA_ERROR_NO_MEMORY:           return "NOVA_ERROR_NO_MEMORY";
        case NOVA_ERROR_TIMEOUT:             return "NOVA_ERROR_TIMEOUT";
        case NOVA_ERROR_BUSY:                return "NOVA_ERROR_BUSY";
        case NOVA_ERROR_NOT_SUPPORTED:       return "NOVA_ERROR_NOT_SUPPORTED";
        case NOVA_ERROR_NOT_FOUND:           return "NOVA_ERROR_NOT_FOUND";
        case NOVA_ERROR_BUFFER_OVERFLOW:     return "NOVA_ERROR_BUFFER_OVERFLOW";
        case NOVA_ERROR_IO:                  return "NOVA_ERROR_IO";
        case NOVA_ERROR_CRC_MISMATCH:        return "NOVA_ERROR_CRC_MISMATCH";
        case NOVA_ERROR_SIGNATURE_INVALID:   return "NOVA_ERROR_SIGNATURE_INVALID";
        case NOVA_ERROR_PERMISSION_DENIED:   return "NOVA_ERROR_PERMISSION_DENIED";
        case NOVA_ERROR_HAL_DISPLAY_INIT:    return "NOVA_ERROR_HAL_DISPLAY_INIT";
        case NOVA_ERROR_HAL_TOUCH_INIT:      return "NOVA_ERROR_HAL_TOUCH_INIT";
        case NOVA_ERROR_HAL_SENSOR_INIT:     return "NOVA_ERROR_HAL_SENSOR_INIT";
        case NOVA_ERROR_HAL_POWER_INIT:      return "NOVA_ERROR_HAL_POWER_INIT";
        case NOVA_ERROR_HAL_STORAGE_INIT:    return "NOVA_ERROR_HAL_STORAGE_INIT";
        case NOVA_ERROR_HAL_AUDIO_INIT:      return "NOVA_ERROR_HAL_AUDIO_INIT";
        case NOVA_ERROR_HAL_HAPTIC_INIT:     return "NOVA_ERROR_HAL_HAPTIC_INIT";
        case NOVA_ERROR_HAL_BLE_INIT:        return "NOVA_ERROR_HAL_BLE_INIT";
        case NOVA_ERROR_BOOT_CRASH_LOOP:     return "NOVA_ERROR_BOOT_CRASH_LOOP";
        case NOVA_ERROR_BOOT_SAFE_MODE:      return "NOVA_ERROR_BOOT_SAFE_MODE";
        case NOVA_ERROR_DIAG_SELFTEST_FAIL:  return "NOVA_ERROR_DIAG_SELFTEST_FAIL";
        case NOVA_ERROR_POWER_CRITICAL_BAT:  return "NOVA_ERROR_POWER_CRITICAL_BAT";
        case NOVA_ERROR_SECURITY_AUTH_FAIL:  return "NOVA_ERROR_SECURITY_AUTH_FAIL";
        case NOVA_ERROR_OTA_VERIFY_FAIL:     return "NOVA_ERROR_OTA_VERIFY_FAIL";
        default:                             return "NOVA_ERROR_CODE_UNKNOWN";
    }
}

#ifdef __cplusplus
}
#endif

#endif /* NOVA_ERRORS_H */
