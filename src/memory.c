#include "../include/memory.h"
#include "../include/datapath.h"
#include "../include/shifter.h"
#include "../include/cache.h"
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

void run_mbr(mar* a, mbr* b, memory* mem, shifter* s, cache* cache) {
    if (b->control_rd == 1) {
        m_read(a, b, mem, cache);
    }
    if (b->control_wr == 1) {
        m_write(a, b, mem, cache);
    }
    if (b->control_mbr == 1) {
        copy_array(s->data, b->data);
    }
}

void init_memory(memory* mem) {
    if (!mem) return;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        for (int j = 0; j < 16; j++) {
            mem->data[i][j] = 0;
        }
    }
}

void m_read(mar* a, mbr* b, memory* mem, cache* c) {
    if (!a || !b || !mem) return;

    int addr = address_to_int(a->address);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        printf("Erro: Endereço de memória inválido: %d\n", addr);
        return;
    }

    if (c) {
        cache_read(c, mem, a->address, b->data);
    } else {

        copy_data(b->data, mem->data[addr]);
    }
}

void m_write(mar* a, mbr* b, memory* mem, cache* c) {
    if (!a || !b || !mem) return;

    int addr = address_to_int(a->address);
    if (addr < 0 || addr >= MEMORY_SIZE) {
        printf("Erro: Endereço de memória inválido: %d\n", addr);
        return;
    }

    if (c) {
        cache_write(c, mem, a->address, b->data);
    } else {

        copy_data(mem->data[addr], b->data);
    }
}

void load_program(memory* mem, const char* filename) {
    if (!mem || !filename) return;

    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Erro: Não foi possível abrir arquivo %s\n", filename);
        return;
    }

    int addr = 0;
    int word[16];

    while (addr < MEMORY_SIZE && !feof(file)) {

        int low_byte = fgetc(file);
        int high_byte = fgetc(file);

        if (low_byte == EOF || high_byte == EOF) break;

        // Little-endian: low byte first, high byte second
        int value = (high_byte << 8) | low_byte;
        int_to_bits(value, word, 16);

        copy_data(mem->data[addr], word);
        addr++;
    }

    fclose(file);
    printf("Programa carregado: %d palavras lidas de %s\n", addr, filename);
}