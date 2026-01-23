# ============================================================================
# TEST 03: MEMORY I/O - Memory Read/Write Integrity Verification
# ============================================================================
# OBJECTIVE: Write values to memory, read them back, and verify integrity
# STRATEGY: Write → Read → Subtract → Check if difference = 0
# EXPECTED RESULTS:
#   - Original value written correctly
#   - Value read back matches written value
#   - Difference (write - read) = 0 
#   - Memory [0x300] = 0x0000 (proves read/write integrity)
# ============================================================================

.ORG 0x000

# Test 1: Write a test pattern to memory
LOCO 0x0AB              # AC ← 0x0AB (171 decimal, test pattern)
STOD 0x300              # Memory[0x300] ← 0x0AB

# Test 2: Write second test value
LOCO 0x123              # AC ← 0x123 (291 decimal)  
STOD 0x301              # Memory[0x301] ← 0x123

# Test 3: Write third test value
LOCO 0x456              # AC ← 0x456 (1110 decimal)
STOD 0x302              # Memory[0x302] ← 0x456

# Test 4: Read back first value and verify integrity
LODD 0x300              # AC ← Memory[0x300] (should be 0x0AB)
STOD 0x310              # Memory[0x310] ← read value (for inspection)

# Test 5: Subtract original from read value (should be 0)
LOCO 0x0AB              # AC ← 0x0AB (original value)
STOD 0x311              # Memory[0x311] ← original for comparison  
LODD 0x310              # AC ← read value
SUBD 0x311              # AC ← read - original (should be 0)
STOD 0x312              # Memory[0x312] ← difference (should be 0)

# Test 6: Check if difference is zero (integrity test)
JZER success            # If difference = 0, memory I/O is working

# FAILURE PATH
fail:
LOCO 0xBAD              # AC ← 0xBAD (failure marker)
STOD 0x313              # Memory[0x313] ← failure marker
JUMP halt               # Jump to halt

# SUCCESS PATH  
success:
LOCO 0x600D             # AC ← 0x600D (success marker)
STOD 0x313              # Memory[0x313] ← success marker

# Verify second value as well
LODD 0x301              # AC ← Memory[0x301] (should be 0x123)  
STOD 0x314              # Memory[0x314] ← second read value

# Verify third value
LODD 0x302              # AC ← Memory[0x302] (should be 0x456)
STOD 0x315              # Memory[0x315] ← third read value

# HALT
halt:
JUMP halt               # Infinite loop

# ============================================================================
# EXPECTED FINAL MEMORY STATE:
#   [0x300] = 0x00AB     # First test pattern written
#   [0x301] = 0x0123     # Second test pattern written  
#   [0x302] = 0x0456     # Third test pattern written
#   [0x310] = 0x00AB     # First value read back
#   [0x311] = 0x00AB     # Original value for comparison
#   [0x312] = 0x0000     # Difference (read - original), should be 0
#   [0x313] = 0x600D     # Success marker (proves integrity check passed)
#   [0x314] = 0x0123     # Second value read back  
#   [0x315] = 0x0456     # Third value read back
# EXPECTED REGISTER STATE:
#   AC = 0x0456          # Should contain last read value
# ============================================================================