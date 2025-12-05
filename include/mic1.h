#ifndef MIC1_H
#define MIC1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datapath.h"
#include "alu.h"
#include "memory.h"
#include "shifter.h"
#include "control_unit.h"
#include "cache.h"
#include "connections.h"

typedef struct mic1_cpu {
    register_bank reg_bank;
    latch latch_a;
    latch latch_b;
    decoder decoder_a;
    decoder decoder_b;
    decoderC decoder_c;
    alu alu;
    shifter shifter;
    mar mar;
    mbr mbr;
    memory main_memory;
    cache unified_cache;
    mir mir;
    mpc mpc;
    mmux mmux;
    amux amux;
    control_memory ctrl_mem;
    barrC bus_c;
    int running;
    int cycle_count;
    int clock;
} mic1_cpu;

void init_mic1(mic1_cpu* cpu);
void reset_mic1(mic1_cpu* cpu);
void run_mic1_cycle(mic1_cpu* cpu);
void execute_datapath(mic1_cpu* cpu);
void run_mic1_program(mic1_cpu* cpu);
void step_mic1(mic1_cpu* cpu);
int load_microprogram_file(mic1_cpu* cpu, const char* filename);
int load_program_file(mic1_cpu* cpu, const char* filename);
void print_cpu_state(mic1_cpu* cpu);
void print_registers(mic1_cpu* cpu);
void print_memory_range(mic1_cpu* cpu, int start, int end);
void print_microinstruction(mir* mir);
void connect_components(mic1_cpu* cpu);
int is_cpu_halted(mic1_cpu* cpu);

#define MIC1_WORD_SIZE 16
#define MIC1_ADDRESS_SIZE 12
#define MIC1_MICROADDR_SIZE 8

#endif