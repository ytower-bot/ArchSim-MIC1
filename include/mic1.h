#ifndef MIC1_H
#define MIC1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Includes dos componentes internos
#include "datapath.h"
#include "alu.h"
#include "memory.h"
#include "shifter.h"
#include "control_unit.h"
#include "cache.h"

// Estrutura Principal da CPU MIC-1
typedef struct mic1_cpu {
    // --- Datapath ---
    register_bank reg_bank;
    latch latch_a;
    latch latch_b;

    // Decodificadores de Registradores
    decoder decoder_a;
    decoder decoder_b;
    decoderC decoder_c;

    // Unidades Funcionais
    alu alu;
    shifter shifter;

    // --- Memória ---
    mar mar;
    mbr mbr;
    memory main_memory;
    
    // Caches (Mantidos para compatibilidade com sua estrutura original, 
    // embora a simulação básica possa não usá-los ativamente agora)
    cache data_cache;
    cache instruction_cache;

    // --- Unidade de Controle ---
    mir mir;            // Registrador de Microinstrução
    mpc mpc;            // Contador de Microprograma
    mmux mmux;          // Multiplexador de Microendereço
    amux amux;          // Multiplexador A (Latch A vs MBR)
    control_memory ctrl_mem; // Armazena o microcódigo

    // --- Estado do Sistema ---
    int running;        // Flag de execução (1 = rodando, 0 = parado)
    int cycle_count;    // Contador total de ciclos
    int clock;          // Subciclo atual (se necessário para debug fino)

} mic1_cpu;

// --- Protótipos de Funções ---

// Inicialização e Reset
void init_mic1(mic1_cpu* cpu);
void reset_mic1(mic1_cpu* cpu);

// Controle de Execução
void step_mic1(mic1_cpu* cpu);          // Executa um ciclo completo (4 subciclos)
void run_mic1_program(mic1_cpu* cpu);   // Loop principal de execução

// Carregamento de Dados (File I/O)
int load_microprogram_file(mic1_cpu* cpu, const char* filename);
int load_program_file(mic1_cpu* cpu, const char* filename);

// Funções de Debug e Visualização
void print_cpu_state(mic1_cpu* cpu);
void print_registers(mic1_cpu* cpu);
void print_memory_range(mic1_cpu* cpu, int start, int end);
void print_microinstruction(mir* mir);

// Constantes de Arquitetura
#define MIC1_WORD_SIZE 16
#define MIC1_ADDRESS_SIZE 12
#define MIC1_MICROADDR_SIZE 8

#endif