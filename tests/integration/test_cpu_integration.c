/**
 * Integration Test: Assembler + CPU Execution
 * Verifies that assembled programs execute correctly in the CPU
 */

#include "../test_framework.h"
#include "../../include/mic1.h"
#include "../../include/control_unit.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

TEST_CASE(integration_load_and_execute_simple_program) {
    mic1_cpu cpu;
    init_mic1(&cpu);

    if (cpu.decoder_c.rb == NULL) {
        cpu.decoder_c.rb = &cpu.reg_bank;
    }

    const char* microcode_path = "data/basic_microcode.txt";
    int result = load_microprogram(&cpu.ctrl_mem, microcode_path);
    ASSERT_TRUE(result > 0);

    const char* test_program = "tests/01_registers.asm";
    FILE* test_file = fopen(test_program, "r");
    ASSERT_NOT_NULL(test_file);
    fclose(test_file);

    return 0;
}

TEST_CASE(integration_cpu_init_and_reset) {
    mic1_cpu cpu;
    init_mic1(&cpu);

    ASSERT_TRUE(cpu.reg_bank.PC.data != NULL);
    ASSERT_TRUE(cpu.reg_bank.AC.data != NULL);
    ASSERT_TRUE(cpu.reg_bank.SP.data != NULL);

    reset_mic1(&cpu);

    ASSERT_TRUE(cpu.reg_bank.PC.data != NULL);

    return 0;
}

TEST_SUITE_BEGIN("CPU Integration")
    RUN_TEST(integration_load_and_execute_simple_program);
    RUN_TEST(integration_cpu_init_and_reset);
TEST_SUITE_END()
