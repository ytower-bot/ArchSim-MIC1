# Practical Examples: Multi-Agent System in Action

**Companion to**: REPLICATION_GUIDE.md
**Purpose**: Real-world examples from ArchSim-MIC1 project

---

## 📚 Table of Contents

1. [Complete Task Lifecycle: Test Infrastructure](#example-1-complete-task-lifecycle)
2. [Multi-Agent Collaboration: TUI Debug Mode](#example-2-multi-agent-collaboration)
3. [Handling Conflicts: Commit Pattern Enforcement](#example-3-handling-conflicts)
4. [ADR Creation: Framework Selection](#example-4-adr-creation)
5. [Emergency Response: Build Failure](#example-5-emergency-response)

---

## Example 1: Complete Task Lifecycle

### TASK-001: Create Test Infrastructure

**User Request**: "Create test infrastructure"

---

#### Step 1: Atlas Analyzes

```markdown
### 👤 Atlas - Tech Lead & Architect

Analyzing request. This is a foundational task.

Current state:
- No tests/ directory
- Makefile has test targets but no implementation
- Zero test coverage

Creating TASK-001...
```

#### Step 2: Task Definition

```markdown
### [TASK-001] [PRIORITY: High] Create Test Infrastructure

- **Context**: Makefile references tests/ but directory doesn't exist. No automated testing for C core.
- **Definition of Done (DoD)**:
  - tests/ directory created with unit/, integration/, golden/ subdirectories
  - Test framework selected and documented (ADR)
  - Makefile integration (make test-unit, make test-integration, make test-golden)
  - Example test passing
  - Documentation in tests/README.md
- **Assignee**: Sherlock (QA)
- **Dependencies**: None

Handing off to Sherlock.
```

#### Step 3: Sherlock Takes Ownership

```markdown
### 👤 Sherlock - QA & Test Engineer

Taking ownership of TASK-001.

First, let me verify current state:
[Reads Makefile, checks for existing tests/]

Confirmed: No tests/ directory exists.

Breaking into atomic subtasks:
1. Evaluate test frameworks (Unity vs Pure C)
2. Create directory structure
3. Implement chosen framework
4. Create example test
5. Update Makefile
6. Write documentation
7. Write ADR documenting framework choice

Starting execution...
```

#### Step 4: Execution (Atomic Commits)

```bash
# Commit 1: Infrastructure
git commit -m "feat: add test framework header"

# Commit 2: Example
git commit -m "test: add example test suite"

# Commit 3: Build integration
git commit -m "chore: update Makefile for test targets"

# Commit 4: Documentation
git commit -m "docs: add testing strategy guide"

# Commit 5: Decision record
git commit -m "docs: add ADR for test framework selection"
```

#### Step 5: Verification

```markdown
### 👤 Sherlock - QA

Verification:
```bash
$ make test
==========================================
Running unit tests...
==========================================

>>> Running tests/unit/test_example...

=== Test Suite: Example Test Suite ===

[TEST] arithmetic_addition
[PASS] arithmetic_addition
...
Total:  5
Passed: 5
Failed: 0

✓ All tests passed!
```

All DoD criteria met:
- ✅ Directory structure exists
- ✅ Framework: Pure C (ADR-001)
- ✅ Makefile targets work
- ✅ Example tests passing
- ✅ Documentation complete

TASK-001 complete. Reporting to Atlas.
```

#### Step 6: Atlas Reports Completion

```markdown
### 👤 Atlas - Tech Lead

**TASK-001 Assessment**: ✅ COMPLETE

**Deliverables**:
- tests/test_framework.h (150 LOC, zero dependencies)
- tests/unit/test_example.c (5 passing tests)
- tests/README.md (320 lines documentation)
- .agile/adr/001-test-framework-selection.md

**Commits**:
- bd4247f - feat: add test framework header
- [4 more commits]

**Total**: 5 commits, 777 insertions

**Impact**:
- Test coverage: 0% → Infrastructure ready
- Build targets: 1 → 4 (test, test-unit, test-integration, test-golden)

**Ready for**: TASK-002 (ALU Unit Tests)

Awaiting your approval to proceed.
```

---

## Example 2: Multi-Agent Collaboration

### TASK-004: TUI Debug Mode

**User Request**: "eu quero opcao B + C" (debug improvements + integration tests)

---

#### Phase 1: Atlas Coordinates

```markdown
### 👤 Atlas - Tech Lead

User wants:
- Option B: Improve debug mode (show assembly, cache content, validation)
- Option C: Create integration tests

This requires **two agents**:
- Gopher: Debug mode improvements (Go/TUI)
- Sherlock: Integration tests (C)

Breaking into parallel tracks:

**Track A (Gopher)**: Debug mode enhancements
- Show assembly code in log
- Show cache contents
- Automatic validation on exit

**Track B (Sherlock)**: Integration tests
- CPU initialization test
- Assembler + execution test

Can execute in parallel. Starting with Track A (critical user concern).
```

#### Phase 2: Gopher Executes Track A

```markdown
### 👤 Gopher - TUI & Go Engineer

Taking Track A: Debug mode improvements.

User concern: "Is the screen showing the real memory?"

Solution: Enhanced debug logging with:
1. Assembly code display (current line + context)
2. Full cache state (all 8 lines)
3. Mismatch counter + exit code validation

Atomic commits planned:
- Commit 1: Add assembly code logging
- Commit 2: Add cache content logging
- Commit 3: Add validation on exit

Executing...
```

**Commit 1**:
```bash
git add tui/debug.go tui/model.go
git commit -m "feat: add assembly code display to debug log"
```

**Commit 2**:
```bash
git add tui/debug.go tui/model.go
git commit -m "feat: add cache content logging to debug mode"
```

**Commit 3**:
```bash
git add tui/debug.go tui/main.go
git commit -m "feat: add automatic validation on debug exit"
```

#### Phase 3: Sherlock Executes Track B

```markdown
### 👤 Sherlock - QA

Taking Track B: Integration tests.

Creating tests/integration/test_cpu_integration.c:
- Test 1: Load microcode + program
- Test 2: CPU init + reset

[Implements tests]
```

**Commit**:
```bash
git add tests/integration/test_cpu_integration.c
git commit -m "test: add CPU integration tests"
```

#### Phase 4: Atlas Consolidates

```markdown
### 👤 Atlas - Tech Lead

Both tracks complete:

**Track A (Gopher)**: ✅
- Assembly execution logging
- Cache content display
- Validation with exit codes

**Track B (Sherlock)**: ✅
- 2 integration tests passing

**Total**: 4 commits following conventional pattern

User's requirements (Option B + C) fully met.
```

---

## Example 3: Handling Conflicts

### Scenario: User Requests Large Commit

**User**: "Create all tests in one commit"

---

#### Atlas Intervenes

```markdown
### 👤 Atlas - Tech Lead

User requested single large commit, but our standard is atomic commits.

**Issue**: Violates micro-task philosophy and Conventional Commits standard.

**Recommendation**: Break into logical units:
1. `test: add ALU operation tests` (5 tests)
2. `test: add ALU flag tests` (5 tests)
3. `chore: remove example test file`

Each commit is:
- Independently testable
- Reversible without conflicts
- Clear in git history

Proceeding with atomic approach unless user objects.
```

---

## Example 4: ADR Creation

### ADR-001: Test Framework Selection

---

#### Context: Sherlock Needs to Choose

```markdown
### 👤 Sherlock - QA

TASK-001 requires framework selection. Options:
1. Unity (external, popular)
2. Check (external, mature)
3. Google Test (C++, heavyweight)
4. Pure C (custom, lightweight)

Need to document decision. Creating ADR-001...
```

#### ADR Structure

```markdown
# ADR-001: Test Framework Selection

**Status**: Accepted
**Date**: 2026-01-22
**Deciders**: Atlas, Sherlock, C-3PO
**Technical Story**: TASK-001

## Context

ArchSim-MIC1 is an educational project. Students should:
- Understand all code (including tests)
- Run tests without installing dependencies
- Learn how test frameworks work

## Decision

Use **Pure C custom framework** (tests/test_framework.h)

## Options Considered

### Unity
- Pros: Lightweight, C99, popular
- Cons: External dependency, git submodule complexity

### Check
- Pros: Mature, widely used
- Cons: Requires installation, not immediately transparent

### Google Test
- Pros: Industry standard
- Cons: **C++ not C**, massive dependency

### Pure C (CHOSEN)
- Pros:
  - Zero external dependencies
  - Students can read entire framework (150 LOC)
  - C99 compliant
  - Fully customizable
  - Educational value
- Cons:
  - We maintain it
  - Lacks advanced features (mocking, fixtures)

## Rationale

Educational transparency > external features.
Students can `git clone` and run tests immediately.

## Consequences

**Positive**:
- Students learn how test frameworks work
- No build complexity
- Tests compile with same flags as production

**Negative**:
- We own maintenance
- **Mitigation**: Framework is <200 LOC, stable design

## Validation

Success if:
- New contributor writes test in <10 min
- Test suite runs in <5 seconds
- Zero dependency-related issues

**Review Date**: After Sprint 2
```

---

## Example 5: Emergency Response

### Scenario: Build Failure in CI

**Alert**: GitHub Actions build failing

---

#### Docker Responds

```markdown
### 👤 Docker - DevOps

CI build failure detected:
```
Error: tests/test_example not found
make: *** [test] Error 1
```

Root cause analysis:
- .gitignore pattern: `tests/test_*`
- Intention: Ignore test binaries
- Problem: Also ignoring test source in subdirectories

Issue: Pattern `tests/test_*` is too broad.

Fix:
- Old: `tests/test_*`
- New: `tests/**/test_*` (matches executables in subdirs)
- Add: `!tests/**/*.c` (except .c files)
- Add: `!tests/**/*.h` (except .h files)
```

**Immediate action**:
```bash
git add .gitignore
git commit -m "fix: gitignore pattern for test binaries"
git push
```

**Verification**:
```bash
# Locally
git status  # Should show test sources
make clean && make test  # Should pass

# CI
# Wait for GitHub Actions run
# ✓ Build passes
```

**Report**:
```markdown
### 👤 Docker - DevOps

Emergency resolved.

**Issue**: .gitignore excluding test sources
**Fix**: Updated pattern to match executables only
**Commit**: b74df67
**CI Status**: ✅ Passing

Recommending addition to pre-commit hook: validate .gitignore patterns.
```

---

## 🎯 Key Takeaways from Examples

### 1. Clear Ownership
- Each agent knows their domain
- No overlap or confusion
- Handoffs are explicit

### 2. Atomic Commits
- Every commit has clear purpose
- Follows Conventional Commits
- Independently testable

### 3. Documentation First
- ADRs for important decisions
- README updates with features
- Tests are documentation

### 4. Proactive Communication
- Agents report progress
- Inter-agent coordination explicit
- User always informed

### 5. Quality Gates
- Sherlock validates everything
- Atlas checks DoD before completion
- Docker ensures builds pass

---

## 📋 Checklist for Your Project

Apply these patterns:

- [ ] Multi-step tasks → Atomic commits
- [ ] Framework decisions → ADR
- [ ] Build issues → Docker responds immediately
- [ ] Large features → Track A/B parallel execution
- [ ] User concerns → Appropriate agent responds

---

## 🔗 Related Files

- `REPLICATION_GUIDE.md` - Complete system blueprint
- `.agile/backlog.md` - Current tasks
- `.agile/adr/` - All architecture decisions

---

**End of Examples**
Use these as templates for your own multi-agent workflows.
