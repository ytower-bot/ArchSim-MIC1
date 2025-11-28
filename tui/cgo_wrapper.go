package main

/*
#cgo CFLAGS: -I../include -std=c99
#cgo LDFLAGS: -L../obj -lmic1 -lm

#include <stdlib.h>
#include <stdint.h>
#include "../include/mic1.h"
#include "../include/assembler.h"

// Global CPU instance
static mic1_cpu g_cpu;

// Initialize CPU
void cgo_init_cpu() {
    init_mic1(&g_cpu);
}

// Reset CPU
void cgo_reset_cpu() {
    reset_mic1(&g_cpu);
}

// Step one cycle
void cgo_step_cpu() {
    step_mic1(&g_cpu);
}

// Get register value
uint16_t cgo_get_register(int index) {
    if (index < 0 || index >= 16) return 0;
    
    switch(index) {
        case 0: return bits_to_int(g_cpu.reg_bank.PC.data, 16);
        case 1: return bits_to_int(g_cpu.reg_bank.AC.data, 16);
        case 2: return bits_to_int(g_cpu.reg_bank.SP.data, 16);
        case 3: return bits_to_int(g_cpu.reg_bank.IR.data, 16);
        case 4: return bits_to_int(g_cpu.reg_bank.TIR.data, 16);
        default: return 0;
    }
}

// Get CPU state
int cgo_get_cycles() {
    return g_cpu.cycle_count;
}

int cgo_get_clock() {
    return g_cpu.clock;
}

int cgo_is_running() {
    return g_cpu.running;
}

// Get flags
int cgo_get_flag_n() {
    return g_cpu.alu.flag_n;
}

int cgo_get_flag_z() {
    return g_cpu.alu.flag_z;
}

// Get MPC
uint8_t cgo_get_mpc() {
    return bits_to_int(g_cpu.mpc.address, 8);
}

// Memory operations
uint16_t cgo_read_memory(uint16_t address) {
    if (address >= 4096) return 0;
    return bits_to_int(g_cpu.main_memory.data[address], 16);
}

void cgo_write_memory(uint16_t address, uint16_t value) {
    if (address >= 4096) return;
    int_to_bits(value, g_cpu.main_memory.data[address], 16);
}

// Cache stats
int cgo_get_data_cache_hits() {
    return g_cpu.data_cache.hits;
}

int cgo_get_data_cache_misses() {
    return g_cpu.data_cache.misses;
}

int cgo_get_inst_cache_hits() {
    return g_cpu.instruction_cache.hits;
}

int cgo_get_inst_cache_misses() {
    return g_cpu.instruction_cache.misses;
}

// Assembler wrapper
int cgo_assemble_file(const char* input_file) {
    uint16_t output[MAX_INSTRUCTIONS];
    int size = 0;
    
    // Read file
    FILE* fp = fopen(input_file, "r");
    if (!fp) return -1;
    
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* source = malloc(fsize + 1);
    fread(source, 1, fsize, fp);
    fclose(fp);
    source[fsize] = 0;
    
    // Assemble
    int result = assemble_string(source, output, &size);
    free(source);
    
    if (result != 0) return -1;
    
    // Load into memory
    for (int i = 0; i < size; i++) {
        cgo_write_memory(i * 2, output[i]);
    }
    
    return size;
}
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// CPUWrapper wraps the C MIC-1 CPU
type CPUWrapper struct {
	initialized bool
}

// NewCPUWrapper creates and initializes a new CPU wrapper
func NewCPUWrapper() *CPUWrapper {
	C.cgo_init_cpu()
	return &CPUWrapper{initialized: true}
}

// Reset resets the CPU state
func (cpu *CPUWrapper) Reset() {
	C.cgo_reset_cpu()
}

// Step executes one CPU cycle
func (cpu *CPUWrapper) Step() {
	C.cgo_step_cpu()
}

// GetPC returns the Program Counter value
func (cpu *CPUWrapper) GetPC() uint16 {
	return uint16(C.cgo_get_register(0))
}

// GetAC returns the Accumulator value
func (cpu *CPUWrapper) GetAC() uint16 {
	return uint16(C.cgo_get_register(1))
}

// GetSP returns the Stack Pointer value
func (cpu *CPUWrapper) GetSP() uint16 {
	return uint16(C.cgo_get_register(2))
}

// GetIR returns the Instruction Register value
func (cpu *CPUWrapper) GetIR() uint16 {
	return uint16(C.cgo_get_register(3))
}

// GetTIR returns the Temporary Instruction Register value
func (cpu *CPUWrapper) GetTIR() uint16 {
	return uint16(C.cgo_get_register(4))
}

// GetCycles returns the cycle count
func (cpu *CPUWrapper) GetCycles() int {
	return int(C.cgo_get_cycles())
}

// GetClock returns the clock count
func (cpu *CPUWrapper) GetClock() int {
	return int(C.cgo_get_clock())
}

// IsRunning returns whether CPU is running
func (cpu *CPUWrapper) IsRunning() bool {
	return int(C.cgo_is_running()) != 0
}

// GetFlagN returns the N flag
func (cpu *CPUWrapper) GetFlagN() bool {
	return int(C.cgo_get_flag_n()) != 0
}

// GetFlagZ returns the Z flag
func (cpu *CPUWrapper) GetFlagZ() bool {
	return int(C.cgo_get_flag_z()) != 0
}

// GetMPC returns the Microprogram Counter value
func (cpu *CPUWrapper) GetMPC() uint8 {
	return uint8(C.cgo_get_mpc())
}

// ReadMemory reads a value from memory
func (cpu *CPUWrapper) ReadMemory(address uint16) uint16 {
	return uint16(C.cgo_read_memory(C.uint16_t(address)))
}

// WriteMemory writes a value to memory
func (cpu *CPUWrapper) WriteMemory(address uint16, value uint16) {
	C.cgo_write_memory(C.uint16_t(address), C.uint16_t(value))
}

// GetDataCacheHits returns data cache hits
func (cpu *CPUWrapper) GetDataCacheHits() int {
	return int(C.cgo_get_data_cache_hits())
}

// GetDataCacheMisses returns data cache misses
func (cpu *CPUWrapper) GetDataCacheMisses() int {
	return int(C.cgo_get_data_cache_misses())
}

// GetInstCacheHits returns instruction cache hits
func (cpu *CPUWrapper) GetInstCacheHits() int {
	return int(C.cgo_get_inst_cache_hits())
}

// GetInstCacheMisses returns instruction cache misses
func (cpu *CPUWrapper) GetInstCacheMisses() int {
	return int(C.cgo_get_inst_cache_misses())
}

// AssembleFile assembles an assembly file and loads it into memory
func (cpu *CPUWrapper) AssembleFile(filename string) error {
	cFilename := C.CString(filename)
	defer C.free(unsafe.Pointer(cFilename))
	
	result := C.cgo_assemble_file(cFilename)
	if result < 0 {
		return fmt.Errorf("failed to assemble file: %s", filename)
	}
	
	return nil
}
