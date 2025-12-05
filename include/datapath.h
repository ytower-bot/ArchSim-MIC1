#ifndef DATAPATH_H
#define DATAPATH_H

typedef struct mic1_register {
    int data[16];
} mic1_register;

typedef struct register_bank {

    mic1_register PC;
    mic1_register AC;
    mic1_register IR;
    mic1_register TIR;
    mic1_register SP;
    mic1_register AMASK;
    mic1_register SMASK;
    mic1_register R0;
    mic1_register R1;
    mic1_register Rm1;
    mic1_register A;
    mic1_register B;
    mic1_register C;
    mic1_register D;
    mic1_register E;
    mic1_register F;
} register_bank;

typedef struct latch {
    int data[16];
} latch;

typedef struct decoder {
    register_bank *rb;

    int control[4];
} decoder;

typedef struct decoderC {

    register_bank *rb;
    int control_c[4];
    int control_enc;
} decoderC;

void run_decoder(decoder* d, latch* l);
void init_register(mic1_register*r);
void init_register_bank(register_bank* rb);
void init_decoder(decoder* d, register_bank* rb);
void init_decoderC(decoderC* d, register_bank* rb);
mic1_register* select_register(register_bank* rb, int control[4]);

#endif