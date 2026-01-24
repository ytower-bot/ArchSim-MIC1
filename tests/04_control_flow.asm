; ============================================================================
; TEST 04: CONTROL FLOW - Jumps, Conditions & Loop Counter
; ============================================================================
; OBJECTIVE: Test JUMP, JPOS, JNEG and implement a countdown loop (3->0)
; EXPECTED RESULTS:
;   - Unconditional jump works correctly
;   - Positive jump (JPOS) executes when AC > 0
;   - Negative jump (JNEG) executes when AC < 0
;   - Loop counter decrements correctly
;   - Final success marker at Memory[1058] = 1549
; ============================================================================

; Test 1: Unconditional Jump Test
        JUMP JUMP_TEST      ; Should jump to jump_test label
        LOCO 2989           ; Should NOT execute this (bad marker)
        STOD 1024           ; Should NOT execute this

JUMP_TEST:
        LOCO 1549           ; AC <- 1549 (0x60D good marker - proves jump worked)
        STOD 1024           ; Memory[1024] <- success marker

; Test 2: Positive Number Test (JPOS)
        LOCO 66             ; AC <- 66 (positive number)
        JPOS POS_TEST       ; Should jump because AC > 0
        LOCO 2989           ; Should NOT execute
        STOD 1025           ; Should NOT execute

POS_TEST:
        LOCO 1549           ; AC <- 1549 (success marker)
        STOD 1025           ; Memory[1025] <- success marker

; Test 3: Negative Number Test (JNEG)
; Note: 0xFFFF in 16-bit is -1 (65535 unsigned, or we use a large value > 32767)
        LOCO 4095           ; AC <- 4095 (0xFFF - this is positive, need different approach)
        STOD 1040           ; Store for later subtraction
        LOCO 0              ; AC <- 0
        SUBD 1040           ; AC <- 0 - 4095 = -4095 (negative in twos complement)
        JNEG NEG_TEST       ; Should jump because AC < 0
        LOCO 2989           ; Should NOT execute
        STOD 1026           ; Should NOT execute

NEG_TEST:
        LOCO 1549           ; AC <- 1549 (success marker)
        STOD 1026           ; Memory[1026] <- success marker

; Test 4: Simple Loop Counter - Count from 3 down to 0
        LOCO 3              ; AC <- 3 (loop counter)
        STOD 1048           ; Memory[1048] <- counter storage

LOOP_START:
        LODD 1048           ; AC <- current counter value
        JZER LOOP_DONE      ; If counter = 0, exit loop

; Decrement counter
        LOCO 1              ; AC <- 1
        STOD 1049           ; Memory[1049] <- 1 (for subtraction)
        LODD 1048           ; AC <- counter
        SUBD 1049           ; AC <- counter - 1
        STOD 1048           ; Memory[1048] <- new counter

; Continue loop
        JUMP LOOP_START     ; Loop back

LOOP_DONE:
        LOCO 3566           ; AC <- 3566 (0xDEE - done marker)
        STOD 1056           ; Memory[1056] <- done marker

; Test 5: Zero Test (JPOS should NOT jump on 0)
        LOCO 0              ; AC <- 0
        JPOS SHOULD_NOT     ; Should NOT jump (0 is not positive)
        LOCO 1549           ; AC <- success (this should execute)
        STOD 1057           ; Memory[1057] <- success marker
        JUMP FINAL_HALT     ; Skip the "should not execute" section

SHOULD_NOT:
        LOCO 2989           ; Should NOT execute
        STOD 1057           ; Should NOT execute

FINAL_HALT:
; Final success marker
        LOCO 1549           ; AC <- 1549 (final success)
        STOD 1058           ; Memory[1058] <- final success marker
        JUMP FINAL_HALT     ; Infinite loop

; ============================================================================
; EXPECTED FINAL MEMORY STATE:
;   [1024] = 1549     ; Unconditional jump success marker
;   [1025] = 1549     ; JPOS test success marker
;   [1026] = 1549     ; JNEG test success marker
;   [1048] = 0        ; Final counter value (0)
;   [1056] = 3566     ; Loop completion marker (0xDEE)
;   [1057] = 1549     ; Zero test success (proved JPOS did not jump on 0)
;   [1058] = 1549     ; Final success marker
; EXPECTED REGISTER STATE:
;   AC = 1549         ; Should contain final success marker
; ============================================================================
