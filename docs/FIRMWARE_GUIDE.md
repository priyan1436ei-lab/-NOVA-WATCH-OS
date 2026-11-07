# NOVA WATCH OS - Firmware Engineering Guide

**Version:** 1.0.0  
**Target:** Nordic nRF5340 (Application Core + Network Core)  
**RTOS:** Zephyr RTOS v3.7.0 / nRF Connect SDK v2.7.0  

---

## 1. Modular Subsystem Architecture

NOVA WATCH OS enforces strict decoupling between hardware drivers, kernel management tasks, and UI rendering. The 20 discrete modules are:

```
[ Application Layer: Watch Faces, Widgets, 16 Built-in Apps ]
                          |
[ Core OS Services: App Mgr, Notification Mgr, Health Engine, AI Client ]
                          |
[ Platform Services: Boot Mgr, Power Mgr, Diagnostics Mgr, Storage Mgr ]
                          |
[ Hardware Abstraction Layer (HAL): Display, Touch, Sensor, Audio, etc. ]
                          |
[ Target Physical Drivers (Zephyr)  |  Host Simulation Mocks ]
```

---

## 2. Boot Manager & State Sequencing

The `boot_manager` orchestrates the deterministic progression through seven distinct boot stages:

1. **`BOOT_STAGE_PRE_INIT`**: Low-level crystal oscillator configuration, core register initialization.
2. **`BOOT_STAGE_EARLY_HAL`**: Basic GPIO, clock rails, PMIC power rail verification.
3. **`BOOT_STAGE_STORAGE`**: Mount LittleFS partitions on external QSPI NOR Flash.
4. **`BOOT_STAGE_DIAGNOSTIC`**: Execute subsystem Built-In Self-Tests (BIST).
5. **`BOOT_STAGE_SERVICES`**: Launch Power Manager, BLE stack, and Sensor acquisition workers.
6. **`BOOT_STAGE_UI`**: Initialize LVGL graphics pipeline and load default Watch Face.
7. **`BOOT_STAGE_RUNNING`**: Normal operational loop; feeds hardware watchdog every 2000 ms.

### Crash Loop Mitigation & Safe Mode
If three consecutive boots fail or trigger a Watchdog Reset (`RESET_REASON_WATCHDOG`) without advancing to `BOOT_STAGE_RUNNING`, the Boot Manager locks the system into **`BOOT_STAGE_SAFE_MODE`**. In this mode:
- All custom watch faces and widgets are bypassed.
- Heavy background sensor tasks and Bluetooth radios remain disabled.
- A minimal fallback UI is rendered providing USB/BLE diagnostics and factory recovery options.

---

## 3. Hardware Abstraction Layer (HAL)

Each hardware peripheral is abstracted through a C interface table containing pointers to hardware functions and mock functions:

* `hal_display.h`: Framebuffer flush, backlight brightness (0-100%), power toggle, TE sync.
* `hal_touch.h`: Interrupt-driven coordinate stream, multi-touch debouncing, gestures.
* `hal_sensor.h`: 6-axis IMU (accel/gyro), step count, optical PPG (green/red/IR), skin temperature.
* `hal_power.h`: Battery voltage, charging state, PMIC control, software reboot/shutdown.
* `hal_storage.h`: LittleFS file read/write/delete across 4 partitions.
* `hal_haptic.h`: ROM waveform effect triggers and frequency-timed pulses.
* `hal_audio.h`: PDM digital mic capture ring buffer, I2S DAC speaker output.
* `hal_ble.h`: Advertising control, GATT event notifications, and state change listeners.

---

## 4. Diagnostics Manager & Built-In Self-Tests (BIST)

The Diagnostics Manager maintains a health registry for all 10 hardware subsystems (`SUBSYSTEM_HAL_DISPLAY` through `SUBSYSTEM_SECURITY`). At boot and during periodic maintenance cycles:
- Each subsystem runs its registered BIST verification callback.
- Subsystems report one of `HEALTH_STATE_OK`, `HEALTH_STATE_DEGRADED`, or `HEALTH_STATE_FAILED`.
- Any failure is logged to a non-volatile circular fault log for post-mortem diagnostics.
- Heap high-water marks and stack usage are continuously sampled.

---

## 5. Power & Battery Management

The `power_manager` operates a 5-state power machine:

| Mode | CPU Clock | Display | BLE State | IMU / Step Count | Optical PPG |
|:---|:---|:---|:---|:---|:---|
| **Active** | 128 MHz | ON (60 Hz) | Connected | Active (100 Hz) | Periodic (10s) |
| **Balanced** | 64 MHz | Sleep / Tilt-Wake | Connected | Low-power (25 Hz) | Periodic (5m) |
| **Battery Saver** | 64 MHz | Button Wake Only | Interval (5m) | Low-power (25 Hz) | Disabled |
| **Ultra Saver** | 32 MHz | Minimal Mono | OFF | Step Count Only | Disabled |
| **Deep Sleep** | 32 kHz | OFF | OFF | Suspended | Disabled |

