#include "../include/shifter.h"
#include "../include/datapath.h"
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
    for(int i = 15; i > 0; i--){
        s->data[i] = s->data[i-1];
    }
    s->data[0] = 0;
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
    
    // Zera data[16]
    for (int i = 0; i < 16; i++) {
        s->data[i] = 0;
    }
    
    // Zera control_sh[2]
    s->control_sh[0] = 0;
    s->control_sh[1] = 0;
}

/**
 * Executa operação do shifter baseada no control.
 * control_sh[2]: 00=nada, 01=left, 10=right, 11=unused
 */
void run_shifter(shifter* s, mbr* b, barrC* c) {
    if (!s) return;
    
    int control_value = (s->control_sh[0] << 1) | s->control_sh[1];
    
    switch (control_value) {
        case SHIFT_NONE:    // 00 - não faz nada
            break;
        case SHIFT_LEFT:    // 01 - shift left
            lshift(s);
            break;
        case SHIFT_RIGHT:   // 10 - shift right
            rshift(s);
            break;
        case SHIFT_UNUSED:  // 11 - não usado
            break;
    }
    
    // Parâmetros b e c não usados nesta versão simplificada
    (void)b;
    (void)c;
}

/**
 * Converte control[2] para valor inteiro.
 */
int get_shifter_control_value(int control[2]) {
    if (!control) return 0;
    return (control[0] << 1) | control[1];
}
