#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/alu.h"

int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

void print_test(const char* name, int passed) {
    tests_run++;
    if (passed) {
        tests_passed++;
        printf("✓ PASS: %s\n", name);
    } else {
        tests_failed++;
        printf("✗ FAIL: %s\n", name);
    }
}

void print_array(const int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d", arr[i]);
    }
}

int arrays_equal(const int* a, const int* b) {
    for (int i = 0; i < 16; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

void test_add_positive() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0};
    int b[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1};
    int expected[16] = {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 0);
    run_alu(&unit);
    
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 0 && unit.flag_z == 0;
    print_test("ADD: 10 + 5 = 15", passed);
}

void test_add_zero() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    int b[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    int expected[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 0);
    run_alu(&unit);
    
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 0 && unit.flag_z == 1;
    print_test("ADD: -1 + 1 = 0 (with Z flag)", passed);
}

void test_add_negative() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1};
    int b[16] = {1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0};
    int expected[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 0);
    run_alu(&unit);
    
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 1 && unit.flag_z == 0;
    print_test("ADD: 5 + (-10) = -5 (with N flag)", passed);
}

void test_and_operation() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};
    int b[16] = {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1};
    int expected[16] = {0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 1);
    run_alu(&unit);
    
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 0 && unit.flag_z == 0;
    print_test("AND: 0xFF00 & 0x0FFF = 0x0F00", passed);
}

void test_and_zero() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0};
    int b[16] = {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1};
    int expected[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 1);
    run_alu(&unit);
    
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 0 && unit.flag_z == 1;
    print_test("AND: No common bits = 0 (with Z flag)", passed);
}

void test_pass_a() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0};
    int b[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    int expected[16] = {0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 2);
    run_alu(&unit);
    
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 0 && unit.flag_z == 0;
    print_test("PASS_A: Output = Input A", passed);
}

void test_pass_a_negative() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {1,1,1,1,1,1,1,1,1,0,0,1,1,1,0,0};
    int b[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int expected[16] = {1,1,1,1,1,1,1,1,1,0,0,1,1,1,0,0};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 2);
    run_alu(&unit);
    
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 1 && unit.flag_z == 0;
    print_test("PASS_A: Negative value (with N flag)", passed);
}

void test_not_positive() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int b[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int expected[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 3);
    run_alu(&unit);
    
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 1 && unit.flag_z == 0;
    print_test("NOT: NOT(0) = -1 (all bits set)", passed);
}

void test_not_pattern() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0};
    int b[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int expected[16] = {1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 3);
    run_alu(&unit);
    
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 1 && unit.flag_z == 0;
    print_test("NOT: NOT(10) = -11", passed);
}

void test_control_signals() {
    alu unit;
    init_alu(&unit);
    
    set_alu_control(&unit, 0);
    int passed = (unit.control[0] == 0 && unit.control[1] == 0);
    
    set_alu_control(&unit, 1);
    passed = passed && (unit.control[0] == 1 && unit.control[1] == 0);
    
    set_alu_control(&unit, 2);
    passed = passed && (unit.control[0] == 0 && unit.control[1] == 1);
    
    set_alu_control(&unit, 3);
    passed = passed && (unit.control[0] == 1 && unit.control[1] == 1);
    
    print_test("Control signal mapping (0-3)", passed);
}

void test_flag_negative() {
    alu unit;
    init_alu(&unit);
    
    int pos[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    memcpy(unit.output, pos, sizeof(pos));
    update_flags(&unit);
    int test1 = (unit.flag_n == 0);
    
    int neg[16] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    memcpy(unit.output, neg, sizeof(neg));
    update_flags(&unit);
    int test2 = (unit.flag_n == 1);
    
    int passed = test1 && test2;
    print_test("Flag N: Set on MSB", passed);
}

void test_flag_zero() {
    alu unit;
    init_alu(&unit);
    
    int zero[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    memcpy(unit.output, zero, sizeof(zero));
    update_flags(&unit);
    int test1 = (unit.flag_z == 1);
    
    int nonzero[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    memcpy(unit.output, nonzero, sizeof(nonzero));
    update_flags(&unit);
    int test2 = (unit.flag_z == 0);
    
    int passed = test1 && test2;
    print_test("Flag Z: Set on all zeros", passed);
}

void test_is_zero_function() {
    int zero[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int nonzero[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    
    int passed = (is_zero(zero) == 1) && (is_zero(nonzero) == 0);
    print_test("is_zero: Correctly identifies zero and non-zero", passed);
}

void test_is_negative_function() {
    int positive[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    int negative[16] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    int passed = (is_negative(positive) == 0) && (is_negative(negative) == 1);
    print_test("is_negative: Correctly checks MSB", passed);
}

void test_combined_workflow() {
    alu unit;
    init_alu(&unit);
    
    int a[16] = {0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0};
    int b[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1};
    
    set_alu_inputs(&unit, a, b);
    set_alu_control(&unit, 0);
    run_alu(&unit);
    
    int expected[16] = {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1};
    int passed = arrays_equal(unit.output, expected) && 
                 unit.flag_n == 0 && unit.flag_z == 0;
    
    print_test("Combined workflow: Init + Set inputs + Control + Run", passed);
}

int main() {
    printf("================================================================================\n");
    printf("ALU Unit Tests - 16-bit Arithmetic Logic Unit Implementation\n");
    printf("================================================================================\n\n");
    
    printf("--- Addition Tests ---\n");
    test_add_positive();
    test_add_zero();
    test_add_negative();
    
    printf("\n--- Bitwise AND Tests ---\n");
    test_and_operation();
    test_and_zero();
    
    printf("\n--- Pass-Through Tests ---\n");
    test_pass_a();
    test_pass_a_negative();
    
    printf("\n--- Bitwise NOT Tests ---\n");
    test_not_positive();
    test_not_pattern();
    
    printf("\n--- Control Signal & Flag Tests ---\n");
    test_control_signals();
    test_flag_negative();
    test_flag_zero();
    test_is_zero_function();
    test_is_negative_function();
    
    printf("\n--- Combined Workflow Tests ---\n");
    test_combined_workflow();
    
    printf("\n================================================================================\n");
    printf("Test Summary\n");
    printf("================================================================================\n");
    printf("Total tests run:    %d\n", tests_run);
    printf("Tests passed:       %d\n", tests_passed);
    printf("Tests failed:       %d\n", tests_failed);
    printf("Success rate:       %.1f%%\n", 
           (tests_run > 0) ? (100.0 * tests_passed / tests_run) : 0.0);
    printf("================================================================================\n");
    
    return (tests_failed == 0) ? 0 : 1;
}
