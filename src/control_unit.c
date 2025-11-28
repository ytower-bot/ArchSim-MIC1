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
#include "../include/control_unit.h"
#include "../include/datapath.h"
#include "../include/memory.h"
#include "../include/alu.h"
#include "../include/shifter.h"
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
    if (!m) {
        return;
    }

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
    m->amux = m->data[0];
    m->mbr = m->data[7];
    m->mar = m->data[8];
    m->rd = m->data[9];
    m->wr = m->data[10];
    m->enc = m->data[11];

    // Extract 2-bit fields
    m->cond[0] = m->data[1];
    m->cond[1] = m->data[2];
    
    m->alu[0] = m->data[3];
    m->alu[1] = m->data[4];
    
    m->sh[0] = m->data[5];
    m->sh[1] = m->data[6];

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
 * @brief Execute MIR operation - Distribute control signals to all MIC-1 components
 *
 * @param m   Pointer to MIR structure
 * @param mb  Pointer to MBR structure
 * @param ma  Pointer to MAR structure
 * @param mmu Pointer to MMUX structure
 * @param amu Pointer to AMUX structure
 * @param s   Pointer to Shifter structure
 * @param al  Pointer to ALU structure
 * @param da  Pointer to Decoder A structure
 * @param db  Pointer to Decoder B structure
 * @param dc  Pointer to Decoder C structure
 *
 * This function:
 * 1. Decodes the microinstruction stored in MIR
 * 2. Distributes control signals to all components of the MIC-1 datapath
 * 3. Each component receives only the signals it needs to operate
 *
 * Signal distribution:
 * - AMUX: Selects between Latch A and MBR for ALU input
 * - COND: Condition code for MMUX (branch control)
 * - ALU: Operation code for ALU (ADD, AND, A, NOT A)
 * - SH: Shifter operation (none, left, right)
 * - MBR: Control signal to load MBR from shifter
 * - MAR: Control signal to load MAR from Latch B
 * - RD: Control signal to read from memory
 * - WR: Control signal to write to memory
 * - ENC: Enable write for Decoder C
 * - C: Destination register selector
 * - B: Source register B selector
 * - A: Source register A selector
 * - ADDR: Used by MMUX for branch target address
 */
void run_mir(mir* m, mbr* mb, mar* ma, mmux* mmu, amux* amu, 
             shifter* s, alu* al, decoder* da, decoder* db, decoderC* dc) {
    
    if (!m) {
        return;
    }

    // Step 1: Decode the microinstruction into individual fields
    decode_microinstruction(m);

    // Step 2: Distribute control signals to components

    // AMUX control - Selects ALU input A source (Latch A or MBR)
    if (amu) {
        amu->control_amux = m->amux;
    }

    // MMUX control - Branch condition
    if (mmu) {
        mmu->control_cond[0] = m->cond[0];
        mmu->control_cond[1] = m->cond[1];
    }

    // ALU control - Operation selection
    if (al) {
        al->control[0] = m->alu[0];
        al->control[1] = m->alu[1];
    }

    // Shifter control - Shift operation
    if (s) {
        s->control_sh[0] = m->sh[0];
        s->control_sh[1] = m->sh[1];
    }

    // MBR control signals
    if (mb) {
        mb->control_mbr = m->mbr;  // Load MBR from shifter
        mb->control_rd = m->rd;    // Read from memory to MBR
        mb->control_wr = m->wr;    // Write from MBR to memory
    }

    // MAR control - Load address from Latch B
    if (ma) {
        ma->control_mar = m->mar;
    }

    // Decoder A control - Source register A selection
    if (da) {
        for (int i = 0; i < 4; i++) {
            da->control[i] = m->a[i];
        }
    }

    // Decoder B control - Source register B selection
    if (db) {
        for (int i = 0; i < 4; i++) {
            db->control[i] = m->b[i];
        }
    }

    // Decoder C control - Destination register selection and write enable
    if (dc) {
        dc->control_enc = m->enc;  // Enable write
        for (int i = 0; i < 4; i++) {
            dc->control_c[i] = m->c[i];
        }
    }

    // Note: The ADDR field (m->addr) is used by MMUX to determine
    // the next microinstruction address, but is accessed directly
    // from the MIR structure by run_mmux(), not copied here.
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
    if (!p) {
        return;
    }

    // Convert address[8] to integer (0-255)
    int addr = bits_to_int(p->address, 8);

    // Increment with overflow handling
    addr = (addr + 1) & 0xFF;  // Mask to 8 bits (0-255)

    // Convert back to 8-bit array
    int_to_bits(addr, p->address, 8);
}

/**
 * @brief Fetch microinstruction from control memory and load into MIR
 *
 * @param p  Pointer to MPC structure (contains current microinstruction address)
 * @param m  Pointer to MIR structure (destination for microinstruction)
 * @param cm Pointer to control_memory structure (source of microinstructions)
 *
 * This function implements the microinstruction fetch cycle:
 * 1. Read the current MPC address
 * 2. Fetch the 32-bit microinstruction from control memory at that address
 * 3. Load the microinstruction into MIR
 *
 * Control memory organization:
 * - cm->microinstructions[0..78][0..31]: 79 microinstructions × 32 bits
 * - MPC address range: 0-78 (valid), 79-255 (undefined behavior)
 *
 * Example:
 *   MPC = 5 -> Fetch microinstructions[5] -> Load into MIR.data[32]
 */
void run_mpc(mpc* p, mir* m, control_memory* cm) {
    // Validate pointers
    if (!p || !m || !cm) {
        return;
    }

    // Convert MPC address[8] to integer index (0-255)
    int index = bits_to_int(p->address, 8);

    // Bounds check: valid microprogram addresses are 0-78
    if (index < 0 || index >= MICROPROGRAM_SIZE) {
        // Invalid address: do nothing or set MIR to NOP (all zeros)
        for (int i = 0; i < 32; i++) {
            m->data[i] = 0;
        }
        return;
    }

    // Fetch microinstruction from control memory and load into MIR
    for (int i = 0; i < 32; i++) {
        m->data[i] = cm->microinstructions[index][i];
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
    if (!m) {
        return 0;
    }

    // Convert control_cond[2] to integer (0-3)
    int cond = bits_to_int(m->control_cond, 2);

    // Evaluate branch condition
    switch (cond) {
        case COND_NONE:     // 0b00 - Never branch
            return 0;
        
        case COND_IF_N:     // 0b01 - Branch if N=1
            return m->alu_n;
        
        case COND_IF_Z:     // 0b10 - Branch if Z=1
            return m->alu_z;
        
        case COND_ALWAYS:   // 0b11 - Always branch
            return 1;
        
        default:
            return 0;
    }
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
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == ';' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        // Parse 32-bit microinstruction
        int valid_bits = 0;
        for (int i = 0; i < 32 && line[i] != '\0' && line[i] != '\n'; i++) {
            if (line[i] == '0') {
                cm->microinstructions[instruction_count][i] = 0;
                valid_bits++;
            } else if (line[i] == '1') {
                cm->microinstructions[instruction_count][i] = 1;
                valid_bits++;
            } else if (line[i] != ' ' && line[i] != '\t') {
                // Invalid character
                fprintf(stderr, "Warning: Invalid character '%c' at instruction %d, bit %d\n", 
                        line[i], instruction_count, i);
            }
        }

        // Validate we got exactly 32 bits
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

