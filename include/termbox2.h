/*
 * Termbox2 - Minimal Terminal UI Library
 * Single-header implementation for ArchSim-MIC1
 * Based on termbox2 by nsf & Adam Saponara (MIT License)
 */

#ifndef TERMBOX2_H
#define TERMBOX2_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <wchar.h>
#include <locale.h>

/* Colors */
#define TB_DEFAULT 0x0000
#define TB_BLACK   0x0001
#define TB_RED     0x0002
#define TB_GREEN   0x0003
#define TB_YELLOW  0x0004
#define TB_BLUE    0x0005
#define TB_MAGENTA 0x0006
#define TB_CYAN    0x0007
#define TB_WHITE   0x0008

/* Attributes */
#define TB_BOLD      0x0100
#define TB_UNDERLINE 0x0200
#define TB_REVERSE   0x0400

/* Keys */
#define TB_KEY_F1             (0xFFFF-0)
#define TB_KEY_F2             (0xFFFF-1)
#define TB_KEY_F3             (0xFFFF-2)
#define TB_KEY_F4             (0xFFFF-3)
#define TB_KEY_F5             (0xFFFF-4)
#define TB_KEY_F6             (0xFFFF-5)
#define TB_KEY_F7             (0xFFFF-6)
#define TB_KEY_F8             (0xFFFF-7)
#define TB_KEY_F9             (0xFFFF-8)
#define TB_KEY_F10            (0xFFFF-9)
#define TB_KEY_F11            (0xFFFF-10)
#define TB_KEY_F12            (0xFFFF-11)
#define TB_KEY_INSERT         (0xFFFF-12)
#define TB_KEY_DELETE         (0xFFFF-13)
#define TB_KEY_HOME           (0xFFFF-14)
#define TB_KEY_END            (0xFFFF-15)
#define TB_KEY_PGUP           (0xFFFF-16)
#define TB_KEY_PGDN           (0xFFFF-17)
#define TB_KEY_ARROW_UP       (0xFFFF-18)
#define TB_KEY_ARROW_DOWN     (0xFFFF-19)
#define TB_KEY_ARROW_LEFT     (0xFFFF-20)
#define TB_KEY_ARROW_RIGHT    (0xFFFF-21)
#define TB_KEY_CTRL_TILDE     0x00
#define TB_KEY_CTRL_A         0x01
#define TB_KEY_CTRL_B         0x02
#define TB_KEY_CTRL_C         0x03
#define TB_KEY_CTRL_D         0x04
#define TB_KEY_CTRL_E         0x05
#define TB_KEY_CTRL_F         0x06
#define TB_KEY_CTRL_G         0x07
#define TB_KEY_BACKSPACE      0x08
#define TB_KEY_TAB            0x09
#define TB_KEY_CTRL_J         0x0A
#define TB_KEY_CTRL_K         0x0B
#define TB_KEY_CTRL_L         0x0C
#define TB_KEY_ENTER          0x0D
#define TB_KEY_CTRL_N         0x0E
#define TB_KEY_CTRL_O         0x0F
#define TB_KEY_CTRL_P         0x10
#define TB_KEY_CTRL_Q         0x11
#define TB_KEY_CTRL_R         0x12
#define TB_KEY_CTRL_S         0x13
#define TB_KEY_CTRL_T         0x14
#define TB_KEY_CTRL_U         0x15
#define TB_KEY_CTRL_V         0x16
#define TB_KEY_CTRL_W         0x17
#define TB_KEY_CTRL_X         0x18
#define TB_KEY_CTRL_Y         0x19
#define TB_KEY_CTRL_Z         0x1A
#define TB_KEY_ESC            0x1B
#define TB_KEY_SPACE          0x20

/* Event types */
#define TB_EVENT_KEY    1
#define TB_EVENT_RESIZE 2
#define TB_EVENT_MOUSE  3

/* Error codes */
#define TB_OK                   0
#define TB_ERR                 -1
#define TB_ERR_NEED_MORE       -2
#define TB_ERR_INIT_ALREADY    -3
#define TB_ERR_MEM             -4
#define TB_ERR_NO_EVENT        -5
#define TB_ERR_NO_TERM         -6
#define TB_ERR_NOT_INIT        -7
#define TB_ERR_OUT_OF_BOUNDS   -8
#define TB_ERR_RESIZE_IOCTL    -10
#define TB_ERR_RESIZE_SSCANF   -11

/* Cell structure */
struct tb_cell {
    uint32_t ch;
    uint16_t fg;
    uint16_t bg;
};

/* Event structure */
struct tb_event {
    uint8_t type;
    uint8_t mod;
    uint16_t key;
    uint32_t ch;
    int32_t w;
    int32_t h;
    int32_t x;
    int32_t y;
};

#ifdef TB_IMPL

/* Internal state */
static struct termios tb_orig_tios;
static int tb_width = 0;
static int tb_height = 0;
static struct tb_cell *tb_buffer = NULL;
static int tb_initialized = 0;

