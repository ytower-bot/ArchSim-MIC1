#include <string.h>
#include <stdint.h>
#include "../include/alu.h"

void init_alu(alu* a) {
    for (int i = 0; i < 16; i++) {
        a->input_a[i] = 0;
        a->input_b[i] = 0;
        a->output[i] = 0;
    }
    a->control[0] = 0;
    a->control[1] = 0;
    a->flag_n = 0;
    a->flag_z = 0;
}

void set_alu_inputs(alu* a, int input_a[16], int input_b[16]) {
    memcpy(a->input_a, input_a, 16 * sizeof(int));
    memcpy(a->input_b, input_b, 16 * sizeof(int));
}

void set_alu_control(alu* a, int control) {
    a->control[0] = control & 0x01;
    a->control[1] = (control >> 1) & 0x01;
}

int is_zero(int data[16]) {
    for (int i = 0; i < 16; i++) {
        if (data[i] != 0) {
            return 0;
        }
    }
    return 1;
}

int is_negative(int data[16]) {
    return data[0];
}

void update_flags(alu* a) {
    a->flag_n = is_negative(a->output);
    a->flag_z = is_zero(a->output);
}

void alu_add(alu* a) {
    int val_a = 0, val_b = 0;
    
    for (int i = 0; i < 16; i++) {
        val_a = (val_a << 1) | a->input_a[i];
        val_b = (val_b << 1) | a->input_b[i];
    }
    
    int result = (int16_t)val_a + (int16_t)val_b;
    result = result & 0xFFFF;
    
    for (int i = 0; i < 16; i++) {
        a->output[15 - i] = (result >> i) & 1;
    }
}

void alu_and(alu* a) {
    for (int i = 0; i < 16; i++) {
        a->output[i] = a->input_a[i] & a->input_b[i];
    }
}

void alu_pass_a(alu* a) {
    memcpy(a->output, a->input_a, 16 * sizeof(int));
}

void alu_not_a(alu* a) {
    for (int i = 0; i < 16; i++) {
        a->output[i] = 1 - a->input_a[i];
    }
}

void run_alu(alu* a) {
    int ctrl = (a->control[1] << 1) | a->control[0];
    
    switch (ctrl) {
        case 0:
            alu_add(a);
            break;
        case 1:
            alu_and(a);
            break;
        case 2:
            alu_pass_a(a);
            break;
        case 3:
            alu_not_a(a);
            break;
    }
    
    update_flags(a);
}
