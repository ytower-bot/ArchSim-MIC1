; ============================================================================
; SANITY CHECK - Encoding Verification Test
; ============================================================================
; Purpose: Verify that opcode encoding uses correct bit positions
;
; Expected binary output (hexdump):
;   LOCO 5   -> 0x7005  (opcode 0x7 in bits[15:12], operand 5 in bits[11:0])
;   LODD 10  -> 0x000A  (opcode 0x0 in bits[15:12], operand 10 in bits[11:0])
;   JUMP 0   -> 0x6000  (opcode 0x6 in bits[15:12], operand 0 in bits[11:0])
;   PUSH     -> 0xF400  (opcode 0xF4 in bits[15:8], no operand)
;   LOCO 255 -> 0x70FF  (opcode 0x7, operand 255)
; ============================================================================

LOCO 5      ; 0x7005 - Load constant 5 into AC
LODD 10     ; 0x000A - Load from address 10
JUMP 0      ; 0x6000 - Jump to address 0
PUSH        ; 0xF400 - Push AC to stack
LOCO 255    ; 0x70FF - Load constant 255
