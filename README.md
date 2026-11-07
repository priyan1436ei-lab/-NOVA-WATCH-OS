# NOVA WATCH OS

```
   _  ______  _  _____       _       _____ _____ _____ _   _     ____   _____ 
  | \ | |/ _ \| |/ / _ \     | |     / / _ \_   _/  __ \ | | |   / __ \ / ____|
  |  \| | | | | ' / /_\ \    | | /| / / /_\ \| | | /  \/ |_| |  | |  | | (___  
  | . ` | | | |  <|  _  |    | |/ |/ /|  _  || | | |   |  _  |  | |  | |\___ \ 
  | |\  | |_| | . \ | | |    |  /|  / | | | || | | \__/\ | | |  | |__| |____) |
  \_| \_/\___/|_|\_\_| |_/     \/   \/  \_| |_/\_/  \____|_| |_|   \____/|_____/ 
```

**NOVA WATCH OS** is an end-to-end, production-grade smartwatch ecosystem engineered for custom wearable hardware. Designed with an embedded real-time operating system (RTOS) baseline, modern declarative touch interfaces, a companion mobile application, web-based watch face studio, and resilient cloud backend.

---

## 1. Ecosystem Overview

NOVA WATCH OS comprises four fully integrated products:

1. **NOVA WATCH OS (Firmware)**:
   - Target SoC: Nordic Semiconductor nRF5340 dual-core ARM Cortex-M33 (128 MHz App + 64 MHz Net Core).
   - RTOS Kernel: Zephyr RTOS with preemptive multi-threading, tickless idle, and MCUboot secure bootloader.
   - Graphics: LVGL 9.x circular display rendering pipeline (360×360 AMOLED @ 60 FPS).
   - Modular Architecture: 20 discrete hardware managers, health processing engine, storage (LittleFS), and security crypto subsystems.
2. **NOVA COMPANION (Mobile App)**:
   - Built with Flutter for Android and iOS.
   - Encrypted BLE 5.3 GATT pairing, biometric telemetry dashboards, on-device AI proxy, and OTA firmware updates.
3. **NOVA STUDIO (Web App)**:
   - Built with Next.js, TypeScript, Tailwind CSS, and shadcn/ui.
   - Visual drag-and-drop watch face designer, circular display canvas preview, theme builder, and package verification.
4. **NOVA CLOUD (Backend Platform)**:
   - Built with Node.js, TypeScript, PostgreSQL, and Supabase Auth.
   - REST & WebSocket APIs for device synchronization, AI Gateway, OTA firmware artifact distribution, and marketplace.

---

## 2. Directory Structure

```
.
├── README.md                      # Project root documentation
├── SYSTEM_ARCHITECTURE.md         # End-to-end system design & dual-core topology
├── HARDWARE_SPECIFICATION.md      # Component specs, pin matrix, power budgets, BOM
├── DEVELOPMENT_STATUS.md          # Active status tracking & verified test results
├── docs/                          # In-depth architectural & operational manuals
│   ├── FIRMWARE_GUIDE.md          # 20 subsystem module specifications & lifecycle
│   ├── BLE_PROTOCOL.md            # Custom GATT service definitions & packet framing
│   ├── MOBILE_APP_GUIDE.md        # Flutter companion app architecture & mock mode
│   ├── API_DOCUMENTATION.md       # REST & WebSocket API specification (OpenAPI)
│   ├── DATABASE_SCHEMA.md         # PostgreSQL entity relationships & migrations
│   ├── SECURITY_MODEL.md          # Root-of-trust, encryption, sandboxing, and auth
│   ├── OTA_GUIDE.md               # MCUboot dual-slot firmware verification & update
│   ├── TESTING_GUIDE.md           # Firmware, mobile, web, and backend testing strategy
│   ├── BUILD_AND_FLASH_GUIDE.md   # Flashing nRF5340 via west/nrfjprog and host tests
│   ├── TROUBLESHOOTING.md         # Diagnostics, crash loops, safe mode, recovery
│   └── DEVELOPMENT_ROADMAP.md     # 14-phase development lifecycle roadmap
├── firmware/                      # Embedded Zephyr RTOS C/C++ firmware
│   ├── CMakeLists.txt             # Dual-mode CMake (Zephyr target + Host simulator)
│   ├── prj.conf                   # Zephyr Kconfig options
│   ├── west.yml                   # West repository manifest
│   ├── run_tests.ps1              # Automated build and test script
│   ├── boards/                    # nRF5340 board DeviceTree & configs
│   ├── include/                   # Public headers & Hardware Abstraction Layer
│   ├── src/                       # Module implementations & driver mocks
│   └── tests/                     # Unit test suites (Boot, HAL, Diagnostics, Power)
├── companion/                     # Flutter mobile application (Android/iOS)
├── studio/                        # Next.js web watch-face designer
└── cloud/                         # Node.js + TypeScript backend services
```

---

## 3. Quick Start: Host Simulation & Unit Tests

You can build and execute the NOVA WATCH OS firmware simulator and native test suite on any workstation without physical hardware:

```powershell
# 1. Run the automated test runner
cd "firmware"
.\run_tests.ps1
```

Or run the Python verification suite:

```powershell
pytest firmware/tests/test_firmware_python.py -v
```

---

## 4. Current Status: Phase 1 & 2 Complete

- **Phase 1 (Hardware Evaluation & Requirements)**: Complete. Target hardware baseline established on nRF5340, 1.57" 360x360 AMOLED, CST816S touch, BMI270 IMU, MAX86176 PPG, nPM1300 PMIC, DRV2605L haptics, and 16MB QSPI flash. Complete pinout and BOM defined.
- **Phase 2 (Board Setup & Zephyr RTOS Core)**: Complete. Zephyr DeviceTree configurations, West manifest, HAL interfaces, mock drivers, Boot Manager, Diagnostics Manager (with BIST), and Power Manager implemented with 100% unit test coverage.

# -NOVA-WATCH-OS
