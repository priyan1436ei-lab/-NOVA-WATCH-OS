# NOVA WATCH OS - Bluetooth Low Energy (BLE) Protocol Specification

**Document Version:** 1.0.0  
**Transport:** Bluetooth 5.3 Low Energy (2 Mbps PHY, Coded PHY supported)  
**Security Level:** LE Security Mode 1, Level 4 (LE Secure Connections, Authenticated Pairing)  
**Base UUID:** `6E400000-B5A3-F393-E0A9-E50E24DCCA9E`  

---

## 1. GATT Service Overview

NOVA WATCH OS exposes a unified suite of custom GATT services designed for high throughput, minimal radio airtime, and secure companion synchronization:

| Service Name | Service UUID (16-bit / 128-bit) | Characteristic UUID | Properties | Description |
|:---|:---|:---|:---|:---|
| **NOVA Device Info** | `0x180A` (Standard) | `0x2A24` (Model)<br>`0x2A26` (Firmware)<br>`0x2A28` (Hardware) | Read | Device identifiers, serial numbers, hardware revisions. |
| **NOVA Device Status** | `0xFF10` | `0xFF11` (Status Telemetry)<br>`0xFF12` (Control Command) | Read, Notify<br>Write | Battery %, temperature, charging state, reboot/shutdown triggers. |
| **Time Sync** | `0x1805` (Standard) | `0x2A2B` (Current Time) | Read, Write, Notify | Unix epoch timestamp, timezone offset, daylight savings flags. |
| **Config Sync** | `0xFF20` | `0xFF21` (Config Block)<br>`0xFF22` (Config Ack) | Read, Write<br>Notify | JSON user preferences, alarm schedules, language, units. |
| **Watch Face Transfer** | `0xFF30` | `0xFF31` (Package Init)<br>`0xFF32` (Package Data)<br>`0xFF33` (Package Status) | Write<br>Write Without Response<br>Notify | Streaming binary transfer of declarative watch face packages. |
| **Health Sync** | `0xFF40` | `0xFF41` (Real-Time Biometrics)<br>`0xFF42` (Historic Ring Buffer) | Notify<br>Read, Write | Heart rate, SpO2, HRV, step counts, sleep stages. |
| **Notification Relay** | `0xFF50` | `0xFF51` (Inbound Alert)<br>`0xFF52` (Alert Action) | Write<br>Notify | Pushed notifications (Call, SMS, App), quick replies. |
| **OTA Firmware Control**| `0xFF60` | `0xFF61` (OTA Control)<br>`0xFF62` (OTA Stream)<br>`0xFF63` (OTA Status) | Write<br>Write Without Response<br>Notify | MCUboot secondary slot firmware staging, CRC check, swap request. |

---

## 2. Packet Framing & MTU Optimization

* **Negotiated MTU:** Target ATT MTU = 247 bytes (Payload = 244 bytes).
* **Connection Interval:** 
  * Normal Operation: 30 ms - 45 ms (low latency, balanced power).
  * High-Throughput Mode (Watch Face / OTA): 7.5 ms - 15 ms (burst transfer > 40 KB/s).
  * Low-Power Background Mode: 1000 ms (connected standby).

### Binary Packet Structure (Stream Characteristics)

```
+------------+------------+----------------+---------------------+------------+
| Magic (2B) | Seq ID (2B)| Length (2B)    | Payload (0..236B)   | CRC16 (2B) |
| 0x4E 0x56  | 0x0001     | 0x00EC         | [Binary Data...]    | 0xA3F2     |
+------------+------------+----------------+---------------------+------------+
```

1. **Magic Header (`0x4E56` = "NV"):** Frame synchronization bytes.
2. **Sequence Identifier:** Monotonically increasing packet counter to detect packet drops.
3. **Payload Length:** Explicit length of data payload.
4. **Payload:** Application data block.
5. **CRC16 (CCITT):** Hardware or software computed cyclic redundancy checksum.
