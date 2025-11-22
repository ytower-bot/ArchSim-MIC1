#ifndef SHIFTER_H
#define SHIFTER_H

// Shifter structure
typedef struct shifter {
    int control_sh[2];
    int data[16];
} shifter;

#define SHIFT_NONE      0b00
#define SHIFT_LEFT      0b01
#define SHIFT_RIGHT     0b10
#define SHIFT_UNUSED    0b11

void set_shifter_input(shifter* s, int input[16]);
void set_shifter_control(shifter* s, int control[2]);
void lshift(shifter* s);
void rshift(shifter* s);
void init_shifter(shifter* s);
int get_shifter_control_value(int control[2]);

#endif