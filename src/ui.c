/**
 * MIC-1 TUI Implementation
 * Native C terminal UI using termbox2
 */

#define TB_IMPL
#include "../include/termbox2.h"
#include "../include/ui.h"
#include "../include/mic1.h"
#include "../include/utils/conversions.h"
#include <stdio.h>
#include <string.h>

/* ============================================================
 * INITIALIZATION
 * ============================================================ */

int ui_init(void) {
    int ret = tb_init();
    if (ret != TB_OK) {
        fprintf(stderr, "Failed to initialize termbox: %d\n", ret);
        return -1;
    }
    return 0;
}

void ui_shutdown(void) {
    tb_shutdown();
}

/* ============================================================
 * DRAWING PRIMITIVES
 * ============================================================ */

void ui_fill_rect(int x, int y, int w, int h, uint32_t ch, uint16_t fg, uint16_t bg) {
    for (int row = y; row < y + h; row++) {
        for (int col = x; col < x + w; col++) {
            tb_set_cell(col, row, ch, fg, bg);
        }
    }
}

void ui_draw_box(int x, int y, int w, int h, const char *title, uint16_t border_color) {
    if (w < 2 || h < 2) return;

    /* Corners */
    tb_set_cell(x, y, BOX_TL, border_color, TB_DEFAULT);
    tb_set_cell(x + w - 1, y, BOX_TR, border_color, TB_DEFAULT);
    tb_set_cell(x, y + h - 1, BOX_BL, border_color, TB_DEFAULT);
    tb_set_cell(x + w - 1, y + h - 1, BOX_BR, border_color, TB_DEFAULT);

    /* Horizontal lines */
    for (int i = 1; i < w - 1; i++) {
        tb_set_cell(x + i, y, BOX_H, border_color, TB_DEFAULT);
        tb_set_cell(x + i, y + h - 1, BOX_H, border_color, TB_DEFAULT);
    }

    /* Vertical lines */
    for (int i = 1; i < h - 1; i++) {
        tb_set_cell(x, y + i, BOX_V, border_color, TB_DEFAULT);
        tb_set_cell(x + w - 1, y + i, BOX_V, border_color, TB_DEFAULT);
    }

    /* Title */
    if (title && strlen(title) > 0) {
        int tlen = strlen(title);
        int tx = x + (w - tlen - 4) / 2;
        if (tx < x + 1) tx = x + 1;

        tb_set_cell(tx, y, BOX_RT, border_color, TB_DEFAULT);
        tb_print(tx + 1, y, UI_COLOR_TITLE, TB_DEFAULT, " ");
        tb_print(tx + 2, y, UI_COLOR_TITLE, TB_DEFAULT, title);
        tb_print(tx + 2 + tlen, y, UI_COLOR_TITLE, TB_DEFAULT, " ");
        tb_set_cell(tx + 3 + tlen, y, BOX_LT, border_color, TB_DEFAULT);
    }
}

void ui_draw_box_double(int x, int y, int w, int h, const char *title) {
    if (w < 2 || h < 2) return;

    uint16_t bc = UI_COLOR_BORDER | TB_BOLD;

    /* Corners */
    tb_set_cell(x, y, BOX2_TL, bc, TB_DEFAULT);
    tb_set_cell(x + w - 1, y, BOX2_TR, bc, TB_DEFAULT);
    tb_set_cell(x, y + h - 1, BOX2_BL, bc, TB_DEFAULT);
    tb_set_cell(x + w - 1, y + h - 1, BOX2_BR, bc, TB_DEFAULT);

    /* Horizontal lines */
    for (int i = 1; i < w - 1; i++) {
        tb_set_cell(x + i, y, BOX2_H, bc, TB_DEFAULT);
        tb_set_cell(x + i, y + h - 1, BOX2_H, bc, TB_DEFAULT);
    }

    /* Vertical lines */
    for (int i = 1; i < h - 1; i++) {
        tb_set_cell(x, y + i, BOX2_V, bc, TB_DEFAULT);
        tb_set_cell(x + w - 1, y + i, BOX2_V, bc, TB_DEFAULT);
    }

    /* Title */
    if (title && strlen(title) > 0) {
        int tlen = strlen(title);
        int tx = x + (w - tlen - 2) / 2;
        tb_print(tx, y, UI_COLOR_TITLE, TB_DEFAULT, " ");
        tb_print(tx + 1, y, UI_COLOR_TITLE, TB_DEFAULT, title);
        tb_print(tx + 1 + tlen, y, UI_COLOR_TITLE, TB_DEFAULT, " ");
    }
}

/* ============================================================
 * INSTRUCTION DECODING
 * ============================================================ */

