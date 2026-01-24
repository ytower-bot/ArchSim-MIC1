; ============================================================================
; TEST 03: MEMORY I/O - Memory Read/Write Integrity Verification
; ============================================================================
; OBJECTIVE: Write values to memory, read them back, and verify integrity
; STRATEGY: Write -> Read -> Subtract -> Check if difference = 0
; EXPECTED RESULTS:
;   - Original value written correctly
;   - Value read back matches written value
;   - Difference (write - read) = 0
;   - Memory [787] = 1549 (proves read/write integrity)
; ============================================================================

; Test 1: Write a test pattern to memory
        LOCO 171            ; AC <- 171 (0xAB test pattern)
        STOD 768            ; Memory[768] <- 171

; Test 2: Write second test value
        LOCO 291            ; AC <- 291 (0x123)
        STOD 769            ; Memory[769] <- 291

; Test 3: Write third test value
        LOCO 1110           ; AC <- 1110 (0x456)
        STOD 770            ; Memory[770] <- 1110

; Test 4: Read back first value and verify integrity
        LODD 768            ; AC <- Memory[768] (should be 171)
        STOD 784            ; Memory[784] <- read value (for inspection)

; Test 5: Subtract original from read value (should be 0)
        LOCO 171            ; AC <- 171 (original value)
        STOD 785            ; Memory[785] <- original for comparison
        LODD 784            ; AC <- read value
        SUBD 785            ; AC <- read - original (should be 0)
        STOD 786            ; Memory[786] <- difference (should be 0)

; Test 6: Check if difference is zero (integrity test)
        JZER SUCCESS        ; If difference = 0, memory I/O is working

; FAILURE PATH
FAIL:
        LOCO 2989           ; AC <- 2989 (0xBAD failure marker)
        STOD 787            ; Memory[787] <- failure marker
        JUMP HALT           ; Jump to halt

; SUCCESS PATH
SUCCESS:
        LOCO 1549           ; AC <- 1549 (0x60D success marker)
        STOD 787            ; Memory[787] <- success marker

; Verify second value as well
        LODD 769            ; AC <- Memory[769] (should be 291)
        STOD 788            ; Memory[788] <- second read value

; Verify third value
        LODD 770            ; AC <- Memory[770] (should be 1110)
        STOD 789            ; Memory[789] <- third read value

; HALT
HALT:
        JUMP HALT           ; Infinite loop

; ============================================================================
; EXPECTED FINAL MEMORY STATE:
;   [768] = 171       ; First test pattern written
;   [769] = 291       ; Second test pattern written
;   [770] = 1110      ; Third test pattern written
;   [784] = 171       ; First value read back
;   [785] = 171       ; Original value for comparison
;   [786] = 0         ; Difference (read - original), should be 0
;   [787] = 1549      ; Success marker (proves integrity check passed)
;   [788] = 291       ; Second value read back
;   [789] = 1110      ; Third value read back
; EXPECTED REGISTER STATE:
;   AC = 1110         ; Should contain last read value
; ============================================================================
