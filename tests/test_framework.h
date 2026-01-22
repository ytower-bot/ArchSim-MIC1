/**
 * MIC1-Test: Simple C99 Test Framework
 *
 * Zero external dependencies, designed for educational projects.
 * Provides basic assertion macros and test runner utilities.
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test statistics */
typedef struct {
    int tests_run;
    int tests_passed;
    int tests_failed;
    const char* current_test;
} test_stats_t;

static test_stats_t g_test_stats = {0, 0, 0, NULL};

/* Color output for terminals (can be disabled with NO_COLOR env var) */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"

#define TEST_PRINTF(color, ...) do { \
    if (getenv("NO_COLOR") == NULL) printf(color); \
    printf(__VA_ARGS__); \
    if (getenv("NO_COLOR") == NULL) printf(COLOR_RESET); \
} while(0)

/* Assertion macros */
#define ASSERT_TRUE(condition) do { \
    if (!(condition)) { \
        TEST_PRINTF(COLOR_RED, "[FAIL] %s:%d: Expected TRUE, got FALSE\n", __FILE__, __LINE__); \
        TEST_PRINTF(COLOR_RED, "       Expression: %s\n", #condition); \
        g_test_stats.tests_failed++; \
        return -1; \
    } \
} while(0)

#define ASSERT_FALSE(condition) do { \
    if (condition) { \
        TEST_PRINTF(COLOR_RED, "[FAIL] %s:%d: Expected FALSE, got TRUE\n", __FILE__, __LINE__); \
        TEST_PRINTF(COLOR_RED, "       Expression: %s\n", #condition); \
        g_test_stats.tests_failed++; \
        return -1; \
    } \
} while(0)

#define ASSERT_EQUAL(expected, actual) do { \
    if ((expected) != (actual)) { \
        TEST_PRINTF(COLOR_RED, "[FAIL] %s:%d: Expected %d, got %d\n", __FILE__, __LINE__, (int)(expected), (int)(actual)); \
        TEST_PRINTF(COLOR_RED, "       Expression: %s == %s\n", #expected, #actual); \
        g_test_stats.tests_failed++; \
        return -1; \
    } \
} while(0)

#define ASSERT_NOT_EQUAL(expected, actual) do { \
    if ((expected) == (actual)) { \
        TEST_PRINTF(COLOR_RED, "[FAIL] %s:%d: Expected values to differ, both are %d\n", __FILE__, __LINE__, (int)(actual)); \
        TEST_PRINTF(COLOR_RED, "       Expression: %s != %s\n", #expected, #actual); \
        g_test_stats.tests_failed++; \
        return -1; \
    } \
} while(0)

#define ASSERT_STR_EQUAL(expected, actual) do { \
    if (strcmp((expected), (actual)) != 0) { \
        TEST_PRINTF(COLOR_RED, "[FAIL] %s:%d: String mismatch\n", __FILE__, __LINE__); \
        TEST_PRINTF(COLOR_RED, "       Expected: \"%s\"\n", (expected)); \
        TEST_PRINTF(COLOR_RED, "       Got:      \"%s\"\n", (actual)); \
        g_test_stats.tests_failed++; \
        return -1; \
    } \
} while(0)

#define ASSERT_NULL(ptr) do { \
    if ((ptr) != NULL) { \
        TEST_PRINTF(COLOR_RED, "[FAIL] %s:%d: Expected NULL, got %p\n", __FILE__, __LINE__, (void*)(ptr)); \
        TEST_PRINTF(COLOR_RED, "       Expression: %s\n", #ptr); \
        g_test_stats.tests_failed++; \
        return -1; \
    } \
} while(0)

#define ASSERT_NOT_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        TEST_PRINTF(COLOR_RED, "[FAIL] %s:%d: Expected non-NULL pointer\n", __FILE__, __LINE__); \
        TEST_PRINTF(COLOR_RED, "       Expression: %s\n", #ptr); \
        g_test_stats.tests_failed++; \
        return -1; \
    } \
} while(0)

/* Test case definition */
#define TEST_CASE(name) \
    static int test_##name(void); \
    static int run_test_##name(void) { \
        g_test_stats.current_test = #name; \
        g_test_stats.tests_run++; \
        TEST_PRINTF(COLOR_CYAN, "[TEST] %s\n", #name); \
        int result = test_##name(); \
        if (result == 0) { \
            TEST_PRINTF(COLOR_GREEN, "[PASS] %s\n", #name); \
            g_test_stats.tests_passed++; \
        } \
        return result; \
    } \
    static int test_##name(void)

/* Test runner */
#define RUN_TEST(name) do { \
    run_test_##name(); \
} while(0)

/* Test suite initialization and summary */
#define TEST_SUITE_BEGIN(suite_name) \
    int main(void) { \
        TEST_PRINTF(COLOR_YELLOW, "\n=== Test Suite: %s ===\n\n", suite_name); \
        g_test_stats.tests_run = 0; \
        g_test_stats.tests_passed = 0; \
        g_test_stats.tests_failed = 0;

#define TEST_SUITE_END() \
        TEST_PRINTF(COLOR_YELLOW, "\n=== Test Summary ===\n"); \
        printf("Total:  %d\n", g_test_stats.tests_run); \
        TEST_PRINTF(COLOR_GREEN, "Passed: %d\n", g_test_stats.tests_passed); \
        if (g_test_stats.tests_failed > 0) { \
            TEST_PRINTF(COLOR_RED, "Failed: %d\n", g_test_stats.tests_failed); \
        } else { \
            printf("Failed: 0\n"); \
        } \
        printf("\n"); \
        return (g_test_stats.tests_failed > 0) ? EXIT_FAILURE : EXIT_SUCCESS; \
    }

#endif /* TEST_FRAMEWORK_H */
