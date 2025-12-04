/**
 * @file control_unit.c
 * @brief Control Unit implementation for MIC-1 simulator - FASE 4
 * 
 * Implements the control unit components:
 * - MIR (Microinstruction Register): Decodes and distributes control signals
 * - Helper functions for MIR initialization and microinstruction decoding
 * 
 * @date 28/11/2024
 * @phase FASE_4
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/control_unit.h"
#include "../include/utils/conversions.h"

/**
 * @brief Initialize Microinstruction Register (MIR) with zeros
 *
 * @param m Pointer to the MIR structure
 *
 * Initializes all fields of the MIR structure to zero:
 * - data[32]: 32-bit microinstruction word
 * - amux: AMUX control signal
 * - cond[2]: Condition control signals
 * - alu[2]: ALU control signals
 * - sh[2]: Shifter control signals
 * - mbr, mar, rd, wr, enc: Control signals
 * - c[4], b[4], a[4]: Register selection fields
 * - addr[8]: Next microinstruction address
 */
void init_mir(mir* m) {
    if (!m) return;


    // Initialize the 32-bit microinstruction data array
    for (int i = 0; i < 32; i++) {
        m->data[i] = 0;
    }

    // Initialize single-bit control signals
    m->amux = 0;
    m->mbr = 0;
    m->mar = 0;
    m->rd = 0;
    m->wr = 0;
    m->enc = 0;

    // Initialize 2-bit control fields
    m->cond[0] = 0;
    m->cond[1] = 0;
    m->alu[0] = 0;
    m->alu[1] = 0;
    m->sh[0] = 0;
    m->sh[1] = 0;

    // Initialize 4-bit register selection fields
    for (int i = 0; i < 4; i++) {
        m->c[i] = 0;
        m->b[i] = 0;
        m->a[i] = 0;
    }

    // Initialize 8-bit address field
    for (int i = 0; i < 8; i++) {
        m->addr[i] = 0;
    }
}

/**
 * @brief Initialize Microprogram Counter (MPC) with zeros
 *
 * @param p Pointer to the MPC structure
 *
 * Initializes the MPC address field to zero (address = 0).
 * This sets the starting point of microprogram execution.
 */
void init_mpc(mpc* p) {
    if (!p) {
        return;
    }

    // Initialize 8-bit address field to zero
    for (int i = 0; i < 8; i++) {
        p->address[i] = 0;
    }
}

/**
 * @brief Initialize Microaddress Multiplexer (MMUX) with zeros
 *
 * @param m Pointer to the MMUX structure
 *
 * Initializes all fields of the MMUX structure to zero:
 * - control_cond[2]: Condition control signals (COND bits)
 * - alu_n: ALU negative flag
 * - alu_z: ALU zero flag
 */
void init_mmux(mmux* m) {
    if (!m) {
        return;
    }

    // Initialize condition control bits
    m->control_cond[0] = 0;
    m->control_cond[1] = 0;

    // Initialize ALU flags
    m->alu_n = 0;
    m->alu_z = 0;
}

/**
 * @brief Initialize AMUX structure
 *
 * @param a Pointer to AMUX structure
 *
 * Initializes the AMUX control signal to 0 (selects Latch A as default).
 */
void init_amux(amux* a) {
    if (!a) {
        return;
    }
    a->control_amux = 0;
}

/**
 * @brief Initialize control memory structure
 *
 * @param cm Pointer to control_memory structure
 *
 * Initializes all microinstructions in the control memory to zero.
 * The control memory stores 79 microinstructions, each 32 bits wide.
 */
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

/**
 * @brief Decode microinstruction from 32-bit data into individual fields
 *
 * @param m Pointer to the MIR structure
 *
 * Extracts control signals from the 32-bit microinstruction word (m->data)
 * and stores them in the individual control fields.
 * 
 * Microinstruction format (32 bits):
 * Bit [0]:      AMUX  - Selects ALU input (Latch A or MBR)
 * Bits [1-2]:   COND  - Branch condition
 * Bits [3-4]:   ALU   - ALU operation
 * Bits [5-6]:   SH    - Shifter operation
 * Bit [7]:      MBR   - Load MBR from shifter
 * Bit [8]:      MAR   - Load MAR from Latch B
 * Bit [9]:      RD    - Read from memory
 * Bit [10]:     WR    - Write to memory
 * Bit [11]:     ENC   - Enable register write (Decoder C)
 * Bits [12-15]: C     - Destination register selector (Decoder C)
 * Bits [16-19]: B     - Source register B selector (Decoder B)
 * Bits [20-23]: A     - Source register A selector (Decoder A)
 * Bits [24-31]: ADDR  - Next microinstruction address
 */
