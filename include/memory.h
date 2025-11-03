#ifndef MEMORY_H
#define MEMORY_H

#define MEMORY_SIZE 4096

// Memory Address Register
typedef struct mar {
    int control_mar;
    int address[12];
} mar;

// Memory Buffer Register
typedef struct mbr {
    int control_rd;
    int control_wr;
    int control_mbr;
    int data[16];
} mbr;

// Main Memory
typedef struct memory {
    int data[MEMORY_SIZE][16];
} memory;

void run_mar(mar* a, struct latch* lB);
void init_mar(mar* a);
void run_mbr(mar* a, mbr* b, memory* mem);
void m_read(mar* a, mbr* b, memory* mem);
void m_write(mar* a, mbr* b, memory* mem);
void init_mbr(mbr* b);
void init_memory(memory* mem);
void load_program(memory* mem, const char* filename);
int address_to_int(int address[12]);
void int_to_address(int addr, int address[12]);
void copy_data(int dest[16], int src[16]);

#endif