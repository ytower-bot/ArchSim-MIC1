package main

import (
	"fmt"
	"log"
	"os"
	"time"
)

var (
	debugEnabled bool
	debugLogger  *log.Logger
	debugFile    *os.File
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

// CloseDebugMode closes the debug log file
func CloseDebugMode() {
	if debugFile != nil {
		debugLogger.Println()
		debugLogger.Println("========================================")
		debugLogger.Printf("Debug session ended: %s\n", time.Now().Format("2006-01-02 15:04:05"))
		debugLogger.Println("========================================")
		debugFile.Close()
	}
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
		debugLogger.Printf("  ⚠️  WARNING: %d memory mismatches detected!\n", mismatches)
	} else {
		debugLogger.Println("  ✓ All memory values match")
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

// Helper functions for formatted output
func logStateComparison(label string, coreVal, tuiVal uint16) {
	status := "✓"
	if coreVal != tuiVal {
		status = "✗ MISMATCH"
	}
	debugLogger.Printf("%s:  C: 0x%04X  |  TUI: 0x%04X  %s\n", label, coreVal, tuiVal, status)
}

func logStateComparison8(label string, coreVal, tuiVal uint8) {
	status := "✓"
	if coreVal != tuiVal {
		status = "✗ MISMATCH"
	}
	debugLogger.Printf("%s:  C: 0x%02X    |  TUI: 0x%02X    %s\n", label, coreVal, tuiVal, status)
}

func logFlagComparison(label string, coreVal, tuiVal bool) {
	status := "✓"
	if coreVal != tuiVal {
		status = "✗ MISMATCH"
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