static const char *opcode_names[] = {
    "LODD", "STOD", "ADDD", "SUBD",  /* 0-3 */
    "JPOS", "JZER", "JUMP", "LOCO",  /* 4-7 */
    "LODL", "STOL", "ADDL", "SUBL",  /* 8-B */
    "JNEG", "JNZE", "CALL", "F-OP"   /* C-F */
};

static const char *fop_names[] = {
    "PSHI", "????", "POPI", "????",
    "PUSH", "????", "POP ", "????",
    "RETN", "????", "SWAP", "????",
    "INSP", "????", "DESP", "????"
};

const char *ui_opcode_name(int opcode) {
    if (opcode >= 0 && opcode <= 0xF)
        return opcode_names[opcode];
    return "????";
}

void ui_format_instruction(char *buf, int instr) {
    int opcode = (instr >> 12) & 0xF;
    int operand = instr & 0x0FFF;

    if (opcode == 0xF) {
        int subop = (operand >> 8) & 0xF;
        const char *name = fop_names[subop];
        if (subop == 0xC || subop == 0xE) {
            /* INSP/DESP have operand */
            sprintf(buf, "%s %03X", name, operand & 0xFF);
        } else {
            sprintf(buf, "%s", name);
        }
    } else {
        sprintf(buf, "%s %03X", opcode_names[opcode], operand);
    }
}

/* ============================================================
 * HIGH-LEVEL PANELS
 * ============================================================ */

void ui_draw_registers(int x, int y, mic1_cpu *cpu) {
    int w = 20, h = 10;
    ui_draw_box(x, y, w, h, "Registers", UI_COLOR_BORDER);

    int pc = bits_to_int(cpu->reg_bank.PC.data, 16);
    int ac = bits_to_int(cpu->reg_bank.AC.data, 16);
    int sp = bits_to_int(cpu->reg_bank.SP.data, 16);
    int ir = bits_to_int(cpu->reg_bank.IR.data, 16);

    char buf[32];

    /* PC */
    tb_print(x + 2, y + 2, UI_COLOR_LABEL, TB_DEFAULT, "PC:");
    sprintf(buf, "%04X", pc);
    tb_print(x + 7, y + 2, UI_COLOR_VALUE, TB_DEFAULT, buf);

    /* AC */
    tb_print(x + 2, y + 3, UI_COLOR_LABEL, TB_DEFAULT, "AC:");
    sprintf(buf, "%04X", ac);
    tb_print(x + 7, y + 3, UI_COLOR_VALUE, TB_DEFAULT, buf);
    sprintf(buf, "(%d)", (int16_t)ac);
    tb_print(x + 12, y + 3, TB_WHITE, TB_DEFAULT, buf);

    /* SP */
    tb_print(x + 2, y + 4, UI_COLOR_LABEL, TB_DEFAULT, "SP:");
    sprintf(buf, "%04X", sp);
    tb_print(x + 7, y + 4, UI_COLOR_VALUE, TB_DEFAULT, buf);

    /* IR */
    tb_print(x + 2, y + 5, UI_COLOR_LABEL, TB_DEFAULT, "IR:");
    sprintf(buf, "%04X", ir);
    tb_print(x + 7, y + 5, UI_COLOR_VALUE, TB_DEFAULT, buf);

    /* Decoded instruction */
    tb_print(x + 2, y + 7, TB_WHITE, TB_DEFAULT, "Instr:");
    ui_format_instruction(buf, ir);
    tb_print(x + 9, y + 7, UI_COLOR_MNEMONIC, TB_DEFAULT, buf);
}

void ui_draw_code(int x, int y, int w, int h, mic1_cpu *cpu, int highlight_pc) {
    ui_draw_box(x, y, w, h, "Code", UI_COLOR_BORDER);

    int pc = bits_to_int(cpu->reg_bank.PC.data, 16);
    int visible_lines = h - 2;
    int start_addr = pc - visible_lines / 2;
    if (start_addr < 0) start_addr = 0;

    char buf[64];
    for (int i = 0; i < visible_lines && (start_addr + i) < MEMORY_SIZE; i++) {
        int addr = start_addr + i;
        int instr = bits_to_int(cpu->main_memory.data[addr], 16);

        int row = y + 1 + i;
        int is_current = (addr == highlight_pc);

        /* Highlight current line */
        uint16_t fg = TB_WHITE;
        uint16_t bg = TB_DEFAULT;
        if (is_current) {
            fg = UI_COLOR_HIGHLIGHT;
            bg = UI_COLOR_BG_HL;
            /* Fill background */
            for (int j = 1; j < w - 1; j++) {
                tb_set_cell(x + j, row, ' ', fg, bg);
            }
        }

        /* Address */
        sprintf(buf, "%03X:", addr);
        tb_print(x + 2, row, is_current ? fg : UI_COLOR_ADDR, bg, buf);

        /* Raw instruction */
        sprintf(buf, "%04X", instr);
        tb_print(x + 7, row, is_current ? fg : TB_WHITE, bg, buf);

        /* Decoded */
        ui_format_instruction(buf, instr);
        tb_print(x + 13, row, is_current ? fg : UI_COLOR_MNEMONIC, bg, buf);

        /* Stop if we hit a halt (JUMP to self) */
        int opcode = (instr >> 12) & 0xF;
        int operand = instr & 0x0FFF;
        if (opcode == 6 && operand == addr) {
            tb_print(x + 22, row, TB_RED, bg, "[HALT]");
        }
    }
}

