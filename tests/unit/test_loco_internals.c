/*
 * test_loco_internals.c - Unit tests for LOCO instruction hardware
 *
 * Purpose: Isolate and test the specific hardware operations required by LOCO
 *          instruction microcode to prove/disprove correct behavior.
 *
 * Test Strategy:
 *   1. Test shifter operations (left shift for extracting 12-bit constant)
 *   2. Test ALU AND operation (masking with AMASK)
 *   3. Simulate the exact microcode sequence for LOCO 2 (0x7002)
 *
 * Expected Behavior (per Tanenbaum MIC-1 specification):
 *   - LOCO instruction: AC ← IR AND AMASK
 *   - IR = 0x7002 (opcode 0x7 in bits [15:12], operand 0x002 in bits [11:0])
 *   - AMASK = 0x0FFF (masks lower 12 bits)
 *   - Expected result: AC = 0x0002
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../../include/shifter.h"
#include "../../include/alu.h"
#include "../../include/datapath.h"
#include "../../include/utils/conversions.h"

/* Test result tracking */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf("  [PASS] %s\n", message); \
        } else { \
            tests_failed++; \
            printf("  [FAIL] %s\n", message); \
        } \
    } while (0)

#define TEST_SECTION(name) \
    printf("\n=== TEST SECTION: %s ===\n", name)

/* Helper: Convert bit array to hex string for debugging */
void print_bits_hex(int bits[16], const char* label) {
    int value = array_to_int(bits);
    printf("  %s: 0x%04X (", label, value & 0xFFFF);
    for (int i = 0; i < 16; i++) {
        printf("%d", bits[i]);
    }
    printf(")\n");
}

/*
 * TEST 1: Shifter Control Signal Decoding
 *
 * Per MIC-1 spec:
 *   SH[1:0] = 00 → No shift
 *   SH[1:0] = 01 → Right shift by 1 (arithmetic)
 *   SH[1:0] = 10 → Left shift by 8
 *   SH[1:0] = 11 → Unused
 */
void test_shifter_control_signals() {
    TEST_SECTION("Shifter Control Signal Decoding");

    shifter s;
    init_shifter(&s);

    /* Test 1a: Control bits interpretation */
    s.control_sh[0] = 0;
    s.control_sh[1] = 0;
    int ctrl_00 = (s.control_sh[1] << 1) | s.control_sh[0];
    TEST_ASSERT(ctrl_00 == SHIFT_NONE, "SH=00 decodes to SHIFT_NONE");

    s.control_sh[0] = 1;
    s.control_sh[1] = 0;
    int ctrl_01 = (s.control_sh[1] << 1) | s.control_sh[0];
    TEST_ASSERT(ctrl_01 == SHIFT_RIGHT, "SH=01 decodes to SHIFT_RIGHT");

    s.control_sh[0] = 0;
    s.control_sh[1] = 1;
    int ctrl_10 = (s.control_sh[1] << 1) | s.control_sh[0];
    TEST_ASSERT(ctrl_10 == SHIFT_LEFT, "SH=10 decodes to SHIFT_LEFT");
}

/*
 * TEST 2: Shifter Left Shift Operation
 *
 * For LOCO, we need to verify left shift works correctly.
 * The microcode doesn't actually use left shift for LOCO,
 * but let's test the basic shift operations.
 */
void test_shifter_left_shift() {
    TEST_SECTION("Shifter Left Shift Operation");

    shifter s;
    init_shifter(&s);

    /* Test 2a: Simple left shift of 0x0001 → 0x0002 */
    int_to_array(0x0001, s.data);
    printf("\n  Before shift:\n");
    print_bits_hex(s.data, "Input");

    lshift(&s);

    printf("  After lshift():\n");
    print_bits_hex(s.data, "Output");

    int result = array_to_int(s.data);
    TEST_ASSERT(result == 0x0002, "lshift(0x0001) = 0x0002");

    /* Test 2b: Left shift 0x7002 */
    int_to_array(0x7002, s.data);
    printf("\n  Before shift:\n");
    print_bits_hex(s.data, "Input");

    lshift(&s);

    printf("  After lshift():\n");
    print_bits_hex(s.data, "Output");

    result = array_to_int(s.data);
    TEST_ASSERT(result == 0xE004 || result == -8188,
                "lshift(0x7002) produces correct result");
}

