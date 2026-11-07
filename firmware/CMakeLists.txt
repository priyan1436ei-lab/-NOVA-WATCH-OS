cmake_minimum_required(VERSION 3.20.0)

# Check if Zephyr environment is present
if(DEFINED ENV{ZEPHYR_BASE})
    find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
    project(nova_watch_os)

    target_include_directories(app PRIVATE include)
    target_sources(app PRIVATE
        src/main.c
        src/hal/hal_display_mock.c
        src/hal/hal_touch_mock.c
        src/hal/hal_sensor_mock.c
        src/hal/hal_power_mock.c
        src/hal/hal_storage_mock.c
        src/hal/hal_audio_mock.c
        src/hal/hal_haptic_mock.c
        src/hal/hal_ble_mock.c
        src/hal/hal_zephyr.c
        src/modules/boot_manager.c
        src/modules/diagnostics_manager.c
        src/modules/power_manager.c
        src/modules/hal_manager.c
    )
else()
    # Standalone Host Build & Test Harness
    project(nova_watch_os C)

    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)

    # Enable all warnings
    if(MSVC)
        add_compile_options(/W4 /WX-)
    else()
        add_compile_options(-Wall -Wextra -Wno-unused-parameter)
    endif()

    include_directories(include)

    # Core Library
    add_library(nova_core STATIC
        src/hal/hal_display_mock.c
        src/hal/hal_touch_mock.c
        src/hal/hal_sensor_mock.c
        src/hal/hal_power_mock.c
        src/hal/hal_storage_mock.c
        src/hal/hal_audio_mock.c
        src/hal/hal_haptic_mock.c
        src/hal/hal_ble_mock.c
        src/hal/hal_zephyr.c
        src/modules/boot_manager.c
        src/modules/diagnostics_manager.c
        src/modules/power_manager.c
        src/modules/hal_manager.c
    )

    # Firmware Simulator Binary
    add_executable(nova_firmware_sim src/main.c)
    target_link_libraries(nova_firmware_sim PRIVATE nova_core)

    # Unit Tests
    enable_testing()
    add_subdirectory(tests)
endif()

