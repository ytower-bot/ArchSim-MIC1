package main

/*
#cgo CFLAGS: -I../include -std=c99
#cgo LDFLAGS: -L../obj -lmic1 -lm

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../include/mic1.h"
#include "../include/assembler.h"
#include "../include/control_unit.h"
#include "../include/utils/conversions.h"

static mic1_cpu* g_cpu = NULL;
static char g_microcode_path[512] = "";

int try_load_microprogram(control_memory* cm, const char* filename) {
    if (filename == NULL) {
        fprintf(stderr, "ERROR: No microcode path provided\n");
        return -1;
    }

    int result = load_microprogram(cm, filename);
    if (result >= 0) {
        strncpy(g_microcode_path, filename, sizeof(g_microcode_path) - 1);
        fprintf(stderr, "INFO: Loaded %d microinstructions from %s\n", result, filename);
    } else {
        fprintf(stderr, "ERROR: Failed to load microprogram from %s\n", filename);
    }
    return result;
}

void cgo_init_cpu() {
    if (g_cpu == NULL) {
        g_cpu = (mic1_cpu*)malloc(sizeof(mic1_cpu));
        if (g_cpu == NULL) {
            fprintf(stderr, "FATAL: Failed to allocate mic1_cpu (%zu bytes)\n", sizeof(mic1_cpu));
            return;
        }
        memset(g_cpu, 0, sizeof(mic1_cpu));
    }
    init_mic1(g_cpu);

    if (g_cpu->decoder_c.rb == NULL) {
        g_cpu->decoder_c.rb = &g_cpu->reg_bank;
    }
}

void cgo_reset_cpu() {
    if (g_cpu == NULL) {
        fprintf(stderr, "ERROR: CPU not initialized\n");
        return;
    }

    memory saved_memory;
    for (int i = 0; i < 4096; i++) {
        for (int j = 0; j < 16; j++) {
            saved_memory.data[i][j] = g_cpu->main_memory.data[i][j];
        }
    }

    reset_mic1(g_cpu);

    for (int i = 0; i < 4096; i++) {
        for (int j = 0; j < 16; j++) {
            g_cpu->main_memory.data[i][j] = saved_memory.data[i][j];
        }
    }

    if (g_cpu->decoder_c.rb == NULL) {
        g_cpu->decoder_c.rb = &g_cpu->reg_bank;
    }

    if (strlen(g_microcode_path) > 0) {
        int result = load_microprogram(&g_cpu->ctrl_mem, g_microcode_path);
        if (result < 0) {
            fprintf(stderr, "Warning: Failed to reload from %s\n", g_microcode_path);
        }
    }
}

int cgo_load_microcode(const char* path) {
    if (g_cpu == NULL) {
        fprintf(stderr, "ERROR: CPU not initialized\n");
        return -1;
    }
    int result = load_microprogram(&g_cpu->ctrl_mem, path);
    if (result >= 0) {
        strncpy(g_microcode_path, path, sizeof(g_microcode_path) - 1);
        fprintf(stderr, "INFO: Loaded %d microinstructions from %s\n", result, path);
    } else {
        fprintf(stderr, "ERROR: Failed to load microprogram from %s\n", path);
    }
    return result;
}

const char* cgo_get_microcode_path() {
    return g_microcode_path;
}

void cgo_cleanup_cpu() {
    if (g_cpu != NULL) {
        free(g_cpu);
        g_cpu = NULL;
    }
}

void cgo_step_cpu() {
    if (g_cpu == NULL) {
        return;
    }

    if (!g_cpu->decoder_c.rb) {
        g_cpu->decoder_c.rb = &g_cpu->reg_bank;
    }

    step_mic1(g_cpu);

    if (!g_cpu->decoder_c.rb) {
        g_cpu->decoder_c.rb = &g_cpu->reg_bank;
    }
}

uint16_t cgo_get_register(int index) {
    if (g_cpu == NULL) return 0;
    if (index < 0 || index >= 16) return 0;

    int value = 0;
    switch(index) {
        case 0:
            value = array_to_int(g_cpu->reg_bank.PC.data);
            break;
        case 1:
            value = array_to_int(g_cpu->reg_bank.AC.data);
            break;
        case 2:
            value = array_to_int(g_cpu->reg_bank.IR.data);
            break;
        case 3:
            value = array_to_int(g_cpu->reg_bank.TIR.data);
            break;
        case 4:
            value = array_to_int(g_cpu->reg_bank.SP.data);
            break;
        default: return 0;
    }

    if (value < 0) {
        return (uint16_t)(value + 65536);
    }
    return (uint16_t)value;
}

int cgo_get_cycles() {
    if (g_cpu == NULL) return 0;
    return g_cpu->cycle_count;
}

int cgo_get_clock() {
    if (g_cpu == NULL) return 0;
    return g_cpu->clock;
}

int cgo_is_running() {
    if (g_cpu == NULL) return 0;
    return g_cpu->running;
}

int cgo_get_flag_n() {
    if (g_cpu == NULL) return 0;
    return g_cpu->alu.flag_n;
}

int cgo_get_flag_z() {
    if (g_cpu == NULL) return 0;
    return g_cpu->alu.flag_z;
}

uint8_t cgo_get_mpc() {
    if (g_cpu == NULL) return 0;
    return bits_to_int(g_cpu->mpc.address, 8);
}

uint16_t cgo_read_memory(uint16_t address) {
    if (g_cpu == NULL) return 0;
    if (address >= 4096) return 0;
    return bits_to_int(g_cpu->main_memory.data[address], 16);
}

void cgo_write_memory(uint16_t address, uint16_t value) {
    if (g_cpu == NULL) return;
    if (address >= 4096) return;
    int_to_bits(value, g_cpu->main_memory.data[address], 16);
}

int cgo_get_cache_hits() {
    if (g_cpu == NULL) return 0;
    return g_cpu->unified_cache.hits;
}

int cgo_get_cache_misses() {
    if (g_cpu == NULL) return 0;
    return g_cpu->unified_cache.misses;
}

int cgo_get_cache_line_valid(int line_index) {
    if (g_cpu == NULL) return 0;
    if (line_index < 0 || line_index >= 8) return 0;
    return g_cpu->unified_cache.lines[line_index].valid;
}

int cgo_get_cache_line_tag(int line_index) {
    if (g_cpu == NULL) return 0;
    if (line_index < 0 || line_index >= 8) return 0;
    return bits_to_int(g_cpu->unified_cache.lines[line_index].tag, 7);
}

uint16_t cgo_get_cache_line_word(int line_index, int word_offset) {
    if (g_cpu == NULL) return 0;
    if (line_index < 0 || line_index >= 8) return 0;
    if (word_offset < 0 || word_offset >= 4) return 0;
    return bits_to_int(g_cpu->unified_cache.lines[line_index].data[word_offset], 16);
}

int cgo_assemble_file(const char* input_file) {
    if (g_cpu == NULL) {
        fprintf(stderr, "ERROR: CPU not initialized\n");
        return -1;
    }


    uint16_t* output = (uint16_t*)malloc(MAX_INSTRUCTIONS * sizeof(uint16_t));
    if (!output) {
        fprintf(stderr, "ERROR: Failed to allocate output buffer\n");
        return -1;
    }
    memset(output, 0, MAX_INSTRUCTIONS * sizeof(uint16_t));


    int* size_ptr = (int*)malloc(sizeof(int));
    if (!size_ptr) {
        fprintf(stderr, "ERROR: Failed to allocate size variable\n");
        free(output);
        return -1;
    }
    *size_ptr = 0;

    FILE* fp = fopen(input_file, "r");
    if (!fp) {
        fprintf(stderr, "ERROR: Cannot open file %s\n", input_file);
        free(size_ptr);
        free(output);
        return -1;
    }


    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);


    char* source = (char*)malloc(fsize + 1);
    if (!source) {
        fprintf(stderr, "ERROR: Failed to allocate source buffer\n");
        fclose(fp);
        free(size_ptr);
        free(output);
        return -1;
    }

    size_t read_size = fread(source, 1, fsize, fp);
    fclose(fp);
    source[read_size] = 0;


    int result = assemble_string(source, output, size_ptr);
    free(source);


    if (result != 0) {
        free(size_ptr);
        free(output);
        return -1;
    }

    for (int i = 0; i < *size_ptr; i++) {
        cgo_write_memory(i, output[i]);
    }


    int final_size = *size_ptr;
    free(size_ptr);
    free(output);
    return final_size;
}
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