void decode_microinstruction(mir* m) {
    if (!m) {
        return;
    }

    // Extract single-bit fields
    m->amux = m->data[0]; // Bit 0

    // Extract 2-bit fields
    m->cond[0] = m->data[1]; // Bits 1-2
    m->cond[1] = m->data[2];

    m->alu[0] = m->data[3]; // Bits 3-4
    m->alu[1] = m->data[4];

    m->sh[0] = m->data[5]; //Bits 5-6
    m->sh[1] = m->data[6];

    m->mbr = m->data[7];
    m->mar = m->data[8];
    m->rd = m->data[9];
    m->wr = m->data[10];
    m->enc = m->data[11];
    


    // Extract 4-bit register selection fields
    for (int i = 0; i < 4; i++) {
        m->c[i] = m->data[12 + i];  // Bits 12-15
        m->b[i] = m->data[16 + i];  // Bits 16-19
        m->a[i] = m->data[20 + i];  // Bits 20-23
    }

    // Extract 8-bit address field
    for (int i = 0; i < 8; i++) {
        m->addr[i] = m->data[24 + i];  // Bits 24-31
    }
}

/**
 * @brief Evaluate if a branch should be taken
 *
 * @param m Pointer to the MMUX structure
 *
 * @return 1 if branch should be taken, 0 otherwise
 *
 * Evaluates the branch condition based on control_cond[2] and ALU flags:
 * - COND_NONE (0b00): Never branch -> return 0
 * - COND_IF_N (0b01): Branch if N=1 -> return alu_n
 * - COND_IF_Z (0b10): Branch if Z=1 -> return alu_z
 * - COND_ALWAYS (0b11): Always branch -> return 1
 *
 * Example:
 *   m->control_cond = [0,1] (COND_IF_N), m->alu_n = 1 -> returns 1
 *   m->control_cond = [1,0] (COND_IF_Z), m->alu_z = 0 -> returns 0
 */
int should_branch(mmux* m) {
    if (!m) return 0;

    // COND field: bit 0 é High (L), bit 1 é Low (R)
    int L = m->control_cond[0]; 
    int R = m->control_cond[1];
    
    // Flags da ALU
    int N = m->alu_n;
    int Z = m->alu_z;

    // Lógica Booleana conforme Tanenbaum (descrição textual da lógica do MMUX):
    // Mmux = (L' . R . N) + (L . R' . Z) + (L . R)
    // Onde: . é AND, + é OR, ' é NOT
    
    int cond_n = (!L && R && N);      // 01 e N=1 (JNEG)
    int cond_z = (L && !R && Z);      // 10 e Z=1 (JZER)
    int cond_jump = (L && R);         // 11 (JUMP incondicional)

    return (cond_n || cond_z || cond_jump);
}

/**
 * @brief Increment Microprogram Counter by 1
 *
 * @param p Pointer to the MPC structure
 *
 * Increments the MPC address by 1 (MPC = MPC + 1).
 * Handles overflow: if address reaches 256, wraps back to 0.
 *
 * Implementation:
 * 1. Convert address[8] bits to integer (0-255)
 * 2. Increment the value
 * 3. Handle overflow (256 -> 0)
 * 4. Convert back to 8-bit array
 *
 * Example:
 *   address = [0,0,0,0,0,1,0,1] (5 decimal)
 *   After increment: [0,0,0,0,0,1,1,0] (6 decimal)
 */
void increment_mpc(mpc* p) {
    // Convert address[8] to integer (0-255)
    int addr = bits_to_int(p->address, 8);

    // Increment with overflow handling
    addr = (addr + 1) & 0xFF;  // Mask to 8 bits (0-255)

    // Convert back to 8-bit array
    int_to_bits(addr, p->address, 8);
}

