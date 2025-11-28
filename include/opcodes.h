/**
 * @file opcodes.h
 * @brief MIC-1 Instruction Set Opcodes
 * 
 * Defines the macro instruction opcodes for the MIC-1 architecture.
 * Each opcode is 8 bits (stored in upper byte of 16-bit instruction word).
 * Format: [OPCODE(8 bits)][OPERAND(8 bits)]
 */

#ifndef OPCODES_H
#define OPCODES_H

// ============================================================================
// MIC-1 INSTRUCTION SET - 24 OPCODES
// ============================================================================

// --- Memory Operations (Direct Addressing) ---
#define OP_LODD  0x00  // Load Direct:     AC ← Memory[addr]
#define OP_STOD  0x01  // Store Direct:    Memory[addr] ← AC
#define OP_ADDD  0x02  // Add Direct:      AC ← AC + Memory[addr]
#define OP_SUBD  0x03  // Subtract Direct: AC ← AC - Memory[addr]

// --- Memory Operations (Local Addressing) ---
#define OP_LODL  0x04  // Load Local:      AC ← Memory[SP + offset]
#define OP_STOL  0x05  // Store Local:     Memory[SP + offset] ← AC
#define OP_ADDL  0x06  // Add Local:       AC ← AC + Memory[SP + offset]
#define OP_SUBL  0x07  // Subtract Local:  AC ← AC - Memory[SP + offset]

// --- Immediate Operations ---
#define OP_LOCO  0x08  // Load Constant:   AC ← operand (8-bit signed)

// --- Control Flow (Conditional Jumps) ---
#define OP_JPOS  0x09  // Jump if Positive:    if (AC >= 0) PC ← addr
#define OP_JZER  0x0A  // Jump if Zero:        if (AC == 0) PC ← addr
#define OP_JNEG  0x0B  // Jump if Negative:    if (AC < 0) PC ← addr
#define OP_JNZE  0x0C  // Jump if Not Zero:    if (AC != 0) PC ← addr

// --- Control Flow (Unconditional) ---
#define OP_JUMP  0x0D  // Unconditional Jump:  PC ← addr
#define OP_CALL  0x0E  // Call Procedure:      SP←SP-1, Memory[SP]←PC, PC←addr
#define OP_RETN  0x0F  // Return:              PC ← Memory[SP], SP←SP+1

// --- Stack Operations ---
#define OP_PSHI  0x10  // Push Immediate:  SP←SP-1, Memory[SP]←operand
#define OP_PUSH  0x11  // Push AC:         SP←SP-1, Memory[SP]←AC
#define OP_POP   0x12  // Pop:             SP←SP+1
#define OP_POPA  0x13  // Pop to AC:       AC←Memory[SP], SP←SP+1

// --- Stack Manipulation ---
#define OP_SWAP  0x14  // Swap Top Two:    temp←Memory[SP], Memory[SP]←Memory[SP+1], Memory[SP+1]←temp
#define OP_INSP  0x15  // Increment SP:    SP←SP+1
#define OP_DESP  0x16  // Decrement SP:    SP←SP-1

// --- System ---
#define OP_HALT  0x17  // Halt Execution:  Stop CPU

// --- Reserved for Future Use ---
#define OP_NOP   0xFF  // No Operation (not part of standard ISA)

// ============================================================================
// MICROCODE ADDRESS MAP
// ============================================================================

/**
 * Microprograma Organization:
 * 
 * Address Range | Purpose
 * --------------|--------------------------------------------------------
 * 0x00 - 0x03   | Common Fetch Routine (fetches next instruction)
 * 0x04 - 0x0F   | Decode and Dispatch (jumps to instruction microcode)
 * 0x10 - 0x1F   | LODD microcode
 * 0x20 - 0x2F   | STOD microcode
 * 0x30 - 0x3F   | ADDD microcode
 * 0x40 - 0x4F   | SUBD microcode
 * 0x50 - 0x5F   | LODL microcode
 * 0x60 - 0x6F   | STOL microcode
 * 0x70 - 0x7F   | ADDL microcode
 * 0x80 - 0x8F   | SUBL microcode
 * 0x90 - 0x9F   | LOCO microcode
 * 0xA0 - 0xAF   | JPOS microcode
 * 0xB0 - 0xBF   | JZER microcode
 * 0xC0 - 0xCF   | JNEG microcode
 * 0xD0 - 0xDF   | JNZE microcode
 * 0xE0 - 0xEF   | JUMP microcode
 * 0xF0 - 0xFF   | HALT microcode
 */

// Microcode start addresses for each instruction
#define MICROCODE_FETCH    0x00  // Common fetch routine
#define MICROCODE_DECODE   0x04  // Decode and dispatch
#define MICROCODE_LODD     0x10
#define MICROCODE_STOD     0x20
#define MICROCODE_ADDD     0x30
#define MICROCODE_SUBD     0x40
#define MICROCODE_LODL     0x50
#define MICROCODE_STOL     0x60
#define MICROCODE_ADDL     0x70
#define MICROCODE_SUBL     0x80
#define MICROCODE_LOCO     0x90
#define MICROCODE_JPOS     0xA0
#define MICROCODE_JZER     0xB0
#define MICROCODE_JNEG     0xC0
#define MICROCODE_JNZE     0xD0
#define MICROCODE_JUMP     0xE0
#define MICROCODE_HALT     0xF0

// ============================================================================
// INSTRUCTION FORMAT
// ============================================================================

/**
 * Instruction Word Format (16 bits):
 * 
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |   15   |   14   |   13   |   12   |   11   |   10   |    9   |    8   |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |                         OPCODE (8 bits)                                |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |    7   |    6   |    5   |    4   |    3   |    2   |    1   |    0   |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |                       OPERAND (8 bits)                                 |
 * |              (address, offset, or immediate value)                     |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 */

// Helper macros for instruction encoding/decoding
#define MAKE_INSTRUCTION(opcode, operand) (((opcode) << 8) | ((operand) & 0xFF))
#define GET_OPCODE(instruction) (((instruction) >> 8) & 0xFF)
#define GET_OPERAND(instruction) ((instruction) & 0xFF)

#endif // OPCODES_H
