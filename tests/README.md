# ArchSim-MIC1 Test Suite

Comprehensive test infrastructure for the MIC-1 simulator, ensuring compliance with Tanenbaum's "Structured Computer Organization" specification.

---

## 📁 Directory Structure

```
tests/
├── README.md                    # This file
├── test_framework.h             # Pure C test framework (zero dependencies)
├── unit/                        # Unit tests for individual components
│   └── test_example.c          # Example test suite (delete after creating real tests)
├── integration/                 # Integration tests (multi-component)
└── golden/                      # Golden file tests (assembler validation)
```

---

## 🧪 Test Framework: MIC1-Test

**Philosophy**: Zero external dependencies, C99-compliant, educational-friendly.

### Why Pure C?
- **No External Dependencies**: Students can run tests without installing Unity, Check, or Google Test
- **C99 Compliant**: Matches project standard (`-std=c99`)
- **Educational Transparency**: Students can read the framework source code
- **Portable**: Works on any platform with a C compiler

### Available Assertions

```c
ASSERT_TRUE(condition)              // Verify condition is true
ASSERT_FALSE(condition)             // Verify condition is false
ASSERT_EQUAL(expected, actual)      // Verify integer equality
ASSERT_NOT_EQUAL(expected, actual)  // Verify integer inequality
ASSERT_STR_EQUAL(expected, actual)  // Verify string equality
ASSERT_NULL(ptr)                    // Verify pointer is NULL
ASSERT_NOT_NULL(ptr)                // Verify pointer is not NULL
```

### Writing a Test

```c
#include "../test_framework.h"
#include "../../include/alu.h"  // Component under test

TEST_CASE(alu_operation_add) {
    // Test ALU addition
    uint16_t result = alu_execute(0x0005, 0x0003, ALU_OP_ADD);
    ASSERT_EQUAL(0x0008, result);
    return 0;
}

TEST_CASE(alu_flag_zero) {
    // Test zero flag
    uint16_t result = alu_execute(0x0000, 0x0000, ALU_OP_ADD);
    ASSERT_EQUAL(0x0000, result);
    ASSERT_TRUE(alu_get_z_flag());
    return 0;
}

TEST_SUITE_BEGIN("ALU Tests")
    RUN_TEST(alu_operation_add);
    RUN_TEST(alu_flag_zero);
TEST_SUITE_END()
```

---

## 🔧 Running Tests

### All Tests
```bash
make test
```
Runs: Unit tests → Integration tests → Golden file tests

### Unit Tests Only
```bash
make test-unit
```

### Integration Tests Only
```bash
make test-integration
```

### Golden File Tests Only
```bash
make test-golden
```

### Specific Test
```bash
./tests/unit/test_alu
```

---

## 📋 Test Categories

### 1. Unit Tests (`unit/`)

**Purpose**: Verify individual components in isolation.

**Target Components**:
- `test_alu.c` - ALU operations and flags (N, Z)
- `test_shifter.c` - Bit shifting operations
- `test_memory.c` - Memory read/write operations
- `test_cache.c` - Cache hits, misses, eviction
- `test_control_unit.c` - Microinstruction decoding
- `test_datapath.c` - Register bank, latches, decoders
- `test_assembler.c` - Instruction parsing, symbol table

**Coverage Goal**: >70% for core components.

---

### 2. Integration Tests (`integration/`)

**Purpose**: Verify interactions between multiple components.

**Example Tests**:
- `test_memory_cache.c` - Memory ↔ Cache synchronization
- `test_fetch_decode.c` - Fetch-decode-execute cycle
- `test_cpu_lifecycle.c` - Init → Reset → Execute → Shutdown
- `test_assembler_to_execution.c` - Assemble → Load → Execute pipeline

**Focus**: Ensure components communicate correctly via their interfaces.

---

### 3. Golden File Tests (`golden/`)

**Purpose**: Validate assembler output against known-good binaries.

