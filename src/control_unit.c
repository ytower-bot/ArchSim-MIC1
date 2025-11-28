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
