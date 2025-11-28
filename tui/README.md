# ArchSim MIC-1 TUI

Terminal User Interface for the MIC-1 Simulator.

## Building

```bash
cd tui
go build -o archsim-tui
```

## Running

```bash
./archsim-tui
```

## Keybindings

- `s` - Step one cycle
- `r` - Run/Pause execution
- `x` - Reset CPU
- `l` - Load assembly file
- `h` or `?` - Toggle help menu
- `q` or `Ctrl+C` - Quit

## Requirements

- Go 1.24+
- Terminal with ANSI support
