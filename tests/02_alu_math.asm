; ============================================================================
; TEST 02: ALU MATH - Arithmetic Operations & Flag Testing
; ============================================================================
; OBJECTIVE: Test ADDD, SUBD operations and verify Z flag with JZER
; EXPECTED RESULTS:
;   - Addition: 5 + 3 = 8
;   - Subtraction: 8 - 5 = 3
;   - Zero result: 3 - 3 = 0 (sets Z flag)
;   - JZER should jump to success when AC = 0
;   - Memory [518] = 1549 (success marker: 0x60D)
; ============================================================================

; Test 1: Basic Addition
        LOCO 5              ; AC <- 5
        STOD 512            ; Memory[512] <- 5 (operand storage)
        LOCO 3              ; AC <- 3
        ADDD 512            ; AC <- AC + Memory[512] = 3 + 5 = 8
        STOD 513            ; Memory[513] <- 8 (result storage)

; Test 2: Basic Subtraction
        LOCO 5              ; AC <- 5
        STOD 514            ; Memory[514] <- 5 (operand storage)
        LODD 513            ; AC <- 8 (load previous result)
        SUBD 514            ; AC <- AC - Memory[514] = 8 - 5 = 3
        STOD 515            ; Memory[515] <- 3 (result storage)

; Test 3: Zero Result & Flag Test
        LODD 515            ; AC <- 3
        STOD 516            ; Memory[516] <- 3
        SUBD 516            ; AC <- AC - Memory[516] = 3 - 3 = 0 (sets Z flag)
        STOD 517            ; Memory[517] <- 0 (zero result)

; Test 4: Conditional Jump on Zero
        JZER SUCCESS        ; If AC = 0, jump to success (should jump!)

; FAILURE PATH (should not reach here)
FAIL:
        LOCO 2989           ; AC <- 2989 (0xBAD failure marker)
        STOD 518            ; Memory[518] <- failure marker
        JUMP HALT           ; Jump to halt

; SUCCESS PATH (should reach here via JZER)
SUCCESS:
        LOCO 1549           ; AC <- 1549 (0x60D success marker)
        STOD 518            ; Memory[518] <- success marker

; HALT (infinite loop)
HALT:
        JUMP HALT           ; Infinite loop

; ============================================================================
; EXPECTED FINAL MEMORY STATE:
;   [512] = 5         ; First operand
;   [513] = 8         ; Addition result (5 + 3)
;   [514] = 5         ; Second operand
;   [515] = 3         ; Subtraction result (8 - 5)
;   [516] = 3         ; Zero test operand
;   [517] = 0         ; Zero result (3 - 3)
;   [518] = 1549      ; Success marker (proves JZER worked)
; EXPECTED REGISTER STATE:
;   AC = 1549         ; Should contain success marker
;   PC = halt address ; Should be in halt loop
; ============================================================================
