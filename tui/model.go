package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"

	"github.com/charmbracelet/bubbles/viewport"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
)

type model struct {

	codeViewport  viewport.Model
	stateViewport viewport.Model

	cpuWrapper    *CPUWrapper
	cpu           CPUState
	sourceCode    []string
	currentLine   int
	loadedFile    string
	microcodePath string
	running       bool
	showHelp      bool
	showMicrocode bool
	errorMsg      string

	memoryFormat    string
	showMemoryView  bool
	showResultsView bool
	showCacheView   bool
	memorySnapshot  map[uint16]uint16

	showFilePicker bool
	asmFiles       []string
	selectedFile   int

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
	Hits   int
	Misses int
}

func initialModel(filename string) model {
	wrapper := NewCPUWrapper()

	microcodePath := getDefaultMicrocodePath()
	if err := LoadMicrocode(microcodePath); err != nil {

		fmt.Fprintf(os.Stderr, "Warning: Could not load microcode: %v\n", err)
	}

	m := model{
		cpuWrapper:    wrapper,
		microcodePath: GetMicrocodePath(),
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
		sourceCode:      []string{"No program loaded."},
		currentLine:     0,
		running:         false,
		showHelp:        false,
		showMicrocode:   false,
		memoryFormat:    "hex",
		showMemoryView:  false,
		showResultsView: false,
		showCacheView:   false,
		memorySnapshot:  make(map[uint16]uint16),
		showFilePicker:  false,
		asmFiles:        []string{},
		selectedFile:    0,
	}

	m.syncCPUState()

	if filename != "" {
		m.loadFile(filename)
	}

	return m
}

func (m model) Init() tea.Cmd {
	return nil
}

func (m *model) syncCPUState() {
	// Capture C core state BEFORE updating TUI state
	coreState := CPUState{
		PC:     m.cpuWrapper.GetPC(),
		AC:     m.cpuWrapper.GetAC(),
		SP:     m.cpuWrapper.GetSP(),
		IR:     m.cpuWrapper.GetIR(),
		TIR:    m.cpuWrapper.GetTIR(),
		MPC:    m.cpuWrapper.GetMPC(),
		Cycles: m.cpuWrapper.GetCycles(),
		Clock:  m.cpuWrapper.GetClock(),
		N:      m.cpuWrapper.GetFlagN(),
		Z:      m.cpuWrapper.GetFlagZ(),
		CacheStats: CacheStats{
			Hits:   m.cpuWrapper.GetCacheHits(),
			Misses: m.cpuWrapper.GetCacheMisses(),
		},
	}

	// Update TUI state from C core
	m.cpu.PC = coreState.PC
	m.cpu.AC = coreState.AC
	m.cpu.SP = coreState.SP
	m.cpu.IR = coreState.IR
	m.cpu.TIR = coreState.TIR
	m.cpu.MPC = coreState.MPC
	m.cpu.Cycles = coreState.Cycles
	m.cpu.Clock = coreState.Clock
	m.cpu.N = coreState.N
	m.cpu.Z = coreState.Z
	m.cpu.CacheStats.Hits = coreState.CacheStats.Hits
	m.cpu.CacheStats.Misses = coreState.CacheStats.Misses

	// Debug: Log assembly execution
	if debugEnabled {
		DebugAssemblyExecution(m.cpu.Cycles, m.cpu.PC, m.sourceCode, m.currentLine)
	}

	// Debug: Log state comparison
	if debugEnabled {
		DebugCPUState(m.cpu.Cycles, coreState, m.cpu)
		DebugCacheContent(m.cpu.Cycles, m.cpuWrapper)
	}

	m.updateMemorySnapshot()

	// Debug: Log memory comparison
	if debugEnabled && len(m.memorySnapshot) > 0 {
		// Read memory from C core for comparison
		coreMemory := make(map[uint16]uint16)
		addresses := make([]uint16, 0, len(m.memorySnapshot))
		for addr := range m.memorySnapshot {
			addresses = append(addresses, addr)
			coreMemory[addr] = m.cpuWrapper.ReadMemory(addr)
		}
		DebugMemoryState(m.cpu.Cycles, addresses, coreMemory, m.memorySnapshot)
	}
}

