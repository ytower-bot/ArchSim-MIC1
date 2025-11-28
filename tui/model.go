package main

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/bubbles/viewport"
	tea "github.com/charmbracelet/bubbletea"
)

type model struct {
	// ViewPorts
	codeViewport  viewport.Model
	stateViewport viewport.Model

	// State
	cpuWrapper    *CPUWrapper
	cpu           CPUState
	sourceCode    []string
	currentLine   int
	loadedFile    string
	running       bool
	showHelp      bool

	// Layout
	width  int
	height int
}

type CPUState struct {
	PC         uint16
	AC         uint16
	SP         uint16
	IR         uint16
	TIR        uint16
	MPC        uint8
	Cycles     int
	Clock      int
	N          bool
	Z          bool
	Memory     map[uint16]uint16
	CacheStats CacheStats
}

type CacheStats struct {
	DataHits   int
	DataMisses int
	InstHits   int
	InstMisses int
}

func initialModel() model {
	wrapper := NewCPUWrapper()
	
	return model{
		cpuWrapper: wrapper,
		cpu: CPUState{
			PC:     0x0000,
			AC:     0x0000,
			SP:     0x0FFF,
			IR:     0x0000,
			TIR:    0x0000,
			MPC:    0x00,
			Cycles: 0,
			Clock:  0,
			Memory: make(map[uint16]uint16),
		},
		sourceCode:  []string{"No program loaded."},
		currentLine: 0,
		running:     false,
		showHelp:    false,
	}
}

func (m model) Init() tea.Cmd {
	return nil
}

func (m *model) syncCPUState() {
	m.cpu.PC = m.cpuWrapper.GetPC()
	m.cpu.AC = m.cpuWrapper.GetAC()
	m.cpu.SP = m.cpuWrapper.GetSP()
	m.cpu.IR = m.cpuWrapper.GetIR()
	m.cpu.TIR = m.cpuWrapper.GetTIR()
	m.cpu.MPC = m.cpuWrapper.GetMPC()
	m.cpu.Cycles = m.cpuWrapper.GetCycles()
	m.cpu.Clock = m.cpuWrapper.GetClock()
	m.cpu.N = m.cpuWrapper.GetFlagN()
	m.cpu.Z = m.cpuWrapper.GetFlagZ()
	m.cpu.CacheStats.DataHits = m.cpuWrapper.GetDataCacheHits()
	m.cpu.CacheStats.DataMisses = m.cpuWrapper.GetDataCacheMisses()
	m.cpu.CacheStats.InstHits = m.cpuWrapper.GetInstCacheHits()
	m.cpu.CacheStats.InstMisses = m.cpuWrapper.GetInstCacheMisses()
}

func (m model) getStatusLine() string {
	status := "Stopped"
	if m.running {
		status = "Running"
	}
	fileName := "No file"
	if m.loadedFile != "" {
		fileName = m.loadedFile
	}
	return fmt.Sprintf(" %s | %s | Cycle: %d", fileName, status, m.cpu.Cycles)
}

func (m model) getLeftPanelWidth() int {
	return m.width / 2
}

func (m model) getRightPanelWidth() int {
	return m.width - m.getLeftPanelWidth()
}

func (m model) renderSourceCode() string {
	var b strings.Builder
	b.WriteString("SOURCE CODE\n\n")

	visibleLines := m.height - 10
	start := 0
	if m.currentLine > visibleLines/2 {
		start = m.currentLine - visibleLines/2
	}
	end := start + visibleLines
	if end > len(m.sourceCode) {
		end = len(m.sourceCode)
	}

	for i := start; i < end; i++ {
		prefix := "  "
		if i == m.currentLine {
			prefix = "> "
		}
		b.WriteString(fmt.Sprintf("%s%3d | %s\n", prefix, i+1, m.sourceCode[i]))
	}

	return b.String()
}

func (m model) renderState() string {
	var b strings.Builder

	b.WriteString("CPU STATE\n\n")
	b.WriteString(fmt.Sprintf("Status: %s\n", map[bool]string{true: "Running", false: "Stopped"}[m.running]))
	b.WriteString(fmt.Sprintf("Cycles: %d\n", m.cpu.Cycles))
	b.WriteString(fmt.Sprintf("Clock:  %d\n\n", m.cpu.Clock))

	b.WriteString("REGISTERS\n\n")
	b.WriteString(fmt.Sprintf("PC:  0x%04X\n", m.cpu.PC))
	b.WriteString(fmt.Sprintf("AC:  0x%04X (%d)\n", m.cpu.AC, m.cpu.AC))
	b.WriteString(fmt.Sprintf("SP:  0x%04X\n", m.cpu.SP))
	b.WriteString(fmt.Sprintf("IR:  0x%04X\n", m.cpu.IR))
	b.WriteString(fmt.Sprintf("TIR: 0x%04X\n\n", m.cpu.TIR))

	b.WriteString("CONTROL UNIT\n\n")
	b.WriteString(fmt.Sprintf("MPC:  0x%02X\n", m.cpu.MPC))
	b.WriteString(fmt.Sprintf("N: %t  Z: %t\n\n", m.cpu.N, m.cpu.Z))

	b.WriteString("MEMORY\n\n")
	if len(m.cpu.Memory) == 0 {
		b.WriteString("Empty\n\n")
	} else {
		count := 0
		for addr := uint16(0x100); addr < 0x110 && count < 5; addr++ {
			if val, ok := m.cpu.Memory[addr]; ok {
				b.WriteString(fmt.Sprintf("0x%03X: 0x%04X (%d)\n", addr, val, val))
				count++
			}
		}
		b.WriteString("\n")
	}

	b.WriteString("CACHE STATS\n\n")
	dataTotal := m.cpu.CacheStats.DataHits + m.cpu.CacheStats.DataMisses
	instTotal := m.cpu.CacheStats.InstHits + m.cpu.CacheStats.InstMisses
	dataRate := 0.0
	instRate := 0.0
	if dataTotal > 0 {
		dataRate = float64(m.cpu.CacheStats.DataHits) / float64(dataTotal) * 100
	}
	if instTotal > 0 {
		instRate = float64(m.cpu.CacheStats.InstHits) / float64(instTotal) * 100
	}
	b.WriteString(fmt.Sprintf("Data: %.1f%% hit\n", dataRate))
	b.WriteString(fmt.Sprintf("Inst: %.1f%% hit\n", instRate))

	return b.String()
}

func (m model) renderHelp() string {
	return `
KEYBINDINGS

  s         Step one cycle
  r         Run/Pause
  x         Reset CPU
  l         Load file
  h/?       Toggle help
  q/ctrl+c  Quit

Press h to close this menu.
`
}
