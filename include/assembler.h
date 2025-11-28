#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>

// Opcode definitions for MIC-1 basic instruction set
#define OP_LODD  0x00  // Load Direct:  AC <- Memory[addr]
#define OP_STOD  0x01  // Store Direct: Memory[addr] <- AC
#define OP_ADDD  0x02  // Add Direct:   AC <- AC + Memory[addr]
#define OP_SUBD  0x03  // Sub Direct:   AC <- AC - Memory[addr]
#define OP_LOCO  0x04  // Load Const:   AC <- constant
#define OP_JUMP  0x05  // Jump:         PC <- addr
#define OP_JZER  0x06  // Jump if Zero: if (AC == 0) PC <- addr
#define OP_HALT  0x07  // Halt:         Stop execution

// Maximum sizes
#define MAX_LABELS 256
#define MAX_INSTRUCTIONS 4096
#define MAX_LINE_LENGTH 256
#define MAX_LABEL_LENGTH 64

// Symbol table entry
typedef struct {
    char label[MAX_LABEL_LENGTH];
    uint16_t address;
} symbol_t;

// Instruction structure
typedef struct {
    uint8_t opcode;
    uint16_t operand;
    int has_label_ref;
    char label_ref[MAX_LABEL_LENGTH];
} instruction_t;

// Assembler state
typedef struct {
    symbol_t symbols[MAX_LABELS];
    int symbol_count;
    instruction_t instructions[MAX_INSTRUCTIONS];
    int instruction_count;
    int current_address;
    int error_count;
    char error_msg[256];
} assembler_t;

// Public API
int assemble_file(const char* input_file, const char* output_file);
int assemble_string(const char* source, uint16_t* output, int* output_size);

// Internal functions
void init_assembler(assembler_t* as);
int pass1(assembler_t* as, const char* source);
int pass2(assembler_t* as, uint16_t* output);
int parse_line(assembler_t* as, const char* line, int pass);
int add_symbol(assembler_t* as, const char* label, uint16_t address);
int lookup_symbol(assembler_t* as, const char* label);
uint8_t parse_opcode(const char* mnemonic);
int is_valid_opcode(const char* mnemonic);
void trim_whitespace(char* str);
void remove_comments(char* line);
int parse_operand(const char* operand_str);

#endif
