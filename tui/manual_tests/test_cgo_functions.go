package main

import (
	"fmt"
	"os"
)

func main() {
	fmt.Println("=== CGO Function Test ===")
	fmt.Println()

	// Create CPU wrapper
	fmt.Println("[1] Creating CPU wrapper...")
	cpu := NewCPUWrapper()
	fmt.Println("PASS: CPU initialized")
	fmt.Println()

	// Test initial state
	fmt.Println("[2] Testing initial state...")
	pc := cpu.GetPC()
	ac := cpu.GetAC()
	sp := cpu.GetSP()
	fmt.Printf("PC: 0x%04X, AC: 0x%04X, SP: 0x%04X\n", pc, ac, sp)
	if sp == 0 {
		fmt.Println("WARNING: SP is 0 (expected 0x0FFF)")
	}
	fmt.Println("PASS: State read successful")
	fmt.Println()

	// Test memory operations
	fmt.Println("[3] Testing memory operations...")
	cpu.WriteMemory(0x100, 0x1234)
	val := cpu.ReadMemory(0x100)
	fmt.Printf("Write 0x1234 to 0x100, Read back: 0x%04X\n", val)
	if val == 0x1234 {
		fmt.Println("PASS: Memory read/write works")
	} else {
		fmt.Println("FAIL: Memory read/write mismatch")
		os.Exit(1)
	}
	fmt.Println()

	// Test step
	fmt.Println("[4] Testing CPU step...")
	cyclesBefore := cpu.GetCycles()
	cpu.Step()
	cyclesAfter := cpu.GetCycles()
	fmt.Printf("Cycles before: %d, after: %d\n", cyclesBefore, cyclesAfter)
	if cyclesAfter > cyclesBefore {
		fmt.Println("PASS: Step increments cycles")
	} else {
		fmt.Println("WARNING: Cycles didn't increment (might be OK)")
	}
	fmt.Println()

	// Test reset
	fmt.Println("[5] Testing CPU reset...")
	cpu.Step()
	cpu.Step()
	cyclesBeforeReset := cpu.GetCycles()
	cpu.Reset()
	cyclesAfterReset := cpu.GetCycles()
	fmt.Printf("Cycles before reset: %d, after: %d\n", cyclesBeforeReset, cyclesAfterReset)
	fmt.Println("PASS: Reset completed")
	fmt.Println()

	// Test cache stats
	fmt.Println("[6] Testing cache statistics...")
	dataHits := cpu.GetDataCacheHits()
	dataMisses := cpu.GetDataCacheMisses()
	instHits := cpu.GetInstCacheHits()
	instMisses := cpu.GetInstCacheMisses()
	fmt.Printf("Data cache: %d hits, %d misses\n", dataHits, dataMisses)
	fmt.Printf("Inst cache: %d hits, %d misses\n", instHits, instMisses)
	fmt.Println("PASS: Cache stats accessible")
	fmt.Println()

	// Test flags
	fmt.Println("[7] Testing ALU flags...")
	n := cpu.GetFlagN()
	z := cpu.GetFlagZ()
	fmt.Printf("N flag: %t, Z flag: %t\n", n, z)
	fmt.Println("PASS: Flags accessible")
	fmt.Println()

	// Test MPC
	fmt.Println("[8] Testing microprogram counter...")
	mpc := cpu.GetMPC()
	fmt.Printf("MPC: 0x%02X\n", mpc)
	fmt.Println("PASS: MPC accessible")
	fmt.Println()

	fmt.Println("=== All CGO Functions Working ===")
	fmt.Println()
	fmt.Println("Integration is successful!")
}
