/**
 * ALU Operation Tests
 * Tests the 4 ALU operations: A+B, A AND B, pass A, NOT A
 */

#include "../test_framework.h"
#include "../../include/alu.h"
#include <stdint.h>
#include <string.h>

// Helper: Convert uint16_t to bit array (MSB first)
void uint16_to_bits(uint16_t val, int bits[16]) {
    for (int i = 0; i < 16; i++) {
        bits[i] = (val >> (15 - i)) & 1;
    }
}

// Helper: Convert bit array to uint16_t
uint16_t bits_to_uint16(int bits[16]) {
    uint16_t val = 0;
    for (int i = 0; i < 16; i++) {
        val = (val << 1) | bits[i];
    }
    return val;
}

TEST_CASE(alu_operation_add_simple) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(5, input_a);
    uint16_to_bits(3, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_A_PLUS_B);
    run_alu(&a);

    uint16_t result = bits_to_uint16(a.output);
    ASSERT_EQUAL(8, result);
    return 0;
}

TEST_CASE(alu_operation_add_large) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(1000, input_a);
    uint16_to_bits(2000, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_A_PLUS_B);
    run_alu(&a);

    uint16_t result = bits_to_uint16(a.output);
    ASSERT_EQUAL(3000, result);
    return 0;
}

TEST_CASE(alu_operation_and) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(0xFF00, input_a);
    uint16_to_bits(0x0F0F, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_A_AND_B);
    run_alu(&a);

    uint16_t result = bits_to_uint16(a.output);
    ASSERT_EQUAL(0x0F00, result);
    return 0;
}

TEST_CASE(alu_operation_pass_a) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(0xABCD, input_a);
    uint16_to_bits(0x1234, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_A);
    run_alu(&a);

    uint16_t result = bits_to_uint16(a.output);
    ASSERT_EQUAL(0xABCD, result);
    return 0;
}

TEST_CASE(alu_operation_not_a) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(0xFF00, input_a);
    uint16_to_bits(0x0000, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_NOT_A);
    run_alu(&a);

    uint16_t result = bits_to_uint16(a.output);
    ASSERT_EQUAL(0x00FF, result);
    return 0;
}

TEST_SUITE_BEGIN("ALU Operations")
    RUN_TEST(alu_operation_add_simple);
    RUN_TEST(alu_operation_add_large);
    RUN_TEST(alu_operation_and);
    RUN_TEST(alu_operation_pass_a);
    RUN_TEST(alu_operation_not_a);
TEST_SUITE_END()
