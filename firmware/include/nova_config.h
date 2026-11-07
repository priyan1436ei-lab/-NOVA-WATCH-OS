#ifndef NOVA_CONFIG_H
#define NOVA_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* System Firmware Metadata */
#define NOVA_OS_VERSION_MAJOR       1
#define NOVA_OS_VERSION_MINOR       0
#define NOVA_OS_VERSION_PATCH       0
#define NOVA_OS_VERSION_BUILD       101
#define NOVA_OS_VERSION_STRING      "NOVA OS 1.0.0"
#define NOVA_DEVICE_MODEL           "NOVA-WATCH-REV-A"

/* Display Hardware Parameters */
#define NOVA_DISPLAY_WIDTH          360
#define NOVA_DISPLAY_HEIGHT         360
#define NOVA_DISPLAY_IS_ROUND       1
#define NOVA_DISPLAY_COLOR_DEPTH    16      /* RGB565 */
#define NOVA_DISPLAY_FRAME_RATE     60      /* Target FPS */
#define NOVA_DISPLAY_BRIGHTNESS_MAX 100
#define NOVA_DISPLAY_BRIGHTNESS_DEF 70

/* Touch & Gesture Parameters */
#define NOVA_TOUCH_MAX_POINTS       2
#define NOVA_TOUCH_DEBOUNCE_MS      15
#define NOVA_SWIPE_THRESHOLD_PX     40
#define NOVA_LONG_PRESS_MS          600

/* Memory & Buffer Sizes */
#define NOVA_LVGL_BUFFER_LINES      40
#define NOVA_LVGL_BUFFER_BYTES      (NOVA_DISPLAY_WIDTH * NOVA_LVGL_BUFFER_LINES * (NOVA_DISPLAY_COLOR_DEPTH / 8))
#define NOVA_HEAP_SIZE_BYTES        (112 * 1024)
#define NOVA_SENSOR_FIFO_DEPTH      64
#define NOVA_AUDIO_SAMPLE_RATE_HZ   16000
#define NOVA_AUDIO_RING_BUFFER_BYTES 8192

/* Power & Battery Thresholds */
#define NOVA_BATTERY_CAPACITY_MAH   350
#define NOVA_BATTERY_VOLT_FULL_MV   4200
#define NOVA_BATTERY_VOLT_NOM_MV    3700
#define NOVA_BATTERY_VOLT_WARN_MV   3450    /* ~15% */
#define NOVA_BATTERY_VOLT_CRIT_MV   3300    /* ~5% */
#define NOVA_BATTERY_VOLT_CUTOFF_MV 3100    /* Forced shutdown */

#define NOVA_BATTERY_SOC_WARN       20
#define NOVA_BATTERY_SOC_CRIT       10
#define NOVA_BATTERY_SOC_SHUTDOWN   3

/* Storage & Partitions */
#define NOVA_FLASH_TOTAL_SIZE_MB    16
#define NOVA_LFS_BLOCK_SIZE         4096
#define NOVA_LFS_LOOKAHEAD_SIZE     256
#define NOVA_WATCH_FACES_MAX        16

/* Watchdog & Safety */
#define NOVA_WATCHDOG_TIMEOUT_MS    8000
#define NOVA_CRASH_LOOP_THRESHOLD   3
#define NOVA_BOOT_COUNT_MAGIC       0x4E4F5641 /* "NOVA" */

#ifdef __cplusplus
}
#endif

#endif /* NOVA_CONFIG_H */
