#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t tests_run;
    uint32_t tests_passed;
    uint32_t tests_failed;
} test_suite_result_t;

extern test_suite_result_t g_test_results;

#define TEST_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        printf("  [FAIL] %s:%d: %s (%s)\n", __FILE__, __LINE__, msg, #cond); \
        g_test_results.tests_failed++; \
        return; \
    } \
} while(0)

#define TEST_ASSERT_EQUAL(expected, actual, msg) do { \
    if ((expected) != (actual)) { \
        printf("  [FAIL] %s:%d: %s (expected %lld, got %lld)\n", __FILE__, __LINE__, msg, (long long)(expected), (long long)(actual)); \
        g_test_results.tests_failed++; \
        return; \
    } \
} while(0)

#define RUN_TEST(test_func) do { \
    printf("Running %s...\n", #test_func); \
    g_test_results.tests_run++; \
    uint32_t prev_failed = g_test_results.tests_failed; \
    test_func(); \
    if (g_test_results.tests_failed == prev_failed) { \
        printf("  [PASS] %s\n", #test_func); \
        g_test_results.tests_passed++; \
    } \
} while(0)

void run_boot_manager_tests(void);
void run_hal_tests(void);
void run_diagnostics_tests(void);
void run_power_manager_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_RUNNER_H */
