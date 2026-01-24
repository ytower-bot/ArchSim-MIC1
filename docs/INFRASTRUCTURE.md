# Infrastructure Guide

Build and run guide for ArchSim-MIC1 (Headless Backend).

---

## Quick Start

```bash
# Native
make all
./mic1asm tests/01_registers.asm test.bin
./mic1_simulator test.bin 100

# Docker
make docker-test
```

---

## Prerequisites

### Native Build

| Tool | Version | Check |
|------|---------|-------|
| GCC | 11+ | `gcc --version` |
| Make | 3.8+ | `make --version` |

### Docker Build

| Tool | Version | Check |
|------|---------|-------|
| Docker | 20+ | `docker --version` |

---

## Make Targets

Run `make help` for full list.

### Building

```bash
make all      # Build simulator + assembler
make debug    # Build with debug symbols
make asm      # Assemble all examples/*.asm
```

### Testing

```bash
make verify   # Assemble test + run simulator
make ci-test  # Same as verify (for CI)
```

### Docker

```bash
make docker-build   # Build image
make docker-test    # Run verify in container
make docker-shell   # Interactive shell
make docker-clean   # Remove image
```

### Maintenance

```bash
make clean    # Remove object files
make fclean   # Remove all artifacts
make re       # Full rebuild
```

---

## Docker Details

### Image Specifications

- Base: `alpine:3.19`
- Size: ~50MB
- Contains: gcc, musl-dev, make

### Running Custom Programs

```bash
docker run --rm -v $(pwd)/myprogram.asm:/app/test.asm \
  archsim-mic1:latest sh -c \
  "./mic1asm test.asm test.bin && ./mic1_simulator test.bin 500"
```

---

## CI/CD

GitHub Actions workflow (`.github/workflows/build.yml`):

1. Checkout code
2. `make all` - Build binaries
3. `make ci-test` - Assemble test program and run simulator

Passes if simulator exits with code 0.

---

## Project Structure

```
ArchSim-MIC1/
├── src/           # C source files
├── include/       # C headers
├── tests/         # Test programs (.asm)
├── examples/      # Example programs
├── docs/          # Documentation
├── Makefile       # Build system
└── Dockerfile     # Container (Alpine + GCC)
```
