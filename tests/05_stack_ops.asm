; ============================================================================
; TEST 05: STACK OPERATIONS - CRITICAL SP FIX VERIFICATION  
; ============================================================================
; CRITICAL: This test specifically addresses the SP bug where SP was stuck at 0x0000
; 
; OBJECTIVE: Verify Stack Pointer (SP) moves correctly during PUSH/POP operations
; INITIAL STATE: SP should start at 4095 (0x0FFF) - top of stack
; EXPECTED BEHAVIOR:
;   1. SP decrements on PUSH (SP--)  
;   2. SP increments on POP (SP++)
;   3. Values pushed/popped correctly
;   4. CALL/RETN work with nested subroutines
; 
; THIS TEST WILL FAIL IF SP REMAINS AT 0x0000!
; ============================================================================

START:
; Test 1: PUSH Operations - SP should decrement
        LOCO 273        ; AC ← 273 (0x111 hex)
        PUSH            ; PUSH AC to stack (SP should become 4094)

        LOCO 546        ; AC ← 546 (0x222 hex) 
        PUSH            ; PUSH AC to stack (SP should become 4093)

        LOCO 819        ; AC ← 819 (0x333 hex)
        PUSH            ; PUSH AC to stack (SP should become 4092)

; Test 2: POP Operations - SP should increment and values should return
        POP             ; POP from stack → AC (should get 819, SP → 4093)
        STOD 500        ; Memory[500] ← popped value (should be 819)

        POP             ; POP from stack → AC (should get 546, SP → 4094)  
        STOD 501        ; Memory[501] ← popped value (should be 546)

        POP             ; POP from stack → AC (should get 273, SP → 4095)
        STOD 502        ; Memory[502] ← popped value (should be 273)

; Test 3: Verify POP values are correct (check LIFO order)
        LODD 500        ; AC ← first popped value
        STOD 510        ; Memory[510] ← first popped (819)
        LOCO 819        ; Expected value  
        STOD 511        ; Memory[511] ← expected (819)
        LODD 510        ; AC ← actual
        SUBD 511        ; AC ← actual - expected (should be 0)
        STOD 512        ; Memory[512] ← difference (should be 0)

; Test 4: CALL and RETN - Subroutine Test  
        LOCO 3275       ; AC ← 3275 (0xCCB hex - call marker)
        STOD 520        ; Memory[520] ← call marker
        CALL SUBROUTINE ; Call subroutine (saves PC on stack, SP--)

; Return point from subroutine
        LOCO 3050       ; AC ← 3050 (0xBEA hex - return marker)
        STOD 521        ; Memory[521] ← return marker
        JUMP ENDTEST    ; Jump to end

; Subroutine definition
SUBROUTINE:
        LOCO 3051       ; AC ← 3051 (0xBEB hex - subroutine marker)
        STOD 522        ; Memory[522] ← subroutine marker
        RETN            ; Return from subroutine (pops PC, SP++)

; End test
ENDTEST:
; Test 5: Final stack integrity test
        LOCO 2457       ; AC ← 2457 (0x999 hex)
        PUSH            ; Push final value
        POP             ; Pop it back immediately
        STOD 525        ; Memory[525] ← should be 2457

; Success marker
        LOCO 1549       ; AC ← 1549 (0x60D hex - success marker)
        STOD 599        ; Memory[599] ← success marker

; HALT
HALT:   JUMP HALT       ; Infinite loop

; ============================================================================
; EXPECTED FINAL MEMORY STATE (if SP works correctly):
;   [500] = 819          ; Third value popped (LIFO order)
;   [501] = 546          ; Second value popped  
;   [502] = 273          ; First value popped
;   [512] = 0            ; Difference test (actual - expected = 0)
;   [520] = 3275         ; Call marker
;   [521] = 3050         ; Return marker (proves CALL/RETN worked)
;   [522] = 3051         ; Subroutine marker
;   [525] = 2457         ; Final push/pop test value
;   [599] = 1549         ; Success marker
;
; CRITICAL SP BEHAVIOR VERIFICATION:
;   - If SP was stuck at 0x0000, PUSH/POP operations would overwrite memory[0]
;   - If SP starts at 4095 and moves correctly, stack operations use high memory
;   - CALL/RETN would fail completely if SP doesn't work
; ============================================================================