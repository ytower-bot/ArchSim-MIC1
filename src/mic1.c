#include <stdio.h>
#include <stdlib.h>
#include "../include/mic1.h"
#include "../include/utils/conversions.h"
#include <conio.h> // Necessário para _kbhit e _getch
#include <windows.h> // Para Sleep

void init_ui();
void close_ui();
void draw_dashboard(mic1_cpu *cpu);

// Protótipo local para garantir compilação caso o header não tenha sido corrigido ainda
void run_shifter(shifter* s);

void init_mic1(mic1_cpu* cpu) {
    if (!cpu) return;
    
    cpu->running = 0;
    cpu->cycle_count = 0;
    cpu->clock = 0;
    
    // 1. Inicializa Datapath (Registradores e Constantes 0, +1, -1, etc.)
    init_register_bank(&cpu->reg_bank);
    
    // Zera latches
    for(int i=0; i<16; i++) {
        cpu->latch_a.data[i] = 0;
        cpu->latch_b.data[i] = 0;
    }

    // 2. Inicializa Unidades Funcionais
    init_alu(&cpu->alu);
    init_shifter(&cpu->shifter);
    
    // 3. Inicializa Memória e Registradores de Interface
    init_mar(&cpu->mar);
    init_mbr(&cpu->mbr);
    init_memory(&cpu->main_memory);
    
    // 4. Inicializa Unidade de Controle
    init_mir(&cpu->mir);
    init_mpc(&cpu->mpc);
    init_mmux(&cpu->mmux);
    init_amux(&cpu->amux);
    init_control_memory(&cpu->ctrl_mem);
    
    // 5. Conecta decodificadores ao banco de registradores
    cpu->decoder_a.rb = &cpu->reg_bank;
    cpu->decoder_b.rb = &cpu->reg_bank;
    cpu->decoder_c.rb = &cpu->reg_bank;
}

void reset_mic1(mic1_cpu* cpu) {
    init_mic1(cpu);
}

/**
 * Executa um ciclo de clock completo do MIC-1.
 * Estruturado conforme Tanenbaum, Cap. 4, Figura 4.2 (Temporização).
 */
void step_mic1(mic1_cpu* cpu) {
    if (!cpu) return;

    // ====================================================================
    // SUBCICLO 1: Busca da Microinstrução (Fetch)
    // O MIR é carregado com a palavra da Memória de Controle apontada pelo MPC.
    // ====================================================================
    fetch_microinstruction(&cpu->ctrl_mem, &cpu->mpc, &cpu->mir);

    // ====================================================================
    // SUBCICLO 2: Decodificação e Carga dos Latches A e B
    // Os sinais de controle estabilizam. Os valores dos registradores selecionados
    // (pelos campos A e B do MIR) fluem para os barramentos A e B e são 
    // capturados pelos Latches A e B. MPC é incrementado aqui para agilizar.
    // ====================================================================
    
    // Configura decodificadores
    for(int i=0; i<4; i++) {
        cpu->decoder_a.control[i] = cpu->mir.a[i];
        cpu->decoder_b.control[i] = cpu->mir.b[i];
    }
    
    // Leitura: Registradores -> Latches
    run_decoder(&cpu->decoder_a, &cpu->latch_a);
    run_decoder(&cpu->decoder_b, &cpu->latch_b);

    // ====================================================================
    // SUBCICLO 3: Execução (ALU e Shifter) e Carga do MAR
    // A ALU opera sobre os dados dos Latches. O MAR é carregado se necessário.
    // ====================================================================
    
    // 1. AMUX: Seleciona se a entrada A da ALU vem do Latch A ou do MBR
    cpu->amux.control_amux = cpu->mir.amux;
    run_amux(&cpu->amux, &cpu->mbr, &cpu->latch_a, &cpu->alu);

    // 2. Prepara entrada B da ALU (vem direto do Latch B)
    copy_array(cpu->latch_b.data, cpu->alu.input_b);

    // 3. Executa a ALU
    // Combina os 2 bits de controle (alu[0], alu[1])
    int alu_ctrl = (cpu->mir.alu[0] << 1) | cpu->mir.alu[1];
    set_alu_control(&cpu->alu, alu_ctrl);
    run_alu(&cpu->alu); // Gera flags N e Z

    // 4. Executa o Shifter (Deslocador)
    // A saída da ALU passa pelo Shifter e vai para o Barramento C (simulado aqui)
    set_shifter_input(&cpu->shifter, cpu->alu.output);
    set_shifter_control(&cpu->shifter, cpu->mir.sh);
    run_shifter(&cpu->shifter); // Resultado final está em cpu->shifter.data

    // 5. Carrega MAR (se o bit MAR do MIR for 1)
    // O MAR recebe dados do Latch B (Barramento B), conforme Tanenbaum
    cpu->mar.control_mar = cpu->mir.mar;
    run_mar(&cpu->mar, &cpu->latch_b);

    // ====================================================================
    // SUBCICLO 4: Escrita (Write-back) e Microssequenciamento
    // O resultado do Shifter é escrito nos registradores e/ou MBR.
    // O MPC é atualizado para o endereço da próxima microinstrução.
    // ====================================================================

    // 1. Controle do MBR e Acesso à Memória Principal
    cpu->mbr.control_mbr = cpu->mir.mbr; // Se 1, carrega do Shifter (Barramento C)
    cpu->mbr.control_rd  = cpu->mir.rd;  // Se 1, lê da Memória Principal
    cpu->mbr.control_wr  = cpu->mir.wr;  // Se 1, escreve na Memória Principal
    
    // Executa lógica do MBR (pode ler da RAM, escrever na RAM ou pegar do Shifter)
    run_mbr(&cpu->mar, &cpu->mbr, &cpu->main_memory, &cpu->shifter);

    // 2. Escrita no Banco de Registradores (via Decoder C)
    for(int i=0; i<4; i++) cpu->decoder_c.control_c[i] = cpu->mir.c[i];
    cpu->decoder_c.control_enc = cpu->mir.enc;
    
    // Escreve o dado do Shifter no registrador selecionado
    run_decoderC(&cpu->decoder_c, &cpu->shifter);

    // 3. Atualiza MPC (Próxima Microinstrução)
    // Passa as flags da ALU para o MMUX decidir se deve saltar ou não
    cpu->mmux.alu_n = cpu->alu.flag_n;
    cpu->mmux.alu_z = cpu->alu.flag_z;
    cpu->mmux.control_cond[0] = cpu->mir.cond[0];
    cpu->mmux.control_cond[1] = cpu->mir.cond[1];

    // Decide: MPC = (MPC + 1) ou MPC = MIR.ADDR
    update_control(&cpu->mpc, &cpu->mmux, &cpu->mir);

    cpu->cycle_count++;
}

