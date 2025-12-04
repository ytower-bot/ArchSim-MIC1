#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>

// Definições de Opcode para MAC-1 (Tanenbaum Fig. 4.14)
// Instruções com Operando de Endereço/Constante (Bits 15-12)
#define OP_LODD  0x0  // Carrega direto
#define OP_STOD  0x1  // Armazena direto
#define OP_ADDD  0x2  // Soma direto
#define OP_SUBD  0x3  // Subtrai direto
#define OP_JPOS  0x4  // Desvia se positivo
#define OP_JZER  0x5  // Desvia se zero
#define OP_JUMP  0x6  // Desvio incondicional
#define OP_LOCO  0x7  // Carrega constante (0-4095)
#define OP_LODL  0x8  // Carrega local
#define OP_STOL  0x9  // Armazena local
#define OP_ADDL  0xA  // Soma local
#define OP_SUBL  0xB  // Subtrai local
#define OP_JNEG  0xC  // Desvia se negativo
#define OP_JNZE  0xD  // Desvia se não zero
#define OP_CALL  0xE  // Chama procedimento

// Instruções Especiais (Prefixo 0xF / 1111)
#define OP_SPEC  0xF 

// Sub-opcodes para instruções especiais (Bits 11-0 determinam a instrução)
// Valores baseados na Fig 4.14 (Tanenbaum)
#define OPL_PSHI 0x000 // 1111 0000 ...
#define OPL_POPI 0x200 // 1111 0010 ...
#define OPL_PUSH 0x400 // 1111 0100 ...
#define OPL_POP  0x600 // 1111 0110 ...
#define OPL_RETN 0x800 // 1111 1000 ...
#define OPL_SWAP 0xA00 // 1111 1010 ...
#define OPL_INSP 0xC00 // 1111 1100 ...
#define OPL_DESP 0xE00 // 1111 1110 ...

// Limites
#define MAX_LABELS 256
#define MAX_INSTRUCTIONS 4096
#define MAX_LINE_LENGTH 256
#define MAX_LABEL_LENGTH 64

typedef struct {
    char label[MAX_LABEL_LENGTH];
    uint16_t address; // Endereço em palavras (word address)
} symbol_t;

typedef struct {
    symbol_t symbols[MAX_LABELS];
    int symbol_count;
    int current_address; // Em bytes
    int error_count;
    char error_msg[256];
} assembler_t;

// Funções
int assemble_file(const char* input_file, const char* output_file);
void init_assembler(assembler_t* as);

#endif