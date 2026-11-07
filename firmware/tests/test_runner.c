#include "test_runner.h"

test_suite_result_t g_test_results = {0, 0, 0};

int main(void) {
    printf("===================================================\n");
    printf("   NOVA WATCH OS - PHASE 2 UNIT TEST SUITE        \n");
    printf("===================================================\n\n");

    printf("--- [TEST SUITE 1] Boot Manager ---\n");
    run_boot_manager_tests();
    printf("\n");

    printf("--- [TEST SUITE 2] Hardware Abstraction Layer (HAL) ---\n");
    run_hal_tests();
    printf("\n");

    printf("--- [TEST SUITE 3] Diagnostics & BIST Manager ---\n");
    run_diagnostics_tests();
    printf("\n");

    printf("--- [TEST SUITE 4] Power & Battery Manager ---\n");
    run_power_manager_tests();
    printf("\n");

    printf("===================================================\n");
    printf("                  TEST SUMMARY                     \n");
    printf("===================================================\n");
    printf("  Total Tests Run:    %u\n", g_test_results.tests_run);
    printf("  Passed:             %u\n", g_test_results.tests_passed);
    printf("  Failed:             %u\n", g_test_results.tests_failed);
    printf("===================================================\n");

    return (g_test_results.tests_failed == 0) ? 0 : 1;
}

