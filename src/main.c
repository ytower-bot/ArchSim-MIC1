/**
 * MIC-1 Execution Trace Logger
 *
 * Non-interactive, deterministic trace output.
 * Runs exactly N cycles and prints register state each cycle.
 *
 * Usage: ./mic1_simulator <program.bin> [cycles]
 */

#include "../include/mic1.h"
#include "../include/utils/conversions.h"

#define DEFAULT_CYCLES 50
#define MAX_CYCLES 10000

/* Helper macros using existing bits_to_int function */
#define REG16(reg) bits_to_int((reg).data, 16)
#define ADDR12(addr) bits_to_int(addr, 12)
#define MEM16(mem) bits_to_int(mem, 16)

/**
 * Print trace header
 */
static void print_header(void) {
    printf("\n");
    printf("=============================================================\n");
    printf("  MIC-1 EXECUTION TRACE\n");
    printf("=============================================================\n");
    printf("\n");
    printf(" CYCLE |  PC  |  AC  |  SP  |  IR  | INSTR    | MEANING\n");
    printf("-------|------|------|------|------|----------|------------------\n");
}

/**
 * Get opcode mnemonic
 */
static const char* opcode_mnemonic(int opcode) {
    static const char* mnemonics[] = {
        "LODD", "STOD", "ADDD", "SUBD",  /* 0-3 */
        "JPOS", "JZER", "JUMP", "LOCO",  /* 4-7 */
        "LODL", "STOL", "ADDL", "SUBL",  /* 8-B */
        "JNEG", "JNZE", "CALL", "PSHI",  /* C-F */
    };
    if (opcode >= 0 && opcode <= 0xF) {
        return mnemonics[opcode];
    }
    return "????";
}

/**
 * Print single trace line
 */
static void print_trace_line(int cycle, mic1_cpu* cpu) {
    int pc  = REG16(cpu->reg_bank.PC);
    int ac  = REG16(cpu->reg_bank.AC);
    int sp  = REG16(cpu->reg_bank.SP);
    int ir  = REG16(cpu->reg_bank.IR);

    /* Read instruction at current PC */
    int instr = 0;
    if (pc < MEMORY_SIZE) {
        instr = MEM16(cpu->main_memory.data[pc]);
    }

    int opcode = (instr >> 12) & 0xF;
    int operand = instr & 0x0FFF;

    printf(" %5d | %04X | %04X | %04X | %04X | %-4s %03X |",
           cycle, pc, ac, sp, ir,
           opcode_mnemonic(opcode), operand);

    /* Brief meaning */
    switch (opcode) {
        case 0x0: printf(" AC<-M[%03X]", operand); break;
        case 0x1: printf(" M[%03X]<-AC", operand); break;
        case 0x2: printf(" AC+=M[%03X]", operand); break;
        case 0x3: printf(" AC-=M[%03X]", operand); break;
        case 0x4: printf(" if AC>0: goto %03X", operand); break;
        case 0x5: printf(" if AC=0: goto %03X", operand); break;
        case 0x6: printf(" goto %03X", operand); break;
        case 0x7: printf(" AC<-%d", operand); break;
        case 0x8: printf(" AC<-M[SP+%d]", operand); break;
        case 0x9: printf(" M[SP+%d]<-AC", operand); break;
        case 0xA: printf(" AC+=M[SP+%d]", operand); break;
        case 0xB: printf(" AC-=M[SP+%d]", operand); break;
        case 0xC: printf(" if AC<0: goto %03X", operand); break;
        case 0xD: printf(" if AC!=0: goto %03X", operand); break;
        case 0xE: printf(" call %03X", operand); break;
        case 0xF: printf(" push indirect"); break;
        default:  printf(" (unknown)"); break;
    }
    printf("\n");
}

/**
 * Print memory dump for verification
 */
static void print_memory_dump(mic1_cpu* cpu, const char* title, int start, int count) {
    printf("\n%s [%03X-%03X]:\n  ", title, start, start + count - 1);
    for (int i = 0; i < count && (start + i) < MEMORY_SIZE; i++) {
        printf("%04X ", MEM16(cpu->main_memory.data[start + i]));
        if ((i + 1) % 8 == 0 && i + 1 < count) printf("\n  ");
    }
    printf("\n");
}

/**
 * Initialize SP to top of stack
 */
static void init_sp(mic1_cpu* cpu) {
    /* Set SP to 0x0FFF */
    for (int i = 0; i < 16; i++) {
        cpu->reg_bank.SP.data[i] = 0;
    }
    /* 0x0FFF = 0000 1111 1111 1111 */
    for (int i = 4; i < 16; i++) {
        cpu->reg_bank.SP.data[i] = 1;
    }
}

/**
 * Check if filename ends with given extension
 */