void ui_draw_stack(int x, int y, int h, mic1_cpu *cpu) {
    int w = 16;
    ui_draw_box(x, y, w, h, "Stack", UI_COLOR_BORDER);

    int sp = bits_to_int(cpu->reg_bank.SP.data, 16);
    int visible = h - 2;

    char buf[32];
    for (int i = 0; i < visible; i++) {
        int addr = sp + i;
        if (addr >= MEMORY_SIZE) break;

        int val = bits_to_int(cpu->main_memory.data[addr], 16);
        int row = y + 1 + i;

        /* Mark SP position */
        if (i == 0) {
            tb_print(x + 1, row, TB_YELLOW | TB_BOLD, TB_DEFAULT, ">");
        }

        sprintf(buf, "%03X: %04X", addr, val);
        tb_print(x + 2, row, i == 0 ? (TB_GREEN | TB_BOLD) : TB_WHITE, TB_DEFAULT, buf);
    }
}

void ui_draw_memory(int x, int y, int w, int h, mic1_cpu *cpu, int start_addr) {
    ui_draw_box(x, y, w, h, "Memory", UI_COLOR_BORDER);

    int visible = h - 2;
    char buf[64];

    for (int i = 0; i < visible; i++) {
        int addr = start_addr + i * 8;
        if (addr >= MEMORY_SIZE) break;

        int row = y + 1 + i;

        /* Address */
        sprintf(buf, "%03X:", addr);
        tb_print(x + 2, row, UI_COLOR_ADDR, TB_DEFAULT, buf);

        /* 8 words per line */
        int col = x + 7;
        for (int j = 0; j < 8 && (addr + j) < MEMORY_SIZE; j++) {
            int val = bits_to_int(cpu->main_memory.data[addr + j], 16);
            sprintf(buf, "%04X", val);
            tb_print(col, row, TB_WHITE, TB_DEFAULT, buf);
            col += 5;
        }
    }
}

void ui_draw_help(int x, int y) {
    int w = 26, h = 10;
    ui_draw_box(x, y, w, h, "Controls", UI_COLOR_BORDER);

    tb_print(x + 2, y + 2, UI_COLOR_LABEL, TB_DEFAULT, "s");
    tb_print(x + 5, y + 2, UI_COLOR_HELP, TB_DEFAULT, "Step (1 cycle)");

    tb_print(x + 2, y + 3, UI_COLOR_LABEL, TB_DEFAULT, "r");
    tb_print(x + 5, y + 3, UI_COLOR_HELP, TB_DEFAULT, "Run/Pause");

    tb_print(x + 2, y + 4, UI_COLOR_LABEL, TB_DEFAULT, "x");
    tb_print(x + 5, y + 4, UI_COLOR_HELP, TB_DEFAULT, "Reset CPU");

    tb_print(x + 2, y + 5, UI_COLOR_LABEL, TB_DEFAULT, "+/-");
    tb_print(x + 5, y + 5, UI_COLOR_HELP, TB_DEFAULT, "Speed up/down");

    tb_print(x + 2, y + 6, UI_COLOR_LABEL, TB_DEFAULT, "m");
    tb_print(x + 5, y + 6, UI_COLOR_HELP, TB_DEFAULT, "Memory view");

    tb_print(x + 2, y + 7, UI_COLOR_LABEL, TB_DEFAULT, "q/ESC");
    tb_print(x + 8, y + 7, UI_COLOR_HELP, TB_DEFAULT, "Quit");
}

void ui_draw_status(int y, ui_state_t *state) {
    int w = tb_width();
    char buf[128];

    /* Clear status line */
    for (int i = 0; i < w; i++) {
        tb_set_cell(i, y, ' ', TB_WHITE, TB_BLUE);
    }

    /* Left side: mode and cycle count */
    sprintf(buf, " %s | Cycle: %d",
            state->auto_run ? "RUNNING" : "PAUSED",
            state->cycle_count);
    tb_print(0, y, TB_WHITE | TB_BOLD, TB_BLUE, buf);

    /* Right side: loaded file */
    if (state->loaded_file[0]) {
        int len = strlen(state->loaded_file);
        tb_print(w - len - 2, y, TB_YELLOW, TB_BLUE, state->loaded_file);
    }
}
