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
