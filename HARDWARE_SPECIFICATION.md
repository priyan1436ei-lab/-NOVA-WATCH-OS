# NOVA WATCH OS - Hardware Specification (Phase 1 Baseline)

**Document Version:** 1.0.0  
**Target Hardware Revision:** REV A (Custom Hardware Baseline)  
**Reference Platform:** Nordic Semiconductor nRF5340 Dual-Core System-on-Chip (SoC)  
**Status:** Approved for Phase 1 & 2 Firmware Development  

---

## 1. System Overview & Core Architecture

NOVA WATCH OS targets a custom circular smartwatch hardware platform designed for ultra-low power consumption, high-contrast visual performance, and rich biometric monitoring. The core architecture centers on the **Nordic Semiconductor nRF5340** dual-core SoC, supported by an external Power Management Integrated Circuit (PMIC), High-Speed QSPI NOR Flash, circular AMOLED display, and a dedicated multi-sensor biometric array.

```
+---------------------------------------------------------------------------------------+
|                                    NOVA SMARTWATCH REV A                              |
|                                                                                       |
|   +------------------------------------+      +-----------------------------------+   |
|   |         nRF5340 Dual-Core          |      |         nPM1300 PMIC              |   |
|   |                                    |      |                                   |   |
|   |  [App Core: Cortex-M33 @ 128MHz]   |<---->|  Buck 1: VDD_SYS (1.8V Core/IO)   |   |
|   |   - 1 MB Flash, 512 KB RAM         | I2C2 |  Buck 2: VDD_DISP (3.3V Disp/Aud) |   |
|   |   - DSP, FPU, TrustZone            |      |  Linear Li-Po Charger (350mAh)    |   |
|   |                                    |      |  Accurate Fuel Gauge & NTC Monitor|   |
|   |  [Net Core: Cortex-M33 @ 64MHz]    |      +-----------------------------------+   |
|   |   - 256 KB Flash, 64 KB RAM        |                                              |
|   |   - 2.4 GHz Transceiver (BLE 5.3)  |                                              |
|   +------------------------------------+                                              |
|         |           |           |                                                     |
|       QSPI         I2C1        SPI2 / PDM / I2S                                       |
|         |           |           |                                                     |
|         v           |           +-----------------------------------------------+     |
|   +-----------+     |           |                       |                       |     |
|   | W25Q128JV |     |           v                       v                       v     |
|   | 16MB QSPI |     |      +---------+             +---------+             +---------+|
|   | NOR Flash |     |      |MAX86176 |             |SPH0645  |             |MAX98357A||
|   | (XiP, LFS)|     |      |Dual PPG |             |PDM Mic  |             |I2S Audio||
|   +-----------+     |      |Biometrics             |Voice AI |             |Class D  ||
|                     |      +---------+             +---------+             +---------+|
|                     v                                                                 |
|   +-------------------------------------------------------------+                     |
|   |         I2C1 Sensor & Peripheral Bus (400 kHz)              |                     |
|   |                                                             |                     |
|   |   +-------------------+  +-------------------+  +---------+ |                     |
|   |   |  Visionox 1.57"   |  |  Hynitron CST816S |  | BMI270  | |                     |
|   |   |  360x360 AMOLED   |  |  Capacitive Touch |  | 6-Axis  | |                     |
|   |   |  CO5300 / RM69090 |  |  Gesture Engine   |  | IMU/Step| |                     |
|   |   |  (QSPI Display)   |  |  (Interrupt-driven|  | Counter | |                     |
|   |   +-------------------+  +-------------------+  +---------+ |                     |
|   |                                                             |                     |
|   |   +-------------------+                                     |                     |
|   |   |  TI DRV2605L      |                                     |                     |
|   |   |  LRA Haptic Motor |                                     |                     |
|   |   |  Driver Engine    |                                     |                     |
|   |   +-------------------+                                     |                     |
|   +-------------------------------------------------------------+                     |
+---------------------------------------------------------------------------------------+
```

---

## 2. Component Specifications

### 2.1 Microcontroller & Processing Unit
* **Model:** Nordic Semiconductor **nRF5340** (CLAA-A0, WLCSP94 or QFN94 package)
* **Application Processor:**
  * ARM® Cortex®-M33 with FPU (single precision) and DSP instruction set
  * Configurable clock frequency: 64 MHz / 128 MHz
  * Internal Memory: 1024 KB Flash with cache, 512 KB Low-leakage SRAM
  * Security: ARM TrustZone® CryptoCell-312 cryptographic co-processor