/* UTF-8 encoding helper */
static int tb_utf8_char_to_unicode(uint32_t *out, const char *c) {
    if (*c == 0) return 0;

    unsigned char uc = (unsigned char)*c;
    if (uc < 0x80) {
        *out = uc;
        return 1;
    } else if ((uc & 0xE0) == 0xC0) {
        *out = (uc & 0x1F) << 6 | (c[1] & 0x3F);
        return 2;
    } else if ((uc & 0xF0) == 0xE0) {
        *out = (uc & 0x0F) << 12 | (c[1] & 0x3F) << 6 | (c[2] & 0x3F);
        return 3;
    } else if ((uc & 0xF8) == 0xF0) {
        *out = (uc & 0x07) << 18 | (c[1] & 0x3F) << 12 | (c[2] & 0x3F) << 6 | (c[3] & 0x3F);
        return 4;
    }
    *out = uc;
    return 1;
}

static int tb_utf8_unicode_to_char(char *out, uint32_t c) {
    if (c < 0x80) {
        out[0] = c;
        return 1;
    } else if (c < 0x800) {
        out[0] = 0xC0 | (c >> 6);
        out[1] = 0x80 | (c & 0x3F);
        return 2;
    } else if (c < 0x10000) {
        out[0] = 0xE0 | (c >> 12);
        out[1] = 0x80 | ((c >> 6) & 0x3F);
        out[2] = 0x80 | (c & 0x3F);
        return 3;
    } else {
        out[0] = 0xF0 | (c >> 18);
        out[1] = 0x80 | ((c >> 12) & 0x3F);
        out[2] = 0x80 | ((c >> 6) & 0x3F);
        out[3] = 0x80 | (c & 0x3F);
        return 4;
    }
}

/* Get terminal size */
static int tb_get_term_size(int *w, int *h) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
        return TB_ERR_RESIZE_IOCTL;
    *w = ws.ws_col;
    *h = ws.ws_row;
    return TB_OK;
}

/* Signal handler for resize */
static volatile sig_atomic_t tb_resize_pending = 0;

static void tb_sigwinch_handler(int sig) {
    (void)sig;
    tb_resize_pending = 1;
}

/* Initialize termbox */
int tb_init(void) {
    if (tb_initialized)
        return TB_ERR_INIT_ALREADY;

    setlocale(LC_ALL, "");

    /* Get terminal size */
    if (tb_get_term_size(&tb_width, &tb_height) != TB_OK)
        return TB_ERR_RESIZE_IOCTL;

    /* Allocate buffer */
    tb_buffer = calloc(tb_width * tb_height, sizeof(struct tb_cell));
    if (!tb_buffer)
        return TB_ERR_MEM;

    /* Save terminal state */
    tcgetattr(STDIN_FILENO, &tb_orig_tios);

    /* Set raw mode */
    struct termios raw = tb_orig_tios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    /* Setup signal handler */
    struct sigaction sa;
    sa.sa_handler = tb_sigwinch_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGWINCH, &sa, NULL);

    /* Enter alternate screen, hide cursor */
    printf("\x1b[?1049h\x1b[?25l");
    fflush(stdout);

    tb_initialized = 1;
    return TB_OK;
}

/* Shutdown termbox */
int tb_shutdown(void) {
    if (!tb_initialized)
        return TB_ERR_NOT_INIT;

    /* Show cursor, leave alternate screen */
    printf("\x1b[?25h\x1b[?1049l");
    fflush(stdout);

    /* Restore terminal */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tb_orig_tios);

    /* Free buffer */
    free(tb_buffer);
    tb_buffer = NULL;

    tb_initialized = 0;
    return TB_OK;
}

/* Get dimensions */
int tb_width_fn(void) { return tb_width; }
int tb_height_fn(void) { return tb_height; }

/* Clear screen buffer */
int tb_clear(void) {
    if (!tb_initialized) return TB_ERR_NOT_INIT;
    memset(tb_buffer, 0, tb_width * tb_height * sizeof(struct tb_cell));
    return TB_OK;
}

/* Set cell */
int tb_set_cell(int x, int y, uint32_t ch, uint16_t fg, uint16_t bg) {
    if (!tb_initialized) return TB_ERR_NOT_INIT;
    if (x < 0 || x >= tb_width || y < 0 || y >= tb_height)
        return TB_ERR_OUT_OF_BOUNDS;

    struct tb_cell *cell = &tb_buffer[y * tb_width + x];
    cell->ch = ch;
    cell->fg = fg;
    cell->bg = bg;
    return TB_OK;
}

/* Print string at position */
int tb_print(int x, int y, uint16_t fg, uint16_t bg, const char *str) {
    if (!tb_initialized) return TB_ERR_NOT_INIT;

    int ox = x;
    while (*str && x < tb_width) {
        uint32_t ch;
        int len = tb_utf8_char_to_unicode(&ch, str);
        tb_set_cell(x, y, ch, fg, bg);
        str += len;
        x++;
    }
    return x - ox;
}

