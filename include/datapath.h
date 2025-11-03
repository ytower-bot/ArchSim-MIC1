#ifndef DATAPATH_H
#define DATAPATH_H

// Individual register structure
typedef struct mic1_register {
    int data[16];
} mic1_register;

// Register bank with 16 registers
typedef struct register_bank {
    mic1_register registers[16];
} register_bank;

// Latch structure
typedef struct latch {
    int data[16];
} latch;

// Source register decoder
typedef struct decoder {
    mic1_register* registers[16];
    int control[4];
} decoder;

// Destination register decoder
typedef struct decoderC {
    mic1_register* registers[16];
    int control_c[4];
    int control_enc;
} decoderC;

void run_decoder(decoder* d, latch* l);
void init_register_bank(register_bank* rb);
void init_decoder(decoder* d, register_bank* rb);
void init_decoderC(decoderC* d, register_bank* rb);

#endif