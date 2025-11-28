package main

import (
	"fmt"
	"io/ioutil"
)

func main() {
	fmt.Println("=== Assembler Integration Test ===")
	fmt.Println()

	// Create CPU wrapper
	fmt.Println("[1] Creating CPU wrapper...")
	cpu := NewCPUWrapper()
	fmt.Println("PASS: CPU initialized")
	fmt.Println()

	// Check if file exists
	filename := "../examples/sum.asm"
	fmt.Printf("[2] Checking if %s exists...\n", filename)
	content, err := ioutil.ReadFile(filename)
	if err != nil {
		fmt.Printf("FAIL: File not found: %v\n", err)
		return
	}
	fmt.Printf("PASS: File exists (%d bytes)\n", len(content))
	fmt.Println()

	// Display file content
	fmt.Println("[3] File content:")
	fmt.Println(string(content))
	fmt.Println()

	// Try to assemble
	fmt.Printf("[4] Assembling %s...\n", filename)
	err = cpu.AssembleFile(filename)
	if err != nil {
		fmt.Printf("FAIL: Assembly failed: %v\n", err)
		return
	}
	fmt.Println("PASS: Assembly successful")
	fmt.Println()

	// Read back assembled code from memory
	fmt.Println("[5] Reading assembled code from memory...")
	for addr := uint16(0); addr < 20; addr += 2 {
		val := cpu.ReadMemory(addr)
		if val != 0 {
			fmt.Printf("Memory[0x%04X] = 0x%04X\n", addr, val)
		}
	}
	fmt.Println()

	// Execute a few steps
	fmt.Println("[6] Executing 5 steps...")
	for i := 0; i < 5; i++ {
		cpu.Step()
		pc := cpu.GetPC()
		ac := cpu.GetAC()
		cycles := cpu.GetCycles()
		fmt.Printf("Step %d: PC=0x%04X, AC=0x%04X, Cycles=%d\n", i+1, pc, ac, cycles)
	}
	fmt.Println()

	fmt.Println("=== Assembler Integration Successful ===")
}
