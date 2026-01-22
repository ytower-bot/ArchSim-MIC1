# ArchSim-MIC1 Sprint Backlog
**Last Updated**: 2026-01-22
**Current Sprint**: Sprint 1 - Stabilization & Observability

---

## SPRINT 1 BACKLOG

### [TASK-001] [PRIORITY: High] Create tests/ Directory Structure
- **Context**: Makefile references `tests/` but directory doesn't exist. No automated testing for C core.
- **Definition of Done (DoD)**:
  - `tests/` directory created
  - Folder structure: `tests/unit/`, `tests/integration/`, `tests/golden/`
  - Basic test runner framework selected (Unity or pure C with custom macros)
- **Assignee**: Sherlock (QA & Test Engineer)
- **Dependencies**: None

---

### [TASK-002] [PRIORITY: High] ALU Unit Tests (C Core Verification)
- **Context**: `src/alu.c` has 4 operations (A, B, A+B, A AND B) + N/Z flag logic. Must match Tenenbaum spec exactly.
- **Definition of Done (DoD)**:
  - `tests/unit/test_alu.c` created
  - Test cases for:
    - Operation 0 (pass A)
    - Operation 1 (pass B)
    - Operation 2 (A + B, including overflow)
    - Operation 3 (A AND B)
    - N flag (result < 0)
    - Z flag (result == 0)
  - Tests pass via `make test-alu`
- **Assignee**: C-3PO (Core C Engineer) + Sherlock (QA)
- **Dependencies**: [TASK-001]

---

### [TASK-003] [PRIORITY: High] Assembler Golden File Tests
- **Context**: `src/assembler.c` generates binary output. No validation that output matches reference binaries.
- **Definition of Done (DoD)**:
  - `tests/golden/` created with:
    - `simple.asm` + `simple.bin` (reference binary)
    - `stack_ops.asm` + `stack_ops.bin`
    - `all_opcodes.asm` + `all_opcodes.bin`
  - Test script: `tests/test_assembler_golden.sh`
  - Script runs assembler, compares output via `cmp` or hex diff
  - Exit code 0 if all match
- **Assignee**: Asm (Assembly Specialist) + Sherlock (QA)
- **Dependencies**: [TASK-001]

---

### [TASK-004] [PRIORITY: Critical] TUI Debug Mode Implementation
- **Context**: User reports "Is the screen showing the real memory?" - need observability.
- **Definition of Done (DoD)**:
  - New Go command-line flag: `--debug` or `-d`
  - When enabled, TUI writes to `tui_debug.log` on every render:
    ```
    [CYCLE 42]
    C Memory State:
      AC=0x0042 PC=0x0100 SP=0x0FFF
      Memory[0x0100]=0xABCD
    TUI Render State:
      Displayed AC=0x0042 (MATCH)
      Displayed PC=0x0100 (MATCH)
      Displayed Memory[0x0100]=0xABCD (MATCH)
    ```
  - Log includes timestamp, cycle count, comparison between CGO data and rendered values
  - Documented in `tui/TUI_GUIDE.md`
- **Assignee**: Gopher (TUI & Go Engineer)
- **Dependencies**: None

---

### [TASK-005] [PRIORITY: Medium] Memory Integration Test
- **Context**: Verify `src/memory.c` + `src/cache.c` integration. Ensure cache hits/misses update main memory correctly.
- **Definition of Done (DoD)**:
  - `tests/integration/test_memory_cache.c` created
  - Test cases:
    - Write to address 0x100, verify in main memory
    - Read from 0x100, verify cache hit
    - Write to 9 different addresses (force cache eviction), verify writeback
  - Tests pass via `make test-memory`
- **Assignee**: C-3PO (Core C Engineer)
- **Dependencies**: [TASK-001]

---

### [TASK-006] [PRIORITY: Medium] CI/CD Enhancement
- **Context**: Current `.github/workflows/build.yml` only builds C code. Doesn't test TUI or run tests.
- **Definition of Done (DoD)**:
  - Update `build.yml`:
    - Job 1: Build C core + run unit tests (`make test`)
    - Job 2: Build TUI (`make tui`)
    - Job 3: Run integration tests
  - Add Go cache for faster builds
  - Upload test results as artifacts
- **Assignee**: Docker (DevOps)
- **Dependencies**: [TASK-001], [TASK-002], [TASK-003]

---

### [TASK-007] [PRIORITY: Low] Professional README Enhancement
- **Context**: Current README is in Portuguese, lacks badges, no architecture diagram.
- **Definition of Done (DoD)**:
  - Translate README.md to English (keep Portuguese in `docs/README_PT.md`)
  - Add badges:
    - ![Build Status](https://github.com/ytower/ArchSim-MIC1/workflows/Build%20and%20Test/badge.svg)
    - ![License](https://img.shields.io/badge/license-MIT-blue.svg)
    - ![Language](https://img.shields.io/badge/C-99-blue.svg)
    - ![Go](https://img.shields.io/badge/Go-1.24-blue.svg)
  - Add ASCII architecture diagram showing:
    - C Core (ALU, Control Unit, Memory)
    - CGO Bridge
    - Go TUI (Bubbletea)
  - Add "Quick Start" section (3 steps max)
- **Assignee**: Scribe (Tech Writer)
- **Dependencies**: None

---

### [TASK-008] [PRIORITY: Low] ADR: Test Framework Selection
- **Context**: Need to document WHY we chose Unity (or pure C) for testing.
- **Definition of Done (DoD)**:
  - `.agile/adr/001-test-framework.md` created
  - Document:
    - Options considered (Unity, Check, Pure C, Google Test)
    - Decision: [Framework Name]
    - Rationale: (Simplicity, C99 compatibility, no external deps, etc.)
    - Consequences: (How we'll structure tests)
- **Assignee**: Atlas (Tech Lead)
- **Dependencies**: [TASK-001]

---

## BACKLOG (Not in Sprint 1)

### [TASK-009] [PRIORITY: Low] Python Bindings via CFFI
- **Context**: Some users prefer Python for scripting simulations.
- **DoD**: TBD
- **Assignee**: TBD

### [TASK-010] [PRIORITY: Low] Web UI Alternative
- **Context**: Browser-based UI for users without terminal access.
- **DoD**: TBD
- **Assignee**: TBD

---

## COMPLETED TASKS
(None yet)
