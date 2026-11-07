#include <stdio.h>
#include <stdbool.h>
#include "nova_config.h"
#include "nova_errors.h"
#include "modules/boot_manager.h"
#include "modules/diagnostics_manager.h"
#include "modules/power_manager.h"
#include "modules/hal_manager.h"

static void on_power_mode_change(nova_power_mode_t old_mode, nova_power_mode_t new_mode, void *user_data) {
    (void)user_data;
    printf("[POWER] Mode changed from %d to %d\n", old_mode, new_mode);
}

static void on_battery_alert(battery_alert_level_t level, uint8_t soc_pct, void *user_data) {
    (void)user_data;
    printf("[POWER ALERT] Level %d reached, Battery SoC = %u%%\n", level, soc_pct);
}

int main(void) {
    printf("========================================\n");
    printf("        NOVA WATCH OS - v%s         \n", NOVA_OS_VERSION_STRING);
    printf("   Target: %s (Dual-Core M33)  \n", NOVA_DEVICE_MODEL);
    printf("========================================\n");

    /* Step 1: Initialize Boot Manager */
    printf("[BOOT] Initializing Boot Manager...\n");
    nova_status_t status = boot_manager_init(RESET_REASON_POWER_ON);
    if (status != NOVA_SUCCESS && status != NOVA_ERROR_BOOT_SAFE_MODE) {
        printf("[BOOT ERROR] Failed to initialize boot manager: %s\n", nova_status_to_string(status));
        return 1;
    }

    /* Step 2: Initialize HAL Driver Subsystem */
    printf("[HAL] Initializing Hardware Abstraction Layer...\n");
    boot_manager_advance_stage(BOOT_STAGE_EARLY_HAL);
    status = hal_manager_init(HAL_DRIVER_MODE_MOCK);
    if (status != NOVA_SUCCESS) {
        printf("[HAL ERROR] Failed to initialize HAL manager: %s\n", nova_status_to_string(status));
        return 1;
    }

    /* Step 3: Initialize Diagnostics & Register Subsystems */
    printf("[DIAG] Initializing Diagnostics Manager...\n");
    boot_manager_advance_stage(BOOT_STAGE_DIAGNOSTIC);
    diagnostics_manager_init();

    status = hal_manager_init_all_drivers();
    if (status != NOVA_SUCCESS) {
        printf("[HAL ERROR] Failed to initialize all drivers: %s\n", nova_status_to_string(status));
        return 1;
    }

    /* Step 4: Run Built-In Self-Test (BIST) */
    printf("[BIST] Running Subsystem Built-In Self-Tests...\n");
    status = diagnostics_run_all_bist();
    if (status == NOVA_SUCCESS) {
        printf("[BIST SUCCESS] All core hardware subsystems passed self-test!\n");
    } else {
        printf("[BIST WARNING] Some subsystems degraded or failed: %s\n", nova_status_to_string(status));
    }

    /* Step 5: Initialize Power Manager */
    printf("[POWER] Initializing Power & Battery Manager...\n");
    boot_manager_advance_stage(BOOT_STAGE_SERVICES);
    status = power_manager_init(on_power_mode_change, on_battery_alert, NULL);
    if (status != NOVA_SUCCESS) {
        printf("[POWER ERROR] Failed to init power manager: %s\n", nova_status_to_string(status));
        return 1;
    }

    /* Step 6: Advance to Running State */
    boot_manager_record_successful_boot();
    printf("[BOOT SUCCESS] NOVA WATCH OS running in stage: %d\n", boot_manager_get_current_stage());

    /* Feed Watchdog */
    boot_manager_feed_watchdog();
    printf("[WATCHDOG] Heartbeat fed. System operational.\n");

    return 0;
}
