/*
 * test_loco_execution.c - Integration test for LOCO instruction
 *
 * Loads proof_math.asm binary and verifies:
 *   1. LOCO 2 correctly loads 0x0002 into AC
 *   2. After STOD/ADDD sequence, memory contains expected values
 *
 * Expected final state:
 *   AC = 0x0004
 *   Memory[0x80] = 0x0002
 *   Memory[0x81] = 0x0002
 *   Memory[0x82] = 0x0004
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/mic1.h"
#include "../include/control_unit.h"
#include "../include/memory.h"
#include "../include/utils/conversions.h"

void print_register_value(const char* name, int bits[16]) {
    int value = array_to_int(bits);
    printf("  %-8s = 0x%04X (%d)\n", name, value & 0xFFFF, value);
}

int main(int argc, char** argv) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  LOCO INSTRUCTION EXECUTION TEST                           ║\n");
    printf("║  Testing: Complete LOCO instruction in MIC-1 simulator    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");

    /* Initialize CPU */
    mic1_cpu cpu;
    init_mic1(&cpu);

    /* Load microprogram */
    printf("[1] Loading microprogram...\n");
    int ucode_count = load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    if (ucode_count <= 0) {
        fprintf(stderr, "Error: Failed to load microprogram\n");
        return 1;
    }
    printf("    Loaded %d microinstructions\n", ucode_count);

    /* Load program binary */
    printf("\n[2] Loading program binary...\n");
    const char* program_file = (argc > 1) ? argv[1] : "tests/proof_math.bin";
    FILE* fp = fopen(program_file, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open %s\n", program_file);
        return 1;
    }

    /* Read binary into memory starting at address 0 */
    unsigned char buffer[4096];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    printf("    Read %zu bytes from %s\n", bytes_read, program_file);

    /* Load into main memory as 16-bit words */
    for (size_t i = 0; i < bytes_read; i += 2) {
        if (i + 1 < bytes_read) {
            uint16_t word = (buffer[i] << 8) | buffer[i + 1];
            int addr = i / 2;

            /* Store word in memory */
            for (int bit = 0; bit < 16; bit++) {
                cpu.main_memory.data[addr][bit] = (word >> (15 - bit)) & 1;
            }
        }
    }

    /* Print first few instructions */
    printf("\n[3] Program loaded at address 0:\n");
    for (int addr = 0; addr < 6 && addr < (int)(bytes_read / 2); addr++) {
        int value = 0;
        for (int bit = 0; bit < 16; bit++) {
            value = (value << 1) | cpu.main_memory.data[addr][bit];
        }
        printf("    [0x%02X] = 0x%04X\n", addr, value);
    }

    /* Execute program step by step */
    printf("\n[4] Executing program...\n");

    /* Step 1: Execute LOCO 2 instruction */
    printf("\n--- Before first instruction ---\n");
    print_register_value("PC", cpu.reg_bank.PC.data);
    print_register_value("AC", cpu.reg_bank.AC.data);
    print_register_value("IR", cpu.reg_bank.IR.data);

    /* Run fetch-decode-execute cycles until we complete LOCO */
    /* LOCO takes: fetch (5 cycles) + dispatch (varies) + execute (1 cycle) */
    /* Let's run 50 cycles to complete first instruction */
    for (int i = 0; i < 50; i++) {
        run_mic1_cycle(&cpu);

        /* Check if PC advanced to 1 (completed LOCO) */
        int pc_value = array_to_int(cpu.reg_bank.PC.data);
        if (pc_value == 1) {
            printf("\n--- After LOCO 2 (cycle %d) ---\n", i + 1);
            print_register_value("PC", cpu.reg_bank.PC.data);
            print_register_value("AC", cpu.reg_bank.AC.data);
            print_register_value("IR", cpu.reg_bank.IR.data);

            int ac_value = array_to_int(cpu.reg_bank.AC.data);
            if (ac_value == 0x0002) {
                printf("  ✓ LOCO 2 SUCCESS: AC = 0x0002\n");
            } else {
                printf("  ✗ LOCO 2 FAILED: AC = 0x%04X (expected 0x0002)\n",
                       ac_value & 0xFFFF);
                return 1;
            }
            break;
        }
    }

    /* Continue execution for the rest of the program */
    printf("\n[5] Continuing execution for remaining instructions...\n");
    for (int i = 0; i < 500; i++) {
        int pc_before = array_to_int(cpu.reg_bank.PC.data);
        run_mic1_cycle(&cpu);
        int pc_after = array_to_int(cpu.reg_bank.PC.data);

        /* Detect JUMP to self (halt condition) */
        if (pc_after == 5 && pc_before == 5) {
            printf("    Halted at JUMP 5 after %d total cycles\n",
                   cpu.cycle_count);
            break;
        }
    }

    /* Verify final state */
    printf("\n[6] Verifying final state...\n");
    print_register_value("PC", cpu.reg_bank.PC.data);
    print_register_value("AC", cpu.reg_bank.AC.data);

    /* Read memory at addresses 0x80, 0x81, 0x82 */
    int mem_80 = 0, mem_81 = 0, mem_82 = 0;
    for (int bit = 0; bit < 16; bit++) {
        mem_80 = (mem_80 << 1) | cpu.main_memory.data[0x80][bit];
        mem_81 = (mem_81 << 1) | cpu.main_memory.data[0x81][bit];
        mem_82 = (mem_82 << 1) | cpu.main_memory.data[0x82][bit];
    }

    printf("  Memory[0x80] = 0x%04X (expected 0x0002)\n", mem_80 & 0xFFFF);
    printf("  Memory[0x81] = 0x%04X (expected 0x0002)\n", mem_81 & 0xFFFF);
    printf("  Memory[0x82] = 0x%04X (expected 0x0004)\n", mem_82 & 0xFFFF);

    int ac_final = array_to_int(cpu.reg_bank.AC.data);
    printf("  AC (final)   = 0x%04X (expected 0x0004)\n", ac_final & 0xFFFF);

    /* Check results */
    int errors = 0;
    if (mem_80 != 0x0002) {
        printf("  ✗ Memory[0x80] incorrect\n");
        errors++;
    }
    if (mem_81 != 0x0002) {
        printf("  ✗ Memory[0x81] incorrect\n");
        errors++;
    }
    if (mem_82 != 0x0004) {
        printf("  ✗ Memory[0x82] incorrect\n");
        errors++;
    }
    if (ac_final != 0x0004) {
        printf("  ✗ AC incorrect\n");
        errors++;
    }

    printf("\n");
    if (errors == 0) {
        printf("╔════════════════════════════════════════════════════════════╗\n");
        printf("║  ✓ ALL CHECKS PASSED - LOCO WORKS CORRECTLY!             ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        return 0;
    } else {
        printf("╔════════════════════════════════════════════════════════════╗\n");
        printf("║  ✗ %d CHECK(S) FAILED - DEBUG REQUIRED                    ║\n", errors);
        printf("╚════════════════════════════════════════════════════════════╝\n");
        return 1;
    }
}
