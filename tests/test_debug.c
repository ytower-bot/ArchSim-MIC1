#include <stdio.h>
#include "control_unit.h"

int main() {
    control_memory cm;
    init_control_memory(&cm);
    load_microprogram(&cm, "data/basic_microcode.txt");
    
    mir m;
    for (int i = 0; i < 32; i++) {
        m.data[i] = cm.microinstructions[3][i];
    }
    
    printf("Instruction 3 bits: ");
    for (int i = 0; i < 32; i++) {
        printf("%d", m.data[i]);
        if ((i+1) % 4 == 0) printf(" ");
    }
    printf("\n");
    
    printf("Bit 0 (AMUX): %d\n", m.data[0]);
    printf("Bits 1-2 (COND): %d%d\n", m.data[1], m.data[2]);
    printf("Bits 3-4 (ALU): %d%d\n", m.data[3], m.data[4]);
    printf("Bits 5-6 (SH): %d%d\n", m.data[5], m.data[6]);
    printf("Bit 7 (MBR): %d\n", m.data[7]);
    printf("Bit 8 (MAR): %d\n", m.data[8]);
    printf("Bit 9 (RD): %d\n", m.data[9]);
    printf("Bit 10 (WR): %d\n", m.data[10]);
    printf("Bit 11 (ENC): %d\n", m.data[11]);
    printf("Bits 12-15 (C): %d%d%d%d\n", m.data[12], m.data[13], m.data[14], m.data[15]);
    printf("Bits 16-19 (B): %d%d%d%d\n", m.data[16], m.data[17], m.data[18], m.data[19]);
    printf("Bits 20-23 (A): %d%d%d%d\n", m.data[20], m.data[21], m.data[22], m.data[23]);
    
    decode_microinstruction(&m);
    
    printf("\nAfter decode:\n");
    printf("AMUX: %d\n", m.amux);
    printf("ALU:  %d (bits: %d%d)\n", bits_to_int(m.alu, 2), m.alu[0], m.alu[1]);
    printf("C:    %d (bits: %d%d%d%d)\n", bits_to_int(m.c, 4), m.c[0], m.c[1], m.c[2], m.c[3]);
    printf("B:    %d (bits: %d%d%d%d)\n", bits_to_int(m.b, 4), m.b[0], m.b[1], m.b[2], m.b[3]);
    printf("A:    %d (bits: %d%d%d%d)\n", bits_to_int(m.a, 4), m.a[0], m.a[1], m.a[2], m.a[3]);
    
    return 0;
}
