/**
 * MIC-1 Interactive TUI
 *
 * Terminal-based visual debugger using termbox2.
 *
 * Usage: ./mic1_tui <program.bin>
 *
 * Controls:
 *   s     - Step (execute 1 cycle)
 *   r     - Run/Pause continuous execution
 *   x     - Reset CPU
 *   +/-   - Adjust speed
 *   m     - Toggle memory view
 *   q/ESC - Quit
 */

#include "../include/mic1.h"
#include "../include/ui.h"
#include "../include/termbox2.h"
#include "../include/utils/conversions.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Helper macros */
#define REG16(reg) bits_to_int((reg).data, 16)
#define MEM16(mem) bits_to_int(mem, 16)

/* Global state */
static mic1_cpu cpu;
static ui_state_t ui_state;
static int show_memory_panel = 0;
static int memory_view_addr = 0x000;

/**
 * Initialize SP to top of stack (0x0FFF)
 */
static void init_sp(mic1_cpu *cpu) {
    for (int i = 0; i < 16; i++) {
        cpu->reg_bank.SP.data[i] = 0;
    }
    for (int i = 0; i < 12; i++) {
        cpu->reg_bank.SP.data[i] = 1;
    }
}

/**
 * Reset CPU state
 */
static void reset_cpu(void) {
    init_mic1(&cpu);
    init_sp(&cpu);
    cpu.running = 1;
    ui_state.cycle_count = 0;
    ui_state.auto_run = 0;
    strcpy(ui_state.status_msg, "CPU Reset");
}

/**
 * Execute one step
 */
static void do_step(void) {
    if (!cpu.running) return;

    step_mic1(&cpu);
    ui_state.cycle_count++;

    /* Check for halt */
    int pc = REG16(cpu.reg_bank.PC);
    int instr = MEM16(cpu.main_memory.data[pc]);
    int opcode = (instr >> 12) & 0xF;
    int operand = instr & 0x0FFF;

    if (opcode == 0x6 && operand == pc) {
        cpu.running = 0;
        ui_state.auto_run = 0;
        strcpy(ui_state.status_msg, "HALT detected");
    }
}

/**
 * Draw all UI panels
 */
static void draw_ui(void) {
    int w = tb_width();
    int h = tb_height();

    tb_clear();

    /* Title bar */
    char title[64];
    sprintf(title, " MIC-1 Simulator v1.0 ");
    int tx = (w - strlen(title)) / 2;
    for (int i = 0; i < w; i++) {
        tb_set_cell(i, 0, ' ', TB_WHITE, TB_BLUE);
    }
    tb_print(tx, 0, TB_WHITE | TB_BOLD, TB_BLUE, title);

    /* Layout calculation */
    int panel_y = 2;
    int code_h = h - 14;
    if (code_h < 10) code_h = 10;

    /* Left column: Registers + Help */
    ui_draw_registers(1, panel_y, &cpu);
    ui_draw_help(1, panel_y + 11);

    /* Center: Code view */
    int code_x = 23;
    int code_w = 38;
    int pc = REG16(cpu.reg_bank.PC);
    ui_draw_code(code_x, panel_y, code_w, code_h, &cpu, pc);

    /* Right: Stack or Memory */
    int right_x = code_x + code_w + 1;
    int right_w = w - right_x - 1;

    if (show_memory_panel) {
        ui_draw_memory(right_x, panel_y, right_w > 48 ? 48 : right_w, code_h, &cpu, memory_view_addr);
    } else {
        ui_draw_stack(right_x, panel_y, code_h, &cpu);
    }

    /* Status bar at bottom */
    ui_draw_status(h - 1, &ui_state);

    tb_present();
}

/**
 * Handle keyboard input
 */