/* Present buffer to screen */
int tb_present(void) {
    if (!tb_initialized) return TB_ERR_NOT_INIT;

    /* Move to home */
    printf("\x1b[H");

    uint16_t last_fg = TB_DEFAULT;
    uint16_t last_bg = TB_DEFAULT;

    for (int y = 0; y < tb_height; y++) {
        for (int x = 0; x < tb_width; x++) {
            struct tb_cell *cell = &tb_buffer[y * tb_width + x];

            /* Update colors if changed */
            if (cell->fg != last_fg || cell->bg != last_bg) {
                /* Reset */
                printf("\x1b[0m");

                /* Foreground */
                if (cell->fg & TB_BOLD) printf("\x1b[1m");
                if (cell->fg & TB_UNDERLINE) printf("\x1b[4m");
                if (cell->fg & TB_REVERSE) printf("\x1b[7m");

                int fgc = cell->fg & 0x00FF;
                int bgc = cell->bg & 0x00FF;

                if (fgc > 0 && fgc <= 8) printf("\x1b[%dm", 29 + fgc);
                if (bgc > 0 && bgc <= 8) printf("\x1b[%dm", 39 + bgc);

                last_fg = cell->fg;
                last_bg = cell->bg;
            }

            /* Print character */
            if (cell->ch == 0 || cell->ch == ' ') {
                putchar(' ');
            } else {
                char buf[5] = {0};
                tb_utf8_unicode_to_char(buf, cell->ch);
                printf("%s", buf);
            }
        }
        if (y < tb_height - 1) printf("\r\n");
    }

    printf("\x1b[0m");
    fflush(stdout);
    return TB_OK;
}

/* Poll for event (blocking) */
int tb_poll_event(struct tb_event *ev) {
    if (!tb_initialized) return TB_ERR_NOT_INIT;

    memset(ev, 0, sizeof(struct tb_event));

    /* Check for resize */
    if (tb_resize_pending) {
        tb_resize_pending = 0;
        int nw, nh;
        tb_get_term_size(&nw, &nh);
        if (nw != tb_width || nh != tb_height) {
            tb_width = nw;
            tb_height = nh;
            free(tb_buffer);
            tb_buffer = calloc(tb_width * tb_height, sizeof(struct tb_cell));
            ev->type = TB_EVENT_RESIZE;
            ev->w = tb_width;
            ev->h = tb_height;
            return TB_OK;
        }
    }

    /* Read input */
    char buf[16];
    int n = read(STDIN_FILENO, buf, sizeof(buf));

    if (n <= 0) {
        return TB_ERR_NO_EVENT;
    }

    ev->type = TB_EVENT_KEY;

    /* Parse escape sequences */
    if (buf[0] == '\x1b') {
        if (n == 1) {
            ev->key = TB_KEY_ESC;
        } else if (buf[1] == '[') {
            switch (buf[2]) {
                case 'A': ev->key = TB_KEY_ARROW_UP; break;
                case 'B': ev->key = TB_KEY_ARROW_DOWN; break;
                case 'C': ev->key = TB_KEY_ARROW_RIGHT; break;
                case 'D': ev->key = TB_KEY_ARROW_LEFT; break;
                case 'H': ev->key = TB_KEY_HOME; break;
                case 'F': ev->key = TB_KEY_END; break;
                case '3': ev->key = TB_KEY_DELETE; break;
                case '5': ev->key = TB_KEY_PGUP; break;
                case '6': ev->key = TB_KEY_PGDN; break;
                default: ev->key = 0; break;
            }
        }
    } else if (buf[0] < 32) {
        ev->key = buf[0];
    } else {
        tb_utf8_char_to_unicode(&ev->ch, buf);
    }

    return TB_OK;
}

/* Peek for event (non-blocking) */
int tb_peek_event(struct tb_event *ev, int timeout_ms) {
    (void)timeout_ms;
    return tb_poll_event(ev);
}

/* Macro-style function accessors */
#define tb_width() tb_width_fn()
#define tb_height() tb_height_fn()

#else /* !TB_IMPL */

/* Function declarations */
int tb_init(void);
int tb_shutdown(void);
int tb_width_fn(void);
int tb_height_fn(void);
int tb_clear(void);
int tb_set_cell(int x, int y, uint32_t ch, uint16_t fg, uint16_t bg);
int tb_print(int x, int y, uint16_t fg, uint16_t bg, const char *str);
int tb_present(void);
int tb_poll_event(struct tb_event *ev);
int tb_peek_event(struct tb_event *ev, int timeout_ms);

#define tb_width() tb_width_fn()
#define tb_height() tb_height_fn()

#endif /* TB_IMPL */

#endif /* TERMBOX2_H */
