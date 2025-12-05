#include "../include/mic1.h"
#include "../include/assembler.h"
#include "../include/utils/conversions.h"
#include <stdlib.h>
#include <string.h>

static mic1_cpu* g_cpu = NULL;
static char g_microcode_path[512] = {0};

void cgo_init_cpu(void) {
    if (g_cpu == NULL) {
        g_cpu = (mic1_cpu*)malloc(sizeof(mic1_cpu));
        if (g_cpu) {
            init_mic1(g_cpu);
        }
    }
}

void cgo_reset_cpu(void) {
    if (g_cpu) {
        reset_mic1(g_cpu);
    }
}

void cgo_step_cpu(void) {
    if (g_cpu) {
        step_mic1(g_cpu);
    }
}

uint16_t cgo_get_register(int index) {
    if (g_cpu == NULL) return 0;
    if (index < 0 || index >= 16) return 0;

    switch(index) {
        case 0: return bits_to_int(g_cpu->reg_bank.PC.data, 16);
        case 1: return bits_to_int(g_cpu->reg_bank.AC.data, 16);
        case 2: return bits_to_int(g_cpu->reg_bank.IR.data, 16);
        case 3: return bits_to_int(g_cpu->reg_bank.TIR.data, 16);
        case 4: return bits_to_int(g_cpu->reg_bank.SP.data, 16);
        default: return 0;
    }
}

int cgo_get_cycles(void) {
    if (g_cpu == NULL) return 0;
    return g_cpu->cycle_count;
}

int cgo_get_clock(void) {
    if (g_cpu == NULL) return 0;
    return g_cpu->clock;
}

int cgo_is_running(void) {
    if (g_cpu == NULL) return 0;
    return g_cpu->running;
}

int cgo_get_flag_n(void) {
    if (g_cpu == NULL) return 0;
    return g_cpu->alu.flag_n;
}

int cgo_get_flag_z(void) {
    if (g_cpu == NULL) return 0;
    return g_cpu->alu.flag_z;
}

uint8_t cgo_get_mpc(void) {
    if (g_cpu == NULL) return 0;
    return (uint8_t)bits_to_int(g_cpu->mpc.address, 8);
}

uint16_t cgo_read_memory(uint16_t address) {
    if (g_cpu == NULL) return 0;
    if (address >= 4096) return 0;
    return bits_to_int(g_cpu->main_memory.data[address], 16);
}

void cgo_write_memory(uint16_t address, uint16_t value) {
    if (g_cpu == NULL) return;
    if (address >= 4096) return;
    int_to_bits(value, g_cpu->main_memory.data[address], 16);
}

int cgo_get_cache_hits(void) {
    if (g_cpu == NULL) return 0;
    return g_cpu->unified_cache.hits;
}

int cgo_get_cache_misses(void) {
    if (g_cpu == NULL) return 0;
    return g_cpu->unified_cache.misses;
}

int cgo_get_cache_line_valid(int line_index) {
    if (g_cpu == NULL) return 0;
    if (line_index < 0 || line_index >= 8) return 0;
    return g_cpu->unified_cache.lines[line_index].valid;
}

uint8_t cgo_get_cache_line_tag(int line_index) {
    if (g_cpu == NULL) return 0;
    if (line_index < 0 || line_index >= 8) return 0;
    return (uint8_t)bits_to_int(g_cpu->unified_cache.lines[line_index].tag, 7);
}

uint16_t cgo_get_cache_line_word(int line_index, int word_index) {
    if (g_cpu == NULL) return 0;
    if (line_index < 0 || line_index >= 8) return 0;
    if (word_index < 0 || word_index >= 4) return 0;
    return bits_to_int(g_cpu->unified_cache.lines[line_index].data[word_index], 16);
}

int cgo_assemble_file(const char* filename) {
    if (g_cpu == NULL) return -1;
    char output_file[512];
    strncpy(output_file, filename, sizeof(output_file) - 1);
    output_file[sizeof(output_file) - 1] = '\0';
    
    char* dot = strrchr(output_file, '.');
    if (dot) {
        strcpy(dot, ".bin");
    } else {
        strcat(output_file, ".bin");
    }
    
    return assemble_file(filename, output_file);
}

int cgo_load_microcode(const char* filename) {
    if (g_cpu == NULL) return -1;
    strncpy(g_microcode_path, filename, sizeof(g_microcode_path) - 1);
    g_microcode_path[sizeof(g_microcode_path) - 1] = '\0';
    return load_microprogram(&g_cpu->ctrl_mem, filename);
}

const char* cgo_get_microcode_path(void) {
    return g_microcode_path;
}

void cgo_cleanup_cpu(void) {
    if (g_cpu) {
        free(g_cpu);
        g_cpu = NULL;
    }
}

