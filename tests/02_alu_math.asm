# ============================================================================
# TEST 02: ALU MATH - Arithmetic Operations & Flag Testing
# ============================================================================
# OBJECTIVE: Test ADDD, SUBD operations and verify Z flag with JZER
# EXPECTED RESULTS:
#   - Addition: 5 + 3 = 8
#   - Subtraction: 8 - 5 = 3  
#   - Zero result: 3 - 3 = 0 (sets Z flag)
#   - JZER should jump to success when AC = 0
#   - Memory [0x200] = 0xGOOD (success marker)
# ============================================================================

.ORG 0x000

# Test 1: Basic Addition
LOCO 0x005              # AC ← 5
STOD 0x200              # Memory[0x200] ← 5 (operand storage)
LOCO 0x003              # AC ← 3  
ADDD 0x200              # AC ← AC + Memory[0x200] = 3 + 5 = 8
STOD 0x201              # Memory[0x201] ← 8 (result storage)

# Test 2: Basic Subtraction  
LOCO 0x005              # AC ← 5
STOD 0x202              # Memory[0x202] ← 5 (operand storage)
LODD 0x201              # AC ← 8 (load previous result)
SUBD 0x202              # AC ← AC - Memory[0x202] = 8 - 5 = 3  
STOD 0x203              # Memory[0x203] ← 3 (result storage)

# Test 3: Zero Result & Flag Test
LODD 0x203              # AC ← 3
STOD 0x204              # Memory[0x204] ← 3
SUBD 0x204              # AC ← AC - Memory[0x204] = 3 - 3 = 0 (sets Z flag)
STOD 0x205              # Memory[0x205] ← 0 (zero result)

# Test 4: Conditional Jump on Zero
JZER success            # If AC = 0, jump to success (should jump!)

# FAILURE PATH (should not reach here)
fail:
LOCO 0xBAD              # AC ← 0xBAD (failure marker)
STOD 0x206              # Memory[0x206] ← 0xBAD  
JUMP halt               # Jump to halt

# SUCCESS PATH (should reach here via JZER)
success:
LOCO 0xGOD              # AC ← 0x600D (success marker)  
STOD 0x206              # Memory[0x206] ← 0x600D

# HALT (infinite loop)
halt:
JUMP halt               # Infinite loop

# ============================================================================
# EXPECTED FINAL MEMORY STATE:
#   [0x200] = 0x0005     # First operand
#   [0x201] = 0x0008     # Addition result (5 + 3)
#   [0x202] = 0x0005     # Second operand  
#   [0x203] = 0x0003     # Subtraction result (8 - 5)
#   [0x204] = 0x0003     # Zero test operand
#   [0x205] = 0x0000     # Zero result (3 - 3)
#   [0x206] = 0x600D     # Success marker (proves JZER worked)
# EXPECTED REGISTER STATE:
#   AC = 0x600D          # Should contain success marker
#   PC = halt address    # Should be in halt loop
# ============================================================================