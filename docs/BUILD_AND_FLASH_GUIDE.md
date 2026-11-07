# NOVA WATCH OS - Build and Flashing Guide

**Version:** 1.0.0  
**Target Hardware:** Nordic nRF5340-DK (PCA10095) / Custom Smartwatch Rev A  
**Host Platform:** Windows, Linux, macOS  

---

## 1. Development Workstation Prerequisites

### Required Tools
1. **West (Zephyr Meta-tool):** `pip install west`
2. **CMake (>= 3.20.0):** `pip install cmake`
3. **Ninja (Build system):** `pip install ninja`
4. **ARM GNU Toolchain or Zephyr SDK (v0.16.x+):** `arm-none-eabi-gcc` or `llvm-arm`
5. **nRF Command Line Tools:** `nrfjprog` and J-Link drivers (Nordic Semiconductor)
6. **Host C Compiler (For Unit Tests):** Clang / GCC / MSVC (e.g., portable Zig / MinGW)

---

## 2. Host Simulation & Test Suite (No Hardware Required)

To verify the firmware modules, Hardware Abstraction Layer, diagnostics self-tests, and power state machines on your local development PC:

### Automatic One-Click Execution (PowerShell)
```powershell
cd firmware
.\run_tests.ps1
```

### Manual Host Build with CMake & Ninja
```powershell
cd firmware
cmake -B build -G Ninja -DCMAKE_C_COMPILER="$HOME\.tools\zig\gcc.cmd"
ninja -C build
.\build\nova_tests.exe
.\build\nova_firmware_sim.exe
```

### Python Logic & Budget Verification Suite
```powershell
pytest firmware/tests/test_firmware_python.py -v
```

---

## 3. Physical Hardware Build (nRF5340 Target with Zephyr RTOS)

### Step 1: Initialize the West Workspace
```bash
west init -m https://github.com/nova-watch-os/firmware --mr main workspace
cd workspace
west update
west zephyr-export
```

### Step 2: Build Application Core Firmware
```bash
west build -b nova_watch_nrf5340_cpuapp firmware --pristine
```

### Step 3: Flash Application Core via J-Link
```bash
# Erase and flash MCUboot bootloader + Slot 0 application firmware
nrfjprog --program build/zephyr/zephyr.hex --chiperase --reset --verify
```

### Step 4: Build & Flash Network Core (BLE Controller)
```bash
west build -b nova_watch_nrf5340_cpunet firmware/boards/arm/nova_watch_nrf5340 --pristine
nrfjprog --program build/zephyr/zephyr.hex --coprocessor CP_NETWORK --sectorerase --reset --verify
```

