/**
 * MIC-1 TUI Interface
 * Native C terminal UI using termbox2
 */

#ifndef UI_H
#define UI_H

#include <stdint.h>
#include "mic1.h"

/* Box drawing characters (Unicode) */
#define BOX_TL 0x250C  /* ┌ */
#define BOX_TR 0x2510  /* ┐ */
#define BOX_BL 0x2514  /* └ */
#define BOX_BR 0x2518  /* ┘ */
#define BOX_H  0x2500  /* ─ */
#define BOX_V  0x2502  /* │ */
#define BOX_LT 0x251C  /* ├ */
#define BOX_RT 0x2524  /* ┤ */
#define BOX_TT 0x252C  /* ┬ */
#define BOX_BT 0x2534  /* ┴ */
#define BOX_X  0x253C  /* ┼ */

/* Double-line box characters */
#define BOX2_TL 0x2554  /* ╔ */
#define BOX2_TR 0x2557  /* ╗ */
#define BOX2_BL 0x255A  /* ╚ */
#define BOX2_BR 0x255D  /* ╝ */
#define BOX2_H  0x2550  /* ═ */
#define BOX2_V  0x2551  /* ║ */

/* UI Colors */
#define UI_COLOR_TITLE    (TB_CYAN | TB_BOLD)
#define UI_COLOR_BORDER   TB_WHITE
#define UI_COLOR_LABEL    TB_YELLOW
#define UI_COLOR_VALUE    TB_GREEN
#define UI_COLOR_HIGHLIGHT (TB_BLACK | TB_BOLD)
#define UI_COLOR_BG_HL    TB_YELLOW
#define UI_COLOR_MNEMONIC TB_CYAN
#define UI_COLOR_ADDR     TB_MAGENTA
#define UI_COLOR_HELP     TB_WHITE
#define UI_COLOR_STATUS   (TB_GREEN | TB_BOLD)

/* Panel positions (will be computed dynamically) */
typedef struct {
    int x, y, w, h;
} ui_rect_t;

/* UI State */
typedef struct {
    int running;
    int auto_run;
    int cycle_count;
    int speed_ms;
    char status_msg[64];
    char loaded_file[128];
} ui_state_t;

/* Core functions */
int ui_init(void);
void ui_shutdown(void);

/* Drawing primitives */
void ui_draw_box(int x, int y, int w, int h, const char *title, uint16_t border_color);
void ui_draw_box_double(int x, int y, int w, int h, const char *title);
void ui_fill_rect(int x, int y, int w, int h, uint32_t ch, uint16_t fg, uint16_t bg);

/* High-level panels */
void ui_draw_registers(int x, int y, mic1_cpu *cpu);
void ui_draw_code(int x, int y, int w, int h, mic1_cpu *cpu, int highlight_pc);
void ui_draw_stack(int x, int y, int h, mic1_cpu *cpu);
void ui_draw_memory(int x, int y, int w, int h, mic1_cpu *cpu, int start_addr);
void ui_draw_help(int x, int y);
void ui_draw_status(int y, ui_state_t *state);

/* Utility */
const char *ui_opcode_name(int opcode);
void ui_format_instruction(char *buf, int instr);

#endif /* UI_H */
