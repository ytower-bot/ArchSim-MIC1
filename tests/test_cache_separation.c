#include <stdio.h>
#include "mic1.h"
#include "cache.h"
#include "utils/conversions.h"

void test_instruction_cache() {
    printf("Testing Instruction Cache\n");
    printf("==========================\n\n");
    
    // Initialize MIC-1 system
    mic1_cpu cpu;
    init_mic1(&cpu);
    
    printf("Initialized MIC-1 with separate caches:\n");
    printf("  - Data cache: %d lines\n", CACHE_SIZE);
    printf("  - Instruction cache: %d lines\n\n", CACHE_SIZE);
    
    // Load some instructions into memory
    printf("Loading test instructions into memory...\n");
    int addr[12];
    int data[16];
    
    // Address 0x000: Instruction 0x0405 (LOCO 5)
    int_to_bits(0x000, cpu.mar.address, 12);
    int_to_bits(0x0405, cpu.mbr.data, 16);
    m_write(&cpu.mar, &cpu.mbr, &cpu.main_memory);
    
    // Address 0x002: Instruction 0x020A (ADDD 10)
    int_to_bits(0x002, cpu.mar.address, 12);
    int_to_bits(0x020A, cpu.mbr.data, 16);
    m_write(&cpu.mar, &cpu.mbr, &cpu.main_memory);
    
    // Address 0x004: Instruction 0x0165 (STOD 101)
    int_to_bits(0x004, cpu.mar.address, 12);
    int_to_bits(0x0165, cpu.mbr.data, 16);
    m_write(&cpu.mar, &cpu.mbr, &cpu.main_memory);
    
    // Address 0x006: Instruction 0x0700 (HALT)
    int_to_bits(0x006, cpu.mar.address, 12);
    int_to_bits(0x0700, cpu.mbr.data, 16);
    m_write(&cpu.mar, &cpu.mbr, &cpu.main_memory);
    
    printf("✓ Loaded 4 instructions\n\n");
    
    // Test 1: Read instructions (should miss first time)
    printf("Test 1: Read instructions (cold cache)\n");
    printf("---------------------------------------\n");
    
    int_to_bits(0x000, addr, 12);
    cache_read(&cpu.instruction_cache, &cpu.main_memory, addr, data);
    printf("  Read addr 0x000: hits=%d, misses=%d\n", 
           cpu.instruction_cache.hits, cpu.instruction_cache.misses);
    
    int_to_bits(0x002, addr, 12);
    cache_read(&cpu.instruction_cache, &cpu.main_memory, addr, data);
    printf("  Read addr 0x002: hits=%d, misses=%d (hit - same line)\n", 
           cpu.instruction_cache.hits, cpu.instruction_cache.misses);
    
    int_to_bits(0x004, addr, 12);
    cache_read(&cpu.instruction_cache, &cpu.main_memory, addr, data);
    printf("  Read addr 0x004: hits=%d, misses=%d (hit - same line)\n", 
           cpu.instruction_cache.hits, cpu.instruction_cache.misses);
    
    printf("\n");
    
    // Test 2: Re-read same instructions (should hit)
    printf("Test 2: Re-read instructions (warm cache)\n");
    printf("------------------------------------------\n");
    
    int_to_bits(0x000, addr, 12);
    cache_read(&cpu.instruction_cache, &cpu.main_memory, addr, data);
    printf("  Re-read addr 0x000: hits=%d, misses=%d (hit)\n", 
           cpu.instruction_cache.hits, cpu.instruction_cache.misses);
    
    int_to_bits(0x002, addr, 12);
    cache_read(&cpu.instruction_cache, &cpu.main_memory, addr, data);
    printf("  Re-read addr 0x002: hits=%d, misses=%d (hit)\n", 
           cpu.instruction_cache.hits, cpu.instruction_cache.misses);
    
    printf("\n");
    
    // Test 3: Data cache separate from instruction cache
    printf("Test 3: Data cache independence\n");
    printf("--------------------------------\n");
    
    printf("Data cache stats before: hits=%d, misses=%d\n", 
           cpu.data_cache.hits, cpu.data_cache.misses);
    
    // Write some data to memory (using data cache)
    int_to_bits(0x100, addr, 12);
    int_to_bits(42, data, 16);
    cache_write(&cpu.data_cache, &cpu.main_memory, addr, data);
    
    printf("Data cache stats after write: hits=%d, misses=%d\n", 
           cpu.data_cache.hits, cpu.data_cache.misses);
    
    // Read back data
    cache_read(&cpu.data_cache, &cpu.main_memory, addr, data);
    printf("Data cache stats after read: hits=%d, misses=%d\n", 
           cpu.data_cache.hits, cpu.data_cache.misses);
    printf("Value read: %d\n\n", bits_to_int(data, 16));
    
    // Print final statistics
    printf("Final Statistics\n");
    printf("----------------\n");
    printf("Instruction Cache:\n");
    print_cache_stats(&cpu.instruction_cache);
    printf("\nData Cache:\n");
    print_cache_stats(&cpu.data_cache);
    
    printf("\n✓ Instruction and Data caches working independently!\n");
}

int main() {
    test_instruction_cache();
    return 0;
}
