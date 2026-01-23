; ============================================================================
; PROOF_MATH.ASM - Mathematical Proof Test (2 + 2 = 4)
; ============================================================================
;
; PURPOSE:
;   Prove that the simulator correctly executes arithmetic operations
;   by computing 2 + 2 = 4 and storing intermediate/final values.
;
; EXPECTED RESULTS (check Memory View - press 'd'):
;   [0x80]: 0x0002  (first operand: 2)
;   [0x81]: 0x0002  (second operand: 2)
;   [0x82]: 0x0004  (result: 2 + 2 = 4)
;
; HOW TO VERIFY:
;   1. Load this file in TUI (press 'l')
;   2. Step through with 's' until PC stops advancing
;   3. Press 'd' to open Memory View
;   4. Confirm values at 0x80-0x82 match expected results
;
; NOTE: Using addresses 0x80-0x82 because operand is limited to 8 bits (0-255)
;
; ============================================================================

; Step 1: Load constant 2 into AC
LOCO 2          ; AC = 2

; Step 2: Store first operand at 0x80
STOD 128        ; Memory[0x80] = 2

; Step 3: Store second operand at 0x81 (still 2 in AC)
STOD 129        ; Memory[0x81] = 2

; Step 4: Add the value at 0x80 to AC (2 + 2)
ADDD 128        ; AC = AC + Memory[0x80] = 2 + 2 = 4

; Step 5: Store result at 0x82
STOD 130        ; Memory[0x82] = 4

; Step 6: Halt (infinite loop)
JUMP 5          ; Loop forever (safe halt)