void run_mic1_program(mic1_cpu* cpu) {
    if (!cpu) return;
    
    cpu->running = 1;
    int auto_mode = 0; // 0 = Manual, 1 = Automático

    init_ui(); // Prepara o terminal

    while(cpu->running) {
        draw_dashboard(cpu);
        
        if (auto_mode) {
            // Modo automático: roda e espera um pouco
            Sleep(50); // 50ms de delay (ajuste se quiser mais lento)
            
            // Se apertar algo, pausa ou sai do automático
            if (_kbhit()) {
                char ch = _getch();
                if (ch == 'a' || ch == 'A') auto_mode = 0; // Para
                if (ch == 'q' || ch == 'Q') cpu->running = 0;
            }
            step_mic1(cpu);
        } 
        else {
            // Modo manual: espera tecla
            char ch = _getch(); 
            
            if (ch == 'q' || ch == 'Q') cpu->running = 0;
            else if (ch == 'r' || ch == 'R') {
                reset_mic1(cpu);
                // Recarrega microcódigo se necessário, ou apenas zera regs
            }
            else if (ch == 'a' || ch == 'A') {
                auto_mode = 1; // Liga automático
            }
            else if (ch == ' ') {
                step_mic1(cpu); // Passo a passo
            }
        }
    }
    
    close_ui(); // Restaura terminal
    printf("Simulacao finalizada. Total ciclos: %d\n", cpu->cycle_count);
}

// Wrappers para carregamento de arquivos
int load_microprogram_file(mic1_cpu* cpu, const char* filename) {
    if(!cpu || !filename) return -1;
    return load_microprogram(&cpu->ctrl_mem, filename);
}

int load_program_file(mic1_cpu* cpu, const char* filename) {
    if(!cpu || !filename) return -1;
    // Nota: load_program está em memory.c
    load_program(&cpu->main_memory, filename); 
    return 0;
}

// Funções de Debug e Visualização
void print_cpu_state(mic1_cpu* cpu) {
    printf("Ciclo: %4d | MPC: %3d | PC: %4d | AC: %6d\n", 
        cpu->cycle_count,
        bits_to_int(cpu->mpc.address, 8),
        array_to_int(cpu->reg_bank.PC.data),
        array_to_int(cpu->reg_bank.AC.data));
}

void print_registers(mic1_cpu* cpu) {
    printf("=== Registradores ===\n");
    printf("PC : %6d  (Program Counter)\n", array_to_int(cpu->reg_bank.PC.data));
    printf("AC : %6d  (Accumulator)\n", array_to_int(cpu->reg_bank.AC.data));
    printf("SP : %6d  (Stack Pointer)\n", array_to_int(cpu->reg_bank.SP.data));
    printf("TIR: %6d  (Temp IR)\n", array_to_int(cpu->reg_bank.TIR.data));
    printf("CPP: %6d  (Constant Pool Ptr - LV no Tanenbaum)\n", array_to_int(cpu->reg_bank.F.data)); // F geralmente usado como LV/CPP
    printf("MBR: %6d\n", array_to_int(cpu->mbr.data));
    printf("MAR: %6d\n", address_to_int(cpu->mar.address));
    printf("=====================\n");
}