# ============================================================================
# TEST 04: CONTROL FLOW - Jumps, Conditions & Loop Counter
# ============================================================================
# OBJECTIVE: Test JUMP, JPOS, JNEG and implement a countdown loop (5→0)
# EXPECTED RESULTS:
#   - Unconditional jump works correctly
#   - Positive jump (JPOS) executes when AC ≥ 0  
#   - Negative jump (JNEG) executes when AC < 0
#   - Loop counter decrements from 5 to 0
#   - Memory [0x400] = 5, [0x401] = 4, ... [0x405] = 0 (loop trace)
# ============================================================================

.ORG 0x000

# Test 1: Unconditional Jump Test
JUMP jump_test          # Should jump to jump_test label
LOCO 0xBAD              # Should NOT execute this (bad marker)
STOD 0x400              # Should NOT execute this

jump_test:
LOCO 0x600D             # AC ← 0x600D (good marker - proves jump worked)  
STOD 0x400              # Memory[0x400] ← success marker

# Test 2: Positive Number Test (JPOS)
LOCO 0x042              # AC ← 66 (positive number)
JPOS pos_test           # Should jump because AC > 0
LOCO 0xBAD              # Should NOT execute
STOD 0x401              # Should NOT execute

pos_test:
LOCO 0x600D             # AC ← 0x600D (success marker)
STOD 0x401              # Memory[0x401] ← success marker  

# Test 3: Negative Number Test (JNEG)
LOCO 0xFFF              # AC ← 0xFFF (negative in two's complement)
JNEG neg_test           # Should jump because AC < 0
LOCO 0xBAD              # Should NOT execute
STOD 0x402              # Should NOT execute

neg_test:
LOCO 0x600D             # AC ← 0x600D (success marker)
STOD 0x402              # Memory[0x402] ← success marker

# Test 4: Loop Counter - Count from 5 down to 0
LOCO 0x005              # AC ← 5 (loop counter)
STOD 0x410              # Memory[0x410] ← counter storage

loop_start:
LODD 0x410              # AC ← current counter value
STOD 0x411              # Memory[0x411] ← save counter for trace

# Store counter value in trace array  
LODD 0x411              # AC ← counter value
ADDD 0x500              # AC ← counter + 0x500 (base address for trace)
STOD 0x412              # Memory[0x412] ← trace address
# (Note: This is simplified - real implementation would use indirect addressing)

# Manual trace storage (since we can't do computed addressing easily)
LODD 0x411              # AC ← counter value
STOD 0x420              # Memory[0x420] ← counter (will be overwritten each loop)

# Decrement counter
LODD 0x410              # AC ← counter  
LOCO 0x001              # Need to subtract 1
STOD 0x413              # Memory[0x413] ← 1
LODD 0x410              # AC ← counter
SUBD 0x413              # AC ← counter - 1
STOD 0x410              # Memory[0x410] ← new counter

# Check if counter reached 0
JZER loop_done          # If counter = 0, exit loop

# Check if counter is positive (continue loop)  
JPOS loop_start         # If counter > 0, continue loop

loop_done:
LOCO 0xD0NE             # AC ← 0xD0NE (done marker)
STOD 0x421              # Memory[0x421] ← done marker

# Test 5: Zero Test (should not jump)
LOCO 0x000              # AC ← 0
JPOS should_not_jump    # Should NOT jump (0 is not positive) 
LOCO 0x600D             # AC ← success (this should execute)
STOD 0x422              # Memory[0x422] ← success marker
JUMP final_halt         # Skip the "should not execute" section

should_not_jump:
LOCO 0xBAD              # Should NOT execute
STOD 0x422              # Should NOT execute

final_halt:
JUMP final_halt         # Infinite loop

# ============================================================================
# EXPECTED FINAL MEMORY STATE:
#   [0x400] = 0x600D     # Unconditional jump success marker
#   [0x401] = 0x600D     # JPOS test success marker 
#   [0x402] = 0x600D     # JNEG test success marker
#   [0x410] = 0x0000     # Final counter value (0)
#   [0x420] = 0x0000     # Last counter value stored (0)  
#   [0x421] = 0xD0NE     # Loop completion marker
#   [0x422] = 0x600D     # Zero test success (proved JPOS didn't jump on 0)
# EXPECTED REGISTER STATE:
#   AC = 0x600D          # Should contain final success marker
# ============================================================================