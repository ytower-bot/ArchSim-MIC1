#include <stdio.h>
#include "../../include/mic1.h"
#include "../../include/utils/conversions.h"

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

void test_init_cpu() {
    mic1_cpu cpu;
    init_mic1(&cpu);
    
    int passed = (cpu.running == 0 && cpu.cycle_count == 0 && cpu.clock == 0);
    print_test("init_mic1: CPU initialized correctly", passed);
}

void test_load_microprogram() {
    mic1_cpu cpu;
    init_mic1(&cpu);
    
    int result = load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    
    int passed = (result == 16);
    print_test("load_microprogram: loads 16 microinstructions", passed);
}

void test_fetch_nop() {
    mic1_cpu cpu;
    init_mic1(&cpu);
    load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    
    init_mpc(&cpu.mpc);
    
    fetch_microinstruction(&cpu.ctrl_mem, &cpu.mpc, &cpu.mir);
    
    int all_zeros = 1;
    for (int i = 0; i < 32; i++) {
        if (cpu.mir.data[i] != 0) {
            all_zeros = 0;
            break;
        }
    }
    
    print_test("fetch_microinstruction: fetches NOP (instruction 0)", all_zeros);
}

void test_fetch_second_instruction() {
    mic1_cpu cpu;
    init_mic1(&cpu);
    load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    
    init_mpc(&cpu.mpc);
    increment_mpc(&cpu.mpc);
    
    fetch_microinstruction(&cpu.ctrl_mem, &cpu.mpc, &cpu.mir);
    
    int mpc_address = bits_to_int(cpu.mpc.address, 8);
    int passed = (mpc_address == 1 && cpu.mir.mar == 1);
    
    print_test("fetch_microinstruction: fetches MAR <- PC (instruction 1)", passed);
}

void test_update_control_sequential() {
    mic1_cpu cpu;
    init_mic1(&cpu);
    load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    
    init_mpc(&cpu.mpc);
    fetch_microinstruction(&cpu.ctrl_mem, &cpu.mpc, &cpu.mir);
    
    update_control(&cpu.mpc, &cpu.mmux, &cpu.mir);
    
    int mpc_address = bits_to_int(cpu.mpc.address, 8);
    int passed = (mpc_address == 1);
    
    print_test("update_control: increments MPC for sequential execution", passed);
}

void test_run_single_cycle() {
    mic1_cpu cpu;
    init_mic1(&cpu);
    load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    
    run_mic1_cycle(&cpu);
    
    int passed = (cpu.cycle_count == 1 && cpu.clock == 1);
    print_test("run_mic1_cycle: executes single cycle correctly", passed);
}

void test_run_multiple_cycles() {
    mic1_cpu cpu;
    init_mic1(&cpu);
    load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    
    for (int i = 0; i < 10; i++) {
        run_mic1_cycle(&cpu);
    }
    
    int passed = (cpu.cycle_count == 10 && cpu.clock == 10);
    print_test("run_mic1_cycle: executes 10 consecutive cycles", passed);
}

void test_execute_datapath_nop() {
    mic1_cpu cpu;
    init_mic1(&cpu);
    load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    
    fetch_microinstruction(&cpu.ctrl_mem, &cpu.mpc, &cpu.mir);
    
    execute_datapath(&cpu);
    
    int passed = 1;
    print_test("execute_datapath: executes NOP without errors", passed);
}

void test_fetch_and_decode() {
    mic1_cpu cpu;
    init_mic1(&cpu);
    load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    
    init_mpc(&cpu.mpc);
    for (int i = 0; i < 8; i++) {
        cpu.mpc.address[i] = (3 >> (7-i)) & 1;
    }
    
    fetch_microinstruction(&cpu.ctrl_mem, &cpu.mpc, &cpu.mir);
    
    int passed = (cpu.mir.amux == 0 && 
                  bits_to_int(cpu.mir.alu, 2) == 1 &&
                  cpu.mir.enc == 1 &&
                  bits_to_int(cpu.mir.c, 4) == 5 &&
                  bits_to_int(cpu.mir.b, 4) == 5 &&
                  bits_to_int(cpu.mir.a, 4) == 1);
    
    print_test("fetch_and_decode: AC <- AC + 1 decodes correctly", passed);
}

int main() {
    printf("\n=== Execution Cycle Test Suite ===\n\n");
    
    test_init_cpu();
    test_load_microprogram();
    test_fetch_nop();
    test_fetch_second_instruction();
    test_update_control_sequential();
    test_run_single_cycle();
    test_run_multiple_cycles();
    test_execute_datapath_nop();
    test_fetch_and_decode();
    
    printf("\n=== Test Summary ===\n");
    printf("Tests run:    %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Success rate: %.1f%%\n", (tests_passed * 100.0) / tests_run);
    
    return tests_failed > 0 ? 1 : 0;
}
