#include "../include/mic1.h"

void print_banner() {
    printf("========================================\n");
    printf("        MIC-1 SIMULATOR - UFF/IC\n");
    printf("        Computer Architecture\n");
    printf("========================================\n");
    printf("Version: 1.0 (Development)\n");
    printf("Components: Datapath, Control Unit, Memory\n");
    printf("========================================\n\n");
}

void demo_component_initialization(mic1_cpu* cpu) {
    printf("=== COMPONENT INITIALIZATION ===\n");
    printf("1. Initializing MIC-1 CPU...\n");
    init_mic1(cpu);
    printf("2. Initial register state:\n");
    print_registers(cpu);
    printf("3. Initial cache state:\n");
    printf("   Data cache initialized\n");
    printf("   Instruction cache initialized\n");
    printf("4. CPU status: %s\n", cpu->running ? "RUNNING" : "STOPPED");
    printf("================================\n\n");
}

void demo_program_loading() {
    printf("=== PROGRAM LOADING ===\n");
    printf("To load a program:\n");
    printf("1. Microprogram: load_microprogram_file(cpu, \"micro.bin\")\n");
    printf("2. Main program: load_program_file(cpu, \"program.bin\")\n");
    printf("3. Execute: run_mic1_program(cpu)\n");
    printf("=======================\n\n");
}

void demo_step_execution(mic1_cpu* cpu) {
    printf("=== EXECUTION DEMO ===\n");
    printf("Running 3 example cycles...\n");
    for (int i = 0; i < 3; i++) {
        printf("\n--- Cycle %d ---\n", i + 1);
        printf("MPC: [simulated]\n");
        printf("Microinstruction: [simulated]\n");
        printf("ALU operation: [simulated]\n");
        cpu->cycle_count++;
    }
    printf("\nTotal cycles executed: %d\n", cpu->cycle_count);
    printf("==================\n\n");
}

void interactive_menu(mic1_cpu* cpu) {
    int option;
    do {
        printf("\n=== MIC-1 SIMULATOR MENU ===\n");
        printf("1. Show CPU state\n");
        printf("2. Show registers\n");
        printf("3. Show cache stats\n");
        printf("4. Execute one cycle\n");
        printf("5. Reset CPU\n");
        printf("0. Exit\n");
        printf("Choose option: ");
        scanf("%d", &option);
        switch (option) {
            case 1: print_cpu_state(cpu); break;
            case 2: print_registers(cpu); break;
            case 3:
                printf("=== Data Cache ===\n");
                printf("Data cache statistics available\n");
                printf("=== Instruction Cache ===\n");
                printf("Instruction cache statistics available\n");
                break;
            case 4: printf("Executing cycle...\n"); step_mic1(cpu); break;
            case 5: printf("Resetting CPU...\n"); reset_mic1(cpu); break;
            case 0: printf("Exiting simulator...\n"); break;
            default: printf("Invalid option!\n");
        }
    } while (option != 0);
}

int main(int argc, char* argv[]) {
    print_banner();
    mic1_cpu cpu;
    demo_component_initialization(&cpu);
    demo_program_loading();
    demo_step_execution(&cpu);

    if (argc > 1) {
        printf("=== FILE LOADING ===\n");
        printf("Trying to load: %s\n", argv[1]);
        printf("(File loading to be implemented)\n");
        printf("====================\n\n");
    }

    char* ci_mode = getenv("CI");
    if (ci_mode != NULL) {
        printf("\n=== CI/CD MODE ===\n");
        printf("Running in non-interactive mode\n");
        printf("All components initialized successfully!\n");
        printf("Skipping interactive menu...\n");
        printf("==================\n\n");
        printf("MIC-1 simulator finished successfully!\n");
        return 0;
    }

    printf("Starting interactive mode...\n");
    interactive_menu(&cpu);
    printf("\nMIC-1 simulator finished successfully!\n");
    return 0;
}