
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

    if (!cpu->decoder_c.rb) {
        cpu->decoder_c.rb = &cpu->reg_bank;
    }

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

/**
 * Direct instruction executor (high-level interpretation)
 * Executes one machine instruction without microcode.
 * This is used for testing/tracing when microprogram is not loaded.
 */
static void execute_instruction_direct(mic1_cpu* cpu) {
    if (!cpu) return;

    /* Fetch instruction at PC */
    int pc = bits_to_int(cpu->reg_bank.PC.data, 16);
    if (pc >= MEMORY_SIZE) {
        cpu->running = 0;
        return;
    }

    int instr = bits_to_int(cpu->main_memory.data[pc], 16);
    int opcode = (instr >> 12) & 0xF;
    int operand = instr & 0x0FFF;

    /* Store instruction in IR */
    int_to_bits(instr, cpu->reg_bank.IR.data, 16);

    /* Get current register values */
    int ac = bits_to_int(cpu->reg_bank.AC.data, 16);
    int sp = bits_to_int(cpu->reg_bank.SP.data, 16);

    /* Execute based on opcode */
    int next_pc = pc + 1;  /* Default: advance PC */
    int new_ac = ac;
    int new_sp = sp;
    int mem_addr, mem_val;

    switch (opcode) {
        case 0x0:  /* LODD - Load Direct: AC <- M[addr] */
            mem_val = bits_to_int(cpu->main_memory.data[operand], 16);
            new_ac = mem_val;
            break;

        case 0x1:  /* STOD - Store Direct: M[addr] <- AC */
            int_to_bits(ac, cpu->main_memory.data[operand], 16);
            break;

        case 0x2:  /* ADDD - Add Direct: AC <- AC + M[addr] */
            mem_val = bits_to_int(cpu->main_memory.data[operand], 16);
            new_ac = (ac + mem_val) & 0xFFFF;
            break;

        case 0x3:  /* SUBD - Subtract Direct: AC <- AC - M[addr] */
            mem_val = bits_to_int(cpu->main_memory.data[operand], 16);
            new_ac = (ac - mem_val) & 0xFFFF;
            break;

        case 0x4:  /* JPOS - Jump if Positive: if AC > 0 then PC <- addr */
            /* Treat as signed 16-bit */
            if (ac > 0 && ac < 0x8000) {
                next_pc = operand;
            }
            break;

        case 0x5:  /* JZER - Jump if Zero: if AC == 0 then PC <- addr */
            if (ac == 0) {
                next_pc = operand;
            }
            break;

        case 0x6:  /* JUMP - Unconditional Jump: PC <- addr */
            next_pc = operand;
            break;

        case 0x7:  /* LOCO - Load Constant: AC <- constant */
            new_ac = operand;
            break;

        case 0x8:  /* LODL - Load Local: AC <- M[SP + offset] */
            mem_addr = (sp + operand) & 0xFFF;
            mem_val = bits_to_int(cpu->main_memory.data[mem_addr], 16);
            new_ac = mem_val;
            break;

        case 0x9:  /* STOL - Store Local: M[SP + offset] <- AC */
            mem_addr = (sp + operand) & 0xFFF;
            int_to_bits(ac, cpu->main_memory.data[mem_addr], 16);
            break;

        case 0xA:  /* ADDL - Add Local: AC <- AC + M[SP + offset] */
            mem_addr = (sp + operand) & 0xFFF;
            mem_val = bits_to_int(cpu->main_memory.data[mem_addr], 16);
            new_ac = (ac + mem_val) & 0xFFFF;
            break;

        case 0xB:  /* SUBL - Subtract Local: AC <- AC - M[SP + offset] */
            mem_addr = (sp + operand) & 0xFFF;
            mem_val = bits_to_int(cpu->main_memory.data[mem_addr], 16);
            new_ac = (ac - mem_val) & 0xFFFF;
            break;

        case 0xC:  /* JNEG - Jump if Negative: if AC < 0 then PC <- addr */
            /* Treat as signed 16-bit (negative if bit 15 is set) */
            if (ac >= 0x8000) {
                next_pc = operand;
            }
            break;

        case 0xD:  /* JNZE - Jump if Not Zero: if AC != 0 then PC <- addr */
            if (ac != 0) {
                next_pc = operand;
            }
            break;

        case 0xE:  /* CALL - Call subroutine: SP <- SP - 1; M[SP] <- PC + 1; PC <- addr */
            new_sp = (sp - 1) & 0xFFF;
            int_to_bits(pc + 1, cpu->main_memory.data[new_sp], 16);
            next_pc = operand;
            break;

        case 0xF:  /* PSHI - Push Indirect: SP <- SP - 1; M[SP] <- M[AC] */
            new_sp = (sp - 1) & 0xFFF;
            mem_val = bits_to_int(cpu->main_memory.data[ac & 0xFFF], 16);
            int_to_bits(mem_val, cpu->main_memory.data[new_sp], 16);
            break;

        default:
            /* Unknown opcode - halt */
            cpu->running = 0;
            return;
    }

    /* Update registers */
    int_to_bits(new_ac, cpu->reg_bank.AC.data, 16);
    int_to_bits(new_sp, cpu->reg_bank.SP.data, 16);
    int_to_bits(next_pc, cpu->reg_bank.PC.data, 16);

    cpu->cycle_count++;
}

void step_mic1(mic1_cpu* cpu) {
    if (!cpu) return;
    /* Use direct execution for now (microprogram not loaded) */
    execute_instruction_direct(cpu);
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
    if (!cpu || !filename) return -1;

    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }

    /* Get file size */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size <= 0 || file_size > MEMORY_SIZE * 2) {
        fprintf(stderr, "Error: Invalid file size (%ld bytes)\n", file_size);
        fclose(fp);
        return -1;
    }

    /* Read 16-bit instructions (big-endian) */
    int address = 0;
    unsigned char buf[2];

    while (fread(buf, 1, 2, fp) == 2 && address < MEMORY_SIZE) {
        /* Little-endian: low byte first (matches assembler output) */
        int instruction = (buf[1] << 8) | buf[0];

        /* Convert to bit array (MSB first) */
        for (int i = 0; i < 16; i++) {
            cpu->main_memory.data[address][i] = (instruction >> (15 - i)) & 1;
        }
        address++;
    }

    fclose(fp);
    return 0;  /* Success */
}

void connect_components(mic1_cpu* cpu) {
    if (!cpu) return;
}

int is_cpu_halted(mic1_cpu* cpu) {
    if (!cpu) return 1;
    return !cpu->running;
}