**Structure**:
```
golden/
├── run_golden_tests.sh          # Test runner script
├── simple.asm                   # Source: basic arithmetic
├── simple.bin                   # Reference binary
├── stack_ops.asm                # Source: stack operations
├── stack_ops.bin                # Reference binary
├── all_opcodes.asm              # Source: all 23 IJVM instructions
└── all_opcodes.bin              # Reference binary
```

**Process**:
1. Assembler generates binary from `.asm`
2. Binary is compared byte-for-byte with `.bin` reference
3. Test fails if any byte differs

---

## ✅ Test Quality Standards

### Every Test Must
- [ ] Have a clear, descriptive name (`test_alu_overflow_behavior`)
- [ ] Test ONE specific behavior
- [ ] Include both positive and negative cases
- [ ] Clean up resources (no memory leaks)
- [ ] Return 0 on success, -1 on failure
- [ ] Be independent (order doesn't matter)

### Every Component Must Have
- [ ] At least 3 unit tests (happy path, edge case, error case)
- [ ] Code coverage >70%
- [ ] Tests that verify Tenenbaum spec compliance

---

## 🎨 Test Output

### Success
```
==========================================
Running unit tests...
==========================================

>>> Running tests/unit/test_alu...

=== Test Suite: ALU Tests ===

[TEST] alu_operation_add
[PASS] alu_operation_add
[TEST] alu_flag_zero
[PASS] alu_flag_zero

=== Test Summary ===
Total:  2
Passed: 2
Failed: 0
```

### Failure
```
[TEST] alu_operation_add
[FAIL] src/alu.c:42: Expected 8, got 7
       Expression: expected == actual
[FAIL] alu_operation_add
```

### Disable Colors
```bash
NO_COLOR=1 make test
```

---

## 🚀 CI/CD Integration

Tests are automatically run in GitHub Actions:
```yaml
- name: Run unit tests
  run: make test-unit

- name: Run integration tests
  run: make test-integration

- name: Run golden file tests
  run: make test-golden
```

---

## 📊 Coverage Analysis (Future)

**Planned**: Integrate `gcov` for code coverage analysis.

```bash
# Future command
make coverage
```

This will generate HTML coverage reports showing:
- Line coverage per file
- Branch coverage
- Function coverage

---

## 🐛 Debugging Failed Tests

### 1. Run Specific Test in GDB
```bash
gcc -g -std=c99 -I../include -Itests tests/unit/test_alu.c obj/*.o -o test_alu_debug
gdb ./test_alu_debug
```

### 2. Enable Verbose Output
Add debug prints in your test:
```c
TEST_CASE(alu_debug) {
    printf("[DEBUG] Input A: 0x%04X\n", input_a);
    uint16_t result = alu_execute(input_a, input_b, op);
    printf("[DEBUG] Result: 0x%04X\n", result);
    ASSERT_EQUAL(expected, result);
    return 0;
}
```

### 3. Memory Leak Detection (Valgrind)
```bash
valgrind --leak-check=full ./tests/unit/test_memory
```

---

## 📚 References

- **Tenenbaum**: "Structured Computer Organization" (6th Ed.), Chapter 4
- **Project Spec**: `docs/ArchSim-MIC1.md`
- **IJVM Spec**: `docs/ASSEMBLER_GUIDE.md`

---

## 🤝 Contributing Tests

### Before Submitting
1. All tests pass: `make test`
2. No memory leaks: `valgrind ./your_test`
3. Follows naming convention: `test_component_behavior.c`
4. Includes documentation comment at top of file

### Test Naming Convention
```c
// test_<component>_<behavior>.c
test_alu_overflow_handling.c      // Good
test_memory_cache_writeback.c     // Good
test1.c                            // Bad
my_test.c                          // Bad
```

---

**Last Updated**: 2026-01-22
**Maintainer**: ArchSim Consultants QA Team