/*
 * TEST 3: Shifter Right Shift Operation
 *
 * Per MIC-1 spec, this is arithmetic right shift (SRA1)
 */
void test_shifter_right_shift() {
    TEST_SECTION("Shifter Right Shift Operation");

    shifter s;
    init_shifter(&s);

    /* Test 3a: Simple right shift 0x0004 → 0x0002 */
    int_to_array(0x0004, s.data);
    printf("\n  Before shift:\n");
    print_bits_hex(s.data, "Input");

    rshift(&s);

    printf("  After rshift():\n");
    print_bits_hex(s.data, "Output");

    int result = array_to_int(s.data);
    TEST_ASSERT(result == 0x0002, "rshift(0x0004) = 0x0002");

    /* Test 3b: Right shift with sign extension */
    int_to_array(-4, s.data); // 0xFFFC in 16-bit
    printf("\n  Before shift:\n");
    print_bits_hex(s.data, "Input (negative)");

    rshift(&s);

    printf("  After rshift():\n");
    print_bits_hex(s.data, "Output");

    result = array_to_int(s.data);
    /* Arithmetic right shift should preserve sign bit */
    TEST_ASSERT(result < 0, "rshift maintains sign for negative numbers");
}

/*
 * TEST 4: ALU AND Operation
 *
 * LOCO uses: AC ← IR AND AMASK
 * IR = 0x7002, AMASK = 0x0FFF
 * Expected: 0x7002 & 0x0FFF = 0x0002
 */
void test_alu_and_operation() {
    TEST_SECTION("ALU AND Operation");

    alu a;
    init_alu(&a);

    /* Test 4a: IR AND AMASK for LOCO 2 */
    int ir_value[16];
    int amask_value[16];

    int_to_array(0x7002, ir_value);  /* IR = 0x7002 */
    int_to_array(0x0FFF, amask_value); /* AMASK = 0x0FFF */

    printf("\n  Input A (IR):\n");
    print_bits_hex(ir_value, "IR");
    printf("  Input B (AMASK):\n");
    print_bits_hex(amask_value, "AMASK");

    set_alu_inputs(&a, ir_value, amask_value);
    set_alu_control(&a, ALU_A_AND_B);
    run_alu(&a);

    printf("  Output:\n");
    print_bits_hex(a.output, "Result");

    int result = array_to_int(a.output);
    TEST_ASSERT(result == 0x0002,
                "ALU: 0x7002 AND 0x0FFF = 0x0002");

    /* Test 4b: Verify flags */
    TEST_ASSERT(a.flag_z == 0, "Result is not zero (flag_z = 0)");
    TEST_ASSERT(a.flag_n == 0, "Result is positive (flag_n = 0)");
}

/*
 * TEST 5: Complete LOCO Simulation
 *
 * Simulate the exact hardware flow:
 *   1. IR contains 0x7002
 *   2. Decoder A selects IR
 *   3. Decoder B selects AMASK
 *   4. ALU performs IR AND AMASK
 *   5. Shifter passes through (no shift)
 *   6. Result should be 0x0002
 */
