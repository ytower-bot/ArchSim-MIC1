#include "../include/mic1.h"
#include "../include/datapath.h"

void init_register_bank(register_bank *rb){
    rb = (register_bank*) malloc(sizeof(register_bank));
    for(int i = 0; i < 4; i++) rb->AMASK.data[i] = 0;
    for(int i = 4; i < 16; i++) rb->AMASK.data[i] = 1;
    for(int i = 0; i < 16; i++){
        rb->Rm1.data[i] = 1;
        rb->R0.data[i] = 0;
        rb->SP.data[i] = 0;
    } 
    rb->SP.data[3] = 1;
    for(int i = 0; i < 15; i++) rb->Rm1.data[i] = 0;
    rb->R1.data[15] = 1;
}