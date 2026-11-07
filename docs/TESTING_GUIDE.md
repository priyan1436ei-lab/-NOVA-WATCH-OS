# NOVA WATCH OS - Quality Assurance and Testing Guide

**Version:** 1.0.0  
**Testing Strategy:** Multi-Tiered Verification (Host Unit Tests, RTOS Simulation, Hardware-in-the-Loop)  

---

## 1. Testing Pyramid

```
                / \
               /   \      Hardware-in-the-Loop (HIL)
              / HIL \     - Real nRF5340-DK + PPK2 Power Profiler
             /-------\
            /   INT   \   RTOS Integration Tests
           /           \  - Zephyr native_sim & twister
          /-------------\
         /     UNIT      \ Host Unit Tests (C/C++ & Python)
        /                 \ - Boot Manager, HAL Mocks, Diagnostics, Power
       +-------------------+
```

---

## 2. Test Execution Commands

### Tier 1: Host C Unit Test Suite
* **Tooling:** Clang / GCC via Ninja / CMake
* **Target:** Workstation Host (x86_64)
* **Command:** `cd firmware; .\run_tests.ps1`
* **Coverage:** 16/16 tests covering Boot Manager, HAL, Diagnostics, and Power Manager.

### Tier 2: Python Subsystem & Memory Budget Verification
* **Tooling:** `pytest`
* **Target:** Architectural constraints & calculation routines
* **Command:** `pytest firmware/tests/test_firmware_python.py -v`
* **Coverage:** 5/5 verification tests (SoC interpolation, crash loops, RAM/Flash budgets).

### Tier 3: Zephyr Twister Test Runner (Physical / QEMU)
* **Tooling:** Zephyr `twister` test harness
* **Target:** `nrf5340dk/nrf5340/cpuapp` and `native_sim`
* **Command:** `west twister -T tests -p native_sim`
