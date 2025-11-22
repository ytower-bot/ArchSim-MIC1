#include "shifter.h"
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
    s = (shifter*) malloc (sizeof(shifter));
    int input[16] = {0};
    set_shifter_input(s, input);
    int control[2] = {0};
    set_shifter_control(s, control);

}
