
package main

import (
	"fmt"
	"os"

	tea "github.com/charmbracelet/bubbletea"
)

func main() {
	var filename string

	if len(os.Args) > 1 {
		filename = os.Args[1]

		if _, err := os.Stat(filename); os.IsNotExist(err) {
			fmt.Printf("Error: file not found: %s\n", filename)
			os.Exit(1)
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
}
