# NOVA WATCH OS - Troubleshooting & Diagnostic Runbook

**Version:** 1.0.0  
**Target:** Engineering Diagnostics, Safe Mode Recovery, Flash Errors  

---

## 1. Common Failure Modes & Resolutions

### 1. Watch Enters Safe Mode Automatically
* **Symptom:** The watch boots into a minimal monochrome safe screen displaying "NOVA RECOVERY / SAFE MODE".
* **Root Cause:** The system recorded 3 consecutive watchdog timeouts or system panics without reaching `BOOT_STAGE_RUNNING`.
* **Resolution:**
  1. Connect watch via USB/UART or attach J-Link debugger.
  2. Read the non-volatile fault log using `diagnostics_get_telemetry()`.
  3. Clear the crash counter using `boot_manager_record_successful_boot()` after identifying and resolving the faulting driver or bad watch face file.

### 2. AMOLED Display Shows Blank / No Tearing Effect
* **Symptom:** Backlight / power rail active, but no pixels rendered.
* **Root Cause:** 
  - QSPI flex cable improper seating.
  - SPI frequency configured above display controller maximum (50 MHz).
  - Tearing effect (TE) pin synchronization timeout.
* **Resolution:** Verify pin mappings in `nova_watch_nrf5340_cpuapp.dts` and inspect TE pin waveform with an oscilloscope.

### 3. Touch Coordinates Inverted or Unresponsive
* **Symptom:** Swiping left triggers a swipe right or touch registers on opposite edge.
* **Root Cause:** Touch controller orientation mismatch with display rotation.
* **Resolution:** Ensure `CST816S` coordinates are transformed in `hal_touch.c` based on display orientation flags (`NOVA_DISPLAY_IS_ROUND`).

