# NOVA WATCH OS - Over-The-Air (OTA) Firmware Update Guide

**Version:** 1.0.0  
**Bootloader:** MCUboot Dual-Slot Swap / Overwrite Engine  
**Transfer Channel:** Encrypted BLE 5.3 GATT OTA Service (`0xFF60`)  

---

## 1. Dual-Slot Partition Architecture

NOVA WATCH OS utilizes an A/B dual-slot memory layout to prevent "bricking" during interrupted firmware updates:

* **Slot 0 (Primary Application Slot):** Internal Flash address `0x0001_0000` (432 KB). Holds the currently active, validated bootable image.
* **Slot 1 (Secondary Staging Slot):** Internal Flash address `0x0007_C000` (512 KB) or External QSPI Flash. Used to stage newly downloaded firmware chunks without overwriting the running firmware.
* **Scratch Partition:** Dedicated partition used by MCUboot to perform atomic, power-fail-safe sector swaps.

---

## 2. OTA Protocol Execution Flow

```
[ Companion App ]                                   [ NOVA Watch OS ]
        |                                                   |
        |--- 1. OTA_INIT (Version, File Size, SHA-256) ---->|
        |                                                   | Checks flash space & battery (>30%)
        |<-- 2. OTA_READY (Chunk Size = 240 bytes) ---------|
        |                                                   |
        |=== 3. Streaming Firmware Chunks (0..N) ==========>|
        |                                                   | Writes to Slot 1, updates progress
        |                                                   |
        |--- 4. OTA_FINALIZE (Full File Checksum) --------->|
        |                                                   | Verifies SHA-256 & ECDSA Signature
        |<-- 5. OTA_VERIFIED (Swap Scheduled) --------------|
        |                                                   | Marks image "test-swap" in MCUboot
        |                                                   |
        |                                                   | Reboots into MCUboot
        |                                                   | MCUboot validates & swaps Slot 1 <-> Slot 0
        |                                                   | Boots new image
        |                                                   | Self-test verifies stability
        |<== 6. Reconnect & Confirm Success ===============| Marks image "permanent"
```

### Safety Guarantees
1. **Never erase Slot 0 before validation:** Slot 0 remains 100% untouched until Slot 1 is completely received and its cryptographic signature verified.
2. **Battery Threshold Enforcement:** If battery level is below 30% and the watch is not connected to a charger, OTA updates are rejected.
3. **Rollback Protection:** If the new image fails its Built-In Self-Tests or fails to confirm stability within 60 seconds of booting, MCUboot automatically rolls back to Slot 0.
