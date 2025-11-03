#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

/**
 * @file control_unit.h
 * @brief Unidade de Controle do MIC-1
 * 
 * Contém os componentes de controle:
 * - MIR (Microinstruction Register) - armazena e distribui microinstruções
 * - MPC (Microprogram Counter) - endereço da próxima microinstrução
 * - MMUX - seleciona próximo endereço (sequencial ou desvio)
 * - AMUX - multiplexador para entrada A da ULA
 */

#define MICROPROGRAM_SIZE 256  // Tamanho da memória de controle

// AMUX
typedef struct amux {
    int control_amux;   // AMUX signal: 0=Latch A, 1=MBR
} amux;

// MIR - Microinstruction Register (32 bits divided into fields)
typedef struct mir {
    int data[32];       // Microinstruction complete (32 bits)

    // Fields extracted from the microinstruction:
    int amux;           // [0] - AMUX signal
    int cond[2];        // [1-2] - COND signal
    int alu[2];         // [3-4] - ALU signal
    int sh[2];          // [5-6] - SH (shifter) signal
    int mbr;            // [7] - MBR signal
    int mar;            // [8] - MAR signal
    int rd;             // [9] - RD signal
    int wr;             // [10] - WR signal
    int enc;            // [11] - ENC signal
    int c[4];           // [12-15] - C signal
    int b[4];           // [16-19] - B signal
    int a[4];           // [20-23] - A signal
    int addr[8];        // [24-31] - ADDR signal
} mir;

// MPC - Microprogram Counter
typedef struct mpc {
    int address[8];     // Address of the next microinstruction (8 bits = 256 positions)
} mpc;

// MMUX
typedef struct mmux {
    int control_cond[2]; // COND signal (2 bits)
    int alu_n;          // ALU N flag
    int alu_z;          // ALU Z flag
} mmux;

// microinstruction memory
typedef struct control_memory {
    int microinstructions[MICROPROGRAM_SIZE][32]; // 256 microinstructions of 32 bits
} control_memory;

// Condition codes for MMUX
#define COND_NONE       0b00    // No branch (sequential)
#define COND_IF_N       0b01    // Branch if N = 1
#define COND_IF_Z       0b10    // Branch if Z = 1
#define COND_ALWAYS     0b11    // Always branch

// MIR
void run_mir(mir* m, struct mbr* mb, struct mar* ma, mmux* mmu, 
             amux* amu, struct shifter* s, struct alu* al, 
             struct decoder* da, struct decoder* db, struct decoderC* dc);
void decode_microinstruction(mir* m);
void init_mir(mir* m);

// MPC
void run_mpc(mpc* p, mir* m, control_memory* cm);
void increment_mpc(mpc* p);
void init_mpc(mpc* p);

// MMUX
void run_mmux(mmux* m, mpc* p, mir* mir);
int should_branch(mmux* m);
void init_mmux(mmux* m);

// AMUX
void run_amux(amux* a, struct mbr* b, struct latch* lA, struct alu* u);
void init_amux(amux* a);

// Control Memory
void init_control_memory(control_memory* cm);
void load_microprogram(control_memory* cm, const char* filename);

// Auxiliary functions
int bits_to_int(int bits[], int size);
void int_to_bits(int value, int bits[], int size);

#endif // CONTROL_UNIT_H