#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

int test_assemble_file(const char* filename) {
    // Read file
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("  ✗ Cannot open file\n");
        return -1;
    }
    
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* source = malloc(fsize + 1);
    fread(source, 1, fsize, fp);
    fclose(fp);
    source[fsize] = 0;
    
    // Assemble
    uint16_t output[MAX_INSTRUCTIONS];
    int size;
    
    printf("Assembling %s...\n", filename);
    int result = assemble_string(source, output, &size);
    free(source);
    
    if (result != 0) {
        printf("  ✗ Assembly failed\n");
        return -1;
    }
    
    printf("  ✓ Assembly successful: %d instructions\n", size);
    
    // Show first few instructions
    int show = size < 5 ? size : 5;
    for (int i = 0; i < show; i++) {
        printf("    [%04X] %04X\n", i * 2, output[i]);
    }
    if (size > 5) {
        printf("    ... (%d more)\n", size - 5);
    }
    
    return 0;
}

int main() {
    printf("========================================\n");
    printf("Testing Assembler with Example Files\n");
    printf("========================================\n\n");
    
    int errors = 0;
    
    // Test each example file
    const char* files[] = {
        "examples/sum.asm",
        "examples/maximum.asm",
        "examples/loop.asm"
    };
    
    for (int i = 0; i < 3; i++) {
        if (test_assemble_file(files[i]) != 0) {
            errors++;
        }
        printf("\n");
    }
    
    printf("========================================\n");
    if (errors == 0) {
        printf("All assembler tests passed!\n");
    } else {
        printf("Failed tests: %d\n", errors);
    }
    printf("========================================\n");
    
    return errors;
}
