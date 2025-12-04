#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mic1.h"

void print_usage(const char* prog_name) {
    printf("Uso: %s [arquivo_microcodigo] [arquivo_programa_opcional]\n", prog_name);
    printf("Exemplo: %s data/microcode.txt programa.bin\n", prog_name);
}

int main(int argc, char* argv[]) {
    mic1_cpu cpu;
    const char* micro_file = "data/microcode.txt";
    const char* prog_file = NULL;

    printf("==========================================\n");
    printf("      SIMULADOR MIC-1 (TANENBAUM)         \n");
    printf("==========================================\n");

    // 1. Tratamento de Argumentos
    if (argc > 1) {
        micro_file = argv[1];
    }
    if (argc > 2) {
        prog_file = argv[2];
    }

    // 2. Inicialização do Hardware
    printf("[INIT] Inicializando componentes da CPU...\n");
    init_mic1(&cpu);

    // 3. Carga do Microcódigo (Firmware)
    printf("[LOAD] Carregando microcodigo de '%s'...\n", micro_file);
    int micro_ops = load_microprogram_file(&cpu, micro_file);
    
    if (micro_ops <= 0) {
        printf("[ERRO] Falha ao carregar microcodigo ou arquivo vazio.\n");
        printf("       Certifique-se de que o arquivo existe e contem linhas de 32 bits (0s e 1s).\n");
        printf("       Voce pode criar um arquivo padrao em 'data/microcode.txt'.\n");
        return 1;
    }
    printf("[OK]   %d microinstrucoes carregadas na memoria de controle.\n", micro_ops);

    // 4. Carga do Programa (Memória Principal)
    if (prog_file != NULL) {
        printf("[LOAD] Carregando programa na memoria principal de '%s'...\n", prog_file);
        // Nota: load_program_file atualmente espera um arquivo binário ou texto conforme implementado em memory.c
        if (load_program_file(&cpu, prog_file) != 0) {
             printf("[AVISO] Problema ao ler arquivo de programa. Continuando com memoria zerada.\n");
        } else {
             printf("[OK]   Programa carregado.\n");
        }
    } else {
        printf("[INFO] Nenhum programa de usuario fornecido. Executando apenas microcodigo.\n");
    }

    // 5. Estado Inicial
    printf("\n--- Estado Inicial do Datapath ---\n");
    print_registers(&cpu);

    // 6. Execução
    printf("\n");
    run_mic1_program(&cpu);

    // 7. Estado Final
    printf("\n--- Estado Final do Datapath ---\n");
    print_registers(&cpu);

    return 0;
}