# MIC-1 Simulator Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I include/
DEBUGFLAGS = -g -DDEBUG

SRCDIR = src
INCDIR = include
OBJDIR = obj
TESTDIR = tests

TARGET = mic1_simulator
TUI = mic1_tui
ASSEMBLER = mic1asm

# Source files
ALL_SOURCES = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/utils/*.c)
EXCLUDED = $(SRCDIR)/memoryini.c $(SRCDIR)/memoryread.c $(SRCDIR)/mic1asm.c $(SRCDIR)/main_tui.c $(SRCDIR)/ui.c
SOURCES = $(filter-out $(EXCLUDED), $(ALL_SOURCES))
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Library objects (core without main files)
LIB_SOURCES = $(filter-out $(SRCDIR)/main.c $(SRCDIR)/main_tui.c $(SRCDIR)/mic1asm.c $(SRCDIR)/ui.c, $(ALL_SOURCES))
LIB_SOURCES := $(filter-out $(SRCDIR)/memoryini.c $(SRCDIR)/memoryread.c, $(LIB_SOURCES))
LIB_OBJECTS = $(LIB_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# TUI objects
TUI_SOURCES = $(SRCDIR)/main_tui.c $(SRCDIR)/ui.c
TUI_OBJECTS = $(OBJDIR)/main_tui.o $(OBJDIR)/ui.o

HEADERS = $(wildcard $(INCDIR)/*.h)

# === BUILD TARGETS ===

all: $(TARGET) $(ASSEMBLER)
	@echo "[OK] Build complete: $(TARGET), $(ASSEMBLER)"

full: all $(TUI)
	@echo "[OK] Full build: $(TARGET), $(ASSEMBLER), $(TUI)"

$(OBJDIR):
	@mkdir -p $(OBJDIR) $(OBJDIR)/utils

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	@echo "[LD] $@"
	@$(CC) $(OBJECTS) -o $@

$(ASSEMBLER): $(OBJDIR)/mic1asm.o $(LIB_OBJECTS)
	@echo "[LD] $@"
	@$(CC) $^ -o $@

# TUI build
$(TUI): $(TUI_OBJECTS) $(LIB_OBJECTS)
	@echo "[LD] $@"
	@$(CC) $^ -o $@

tui: $(TUI)
	@echo "[OK] TUI built: $(TUI)"

tui-run: $(TUI) $(ASSEMBLER)
	@if [ ! -f tests/demo.bin ]; then ./$(ASSEMBLER) tests/01_registers.asm tests/demo.bin; fi
	./$(TUI) tests/demo.bin

debug: CFLAGS += $(DEBUGFLAGS)
debug: clean all

# === TEST/VERIFY TARGETS ===

verify: $(ASSEMBLER) $(TARGET)
	@echo "=== MIC-1 Verification ==="
	@./$(ASSEMBLER) $(TESTDIR)/01_registers.asm $(TESTDIR)/verify.bin
	@./$(TARGET) $(TESTDIR)/verify.bin 100
	@rm -f $(TESTDIR)/verify.bin
	@echo "=== Verification PASSED ==="

ci-test: verify

# === CLEAN TARGETS ===

clean:
	@rm -rf $(OBJDIR)
	@find $(TESTDIR) -name "*.bin" -type f -delete 2>/dev/null || true
	@find $(TESTDIR) -name "*.dSYM" -type d -exec rm -rf {} + 2>/dev/null || true
	@echo "[CLEAN] Build artifacts removed"

fclean: clean
	@rm -f $(TARGET) $(ASSEMBLER) $(TUI)
	@echo "[CLEAN] All binaries removed"

re: fclean all

# === UTILITY TARGETS ===

asm: $(ASSEMBLER)
	@for f in examples/*.asm; do [ -f "$$f" ] && ./$(ASSEMBLER) "$$f"; done

run: $(TARGET)
	@echo "Usage: ./$(TARGET) <program.bin> [cycles]"

# === DOCKER TARGETS ===

DOCKER_IMAGE = archsim-mic1
DOCKER_TAG = latest

docker-build:
	@docker build -t $(DOCKER_IMAGE):$(DOCKER_TAG) .

docker-test: docker-build
	@docker run --rm $(DOCKER_IMAGE):$(DOCKER_TAG) make verify

docker-shell: docker-build
	@docker run --rm -it $(DOCKER_IMAGE):$(DOCKER_TAG) /bin/sh

docker-clean:
	@docker rmi $(DOCKER_IMAGE):$(DOCKER_TAG) 2>/dev/null || true

# === HELP ===

help:
	@echo "MIC-1 Simulator"
	@echo ""
	@echo "Build:"
	@echo "  make all      Build CLI simulator + assembler"
	@echo "  make tui      Build interactive TUI"
	@echo "  make full     Build everything"
	@echo "  make debug    Build with debug symbols"
	@echo ""
	@echo "Run:"
	@echo "  ./mic1asm <input.asm> [output.bin]"
	@echo "  ./mic1_simulator <program.bin> [cycles]"
	@echo "  ./mic1_tui <program.bin>"
	@echo ""
	@echo "  make tui-run  Build TUI and run with demo"
	@echo ""
	@echo "Test:"
	@echo "  make verify   Assemble + run test"
	@echo ""
	@echo "Clean:"
	@echo "  make clean    Remove objects"
	@echo "  make fclean   Remove all"
	@echo "  make re       Full rebuild"

.PHONY: all full tui tui-run debug verify ci-test clean fclean re asm run help
.PHONY: docker-build docker-test docker-shell docker-clean