func (m *model) updateMemorySnapshot() {

	regions := []struct {
		start uint16
		end   uint16
	}{
		{0x0000, 0x00FF},
		{0x0100, 0x01FF},
		{m.cpu.PC & 0xFFF0, (m.cpu.PC & 0xFFF0) + 0x0010},
		{m.cpu.SP & 0xFFF0, (m.cpu.SP & 0xFFF0) + 0x0010},
	}

	for _, region := range regions {
		for addr := region.start; addr <= region.end && addr < 0x1000; addr++ {
			val := m.cpuWrapper.ReadMemory(addr)
			if val != 0 || m.memorySnapshot[addr] != 0 {
				m.memorySnapshot[addr] = val
			}
		}
	}
}

func (m model) formatValue(val uint16) string {
	switch m.memoryFormat {
	case "hex":
		return fmt.Sprintf("0x%04X", val)
	case "decimal":
		return fmt.Sprintf("%d", val)
	case "ascii":
		if val >= 32 && val <= 126 {
			return fmt.Sprintf("'%c' (0x%04X)", val, val)
		}
		return fmt.Sprintf("0x%04X", val)
	default:
		return fmt.Sprintf("0x%04X", val)
	}
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

	line := fmt.Sprintf(" %s | %s | Cycle: %d", fileName, status, m.cpu.Cycles)
	if m.errorMsg != "" {
		line += fmt.Sprintf(" | ERROR: %s", m.errorMsg)
	}
	return line
}

