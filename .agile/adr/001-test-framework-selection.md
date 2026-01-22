# ADR-001: Test Framework Selection

**Status**: Accepted
**Date**: 2026-01-22
**Deciders**: Atlas (Tech Lead), Sherlock (QA), C-3PO (Core Engineer)
**Technical Story**: [TASK-001] Create tests/ Directory Structure

---

## Context

The ArchSim-MIC1 project had no automated testing infrastructure. We needed to select a test framework that:
1. Works with C99 (project standard)
2. Is appropriate for an educational project
3. Enables testing of core components (ALU, memory, assembler)
4. Supports CI/CD integration
5. Has minimal barriers to entry for students/contributors

The primary constraint: **This is an educational project**. Students should be able to read, understand, and extend the test framework without needing to learn a complex third-party testing library.

---

## Decision

**We chose to implement a custom Pure C test framework ("MIC1-Test")** rather than adopting an external framework.

---

## Options Considered

### Option 1: Unity Test Framework
- **Pros**:
  - Lightweight (~1000 LOC)
  - C99 compliant
  - Industry-proven
  - Good documentation
  - Color output support
- **Cons**:
  - External dependency (git submodule or manual download)
  - Students must learn Unity-specific macros
  - Adds complexity to build system
  - Not immediately transparent (source is in another repo)

### Option 2: Check Framework
- **Pros**:
  - Mature and stable
  - Fork-based test isolation
  - Widely used in open-source projects
- **Cons**:
  - Requires installation (apt/brew)
  - C99 support varies by version
  - Heavyweight for educational project
  - Complex setup for beginners

### Option 3: Google Test (gtest)
- **Pros**:
  - Industry standard
  - Rich feature set
  - Excellent CI/CD integration
- **Cons**:
  - **C++ framework** (not C)
  - Massive dependency (entire Google test suite)
  - Overkill for this project
  - Students must understand C++ to extend tests

### Option 4: Pure C Custom Framework (CHOSEN)
- **Pros**:
  - **Zero external dependencies** (self-contained in `tests/test_framework.h`)
  - Students can read the entire framework (150 LOC)
  - C99 compliant by design
  - Fully customizable for project needs
  - No installation required
  - Transparent implementation (macros expand to readable C)
  - Educational value: students learn how test frameworks work
- **Cons**:
  - We maintain it (but it's only 150 LOC)
  - Lacks advanced features (mocking, fixtures, parameterized tests)
  - No external community support

---

## Rationale

**Key Decision Factors**:

1. **Educational Transparency**: This is a Tanenbaum-based educational project. Students should be able to understand every line of code, including the test framework. A 150-line header file is far more approachable than a multi-file external framework.

2. **Zero Friction**: With a custom framework, students can:
   - Clone the repo
   - Run `make test`
   - Start writing tests immediately

   No need to install Unity, no git submodules, no package managers.

3. **Project Scope**: We're testing:
   - ALU operations (4 operations, 2 flags)
   - Memory/cache interactions
   - Assembler parsing

   We don't need mock objects, fixtures, or parameterized tests. Simple assertions are sufficient.

4. **C99 Compliance**: Our framework uses only C99 features, matching the project's existing standard (`CFLAGS = -std=c99`).

5. **Maintainability**: The framework is <200 LOC. If we need to add features (e.g., `ASSERT_FLOAT_EQUAL`), it's trivial to extend.

---

## Consequences

**Positive**:
- ✅ Students can read and understand `test_framework.h` in 5 minutes
- ✅ No build system changes required (just `#include "../test_framework.h"`)
- ✅ Tests compile with the same flags as production code
- ✅ Color-coded output improves test readability
- ✅ Easy to extend (add new assertion macros as needed)
- ✅ CI/CD integration is trivial (just run the binary)

**Negative**:
- ⚠️ We own the framework (but it's stable - test frameworks don't change often)
- ⚠️ Lacks advanced features like:
  - Test fixtures (setup/teardown per test)
  - Parameterized tests
  - Mock object support
  - Test discovery (we manually register tests)

  **Mitigation**: These features aren't needed for our use case. If they become necessary, we can add them incrementally.

**Neutral**:
- Framework is specific to this project (not reusable elsewhere without copy-paste)
- Contributors must learn our macro conventions (but they're simple: `TEST_CASE`, `ASSERT_EQUAL`, etc.)

---

## Validation

**Success Criteria**:
1. ✅ All existing tests compile and pass (`make test`)
2. ✅ New contributors can write a test in <10 minutes
3. ✅ Test output is clear and actionable (shows file, line, expected vs. actual)
4. ✅ CI/CD pipeline runs tests successfully

**Metrics to Monitor**:
- Time for new contributors to write their first test (target: <10 min)
- Number of bugs caught by tests (target: >80% of regressions)
- Test suite execution time (target: <5 seconds for all tests)

**Review Date**: After Sprint 2 (once we have 10+ test files)
- If the framework becomes a bottleneck, we'll revisit Unity or Check
- If students struggle to use it, we'll improve documentation

---

## Implementation Details

**Framework Location**: `tests/test_framework.h` (single header, 150 LOC)

**Core Macros**:
```c
TEST_CASE(name)              // Define a test
RUN_TEST(name)               // Execute a test
TEST_SUITE_BEGIN(suite_name) // Start test suite
TEST_SUITE_END()             // End test suite and return exit code
```

**Assertion Macros**:
- `ASSERT_TRUE`, `ASSERT_FALSE`
- `ASSERT_EQUAL`, `ASSERT_NOT_EQUAL`
- `ASSERT_STR_EQUAL`
- `ASSERT_NULL`, `ASSERT_NOT_NULL`

**Build Integration**: Modified `Makefile` to support:
- `make test-unit` - Run unit tests
- `make test-integration` - Run integration tests
- `make test-golden` - Run assembler golden file tests
- `make test` - Run all tests

---

## References

- [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity) - Considered but rejected
- [Check Framework](https://libcheck.github.io/check/) - Considered but rejected
- `tests/test_framework.h:1` - Our implementation
- `tests/README.md` - Testing strategy documentation
- Tenenbaum, "Structured Computer Organization" (6th Ed.) - Project specification

---

## Related ADRs

- None (this is the first ADR)

---

**Decision Made By**: Atlas, Sherlock, C-3PO
**Approved**: 2026-01-22
**Next Review**: After Sprint 2
