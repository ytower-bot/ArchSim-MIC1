package main

import (
	"time"

	tea "github.com/charmbracelet/bubbletea"
)

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		return m.handleKeyPress(msg)
	case tea.WindowSizeMsg:
		m.width = msg.Width
		m.height = msg.Height
		return m, nil
	case tickMsg:
		if m.running {
			m = m.handleStep()
			return m, m.tickCmd()
		}
		return m, nil
	}
	return m, nil
}

func (m model) handleKeyPress(msg tea.KeyMsg) (tea.Model, tea.Cmd) {
	// Global keys
	switch msg.String() {
	case "q", "ctrl+c":
		return m, tea.Quit
	case "h", "?":
		m.showHelp = !m.showHelp
		return m, nil
	}

	// If help is shown, only handle toggle
	if m.showHelp {
		return m, nil
	}

	// Main controls
	switch msg.String() {
	case "s":
		return m.handleStep(), nil
	case "r":
		m.running = !m.running
		if m.running {
			return m, m.tickCmd()
		}
		return m, nil
	case "x":
		return m.handleReset(), nil
	case "l":
		return m.handleLoad(), nil
	}

	return m, nil
}

func (m model) handleStep() model {
	// TODO: Call CGO step function
	m.cpu.Cycles++
	m.cpu.Clock++
	if m.currentLine < len(m.sourceCode)-1 {
		m.currentLine++
	}
	// Simulate some execution
	m.cpu.PC += 2
	return m
}

func (m model) handleReset() model {
	m.cpu = CPUState{
		PC:     0x0000,
		AC:     0x0000,
		SP:     0x0FFF,
		IR:     0x0000,
		TIR:    0x0000,
		MPC:    0x00,
		Cycles: 0,
		Clock:  0,
		Memory: make(map[uint16]uint16),
	}
	m.currentLine = 0
	m.running = false
	return m
}

func (m model) handleLoad() model {
	// TODO: Implement file picker
	// For now, load example
	m.loadedFile = "examples/sum.asm"
	m.sourceCode = []string{
		"; Sum example",
		"START:  LOCO 10",
		"        STOD 100",
		"        LOCO 20",
		"        STOD 101",
		"        LODD 100",
		"        ADDD 101",
		"        STOD 102",
		"        HALT",
	}
	m.currentLine = 0
	m.cpu.Memory[0x100] = 10
	m.cpu.Memory[0x101] = 20
	m.cpu.Memory[0x102] = 30
	return m
}

func (m model) tickCmd() tea.Cmd {
	return tea.Tick(50*time.Millisecond, func(t time.Time) tea.Msg {
		return tickMsg{}
	})
}

type tickMsg struct{}
