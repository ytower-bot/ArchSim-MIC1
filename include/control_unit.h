#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#define MICROPROGRAM_SIZE 256

#include "datapath.h"
#include "memory.h"
#include "alu.h"

// AMUX multiplexer
typedef struct amux {
    int control_amux; // 0 = latch A, 1 = MBR
} amux;

// Microinstruction Register
typedef struct mir {
    int data[32]; // Instrução inteira

    int amux;
    int cond[2];
    int alu[2];
    int sh[2];
    int mbr;
    int mar;
    int rd;
    int wr;
    int enc;
    int c[4];
    int b[4];
    int a[4];
    int addr[8];
} mir;

// Microprogram Counter
typedef struct mpc {
    int address[8];
} mpc;

// Microaddress Multiplexer
typedef struct mmux {
    int control_cond[2];
    int alu_n;
    int alu_z;
} mmux;

// Control Memory
typedef struct control_memory {
    int microinstructions[MICROPROGRAM_SIZE][32];
} control_memory;

#define COND_NONE       0b00
#define COND_IF_N       0b01
#define COND_IF_Z       0b10
#define COND_ALWAYS     0b11

// Funções
void init_mir(mir* m);
void init_mpc(mpc* p);
void init_mmux(mmux* m);
void init_amux(amux* a);
void init_control_memory(control_memory* cm);

// Decodificação
void decode_microinstruction(mir* m);
int load_microprogram(control_memory* cm, const char* filename);
void fetch_microinstruction(control_memory* cm, mpc* p, mir* m);

void run_amux(amux* a, mbr* mb, latch* lA, alu* u);
void run_mmux(mmux* m, mpc* p, mir* mir);
int should_branch(mmux* m);

void update_control(mpc* p, mmux* mmux, mir* m);

#endif