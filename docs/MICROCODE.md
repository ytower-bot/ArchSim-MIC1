# Microcode Implementation Notes

Technical documentation for the MIC-1 microprogram implementation.

---

## Instruction Encoding Format

### MAC-1 ISA (16-bit instructions)

```
Standard Instructions (opcodes 0x0-0xE):
┌─────────────┬─────────────────────────────┐
│   15:12     │           11:0              │
├─────────────┼─────────────────────────────┤
│   Opcode    │         Operand             │
│   (4 bits)  │        (12 bits)            │
└─────────────┴─────────────────────────────┘

Special Instructions (opcode 0xF):
┌─────────────┬─────────────┬───────────────┐
│   15:12     │    11:8     │      7:0      │
├─────────────┼─────────────┼───────────────┤
│    0xF      │  Sub-opcode │   Operand     │
│   (4 bits)  │   (4 bits)  │   (8 bits)    │
└─────────────┴─────────────┴───────────────┘
```

### Opcode Table

| Mnemonic | Binary | Hex | Description |
|----------|--------|-----|-------------|
| LODD | 0000 | 0x0 | Load direct |
| STOD | 0001 | 0x1 | Store direct |
| ADDD | 0010 | 0x2 | Add direct |
| SUBD | 0011 | 0x3 | Subtract direct |
| JPOS | 0100 | 0x4 | Jump if positive |
| JZER | 0101 | 0x5 | Jump if zero |
| JUMP | 0110 | 0x6 | Unconditional jump |
| LOCO | 0111 | 0x7 | Load constant |
| LODL | 1000 | 0x8 | Load local |
| STOL | 1001 | 0x9 | Store local |
| ADDL | 1010 | 0xA | Add local |
| SUBL | 1011 | 0xB | Subtract local |
| JNEG | 1100 | 0xC | Jump if negative |
| JNZE | 1101 | 0xD | Jump if not zero |
| CALL | 1110 | 0xE | Call subroutine |
| *special* | 1111 | 0xF | Extended instructions |

### Special Instructions (0xF prefix)

| Mnemonic | Sub-opcode | Full Code | Description |
|----------|------------|-----------|-------------|
| PSHI | 0000 | 0xF0 | Push indirect |
| POPI | 0010 | 0xF2 | Pop indirect |
| PUSH | 0100 | 0xF4 | Push AC |
| POP | 0110 | 0xF6 | Pop to AC |
| RETN | 1000 | 0xF8 | Return |
| SWAP | 1010 | 0xFA | Swap AC and SP |
| INSP | 1100 | 0xFC | Increment SP |
| DESP | 1110 | 0xFE | Decrement SP |

---

## Microprogram Address Mapping

### Important Note for Developers

The microprogram in `data/basic_microcode.txt` uses **reorganized addresses** compared to the original PDF reference (`reference/microprograma.pdf`). This is intentional and does NOT affect correctness.

### Address Differences

| Phase | PDF Addresses | Implementation Addresses |
|-------|---------------|-------------------------|
| Fetch | 0-2 | 0x00-0x04 |
| Decode/Dispatch | 3-5 | 0x05-0x13 |
| LODD | 6-8 | 0x14-0x17 |
| STOD | 9-10 | 0x18-0x1B |
| ADDD | 12-14 | 0x1C-0x1F |
| SUBD | 15-18 | 0x20-0x27 |
| JPOS | 21-22 | 0x28-0x2B |
| JZER | 23-24 | 0x2C-0x2F |
| JUMP | 26 | 0x30-0x33 |
| LOCO | 27 | 0x34-0x37 |
| ... | ... | ... |

### Why the Difference?

1. **Alignment**: Implementation uses 4-address blocks per instruction for cleaner dispatch
2. **Padding**: Extra NOPs added for power-of-2 alignment
3. **Dispatch Logic**: `run_mmux()` calculates target addresses dynamically

### Verification

Both implementations are **semantically equivalent**:
- Same register transfers
- Same ALU operations
- Same control flow logic
- Different absolute addresses, same relative jumps

---

## Microinstruction Format (32 bits)

```
Bit Layout:
┌──────┬───┬───┬───┬───┬──┬──┬──┬───┬───┬─────┬─────┬──────┬─────┐
│ ADDR │ A │ B │ C │ENC│WR│RD│MAR│MBR│ SH │ ALU │ COND │AMUX │
├──────┼───┼───┼───┼───┼──┼──┼───┼───┼────┼─────┼──────┼─────┤
│31-24 │23-20│19-16│15-12│11│10│9│ 8 │ 7 │6-5 │ 4-3 │ 2-1  │  0  │
└──────┴───┴───┴───┴───┴──┴──┴───┴───┴────┴─────┴──────┴─────┘

ADDR[7:0]  - Next microinstruction address (or 0xFF for dispatch)
A[3:0]     - Source register A selector
B[3:0]     - Source register B selector
C[3:0]     - Destination register selector
ENC        - Enable register write
WR         - Memory write
RD         - Memory read
MAR        - Load MAR from B bus
MBR        - Load MBR from shifter
SH[1:0]    - Shifter control (00=none, 01=right, 10=left)
ALU[1:0]   - ALU operation (00=A+B, 01=A AND B, 10=A, 11=NOT A)
COND[1:0]  - Branch condition (00=none, 01=if_N, 10=if_Z, 11=always)
AMUX       - A input mux (0=Latch_A, 1=MBR)
```

---

## Register File

| Index | Name | Purpose |
|-------|------|---------|
| 0 | PC | Program Counter |
| 1 | AC | Accumulator |
| 2 | IR | Instruction Register |
| 3 | TIR | Temporary IR (for decoding) |
| 4 | SP | Stack Pointer |
| 5 | AMASK | Address Mask (0x0FFF) |
| 6 | SMASK | Short Mask (0x00FF) |
| 7 | R0 | Constant 0 |
| 8 | R1 | Constant +1 |
| 9 | Rm1 | Constant -1 |
| 10-15 | A-F | Temporary registers |

---

## References

- Tanenbaum, A. S. "Structured Computer Organization" - Chapter 4
- `reference/microprograma.pdf` - Original microprogram listing
- `reference/microarquitetura.pdf` - Datapath diagram
