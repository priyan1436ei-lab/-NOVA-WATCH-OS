# NOVA WATCH OS - Security & Trust Model

**Version:** 1.0.0  
**Target:** ARM TrustZone-M, Nordic CryptoCell-312, MCUboot, Encrypted LittleFS  

---

## 1. Threat Matrix & Defense Strategy

| Threat Vector | Mitigation Strategy | Hardware / Protocol Mechanism |
|:---|:---|:---|
| **Malicious Firmware Flashing** | Cryptographically signed binaries verified before boot. Unsigned binaries rejected. | MCUboot + RSA-3072 / ECDSA-P256 hardware signature verification. |
| **Firmware Downgrade (Rollback)** | Monotonic version counters checked during staging. Older revisions rejected. | Non-volatile security counters in hardware flash. |
| **BLE Eavesdropping / Spoofing** | LE Secure Connections with Diffie-Hellman P-256 key exchange, numeric comparison. | BLE Security Mode 1 Level 4 + AES-128-CCM encryption. |
| **Untrusted Watch Face Execution** | Sandboxed declarative JSON specification. No native machine code execution allowed. | Pure data-driven parser with strict resource quotas. |
| **Health Data Extraction at Rest** | Transparent file-level encryption for LittleFS health database. | AES-128-XTS encrypted partitions keyed via CryptoCell-312. |
| **Crash Loop Exploitation** | Automatic fallback to safe mode after 3 consecutive crashes; disables radios. | Boot Manager non-volatile crash accounting & watchdog. |

---

## 2. Secure Boot Chain (MCUboot)

```
[ Power-On / Hardware Reset ]
              |
              v
[ MCUboot Bootloader (Write-Protected Internal Flash) ]
              |
              v
[ Read Slot 0 (Active) & Slot 1 (Staged) Image Headers ]
              |
      Is Slot 1 Staged?
         /         \
       YES          NO
       /             \
[ Verify ECDSA-P256 ]  [ Verify Slot 0 Signature ]
[ Check Version > Current ]       |
       |                          v
[ Swap Slot 1 <-> Slot 0 ]    [ Authenticated? ]
       |                          |       \
       +------------>-------------+       FAIL
                                  |         \
                                 PASS     [ Halt / Recovery Mode ]
                                  |
                                  v
                    [ Boot Application Core Image ]
```
