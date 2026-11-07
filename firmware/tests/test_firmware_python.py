"""
NOVA WATCH OS - Firmware Logic & Subsystem Verification Suite
Tests state machines, voltage-to-SoC curves, memory budgeting, and error codes.
"""

import pytest

class BootStage:
    PRE_INIT = 0
    EARLY_HAL = 1
    STORAGE = 2
    DIAGNOSTIC = 3
    SERVICES = 4
    UI = 5
    RUNNING = 6
    SAFE_MODE = 7

class ResetReason:
    UNKNOWN = 0
    POWER_ON = 1
    PIN_RESET = 2
    WATCHDOG = 3
    SOFTWARE = 4
    BROWNOUT = 5
    CRASH_REBOOT = 6

class PowerMode:
    ACTIVE = 0
    BALANCED = 1
    BATTERY_SAVER = 2
    ULTRA_SAVER = 3
    SLEEP = 4

class BatteryAlertLevel:
    NONE = 0
    LOW = 1
    CRITICAL = 2
    SHUTDOWN = 3

SOC_CURVE = [
    (4200, 100),
    (4050, 90),
    (3920, 80),
    (3830, 70),
    (3770, 60),
    (3720, 50),
    (3670, 40),
    (3620, 30),
    (3550, 20),
    (3450, 10),
    (3350, 5),
    (3200, 2),
    (3000, 0)
]

def voltage_to_soc(voltage_mv: int) -> int:
    if voltage_mv >= SOC_CURVE[0][0]:
        return 100
    if voltage_mv <= SOC_CURVE[-1][0]:
        return 0
    for i in range(len(SOC_CURVE) - 1):
        v_high, s_high = SOC_CURVE[i]
        v_low, s_low = SOC_CURVE[i + 1]
        if v_low <= voltage_mv <= v_high:
            delta_v = v_high - v_low
            offset_v = voltage_mv - v_low
            delta_s = s_high - s_low
            return int(s_low + (offset_v * delta_s) / delta_v)
    return 0

def evaluate_battery_alert(soc_pct: int, voltage_mv: int) -> int:
    if soc_pct <= 3 or voltage_mv <= 3100:
        return BatteryAlertLevel.SHUTDOWN
    if soc_pct <= 10 or voltage_mv <= 3300:
        return BatteryAlertLevel.CRITICAL
    if soc_pct <= 20 or voltage_mv <= 3450:
        return BatteryAlertLevel.LOW
    return BatteryAlertLevel.NONE

class BootManagerMock:
    def __init__(self):
        self.stage = BootStage.PRE_INIT
        self.reset_reason = ResetReason.UNKNOWN
        self.boot_count = 0
        self.crash_count = 0
        self.safe_mode = False

    def init(self, reason: int):
        self.reset_reason = reason
        self.boot_count += 1
        self.stage = BootStage.PRE_INIT
        if reason in (ResetReason.WATCHDOG, ResetReason.CRASH_REBOOT):
            self.crash_count += 1
        if self.crash_count >= 3:
            self.safe_mode = True
            self.stage = BootStage.SAFE_MODE
            return -201 # NOVA_ERROR_BOOT_SAFE_MODE
        self.safe_mode = False
        return 0

    def advance_stage(self, next_stage: int):
        if self.safe_mode:
            return -3 # NOVA_ERROR_INVALID_STATE
        if next_stage < self.stage and next_stage != BootStage.SAFE_MODE:
            return -2 # NOVA_ERROR_INVALID_PARAM
        self.stage = next_stage
        return 0

    def record_successful_boot(self):
        self.crash_count = 0
        self.safe_mode = False
        self.stage = BootStage.RUNNING
        return 0

def test_voltage_to_soc_curve():
    assert voltage_to_soc(4250) == 100
    assert voltage_to_soc(4200) == 100
    assert voltage_to_soc(4050) == 90
    assert voltage_to_soc(3920) == 80
    assert voltage_to_soc(3720) == 50
    assert voltage_to_soc(3550) == 20
    assert voltage_to_soc(3450) == 10
    assert voltage_to_soc(3000) == 0
    assert voltage_to_soc(2800) == 0

def test_battery_alerts():
    assert evaluate_battery_alert(85, 3950) == BatteryAlertLevel.NONE
    assert evaluate_battery_alert(18, 3520) == BatteryAlertLevel.LOW
    assert evaluate_battery_alert(8, 3400) == BatteryAlertLevel.CRITICAL
    assert evaluate_battery_alert(2, 3080) == BatteryAlertLevel.SHUTDOWN

def test_boot_manager_normal_cycle():
    mgr = BootManagerMock()
    assert mgr.init(ResetReason.POWER_ON) == 0
    assert mgr.stage == BootStage.PRE_INIT
    assert not mgr.safe_mode

    assert mgr.advance_stage(BootStage.EARLY_HAL) == 0
    assert mgr.advance_stage(BootStage.STORAGE) == 0
    assert mgr.advance_stage(BootStage.DIAGNOSTIC) == 0
    assert mgr.advance_stage(BootStage.SERVICES) == 0
    assert mgr.advance_stage(BootStage.UI) == 0
    assert mgr.record_successful_boot() == 0
    assert mgr.stage == BootStage.RUNNING
    assert mgr.crash_count == 0

def test_boot_manager_crash_loop_recovery():
    mgr = BootManagerMock()
    # Crash 1
    assert mgr.init(ResetReason.WATCHDOG) == 0
    assert not mgr.safe_mode
    # Crash 2
    assert mgr.init(ResetReason.WATCHDOG) == 0
    assert not mgr.safe_mode
    # Crash 3 -> Triggers Safe Mode
    assert mgr.init(ResetReason.WATCHDOG) == -201
    assert mgr.safe_mode is True
    assert mgr.stage == BootStage.SAFE_MODE

    # Attempting to load UI in safe mode fails
    assert mgr.advance_stage(BootStage.UI) == -3

    # Recovery / user clearing
    assert mgr.record_successful_boot() == 0
    assert mgr.crash_count == 0

def test_hardware_budget_constraints():
    # Application Core limits
    APP_FLASH_KB = 1024
    APP_RAM_KB = 512

    MCUBOOT_FLASH_KB = 64
    IMAGE_0_FLASH_KB = 432
    IMAGE_1_FLASH_KB = 512
    NVS_FLASH_KB = 16
    assert (MCUBOOT_FLASH_KB + IMAGE_0_FLASH_KB + IMAGE_1_FLASH_KB + NVS_FLASH_KB) <= APP_FLASH_KB

    KERNEL_RAM_KB = 32
    MODULE_STACKS_RAM_KB = 96
    LVGL_RENDER_RAM_KB = 144
    HEAP_RAM_KB = 112
    DSP_BUFFERS_RAM_KB = 112
    SHARED_IPC_RAM_KB = 16
    assert (KERNEL_RAM_KB + MODULE_STACKS_RAM_KB + LVGL_RENDER_RAM_KB + HEAP_RAM_KB + DSP_BUFFERS_RAM_KB + SHARED_IPC_RAM_KB) <= APP_RAM_KB
