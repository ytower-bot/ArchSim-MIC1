# MIC-1 Simulator Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I include/
DEBUGFLAGS = -g -DDEBUG

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
TESTDIR = tests

# Target executables
TARGET = mic1_simulator
ASSEMBLER = mic1asm

# Source files (excluding main.c for library build)
ALL_SOURCES = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/utils/*.c)
EXCLUDED = $(SRCDIR)/memoryini.c $(SRCDIR)/memoryread.c $(SRCDIR)/mic1asm.c
SOURCES = $(filter-out $(EXCLUDED), $(ALL_SOURCES))
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Library objects (everything except main.c and mic1asm.c)
LIB_SOURCES = $(filter-out $(SRCDIR)/main.c $(SRCDIR)/mic1asm.c, $(SOURCES))
LIB_OBJECTS = $(LIB_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Test files
TEST_UNIT_SOURCES = $(wildcard $(TESTDIR)/unit/*.c)
TEST_INTEGRATION_SOURCES = $(wildcard $(TESTDIR)/integration/*.c)
TEST_SOURCES = $(TEST_UNIT_SOURCES) $(TEST_INTEGRATION_SOURCES)
TEST_BINS = $(TEST_UNIT_SOURCES:$(TESTDIR)/unit/%.c=$(TESTDIR)/unit/%) \
            $(TEST_INTEGRATION_SOURCES:$(TESTDIR)/integration/%.c=$(TESTDIR)/integration/%)

# Header files
HEADERS = $(wildcard $(INCDIR)/*.h)

# Default target
all: $(TARGET) $(ASSEMBLER)
	@echo "[OK] MIC-1 Simulator and Assembler compiled successfully"

# Create object directory if it doesn't exist
$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/utils

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link main executable
$(TARGET): $(OBJECTS)
	@echo "[LD] $@"
	@$(CC) $(OBJECTS) -o $(TARGET)

# Link assembler executable
$(ASSEMBLER): $(OBJDIR)/mic1asm.o $(LIB_OBJECTS)
	@echo "[LD] $@"
	@$(CC) $(OBJDIR)/mic1asm.o $(LIB_OBJECTS) -o $(ASSEMBLER)

# Create static library for CGO
$(OBJDIR)/libmic1.a: $(LIB_OBJECTS)
	@echo "[AR] $@"
	@ar rcs $@ $(LIB_OBJECTS)

# Debug build
debug: CFLAGS += $(DEBUGFLAGS)
debug: clean $(TARGET)

# Compile and run all tests
test: test-unit test-integration test-golden
	@echo ""
	@echo "=========================================="
	@echo "✓ All tests passed!"
	@echo "=========================================="

# Run unit tests only
test-unit: $(TEST_UNIT_SOURCES:$(TESTDIR)/unit/%.c=$(TESTDIR)/unit/%)
	@echo "=========================================="
	@echo "Running unit tests..."
	@echo "=========================================="
	@if [ -z "$(TEST_UNIT_SOURCES)" ]; then \
		echo "No unit tests found."; \
	else \
		for test in $^; do \
			echo ""; \
			echo ">>> Running $$test..."; \
			./$$test || exit 1; \
		done; \
	fi

# Run integration tests only
test-integration: $(TEST_INTEGRATION_SOURCES:$(TESTDIR)/integration/%.c=$(TESTDIR)/integration/%)
	@echo "=========================================="
	@echo "Running integration tests..."
	@echo "=========================================="
	@if [ -z "$(TEST_INTEGRATION_SOURCES)" ]; then \
		echo "No integration tests found."; \
	else \
		for test in $^; do \
			echo ""; \
			echo ">>> Running $$test..."; \
			./$$test || exit 1; \
		done; \
	fi

# Run golden file tests (assembler output validation)
test-golden: $(ASSEMBLER)
	@echo "=========================================="
	@echo "Running golden file tests..."
	@echo "=========================================="
	@if [ -f "$(TESTDIR)/golden/run_golden_tests.sh" ]; then \
		cd $(TESTDIR)/golden && ./run_golden_tests.sh; \
	else \
		echo "No golden tests found."; \
	fi

# Compile individual test binaries
$(TESTDIR)/unit/%: $(TESTDIR)/unit/%.c $(LIB_OBJECTS)
	@echo "[CC+LD] $@"
	@$(CC) $(CFLAGS) -I$(TESTDIR) $< $(LIB_OBJECTS) -o $@

$(TESTDIR)/integration/%: $(TESTDIR)/integration/%.c $(LIB_OBJECTS)
	@echo "[CC+LD] $@"
	@$(CC) $(CFLAGS) -I$(TESTDIR) $< $(LIB_OBJECTS) -o $@

# Run the simulator
run: $(TARGET)
	./$(TARGET)

# Clean object files and test binaries
clean:
	@rm -rf $(OBJDIR)
	@rm -f $(TEST_BINS)
	@find $(TESTDIR) -type f -executable -delete 2>/dev/null || true
	@echo "[CLEAN] Object files and test binaries removed"

# Clean everything
fclean: clean tui-clean
	@rm -f $(TARGET) $(ASSEMBLER)
	@echo "[CLEAN] All build artifacts removed"

# Rebuild everything
re: fclean all

# Install (copy to system path - optional)
install: $(TARGET)
	@cp $(TARGET) /usr/local/bin/
	@echo "[INSTALL] MIC-1 Simulator installed to /usr/local/bin/"

# Docker targets
docker-build:
	@echo "[DOCKER] Building image..."
	@docker build -t mic1-simulator .

docker-test: docker-build
	@echo "[DOCKER] Running tests..."
	@docker run --rm mic1-simulator make test

docker-shell: docker-build
	@echo "[DOCKER] Starting shell..."
	@docker run --rm -it mic1-simulator /bin/bash

# Show help
help:
	@echo "MIC-1 Simulator Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all            - Build simulator + assembler (default)"
	@echo "  debug          - Build with debug symbols"
	@echo "  test           - Build and run all tests (unit + integration + golden)"
	@echo "  test-unit      - Run unit tests only"
	@echo "  test-integration - Run integration tests only"
	@echo "  test-golden    - Run golden file tests (assembler validation)"
	@echo "  run            - Build and run simulator"
	@echo "  asm            - Assemble examples (examples/*.asm -> examples/*.bin)"
	@echo "  tui            - Build TUI"
	@echo "  tui-run        - Build and run TUI"
	@echo "  tui-clean      - Remove TUI binary"
	@echo "  clean          - Remove object files and test binaries"
	@echo "  fclean         - Remove all build artifacts"
	@echo "  re             - Rebuild everything"
	@echo "  install        - Install to system path"
	@echo "  docker-build   - Build Docker image"
	@echo "  docker-test    - Build and test in Docker"
	@echo "  docker-shell   - Start Docker shell"
	@echo "  help           - Show this help"

# Assemble all example programs
asm: $(ASSEMBLER)
	@echo "════════════════════════════════════════"
	@echo "  Assembling Example Programs"
	@echo "════════════════════════════════════════"
	@for file in examples/*.asm; do \
		if [ -f "$$file" ]; then \
			echo ""; \
			./$(ASSEMBLER) "$$file"; \
		fi \
	done
	@echo "════════════════════════════════════════"
	@echo "✓ All examples assembled!"
	@echo "════════════════════════════════════════"

# TUI targets
TUI_DIR = tui
TUI_BIN = $(TUI_DIR)/archsim-tui
GO = go

tui: $(OBJDIR)/libmic1.a
	@echo "[GO] Tidying TUI module..."
	@cd $(TUI_DIR) && $(GO) mod tidy
	@echo "[GO] Building TUI..."
	@cd $(TUI_DIR) && $(GO) build -o archsim-tui
	@echo "[OK] TUI built successfully: $(TUI_BIN)"

tui-run: tui
	@cd $(TUI_DIR) && ./archsim-tui

tui-testsp: $(OBJDIR)/libmic1.a
	@echo "[GO] Running TUI SP test (tagged)"
	@cd $(TUI_DIR) && $(GO) run -tags testsp .

tui-clean:
	@rm -f $(TUI_BIN)
	@echo "[CLEAN] TUI binary removed"

# Phony targets
.PHONY: all debug test test-unit test-integration test-golden run asm clean fclean re install docker-build docker-test docker-shell help tui tui-run tui-clean
