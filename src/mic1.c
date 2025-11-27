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

// Datapath functions
// init_register_bank implementada em datapath.c
void init_decoder(decoder* d, register_bank* rb) { if (!d || !rb) return; }
void init_decoderC(decoderC* d, register_bank* rb) { if (!d || !rb) return; }
void run_decoder(decoder* d, latch* l) { if (!d || !l) return; }
void run_decoderC(decoderC* d, shifter* s) { if (!d || !s) return; }

// ALU functions - implemented in src/alu.c

// Memory functions
void init_mar(mar* a) { if (!a) return; a->control_mar = 0; }
void init_mbr(mbr* b) { if (!b) return; b->control_rd = 0; b->control_wr = 0; b->control_mbr = 0; }
void init_memory(memory* mem) { if (!mem) return; }
void run_mar(mar* a, struct latch* lB) { if (!a || !lB) return; }
void run_mbr(mar* a, mbr* b, memory* mem) { if (!a || !b || !mem) return; }
void m_read(mar* a, mbr* b, memory* mem) { if (!a || !b || !mem) return; }
void m_write(mar* a, mbr* b, memory* mem) { if (!a || !b || !mem) return; }
void load_program(memory* mem, const char* filename) { if (!mem || !filename) return; }
int address_to_int(int address[12]) { if (!address) return 0; return 0; }
void int_to_address(int addr, int address[12]) { if (!address) return; }
void copy_data(int dest[16], int src[16]) { if (!dest || !src) return; }

// Shifter functions
void init_shifter(shifter* s) { if (!s) return; }
void run_shifter(shifter* s, mbr* b, barrC* c) { if (!s || !b || !c) return; }
void set_shifter_input(shifter* s, int input[16]) { if (!s || !input) return; }
void set_shifter_control(shifter* s, int control[2]) { if (!s || !control) return; }
void lshift(shifter* s) { if (!s) return; }
void rshift(shifter* s) { if (!s) return; }
int get_shifter_control_value(int control[2]) { if (!control) return 0; return 0; }

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
int bits_to_int(int bits[], int size) { if (!bits || size <= 0) return 0; return 0; }
void int_to_bits(int value, int bits[], int size) { if (!bits || size <= 0) return; }

// Cache functions
void init_cache(cache* c) { if (!c) return; c->hits = 0; c->misses = 0; for (int i = 0; i < CACHE_SIZE; i++) { c->lines[i].valid = 0; } }
int cache_read(cache* c, struct memory* mem, int address[12], int data[16]) { if (!c || !mem || !address || !data) return 0; return 0; }
void cache_write(cache* c, struct memory* mem, int address[12], int data[16]) { if (!c || !mem || !address || !data) return; }
int cache_lookup(cache* c, address_fields* addr) { if (!c || !addr) return 0; return 0; }
void cache_load_block(cache* c, struct memory* mem, address_fields* addr) { if (!c || !mem || !addr) return; }
void decompose_address(int address[12], address_fields* addr) { if (!address || !addr) return; }
int compare_tags(int tag1[TAG_BITS], int tag2[TAG_BITS]) { if (!tag1 || !tag2) return 0; return 0; }
int line_index_to_int(int line[LINE_BITS]) { if (!line) return 0; return 0; }
int word_offset_to_int(int word[WORD_BITS]) { if (!word) return 0; return 0; }
void copy_tag(int dest[TAG_BITS], int src[TAG_BITS]) { if (!dest || !src) return; }
void print_cache_stats(cache* c) { if (!c) return; printf("=== CACHE STATS ===\nHits: %d\nMisses: %d\nHit Rate: %.2f%%\n==================\n", c->hits, c->misses, get_hit_rate(c)); }
void reset_cache_stats(cache* c) { if (!c) return; c->hits = 0; c->misses = 0; }
double get_hit_rate(cache* c) { if (!c || (c->hits + c->misses) == 0) return 0.0; return ((double)c->hits / (c->hits + c->misses)) * 100.0; }
void print_cache_line(cache_line* line, int line_num) { if (!line) return; printf("Line %d: %s\n", line_num, line->valid ? "VALID" : "INVALID"); }
void print_cache_state(cache* c) { if (!c) return; printf("=== CACHE STATE ===\n"); for (int i = 0; i < CACHE_SIZE; i++) { print_cache_line(&c->lines[i], i); } printf("==================\n"); }