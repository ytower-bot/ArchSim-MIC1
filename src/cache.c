#include "../include/cache.h"
#include "../include/memory.h"
#include "../include/utils/conversions.h"
#include <stdio.h>

/**
 * Inicializa cache com todas linhas inválidas.
 */
void init_cache(cache* c) {
    if (!c) return;
    
    c->hits = 0;
    c->misses = 0;
    
    for (int i = 0; i < CACHE_SIZE; i++) {
        c->lines[i].valid = 0;
        
        // Zera tag
        for (int j = 0; j < TAG_BITS; j++) {
            c->lines[i].tag[j] = 0;
        }
        
        // Zera dados
        for (int j = 0; j < LINE_WORDS; j++) {
            for (int k = 0; k < 16; k++) {
                c->lines[i].data[j][k] = 0;
            }
        }
    }
}

/**
 * Decompõe endereço de 12 bits em: tag(7), line(3), word(2).
 */
void decompose_address(int address[12], address_fields* addr) {
    if (!address || !addr) return;
    
    // Tag: bits 0-6 (7 bits)
    for (int i = 0; i < TAG_BITS; i++) {
        addr->tag[i] = address[i];
    }
    
    // Line: bits 7-9 (3 bits)
    for (int i = 0; i < LINE_BITS; i++) {
        addr->line[i] = address[TAG_BITS + i];
    }
    
    // Word: bits 10-11 (2 bits)
    for (int i = 0; i < WORD_BITS; i++) {
        addr->word[i] = address[TAG_BITS + LINE_BITS + i];
    }
}

/**
 * Compara duas tags de 7 bits.
 * Retorna 1 se iguais, 0 caso contrário.
 */
int compare_tags(int tag1[TAG_BITS], int tag2[TAG_BITS]) {
    if (!tag1 || !tag2) return 0;
    
    for (int i = 0; i < TAG_BITS; i++) {
        if (tag1[i] != tag2[i]) return 0;
    }
    return 1;
}

/**
 * Converte line[3] para índice inteiro (0-7).
 */
int line_index_to_int(int line[LINE_BITS]) {
    if (!line) return 0;
    return bits_to_int(line, LINE_BITS);
}

/**
 * Converte word[2] para offset inteiro (0-3).
 */
int word_offset_to_int(int word[WORD_BITS]) {
    if (!word) return 0;
    return bits_to_int(word, WORD_BITS);
}

/**
 * Copia tag de 7 bits.
 */
void copy_tag(int dest[TAG_BITS], int src[TAG_BITS]) {
    if (!dest || !src) return;
    
    for (int i = 0; i < TAG_BITS; i++) {
        dest[i] = src[i];
    }
}

/**
 * Verifica se endereço está no cache.
 * Retorna 1 (hit) ou 0 (miss), atualiza estatísticas.
 */
int cache_lookup(cache* c, address_fields* addr) {
    if (!c || !addr) return 0;
    
    int line_idx = line_index_to_int(addr->line);
    cache_line* line = &c->lines[line_idx];
    
    // Verifica valid bit e compara tags
    if (line->valid && compare_tags(line->tag, addr->tag)) {
        c->hits++;
        return 1; // HIT
    }
    
    c->misses++;
    return 0; // MISS
}

/**
 * Carrega bloco de 4 palavras da memória para cache.
 */
void cache_load_block(cache* c, memory* mem, address_fields* addr) {
    if (!c || !mem || !addr) return;
    
    int line_idx = line_index_to_int(addr->line);
    cache_line* line = &c->lines[line_idx];
    
    // Copia tag
    copy_tag(line->tag, addr->tag);
    
    // Calcula endereço base do bloco (ignora word offset)
    int base_addr = 0;
    for (int i = 0; i < TAG_BITS; i++) {
        base_addr = base_addr * 2 + addr->tag[i];
    }
    for (int i = 0; i < LINE_BITS; i++) {
        base_addr = base_addr * 2 + addr->line[i];
    }
    base_addr = base_addr * 4; // Multiplica por LINE_WORDS
    
    // Carrega 4 palavras da memória
    for (int i = 0; i < LINE_WORDS; i++) {
        int word_addr = base_addr + i;
        if (word_addr < MEMORY_SIZE) {
            copy_data(line->data[i], mem->data[word_addr]);
        }
    }
    
    line->valid = 1;
}

/**
 * Lê palavra do cache ou memória.
 * Se miss, carrega bloco primeiro.
 */
int cache_read(cache* c, memory* mem, int address[12], int data[16]) {
    if (!c || !mem || !address || !data) return 0;
    
    address_fields addr;
    decompose_address(address, &addr);
    
    int hit = cache_lookup(c, &addr);
    
    if (!hit) {
        // MISS: carrega bloco da memória
        cache_load_block(c, mem, &addr);
    }
    
    // Lê palavra do cache
    int line_idx = line_index_to_int(addr.line);
    int word_offset = word_offset_to_int(addr.word);
    
    copy_data(data, c->lines[line_idx].data[word_offset]);
    
    return hit;
}

/**
 * Escreve palavra no cache e memória (write-through).
 */
void cache_write(cache* c, memory* mem, int address[12], int data[16]) {
    if (!c || !mem || !address || !data) return;
    
    address_fields addr;
    decompose_address(address, &addr);
    
    // Write-through: sempre escreve na memória
    int mem_addr = address_to_int(address);
    if (mem_addr >= 0 && mem_addr < MEMORY_SIZE) {
        copy_data(mem->data[mem_addr], data);
    }
    
    // Se hit, atualiza cache também
    int line_idx = line_index_to_int(addr.line);
    cache_line* line = &c->lines[line_idx];
    
    if (line->valid && compare_tags(line->tag, addr.tag)) {
        int word_offset = word_offset_to_int(addr.word);
        copy_data(line->data[word_offset], data);
    }
}

/**
 * Imprime estatísticas do cache.
 */
void print_cache_stats(cache* c) {
    if (!c) return;
    
    printf("=== CACHE STATS ===\n");
    printf("Hits: %d\n", c->hits);
    printf("Misses: %d\n", c->misses);
    
    int total = c->hits + c->misses;
    if (total > 0) {
        double hit_rate = (double)c->hits / total * 100.0;
        printf("Hit Rate: %.2f%%\n", hit_rate);
    } else {
        printf("Hit Rate: 0.00%%\n");
    }
    printf("==================\n");
}

/**
 * Reseta estatísticas do cache.
 */
void reset_cache_stats(cache* c) {
    if (!c) return;
    c->hits = 0;
    c->misses = 0;
}

/**
 * Retorna taxa de acerto do cache.
 */
double get_hit_rate(cache* c) {
    if (!c) return 0.0;
    
    int total = c->hits + c->misses;
    if (total == 0) return 0.0;
    
    return (double)c->hits / total * 100.0;
}

/**
 * Imprime linha específica do cache.
 */
void print_cache_line(cache_line* line, int line_num) {
    if (!line) return;
    
    printf("Line %d: ", line_num);
    if (!line->valid) {
        printf("INVALID\n");
        return;
    }
    
    printf("Valid | Tag: ");
    for (int i = 0; i < TAG_BITS; i++) {
        printf("%d", line->tag[i]);
    }
    printf(" | Data: [%d words]\n", LINE_WORDS);
}

/**
 * Imprime estado completo do cache.
 */
void print_cache_state(cache* c) {
    if (!c) return;
    
    printf("=== CACHE STATE ===\n");
    for (int i = 0; i < CACHE_SIZE; i++) {
        print_cache_line(&c->lines[i], i);
    }
    print_cache_stats(c);
    printf("==================\n");
}
