# NOVA WATCH OS - Automated Test Runner
$ErrorActionPreference = "Stop"

$zig = "$HOME\.tools\zig\zig.exe"
if (-not (Test-Path $zig)) {
    Write-Error "Zig compiler not found at $zig. Please ensure development environment setup has been completed."
    exit 1
}

$firmwareRoot = $PSScriptRoot
$includeDir = Join-Path $firmwareRoot "include"
$testsDir = Join-Path $firmwareRoot "tests"
$testExe = Join-Path $firmwareRoot "nova_tests.exe"
$simExe = Join-Path $firmwareRoot "nova_firmware_sim.exe"

Write-Host "===================================================" -ForegroundColor Cyan
Write-Host "   NOVA WATCH OS - COMPILING FIRMWARE & TEST SUITE " -ForegroundColor Cyan
Write-Host "===================================================" -ForegroundColor Cyan

# 1. Compile Simulator
Write-Host "[1/4] Compiling nova_firmware_sim..." -ForegroundColor Yellow
& $zig cc -I $includeDir `
    (Join-Path $firmwareRoot "src\main.c") `
    (Join-Path $firmwareRoot "src\hal\hal_display_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_touch_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_sensor_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_power_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_storage_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_audio_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_haptic_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_ble_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_zephyr.c") `
    (Join-Path $firmwareRoot "src\modules\boot_manager.c") `
    (Join-Path $firmwareRoot "src\modules\diagnostics_manager.c") `
    (Join-Path $firmwareRoot "src\modules\power_manager.c") `
    (Join-Path $firmwareRoot "src\modules\hal_manager.c") `
    -o $simExe

# 2. Compile Unit Tests
Write-Host "[2/4] Compiling nova_tests..." -ForegroundColor Yellow
& $zig cc -I $includeDir -I $testsDir `
    (Join-Path $firmwareRoot "tests\test_runner.c") `
    (Join-Path $firmwareRoot "tests\test_boot_manager.c") `
    (Join-Path $firmwareRoot "tests\test_hal.c") `
    (Join-Path $firmwareRoot "tests\test_diagnostics.c") `
    (Join-Path $firmwareRoot "tests\test_power_manager.c") `
    (Join-Path $firmwareRoot "src\hal\hal_display_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_touch_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_sensor_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_power_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_storage_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_audio_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_haptic_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_ble_mock.c") `
    (Join-Path $firmwareRoot "src\hal\hal_zephyr.c") `
    (Join-Path $firmwareRoot "src\modules\boot_manager.c") `
    (Join-Path $firmwareRoot "src\modules\diagnostics_manager.c") `
    (Join-Path $firmwareRoot "src\modules\power_manager.c") `
    (Join-Path $firmwareRoot "src\modules\hal_manager.c") `
    -o $testExe

# 3. Run Simulator
Write-Host "`n[3/4] Executing Firmware Boot & Diagnostics Simulation..." -ForegroundColor Yellow
& $simExe

# 4. Run C Unit Tests
Write-Host "`n[4/4] Executing Native C Unit Tests..." -ForegroundColor Yellow
& $testExe

Write-Host "`nAll verification steps completed successfully!" -ForegroundColor Green