/**
 * @brief Execute MMUX operation - Select next microinstruction address
 *
 * @param m   Pointer to MMUX structure
 * @param p   Pointer to MPC structure (updated with next address)
 * @param mir Pointer to MIR structure (contains ADDR field)
 *
 * This function decides the next microinstruction address based on branch condition:
 * 
 * If should_branch() returns 1 (branch taken):
 *   - Load mir->addr[8] into mpc->address[8]
 *   - MPC = ADDR from MIR (jump to branch target)
 * 
 * If should_branch() returns 0 (no branch):
 *   - Call increment_mpc() to add 1 to MPC
 *   - MPC = MPC + 1 (sequential execution)
 *
 * Example:
 *   // Branch taken
 *   mir->addr = [0,0,1,0,1,0,0,0] (address 40)
 *   should_branch() = 1
 *   -> mpc->address = [0,0,1,0,1,0,0,0] (jump to 40)
 *
 *   // No branch
 *   mpc->address = [0,0,0,0,0,1,0,1] (address 5)
 *   should_branch() = 0
 *   -> mpc->address = [0,0,0,0,0,1,1,0] (increment to 6)
 */
void run_mmux(mmux* m, mpc* p, mir* mir) {
    // Validate pointers
    if (!m || !p || !mir) {
        return;
    }

    // Decide if branch should be taken
    if (should_branch(m)) {
        // Branch: Load ADDR from MIR into MPC        
        for (int i = 0; i < 8; i++) {
            p->address[i] = mir->addr[i];
        }
    } else {
        // No branch: Increment MPC (sequential execution)
        increment_mpc(p);
    }
}

/**
 * @brief Execute AMUX operation - Select ALU input A source
 *
 * @param a  Pointer to AMUX structure
 * @param mb Pointer to MBR structure
 * @param lA Pointer to Latch A structure
 * @param u  Pointer to ALU structure
 *
 * This function selects the source for ALU input A based on control_amux:
 * - control_amux = 0: Copy Latch A data to ALU input A
 * - control_amux = 1: Copy MBR data to ALU input A
 *
 * The selected data is copied to u->input_a[16].
 */
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

/**
 * @brief Fetch microinstruction from control memory into MIR
 *
 * @param cm Pointer to control_memory structure
 * @param mpc Pointer to microprogram counter
 * @param m Pointer to MIR structure
 *
 * Reads the microinstruction at the address specified by MPC from control memory
 * and loads it into the MIR. Then decodes the microinstruction fields.
 */
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

/**
 * @brief Update MPC based on MMUX branch decision
 *
 * @param p Pointer to microprogram counter
 * @param mmux Pointer to MMUX structure
 * @param m Pointer to MIR structure
 *
 * If MMUX indicates a branch should be taken, loads MIR.ADDR into MPC.
 * Otherwise, increments MPC by 1.
 */
void update_control(mpc* p, mmux* mmux, mir* m) {
    if (!p || !mmux || !m) {
        return;
    }

    // run_mmux already handles both branching and incrementing
    run_mmux(mmux, p, m);
}

/**
 * @brief Load microprograma from file into control memory
 *
 * @param cm Pointer to control_memory structure
 * @param filename Path to microprograma file
 *
 * Loads microinstructions from a binary file into control memory.
 * File format: Each line contains 32 binary digits (0 or 1) representing one microinstruction.
 * Lines starting with '#' or ';' are comments and ignored.
 * Empty lines are ignored.
 *
 * Format example:
 * # NOP instruction
 * 00000000000000000000000000000000
 * # MAR <- PC
 * 00000000000100000000000000000001
 *
 * Returns: Number of microinstructions loaded, or -1 on error.
 */
int load_microprogram(control_memory* cm, const char* filename) {
    if (!cm || !filename) return -1;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir microprograma: %s\n", filename);
        return -1;
    }

    char line[256];
    int addr = 0;

    while (fgets(line, sizeof(line), file) && addr < MICROPROGRAM_SIZE) {
        // Pula linhas vazias ou comentários puros
        if (line[0] == '#' || line[0] == ';' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        int bit_count = 0;
        int i = 0;
        
        // Loop robusto: procura 32 bits válidos na linha, ignorando espaços
        while (line[i] != '\0' && line[i] != '\n' && line[i] != ';' && bit_count < 32) {
            if (line[i] == '0') {
                cm->microinstructions[addr][bit_count++] = 0;
            } else if (line[i] == '1') {
                cm->microinstructions[addr][bit_count++] = 1;
            }
            i++;
        }

        // Só avança o endereço se leu uma linha completa válida (32 bits)
        if (bit_count == 32) {
            addr++;
        }
    }

    fclose(file);
    printf("Microprograma carregado: %d instrucoes.\n", addr);
    return addr;
}