* **Network Processor:**
  * ARM® Cortex®-M33 @ 64 MHz
  * Internal Memory: 256 KB Flash, 64 KB RAM
  * Dedicated 2.4 GHz multiprotocol radio supporting Bluetooth® 5.3 Low Energy (1M, 2M, Coded Long Range PHYs, Direction Finding AoA/AoD)
* **Inter-Processor Communication (IPC):**
  * Hardware OpenAMP / RPMsg mailbox with shared dual-port SRAM and hardware semaphores.

### 2.2 Display Subsystem
* **Type:** 1.57-inch Circular Active Matrix Organic Light-Emitting Diode (AMOLED)
* **Active Resolution:** 360 × 360 pixels (Circular mask active area: Diameter = 35.8 mm)
* **Pixel Density:** 326 PPI
* **Color Depth:** 16.7M (RGB888 24-bit) or 65K (RGB565 16-bit low-power mode)
* **Peak Brightness:** 800 cd/m² (Automatic Ambient Light Scaling)
* **Driver IC:** Chip-on-Glass (COG) **CO5300** or **RM69090**
* **Host Interface:** 
  * 4-wire Quad SPI (QSPI) or Dual-SPI + D/CX command interface @ up to 50 MHz
  * Tearing Effect (TE) synchronization line to prevent visual tearing
  * Power rails: VDD_IO = 1.8V, VCI = 3.3V, ELVDD/ELVSS generated via internal PMIC/charge pump.

### 2.3 Capacitive Touch Interface
* **Controller:** Hynitron **CST816S** (Ultra-low power single/dual-point capacitive touch controller)
* **Surface Lens:** 2.5D Curved Corning Gorilla Glass 3 with Oleophobic and Anti-Fingerprint (AF) coating
* **Interface:** Fast-mode I2C (address `0x15`), clock frequency up to 400 kHz
* **Interrupt Line:** Active-low edge-triggered interrupt (`TOUCH_INT_N`)
* **Reporting Modes:** Coordinate stream (X: 0..359, Y: 0..359) and hardware gesture engine (Single Tap, Double Tap, Long Press, Swipe Up, Swipe Down, Swipe Left, Swipe Right)
* **Operating Current:** Active: < 1.2 mA; Monitor/Wake-on-touch: 8 µA; Deep Sleep: 1.5 µA.

### 2.4 External Non-Volatile Storage
* **Model:** Winbond **W25Q128JVSIM** (128 Mbit / 16 MByte NOR Flash)
* **Interface:** Quad-SPI (QSPI) with eXecute-in-Place (XiP) support @ 32 MHz
* **Erase Blocks:** 4 KB Sector, 32 KB/64 KB Block
* **Endurance:** 100,000 Program/Erase cycles, 20-year data retention
* **Partition Allocation:**
  * `0x0000_0000` - `0x0020_0000` (2 MB): Secondary OTA Firmware Staging Slot (MCUboot)
  * `0x0020_0000` - `0x00A0_0000` (8 MB): Watch Face Packages & Graphics Assets (LittleFS)
  * `0x00A0_0000` - `0x00E0_0000` (4 MB): Offline Health Metrics, Sensor Ring Buffer, Sleep logs
  * `0x00E0_0000` - `0x0100_0000` (2 MB): Crash Dumps, Diagnostics, Audit Log, User Config

### 2.5 Motion Sensing (Inertial Measurement Unit)
* **Model:** Bosch Sensortec **BMI270**
* **Type:** Ultra-low power 6-axis IMU (3-axis 16-bit accelerometer + 3-axis 16-bit gyroscope)
* **Interface:** I2C (address `0x68`) or SPI mode 3 @ up to 10 MHz
* **On-Chip Features:** Integrated Wearable Gesture Engine (wrist tilt to wake, push to open, glance detection), step counter, step detector, significant motion detection
* **Current Consumption:** Full 6-axis @ 100 Hz ODR: 685 µA; Accelerometer low-power step count: 18 µA; Suspend mode: 2 µA.