void test_complete_loco_simulation() {
    TEST_SECTION("Complete LOCO Instruction Simulation");

    /* Setup register bank */
    register_bank rb;
    init_register_bank(&rb);

    /* Load IR with 0x7002 (LOCO 2 instruction) */
    int_to_array(0x7002, rb.IR.data);
    printf("\n  Initial state:\n");
    print_bits_hex(rb.IR.data, "IR (instruction)");
    print_bits_hex(rb.AMASK.data, "AMASK (mask)");

    /* Setup decoders */
    decoder dec_a;
    decoder dec_b;
    latch latch_a, latch_b;

    init_decoder(&dec_a, &rb);
    init_decoder(&dec_b, &rb);

    /* Configure decoder A to select IR (register 2) */
    dec_a.control[0] = 0; /* bit 0 */
    dec_a.control[1] = 1; /* bit 1 */
    dec_a.control[2] = 0; /* bit 2 */
    dec_a.control[3] = 0; /* bit 3 */
    /* Binary: 0010 = 2 (IR) */

    /* Configure decoder B to select AMASK (register 5) */
    dec_b.control[0] = 1; /* bit 0 */
    dec_b.control[1] = 0; /* bit 1 */
    dec_b.control[2] = 1; /* bit 2 */
    dec_b.control[3] = 0; /* bit 3 */
    /* Binary: 0101 = 5 (AMASK) */

    /* Run decoders */
    run_decoder(&dec_a, &latch_a);
    run_decoder(&dec_b, &latch_b);

    printf("\n  After decoder selection:\n");
    print_bits_hex(latch_a.data, "Latch A (from IR)");
    print_bits_hex(latch_b.data, "Latch B (from AMASK)");

    /* Setup ALU */
    alu a;
    init_alu(&a);
    set_alu_inputs(&a, latch_a.data, latch_b.data);
    set_alu_control(&a, ALU_A_AND_B);
    run_alu(&a);

    printf("\n  After ALU (AND operation):\n");
    print_bits_hex(a.output, "ALU output");

    /* Setup shifter (no shift for LOCO) */
    shifter s;
    init_shifter(&s);
    copy_array(a.output, s.data);
    s.control_sh[0] = 0; /* No shift */
    s.control_sh[1] = 0;

    /* Note: run_shifter modifies shifter->data in place */
    run_shifter(&s, NULL, NULL);

    printf("\n  After shifter (no shift):\n");
    print_bits_hex(s.data, "Shifter output");

    /* This should be written to AC */
    int final_result = array_to_int(s.data);

    printf("\n  FINAL RESULT: 0x%04X (decimal: %d)\n",
           final_result & 0xFFFF, final_result);

    TEST_ASSERT(final_result == 0x0002,
                "Complete LOCO 2 simulation produces AC = 0x0002");
}

/*
 * TEST 6: Edge Cases
 */
void test_edge_cases() {
    TEST_SECTION("Edge Cases");

    alu a;
    init_alu(&a);

    /* Test 6a: LOCO 0 */
    int ir_zero[16];
    int amask[16];
    int_to_array(0x7000, ir_zero);  /* LOCO 0 */
    int_to_array(0x0FFF, amask);

    set_alu_inputs(&a, ir_zero, amask);
    set_alu_control(&a, ALU_A_AND_B);
    run_alu(&a);

    int result = array_to_int(a.output);
    TEST_ASSERT(result == 0x0000 && a.flag_z == 1,
                "LOCO 0 produces zero with Z flag set");

    /* Test 6b: LOCO with max 12-bit value (0xFFF = 4095) */
    int ir_max[16];
    int_to_array(0x7FFF, ir_max);  /* LOCO 4095 */

    set_alu_inputs(&a, ir_max, amask);
    set_alu_control(&a, ALU_A_AND_B);
    run_alu(&a);

    result = array_to_int(a.output);
    TEST_ASSERT(result == 0x0FFF,
                "LOCO 4095 produces maximum 12-bit value");
}

/*
 * Main test runner
 */
int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  LOCO INSTRUCTION HARDWARE UNIT TESTS                      ║\n");
    printf("║  Testing: Shifter, ALU, and Complete LOCO Datapath       ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    test_shifter_control_signals();
    test_shifter_left_shift();
    test_shifter_right_shift();
    test_alu_and_operation();
    test_complete_loco_simulation();
    test_edge_cases();

    /* Summary */
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TEST SUMMARY                                              ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Total:  %3d                                               ║\n", tests_run);
    printf("║  Passed: %3d                                               ║\n", tests_passed);
    printf("║  Failed: %3d                                               ║\n", tests_failed);
    printf("╠════════════════════════════════════════════════════════════╣\n");

    if (tests_failed == 0) {
        printf("║  STATUS: ✓ ALL TESTS PASSED                               ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        return 0;
    } else {
        printf("║  STATUS: ✗ SOME TESTS FAILED - DEBUG REQUIRED            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        return 1;
    }
}
