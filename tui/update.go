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
	m.cpuWrapper.Step()
	m.syncCPUState()
	
	// Update current line based on PC
	pcVal := int(m.cpu.PC)
	if pcVal/2 < len(m.sourceCode) {
		m.currentLine = pcVal / 2
	}
	
	return m
}

func (m model) handleReset() model {
	m.cpuWrapper.Reset()
	m.syncCPUState()
	m.currentLine = 0
	m.running = false
	return m
}

func (m model) handleLoad() model {
	// For now, load hardcoded example
	// TODO: Implement file picker
	filename := "../examples/sum.asm"
	
	// Try to assemble and load
	err := m.cpuWrapper.AssembleFile(filename)
	if err == nil {
		m.loadedFile = "examples/sum.asm"
		// Read the source file for display
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
		m.syncCPUState()
	}
	
	return m
}

func (m model) tickCmd() tea.Cmd {
	return tea.Tick(50*time.Millisecond, func(t time.Time) tea.Msg {
		return tickMsg{}
	})
}

type tickMsg struct{}
