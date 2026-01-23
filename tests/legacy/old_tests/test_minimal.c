/*
 * test_minimal.c - Minimal MIC-1 cycle test
 *
 * Just verifies that the microcode executes and PC increments
 */

#include <stdio.h>
#include "../include/mic1.h"
#include "../include/control_unit.h"
#include "../include/utils/conversions.h"

int main(void) {
    printf("=== MINIMAL MIC-1 TEST ===\n\n");

    mic1_cpu cpu;
    init_mic1(&cpu);

    printf("[1] Loading microprogram...\n");
    int count = load_microprogram(&cpu.ctrl_mem, "data/basic_microcode.txt");
    printf("    Loaded %d microinstructions\n", count);

    /* Put a simple instruction in memory[0]: LOCO 2 = 0x7002 */
    printf("\n[2] Loading instruction 0x7002 (LOCO 2) at address 0...\n");
    int_to_array(0x7002, cpu.main_memory.data[0]);

    printf("\n[3] Initial state:\n");
    printf("    MPC = %d\n", bits_to_int(cpu.mpc.address, 8));
    printf("    PC  = %d\n", array_to_int(cpu.reg_bank.PC.data));
    printf("    AC  = 0x%04X\n", array_to_int(cpu.reg_bank.AC.data) & 0xFFFF);
    printf("    IR  = 0x%04X\n", array_to_int(cpu.reg_bank.IR.data) & 0xFFFF);

    printf("\n[4] Executing 10 microcycles...\n");
    for (int i = 0; i < 10; i++) {
        int mpc_before = bits_to_int(cpu.mpc.address, 8);
        run_mic1_cycle(&cpu);
        int mpc_after = bits_to_int(cpu.mpc.address, 8);
        int pc = array_to_int(cpu.reg_bank.PC.data);
        int ac = array_to_int(cpu.reg_bank.AC.data);
        int ir = array_to_int(cpu.reg_bank.IR.data);

        printf("    Cycle %d: MPC %02X->%02X, PC=%d, AC=0x%04X, IR=0x%04X\n",
               i + 1, mpc_before, mpc_after, pc, ac & 0xFFFF, ir & 0xFFFF);
    }

    printf("\n[5] Final state:\n");
    printf("    PC  = %d\n", array_to_int(cpu.reg_bank.PC.data));
    printf("    AC  = 0x%04X\n", array_to_int(cpu.reg_bank.AC.data) & 0xFFFF);
    printf("    IR  = 0x%04X\n", array_to_int(cpu.reg_bank.IR.data) & 0xFFFF);

    printf("\n=== END ===\n");
    return 0;
}
