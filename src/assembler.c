#include "../include/assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void init_assembler(assembler_t* as) {
    as->symbol_count = 0;
    as->current_address = 0;
    as->error_count = 0;
    memset(as->error_msg, 0, sizeof(as->error_msg));
}

// Funções auxiliares de string
void trim_whitespace(char* str) {
    char* start = str;
    while(isspace((unsigned char)*start)) start++;
    if(*start == 0) { *str = 0; return; }
    char* end = start + strlen(start) - 1;
    while(end > start && isspace((unsigned char)*end)) end--;
    *(end+1) = 0;
    memmove(str, start, strlen(start)+1);
}

void remove_comments(char* line) {
    char* p = strchr(line, '/'); // Comentários iniciam com /
    if(p) *p = 0;
    p = strchr(line, ';');       // Ou com ;
    if(p) *p = 0;
}

// Identifica o Opcode
int get_opcode_info(const char* mnemonic, uint16_t* opcode, int* needs_operand) {
    // Instruções padrão (Bits 15-12)
    if (strcmp(mnemonic, "LODD") == 0) { *opcode = OP_LODD; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "STOD") == 0) { *opcode = OP_STOD; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "ADDD") == 0) { *opcode = OP_ADDD; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "SUBD") == 0) { *opcode = OP_SUBD; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "JPOS") == 0) { *opcode = OP_JPOS; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "JZER") == 0) { *opcode = OP_JZER; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "JUMP") == 0) { *opcode = OP_JUMP; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "LOCO") == 0) { *opcode = OP_LOCO; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "LODL") == 0) { *opcode = OP_LODL; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "STOL") == 0) { *opcode = OP_STOL; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "ADDL") == 0) { *opcode = OP_ADDL; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "SUBL") == 0) { *opcode = OP_SUBL; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "JNEG") == 0) { *opcode = OP_JNEG; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "JNZE") == 0) { *opcode = OP_JNZE; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "CALL") == 0) { *opcode = OP_CALL; *needs_operand = 1; return 1; }

    // Instruções Especiais (0xF...)
    if (strcmp(mnemonic, "PSHI") == 0) { *opcode = (OP_SPEC << 12) | OPL_PSHI; *needs_operand = 0; return 1; }
    if (strcmp(mnemonic, "POPI") == 0) { *opcode = (OP_SPEC << 12) | OPL_POPI; *needs_operand = 0; return 1; }
    if (strcmp(mnemonic, "PUSH") == 0) { *opcode = (OP_SPEC << 12) | OPL_PUSH; *needs_operand = 0; return 1; }
    if (strcmp(mnemonic, "POP")  == 0) { *opcode = (OP_SPEC << 12) | OPL_POP;  *needs_operand = 0; return 1; }
    if (strcmp(mnemonic, "RETN") == 0) { *opcode = (OP_SPEC << 12) | OPL_RETN; *needs_operand = 0; return 1; }
    if (strcmp(mnemonic, "SWAP") == 0) { *opcode = (OP_SPEC << 12) | OPL_SWAP; *needs_operand = 0; return 1; }
    
    // Instruções Especiais com operando de 8 bits
    if (strcmp(mnemonic, "INSP") == 0) { *opcode = (OP_SPEC << 12) | OPL_INSP; *needs_operand = 1; return 1; }
    if (strcmp(mnemonic, "DESP") == 0) { *opcode = (OP_SPEC << 12) | OPL_DESP; *needs_operand = 1; return 1; }

    return 0; // Inválido
}

int add_symbol(assembler_t* as, char* label, uint16_t addr) {
    for(int i=0; i<as->symbol_count; i++) {
        if(strcmp(as->symbols[i].label, label) == 0) return -1; // Duplicado
    }
    strcpy(as->symbols[as->symbol_count].label, label);
    as->symbols[as->symbol_count].address = addr;
    as->symbol_count++;
    return 0;
}

int get_symbol_addr(assembler_t* as, char* label) {
    for(int i=0; i<as->symbol_count; i++) {
        if(strcmp(as->symbols[i].label, label) == 0) 
            return as->symbols[i].address;
    }
    return -1;
}

