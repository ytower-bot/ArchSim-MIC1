#ifndef MEMORY_H
#define MEMORY_H

#include "datapath.h"
#include "shifter.h"

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

// Funções
void init_mar(mar* a);
void init_mbr(mbr* b);
void init_memory(memory* mem);

void run_mar(mar* a, latch* lB);
void run_mbr(mar* a, mbr* b, memory* mem, shifter* s);

void m_read(mar* a, mbr* b, memory* mem);
void m_write(mar* a, mbr* b, memory* mem);

void load_program(memory* mem, const char* filename);
#endif