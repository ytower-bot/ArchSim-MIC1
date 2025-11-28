#include "../include/memory.h"
#include "../include/datapath.h"
#include "../include/shifter.h"
#include "../include/utils/conversions.h"
#include <stdio.h>
#include <stdlib.h>

void init_mar(mar* a) {
    a->control_mar = 0;
    for (int i = 0; i < 12; i++) {
        a->address[i] = 0;
    }
}

void run_mar(mar* a, latch* lB) {
    if (a->control_mar == 1) {
        int j = 4;
        for(int i = 0; i<12;i++){
            a->address[i] = lB->data[j++];
        }
    }
}

void init_mbr(mbr* b) {
    b->control_rd = 0;
    b->control_wr = 0;
    b->control_mbr = 0;
    for (int i = 0; i < 16; i++) {
        b->data[i] = 0;
    }
}

void run_mbr(mar* a, mbr* b, memory* mem, shifter* s) {
    if (b->control_rd == 1) {
        m_read(a, b, mem);
    }
    if (b->control_wr == 1) {
        m_write(a, b, mem);
    }
    if (b->control_mbr == 1) {
        copy_array(s->data, b->data);
    }
}