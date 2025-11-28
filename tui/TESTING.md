# Testing Instructions

## Building

From project root:
```bash
make tui
```

Or from tui directory:
```bash
cd tui
go build -o archsim-tui
```

## Running

```bash
make tui-run
```

Or:
```bash
cd tui
./archsim-tui
```

## Testing Keybindings

Once the TUI is running:

1. Press `h` or `?` to open help menu
2. Press `h` again to close help
3. Press `l` to load example program (sum.asm simulation)
4. Press `s` to step through execution (one cycle at a time)
5. Press `r` to toggle run/pause (continuous execution)
6. Press `x` to reset CPU state
7. Press `q` or `Ctrl+C` to quit

## What You Should See

### Main View (Split Layout)

Left Panel:
- Source code with line numbers
- Current line highlighted with `>`
- Example assembly code

Right Panel:
- CPU State (Running/Stopped, Cycles, Clock)
- Registers (PC, AC, SP, IR, TIR)
- Control Unit (MPC, N, Z flags)
- Memory contents (0x100-0x10F)
- Cache statistics

Top:
- Title bar "MIC-1 SIMULATOR"

Bottom:
- Status line with filename, status, and cycle count
- Keybinding hints

### Help Menu

Overlay window with all keybindings listed.
Press `h` to close.

## Expected Behavior

- Step (`s`): Increments cycle, moves to next line, updates PC
- Run (`r`): Starts continuous execution at ~50ms per cycle
- Reset (`x`): Resets all state to initial values
- Load (`l`): Loads example sum.asm program with memory values

## Current Limitations

This is Phase 1 - simulation only:
- No actual C backend integration yet
- File picker not implemented (loads hardcoded example)
- Execution is simulated (not real MIC-1 cycles)

Phase 2 will add CGO integration with real MIC-1 execution.
