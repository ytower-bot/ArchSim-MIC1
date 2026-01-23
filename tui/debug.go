package main

import (
	"fmt"
	"log"
	"os"
	"time"
)

var (
	debugEnabled       bool
	debugLogger        *log.Logger
	debugFile          *os.File
	debugMismatchCount int
)

// InitDebugMode initializes debug logging to tui_debug.log
func InitDebugMode() error {
	var err error
	debugFile, err = os.OpenFile("tui_debug.log", os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0644)
	if err != nil {
		return fmt.Errorf("failed to create debug log file: %w", err)
	}

	debugLogger = log.New(debugFile, "", 0) // No prefix, we'll format ourselves
	debugEnabled = true

	debugLogger.Println("========================================")
	debugLogger.Printf("ArchSim-MIC1 TUI Debug Log\n")
	debugLogger.Printf("Started: %s\n", time.Now().Format("2006-01-02 15:04:05"))
	debugLogger.Println("========================================")
	debugLogger.Println()

	return nil
}

// CloseDebugMode closes the debug log file and returns error if mismatches found
func CloseDebugMode() error {
	if debugFile != nil {
		debugLogger.Println()
		debugLogger.Println("========================================")
		debugLogger.Printf("Debug session ended: %s\n", time.Now().Format("2006-01-02 15:04:05"))

		if debugMismatchCount > 0 {
			debugLogger.Printf("\nWARNING: %d mismatches detected!\n", debugMismatchCount)
			debugLogger.Println("TUI state does NOT match C core state.")
			debugLogger.Println("========================================")
			debugFile.Close()
			return fmt.Errorf("%d mismatch(es) detected between TUI and C core", debugMismatchCount)
		}

		debugLogger.Println("No mismatches detected. TUI state matches C core.")
		debugLogger.Println("========================================")
		debugFile.Close()
	}
	return nil
}

// DebugLog writes a debug message if debug mode is enabled
func DebugLog(format string, args ...interface{}) {
	if debugEnabled && debugLogger != nil {
		debugLogger.Printf(format, args...)
	}
}

// DebugCPUState logs CPU state comparison between C core and Go TUI
func DebugCPUState(cycle int, coreState, tuiState CPUState) {
	if !debugEnabled {
		return
	}

	debugLogger.Printf("[CYCLE %d] ==================== CPU State Comparison ====================\n", cycle)
	debugLogger.Println()

	// Registers comparison
	debugLogger.Println("  REGISTERS:")
	logStateComparison("    PC ", coreState.PC, tuiState.PC)
	logStateComparison("    AC ", coreState.AC, tuiState.AC)
	logStateComparison("    SP ", coreState.SP, tuiState.SP)
	logStateComparison("    IR ", coreState.IR, tuiState.IR)
	logStateComparison("    TIR", coreState.TIR, tuiState.TIR)
	logStateComparison8("    MPC", coreState.MPC, tuiState.MPC)
	debugLogger.Println()

	// Flags comparison
	debugLogger.Println("  FLAGS:")
	logFlagComparison("    N  ", coreState.N, tuiState.N)
	logFlagComparison("    Z  ", coreState.Z, tuiState.Z)
	debugLogger.Println()

	// Cycle/Clock comparison
	debugLogger.Println("  COUNTERS:")
	logStateComparison("    Cycles", uint16(coreState.Cycles), uint16(tuiState.Cycles))
	logStateComparison("    Clock ", uint16(coreState.Clock), uint16(tuiState.Clock))
	debugLogger.Println()

	// Cache stats
	debugLogger.Println("  CACHE:")
	logStateComparison("    Hits  ", uint16(coreState.CacheStats.Hits), uint16(tuiState.CacheStats.Hits))
	logStateComparison("    Misses", uint16(coreState.CacheStats.Misses), uint16(tuiState.CacheStats.Misses))
	debugLogger.Println()

	debugLogger.Println("========================================================================")
	debugLogger.Println()
}

// DebugMemoryState logs memory comparison for specific addresses
func DebugMemoryState(cycle int, addresses []uint16, coreValues, tuiValues map[uint16]uint16) {
	if !debugEnabled || len(addresses) == 0 {
		return
	}

	debugLogger.Printf("[CYCLE %d] ==================== Memory State Comparison ===================\n", cycle)
	debugLogger.Println()
	debugLogger.Println("  MEMORY (showing non-zero values):")

	mismatches := 0
	for _, addr := range addresses {
		coreVal := coreValues[addr]
		tuiVal := tuiValues[addr]

		if coreVal != 0 || tuiVal != 0 {
			status := "✓ MATCH"
			if coreVal != tuiVal {
				status = "✗ MISMATCH"
				mismatches++
			}

			debugLogger.Printf("    [0x%04X]  C: 0x%04X  |  TUI: 0x%04X  %s\n",
				addr, coreVal, tuiVal, status)
		}
	}

	debugLogger.Println()
	if mismatches > 0 {
		debugLogger.Printf("  WARNING: %d memory mismatches detected!\n", mismatches)
	} else {
		debugLogger.Println("  All memory values match")
	}
	debugLogger.Println()
	debugLogger.Println("========================================================================")
	debugLogger.Println()
}