### 2.6 Biometric Sensing Subsystem (PPG)
* **Model:** Analog Devices **MAX86176** or **MAX86141**
* **Type:** High-performance Dual-Channel Optical Analog Front-End (AFE) for Biometric Pulse Oximetry (SpO2), Heart Rate (HR), and Heart Rate Variability (HRV)
* **Optics:** 2x Green LEDs (525 nm, HR), 1x Red LED (660 nm, SpO2), 1x Infrared LED (940 nm, SpO2/HRV), paired with matched multi-channel photodiodes
* **SNR:** > 110 dB optical SNR with ambient light cancellation (ALC) up to 100 µA ambient current
* **Interface:** SPI mode 0 @ up to 8 MHz (or I2C `0x5A`)
* **FIFO:** 32-sample biometric data FIFO buffer with configurable watermark interrupt.

### 2.7 Power Management & Battery Subsystem
* **PMIC Model:** Nordic Semiconductor **nPM1300** (or TI BQ25120A)
* **Battery Cell:** High-density rechargeable Lithium-Polymer (Li-Po) cell
  * Nominal Voltage: 3.7 V (Cutoff: 3.0 V, Full Charge Float: 4.20 V / 4.35 V)
  * Rated Capacity: 350 mAh / 1.30 Wh
* **Charging Interface:** Magnetic 2-pin pogo pin connector with reverse-polarity protection diode
* **Charging Controller:** Linear charger with configurable constant-current (CC) up to 300 mA, constant-voltage (CV) with termination @ 10 mA
* **Fuel Gauge:** High-precision coulomb counter + voltage/current ADC monitor (< 1% error)
* **Thermal Protection:** 10 kΩ NTC thermistor input adhering to JEITA standard charging profiles.

### 2.8 Haptic Feedback Subsystem
* **Driver IC:** Texas Instruments **DRV2605L**
* **Actuator:** Linear Resonant Actuator (LRA) tuned for 170 Hz resonance
* **Interface:** I2C (address `0x5A`)
* **Integrated ROM:** Immersion TouchSense® 2200 licensed waveform library (123 haptic effects: sharp clicks, soft pulses, transition alerts, ramps).

### 2.9 Audio Subsystem
* **Digital Microphone:** Knowles **SPH0645LM4H**
  * Type: PDM (Pulse Density Modulation) digital MEMS microphone
  * Sensitivity: -26 dBFS @ 1 kHz 94 dB SPL
  * Acoustic Overload Point: 120 dB SPL
  * Operating Current: 600 µA active, 15 µA standby
* **Audio Amplifier:** Maxim Integrated **MAX98357A**
  * Type: Mono Class-D audio amplifier with integrated I2S DAC
  * Output Power: 3.2 W into 4 Ω at 5V, 0.6 W into 8 Ω at 3.3V
  * Standby Current: 2.4 µA
* **Acoustic Transducer:** 15 × 11 × 3 mm waterproof micro-speaker (8 Ω, 0.7 W rated).

---

## 3. Microcontroller Pin Interconnect Matrix (nRF5340)

