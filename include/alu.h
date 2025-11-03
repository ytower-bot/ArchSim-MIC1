#ifndef ALU_H
#define ALU_H

/**
 * @file alu.h
 * @brief Unidade Lógica Aritmética do MIC-1
 * 
 * A ULA executa operações aritméticas e lógicas entre duas entradas:
 * - Entrada A: vem do AMUX (Latch A ou MBR)
 * - Entrada B: vem do Latch B
 * 
 * Produz flags N (negativo) e Z (zero) para controle de fluxo.
 */

// ALU structure
typedef struct alu {
    int input_a[16];    // Input A (16 bits)
    int input_b[16];    // Input B (16 bits)
    int output[16];     // Output (16 bits)
    int control[2];     // ALU control signal (2 bits)
    int flag_n;         // Flag N: 1 if result is negative, 0 otherwise
    int flag_z;         // Flag Z: 1 if result is zero, 0 otherwise
} alu;

// ALU operation codes (2-bit control)
#define ALU_A_PLUS_B    0b00    // A + B
#define ALU_A_AND_B     0b01    // A AND B
#define ALU_A           0b10    // A only
#define ALU_NOT_A       0b11    // NOT A

// ALU functions
void run_alu(alu* a);
void set_alu_inputs(alu* a, int input_a[16], int input_b[16]);
void set_alu_control(alu* a, int control);
void update_flags(alu* a);

// Specific operations
void alu_add(alu* a);
void alu_and(alu* a);
void alu_pass_a(alu* a);
void alu_not_a(alu* a);

// Auxiliary functions
void init_alu(alu* a);
int is_zero(int data[16]);
int is_negative(int data[16]);

#endif // ALU_H