// DebugRenderState logs what's being displayed in the TUI
func DebugRenderState(cycle int, displayedPC, displayedAC, displayedSP uint16) {
	if !debugEnabled {
		return
	}

	debugLogger.Printf("[CYCLE %d] ==================== TUI Render State ========================\n", cycle)
	debugLogger.Println()
	debugLogger.Println("  DISPLAYED VALUES:")
	debugLogger.Printf("    PC  = 0x%04X\n", displayedPC)
	debugLogger.Printf("    AC  = 0x%04X\n", displayedAC)
	debugLogger.Printf("    SP  = 0x%04X\n", displayedSP)
	debugLogger.Println()
	debugLogger.Println("========================================================================")
	debugLogger.Println()
}

// DebugAssemblyExecution logs the assembly code being executed
func DebugAssemblyExecution(cycle int, pc uint16, sourceCode []string, currentLine int) {
	if !debugEnabled {
		return
	}

	debugLogger.Printf("[CYCLE %d] ==================== Assembly Execution =======================\n", cycle)
	debugLogger.Println()
	debugLogger.Printf("  PC: 0x%04X (line %d)\n", pc, currentLine)
	debugLogger.Println()

	if len(sourceCode) > 0 && currentLine >= 0 && currentLine < len(sourceCode) {
		start := currentLine - 2
		if start < 0 {
			start = 0
		}
		end := currentLine + 3
		if end > len(sourceCode) {
			end = len(sourceCode)
		}

		debugLogger.Println("  CODE:")
		for i := start; i < end; i++ {
			marker := "   "
			if i == currentLine {
				marker = ">>>"
			}
			debugLogger.Printf("    %s %04d: %s\n", marker, i, sourceCode[i])
		}
	} else {
		debugLogger.Println("  CODE: (no source available)")
	}

	debugLogger.Println()
	debugLogger.Println("========================================================================")
	debugLogger.Println()
}

// DebugCacheContent logs the complete cache state
func DebugCacheContent(cycle int, cpuWrapper *CPUWrapper) {
	if !debugEnabled {
		return
	}

	debugLogger.Printf("[CYCLE %d] ==================== Cache Content ============================\n", cycle)
	debugLogger.Println()
	debugLogger.Println("  CACHE STATE (8 lines × 4 words):")
	debugLogger.Println()

	for i := 0; i < 8; i++ {
		line := cpuWrapper.GetCacheLineInfo(i)
		if line.Valid {
			debugLogger.Printf("    Line %d [V] Tag: 0x%02X | Data: [0x%04X 0x%04X 0x%04X 0x%04X]\n",
				i, line.Tag, line.Data[0], line.Data[1], line.Data[2], line.Data[3])
		} else {
			debugLogger.Printf("    Line %d [ ] (invalid)\n", i)
		}
	}

	debugLogger.Println()
	debugLogger.Println("========================================================================")
	debugLogger.Println()
}

// GetMismatchCount returns the number of mismatches detected
func GetMismatchCount() int {
	return debugMismatchCount
}

// Helper functions for formatted output
func logStateComparison(label string, coreVal, tuiVal uint16) {
	status := "✓"
	if coreVal != tuiVal {
		status = "✗ MISMATCH"
		debugMismatchCount++
	}
	debugLogger.Printf("%s:  C: 0x%04X  |  TUI: 0x%04X  %s\n", label, coreVal, tuiVal, status)
}

func logStateComparison8(label string, coreVal, tuiVal uint8) {
	status := "✓"
	if coreVal != tuiVal {
		status = "✗ MISMATCH"
		debugMismatchCount++
	}
	debugLogger.Printf("%s:  C: 0x%02X    |  TUI: 0x%02X    %s\n", label, coreVal, tuiVal, status)
}

func logFlagComparison(label string, coreVal, tuiVal bool) {
	status := "✓"
	if coreVal != tuiVal {
		status = "✗ MISMATCH"
		debugMismatchCount++
	}
	coreBit := 0
	if coreVal {
		coreBit = 1
	}
	tuiBit := 0
	if tuiVal {
		tuiBit = 1
	}
	debugLogger.Printf("%s:  C: %d      |  TUI: %d      %s\n", label, coreBit, tuiBit, status)
}