| Pin Name | nRF5340 GPIO | Function / Peripheral | Direction | Pull / Drive | Target Peripheral | Description |
|:---|:---|:---|:---|:---|:---|:---|
| **P0.00** | P0.00 | XL1 (32.768 kHz Osc) | Input | None | Crystal Y1 | Low-frequency RTOS Clock |
| **P0.01** | P0.01 | XL2 (32.768 kHz Osc) | Output | None | Crystal Y1 | Low-frequency RTOS Clock |
| **P0.04** | P0.04 | I2C1_SCL | In/Out | Pull-up (4.7k) | CST816S, BMI270, nPM1300 | Sensor Bus Clock (400 kHz) |
| **P0.05** | P0.05 | I2C1_SDA | In/Out | Pull-up (4.7k) | CST816S, BMI270, nPM1300 | Sensor Bus Data (400 kHz) |
| **P0.06** | P0.06 | TOUCH_INT_N | Input | Pull-up | CST816S Touch | Touch wake/event interrupt |
| **P0.07** | P0.07 | TOUCH_RST_N | Output | High Drive | CST816S Touch | Touch hardware reset |
| **P0.08** | P0.08 | IMU_INT1 | Input | Pull-down | BMI270 IMU | Step / tilt / motion interrupt |
| **P0.09** | P0.09 | IMU_INT2 | Input | Pull-down | BMI270 IMU | FIFO watermark interrupt |
| **P0.10** | P0.10 | PMIC_INT_N | Input | Pull-up | nPM1300 PMIC | Charge status / low battery IRQ |
| **P0.11** | P0.11 | HAPTIC_EN | Output | Push-pull | DRV2605L Haptics | Haptic driver enable |
| **P0.13** | P0.13 | QSPI_SCK | Output | High Drive | W25Q128JV Flash | QSPI Clock (32 MHz) |
| **P0.14** | P0.14 | QSPI_CSN | Output | High Drive | W25Q128JV Flash | QSPI Chip Select |
| **P0.15** | P0.15 | QSPI_IO0 | In/Out | High Drive | W25Q128JV Flash | QSPI Data 0 |
| **P0.16** | P0.16 | QSPI_IO1 | In/Out | High Drive | W25Q128JV Flash | QSPI Data 1 |
| **P0.17** | P0.17 | QSPI_IO2 | In/Out | High Drive | W25Q128JV Flash | QSPI Data 2 / WP |
| **P0.18** | P0.18 | QSPI_IO3 | In/Out | High Drive | W25Q128JV Flash | QSPI Data 3 / HOLD |
| **P0.20** | P0.20 | DISP_CSN | Output | High Drive | CO5300 AMOLED | Display Chip Select |
| **P0.21** | P0.21 | DISP_SCK | Output | High Drive | CO5300 AMOLED | Display SPI Clock (50 MHz) |
| **P0.22** | P0.22 | DISP_MOSI | Output | High Drive | CO5300 AMOLED | Display SPI MOSI (Data In) |
| **P0.23** | P0.23 | DISP_DCX | Output | Push-pull | CO5300 AMOLED | Display Data / Command Select |
| **P0.24** | P0.24 | DISP_RST_N | Output | Push-pull | CO5300 AMOLED | Display Reset (Active Low) |
| **P0.25** | P0.25 | DISP_TE | Input | Pull-down | CO5300 AMOLED | Tearing Effect VSYNC Sync |
| **P0.28** | P0.28 | PPG_SPI_SCK | Output | High Drive | MAX86176 PPG | PPG SPI Clock |
| **P0.29** | P0.29 | PPG_SPI_MOSI| Output | High Drive | MAX86176 PPG | PPG SPI MOSI |
| **P0.30** | P0.30 | PPG_SPI_MISO| Input | None | MAX86176 PPG | PPG SPI MISO |
| **P0.31** | P0.31 | PPG_SPI_CSN | Output | High Drive | MAX86176 PPG | PPG Chip Select |
| **P1.00** | P1.00 | PPG_INT_N | Input | Pull-up | MAX86176 PPG | Biometric FIFO Interrupt |
| **P1.02** | P1.02 | PDM_CLK | Output | High Drive | SPH0645 Mic | Digital Mic Clock |
| **P1.03** | P1.03 | PDM_DATA | Input | Pull-down | SPH0645 Mic | Digital Mic Data |
| **P1.04** | P1.04 | I2S_BCLK | Output | High Drive | MAX98357A Amp | Audio Bit Clock |
| **P1.05** | P1.05 | I2S_LRCK | Output | High Drive | MAX98357A Amp | Audio Word Clock (LRCLK) |
| **P1.06** | P1.06 | I2S_SDATA | Output | High Drive | MAX98357A Amp | Audio Serial Data |
| **P1.08** | P1.08 | BTN1_CROWN | Input | Pull-up | Physical Button | Main Navigation / Crown Button |
| **P1.09** | P1.09 | BTN2_SHORTCUT| Input | Pull-up | Physical Button | Customizable Shortcut / Back |
| **P1.10** | P1.10 | VDD_PERIPH_EN| Output | Push-pull | Power Gate FET | 3.3V Peripheral Rail Enable |

---

## 4. Power Budgets and Battery Life Projections

| Operating Mode | Subsystem States | Typical Current | Estimated Battery Life (350 mAh) |
|:---|:---|:---|:---|
| **Active Display & UI** | 128 MHz CPU, Display ON @ 60 FPS, Touch Active, BLE Connected, IMU Active | 22.4 mA | **~15.6 Hours** of continuous screen-on time |
| **Balanced (Typical Use)** | 64 MHz CPU, Display OFF, Wrist-Tilt Wake Enabled, BLE Adv/Conn @ 1s, Step Counter ON, Periodic Heart Rate (every 5 min) | 3.8 mA | **~92 Hours (~3.8 Days)** |
| **Always-On Display (AOD)** | AMOLED 1 Hz Refresh, 5% Pixel Ratio, BLE Connected, Sensors Polling | 5.2 mA | **~67 Hours (~2.8 Days)** |
| **Battery Saver Mode** | AMOLED OFF, Wake on Button only, BLE background sync @ 5 min, Step counting active, Continuous HR disabled | 1.4 mA | **~250 Hours (~10.4 Days)** |
| **Ultra Power Saver** | Minimal UI, Monochrome time on button press, Radio OFF, IMU step count only | 0.42 mA | **~833 Hours (~34.7 Days)** |
| **Deep Standby (Storage)** | nRF5340 System OFF, PMIC ship mode, Flash deep powerdown | 42 µA | **> 340 Days** |

