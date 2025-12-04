// Arquivo: src/view_windows.c
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h> // Para _getch() e _kbhit()

#include "../include/mic1.h"
#include "../include/utils/conversions.h"

// --- Funções Auxiliares do Windows ---

void init_ui() {
    // Esconde o cursor piscante
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
    
    // Limpa a tela uma vez
    system("cls");
}

void close_ui() {
    // Restaura o cursor
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 20;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
    
    // Reseta cores
    SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

// Move o cursor sem limpar a tela (evita flicker)
void move_cursor(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Define cor do texto (Foreground + Intensity para brilho)
// 1=Azul, 2=Verde, 3=Ciano, 4=Vermelho, 6=Amarelo, 7=Branco, 0=Reset
void set_color(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Default (Branco)
    
    switch(color) {
        case 1: attr = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break; // Label (Azul)
        case 2: attr = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break; // Valor (Verde)
        case 3: attr = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break; // Ciano
        case 4: attr = FOREGROUND_RED | FOREGROUND_INTENSITY; break; // Destaque/Erro
        case 5: attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break; // Amarelo
        case 7: attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break; // Branco Forte
    }
    SetConsoleTextAttribute(hConsole, attr);
}

// --- Desenho do Dashboard ---

void draw_dashboard(mic1_cpu *cpu) {
    // Converte valores para facilitar
    int pc = array_to_int(cpu->reg_bank.PC.data);
    int ac = array_to_int(cpu->reg_bank.AC.data);
    int sp = array_to_int(cpu->reg_bank.SP.data);
    int tir = array_to_int(cpu->reg_bank.TIR.data);
    int mar = address_to_int(cpu->mar.address);
    int mbr = array_to_int(cpu->mbr.data);
    int mpc = bits_to_int(cpu->mpc.address, 8);

    // INÍCIO DO DESENHO
    move_cursor(0, 0); // Topo da tela

    set_color(3); // Ciano
    printf("===============================================================================\n");
    printf("  SIMULADOR MIC-1 (Windows Nativo) | Ciclo: %-6d | MPC: 0x%02X (%d)   \n", cpu->cycle_count, mpc, mpc);
    printf("===============================================================================\n");

    // LINHA 1: Cabeçalhos
    set_color(5); // Amarelo
    move_cursor(2, 3);  printf("REGISTRADORES PRINCIPAIS");
    move_cursor(40, 3); printf("BARRAMENTOS E LATCHES");
    
    // COLUNA 1: Registradores
    set_color(7); // Branco
    move_cursor(2, 5); printf("PC  : "); set_color(2); printf("%04X", pc & 0xFFFF); set_color(7); printf(" (%d)", pc);
    move_cursor(2, 6); printf("AC  : "); set_color(2); printf("%04X", ac & 0xFFFF); set_color(7); printf(" (%d)", ac);
    move_cursor(2, 7); printf("SP  : "); set_color(2); printf("%04X", sp & 0xFFFF); set_color(7); printf(" (%d)", sp);
    move_cursor(2, 8); printf("TIR : "); set_color(2); printf("%04X", tir & 0xFFFF); set_color(7); printf(" (%d)", tir);

    // COLUNA 2: Datapath
    move_cursor(40, 5); printf("Latch A : %04X", array_to_int(cpu->latch_a.data) & 0xFFFF);
    move_cursor(40, 6); printf("Latch B : %04X", array_to_int(cpu->latch_b.data) & 0xFFFF);
    move_cursor(40, 7); printf("ALU Out : %04X", array_to_int(cpu->alu.output) & 0xFFFF);
    
    // Flags
    move_cursor(60, 7);
    if(cpu->alu.flag_n) { set_color(4); printf("[N]"); } else { set_color(7); printf("[ ]"); }
    if(cpu->alu.flag_z) { set_color(4); printf("[Z]"); } else { set_color(7); printf("[ ]"); }

    move_cursor(40, 8); 
    set_color(7); printf("Shifter : %04X", array_to_int(cpu->shifter.data) & 0xFFFF);

    // LINHA 2: Memória e Controle
    set_color(5); // Amarelo
    move_cursor(2, 11); printf("INTERFACE MEMORIA");
    move_cursor(40, 11); printf("MICROINSTRUCAO (MIR)");

    set_color(7);
    move_cursor(2, 13); printf("MAR : "); set_color(2); printf("%03X", mar); set_color(7); printf(" (%d)", mar);
    move_cursor(2, 14); printf("MBR : "); set_color(2); printf("%04X", mbr & 0xFFFF); set_color(7); printf(" (%d)", mbr);

    // Detalhes do MIR
    move_cursor(40, 13); printf("AMUX: %d  COND: %d%d", cpu->mir.amux, cpu->mir.cond[0], cpu->mir.cond[1]);
    move_cursor(40, 14); printf("ALU : %d%d  SH  : %d%d", cpu->mir.alu[0], cpu->mir.alu[1], cpu->mir.sh[0], cpu->mir.sh[1]);
    move_cursor(40, 15); printf("MEM : rd=%d wr=%d fetch=%d", cpu->mir.rd, cpu->mir.wr, cpu->mir.mbr);

    // Rodapé
    set_color(3);
    move_cursor(0, 18);
    printf("===============================================================================\n");
    set_color(7);
    printf(" [ESPACO] Proximo Passo  |  [A] Automatico  |  [R] Reset  |  [Q] Sair   \n");
}