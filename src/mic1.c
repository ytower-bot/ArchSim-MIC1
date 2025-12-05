
#include "../include/mic1.h"
#include "../include/utils/conversions.h"

void init_mic1(mic1_cpu* cpu) {
    if (!cpu) return;
    cpu->running = 0;
    cpu->cycle_count = 0;
    cpu->clock = 0;

    init_register_bank(&cpu->reg_bank);
    init_alu(&cpu->alu);
    init_cache(&cpu->unified_cache);
    init_memory(&cpu->main_memory);
    init_mar(&cpu->mar);
    init_mbr(&cpu->mbr);
    init_shifter(&cpu->shifter);
    init_mir(&cpu->mir);
    init_mpc(&cpu->mpc);
    init_mmux(&cpu->mmux);
    init_amux(&cpu->amux);
    init_control_memory(&cpu->ctrl_mem);

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

    cpu->running = 0;
    cpu->cycle_count = 0;
    cpu->clock = 0;

    init_register_bank(&cpu->reg_bank);

    init_alu(&cpu->alu);

    init_cache(&cpu->unified_cache);

    init_mar(&cpu->mar);
    init_mbr(&cpu->mbr);

    init_shifter(&cpu->shifter);

    init_mir(&cpu->mir);
    init_mpc(&cpu->mpc);

    init_mmux(&cpu->mmux);
    init_amux(&cpu->amux);

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

    cpu->mar.control_mar = m->mar;
    if (m->mar) {
        run_mar(&cpu->mar, &cpu->latch_b);
    }

    cpu->mbr.control_rd = m->rd;
    if (m->rd) {
        m_read(&cpu->mar, &cpu->mbr, &cpu->main_memory, &cpu->unified_cache);
    }

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
        cpu->mmux.control_cond[i] = m->cond[i];
    }

    for (int i = 0; i < 2; i++) {
        cpu->shifter.control_sh[i] = m->sh[i];
    }
    copy_array(cpu->alu.output, cpu->shifter.data);

    run_shifter(&cpu->shifter, &cpu->mbr, &cpu->bus_c);

    cpu->mbr.control_mbr = m->mbr;
    if (m->mbr) {

        copy_array(cpu->shifter.data, cpu->mbr.data);
    }

    run_decoderC(&cpu->decoder_c, &cpu->shifter);

    cpu->mbr.control_wr = m->wr;
    if (m->wr) {

        m_write(&cpu->mar, &cpu->mbr, &cpu->main_memory, &cpu->unified_cache);
    }
}

void run_mic1_cycle(mic1_cpu* cpu) {
    if (!cpu) return;

    fetch_microinstruction(&cpu->ctrl_mem, &cpu->mpc, &cpu->mir);
    execute_datapath(cpu);
    update_control(&cpu->mpc, &cpu->mmux, &cpu->mir, &cpu->mbr);

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
