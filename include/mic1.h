#ifndef MIC1_H
#define MIC1_H

/**
 * @file mic1.h
 * @brief Header principal do simulador MIC-1
 * 
 * Este arquivo inclui todos os componentes do simulador e define
 * a estrutura principal que representa o processador MIC-1 completo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datapath.h"
#include "alu.h"
#include "memory.h"
#include "shifter.h"
#include "control_unit.h"
#include "cache.h"

// main structure - MIC-1 CPU
typedef struct mic1_cpu {
    // data path components
    register_bank reg_bank;     // Banco de 16 registradores
    latch latch_a;             // Latch A
    latch latch_b;             // Latch B
    decoder decoder_a;         // Decodificador A
    decoder decoder_b;         // Decodificador B
    decoderC decoder_c;        // Decodificador C
    alu alu;                   // Unidade Lógica Aritmética
    shifter shifter;           // Deslocador
    
    // memory
    mar mar;                   // Memory Address Register
    mbr mbr;                   // Memory Buffer Register
    memory main_memory;        // Memória Principal
    cache cache;               // Cache
    
    // alu control unit
    mir mir;                   // Microinstruction Register
    mpc mpc;                   // Microprogram Counter
    mmux mmux;                 // Multiplexador de endereço
    amux amux;                 // Multiplexador da ULA
    control_memory ctrl_mem;   // Memória de Controle
    
    // cpu state
    int running;               // 1=rodando, 0=parado
    int cycle_count;           // Contador de ciclos
    int clock;                 // Clock atual
} mic1_cpu;

// C structure for bus C
typedef struct busC {
    int data[16];              // bus C data (16 bits)
} busC;

// main functions
void init_mic1(mic1_cpu* cpu);
void reset_mic1(mic1_cpu* cpu);
void run_mic1_cycle(mic1_cpu* cpu);
void run_mic1_program(mic1_cpu* cpu);
void step_mic1(mic1_cpu* cpu);

// loading functions
int load_microprogram_file(mic1_cpu* cpu, const char* filename);
int load_program_file(mic1_cpu* cpu, const char* filename);

// debug and visualization functions
void print_cpu_state(mic1_cpu* cpu);
void print_registers(mic1_cpu* cpu);
void print_memory_range(mic1_cpu* cpu, int start, int end);
void print_microinstruction(mir* mir);

// auxiliary functions
void connect_components(mic1_cpu* cpu);
int is_cpu_halted(mic1_cpu* cpu);

// constants
#define MIC1_WORD_SIZE 16
#define MIC1_ADDRESS_SIZE 12
#define MIC1_MICROADDR_SIZE 8

// Special registers (indices in the register bank)
#define REG_PC    0    // Program Counter
#define REG_AC    1    // Accumulator
#define REG_SP    2    // Stack Pointer
#define REG_IR    3    // Instruction Register
#define REG_TIR   4    // Top of Instruction Register
#define REG_0     5    // Constant 0
#define REG_1     6    // Constant 1
#define REG_NEG1  7    // Constant -1
#define REG_AMASK 8    // Address Mask
#define REG_SMASK 9    // Stack Mask
#define REG_A     10   // Register A
#define REG_B     11   // Register B
#define REG_C     12   // Register C
#define REG_D     13   // Register D
#define REG_E     14   // Register E
#define REG_F     15   // Register F

#endif // MIC1_H