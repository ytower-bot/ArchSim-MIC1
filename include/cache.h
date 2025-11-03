#ifndef CACHE_H
#define CACHE_H

#define CACHE_SIZE 8
#define LINE_WORDS 4
#define TAG_BITS 7
#define LINE_BITS 3
#define WORD_BITS 2

// Cache line structure
typedef struct cache_line {
    int valid;
    int tag[TAG_BITS];
    int data[LINE_WORDS][16];
} cache_line;

// Cache structure
typedef struct cache {
    cache_line lines[CACHE_SIZE];
    int hits;
    int misses;
} cache;

// Address decomposition
typedef struct address_fields {
    int tag[TAG_BITS];
    int line[LINE_BITS];
    int word[WORD_BITS];
} address_fields;

int cache_read(cache* c, struct memory* mem, int address[12], int data[16]);
void cache_write(cache* c, struct memory* mem, int address[12], int data[16]);
int cache_lookup(cache* c, address_fields* addr);
void cache_load_block(cache* c, struct memory* mem, address_fields* addr);
void init_cache(cache* c);
void decompose_address(int address[12], address_fields* addr);
int compare_tags(int tag1[TAG_BITS], int tag2[TAG_BITS]);
int line_index_to_int(int line[LINE_BITS]);
int word_offset_to_int(int word[WORD_BITS]);
void copy_tag(int dest[TAG_BITS], int src[TAG_BITS]);
void print_cache_stats(cache* c);
void reset_cache_stats(cache* c);
double get_hit_rate(cache* c);
void print_cache_line(cache_line* line, int line_num);
void print_cache_state(cache* c);

#endif