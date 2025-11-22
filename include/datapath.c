#include "datapath.h"



void init_decoder(decoder*d, register_bank*rb){
    d = (decoder*) malloc(sizeof(decoder));
    d->rb = rb;
    for(int i = 0; i < 4; i++) d->control[i] = 0;
}

void init_decoderC(decoderC*d, register_bank*rb){
    d = (decoderC*) malloc(sizeof(decoderC));
    d->rb = rb;
    for(int i = 0; i < 4; i++) d->control_c[i] = 0;
    d->control_enc = 0;
}

void init_register(mic1_register*r, int v){
    for(int i = 0; i < 16; i++){
        r->data[i] = v;
    }
}