// Processa uma linha (Pass 1 ou Pass 2)
int process_line(assembler_t* as, char* line, int pass, FILE* out_file) {
    char label[64] = "";
    char mnemonic[16] = "";
    char operand_str[64] = "";
    
    remove_comments(line);
    trim_whitespace(line);
    if(strlen(line) == 0) return 0;

    char* token = strtok(line, " \t");
    if(!token) return 0;

    // Verifica se é Label (termina com :)
    if(token[strlen(token)-1] == ':') {
        token[strlen(token)-1] = 0; // Remove :
        strcpy(label, token);
        if(pass == 1) {
            // Endereço atual em Palavras (bytes / 2)
            if(add_symbol(as, label, as->current_address / 2) != 0) {
                printf("Erro: Label duplicado '%s'\n", label);
                return -1;
            }
        }
        token = strtok(NULL, " \t"); // Pega próximo token
    }

    if(!token) return 0; // Linha só com label

    strcpy(mnemonic, token);
    
    // Tratamento especial para .DATA ou variáveis
    // Tanenbaum não define diretivas, mas é útil para alocar variáveis
    if (strcmp(mnemonic, "DW") == 0) { // Define Word
         token = strtok(NULL, " \t");
         if (pass == 2 && out_file) {
             int val = atoi(token);
             uint8_t bytes[2];
             bytes[0] = (val >> 8) & 0xFF; // Big Endian
             bytes[1] = val & 0xFF;
             fwrite(bytes, 1, 2, out_file);
         }
         as->current_address += 2;
         return 0;
    }

    uint16_t base_opcode = 0;
    int needs_op = 0;
    
    if(!get_opcode_info(mnemonic, &base_opcode, &needs_op)) {
        printf("Erro: Opcode invalido '%s'\n", mnemonic);
        return -1;
    }

    int operand_val = 0;
    if(needs_op) {
        token = strtok(NULL, " \t");
        if(!token) {
            printf("Erro: Operando faltando para %s\n", mnemonic);
            return -1;
        }
        
        // Tenta converter número
        char* end;
        long val = strtol(token, &end, 0);
        if(*end == 0) {
            operand_val = (int)val;
        } else {
            // É um label
            if(pass == 2) {
                int sym_addr = get_symbol_addr(as, token);
                if(sym_addr == -1) {
                    printf("Erro: Label indefinido '%s'\n", token);
                    return -1;
                }
                operand_val = sym_addr;
            }
        }
    }

    if(pass == 2 && out_file) {
        uint16_t final_instr = 0;
        
        if ((base_opcode >> 12) == 0xF) {
            // Instruções especiais (INSP, DESP usam 8 bits inferiores)
            final_instr = base_opcode | (operand_val & 0xFF);
        } else {
            // Instruções padrão (Opcode 4 bits + Endereço 12 bits)
            final_instr = (base_opcode << 12) | (operand_val & 0xFFF);
        }

        // Escreve Big Endian (Tanenbaum Mac-1 é Big Endian)
        uint8_t bytes[2];
        bytes[0] = (final_instr >> 8) & 0xFF;
        bytes[1] = final_instr & 0xFF;
        fwrite(bytes, 1, 2, out_file);
    }

    as->current_address += 2; // Cada instrução tem 16 bits (2 bytes)
    return 0;
}

int assemble_file(const char* input_file, const char* output_file) {
    assembler_t as;
    init_assembler(&as);
    char line[256];
    
    // PASS 1: Simbolos
    FILE* in = fopen(input_file, "r");
    if(!in) return -1;
    
    while(fgets(line, sizeof(line), in)) {
        char temp[256]; strcpy(temp, line);
        if(process_line(&as, temp, 1, NULL) != 0) return -1;
    }
    fclose(in);

    // Reset para PASS 2
    as.current_address = 0;
    
    // PASS 2: Geração de Código
    in = fopen(input_file, "r");
    FILE* out = fopen(output_file, "wb");
    if(!out) return -1;

    while(fgets(line, sizeof(line), in)) {
        char temp[256]; strcpy(temp, line);
        if(process_line(&as, temp, 2, out) != 0) return -1;
    }

    fclose(in);
    fclose(out);
    printf("Montagem concluida. %d bytes gerados.\n", as.current_address);
    return 0;
}