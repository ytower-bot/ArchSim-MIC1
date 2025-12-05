#include "../include/assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void init_assembler(assembler_t* as) {
    as->symbol_count = 0;
    as->instruction_count = 0;
    as->current_address = 0;
    as->error_count = 0;
    memset(as->error_msg, 0, sizeof(as->error_msg));
}

void trim_whitespace(char* str) {
    if (!str || *str == '\0') return;

    char* start = str;
    char* end;
    size_t len;

    while (*start && isspace((unsigned char)*start)) start++;

    if (*start == '\0') {
        str[0] = '\0';
        return;
    }

    len = strlen(start);
    if (len == 0) {
        str[0] = '\0';
        return;
    }

    end = start + len - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';

    if (start != str) {
        size_t trim_len = (end - start) + 1;
        memmove(str, start, trim_len + 1);
    }
}

void remove_comments(char* line) {
    char* comment = strchr(line, ';');
    if (comment) {
        *comment = '\0';
    }
}

int is_valid_opcode(const char* mnemonic) {
    if (strcmp(mnemonic, "LODD") == 0) return 1;
    if (strcmp(mnemonic, "STOD") == 0) return 1;
    if (strcmp(mnemonic, "ADDD") == 0) return 1;
    if (strcmp(mnemonic, "SUBD") == 0) return 1;
    if (strcmp(mnemonic, "JPOS") == 0) return 1;
    if (strcmp(mnemonic, "JZER") == 0) return 1;
    if (strcmp(mnemonic, "JUMP") == 0) return 1;
    if (strcmp(mnemonic, "LOCO") == 0) return 1;
    if (strcmp(mnemonic, "LODL") == 0) return 1;
    if (strcmp(mnemonic, "STOL") == 0) return 1;
    if (strcmp(mnemonic, "ADDL") == 0) return 1;
    if (strcmp(mnemonic, "SUBL") == 0) return 1;
    if (strcmp(mnemonic, "JNEG") == 0) return 1;
    if (strcmp(mnemonic, "JNZE") == 0) return 1;
    if (strcmp(mnemonic, "CALL") == 0) return 1;

    if (strcmp(mnemonic, "PSHI") == 0) return 1;
    if (strcmp(mnemonic, "POPI") == 0) return 1;
    if (strcmp(mnemonic, "PUSH") == 0) return 1;
    if (strcmp(mnemonic, "POP") == 0) return 1;
    if (strcmp(mnemonic, "RETN") == 0) return 1;
    if (strcmp(mnemonic, "SWAP") == 0) return 1;
    if (strcmp(mnemonic, "INSP") == 0) return 1;
    if (strcmp(mnemonic, "DESP") == 0) return 1;

    return 0;
}

uint8_t parse_opcode(const char* mnemonic) {
    if (strcmp(mnemonic, "LODD") == 0) return OP_LODD;
    if (strcmp(mnemonic, "STOD") == 0) return OP_STOD;
    if (strcmp(mnemonic, "ADDD") == 0) return OP_ADDD;
    if (strcmp(mnemonic, "SUBD") == 0) return OP_SUBD;
    if (strcmp(mnemonic, "JPOS") == 0) return OP_JPOS;
    if (strcmp(mnemonic, "JZER") == 0) return OP_JZER;
    if (strcmp(mnemonic, "JUMP") == 0) return OP_JUMP;
    if (strcmp(mnemonic, "LOCO") == 0) return OP_LOCO;
    if (strcmp(mnemonic, "LODL") == 0) return OP_LODL;
    if (strcmp(mnemonic, "STOL") == 0) return OP_STOL;
    if (strcmp(mnemonic, "ADDL") == 0) return OP_ADDL;
    if (strcmp(mnemonic, "SUBL") == 0) return OP_SUBL;
    if (strcmp(mnemonic, "JNEG") == 0) return OP_JNEG;
    if (strcmp(mnemonic, "JNZE") == 0) return OP_JNZE;
    if (strcmp(mnemonic, "CALL") == 0) return OP_CALL;

    if (strcmp(mnemonic, "PSHI") == 0) return 0xF0;
    if (strcmp(mnemonic, "POPI") == 0) return 0xF2;
    if (strcmp(mnemonic, "PUSH") == 0) return 0xF4;
    if (strcmp(mnemonic, "POP") == 0) return 0xF6;
    if (strcmp(mnemonic, "RETN") == 0) return 0xF8;
    if (strcmp(mnemonic, "SWAP") == 0) return 0xFA;
    if (strcmp(mnemonic, "INSP") == 0) return 0xFC;
    if (strcmp(mnemonic, "DESP") == 0) return 0xFE;

    return 0xFF;
}

