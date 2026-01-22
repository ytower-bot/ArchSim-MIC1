/**
 * ALU Flag Tests
 * Tests N (negative) and Z (zero) flags
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

TEST_CASE(alu_flag_zero_when_result_is_zero) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(0, input_a);
    uint16_to_bits(0, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_A_PLUS_B);
    run_alu(&a);

    ASSERT_EQUAL(1, a.flag_z);
    ASSERT_EQUAL(0, a.flag_n);
    return 0;
}

TEST_CASE(alu_flag_zero_not_set_when_nonzero) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(5, input_a);
    uint16_to_bits(3, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_A_PLUS_B);
    run_alu(&a);

    ASSERT_EQUAL(0, a.flag_z);
    return 0;
}

TEST_CASE(alu_flag_negative_when_msb_set) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(0x8000, input_a);
    uint16_to_bits(0, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_A);
    run_alu(&a);

    ASSERT_EQUAL(1, a.flag_n);
    ASSERT_EQUAL(0, a.flag_z);
    return 0;
}

TEST_CASE(alu_flag_negative_not_set_when_positive) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(0x7FFF, input_a);
    uint16_to_bits(0, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_A);
    run_alu(&a);

    ASSERT_EQUAL(0, a.flag_n);
    ASSERT_EQUAL(0, a.flag_z);
    return 0;
}

TEST_CASE(alu_flag_both_zero_and_negative) {
    alu a;
    init_alu(&a);

    int input_a[16], input_b[16];
    uint16_to_bits(0, input_a);
    uint16_to_bits(0, input_b);

    set_alu_inputs(&a, input_a, input_b);
    set_alu_control(&a, ALU_A_AND_B);
    run_alu(&a);

    ASSERT_EQUAL(1, a.flag_z);
    ASSERT_EQUAL(0, a.flag_n);
    return 0;
}

TEST_SUITE_BEGIN("ALU Flags")
    RUN_TEST(alu_flag_zero_when_result_is_zero);
    RUN_TEST(alu_flag_zero_not_set_when_nonzero);
    RUN_TEST(alu_flag_negative_when_msb_set);
    RUN_TEST(alu_flag_negative_not_set_when_positive);
    RUN_TEST(alu_flag_both_zero_and_negative);
TEST_SUITE_END()
