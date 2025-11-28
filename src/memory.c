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

/**
 * Inicializa memória principal com zeros.
 * Zera 4096 posições × 16 bits cada.
 */
void init_memory(memory* mem) {
    if (!mem) return;
    
    for (int i = 0; i < MEMORY_SIZE; i++) {
        for (int j = 0; j < 16; j++) {
            mem->data[i][j] = 0;
        }
    }
}

/**
 * Lê palavra da memória para MBR.
 * Converte MAR (12 bits) → índice (0-4095)
 * Copia memory[addr] → MBR.data
 */
void m_read(mar* a, mbr* b, memory* mem) {
    if (!a || !b || !mem) return;
    
    // Converte endereço MAR[12] para índice inteiro
    int addr = address_to_int(a->address);
    
    // Validar range (0-4095)
    if (addr < 0 || addr >= MEMORY_SIZE) {
        printf("Erro: Endereço de memória inválido: %d\n", addr);
        return;
    }
    
    // Copia palavra da memória para MBR
    copy_data(b->data, mem->data[addr]);
}

/**
 * Escreve palavra do MBR na memória.
 * Converte MAR (12 bits) → índice (0-4095)
 * Copia MBR.data → memory[addr]
 */
void m_write(mar* a, mbr* b, memory* mem) {
    if (!a || !b || !mem) return;
    
    // Converte endereço MAR[12] para índice inteiro
    int addr = address_to_int(a->address);
    
    // Validar range (0-4095)
    if (addr < 0 || addr >= MEMORY_SIZE) {
        printf("Erro: Endereço de memória inválido: %d\n", addr);
        return;
    }
    
    // Copia palavra do MBR para memória
    copy_data(mem->data[addr], b->data);
}

/**
 * Carrega programa de arquivo binário na memória.
 * Formato esperado: sequência de palavras de 16 bits.
 */
void load_program(memory* mem, const char* filename) {
    if (!mem || !filename) return;
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Erro: Não foi possível abrir arquivo %s\n", filename);
        return;
    }
    
    int addr = 0;
    int word[16];
    
    // Ler palavras de 16 bits do arquivo
    while (addr < MEMORY_SIZE && !feof(file)) {
        // Ler 16 bits (assumindo formato específico)
        // Implementação simplificada - ajustar conforme formato do arquivo
        int byte1 = fgetc(file);
        int byte2 = fgetc(file);
        
        if (byte1 == EOF || byte2 == EOF) break;
        
        // Converter bytes para array de bits
        int value = (byte1 << 8) | byte2;
        int_to_bits(value, word, 16);
        
        // Copiar para memória
        copy_data(mem->data[addr], word);
        addr++;
    }
    
    fclose(file);
    printf("Programa carregado: %d palavras lidas de %s\n", addr, filename);
}