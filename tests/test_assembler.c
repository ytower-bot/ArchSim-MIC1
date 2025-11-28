#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

void print_hex_dump(uint16_t* data, int size) {
    printf("Binary output (%d instructions):\n", size);
    for (int i = 0; i < size; i++) {
        printf("  [%04X] %04X\n", i * 2, data[i]);
    }
}

int main() {
    printf("Testing MIC-1 Assembler\n");
    printf("=======================\n\n");
    
    // Test 1: Simple program without labels
    printf("Test 1: Simple program (no labels)\n");
    const char* prog1 = 
        "; Simple addition program\n"
        "LOCO 10      ; AC <- 10\n"
        "ADDD 100     ; AC <- AC + Memory[100]\n"
        "STOD 101     ; Memory[101] <- AC\n"
        "HALT\n";
    
    uint16_t output1[MAX_INSTRUCTIONS];
    int size1;
    
    if (assemble_string(prog1, output1, &size1) == 0) {
        printf("✓ Assembly successful\n");
        print_hex_dump(output1, size1);
    } else {
        printf("✗ Assembly failed\n");
        return 1;
    }
    
    printf("\n");
    
    // Test 2: Program with labels
    printf("Test 2: Program with labels\n");
    const char* prog2 = 
        "; Program with jump\n"
        "START:  LOCO 5       ; AC <- 5\n"
        "        ADDD 10      ; AC <- AC + 10\n"
        "        JZER END     ; if AC == 0, jump to END\n"
        "        SUBD 1       ; AC <- AC - 1\n"
        "        JUMP START   ; Jump to START\n"
        "END:    HALT\n";
    
    uint16_t output2[MAX_INSTRUCTIONS];
    int size2;
    
    if (assemble_string(prog2, output2, &size2) == 0) {
        printf("✓ Assembly successful\n");
        print_hex_dump(output2, size2);
    } else {
        printf("✗ Assembly failed\n");
        return 1;
    }
    
    printf("\n");
    
    // Test 3: Test file assembly
    printf("Test 3: Assemble from file\n");
    
    // Create test file
    FILE* fp = fopen("/tmp/test.asm", "w");
    fprintf(fp, "; Test program\n");
    fprintf(fp, "LOOP: LODD 50\n");
    fprintf(fp, "      ADDD 51\n");
    fprintf(fp, "      STOD 52\n");
    fprintf(fp, "      HALT\n");
    fclose(fp);
    
    if (assemble_file("/tmp/test.asm", "/tmp/test.bin") == 0) {
        printf("✓ File assembly successful\n");
        
        // Read and display binary
        fp = fopen("/tmp/test.bin", "rb");
        uint16_t binary[MAX_INSTRUCTIONS];
        int count = fread(binary, sizeof(uint16_t), MAX_INSTRUCTIONS, fp);
        fclose(fp);
        
        print_hex_dump(binary, count);
    } else {
        printf("✗ File assembly failed\n");
        return 1;
    }
    
    printf("\n✓ All tests passed!\n");
    return 0;
}
