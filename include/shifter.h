#ifndef SHIFTER_H
#define SHIFTER_H

/**
 * @file shifter.h
 * @brief Deslocador de bits do MIC-1
 * 
 * O deslocador recebe o resultado da ULA e pode:
 * - Não fazer nada (00)
 * - Deslocar 1 bit à esquerda (01) 
 * - Deslocar 1 bit à direita (10)
 * - Padrão não usado (11)
 * 
 * O resultado vai para o barramento C e pode ir para o MBR.
 */

 // shifter structe
typedef struct shifter {
    int control_sh[2];  // SH control signal (2 bits)
    int data[16];       // ALU output (16 bits)
} shifter;

// operation codes
#define SHIFT_NONE      0b00
#define SHIFT_LEFT      0b01
#define SHIFT_RIGHT     0b10
#define SHIFT_UNUSED    0b11    // default

// main functions
void run_shifter(shifter* s, struct mbr* b, struct busC* c);
void set_shifter_input(shifter* s, int input[16]);
void set_shifter_control(shifter* s, int control);

// operations
void lshift(shifter* s);
void rshift(shifter* s);

// utils
void init_shifter(shifter* s);
int get_shifter_control_value(int control[2]);

#endif