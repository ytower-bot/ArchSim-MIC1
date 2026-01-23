
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/control_unit.h"
#include "../include/datapath.h"
#include "../include/memory.h"
#include "../include/alu.h"
#include "../include/shifter.h"
#include "../include/utils/conversions.h"

void init_mir(mir* m) {
    if (!m) {
        return;
    }

    for (int i = 0; i < 32; i++) {
        m->data[i] = 0;
    }

    m->amux = 0;
    m->mbr = 0;
    m->mar = 0;
    m->rd = 0;
    m->wr = 0;
    m->enc = 0;

    m->cond[0] = 0;
    m->cond[1] = 0;
    m->alu[0] = 0;
    m->alu[1] = 0;
    m->sh[0] = 0;
    m->sh[1] = 0;

    for (int i = 0; i < 4; i++) {
        m->c[i] = 0;
        m->b[i] = 0;
        m->a[i] = 0;
    }

    for (int i = 0; i < 8; i++) {
        m->addr[i] = 0;
    }
}

void decode_microinstruction(mir* m) {
    if (!m) {
        return;
    }

    /*
     * Microinstruction format (32 bits, stored as string from MSB to LSB):
     * data[0..7]   = ADDR[7:0]  (bits 31-24: next address)
     * data[8..11]  = A[3:0]     (bits 23-20: source A register)
     * data[12..15] = B[3:0]     (bits 19-16: source B register)
     * data[16..19] = C[3:0]     (bits 15-12: destination register)
     * data[20]     = ENC        (bit 11: enable write)
     * data[21]     = WR         (bit 10: memory write)
     * data[22]     = RD         (bit 9: memory read)
     * data[23]     = MAR        (bit 8: load MAR)
     * data[24]     = MBR        (bit 7: load MBR)
     * data[25..26] = SH[1:0]    (bits 6-5: shifter control)
     * data[27..28] = ALU[1:0]   (bits 4-3: ALU operation)
     * data[29..30] = COND[1:0]  (bits 2-1: branch condition)
     * data[31]     = AMUX       (bit 0: A-mux select)
     */

    m->amux = m->data[31];
    m->mbr = m->data[24];
    m->mar = m->data[23];
    m->rd = m->data[22];
    m->wr = m->data[21];
    m->enc = m->data[20];

    m->cond[0] = m->data[30];
    m->cond[1] = m->data[29];

    m->alu[0] = m->data[28];
    m->alu[1] = m->data[27];

    m->sh[0] = m->data[26];
    m->sh[1] = m->data[25];

    /*
     * Register fields are 4 bits each, stored MSB first in the string.
     * A[3:0] = data[8..11], B[3:0] = data[12..15], C[3:0] = data[16..19]
     * We need c[0]=MSB, c[3]=LSB to match control_to_index() which expects
     * control[0]=LSB, control[3]=MSB.
     *
     * The string stores: data[16]=C[3], data[17]=C[2], data[18]=C[1], data[19]=C[0]
     * We need: c[0]=C[0]=data[19], c[1]=C[1]=data[18], c[2]=C[2]=data[17], c[3]=C[3]=data[16]
     */
    for (int i = 0; i < 4; i++) {
        m->c[i] = m->data[19 - i];  /* C: data[16..19] -> c[3..0] */
        m->b[i] = m->data[15 - i];  /* B: data[12..15] -> b[3..0] */
        m->a[i] = m->data[11 - i];  /* A: data[8..11]  -> a[3..0] */
    }

    /*
     * ADDR field: bits_to_int expects MSB first (index 0 = MSB).
     * The string stores data[0..7] as ADDR[7..0] (MSB first), so we
     * copy directly since bits_to_int will interpret addr[0] as MSB.
     */
    for (int i = 0; i < 8; i++) {
        m->addr[i] = m->data[i];  /* ADDR: data[0..7] -> addr[0..7] directly */
    }
}

void run_mir(mir* m, mbr* mb, mar* ma, mmux* mmu, amux* amu,
             shifter* s, alu* al, decoder* da, decoder* db, decoderC* dc) {

    if (!m) {
        return;
    }

    decode_microinstruction(m);

    if (amu) {
        amu->control_amux = m->amux;
    }

    if (mmu) {
        mmu->control_cond[0] = m->cond[0];
        mmu->control_cond[1] = m->cond[1];
    }

    if (al) {
        al->control[0] = m->alu[0];
        al->control[1] = m->alu[1];
    }

    if (s) {
        s->control_sh[0] = m->sh[0];
        s->control_sh[1] = m->sh[1];
    }

    if (mb) {
        mb->control_mbr = m->mbr;
        mb->control_rd = m->rd;
        mb->control_wr = m->wr;
    }

    if (ma) {
        ma->control_mar = m->mar;
    }

    if (da) {
        for (int i = 0; i < 4; i++) {
            da->control[i] = m->a[i];
        }
    }

    if (db) {
        for (int i = 0; i < 4; i++) {
            db->control[i] = m->b[i];
        }
    }

    if (dc) {
        dc->control_enc = m->enc;
        for (int i = 0; i < 4; i++) {
            dc->control_c[i] = m->c[i];
        }
    }

}

void init_mpc(mpc* p) {
    if (!p) {
        return;
    }

    for (int i = 0; i < 8; i++) {
        p->address[i] = 0;
    }
}

