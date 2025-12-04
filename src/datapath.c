#include <stdio.h>
#include "../include/datapath.h"
#include "../include/shifter.h" // Necessário para a struct shifter
#include "../include/utils/conversions.h" // Para int_to_array e copy_array

// Auxiliar para converter binário [4] para índice int
static int control_to_index(int control[4]) {
    int index = 0;
    // control[0] é MSB, control[3] é LSB
    if (control[0]) index += 8;
    if (control[1]) index += 4;
    if (control[2]) index += 2;
    if (control[3]) index += 1;
    return index;
}

// Auxiliar para mapear índice -> ponteiro do registrador
// Ordem baseada na Fig. 4.8 do Tanenbaum
static mic1_register* get_register_by_index(register_bank* rb, int index) {
    switch (index) {
        case 0:  return &rb->PC;
        case 1:  return &rb->AC;
        case 2:  return &rb->SP;
        case 3:  return &rb->IR;
        case 4:  return &rb->TIR;
        case 5:  return &rb->R0;    // Constante 0
        case 6:  return &rb->R1;    // Constante +1
        case 7:  return &rb->Rm1;   // Constante -1
        case 8:  return &rb->AMASK; // 007777 (octal)
        case 9:  return &rb->SMASK; // 000377 (octal)
        case 10: return &rb->A;
        case 11: return &rb->B;
        case 12: return &rb->C;
        case 13: return &rb->D;
        case 14: return &rb->E;
        case 15: return &rb->F;
        default: return NULL;
    }
}

void init_register(mic1_register* r) {
    if (!r) return;
    for (int i = 0; i < 16; i++) r->data[i] = 0;
}

void init_register_bank(register_bank* rb) {
    if (!rb) return;

    // Inicializa todos com zero primeiro
    init_register(&rb->PC);
    init_register(&rb->AC);
    init_register(&rb->SP);
    init_register(&rb->IR);
    init_register(&rb->TIR);
    init_register(&rb->A);
    init_register(&rb->B);
    init_register(&rb->C);
    init_register(&rb->D);
    init_register(&rb->E);
    init_register(&rb->F);

    // Inicializa Constantes Específicas do MIC-1 [Tanenbaum p.150]
    
    // 0: Constante Zero
    init_register(&rb->R0); // Já zerado acima
    
    // +1: Constante Um
    int_to_array(1, rb->R1.data);
    
    // -1: Constante Menos Um (Todos os bits 1 em complemento de 2)
    int_to_array(-1, rb->Rm1.data);
    
    // AMASK: Address Mask (007777 octal = 0xFFF hex = 4095 decimal)
    // Usado para mascarar endereços de 12 bits
    int_to_array(0xFFF, rb->AMASK.data); 
    
    // SMASK: Stack Mask (000377 octal = 0xFF hex = 255 decimal)
    // Usado para mascarar offsets de 8 bits
    int_to_array(0xFF, rb->SMASK.data);
}

void init_decoder(decoder* d, register_bank* rb) {
    if (!d || !rb) return;
    d->rb = rb;
    for(int i = 0; i < 4; i++) d->control[i] = 0;
}

void init_decoderC(decoderC* d, register_bank* rb) {
    if (!d || !rb) return;
    d->rb = rb;
    for(int i = 0; i < 4; i++) d->control_c[i] = 0;
    d->control_enc = 0;
}

// Execução do Decodificador de Leitura (Para Barramentos A e B)
// Seleciona um registrador e joga seu valor no Latch
void run_decoder(decoder* d, latch* l) {
    int reg_index = control_to_index(d->control);
    mic1_register* selected = get_register_by_index(d->rb, reg_index);

    if (selected != NULL) {
        copy_array(selected->data, l->data); // Copia Reg -> Latch
    } else {
        // Fallback seguro
        for(int i=0; i<16; i++) l->data[i] = 0;
    }
}

// Execução do Decodificador de Escrita (Para Barramento C)
// Seleciona um registrador e grava o valor do Shifter nele
void run_decoderC(decoderC* d, shifter* s) {
    if (!d || !s) return;
    
    // Verifica sinal ENC (Enable C)
    if (d->control_enc == 0) return;

    int reg_index = control_to_index(d->control_c);

    // PROTEÇÃO DE HARDWARE
    // Tentativas de escrever nos registradores 5 a 9 (Constantes) são ignoradas
    if (reg_index >= 5 && reg_index <= 9) {
        return; 
    }

    mic1_register* selected = get_register_by_index(d->rb, reg_index);
    
    if (selected != NULL) {
        copy_array(s->data, selected->data); // Copia Shifter -> Reg
    }
}