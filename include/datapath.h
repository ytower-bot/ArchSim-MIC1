#ifndef DATAPATH_H
#define DATAPATH_H

/**
 * @file datapath.h
 * @brief Componentes da via de dados do MIC-1
 * 
 * Este arquivo contém as definições para:
 * - Registradores (16 registradores de 16 bits)
 * - Latches A e B 
 * - Decodificadores A, B e C
 */

// individual register
typedef struct register_t {
    int data[16];  // 16 bits por registrador
} register_t;

// register bank with 16 registers
typedef struct register_bank {
    register_t registers[16];
} register_bank;

// latch structure
typedef struct latch {
    int data[16];  // Valor armazenado de 16 bits
} latch;

// decoder for selecting source registers (A and B)
typedef struct decoder {
    register_t* registers[16];  // pointers to the 16 registers
    int control[4];            // Control signal (4 bits, 0-15)
} decoder;

// decoder for selecting destination register (C)
typedef struct decoderC {
    register_t* registers[16];  // pointers to the 16 registers
    int control_c[4];          // Control signal C (4 bits, 0-15)
    int control_enc;           // ENC signal: 0=do not write, 1=allow write
} decoderC;

// decoder functions
void run_decoder(decoder* d, latch* l);
void run_decoderC(decoderC* d, struct shifter* s);

// auxiliary functions
void init_register_bank(register_bank* rb);
void init_decoder(decoder* d, register_bank* rb);
void init_decoderC(decoderC* d, register_bank* rb);

#endif // DATAPATH_H