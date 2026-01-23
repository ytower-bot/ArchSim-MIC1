; ============================================================================
; PROOF_12BIT.ASM - 12-bit Address Range Verification Test
; ============================================================================
;
; PURPOSE:
;   Validate that the assembler and simulator correctly support 12-bit
;   operands (addresses 0x000-0xFFF / 0-4095)
;
; EXPECTED RESULTS (check Memory View - press 'd'):
;   [0x100]: 0x0002  (first operand: 2)
;   [0x101]: 0x0002  (second operand: 2)
;   [0x102]: 0x0004  (result: 2 + 2 = 4)
;
; HOW TO VERIFY:
;   1. Assemble: ./mic1asm tests/proof_12bit.asm tests/proof_12bit.bin
;   2. Hexdump: xxd tests/proof_12bit.bin
;   3. Expected encoding for STOD 0x100: 0x1100 (opcode 1 + operand 0x100)
;
; ============================================================================

; Step 1: Load constant 2 into AC
LOCO 2          ; AC = 2

; Step 2: Store first operand at 0x100 (256 decimal - beyond 8-bit limit)
STOD 256        ; Memory[0x100] = 2

; Step 3: Store second operand at 0x101
STOD 257        ; Memory[0x101] = 2

; Step 4: Add the value at 0x100 to AC (2 + 2)
ADDD 256        ; AC = AC + Memory[0x100] = 2 + 2 = 4

; Step 5: Store result at 0x102
STOD 258        ; Memory[0x102] = 4

; Step 6: Halt (infinite loop)
JUMP 5          ; Loop forever (safe halt)
