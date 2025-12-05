
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../include/assembler.h"

void print_usage(const char* prog_name) {
    printf("MIC-1 Assembler v1.0\n");
    printf("Usage: %s <input.asm> [output.bin]\n", prog_name);
    printf("\n");
    printf("If output file is not specified, uses input name with .bin extension\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s program.asm program.bin\n", prog_name);
    printf("  %s program.asm              (outputs to program.bin)\n", prog_name);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    char output_file[512];

    if (argc >= 3) {
        strncpy(output_file, argv[2], sizeof(output_file) - 1);
    } else {

        strncpy(output_file, input_file, sizeof(output_file) - 1);
        char* ext = strrchr(output_file, '.');
        if (ext && strcmp(ext, ".asm") == 0) {
            strcpy(ext, ".bin");
        } else {
            strcat(output_file, ".bin");
        }
    }
    output_file[sizeof(output_file) - 1] = '\0';

    printf("═══════════════════════════════════════\n");
    printf("  MIC-1 Assembler\n");
    printf("═══════════════════════════════════════\n");
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n", output_file);
    printf("───────────────────────────────────────\n");

    int result = assemble_file(input_file, output_file);

    if (result == 0) {
        printf("───────────────────────────────────────\n");
        printf("✓ Assembly successful!\n");
        printf("═══════════════════════════════════════\n");
        return 0;
    } else {
        printf("───────────────────────────────────────\n");
        printf("✗ Assembly failed!\n");
        printf("═══════════════════════════════════════\n");
        return 1;
    }
}
