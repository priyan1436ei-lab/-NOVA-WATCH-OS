# NOVA WATCH OS - System Architecture

**Version:** 1.0.0  
**Target Platform:** Dual-Core nRF5340 ARM Cortex-M33 + Zephyr RTOS + LVGL  
**Companion Ecosystem:** Flutter Mobile App (Android/iOS) + Next.js Studio + Node.js/TypeScript Cloud  
**Status:** Approved Technical Architecture  

---

## 1. End-to-End System Architecture

The NOVA Smartwatch ecosystem operates as a vertically-integrated, secure personal compute and ambient intelligence platform comprising four core products:

```
+-----------------------------------------------------------------------------------------+
|                                    NOVA CLOUD PLATFORM                                  |
|   (Node.js + TypeScript + PostgreSQL + Supabase Auth + S3/Storage + AI Gateway)         |
+-----------------------------------------------------------------------------------------+
                    ^                                               ^
             REST / WebSockets                               REST / WebSockets
                    v                                               v
+-----------------------------------------+     +-----------------------------------------+
|             NOVA COMPANION              |     |               NOVA STUDIO               |
|         (Flutter Android & iOS)         |     |          (Next.js + TypeScript)         |
|  - BLE Central Manager & Pairing Engine |     |  - Declarative Watch Face Designer      |
|  - Health & Telemetry Synchronization   |     |  - Theme Builder & Widget Customizer    |
|  - AI Assistant Proxy & Voice Pipeline  |     |  - Interactive 360x360 Canvas Simulator |
|  - OTA Firmware Distribution Relay      |     |  - Marketplace & Package Signer         |
+-----------------------------------------+     +-----------------------------------------+
                    ^
             BLE 5.3 GATT (Encrypted)
                    v
+-----------------------------------------------------------------------------------------+
|                                      NOVA WATCH OS                                      |
|            (Embedded Zephyr RTOS + LVGL Graphics + 20 Modular Subsystems)               |
|                                                                                         |
|   +---------------------------------------------------------------------------------+   |
|   |                      USER INTERFACE & APPLICATION LAYER                         |   |
|   |  Home Screen (Clock/Complications) | Navigation (Swipes) | App Launcher         |   |
|   |  Watch Face Engine | Widget Engine | Theme Manager | 16 Built-in Apps           |   |
|   +---------------------------------------------------------------------------------+   |
|                                           |                                             |
|   +---------------------------------------------------------------------------------+   |
|   |                         OS SERVICES & MANAGERS LAYER                            |   |
|   |  App Mgr | Notification Mgr | Health Engine | AI Client | Audio Mgr | OTA Mgr   |   |
|   |  Battery/Power Mgr | Security Mgr | Diagnostics Mgr | Storage Mgr (LittleFS)   |   |
|   +---------------------------------------------------------------------------------+   |
|                                           |                                             |
|   +---------------------------------------------------------------------------------+   |
|   |                      HARDWARE ABSTRACTION LAYER (HAL)                           |   |
|   |  hal_display | hal_touch | hal_sensor | hal_power | hal_storage | hal_ble ...   |   |
|   +---------------------------------------------------------------------------------+   |
|                                           |                                             |
|   +---------------------------------------------------------------------------------+   |
|   |                  ZEPHYR RTOS KERNEL & DEVICE DRIVERS / MOCKS                    |   |
|   |  Preemptive Scheduler | IPC OpenAMP | MCUboot | CryptoCell-312 | Power Mgmt     |   |
|   +---------------------------------------------------------------------------------+   |
+-----------------------------------------------------------------------------------------+
```

---

## 2. Firmware Partitioning: Dual-Core Architecture

The Nordic nRF5340 dual-core architecture separates critical real-time wireless protocol processing from user-facing application and graphical rendering tasks:

```
+-----------------------------------------------------------------------------------------+
|                     APPLICATION CORE (Cortex-M33 @ 128 MHz, 1MB Flash, 512KB RAM)       |
|                                                                                         |
|  - Zephyr RTOS Kernel (Preemptive Multi-threading, Dynamic Slicing, System Tickless Idle) |
|  - LVGL 9.x Graphics Rendering Engine (360x360 circular double-buffer)                 |
|  - Hardware Abstraction Layer (HAL) & Core System Services                              |
|  - Biometric Signal Processing (PPG Heart Rate & SpO2 estimation algorithms)            |
|  - Sensor Fusion (BMI270 6-axis step counting, wrist-tilt gesture detection)            |
|  - Non-Volatile Storage (LittleFS on 16MB external QSPI flash)                          |
|  - Security Manager & Cryptographic Verification (CryptoCell-312)                       |
+-----------------------------------------------------------------------------------------+
                                            |
                         Hardware Inter-Processor Mailbox (IPC)
                         OpenAMP / RPMsg Shared RAM Ring Buffer
                                            |
+-----------------------------------------------------------------------------------------+
|                      NETWORK CORE (Cortex-M33 @ 64 MHz, 256KB Flash, 64KB RAM)          |
|                                                                                         |
|  - Zephyr BLE Controller (Link Layer, PHY 1M/2M/Coded, Isochronous Channels)            |
|  - BLE HCI Host-to-Controller Shared Memory Transport                                  |
|  - Radio Power Sequencing & Antenna Diversity Management                               |
+-----------------------------------------------------------------------------------------+
```

---

## 3. The 20 Firmware Subsystem Modules

NOVA WATCH OS is engineered with strict separation of concerns into 20 modular, event-driven components:

| # | Module Name | Responsibilities | Dependencies |
|:---|:---|:---|:---|
| **1** | **Boot Manager** | Stage sequencing, reset reason detection, safe-mode trigger, watchdog initialization, firmware integrity verification. | HAL, MCUboot, Security Mgr |
| **2** | **Hardware Abstraction Layer (HAL)** | Unified, portable C driver interfaces for Display, Touch, Sensors, PMIC, Flash, BLE, Audio, and Haptics. Enables both hardware and mock execution. | Zephyr Drivers / Mock Drivers |
| **3** | **Display Manager** | Display power rails, timing, frame rate regulation, tearing effect (TE) synchronization, brightness control, and sleep/wake transitions. | HAL Display, PMIC |
| **4** | **Touch Manager** | Interrupt-driven touch acquisition, contact filtering, coordinate debouncing, and raw touch event dispatch. | HAL Touch, Input Mgr |
| **5** | **Input and Gesture Manager** | Multi-point touch recognition, swipe vectors, tap/double-tap, long-press detection, and crown/button debouncing. | Touch Mgr, HAL Buttons |
| **6** | **Application Manager** | App lifecycle (Install, Launch, Foreground, Background, Suspend, Terminate), memory quotas, and execution permissions. | OS Scheduler, Storage Mgr |
| **7** | **Watch Face Engine** | Declarative JSON layout parser, clock rendering, dynamic complication binding, and animated backgrounds. | LVGL, Display Mgr, Sensors |
| **8** | **Widget Engine** | Lightweight glanceable widgets, circular paging, auto-refresh scheduling, and complication providers. | LVGL, Input Mgr, Sensors |
| **9** | **Theme Manager** | Global color palette runtime switching (NOVA Dark, Cyber, Luxury, Fitness, Light), contrast scaling, and font styling. | Watch Face Engine, LVGL |
| **10** | **Notification Manager** | Notification queueing, priority categorization, dismissal tracking, vibration alert mapping, and quick replies. | BLE Mgr, Haptic Mgr, Display |
| **11** | **Bluetooth Manager** | BLE 5.3 GATT server, connection management, MTU negotiation, bonded device security, and service routing. | Network Core IPC, Security |
| **12** | **Sensor Manager** | IMU (BMI270) and PPG (MAX86176) sampling scheduler, FIFO watermark handling, calibration, and power gating. | HAL Sensors, Power Mgr |
| **13** | **Health Processing Engine**| Real-time PPG peak detection, heart rate computation, SpO2 ratio-of-ratios, HRV (SDNN/RMSSD), step counting, and sleep staging. | Sensor Mgr, Storage Mgr |
| **14** | **Battery and Power Manager**| Fuel gauge monitoring, power modes (Active, Balanced, Battery Saver, Ultra Power, Sleep), rail gating, and critical shutdown. | HAL Power, PMIC, All Modules |
| **15** | **Storage Manager** | LittleFS mount and file operations, wear-leveling, flash partition management, user settings serialization, and crash dumps. | HAL Storage (QSPI Flash) |
| **16** | **Audio Manager** | PDM digital microphone streaming, audio capture ring buffer, I2S playback for notification chimes and feedback. | HAL Audio, Power Mgr |
| **17** | **AI Client** | On-device voice trigger and intent parser, companion AI proxy payload formatting, voice query streaming. | Audio Mgr, BLE Mgr, Companion |
| **18** | **Security Manager** | Cryptographic signature validation (Ed25519/ECDSA), secure key storage, BLE pairing authentication, and permission verification. | CryptoCell-312, Storage Mgr |
| **19** | **OTA Manager** | Dual-slot firmware reception, block-level CRC32 and SHA-256 verification, MCUboot swap scheduling, rollback recovery. | BLE Mgr, Storage Mgr, MCUboot |
| **20** | **Diagnostics Manager** | Built-in Self-Test (BIST) suite, stack/heap high-water mark telemetry, watchdog heartbeat registration, and fault logger. | All Modules, Storage Mgr |

