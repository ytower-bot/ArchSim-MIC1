#include "../include/mic1.h"
#include "../include/datapath.h"
#include "../include/utils/conversions.h"

void init_register_bank(register_bank *rb){
    for(int i = 0; i < 16; i++){
        rb->PC.data[i] = 0;
        rb->AC.data[i] = 0;
        rb->IR.data[i] = 0;
        rb->TIR.data[i] = 0;
    }

    for(int i = 0; i < 4; i++) rb->AMASK.data[i] = 0;
    for(int i = 4; i < 16; i++) rb->AMASK.data[i] = 1;
    for(int i = 0; i < 8; i++) rb->SMASK.data[i] = 0;
    for(int i = 8; i < 16; i++) rb->SMASK.data[i] = 1;

    for(int i = 0; i < 16; i++){
        rb->Rm1.data[i] = 1;
        rb->R0.data[i] = 0;
        rb->SP.data[i] = 0;
    }
    for(int i = 0; i < 4; i++) rb->SP.data[i] = 0;
    for(int i = 4; i < 16; i++) rb->SP.data[i] = 1;

    for(int i = 0; i < 15; i++) rb->R1.data[i] = 0;
    rb->R1.data[15] = 1;

    for(int i = 0; i < 16; i++){
        rb->A.data[i] = 0;
        rb->B.data[i] = 0;
        rb->C.data[i] = 0;
        rb->D.data[i] = 0;
        rb->E.data[i] = 0;
        rb->F.data[i] = 0;
    }
}

void init_decoder(decoder*d, register_bank*rb){
    if (!d || !rb) return;
    d->rb = rb;
    for(int i = 0; i < 4; i++) d->control[i] = 0;
}

void init_decoderC(decoderC*d, register_bank*rb){
    if (!d || !rb) return;
    d->rb = rb;
    for(int i = 0; i < 4; i++) d->control_c[i] = 0;
    d->control_enc = 0;
}

int control_to_index(int control[4]) {
    return (control[0] << 3) | (control[1] << 2) | (control[2] << 1) | control[3];
}

void run_decoder(decoder* d, latch* l) {
    int reg_index = control_to_index(d->control);

    mic1_register* selected_register = NULL;

    switch (reg_index) {
        case 0:  selected_register = &d->rb->PC; break;
        case 1:  selected_register = &d->rb->AC; break;
        case 2:  selected_register = &d->rb->IR; break;
        case 3:  selected_register = &d->rb->TIR; break;
        case 4:  selected_register = &d->rb->SP; break;
        case 5:  selected_register = &d->rb->AMASK; break;
        case 6:  selected_register = &d->rb->SMASK; break;
        case 7:  selected_register = &d->rb->R0; break;
        case 8:  selected_register = &d->rb->R1; break;
        case 9:  selected_register = &d->rb->Rm1; break;
        case 10: selected_register = &d->rb->A; break;
        case 11: selected_register = &d->rb->B; break;
        case 12: selected_register = &d->rb->C; break;
        case 13: selected_register = &d->rb->D; break;
        case 14: selected_register = &d->rb->E; break;
        case 15: selected_register = &d->rb->F; break;
        default:
            printf("Erro: Sinal de controle do decoder inválido (%d).\n", reg_index);
            return;
    }

    if (selected_register != NULL) {
        copy_array(selected_register->data, l->data);
    }
}

void run_decoderC(decoderC* d, shifter* s) {
    if (!d || !s) return;

    if (d->control_enc == 0) return;
    
    if (!d->rb) {
        printf("Erro: decoderC->rb é NULL!\n");
        return;
    }

    int reg_index = control_to_index(d->control_c);

    mic1_register* selected_register = NULL;

    switch (reg_index) {
        case 0:  selected_register = &d->rb->PC; break;
        case 1:  selected_register = &d->rb->AC; break;
        case 2:  selected_register = &d->rb->IR; break;
        case 3:  selected_register = &d->rb->TIR; break;
        case 4:  selected_register = &d->rb->SP; break;
        case 5:  selected_register = &d->rb->AMASK; break;
        case 6:  selected_register = &d->rb->SMASK; break;
        case 7:  selected_register = &d->rb->R0; break;
        case 8:  selected_register = &d->rb->R1; break;
        case 9:  selected_register = &d->rb->Rm1; break;
        case 10: selected_register = &d->rb->A; break;
        case 11: selected_register = &d->rb->B; break;
        case 12: selected_register = &d->rb->C; break;
        case 13: selected_register = &d->rb->D; break;
        case 14: selected_register = &d->rb->E; break;
        case 15: selected_register = &d->rb->F; break;
        default:
            printf("Erro: Sinal de controle do decoderC inválido (%d).\n", reg_index);
            return;
    }

    if (selected_register != NULL) {
        copy_array(s->data, selected_register->data);
    }
}
