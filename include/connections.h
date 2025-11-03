#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "datapath.h"
#include "alu.h"
#include "memory.h"
#include "shifter.h"
#include "control_unit.h"
#include "cache.h"

// Bus C structure
typedef struct barrC {
    int data[16];
} barrC;

void run_decoderC(decoderC* d, shifter* s);
void run_shifter(shifter* s, mbr* b, barrC* c);
void run_amux(amux* a, mbr* b, latch* lA, alu* u);
void run_mir(mir* m, mbr* mb, mar* ma, mmux* mmu, 
             amux* amu, shifter* s, alu* al, 
             decoder* da, decoder* db, decoderC* dc);
void connect_all_components(register_bank* rb, latch* la, latch* lb,
                           decoder* da, decoder* db, decoderC* dc,
                           alu* alu, shifter* s, mar* mar, mbr* mbr,
                           cache* cache, memory* mem, barrC* bus_c);

#endif