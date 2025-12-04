#include <stdio.h>
#include "../include/assembler.h"

int main(int argc, char* argv[]) {
    if(argc < 3) {
        printf("Uso: %s <arquivo_fonte.asm> <arquivo_saida.bin>\n", argv[0]);
        return 1;
    }

    printf("Montando %s -> %s ...\n", argv[1], argv[2]);
    
    if(assemble_file(argv[1], argv[2]) == 0) {
        printf("Sucesso!\n");
        return 0;
    } else {
        printf("Falha na montagem.\n");
        return 1;
    }
}