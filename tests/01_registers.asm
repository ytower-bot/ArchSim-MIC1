; ============================================================================
; TEST 01: REGISTERS - Basic Register Movement & Integrity Check
; ============================================================================
; OBJECTIVE: Verify that AC and PC registers work correctly
; EXPECTED RESULTS:
;   - AC should contain 66 (0x42) after loading constant
;   - PC should increment correctly through instructions  
;   - Memory [100] should contain 66 after store
; ============================================================================

START:
; Test 1: Load constant into AC register
        LOCO 66         ; AC ← 66 (0x42 hex)

; Test 2: Store AC to memory to verify register integrity  
        STOD 100        ; Memory[100] ← AC

; Test 3: Load different value to test overwrite
        LOCO 291        ; AC ← 291 (0x123 hex)

; Test 4: Store second value to different location
        STOD 101        ; Memory[101] ← AC

; Test 5: Verify first value still in memory
        LODD 100        ; AC ← Memory[100] (should be 66)

; Test 6: Final store to verify reload worked
        STOD 102        ; Memory[102] ← AC (should be 66)

; HALT (infinite loop)
END:    JUMP END        ; Jump to this instruction (infinite loop)

; ============================================================================
; EXPECTED FINAL MEMORY STATE:
;   [100] = 66        ; First constant stored
;   [101] = 291       ; Second constant stored  
;   [102] = 66        ; First constant reloaded and stored again
; EXPECTED REGISTER STATE:
;   AC = 66           ; Should contain first constant after reload
; ============================================================================