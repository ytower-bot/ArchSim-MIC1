package main

/*
#cgo CFLAGS: -I../include
#cgo LDFLAGS: -L../obj -lmic1 -lm
#include <stdint.h>
#include "../include/mic1.h"
#include "../include/assembler.h"
#include "../include/utils/conversions.h"

extern void cgo_init_cpu(void);
extern void cgo_reset_cpu(void);
extern void cgo_step_cpu(void);
extern uint16_t cgo_get_register(int index);
extern int cgo_get_cycles(void);
extern int cgo_get_clock(void);
extern int cgo_is_running(void);
extern int cgo_get_flag_n(void);
extern int cgo_get_flag_z(void);
extern uint8_t cgo_get_mpc(void);
extern uint16_t cgo_read_memory(uint16_t address);
extern void cgo_write_memory(uint16_t address, uint16_t value);
extern int cgo_get_cache_hits(void);
extern int cgo_get_cache_misses(void);
extern int cgo_get_cache_line_valid(int line_index);
extern uint8_t cgo_get_cache_line_tag(int line_index);
extern uint16_t cgo_get_cache_line_word(int line_index, int word_index);
extern int cgo_assemble_file(const char* filename);
extern int cgo_load_microcode(const char* filename);
extern const char* cgo_get_microcode_path(void);
extern void cgo_cleanup_cpu(void);
*/
import "C"
import (
	"fmt"
	"unsafe"
)

type CPUWrapper struct {
	initialized bool
}

func NewCPUWrapper() *CPUWrapper {
	C.cgo_init_cpu()
	return &CPUWrapper{initialized: true}
}

func (cpu *CPUWrapper) Reset() {
	C.cgo_reset_cpu()
}

func (cpu *CPUWrapper) Step() {
	C.cgo_step_cpu()
}

func (cpu *CPUWrapper) GetPC() uint16 {
	return uint16(C.cgo_get_register(0))
}

func (cpu *CPUWrapper) GetAC() uint16 {
	return uint16(C.cgo_get_register(1))
}

func (cpu *CPUWrapper) GetIR() uint16 {
	return uint16(C.cgo_get_register(2))
}

func (cpu *CPUWrapper) GetTIR() uint16 {
	return uint16(C.cgo_get_register(3))
}

func (cpu *CPUWrapper) GetSP() uint16 {
	return uint16(C.cgo_get_register(4))
}

func (cpu *CPUWrapper) GetCycles() int {
	return int(C.cgo_get_cycles())
}

func (cpu *CPUWrapper) GetClock() int {
	return int(C.cgo_get_clock())
}

func (cpu *CPUWrapper) IsRunning() bool {
	return int(C.cgo_is_running()) != 0
}

func (cpu *CPUWrapper) GetFlagN() bool {
	return int(C.cgo_get_flag_n()) != 0
}

func (cpu *CPUWrapper) GetFlagZ() bool {
	return int(C.cgo_get_flag_z()) != 0
}

func (cpu *CPUWrapper) GetMPC() uint8 {
	return uint8(C.cgo_get_mpc())
}

func (cpu *CPUWrapper) ReadMemory(address uint16) uint16 {
	return uint16(C.cgo_read_memory(C.uint16_t(address)))
}

func (cpu *CPUWrapper) WriteMemory(address uint16, value uint16) {
	C.cgo_write_memory(C.uint16_t(address), C.uint16_t(value))
}

func (cpu *CPUWrapper) GetCacheHits() int {
	return int(C.cgo_get_cache_hits())
}

func (cpu *CPUWrapper) GetCacheMisses() int {
	return int(C.cgo_get_cache_misses())
}

type CacheLineInfo struct {
	Index int
	Valid bool
	Tag   uint8
	Data  [4]uint16
}

func (cpu *CPUWrapper) GetCacheLineInfo(lineIndex int) CacheLineInfo {
	return CacheLineInfo{
		Index: lineIndex,
		Valid: int(C.cgo_get_cache_line_valid(C.int(lineIndex))) != 0,
		Tag:   uint8(C.cgo_get_cache_line_tag(C.int(lineIndex))),
		Data: [4]uint16{
			uint16(C.cgo_get_cache_line_word(C.int(lineIndex), 0)),
			uint16(C.cgo_get_cache_line_word(C.int(lineIndex), 1)),
			uint16(C.cgo_get_cache_line_word(C.int(lineIndex), 2)),
			uint16(C.cgo_get_cache_line_word(C.int(lineIndex), 3)),
		},
	}
}

func (cpu *CPUWrapper) AssembleFile(filename string) error {
	cFilename := C.CString(filename)
	defer C.free(unsafe.Pointer(cFilename))

	result := C.cgo_assemble_file(cFilename)
	if result < 0 {
		return fmt.Errorf("failed to assemble file: %s", filename)
	}

	return nil
}

func LoadMicrocode(filename string) error {
	cFilename := C.CString(filename)
	defer C.free(unsafe.Pointer(cFilename))

	result := int(C.cgo_load_microcode(cFilename))
	if result < 0 {
		return fmt.Errorf("failed to load microcode: %s", filename)
	}

	return nil
}

func GetMicrocodePath() string {
	return C.GoString(C.cgo_get_microcode_path())
}

func CleanupCPU() {
	C.cgo_cleanup_cpu()
}
