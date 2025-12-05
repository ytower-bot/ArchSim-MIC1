#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>

#define OP_LODD  0x0
#define OP_STOD  0x1
#define OP_ADDD  0x2
#define OP_SUBD  0x3
#define OP_JPOS  0x4
#define OP_JZER  0x5
#define OP_JUMP  0x6
#define OP_LOCO  0x7
#define OP_LODL  0x8
#define OP_STOL  0x9
#define OP_ADDL  0xA
#define OP_SUBL  0xB
#define OP_JNEG  0xC
#define OP_JNZE  0xD
#define OP_CALL  0xE
#define OP_SPECIAL 0xF
#define OP_PSHI  0xF00
#define OP_POPI  0xF20
#define OP_PUSH  0xF40
#define OP_POP   0xF60
#define OP_RETN  0xF80
#define OP_SWAP  0xFA0
#define OP_INSP  0xFC0
#define OP_DESP  0xFE0

#define MAX_LABELS 256
#define MAX_INSTRUCTIONS 4096
#define MAX_LINE_LENGTH 256
#define MAX_LABEL_LENGTH 64

typedef struct {
    char label[MAX_LABEL_LENGTH];
    uint16_t address;
} __attribute__((aligned(8))) symbol_t;

typedef struct {
    uint8_t opcode;
    uint16_t operand;
    int has_label_ref;
    char label_ref[MAX_LABEL_LENGTH];
} __attribute__((aligned(8))) instruction_t;

typedef struct {
    symbol_t symbols[MAX_LABELS];
    int symbol_count;
    instruction_t instructions[MAX_INSTRUCTIONS];
    int instruction_count;
    int current_address;
    int error_count;
    char error_msg[256];
} __attribute__((aligned(16))) assembler_t;

int assemble_file(const char* input_file, const char* output_file);
int assemble_string(const char* source, uint16_t* output, int* output_size);
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
