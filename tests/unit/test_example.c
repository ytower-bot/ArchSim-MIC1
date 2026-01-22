/**
 * Example Test Suite
 *
 * Demonstrates the MIC1-Test framework usage.
 * Delete this file after creating real unit tests.
 */

#include "../test_framework.h"

/* Example: Basic arithmetic tests */
TEST_CASE(arithmetic_addition) {
    int result = 2 + 2;
    ASSERT_EQUAL(4, result);
    return 0;
}

TEST_CASE(arithmetic_subtraction) {
    int result = 10 - 3;
    ASSERT_EQUAL(7, result);
    return 0;
}

/* Example: Boolean tests */
TEST_CASE(boolean_true) {
    int value = 1;
    ASSERT_TRUE(value == 1);
    ASSERT_FALSE(value == 0);
    return 0;
}

/* Example: Pointer tests */
TEST_CASE(pointer_null_check) {
    int* null_ptr = NULL;
    int value = 42;
    int* valid_ptr = &value;

    ASSERT_NULL(null_ptr);
    ASSERT_NOT_NULL(valid_ptr);
    ASSERT_EQUAL(42, *valid_ptr);
    return 0;
}

/* Example: String tests */
TEST_CASE(string_comparison) {
    const char* str1 = "MIC-1";
    const char* str2 = "MIC-1";

    ASSERT_STR_EQUAL(str1, str2);
    return 0;
}

/* Test suite runner */
TEST_SUITE_BEGIN("Example Test Suite")
    RUN_TEST(arithmetic_addition);
    RUN_TEST(arithmetic_subtraction);
    RUN_TEST(boolean_true);
    RUN_TEST(pointer_null_check);
    RUN_TEST(string_comparison);
TEST_SUITE_END()
