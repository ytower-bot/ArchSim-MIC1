/**
 * @file mic1.c
 * @brief MIC-1 main functions implementation
 */

#include "../include/mic1.h"

void init_mic1(mic1_cpu* cpu) {
    if (!cpu) return;
    cpu->running = 0;
    cpu->cycle_count = 0;
    cpu->clock = 0;

    // Inicializar banco de registradores
    init_register_bank(&cpu->reg_bank);
    init_alu(&cpu->alu);
    init_cache(&cpu->cache);
    init_memory(&cpu->main_memory);
    init_mar(&cpu->mar);
    init_mbr(&cpu->mbr);
    init_shifter(&cpu->shifter);
    init_mir(&cpu->mir);
    init_mpc(&cpu->mpc);
    init_mmux(&cpu->mmux);
    init_amux(&cpu->amux);
    init_control_memory(&cpu->ctrl_mem);
}

void reset_mic1(mic1_cpu* cpu) {
    if (!cpu) return;
    init_mic1(cpu);
}

void run_mic1_cycle(mic1_cpu* cpu) {
    if (!cpu) return;
    cpu->cycle_count++;
}

void run_mic1_program(mic1_cpu* cpu) {
    if (!cpu) return;
    cpu->running = 1;
}

void step_mic1(mic1_cpu* cpu) {
    if (!cpu) return;
    run_mic1_cycle(cpu);
}

void print_cpu_state(mic1_cpu* cpu) {
    if (!cpu) return;
    printf("=== CPU STATE ===\n");
    printf("Status: %s\n", cpu->running ? "RUNNING" : "STOPPED");
    printf("Cycles: %d\n", cpu->cycle_count);
    printf("Clock: %d\n", cpu->clock);
    printf("================\n");
}

void print_registers(mic1_cpu* cpu) {
    if (!cpu) return;
    printf("=== REGISTERS ===\n");

    // Helper macro para imprimir um registrador
    #define PRINT_REG(name, reg) \
        printf("%-6s: ", name); \
        for (int j = 0; j < 16; j++) { \
            printf("%d", cpu->reg_bank.reg.data[j]); \
        } \
        printf("\n");

    PRINT_REG("PC", PC);
    PRINT_REG("AC", AC);
    PRINT_REG("IR", IR);
    PRINT_REG("TIR", TIR);
    PRINT_REG("SP", SP);
    PRINT_REG("AMASK", AMASK);
    PRINT_REG("SMASK", SMASK);
    PRINT_REG("0", R0);
    PRINT_REG("+1", R1);
    PRINT_REG("-1", Rm1);
    PRINT_REG("A", A);
    PRINT_REG("B", B);
    PRINT_REG("C", C);
    PRINT_REG("D", D);
    PRINT_REG("E", E);
    PRINT_REG("F", F);

    #undef PRINT_REG
    printf("================\n");
}

void print_memory_range(mic1_cpu* cpu, int start, int end) {
    if (!cpu) return;
    printf("=== MEMORY [%d-%d] ===\n", start, end);
    printf("(Implementation pending)\n");
    printf("====================\n");
}

void print_microinstruction(mir* mir) {
    if (!mir) return;
    printf("=== MICROINSTRUCTION ===\n");
    printf("(Implementation pending)\n");
    printf("=======================\n");
}

int load_microprogram_file(mic1_cpu* cpu, const char* filename) {
    if (!cpu || !filename) return 0;
    printf("Loading microprogram: %s\n", filename);
    return 1;
}

int load_program_file(mic1_cpu* cpu, const char* filename) {
    if (!cpu || !filename) return 0;
    printf("Loading program: %s\n", filename);
    return 1;
}

void connect_components(mic1_cpu* cpu) {
    if (!cpu) return;
}

int is_cpu_halted(mic1_cpu* cpu) {
    if (!cpu) return 1;
    return !cpu->running;
}

// Stub implementations for compilation

// Datapath functions - implemented in src/datapath.c
// init_register_bank, init_decoder, init_decoderC, run_decoder
// run_decoderC - implemented in src/datapath.c

// ALU functions - implemented in src/alu.c

// Memory functions - implemented in src/memory.c
// init_mar, init_mbr, run_mar, run_mbr, init_memory, m_read, m_write, load_program
// address_to_int, int_to_address, copy_data - implemented in src/utils/conversions.c

// Shifter functions - implemented in src/shifter.c
// init_shifter, lshift, rshift, set_shifter_input, set_shifter_control
// run_shifter, get_shifter_control_value - implemented in src/shifter.c

// Control unit functions
void init_mir(mir* m) { if (!m) return; }
void init_mpc(mpc* p) { if (!p) return; }
void init_mmux(mmux* m) { if (!m) return; }
void init_amux(amux* a) { if (!a) return; }
void init_control_memory(control_memory* cm) { if (!cm) return; }
void run_mir(mir* m, mbr* mb, mar* ma, mmux* mmu, amux* amu, shifter* s, alu* al, decoder* da, decoder* db, decoderC* dc) { if (!m) return; }
void decode_microinstruction(mir* m) { if (!m) return; }
void run_mpc(mpc* p, mir* m, control_memory* cm) { if (!p || !m || !cm) return; }
void increment_mpc(mpc* p) { if (!p) return; }
void run_mmux(mmux* m, mpc* p, mir* mir) { if (!m || !p || !mir) return; }
int should_branch(mmux* m) { if (!m) return 0; return 0; }
void run_amux(amux* a, mbr* b, latch* lA, alu* u) { if (!a || !b || !lA || !u) return; }
void load_microprogram(control_memory* cm, const char* filename) { if (!cm || !filename) return; }
// bits_to_int, int_to_bits - implemented in src/utils/conversions.c

// Cache functions - implemented in src/cache.c