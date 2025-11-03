#ifndef CACHE_H
#define CACHE_H

/**
 * @file cache.h
 * @brief Sistema de Cache do MIC-1
 * 
 * Cache de mapeamento direto com:
 * - 8 linhas de cache
 * - 4 palavras por linha (16 bits cada)
 * - Endereço de 12 bits dividido em: Tag(7) + Índice(3) + Offset(2)
 * - Política de escrita: escrita imediata na memória principal
 */

#define CACHE_SIZE 8        // 8 lines of cache
#define LINE_WORDS 4        // 4 words per line
#define TAG_BITS 7          // 7 bits for tag
#define LINE_BITS 3         // 3 bits for line index
#define WORD_BITS 2         // 2 bits for word offset

// Cache line
typedef struct cache_line {
    int valid;                      // Valid bit: 0=invalid, 1=valid
    int tag[TAG_BITS];             // 7-bit tag
    int data[LINE_WORDS][16];      // 4 words of 16 bits each
} cache_line;

// Cache structure
typedef struct cache {
    cache_line lines[CACHE_SIZE];   // 8 lines of cache
    int hits;                       // Hit counter
    int misses;                     // Miss counter
} cache;

// address decomposition
typedef struct address_fields {
    int tag[TAG_BITS];     // 7 most significant bits
    int line[LINE_BITS];   // 3 middle bits
    int word[WORD_BITS];   // 2 least significant bits
} address_fields;

// Main cache functions
int cache_read(cache* c, struct memory* mem, int address[12], int data[16]);
void cache_write(cache* c, struct memory* mem, int address[12], int data[16]);
int cache_lookup(cache* c, address_fields* addr);
void cache_load_block(cache* c, struct memory* mem, address_fields* addr);

// Auxiliary functions
void init_cache(cache* c);
void decompose_address(int address[12], address_fields* addr);
int compare_tags(int tag1[TAG_BITS], int tag2[TAG_BITS]);
int line_index_to_int(int line[LINE_BITS]);
int word_offset_to_int(int word[WORD_BITS]);
void copy_tag(int dest[TAG_BITS], int src[TAG_BITS]);

// stats functions
void print_cache_stats(cache* c);
void reset_cache_stats(cache* c);
double get_hit_rate(cache* c);

// Debug functions
void print_cache_line(cache_line* line, int line_num);
void print_cache_state(cache* c);

#endif // CACHE_H