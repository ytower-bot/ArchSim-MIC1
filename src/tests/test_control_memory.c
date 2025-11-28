#include <stdio.h>
#include <string.h>
#include "../../include/control_unit.h"

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

void test_init_control_memory() {
    control_memory cm;
    init_control_memory(&cm);
    
    int all_zeros = 1;
    for (int i = 0; i < MICROPROGRAM_SIZE; i++) {
        for (int j = 0; j < 32; j++) {
            if (cm.microinstructions[i][j] != 0) {
                all_zeros = 0;
                break;
            }
        }
        if (!all_zeros) break;
    }
    
    print_test("init_control_memory: all microinstructions initialized to zero", all_zeros);
}

void test_init_control_memory_null() {
    init_control_memory(NULL);
    print_test("init_control_memory: handles NULL pointer safely", 1);
}

void test_load_microprogram_valid() {
    control_memory cm;
    init_control_memory(&cm);
    
    int result = load_microprogram(&cm, "data/basic_microcode.txt");
    
    int passed = (result == 16);
    print_test("load_microprogram: loads 16 valid microinstructions", passed);
}

void test_load_microprogram_first_instruction() {
    control_memory cm;
    init_control_memory(&cm);
    
    load_microprogram(&cm, "data/basic_microcode.txt");
    
    int all_zeros = 1;
    for (int i = 0; i < 32; i++) {
        if (cm.microinstructions[0][i] != 0) {
            all_zeros = 0;
            break;
        }
    }
    
    print_test("load_microprogram: first instruction is NOP (all zeros)", all_zeros);
}

void test_load_microprogram_second_instruction() {
    control_memory cm;
    init_control_memory(&cm);
    
    load_microprogram(&cm, "data/basic_microcode.txt");
    
    int expected[32] = {
        0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
        0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    
    int matches = 1;
    for (int i = 0; i < 32; i++) {
        if (cm.microinstructions[1][i] != expected[i]) {
            matches = 0;
            break;
        }
    }
    
    print_test("load_microprogram: second instruction is MAR <- PC", matches);
}

void test_load_microprogram_invalid_file() {
    control_memory cm;
    init_control_memory(&cm);
    
    int result = load_microprogram(&cm, "data/nonexistent_file.txt");
    
    int passed = (result == -1);
    print_test("load_microprogram: returns -1 for nonexistent file", passed);
}

void test_load_microprogram_null_cm() {
    int result = load_microprogram(NULL, "data/basic_microcode.txt");
    
    int passed = (result == -1);
    print_test("load_microprogram: returns -1 for NULL control_memory", passed);
}

void test_load_microprogram_null_filename() {
    control_memory cm;
    init_control_memory(&cm);
    
    int result = load_microprogram(&cm, NULL);
    
    int passed = (result == -1);
    print_test("load_microprogram: returns -1 for NULL filename", passed);
}

void test_microinstruction_decoding() {
    control_memory cm;
    init_control_memory(&cm);
    
    load_microprogram(&cm, "data/basic_microcode.txt");
    
    mir m;
    for (int i = 0; i < 32; i++) {
        m.data[i] = cm.microinstructions[3][i];
    }
    
    decode_microinstruction(&m);
    
    int passed = (m.amux == 0 && 
                  bits_to_int(m.alu, 2) == 1 &&
                  bits_to_int(m.c, 4) == 5 &&
                  bits_to_int(m.b, 4) == 5 &&
                  bits_to_int(m.a, 4) == 1);
    
    print_test("load_microprogram: instruction 3 (AC <- AC + 1) decodes correctly", passed);
}

int main() {
    printf("\n=== Control Memory Test Suite ===\n\n");
    
    test_init_control_memory();
    test_init_control_memory_null();
    test_load_microprogram_valid();
    test_load_microprogram_first_instruction();
    test_load_microprogram_second_instruction();
    test_load_microprogram_invalid_file();
    test_load_microprogram_null_cm();
    test_load_microprogram_null_filename();
    test_microinstruction_decoding();
    
    printf("\n=== Test Summary ===\n");
    printf("Tests run:    %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Success rate: %.1f%%\n", (tests_passed * 100.0) / tests_run);
    
    return tests_failed > 0 ? 1 : 0;
}