void increment_mpc(mpc* p) {
    if (!p) {
        return;
    }

    int addr = bits_to_int(p->address, 8);

    addr = (addr + 1) & 0xFF;

    int_to_bits(addr, p->address, 8);
}

void run_mpc(mpc* p, mir* m, control_memory* cm) {

    if (!p || !m || !cm) {
        return;
    }

    int index = bits_to_int(p->address, 8);

    if (index < 0 || index >= MICROPROGRAM_SIZE) {
        for (int i = 0; i < 32; i++) {
            m->data[i] = 0;
        }
        return;
    }

    for (int i = 0; i < 32; i++) {
        m->data[i] = cm->microinstructions[index][i];
    }
}

void init_mmux(mmux* m) {
    if (!m) {
        return;
    }

    m->control_cond[0] = 0;
    m->control_cond[1] = 0;

    m->alu_n = 0;
    m->alu_z = 0;
}

int should_branch(mmux* m) {
    if (!m) {
        return 0;
    }

    int cond = (m->control_cond[1] << 1) | m->control_cond[0];

    int result;
    switch (cond) {
        case COND_NONE:
            result = 0;
            break;
        case COND_IF_N:
            result = m->alu_n;
            break;
        case COND_IF_Z:
            result = m->alu_z;
            break;
        case COND_ALWAYS:
            result = 1;
            break;
        default:
            result = 0;
            break;
    }

    return result;
}

void run_mmux(mmux* m, mpc* p, mir* mir, mbr* mb) {

    if (!m || !p || !mir) {
        return;
    }

        if (should_branch(m)) {
            int addr_value = bits_to_int(mir->addr, 8);
            int cond = (m->control_cond[1] << 1) | m->control_cond[0];

            if (cond == COND_ALWAYS && addr_value == 0xFF && mb) {
                int opcode = 0;
                for (int i = 0; i < 4; i++) {
                    opcode |= (mb->data[12 + i] << (3 - i));
                }

                int base = 0x14;
                int target = base + (opcode << 2);

                if (opcode >= 0x4) target += 4;
                if (opcode >= 0x9) target += 4;
                if (opcode >= 0xA) target += 4;
                if (opcode >= 0xB) target += 4;
                if (opcode >= 0xC) target += 4;
                if (opcode >= 0xF) target += 4;

                int_to_bits(target, p->address, 8);
            } else {
                for (int i = 0; i < 8; i++) {
                    p->address[i] = mir->addr[i];
                }
            }
        } else {
            increment_mpc(p);
        }
}

void init_amux(amux* a) {
    if (!a) {
        return;
    }
    a->control_amux = 0;
}

void run_amux(amux* a, mbr* mb, latch* lA, alu* u) {
    if (!a || !mb || !lA || !u) {
        return;
    }

    if (a->control_amux == 0) {
        copy_data(u->input_a, lA->data);
    } else {
        copy_data(u->input_a, mb->data);
    }
}

void init_control_memory(control_memory* cm) {
    if (!cm) {
        return;
    }

    for (int i = 0; i < MICROPROGRAM_SIZE; i++) {
        for (int j = 0; j < 32; j++) {
            cm->microinstructions[i][j] = 0;
        }
    }
}

int load_microprogram(control_memory* cm, const char* filename) {
    if (!cm || !filename) {
        return -1;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open microprogram file: %s\n", filename);
        return -1;
    }

    char line[128];
    int instruction_count = 0;

    while (fgets(line, sizeof(line), file) && instruction_count < MICROPROGRAM_SIZE) {
        if (line[0] == '#' || line[0] == ';' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        int valid_bits = 0;
        for (int i = 0; i < 32 && line[i] != '\0' && line[i] != '\n'; i++) {
            if (line[i] == '0') {
                cm->microinstructions[instruction_count][i] = 0;
                valid_bits++;
            } else if (line[i] == '1') {
                cm->microinstructions[instruction_count][i] = 1;
                valid_bits++;
            } else if (line[i] != ' ' && line[i] != '\t') {
                fprintf(stderr, "Warning: Invalid character '%c' at instruction %d, bit %d\n",
                        line[i], instruction_count, i);
            }
        }

        if (valid_bits == 32) {
            instruction_count++;
        } else if (valid_bits > 0) {
            fprintf(stderr, "Warning: Incomplete microinstruction at line %d (got %d bits, expected 32)\n",
                    instruction_count + 1, valid_bits);
        }
    }

    fclose(file);

    if (instruction_count == 0) {
        fprintf(stderr, "Error: No valid microinstructions loaded from %s\n", filename);
        return -1;
    }

    return instruction_count;
}

void fetch_microinstruction(control_memory* cm, mpc* p, mir* m) {
    if (!cm || !p || !m) {
        return;
    }

    int address = bits_to_int(p->address, 8);

    if (address < 0 || address >= MICROPROGRAM_SIZE) {
        fprintf(stderr, "Error: MPC address out of bounds: %d\n", address);
        return;
    }

    for (int i = 0; i < 32; i++) {
        m->data[i] = cm->microinstructions[address][i];
    }

    decode_microinstruction(m);
}

void update_control(mpc* p, mmux* mmux, mir* m, mbr* mb) {
    if (!p || !mmux || !m) {
        return;
    }

    run_mmux(mmux, p, m, mb);
}