int parse_operand(const char* operand_str) {
    char* endptr;
    long value;

    if (strncmp(operand_str, "0x", 2) == 0 || strncmp(operand_str, "0X", 2) == 0) {
        value = strtol(operand_str, &endptr, 16);
    } else {
        value = strtol(operand_str, &endptr, 10);
    }

    if (*endptr != '\0' && !isspace((unsigned char)*endptr)) {
        return -1;
    }

    if (value < 0 || value > 255) {
        return -1;
    }

    return (int)value;
}

int add_symbol(assembler_t* as, const char* label, uint16_t address) {
    if (as->symbol_count >= MAX_LABELS) {
        snprintf(as->error_msg, sizeof(as->error_msg), "Symbol table full");
        return -1;
    }

    for (int i = 0; i < as->symbol_count; i++) {
        if (strcmp(as->symbols[i].label, label) == 0) {
            snprintf(as->error_msg, sizeof(as->error_msg), "Duplicate label: %s", label);
            return -1;
        }
    }

    strncpy(as->symbols[as->symbol_count].label, label, MAX_LABEL_LENGTH - 1);
    as->symbols[as->symbol_count].label[MAX_LABEL_LENGTH - 1] = '\0';
    as->symbols[as->symbol_count].address = address;
    as->symbol_count++;

    return 0;
}

int lookup_symbol(assembler_t* as, const char* label) {
    for (int i = 0; i < as->symbol_count; i++) {
        if (strcmp(as->symbols[i].label, label) == 0) {
            return as->symbols[i].address;
        }
    }
    return -1;
}

