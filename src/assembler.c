#include "../include/assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Initialize assembler state
void init_assembler(assembler_t* as) {
    as->symbol_count = 0;
    as->instruction_count = 0;
    as->current_address = 0;
    as->error_count = 0;
    memset(as->error_msg, 0, sizeof(as->error_msg));
}

// Trim leading and trailing whitespace
void trim_whitespace(char* str) {
    char* start = str;
    char* end;
    
    // Trim leading
    while (isspace((unsigned char)*start)) start++;
    
    // Trim trailing
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    // Null terminate
    *(end + 1) = '\0';
    
    // Move trimmed string to start
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

// Remove comments from line
void remove_comments(char* line) {
    char* comment = strchr(line, ';');
    if (comment) {
        *comment = '\0';
    }
}

// Check if string is valid opcode
int is_valid_opcode(const char* mnemonic) {
    if (strcmp(mnemonic, "LODD") == 0) return 1;
    if (strcmp(mnemonic, "STOD") == 0) return 1;
    if (strcmp(mnemonic, "ADDD") == 0) return 1;
    if (strcmp(mnemonic, "SUBD") == 0) return 1;
    if (strcmp(mnemonic, "LOCO") == 0) return 1;
    if (strcmp(mnemonic, "JUMP") == 0) return 1;
    if (strcmp(mnemonic, "JZER") == 0) return 1;
    if (strcmp(mnemonic, "HALT") == 0) return 1;
    return 0;
}

// Parse opcode string to byte value
uint8_t parse_opcode(const char* mnemonic) {
    if (strcmp(mnemonic, "LODD") == 0) return OP_LODD;
    if (strcmp(mnemonic, "STOD") == 0) return OP_STOD;
    if (strcmp(mnemonic, "ADDD") == 0) return OP_ADDD;
    if (strcmp(mnemonic, "SUBD") == 0) return OP_SUBD;
    if (strcmp(mnemonic, "LOCO") == 0) return OP_LOCO;
    if (strcmp(mnemonic, "JUMP") == 0) return OP_JUMP;
    if (strcmp(mnemonic, "JZER") == 0) return OP_JZER;
    if (strcmp(mnemonic, "HALT") == 0) return OP_HALT;
    return 0xFF; // Invalid
}

// Parse operand (decimal or hex)
int parse_operand(const char* operand_str) {
    char* endptr;
    long value;
    
    // Check for hex format (0x prefix)
    if (strncmp(operand_str, "0x", 2) == 0 || strncmp(operand_str, "0X", 2) == 0) {
        value = strtol(operand_str, &endptr, 16);
    } else {
        value = strtol(operand_str, &endptr, 10);
    }
    
    // Validate conversion
    if (*endptr != '\0' && !isspace((unsigned char)*endptr)) {
        return -1; // Invalid number
    }
    
    // Validate range (8-bit operand)
    if (value < 0 || value > 255) {
        return -1; // Out of range
    }
    
    return (int)value;
}

// Add symbol to table
int add_symbol(assembler_t* as, const char* label, uint16_t address) {
    if (as->symbol_count >= MAX_LABELS) {
        snprintf(as->error_msg, sizeof(as->error_msg), "Symbol table full");
        return -1;
    }
    
    // Check for duplicate
    for (int i = 0; i < as->symbol_count; i++) {
        if (strcmp(as->symbols[i].label, label) == 0) {
            snprintf(as->error_msg, sizeof(as->error_msg), "Duplicate label: %s", label);
            return -1;
        }
    }
    
    // Add new symbol
    strncpy(as->symbols[as->symbol_count].label, label, MAX_LABEL_LENGTH - 1);
    as->symbols[as->symbol_count].label[MAX_LABEL_LENGTH - 1] = '\0';
    as->symbols[as->symbol_count].address = address;
    as->symbol_count++;
    
    return 0;
}

// Lookup symbol in table
int lookup_symbol(assembler_t* as, const char* label) {
    for (int i = 0; i < as->symbol_count; i++) {
        if (strcmp(as->symbols[i].label, label) == 0) {
            return as->symbols[i].address;
        }
    }
    return -1; // Not found
}

// Parse single line of assembly
int parse_line(assembler_t* as, const char* line, int pass) {
    char line_copy[MAX_LINE_LENGTH];
    char label[MAX_LABEL_LENGTH] = "";
    char mnemonic[16] = "";
    char operand_str[MAX_LABEL_LENGTH] = "";
    char* token;
    char* rest;
    
    // Copy line for manipulation
    strncpy(line_copy, line, MAX_LINE_LENGTH - 1);
    line_copy[MAX_LINE_LENGTH - 1] = '\0';
    
    // Remove comments
    remove_comments(line_copy);
    
    // Trim whitespace
    trim_whitespace(line_copy);
    
    // Skip empty lines
    if (strlen(line_copy) == 0) {
        return 0;
    }
    
    // Check for label (ends with colon)
    rest = line_copy;
    if (strchr(line_copy, ':')) {
        token = strtok(line_copy, ":");
        if (token) {
            strncpy(label, token, MAX_LABEL_LENGTH - 1);
            label[MAX_LABEL_LENGTH - 1] = '\0';
            trim_whitespace(label);
            
            // Add label in pass 1
            if (pass == 1 && strlen(label) > 0) {
                if (add_symbol(as, label, as->current_address) != 0) {
                    as->error_count++;
                    return -1;
                }
            }
            
            rest = strtok(NULL, "");
            if (!rest) rest = "";
        }
    }
    
    // Parse mnemonic and operand
    trim_whitespace(rest);
    if (strlen(rest) == 0) {
        return 0; // Label-only line
    }
    
    // Extract mnemonic
    token = strtok(rest, " \t");
    if (token) {
        strncpy(mnemonic, token, sizeof(mnemonic) - 1);
        mnemonic[sizeof(mnemonic) - 1] = '\0';
        
        // Convert to uppercase
        for (int i = 0; mnemonic[i]; i++) {
            mnemonic[i] = toupper((unsigned char)mnemonic[i]);
        }
        
        // Extract operand
        token = strtok(NULL, " \t");
        if (token) {
            strncpy(operand_str, token, MAX_LABEL_LENGTH - 1);
            operand_str[MAX_LABEL_LENGTH - 1] = '\0';
            trim_whitespace(operand_str);
        }
    }
    
    // Validate mnemonic
    if (!is_valid_opcode(mnemonic)) {
        snprintf(as->error_msg, sizeof(as->error_msg), "Invalid opcode: %s", mnemonic);
        as->error_count++;
        return -1;
    }
    
    uint8_t opcode = parse_opcode(mnemonic);
    
    // In pass 1, just count instruction
    if (pass == 1) {
        as->current_address += 2; // Each instruction is 2 bytes
        return 0;
    }
    
    // In pass 2, generate instruction
    if (pass == 2) {
        instruction_t* inst = &as->instructions[as->instruction_count];
        inst->opcode = opcode;
        inst->has_label_ref = 0;
        
        // Check if HALT (no operand)
        if (opcode == OP_HALT) {
            inst->operand = 0;
        } else {
            // Parse operand
            if (strlen(operand_str) == 0) {
                snprintf(as->error_msg, sizeof(as->error_msg), "Missing operand for %s", mnemonic);
                as->error_count++;
                return -1;
            }
            
            // Try to parse as number
            int value = parse_operand(operand_str);
            if (value >= 0) {
                inst->operand = (uint16_t)value;
            } else {
                // Assume it's a label reference
                inst->has_label_ref = 1;
                strncpy(inst->label_ref, operand_str, MAX_LABEL_LENGTH - 1);
                inst->label_ref[MAX_LABEL_LENGTH - 1] = '\0';
            }
        }
        
        as->instruction_count++;
        as->current_address += 2;
    }
    
    return 0;
}

// Pass 1: Collect labels
int pass1(assembler_t* as, const char* source) {
    char line[MAX_LINE_LENGTH];
    const char* ptr = source;
    int line_num = 0;
    
    while (*ptr) {
        // Extract line
        int i = 0;
        while (*ptr && *ptr != '\n' && i < MAX_LINE_LENGTH - 1) {
            line[i++] = *ptr++;
        }
        line[i] = '\0';
        
        if (*ptr == '\n') ptr++;
        line_num++;
        
        // Parse line
        if (parse_line(as, line, 1) != 0) {
            fprintf(stderr, "Error on line %d: %s\n", line_num, as->error_msg);
            return -1;
        }
    }
    
    return 0;
}

// Pass 2: Generate code
int pass2(assembler_t* as, uint16_t* output) {
    as->current_address = 0;
    as->instruction_count = 0;
    
    char line[MAX_LINE_LENGTH];
    const char* ptr = (const char*)as; // Placeholder - need to save source
    
    // For now, this is incomplete - need to re-read source
    // Will complete in next iteration
    
    return 0;
}

// Assemble from string
int assemble_string(const char* source, uint16_t* output, int* output_size) {
    assembler_t as;
    init_assembler(&as);
    
    // Pass 1: Collect symbols
    if (pass1(&as, source) != 0) {
        return -1;
    }
    
    // Reset for pass 2
    as.current_address = 0;
    as.instruction_count = 0;
    
    // Pass 2: Generate code (parse again)
    char line[MAX_LINE_LENGTH];
    const char* ptr = source;
    int line_num = 0;
    
    while (*ptr) {
        // Extract line
        int i = 0;
        while (*ptr && *ptr != '\n' && i < MAX_LINE_LENGTH - 1) {
            line[i++] = *ptr++;
        }
        line[i] = '\0';
        
        if (*ptr == '\n') ptr++;
        line_num++;
        
        // Parse line
        if (parse_line(&as, line, 2) != 0) {
            fprintf(stderr, "Error on line %d: %s\n", line_num, as.error_msg);
            return -1;
        }
    }
    
    // Resolve label references and generate output
    for (int i = 0; i < as.instruction_count; i++) {
        instruction_t* inst = &as.instructions[i];
        
        if (inst->has_label_ref) {
            int addr = lookup_symbol(&as, inst->label_ref);
            if (addr < 0) {
                fprintf(stderr, "Undefined label: %s\n", inst->label_ref);
                return -1;
            }
            inst->operand = (uint16_t)(addr / 2); // Convert byte address to word address
        }
        
        // Generate 16-bit instruction: [opcode:8][operand:8]
        output[i] = ((uint16_t)inst->opcode << 8) | (inst->operand & 0xFF);
    }
    
    *output_size = as.instruction_count;
    return 0;
}

// Assemble from file
int assemble_file(const char* input_file, const char* output_file) {
    FILE* fp = fopen(input_file, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open input file: %s\n", input_file);
        return -1;
    }
    
    // Read entire file into memory
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* source = (char*)malloc(file_size + 1);
    if (!source) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(fp);
        return -1;
    }
    
    fread(source, 1, file_size, fp);
    source[file_size] = '\0';
    fclose(fp);
    
    // Assemble
    uint16_t output[MAX_INSTRUCTIONS];
    int output_size;
    
    if (assemble_string(source, output, &output_size) != 0) {
        free(source);
        return -1;
    }
    
    free(source);
    
    // Write binary output
    fp = fopen(output_file, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot create output file: %s\n", output_file);
        return -1;
    }
    
    fwrite(output, sizeof(uint16_t), output_size, fp);
    fclose(fp);
    
    printf("Assembly successful: %d instructions, %d bytes\n", 
           output_size, output_size * 2);
    
    return 0;
}
