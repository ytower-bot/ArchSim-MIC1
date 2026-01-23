#include "../include/shifter.h"
#include "../include/memory.h"
#include "../include/connections.h"

#include <stdio.h>
#include <stdlib.h>

void lshift(shifter*s){
    for(int i = 0; i < 15; i++){
        s->data[i] = s->data[i+1];
    }
    s->data[15] = 0;
}

void rshift(shifter*s){
    /*
     * Arithmetic right shift (SRA1) per MIC-1 specification.
     * Shifts data one bit position right, preserving sign bit.
     * Bit 0 is MSB (sign bit), bit 15 is LSB.
     */
    int sign_bit = s->data[0]; /* Save sign bit */
    for(int i = 15; i > 0; i--){
        s->data[i] = s->data[i-1];
    }
    s->data[0] = sign_bit; /* Replicate sign bit for arithmetic shift */
}

void set_shifter_input(shifter* s, int input[16]){
    for(int i = 0; i < 16; i++){
        s->data[i] = input[i];
    }
}

void set_shifter_control(shifter* s, int control[2]){
    s->control_sh[0] = control[0];
    s->control_sh[1] = control[1];
}

void init_shifter(shifter*s){
    if (!s) return;

    for (int i = 0; i < 16; i++) {
        s->data[i] = 0;
    }

    s->control_sh[0] = 0;
    s->control_sh[1] = 0;
}

void run_shifter(shifter* s, struct mbr* b, struct barrC* c) {
    if (!s) return;

    int control_value = (s->control_sh[1] << 1) | s->control_sh[0];

    switch (control_value) {
        case SHIFT_NONE:
            break;
        case SHIFT_RIGHT:
            rshift(s);
            break;
        case SHIFT_LEFT:
            lshift(s);
            break;
        case SHIFT_UNUSED:
            break;
    }

    (void)b;
    (void)c;
}

int get_shifter_control_value(int control[2]) {
    if (!control) return 0;
    return (control[0] << 1) | control[1];
}
