#ifndef ALU_H
#define ALU_H

typedef struct alu {
    int input_a[16];
    int input_b[16];
    int output[16];
    int control[2];
    int flag_n;
    int flag_z;
} alu;

#define ALU_A_PLUS_B    0b00
#define ALU_A_AND_B     0b01
#define ALU_A           0b10
#define ALU_NOT_A       0b11

void run_alu(alu* a);
void set_alu_inputs(alu* a, int input_a[16], int input_b[16]);
void set_alu_control(alu* a, int control);
void update_flags(alu* a);
void alu_add(alu* a);
void alu_and(alu* a);
void alu_pass_a(alu* a);
void alu_not_a(alu* a);
void init_alu(alu* a);
int is_zero(int data[16]);
int is_negative(int data[16]);

#endif