---

## 5. Bill of Materials (BOM) Baseline

| Item # | Designator | Component / Function | Manufacturer | Manufacturer Part Number (MPN) | Package | Qty | Est. Unit Cost ($ / 10k) |
|:---|:---|:---|:---|:---|:---|:---|:---|
| 1 | U1 | Dual-Core SoC (BLE 5.3 + Crypto) | Nordic Semi | NRF5340-CLAA-R | WLCSP94 | 1 | $3.85 |
| 2 | U2 | PMIC + Fuel Gauge + Charger | Nordic Semi | NPM1300-CAAA-R | QFN32 | 1 | $1.45 |
| 3 | U3 | 128 Mbit (16 MB) QSPI NOR Flash | Winbond | W25Q128JVSIM | SOIC8 / WSON8 | 1 | $0.88 |
| 4 | DISP1 | 1.57" Circular AMOLED (360x360) | Visionox / Topway | H157QVN01.0 / RM69090 | FPC COG | 1 | $9.50 |
| 5 | TOUCH1 | Capacitive Touch Sensor + FPC | Hynitron | CST816S | QFN16 / COF | 1 | $0.62 |
| 6 | U4 | Ultra-Low Power 6-Axis IMU | Bosch Sensortec | BMI270 | LGA-14 | 1 | $1.15 |
| 7 | U5 | Dual-Channel Biometric Optical AFE| Analog Devices | MAX86176ENY+ | WLP25 | 1 | $2.30 |
| 8 | U6 | LRA Haptic Motor Driver | Texas Instruments| DRV2605LDGSR | VSSOP10 | 1 | $0.78 |
| 9 | M1 | Linear Resonant Actuator (170Hz)| Jinlong Machinery| G0832012 | Coin (8x3.2mm) | 1 | $0.65 |
| 10 | MIC1 | Digital PDM MEMS Microphone | Knowles | SPH0645LM4H-B | LGA-4 | 1 | $0.72 |
| 11 | U7 | Mono I2S Class-D Audio Amp | Maxim Integrated | MAX98357AEWL+T | WLP9 | 1 | $0.82 |
| 12 | SPK1 | Miniature Speaker 8Ω 0.7W | CUI Devices | CDM-1511-08-3.0 | 15x11x3mm SMD | 1 | $0.95 |
| 13 | BATT1 | Rechargeable Li-Po Battery 350mAh | EEMB / Custom | LP402535 | Pouch Cell | 1 | $2.40 |
| 14 | Y1 | 32.768 kHz RTC Crystal ±20ppm | Abracon | ABS07-32.768KHZ-T | SMD 3.2x1.5mm | 1 | $0.22 |
| 15 | Y2 | 32.000 MHz HF Crystal ±10ppm | Epson | FA-20H 32.0000MF10Z | SMD 2.5x2.0mm | 1 | $0.28 |
| 16 | ANT1 | 2.4 GHz Ceramic Chip Antenna | Johanson Tech | 2450AT18D0100E | 0805 SMD | 1 | $0.35 |
| **Total**| | | | | | | **~$26.92** |

---

## 6. Hardware Verification & Dependency Matrix

The physical hardware dependencies identified for production verification are:
1. **nRF5340-DK (PCA10095)**: Development board used for initial firmware prototyping.
2. **AMOLED Interface Breakout Board**: High-speed flex cable adapter board connecting the QSPI display to nRF5340 P0.20-P0.25.
3. **Biometric Optical Test Fixture**: Optical skin contact test bench with calibrated LED driver current controls.
4. **Hardware Power Profiler (Nordic PPK2)**: Precision power measurement tool for microampere standby and milliampere active validation.

Until physical custom Rev A PCBs are fabricated and delivered, **all hardware interactions must strictly route through the Hardware Abstraction Layer (HAL) interfaces with verified Host and Zephyr simulation mocks**.

