#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#define MICROPROGRAM_SIZE 256

typedef struct amux {
    int control_amux;
} amux;

typedef struct mir {
    int data[32];
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

typedef struct mpc {
    int address[8];
} mpc;

typedef struct mmux {
    int control_cond[2];
    int alu_n;
    int alu_z;
} mmux;

typedef struct control_memory {
    int microinstructions[MICROPROGRAM_SIZE][32];
} control_memory;

#define COND_NONE       0b00
#define COND_IF_N       0b01
#define COND_IF_Z       0b10
#define COND_ALWAYS     0b11

struct mbr;

void decode_microinstruction(mir* m);
void init_mir(mir* m);
void run_mpc(mpc* p, mir* m, control_memory* cm);
void increment_mpc(mpc* p);
void init_mpc(mpc* p);
void run_mmux(mmux* m, mpc* p, mir* mir, struct mbr* mb);
int should_branch(mmux* m);
void init_mmux(mmux* m);
void init_amux(amux* a);
void init_control_memory(control_memory* cm);
int load_microprogram(control_memory* cm, const char* filename);
void fetch_microinstruction(control_memory* cm, mpc* p, mir* m);
void update_control(mpc* p, mmux* mmux, mir* m, struct mbr* mb);
int bits_to_int(int bits[], int size);
void int_to_bits(int value, int bits[], int size);

#endif