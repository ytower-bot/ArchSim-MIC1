package main

import (
	"flag"
	"fmt"
	"os"

	tea "github.com/charmbracelet/bubbletea"
)

func main() {
	var filename string
	var debug bool

	// Parse command-line flags
	flag.BoolVar(&debug, "debug", false, "Enable debug mode (logs to tui_debug.log)")
	flag.BoolVar(&debug, "d", false, "Enable debug mode (short flag)")
	flag.Parse()

	// Remaining arguments (non-flag arguments)
	args := flag.Args()
	if len(args) > 0 {
		filename = args[0]

		if _, err := os.Stat(filename); os.IsNotExist(err) {
			fmt.Printf("Error: file not found: %s\n", filename)
			os.Exit(1)
		}
	}

	// Initialize debug mode if requested
	var debugErr error
	if debug {
		if err := InitDebugMode(); err != nil {
			fmt.Printf("Warning: Failed to initialize debug mode: %v\n", err)
		} else {
			fmt.Println("Debug mode enabled. Logging to tui_debug.log")
			defer func() {
				if err := CloseDebugMode(); err != nil {
					debugErr = err
					fmt.Printf("\nDebug validation failed: %v\n", err)
					fmt.Println("Check tui_debug.log for details")
				}
			}()
		}
	}

	p := tea.NewProgram(
		initialModel(filename),
		tea.WithAltScreen(),
	)

	if _, err := p.Run(); err != nil {
		fmt.Printf("Error: %v\n", err)
		os.Exit(1)
	}

	if debugErr != nil {
		os.Exit(1)
	}
}
