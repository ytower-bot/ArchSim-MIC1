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

# Target executable
TARGET = mic1_simulator

# Source files
# Excluir arquivos de teste/exemplo que têm seu próprio main()
ALL_SOURCES = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/utils/*.c)
EXCLUDED = $(SRCDIR)/memoryini.c $(SRCDIR)/memoryread.c $(SRCDIR)/$(TESTDIR)/test_alu.c $(SRCDIR)/$(TESTDIR)/test_conversions.c
SOURCES = $(filter-out $(EXCLUDED), $(ALL_SOURCES))
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Header files
HEADERS = $(wildcard $(INCDIR)/*.h)

# Default target
all: $(TARGET)

# Create object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/utils

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)
	@echo "✅ MIC-1 Simulator compiled successfully!"

# Debug build
debug: CFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

# Test build (when test files exist)
test: $(TARGET)
	@echo "Running tests..."
	@if [ -d "$(TESTDIR)" ]; then \
		echo "Test directory found"; \
	else \
		echo "No tests found - create $(TESTDIR) directory for tests"; \
	fi

# Run the simulator
run: $(TARGET)
	./$(TARGET)

# Clean object files
clean:
	rm -rf $(OBJDIR)
	@echo "🧹 Object files cleaned"

# Clean everything
fclean: clean
	rm -f $(TARGET)
	@echo "🧹 All build files cleaned"

# Rebuild everything
re: fclean all

# Install (copy to system path - optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	@echo "📦 MIC-1 Simulator installed to /usr/local/bin/"

# Docker targets
docker-build:
	@echo "🐳 Building Docker image..."
	docker build -t mic1-simulator .

docker-test: docker-build
	@echo "🐳 Testing Docker image..."
	docker run --rm mic1-simulator

docker-shell: docker-build
	@echo "🐳 Starting Docker shell..."
	docker run --rm -it mic1-simulator /bin/bash

# Show help
help:
	@echo "MIC-1 Simulator Build System"
	@echo "Available targets:"
	@echo "  all          - Build the simulator (default)"
	@echo "  debug        - Build with debug symbols"
	@echo "  test         - Build and run tests"
	@echo "  run          - Build and run simulator"
	@echo "  clean        - Remove object files"
	@echo "  fclean       - Remove all build files"
	@echo "  re           - Rebuild everything"
	@echo "  install      - Install to system path"
	@echo "  docker-build - Build Docker image"
	@echo "  docker-test  - Build and test in Docker"
	@echo "  docker-shell - Start Docker shell"
	@echo "  help         - Show this help"

# Phony targets
.PHONY: all debug test run clean fclean re install docker-build docker-test docker-shell help
