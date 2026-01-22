# Infrastructure Guide

Single source of truth for building and running ArchSim-MIC1.

---

## Quick Start

```bash
# Clone and run (Docker)
git clone <repo-url>
cd ArchSim-MIC1
make docker-run

# Clone and run (Native)
git clone <repo-url>
cd ArchSim-MIC1
make tui-run
```

---

## Prerequisites

### Native Build

| Tool | Version | Check |
|------|---------|-------|
| GCC | 11+ | `gcc --version` |
| Go | 1.21+ | `go version` |
| Make | 3.8+ | `make --version` |

### Docker Build

| Tool | Version | Check |
|------|---------|-------|
| Docker | 20+ | `docker --version` |

Install Docker:
- Linux: `sudo apt-get install docker.io`
- macOS/Windows: [Docker Desktop](https://docs.docker.com/get-docker/)

---

## Make Targets

All project operations go through `make`. Run `make help` for full list.

### Building

```bash
make              # Build simulator + assembler
make tui          # Build TUI (Go)
make debug        # Build with debug symbols
make asm          # Assemble all examples/*.asm
```

### Running

```bash
make run          # Run C simulator
make tui-run      # Build and run TUI
```

### Testing

```bash
make test         # Run all tests
make test-unit    # Unit tests only
make test-golden  # Assembler validation
```

### Docker

```bash
make docker-build # Build image
make docker-run   # Build + run TUI in container
make docker-test  # Run tests in container
make docker-shell # Interactive shell in container
make docker-clean # Remove images/containers
```

### Maintenance

```bash
make clean        # Remove object files
make fclean       # Remove all artifacts
make re           # Full rebuild
```

---

## Docker Details

### Image Specifications

- Base: `golang:1.23-bookworm`
- Size: ~500MB
- Platform: `linux/amd64` (works on ARM via emulation)

### What `make docker-run` Does

1. Builds Docker image with all dependencies
2. Compiles C core + Go TUI inside container
3. Assembles example programs
4. Launches TUI with `test_stack.asm` loaded

### Custom Program

```bash
# Mount local file
docker run --rm -it \
  -v $(pwd)/myprogram.asm:/app/examples/myprogram.asm:ro \
  archsim-mic1:latest ../examples/myprogram.asm
```

### Development Mode

Mount source for live editing:

```bash
docker run --rm -it \
  -v $(pwd):/app \
  archsim-mic1:latest /bin/bash
```

---

## TUI Controls

| Key | Action |
|-----|--------|
| `s` | Step (execute 1 cycle) |
| `r` | Run/Pause (continuous execution) |
| `x` | Reset CPU |
| `l` | Load .asm file |
| `q` | Quit |

---

## Troubleshooting

### "docker: command not found"

Docker not installed. See Prerequisites section.

### "Cannot connect to Docker daemon"

```bash
# Linux
sudo systemctl start docker

# macOS/Windows
# Start Docker Desktop application
```

### "Permission denied" (Linux)

```bash
sudo usermod -aG docker $USER
# Logout and login again
```

### TUI doesn't appear

Ensure terminal supports TUI and you're using interactive mode:
```bash
docker run --rm -it archsim-mic1:latest  # -it is required
```

### Build fails with CGO errors

Ensure GCC is installed and CGO_ENABLED=1:
```bash
export CGO_ENABLED=1
make tui
```

---

## Project Structure

```
ArchSim-MIC1/
├── src/           # C source files
├── include/       # C headers
├── tui/           # Go TUI (Bubbletea)
├── tests/         # Test suites
├── examples/      # Assembly programs
├── data/          # Microcode files
├── docs/          # Documentation
├── Makefile       # Build system (single entry point)
├── Dockerfile     # Container definition
└── docker-compose.yml
```

---

## CI/CD

GitHub Actions workflow runs on push/PR:
- Build on Ubuntu and macOS
- Run all tests
- Validate assembler output

See `.github/workflows/build.yml` for details.
