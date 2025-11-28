#!/bin/bash
# Test script for TUI

echo "Building TUI..."
make tui

if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful!"
    echo ""
    echo "To run the TUI:"
    echo "  cd tui && ./archsim-tui"
    echo ""
    echo "Or use:"
    echo "  make tui-run"
    echo ""
    echo "Press any key to launch TUI..."
    read -n 1 -s
    cd tui && ./archsim-tui
else
    echo "Build failed!"
    exit 1
fi
