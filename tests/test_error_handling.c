#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

int test_invalid_code(const char* desc, const char* code) {
    printf("Testing: %s\n", desc);
    
    uint16_t output[MAX_INSTRUCTIONS];
    int size;
    
    int result = assemble_string(code, output, &size);
    
    if (result != 0) {
        printf("  ✓ Correctly rejected invalid code\n");
        return 0;
    } else {
        printf("  ✗ Should have rejected this code\n");
        return -1;
    }
}

int main() {
    printf("========================================\n");
    printf("Testing Error Handling\n");
    printf("========================================\n\n");
    
    int errors = 0;
    
    // Test 1: Invalid opcode
    errors += test_invalid_code(
        "Invalid opcode",
        "INVALID 100\n"
    );
    printf("\n");
    
    // Test 2: Empty program
    printf("Testing: Empty program\n");
    uint16_t output[MAX_INSTRUCTIONS];
    int size;
    int result = assemble_string("", output, &size);
    if (result == 0 && size == 0) {
        printf("  ✓ Empty program handled correctly\n");
    } else {
        printf("  ✗ Empty program not handled\n");
        errors++;
    }
    printf("\n");
    
    // Test 3: Valid program (sanity check)
    printf("Testing: Valid simple program\n");
    result = assemble_string("HALT\n", output, &size);
    if (result == 0 && size == 1) {
        printf("  ✓ Valid program assembled correctly\n");
    } else {
        printf("  ✗ Valid program failed\n");
        errors++;
    }
    printf("\n");
    
    printf("========================================\n");
    if (errors == 0) {
        printf("All error handling tests passed!\n");
    } else {
        printf("Failed tests: %d\n", errors);
    }
    printf("========================================\n");
    
    return errors;
}