---

## 4. Flash and RAM Memory Budgeting

### 4.1 Internal Application Core Memory (nRF5340)
* **Internal Flash (1024 KB):**
  * `0x0000_0000` - `0x0001_0000` (64 KB): **MCUboot Bootloader**
  * `0x0001_0000` - `0x0007_C000` (432 KB): **Primary Application Slot (Slot 0)** - Active Firmware
  * `0x0007_C000` - `0x000F_C000` (512 KB): **Secondary Application Slot (Slot 1)** - Staged Firmware
  * `0x000F_C000` - `0x0010_0000` (16 KB): **Non-Volatile Storage (NVS)** - Keys, Pairing, Device Config
* **Internal RAM (512 KB):**
  * `0x2000_0000` - `0x2000_8000` (32 KB): **Zephyr Kernel & System Stacks**
  * `0x2000_8000` - `0x2002_0000` (96 KB): **Subsystem Module Stacks & Queues**
  * `0x2002_0000` - `0x2004_4000` (144 KB): **LVGL Render Buffer** (2x 360x360 16-bit partial lines, 40 lines each)
  * `0x2004_4000` - `0x2006_0000` (112 KB): **Dynamic Heap & Application Allocations**
  * `0x2006_0000` - `0x2007_C000` (112 KB): **Sensor Ring Buffers & Biometric DSP Buffers**
  * `0x2007_C000` - `0x2008_0000` (16 KB): **Shared IPC Memory (OpenAMP / RPMsg with Net Core)**

### 4.2 External QSPI NOR Flash (16 MB)
* `0x0000_0000` - `0x0020_0000` (2 MB): Backup Recovery Firmware Image & MCUboot Scratch
* `0x0020_0000` - `0x00A0_0000` (8 MB): LittleFS Volume 1 - Watch Faces, UI Fonts, Images, Audio Chimes
* `0x00A0_0000` - `0x00E0_0000` (4 MB): LittleFS Volume 2 - Health History, Activity Logs, Sleep Data
* `0x00E0_0000` - `0x0100_0000` (2 MB): LittleFS Volume 3 - Crash Dumps, Diagnostics, Audit Trail

---

## 5. Event-Driven Asynchronous Message Flow

To ensure high UI responsiveness (60 FPS rendering) without blocking on sensor acquisition, flash writes, or BLE transfers, NOVA WATCH OS utilizes Zephyr asynchronous message queues (`k_msgq`), system events (`k_event`), and cooperative work queues:

```
[ Hardware Interrupt (Touch / Sensor / BLE / PMIC) ]
                         |
                         v (Interrupt Service Routine - ISR)
[ Submit Fast Event to Dispatcher Queue (k_msgq) ]
                         |
                         v (Kernel Context Switch)
[ Core Event Dispatcher Loop (Priority: 5) ]
         |                       |                       |
         v                       v                       v
[ Input / Touch Work ]   [ Sensor / DSP Work ]   [ BLE Communication Work ]
         |                       |                       |
         +-----------------------+-----------------------+
                                 |
                                 v
                [ LVGL UI Thread (Priority: 7) ]
           (Processes UI State, Updates Dirty Rectangles)
                                 |
                                 v
              [ Flush to AMOLED via DMA (QSPI) ]
```

---

## 6. Security Model & Trust Boundaries

1. **Hardware Root of Trust:** The bootloader (MCUboot) resides in write-protected Flash and verifies digital signatures (RSA-3072 or ECDSA-P256) of application firmware before booting.
2. **Rollback Protection:** Monotonic hardware counters prevent flashing older firmware versions with known vulnerabilities.
3. **BLE Security:** All GATT operations carrying health data, watch face packages, or system settings require BLE Security Mode 1 Level 4 (LE Secure Connections, Authenticated Pairing with Numeric Comparison and 128-bit AES-CCM encryption).
4. **App Sandbox:** Third-party watch faces and widgets are declarative JSON data structures parsed by the native engine—**no arbitrary machine code or bytecode is executed on device**.
5. **Data Protection:** Health and biometric records stored in LittleFS are encrypted using AES-128-XTS with keys stored in the nRF5340 secure CryptoCell key vault.

