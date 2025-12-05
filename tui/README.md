# ArchSim MIC-1 TUI

Terminal User Interface for the MIC-1 Simulator.

## Building

From the project root:

```bash
make tui
```

Or manually:

```bash
cd tui
go build -o archsim-tui
```

## Running

Load a program directly:

```bash
./tui/archsim-tui examples/sum.asm
```

Or start without a program and press `l` to load the default example:

```bash
./tui/archsim-tui
```

## Interface Layout

The TUI is divided into two panels:

**Left Panel - Source Code**
- Displays the loaded assembly program
- Current instruction is marked with `->`
- Line numbers for reference

**Right Panel - CPU State**
- Execution status (RUNNING/STOPPED)
- Cycle and clock counters
- Register values (PC, AC, SP, IR, TIR)
- Control unit state (MPC, flags N and Z)
- Memory contents at relevant addresses
- Cache statistics (hit rates)

## Keybindings

- `s` - Step one cycle
- `r` - Run/Pause continuous execution
- `x` - Reset CPU to initial state
- `l` - Load default example (examples/sum.asm)
- `h` or `?` - Toggle help menu
- `q` or `Ctrl+C` - Quit

## Requirements

- Go 1.24+
- Terminal with ANSI support
- C backend compiled (libmic1.a)

## Technical Notes

### Memory Management (ARM64 Compatibility)

The TUI uses heap allocation for the MIC-1 CPU structure to ensure compatibility with ARM64 architectures (Apple Silicon). The CPU structure (~294 KB) is allocated dynamically via `malloc()` to avoid SIGBUS errors that occur with large static allocations in CGO on ARM64 macOS.

**Implementation Details:**
- CPU structure allocated on first `cgo_init_cpu()` call
- Automatic cleanup on program exit
- NULL safety checks in all CGO wrapper functions
- Total size: 301,560 bytes (256 KB memory + 32 KB control memory + datapath)

This approach ensures the TUI works seamlessly on both x86_64 and ARM64 platforms.

## Usage Example

```bash
# Compile everything
make all

# Build TUI
make tui

# Run with a program
./tui/archsim-tui examples/maximum.asm

# Inside TUI:
# - Press 's' to step through instructions
# - Press 'r' to run continuously
# - Press 'h' for help
```
