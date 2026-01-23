package main

import (
	"fmt"
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

	switch msg.String() {
	case "q", "ctrl+c":
		return m, tea.Quit
	case "h", "?":
		m.showHelp = !m.showHelp
		return m, nil
	}

	if m.showHelp {
		return m, nil
	}

	if m.showFilePicker {
		switch msg.String() {
		case "esc", "l":
			m.showFilePicker = false
			return m, nil
		case "up", "k":
			if m.selectedFile > 0 {
				m.selectedFile--
			}
			return m, nil
		case "down", "j":
			if m.selectedFile < len(m.asmFiles)-1 {
				m.selectedFile++
			}
			return m, nil
		case "enter":
			if len(m.asmFiles) > 0 {

				filename := m.asmFiles[m.selectedFile]
				filepath := getAsmFilePath(filename)
				m.loadFile(filepath)
				m.showFilePicker = false
			}
			return m, nil
		}
		return m, nil
	}

	if m.showMemoryView {
		switch msg.String() {
		case "d":
			m.showMemoryView = false
			return m, nil
		}
		return m, nil
	}

	if m.showCacheView {
		switch msg.String() {
		case "c", "esc":
			m.showCacheView = false
			return m, nil
		}
		return m, nil
	}

	switch msg.String() {
	case "s":
		if m.loadedFile == "" {
			return m, nil // Don't step if no program is loaded
		}
		return m.handleStep(), nil
	case "r":
		if m.loadedFile == "" {
			return m, nil // Don't run if no program is loaded
		}
		m.running = !m.running
		if m.running {
			return m, m.tickCmd()
		}
		return m, nil
	case "x":
		return m.handleReset(), nil
	case "l":
		return m.handleLoad(), nil
	case "m":
		return m.handleLoadMicrocode(), nil
	case "v":
		m.showMicrocode = !m.showMicrocode
		return m, nil
	case "d":
		m.showMemoryView = !m.showMemoryView
		return m, nil

	case "c":
		m.showCacheView = !m.showCacheView
		return m, nil
	}

	return m, nil
}

func (m model) handleStep() model {
	m.cpuWrapper.Step()

	(&m).syncCPUState()

	// Use source line mapping instead of PC directly
	pcVal := m.cpu.PC
	sourceLine := m.cpuWrapper.GetSourceLine(pcVal)

	if sourceLine > 0 && sourceLine <= len(m.sourceCode) {
		m.currentLine = sourceLine - 1 // Convert 1-based to 0-based index
	}
	// If no mapping, keep currentLine unchanged

	return m
}

func (m model) handleReset() model {
	// Hard reset: reload the current file if one is loaded
	if m.loadedFile != "" {
		(&m).loadFile(m.loadedFile)
	} else {
		// If no file is loaded, just do a soft reset
		m.cpuWrapper.Reset()
		(&m).syncCPUState()
		m.microcodePath = GetMicrocodePath()
		m.currentLine = 0
	}

	m.running = false
	m.errorMsg = ""
	return m
}

func (m model) handleLoad() model {

	m.asmFiles = findAsmFiles()
	m.selectedFile = 0
	m.showFilePicker = true
	return m
}

func (m model) handleLoadMicrocode() model {

	filename := getDefaultMicrocodePath()
	err := LoadMicrocode(filename)

	if err != nil {
		m.errorMsg = fmt.Sprintf("Failed to load microcode: %v", err)
	} else {
		m.microcodePath = GetMicrocodePath()
		m.errorMsg = "Microcode reloaded successfully"
	}
	return m
}

func (m model) tickCmd() tea.Cmd {
	return tea.Tick(50*time.Millisecond, func(t time.Time) tea.Msg {
		return tickMsg{}
	})
}

type tickMsg struct{}