static int has_extension(const char* filename, const char* ext) {
    size_t fname_len = strlen(filename);
    size_t ext_len = strlen(ext);
    if (fname_len < ext_len) return 0;
    return strcmp(filename + fname_len - ext_len, ext) == 0;
}

int main(int argc, char* argv[]) {
    /* Parse arguments */
    if (argc < 2) {
        fprintf(stderr, "MIC-1 Execution Trace Logger\n");
        fprintf(stderr, "Usage: %s <program.bin> [cycles]\n", argv[0]);
        fprintf(stderr, "  cycles: number of cycles to execute (default: %d, max: %d)\n",
                DEFAULT_CYCLES, MAX_CYCLES);
        return 1;
    }

    /* Protect against running .asm files directly */
    if (has_extension(argv[1], ".asm")) {
        fprintf(stderr, "\n");
        fprintf(stderr, "ERROR: Cannot execute .asm files directly!\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "Assembly files contain text, not machine code.\n");
        fprintf(stderr, "You must compile the .asm file to .bin first.\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "Correct workflow:\n");
        fprintf(stderr, "  1. Assemble:  ./mic1asm %s %.*s.bin\n",
                argv[1], (int)(strlen(argv[1]) - 4), argv[1]);
        fprintf(stderr, "  2. Run:       ./mic1_simulator %.*s.bin\n",
                (int)(strlen(argv[1]) - 4), argv[1]);
        fprintf(stderr, "\n");
        fprintf(stderr, "Or use the helper script:\n");
        fprintf(stderr, "  ./run_test.sh %s\n", argv[1]);
        fprintf(stderr, "\n");
        return 1;
    }

    int num_cycles = DEFAULT_CYCLES;
    if (argc >= 3) {
        num_cycles = atoi(argv[2]);
        if (num_cycles <= 0) num_cycles = DEFAULT_CYCLES;
        if (num_cycles > MAX_CYCLES) num_cycles = MAX_CYCLES;
    }

    /* Initialize CPU */
    mic1_cpu cpu;
    init_mic1(&cpu);
    init_sp(&cpu);

    /* Load program */
    printf("Loading: %s\n", argv[1]);
    if (load_program_file(&cpu, argv[1]) != 0) {
        fprintf(stderr, "Error: Failed to load '%s'\n", argv[1]);
        return 1;
    }

    /* Show loaded code */
    print_memory_dump(&cpu, "CODE", 0x000, 16);
    print_memory_dump(&cpu, "DATA", 0x064, 8);  /* Around address 100 */

    /* Print initial state */
    printf("\nInitial State:\n");
    printf("  PC=%04X  AC=%04X  SP=%04X\n",
           REG16(cpu.reg_bank.PC),
           REG16(cpu.reg_bank.AC),
           REG16(cpu.reg_bank.SP));
    printf("  R0=%04X  R1=%04X  R-1=%04X\n",
           REG16(cpu.reg_bank.R0),
           REG16(cpu.reg_bank.R1),
           REG16(cpu.reg_bank.Rm1));

    /* Start execution */
    cpu.running = 1;
    print_header();

    /* Execute trace loop */
    for (int cycle = 0; cycle < num_cycles; cycle++) {
        /* Print state BEFORE step */
        print_trace_line(cycle, &cpu);

        /* Execute one step */
        step_mic1(&cpu);

        /* Check for halt (JUMP to self or CPU stopped) */
        int pc = REG16(cpu.reg_bank.PC);
        int instr = MEM16(cpu.main_memory.data[pc]);
        int opcode = (instr >> 12) & 0xF;
        int operand = instr & 0x0FFF;

        /* Detect infinite loop (JUMP to current address) */
        if (opcode == 0x6 && operand == pc) {
            printf("-------|------|------|------|------|----------|------------------\n");
            printf(" ** HALT DETECTED (JUMP to self at %03X) **\n", pc);
            break;
        }

        if (!cpu.running || is_cpu_halted(&cpu)) {
            printf("-------|------|------|------|------|----------|------------------\n");
            printf(" ** CPU HALTED **\n");
            break;
        }
    }

    /* Print final state */
    printf("\n");
    printf("=============================================================\n");
    printf("  FINAL STATE (after %d cycles)\n", cpu.cycle_count);
    printf("=============================================================\n");
    printf("  PC=%04X  AC=%04X  SP=%04X  IR=%04X\n",
           REG16(cpu.reg_bank.PC),
           REG16(cpu.reg_bank.AC),
           REG16(cpu.reg_bank.SP),
           REG16(cpu.reg_bank.IR));

    /* Show memory regions that might have changed */
    print_memory_dump(&cpu, "DATA AFTER", 0x064, 8);

    printf("\n--- END OF TRACE ---\n");

    return 0;
}
