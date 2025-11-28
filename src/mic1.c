/**
 * @file mic1.c
 * @brief MIC-1 main functions implementation
 */

#include "../include/mic1.h"
#include "../include/utils/conversions.h"

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
    
    // Initialize decoders with register bank
    cpu->decoder_a.rb = &cpu->reg_bank;
    cpu->decoder_b.rb = &cpu->reg_bank;
    cpu->decoder_c.rb = &cpu->reg_bank;
    
    for(int i = 0; i < 4; i++) {
        cpu->decoder_a.control[i] = 0;
        cpu->decoder_b.control[i] = 0;
        cpu->decoder_c.control_c[i] = 0;
    }
    cpu->decoder_c.control_enc = 0;
}

void reset_mic1(mic1_cpu* cpu) {
    if (!cpu) return;
    init_mic1(cpu);
}

/**
 * @brief Execute datapath operations for current microinstruction
 *
 * @param cpu Pointer to MIC-1 CPU structure
 *
 * Executes the complete datapath sequence:
 * 1. Decoder A/B read registers to Latch A/B
 * 2. AMUX selects ALU input A (Latch A or MBR)
 * 3. ALU executes operation
 * 4. Shifter processes ALU result
 * 5. Decoder C writes result to register (if ENC=1)
 * 6. MAR/MBR handle memory operations (if RD=1 or WR=1)
 */
void execute_datapath(mic1_cpu* cpu) {
    if (!cpu) return;

    mir* m = &cpu->mir;

    for (int i = 0; i < 4; i++) {
        cpu->decoder_a.control[i] = m->a[i];
        cpu->decoder_b.control[i] = m->b[i];
        cpu->decoder_c.control_c[i] = m->c[i];
    }
    cpu->decoder_c.control_enc = m->enc;

    run_decoder(&cpu->decoder_a, &cpu->latch_a);
    run_decoder(&cpu->decoder_b, &cpu->latch_b);

    cpu->amux.control_amux = m->amux;
    run_amux(&cpu->amux, &cpu->mbr, &cpu->latch_a, &cpu->alu);

    copy_array(cpu->latch_b.data, cpu->alu.input_b);

    for (int i = 0; i < 2; i++) {
        cpu->alu.control[i] = m->alu[i];
    }
    run_alu(&cpu->alu);

    cpu->mmux.alu_n = cpu->alu.flag_n;
    cpu->mmux.alu_z = cpu->alu.flag_z;

    for (int i = 0; i < 2; i++) {
        cpu->shifter.control_sh[i] = m->sh[i];
    }
    copy_array(cpu->alu.output, cpu->shifter.data);
    run_shifter(&cpu->shifter, &cpu->mbr, &cpu->bus_c);

    run_decoderC(&cpu->decoder_c, &cpu->shifter);

    if (m->mar) {
        run_mar(&cpu->mar, &cpu->latch_b);
    }

    if (m->rd || m->wr) {
        run_mbr(&cpu->mar, &cpu->mbr, &cpu->main_memory, &cpu->shifter);
    }
}

void run_mic1_cycle(mic1_cpu* cpu) {
    if (!cpu) return;

    fetch_microinstruction(&cpu->ctrl_mem, &cpu->mpc, &cpu->mir);

    execute_datapath(cpu);

    update_control(&cpu->mpc, &cpu->mmux, &cpu->mir);

    cpu->cycle_count++;
    cpu->clock++;
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
// Note: init_mir(), run_mir(), decode_microinstruction(), init_mpc(), run_mpc(), 
//       increment_mpc(), init_mmux(), run_mmux(), should_branch(), init_amux(), run_amux(),
//       init_control_memory(), load_microprogram() - implemented in control_unit.c
// bits_to_int, int_to_bits - implemented in src/utils/conversions.c

// Cache functions - implemented in src/cache.c