func (m *model) loadFile(filename string) {

	file, err := os.Open(filename)
	if err != nil {
		m.errorMsg = fmt.Sprintf("Cannot open file: %v", err)
		return
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	m.sourceCode = []string{}
	for scanner.Scan() {
		m.sourceCode = append(m.sourceCode, scanner.Text())
	}

	if err := scanner.Err(); err != nil {
		m.errorMsg = fmt.Sprintf("Cannot read file: %v", err)
		return
	}

	err = m.cpuWrapper.AssembleFile(filename)
	if err != nil {
		m.errorMsg = fmt.Sprintf("Assembly failed: %v", err)
		return
	}

	m.cpuWrapper.Reset()

	m.loadedFile = filename
	m.currentLine = 0
	m.errorMsg = ""
	m.syncCPUState()
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

	if m.height < 12 {
		b.WriteString("Window too small\n")
		return b.String()
	}

	if len(m.sourceCode) == 0 {
		b.WriteString("No code loaded\n")
		return b.String()
	}

	visibleLines := m.height - 10
	if visibleLines < 1 {
		visibleLines = 1
	}

	start := 0
	if m.currentLine > visibleLines/2 {
		start = m.currentLine - visibleLines/2
	}

	if start < 0 {
		start = 0
	}
	if start >= len(m.sourceCode) {
		start = len(m.sourceCode) - 1
		if start < 0 {
			start = 0
		}
	}

	end := start + visibleLines
	if end > len(m.sourceCode) {
		end = len(m.sourceCode)
	}

	for i := start; i < end; i++ {
		prefix := "  "
		if i == m.currentLine {
			prefix = "->"
		}
		line := m.sourceCode[i]
		maxWidth := 50
		if len(line) > maxWidth {
			line = line[:maxWidth-3] + "..."
		}
		b.WriteString(fmt.Sprintf("%s %3d | %s\n", prefix, i+1, line))
	}

	return b.String()
}

func (m model) renderState() string {
	var b strings.Builder

	availableHeight := m.height - 10
	if availableHeight < 20 {
		b.WriteString("Window too small\n")
		return b.String()
	}

	b.WriteString("CPU STATE\n\n")
	status := "STOPPED"
	if m.running {
		status = "RUNNING"
	}
	b.WriteString(fmt.Sprintf("Status: %s\n", status))
	b.WriteString(fmt.Sprintf("Cycles: %d\n", m.cpu.Cycles))
	b.WriteString(fmt.Sprintf("Clock:  %d\n\n", m.cpu.Clock))

	b.WriteString("REGISTERS\n\n")
	b.WriteString(fmt.Sprintf("PC:  0x%04X\n", m.cpu.PC))
	b.WriteString(fmt.Sprintf("AC:  0x%04X\n", m.cpu.AC))
	b.WriteString(fmt.Sprintf("SP:  0x%04X\n", m.cpu.SP))
	b.WriteString(fmt.Sprintf("IR:  0x%04X\n", m.cpu.IR))
	b.WriteString(fmt.Sprintf("TIR: 0x%04X\n\n", m.cpu.TIR))

	b.WriteString("CONTROL UNIT\n\n")
	b.WriteString(fmt.Sprintf("MPC:  0x%02X\n", m.cpu.MPC))
	b.WriteString(fmt.Sprintf("N: %t  Z: %t\n\n", m.cpu.N, m.cpu.Z))

	b.WriteString("MEMORY\n\n")

	pcAddr := m.cpu.PC
	spAddr := m.cpu.SP

	pcVal := m.cpuWrapper.ReadMemory(pcAddr)
	b.WriteString(fmt.Sprintf("@PC  [0x%04X]: 0x%04X\n", pcAddr, pcVal))

	spVal := m.cpuWrapper.ReadMemory(spAddr)
	b.WriteString(fmt.Sprintf("@SP  [0x%04X]: 0x%04X\n\n", spAddr, spVal))

	b.WriteString("CACHE\n\n")
	total := m.cpu.CacheStats.Hits + m.cpu.CacheStats.Misses
	hitRate := 0.0
	if total > 0 {
		hitRate = float64(m.cpu.CacheStats.Hits) / float64(total) * 100
	}
	b.WriteString(fmt.Sprintf("Hits: %d\n", m.cpu.CacheStats.Hits))
	b.WriteString(fmt.Sprintf("Misses: %d\n", m.cpu.CacheStats.Misses))
	b.WriteString(fmt.Sprintf("Hit Rate: %.1f%%\n", hitRate))

	return b.String()
}

func (m model) renderHelp() string {
	return `
KEYBINDINGS

EXECUTION CONTROL
  s         Step one cycle
  r         Run/Pause
  x         Reset CPU
  l         Load ASM file
  m         Load microcode
  v         View microcode

DISPLAY OVERLAYS
  t         Results view (0x100-0x10F)
  c         Cache view (statistics + lines)
  d         Detailed memory view
  h/?       Toggle this help

OTHER
  q/ctrl+c  Quit
  esc       Close any overlay

NOTE: In detailed memory view (d), use 'f' to cycle
      between hex, decimal, and ASCII formats.

MEMORY LEGEND
  *         Value changed since last cycle
  >XX<      PC location
  ^XX^      SP location

ARCHITECTURE OVERVIEW
This TUI simulates the MIC-1 architecture at the
microprogramming level. The microprogram interprets
assembly instructions.

Components:
- Assembler: Converts .asm → binary
- Microcode: Interprets instructions
- Datapath: Registers, ALU, Shifter
- Control Unit: MPC, MIR, Decoder
- Memory: 4096 words (0x0000-0x0FFF)
- Cache: Unified, 8 lines × 4 words

Press h to close this menu.
`
}

func (m model) renderMemoryView() string {

	overlayWidth := m.width - 10
	if overlayWidth < 40 {
		overlayWidth = 40
	}
	if overlayWidth > m.width {
		overlayWidth = m.width - 2
	}

	overlayHeight := m.height - 10
	if overlayHeight < 10 {
		overlayHeight = 10
	}
	if overlayHeight > m.height {
		overlayHeight = m.height - 2
	}

	memoryStyle := lipgloss.NewStyle().
		BorderStyle(lipgloss.RoundedBorder()).
		BorderForeground(lipgloss.Color("#AA00AA")).
		Padding(1, 2).
		Width(overlayWidth).
		Height(overlayHeight)

	memory := memoryStyle.Render(m.renderMemoryDetailed())

	return lipgloss.Place(
		m.width,
		m.height,
		lipgloss.Center,
		lipgloss.Center,
		memory,
		lipgloss.WithWhitespaceChars(" "),
		lipgloss.WithWhitespaceForeground(lipgloss.Color("#1a1a1a")),
	)
}

func (m model) renderMemoryDetailed() string {
	var b strings.Builder
	b.WriteString("DETAILED MEMORY VIEW [HEX]\n\n")

	stackStart := m.cpu.SP
	if stackStart > 8 {
		stackStart = m.cpu.SP - 8
	} else {
		stackStart = 0
	}

	stackEnd := m.cpu.SP + 8
	if stackEnd >= 0x1000 {
		stackEnd = 0x0FFF
	}

	regions := []struct {
		name  string
		start uint16
		end   uint16
	}{
		{"Code Region", 0x0000, 0x001F},
		{"Data Region", 0x0080, 0x008F},
		{"Stack Region", stackStart, stackEnd},
	}

	for _, region := range regions {
		b.WriteString(fmt.Sprintf("=== %s (0x%04X - 0x%04X) ===\n\n", region.name, region.start, region.end))

		for addr := region.start; addr <= region.end && addr < 0x1000; {
			b.WriteString(fmt.Sprintf("0x%04X:  ", addr))

			for col := 0; col < 4 && addr <= region.end && addr < 0x1000; col++ {
				val := m.cpuWrapper.ReadMemory(addr)

				prefix := " "
				suffix := ""
				if addr == m.cpu.PC {
					prefix = ">"
					suffix = "<"
				} else if addr == m.cpu.SP {
					prefix = "^"
					suffix = "^"
				}

				oldVal, existed := m.memorySnapshot[addr]
				if existed && oldVal != val {
					suffix += "*"
				}

				// Always use hex format for memory dump
				b.WriteString(fmt.Sprintf("%s0x%04X%s ", prefix, val, suffix))
				addr++
			}
			b.WriteString("\n")
		}
		b.WriteString("\n")
	}

	b.WriteString("Legend: >PC< ^SP^ *Changed\n")
	b.WriteString("Press 'd' to close\n")

	return b.String()
}

func (m model) renderResultsDetailed() string {
	var b strings.Builder
	b.WriteString("RESULTS VIEW - Program Output\n\n")
	b.WriteString("Memory 0x100-0x10F (use STOD to write here)\n")
	b.WriteString("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n")

	nonZeroCount := 0
	for addr := uint16(0x100); addr <= 0x10F; addr++ {
		val := m.cpuWrapper.ReadMemory(addr)

		// Check if value changed
		oldVal, existed := m.memorySnapshot[addr]
		changed := existed && oldVal != val

		// Format address
		addrStr := fmt.Sprintf("[0x%03X]:", addr)

		// Format value - always hex primary, decimal secondary
		valStr := fmt.Sprintf("0x%04X", val)

		// Add decimal interpretation
		decStr := fmt.Sprintf("(%d)", int16(val))

		// Build line with change indicator
		if changed {
			// Show change with arrow indicator
			b.WriteString(fmt.Sprintf("%s %s %8s  ◀ CHANGED\n", addrStr, valStr, decStr))
		} else if val != 0 {
			b.WriteString(fmt.Sprintf("%s %s %8s\n", addrStr, valStr, decStr))
			nonZeroCount++
		} else {
			b.WriteString(fmt.Sprintf("%s %s %8s\n", addrStr, valStr, decStr))
		}
	}

	b.WriteString("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n")
	b.WriteString(fmt.Sprintf("Non-zero values: %d | Cycle: %d\n", nonZeroCount, m.cpu.Cycles))
	b.WriteString("\n◀ CHANGED = Value modified this cycle\n")
	b.WriteString("Press 't' to close\n")

	return b.String()
}

func (m model) renderCacheDetailed() string {
	var b strings.Builder
	b.WriteString("CACHE VIEW\n\n")

	total := m.cpu.CacheStats.Hits + m.cpu.CacheStats.Misses
	hitRate := 0.0
	if total > 0 {
		hitRate = float64(m.cpu.CacheStats.Hits) / float64(total) * 100
	}

	b.WriteString("STATISTICS\n\n")
	b.WriteString(fmt.Sprintf("Hits:      %d\n", m.cpu.CacheStats.Hits))
	b.WriteString(fmt.Sprintf("Misses:    %d\n", m.cpu.CacheStats.Misses))
	b.WriteString(fmt.Sprintf("Total:     %d\n", total))
	b.WriteString(fmt.Sprintf("Hit Rate:  %.1f%%\n\n", hitRate))

	b.WriteString("CACHE LINES (Unified, 8 lines x 4 words)\n\n")

	for i := 0; i < 8; i++ {
		lineInfo := m.cpuWrapper.GetCacheLineInfo(i)
		if lineInfo.Valid {
			b.WriteString(fmt.Sprintf("Line %d: [VALID] Tag: 0x%02X\n", i, lineInfo.Tag))
		} else {
			b.WriteString(fmt.Sprintf("Line %d: [INVALID]\n", i))
		}
	}

	b.WriteString("\nPress 'c' to close\n")

	return b.String()
}

func (m model) renderMicrocode() string {
	var b strings.Builder
	b.WriteString("MICROCODE VIEWER\n\n")
	b.WriteString(fmt.Sprintf("Loaded: %s\n", m.microcodePath))
	b.WriteString(fmt.Sprintf("Current MPC: 0x%02X\n\n", m.cpu.MPC))

	if m.microcodePath == "" {
		b.WriteString("No microcode loaded.\n")
		return b.String()
	}

	file, err := os.Open(m.microcodePath)
	if err != nil {
		b.WriteString(fmt.Sprintf("Cannot open: %v\n", err))
		return b.String()
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	lineNum := 0
	instrNum := 0

	startInstr := int(m.cpu.MPC) - 5
	if startInstr < 0 {
		startInstr = 0
	}
	endInstr := startInstr + 15

	for scanner.Scan() {
		line := scanner.Text()

		if len(line) == 0 || line[0] == '#' {
			lineNum++
			continue
		}

		if len(line) >= 32 && (line[0] == '0' || line[0] == '1') {
			if instrNum >= startInstr && instrNum < endInstr {
				prefix := "  "
				if instrNum == int(m.cpu.MPC) {
					prefix = "→"
				}
				b.WriteString(fmt.Sprintf("%s 0x%02X: %s\n", prefix, instrNum, line[:32]))
			}
			instrNum++
		}
		lineNum++
	}

	b.WriteString("\nPress 'v' to close viewer.\n")
	return b.String()
}

func (m model) renderFilePicker() string {
	var b strings.Builder
	b.WriteString("SELECT ASSEMBLY FILE\n\n")

	if len(m.asmFiles) == 0 {
		b.WriteString("No .asm files found in examples/ directory\n\n")
		b.WriteString("Press ESC or 'l' to cancel\n")
		return b.String()
	}

	b.WriteString("Use ↑/↓ or j/k to navigate, Enter to select, ESC to cancel\n\n")

	visibleLines := 15
	start := 0
	if m.selectedFile > visibleLines/2 {
		start = m.selectedFile - visibleLines/2
	}
	if start < 0 {
		start = 0
	}
	end := start + visibleLines
	if end > len(m.asmFiles) {
		end = len(m.asmFiles)
	}

	for i := start; i < end; i++ {
		prefix := "  "
		if i == m.selectedFile {
			prefix = "→ "
		}
		b.WriteString(fmt.Sprintf("%s%s\n", prefix, m.asmFiles[i]))
	}

	b.WriteString(fmt.Sprintf("\nShowing %d of %d files\n", end-start, len(m.asmFiles)))
	return b.String()
}