static int handle_input(void) {
    struct tb_event ev;

    if (tb_poll_event(&ev) != TB_OK) {
        return 1;  /* Continue */
    }

    if (ev.type == TB_EVENT_RESIZE) {
        /* Screen resized, just redraw */
        return 1;
    }

    if (ev.type != TB_EVENT_KEY) {
        return 1;
    }

    /* Handle key press */
    if (ev.key == TB_KEY_ESC || ev.ch == 'q' || ev.ch == 'Q') {
        return 0;  /* Quit */
    }

    switch (ev.ch) {
        case 's':
        case 'S':
            /* Single step */
            do_step();
            break;

        case 'r':
        case 'R':
            /* Toggle run/pause */
            ui_state.auto_run = !ui_state.auto_run;
            strcpy(ui_state.status_msg, ui_state.auto_run ? "Running..." : "Paused");
            break;

        case 'x':
        case 'X':
            /* Reset */
            reset_cpu();
            /* Reload program */
            if (ui_state.loaded_file[0]) {
                load_program_file(&cpu, ui_state.loaded_file);
            }
            break;

        case '+':
        case '=':
            /* Speed up */
            if (ui_state.speed_ms > 10) ui_state.speed_ms -= 10;
            break;

        case '-':
        case '_':
            /* Slow down */
            if (ui_state.speed_ms < 500) ui_state.speed_ms += 10;
            break;

        case 'm':
        case 'M':
            /* Toggle memory view */
            show_memory_panel = !show_memory_panel;
            break;

        case '[':
            /* Memory view: previous page */
            if (memory_view_addr >= 64) memory_view_addr -= 64;
            break;

        case ']':
            /* Memory view: next page */
            if (memory_view_addr < MEMORY_SIZE - 64) memory_view_addr += 64;
            break;

        default:
            break;
    }

    /* Handle arrow keys for memory navigation */
    switch (ev.key) {
        case TB_KEY_ARROW_UP:
            if (memory_view_addr >= 8) memory_view_addr -= 8;
            break;
        case TB_KEY_ARROW_DOWN:
            if (memory_view_addr < MEMORY_SIZE - 8) memory_view_addr += 8;
            break;
        default:
            break;
    }

    return 1;  /* Continue */
}

/**
 * Main event loop
 */
static void main_loop(void) {
    ui_state.running = 1;

    while (ui_state.running) {
        draw_ui();

        /* Auto-run mode: step automatically */
        if (ui_state.auto_run && cpu.running) {
            do_step();

            /* Small delay for visibility */
            struct timespec ts = {0, ui_state.speed_ms * 1000000L};
            nanosleep(&ts, NULL);
        }

        /* Handle input (non-blocking in auto-run mode) */
        ui_state.running = handle_input();
    }
}

/**
 * Check file extension
 */
static int has_extension(const char *filename, const char *ext) {
    size_t fname_len = strlen(filename);
    size_t ext_len = strlen(ext);
    if (fname_len < ext_len) return 0;
    return strcmp(filename + fname_len - ext_len, ext) == 0;
}

int main(int argc, char *argv[]) {
    /* Parse arguments */
    if (argc < 2) {
        fprintf(stderr, "MIC-1 Interactive TUI\n");
        fprintf(stderr, "Usage: %s <program.bin>\n", argv[0]);
        fprintf(stderr, "\nControls:\n");
        fprintf(stderr, "  s     - Step (1 cycle)\n");
        fprintf(stderr, "  r     - Run/Pause\n");
        fprintf(stderr, "  x     - Reset CPU\n");
        fprintf(stderr, "  m     - Toggle memory view\n");
        fprintf(stderr, "  q/ESC - Quit\n");
        return 1;
    }

    /* Check for .asm files */
    if (has_extension(argv[1], ".asm")) {
        fprintf(stderr, "Error: Cannot load .asm files directly.\n");
        fprintf(stderr, "First compile with: ./mic1asm %s output.bin\n", argv[1]);
        return 1;
    }

    /* Initialize CPU */
    init_mic1(&cpu);
    init_sp(&cpu);

    /* Load program */
    if (load_program_file(&cpu, argv[1]) != 0) {
        fprintf(stderr, "Error: Failed to load '%s'\n", argv[1]);
        return 1;
    }

    /* Initialize UI state */
    memset(&ui_state, 0, sizeof(ui_state));
    ui_state.speed_ms = 100;
    strncpy(ui_state.loaded_file, argv[1], sizeof(ui_state.loaded_file) - 1);
    strcpy(ui_state.status_msg, "Ready");
    cpu.running = 1;

    /* Initialize TUI */
    if (ui_init() != 0) {
        fprintf(stderr, "Error: Failed to initialize terminal UI\n");
        return 1;
    }

    /* Run main loop */
    main_loop();

    /* Cleanup */
    ui_shutdown();

    printf("\nExited after %d cycles.\n", ui_state.cycle_count);
    printf("Final state: PC=%04X AC=%04X SP=%04X\n",
           REG16(cpu.reg_bank.PC),
           REG16(cpu.reg_bank.AC),
           REG16(cpu.reg_bank.SP));

    return 0;
}