int parse_line(assembler_t* as, const char* line, int pass) {
    static __attribute__((aligned(16))) char line_copy[MAX_LINE_LENGTH];
    static __attribute__((aligned(16))) char label[MAX_LABEL_LENGTH];
    static __attribute__((aligned(16))) char mnemonic[16];
    static __attribute__((aligned(16))) char operand_str[MAX_LABEL_LENGTH];

    char* token;
    char* rest;

    memset(line_copy, 0, sizeof(line_copy));
    memset(label, 0, sizeof(label));
    memset(mnemonic, 0, sizeof(mnemonic));
    memset(operand_str, 0, sizeof(operand_str));

    if (!line || !as) return -1;

    size_t line_len = strlen(line);
    if (line_len >= MAX_LINE_LENGTH) {
        line_len = MAX_LINE_LENGTH - 1;
    }
    memcpy(line_copy, line, line_len);
    line_copy[line_len] = '\0';

    remove_comments(line_copy);

    trim_whitespace(line_copy);

    if (line_copy[0] == '\0') {
        return 0;
    }

    rest = line_copy;
    if (strchr(line_copy, ':')) {
        token = strtok(line_copy, ":");
        if (token) {
            strncpy(label, token, MAX_LABEL_LENGTH - 1);
            label[MAX_LABEL_LENGTH - 1] = '\0';
            trim_whitespace(label);

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

    trim_whitespace(rest);
    if (strlen(rest) == 0) {
        return 0;
    }

    token = strtok(rest, " \t");
    if (token) {
        strncpy(mnemonic, token, sizeof(mnemonic) - 1);
        mnemonic[sizeof(mnemonic) - 1] = '\0';

        for (int i = 0; mnemonic[i]; i++) {
            mnemonic[i] = toupper((unsigned char)mnemonic[i]);
        }

        token = strtok(NULL, " \t");
        if (token) {
            strncpy(operand_str, token, MAX_LABEL_LENGTH - 1);
            operand_str[MAX_LABEL_LENGTH - 1] = '\0';
            trim_whitespace(operand_str);
        }
    }

    if (!is_valid_opcode(mnemonic)) {
        snprintf(as->error_msg, sizeof(as->error_msg), "Invalid opcode: %s", mnemonic);
        as->error_count++;
        return -1;
    }

    uint8_t opcode = parse_opcode(mnemonic);

    if (pass == 1) {
        as->current_address += 2;
        return 0;
    }

    if (pass == 2) {
        instruction_t* inst = &as->instructions[as->instruction_count];
        inst->opcode = opcode;
        inst->has_label_ref = 0;

        if (opcode >= 0xF0 && opcode <= 0xFA && opcode != 0xFC && opcode != 0xFE) {
            inst->operand = 0;
        } else {
            if (strlen(operand_str) == 0) {
                snprintf(as->error_msg, sizeof(as->error_msg), "Missing operand for %s", mnemonic);
                as->error_count++;
                return -1;
            }

            int value = parse_operand(operand_str);
            if (value >= 0) {
                inst->operand = (uint16_t)value;
            } else {
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

int pass1(assembler_t* as, const char* source) {
    char line[MAX_LINE_LENGTH];
    const char* ptr = source;
    int line_num = 0;

    while (*ptr) {
        int i = 0;
        while (*ptr && *ptr != '\n' && i < MAX_LINE_LENGTH - 1) {
            line[i++] = *ptr++;
        }
        line[i] = '\0';

        if (*ptr == '\n') ptr++;
        line_num++;

        if (parse_line(as, line, 1) != 0) {
            fprintf(stderr, "Error on line %d: %s\n", line_num, as->error_msg);
            return -1;
        }
    }

    return 0;
}

int pass2(assembler_t* as, uint16_t* output) {
    (void)output;

    as->current_address = 0;
    as->instruction_count = 0;

    return 0;
}

int assemble_string(const char* source, uint16_t* output, int* output_size) {
    assembler_t as;
    init_assembler(&as);

    if (pass1(&as, source) != 0) {
        return -1;
    }

    as.current_address = 0;
    as.instruction_count = 0;

    char line[MAX_LINE_LENGTH];
    const char* ptr = source;
    int line_num = 0;

    while (*ptr) {
        int i = 0;
        while (*ptr && *ptr != '\n' && i < MAX_LINE_LENGTH - 1) {
            line[i++] = *ptr++;
        }
        line[i] = '\0';

        if (*ptr == '\n') ptr++;
        line_num++;

        if (parse_line(&as, line, 2) != 0) {
            fprintf(stderr, "Error on line %d: %s\n", line_num, as.error_msg);
            return -1;
        }
    }

    for (int i = 0; i < as.instruction_count; i++) {
        instruction_t* inst = &as.instructions[i];

        if (inst->has_label_ref) {
            int addr = lookup_symbol(&as, inst->label_ref);
            if (addr < 0) {
                fprintf(stderr, "Undefined label: %s\n", inst->label_ref);
                return -1;
            }
            inst->operand = (uint16_t)(addr / 2);
        }

        output[i] = ((uint16_t)inst->opcode << 8) | (inst->operand & 0xFF);
    }

    *output_size = as.instruction_count;
    return 0;
}

int assemble_file(const char* input_file, const char* output_file) {
    FILE* fp = fopen(input_file, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open input file: %s\n", input_file);
        return -1;
    }

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

    uint16_t output[MAX_INSTRUCTIONS];
    int output_size;

    if (assemble_string(source, output, &output_size) != 0) {
        free(source);
        return -1;
    }

    free(source);

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
