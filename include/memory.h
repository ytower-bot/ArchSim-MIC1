#ifndef MEMORY_H
#define MEMORY_H

/**
 * @file memory.h
 * @brief Componentes de memória do MIC-1
 * 
 * - MAR (Memory Address Register) - memory address
 * - MBR (Memory Buffer Register) - memory data
 * - Main memory (4096 positions of 16 bits)
 */

#define MEMORY_SIZE 4096

// MAR
typedef struct mar {
    int control_mar;    // 0 = dont load, 1 = load from latch B
    int address[12];    // address stored (12 bits)
} mar;

// MBR
typedef struct mbr {
    int control_rd;     // 0 = dont read, 1 = read from memory to MBR
    int control_wr;     // 0 = dont write, 1 = write MBR to memory
    int control_mbr;    // 0 = dont load, 1 = load from shifter
    int data[16];       // data stored (16 bits)
} mbr;

typedef struct memory {
    int data[MEMORY_SIZE][16];  // 4096 positions of 16 bits each
} memory;

// MAR functions
void run_mar(mar* a, struct latch* lB);
void init_mar(mar* a);

// MBR functions
void run_mbr(mar* a, mbr* b, memory* mem);
void m_read(mar* a, mbr* b, memory* mem);
void m_write(mar* a, mbr* b, memory* mem);
void init_mbr(mbr* b);

// Memory functions
void init_memory(memory* mem);
void load_program(memory* mem, const char* filename);

// auxiliary functions
int address_to_int(int address[12]);
void int_to_address(int addr, int address[12]);
void copy_data(int dest[16], int src[16]);

#endif // MEMORY_H