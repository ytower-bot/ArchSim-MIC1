# ArchSim-MIC1: Documentação Técnica

## Sumário

1. [Introdução](#introdução)
2. [Arquitetura Geral](#arquitetura-geral)
3. [Via de Dados (Datapath)](#via-de-dados-datapath)
4. [Unidade de Controle](#unidade-de-controle)
5. [Sistema de Memória](#sistema-de-memória)
6. [Montador Assembly](#montador-assembly)
7. [Ciclo de Execução](#ciclo-de-execução)
8. [Referência de Sinais](#referência-de-sinais)

---

## Introdução

O **ArchSim-MIC1** é uma implementação completa em C da microarquitetura MIC-1 descrita por Andrew Tanenbaum no livro "Structured Computer Organization". O simulador implementa:

- Via de dados completa com 16 registradores
- Unidade de controle microprogramada
- Sistema de memória hierárquico (memória principal + caches separadas)
- Montador assembly para 8 instruções básicas
- Suite de testes automatizados

### Especificações Técnicas

| Componente | Especificação |
|------------|---------------|
| **Largura de palavra** | 16 bits |
| **Endereçamento** | 12 bits (4096 palavras) |
| **Microprograma** | 79 microinstruções |
| **Microinstrução** | 32 bits |
| **Registradores** | 16 registradores de propósito geral |
| **Cache de dados** | 8 linhas × 4 palavras (mapeamento direto) |
| **Cache de instruções** | 8 linhas × 4 palavras (independente) |

---

## Arquitetura Geral

A arquitetura MIC-1 segue o modelo de Von Neumann com unidade de controle microprogramada:

```
┌─────────────────────────────────────────────────────────┐
│                    MIC-1 Architecture                    │
├─────────────────────────────────────────────────────────┤
│                                                           │
│  ┌────────────────────────────────────────────────────┐ │
│  │           Control Unit (Unidade de Controle)        │ │
│  │  ┌──────┐  ┌──────┐  ┌──────────────┐  ┌──────┐   │ │
│  │  │ MPC  │→ │ MMUX │→ │Control Memory│→ │ MIR  │   │ │
│  │  └──────┘  └──────┘  └──────────────┘  └──────┘   │ │
│  └────────────────────────────────────────────────────┘ │
│                            ↓                             │
│                    Control Signals (32 bits)             │
│                            ↓                             │
│  ┌────────────────────────────────────────────────────┐ │
│  │              Datapath (Via de Dados)                │ │
│  │                                                      │ │
│  │  ┌──────────┐       ┌─────┐       ┌──────────┐    │ │
│  │  │ Register │ ───→  │ ALU │ ───→  │ Shifter  │    │ │
│  │  │   Bank   │       └─────┘       └──────────┘    │ │
│  │  │(16 regs) │          ↑                ↓          │ │
│  │  └──────────┘       Latch A/B       Result         │ │
│  │                                                      │ │
│  └────────────────────────────────────────────────────┘ │
│                            ↑↓                            │
│  ┌────────────────────────────────────────────────────┐ │
│  │              Memory System (Memória)                │ │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────────────┐ │ │
│  │  │Data Cache│  │Inst Cache│  │ Main Memory      │ │ │
│  │  │ 8 lines  │  │ 8 lines  │  │ 4096 × 16 bits   │ │ │
│  │  └──────────┘  └──────────┘  └──────────────────┘ │ │
│  └────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

---

## Via de Dados (Datapath)

### Banco de Registradores

O MIC-1 possui 16 registradores de 16 bits cada:

| Endereço | Nome | Função |
|----------|------|--------|
| 0x0 | PC | Program Counter (contador de programa) |
| 0x1 | AC | Accumulator (acumulador) |
| 0x2 | SP | Stack Pointer (ponteiro de pilha) |
| 0x3 | IR | Instruction Register (registrador de instrução) |
| 0x4 | TIR | Temporary Instruction Register |
| 0x5 | 0 | Constante zero |
| 0x6 | +1 | Constante um |
| 0x7 | -1 | Constante menos um (0xFFFF) |
| 0x8 | AMASK | Máscara de endereço (0x0FFF) |
| 0x9 | SMASK | Máscara de sinal (0x00FF) |
| 0xA | A | Registrador temporário A |
| 0xB | B | Registrador temporário B |
| 0xC | C | Registrador temporário C |
| 0xD | D | Registrador temporário D |
| 0xE | E | Registrador temporário E |
| 0xF | F | Registrador temporário F |

**Arquivo:** `include/datapath.h`, `src/datapath.c`

### ALU (Unidade Lógico-Aritmética)

A ALU executa 4 operações básicas controladas por 2 sinais (F0, F1):

| F1 | F0 | Operação | Descrição |
|----|----|----|-----------|
| 0 | 0 | A | Passa A sem modificação |
| 0 | 1 | B | Passa B sem modificação |
| 1 | 0 | A + B | Soma de A e B |
| 1 | 1 | A AND B | AND bit-a-bit |

**Saídas:**
- `result` (16 bits): Resultado da operação
- `N`: Flag negativo (bit 15 do resultado)
- `Z`: Flag zero (resultado == 0)

**Arquivo:** `include/alu.h`, `src/alu.c`

### Shifter (Deslocador)

Desloca o resultado da ALU em 1 bit, controlado por 2 sinais (SH0, SH1):

| SH1 | SH0 | Operação | Descrição |
|-----|-----|----------|-----------|
| 0 | 0 | No shift | Sem deslocamento |
| 0 | 1 | Right | Desloca 1 bit à direita |
| 1 | 0 | Left | Desloca 1 bit à esquerda |
| 1 | 1 | Reserved | Reservado |

**Arquivo:** `include/shifter.h`, `src/shifter.c`

### Decoders

#### Decoder A/B
Decodifica endereços de 4 bits para selecionar qual dos 16 registradores será lido:

```c
uint16_t decoder_a(uint16_t registers[16], uint8_t addr) {
    return registers[addr & 0x0F];
}

uint16_t decoder_b(uint16_t registers[16], uint8_t addr) {
    return registers[addr & 0x0F];
}
```

#### Decoder C
Decodifica endereço de 4 bits e sinal ENC para escrever em um registrador:

```c
void decoder_c(uint16_t registers[16], uint8_t addr, 
               uint16_t data, uint8_t enc) {
    if (enc && addr != 0x05) {  // Não escreve em R5 (zero)
        registers[addr & 0x0F] = data;
    }
}
```

### AMUX (Multiplexador de Entrada da ALU)

Seleciona entre Latch A ou MBR como entrada da ALU:

```c
uint16_t amux(uint16_t latch_a, uint16_t mbr, uint8_t amux_sel) {
    return (amux_sel == 0) ? latch_a : mbr;
}
```

### MAR e MBR

- **MAR (Memory Address Register):** Registrador de endereço de memória (12 bits)
- **MBR (Memory Buffer Register):** Registrador de dados de memória (16 bits)

Estes registradores conectam o datapath ao sistema de memória.

**Arquivo:** `include/datapath.h`, `src/datapath.c`

---

## Unidade de Controle

### MIR (Microinstruction Register)

Armazena a microinstrução atual de 32 bits com o seguinte formato:

```
┌─────┬─────┬─────┬─────┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┬────────┐
│AMUX │COND │JUMP │ JAM │ C │ B │ A │SH│ENC│ F0│ F1│RD│WR│ MBR│ ADDR   │
│ 1bit│2bits│3bits│3bits│4b│4b│4b│2b│1b│1b│1b│1b│1b│ 4b │ 8bits  │
└─────┴─────┴─────┴─────┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┴────────┘
 31    30-29 28-26 25-23  22 18 14 10  9  8  7  6  5  4-1    0-7
```

**Campos:**
- **AMUX (1 bit):** Seleciona entrada da ALU (0=Latch A, 1=MBR)
- **COND (2 bits):** Condição de desvio (N, Z)
- **JUMP (3 bits):** Tipo de desvio
- **JAM (3 bits):** Modo de cálculo do próximo MPC
- **C (4 bits):** Endereço do registrador destino
- **B (4 bits):** Endereço do registrador B
- **A (4 bits):** Endereço do registrador A
- **SH (2 bits):** Controle do shifter
- **ENC (1 bit):** Enable de escrita no registrador C
- **F0, F1 (2 bits):** Controle da ALU
- **RD (1 bit):** Read memory
- **WR (1 bit):** Write memory
- **MBR (4 bits):** Flags adicionais
- **ADDR (8 bits):** Endereço de desvio

**Arquivo:** `include/control_unit.h`, `src/control_unit.c`

### MPC (Microprogram Counter)

Contador de 8 bits que aponta para a próxima microinstrução a ser executada (0-255).

### MMUX (Microaddress Multiplexer)

Determina o próximo valor do MPC baseado em:

1. **JAM bits:** Controla como calcular próximo endereço
2. **COND:** Condições (N, Z flags)
3. **JUMP:** Tipo de desvio
4. **ADDR:** Endereço direto de desvio

**Modos de operação:**

| JAM | COND | Próximo MPC |
|-----|------|-------------|
| 000 | - | MPC + 1 (sequencial) |
| 001 | - | ADDR (desvio incondicional) |
| 010 | 01 (Z) | ADDR se Z=1, senão MPC+1 |
| 010 | 10 (N) | ADDR se N=1, senão MPC+1 |
| 100 | - | IR[7:0] (desvio indireto) |

### Control Memory

Memória de 256 palavras × 32 bits contendo o microprograma. No ArchSim-MIC1, usamos 79 microinstruções básicas.

**Arquivo:** `data/basic_microcode.txt`

---

## Sistema de Memória

### Hierarquia de Memória

```
┌──────────────────────────────────────────┐
│         CPU (Datapath + Control)          │
└─────────────────┬────────────────────────┘
                  │
        ┌─────────┴─────────┐
        │                   │
        ▼                   ▼
┌──────────────┐    ┌──────────────┐
│ Data Cache   │    │ Inst Cache   │
│ 8 lines      │    │ 8 lines      │
│ 4 words/line │    │ 4 words/line │
│ Direct Map   │    │ Direct Map   │
└──────┬───────┘    └──────┬───────┘
       │                   │
       └─────────┬─────────┘
                 ▼
        ┌────────────────┐
        │  Main Memory   │
        │ 4096 × 16 bits │
        └────────────────┘
```

### Memória Principal

- **Tamanho:** 4096 palavras × 16 bits
- **Endereçamento:** 12 bits (0x000 - 0xFFF)
- **Inicialização:** Todos os valores em zero

**Arquivo:** `include/memory.h`, `src/memory.c`

### Cache de Dados

- **Organização:** 8 linhas × 4 palavras
- **Mapeamento:** Direto
- **Política de escrita:** Write-through
- **Política de substituição:** Sobrescreve (mapeamento direto)

**Decomposição de endereço (12 bits):**

```
┌─────────┬─────────┬─────────┐
│   TAG   │  LINE   │  WORD   │
│ 7 bits  │ 3 bits  │ 2 bits  │
└─────────┴─────────┴─────────┘
  11-5      4-2       1-0
```

**Exemplo:**
- Endereço `0x123` (0001 0010 0011)
  - TAG = 0x09 (0001001)
  - LINE = 0x1 (001)
  - WORD = 0x3 (11)

**Estrutura:**

```c
typedef struct {
    uint16_t data[4];     // 4 palavras por linha
    uint16_t tag;         // TAG de 7 bits
    uint8_t valid;        // Bit de validade
} CacheLine;

typedef struct {
    CacheLine lines[8];   // 8 linhas
    uint32_t hits;        // Contador de hits
    uint32_t misses;      // Contador de misses
} DataCache;
```

**Arquivo:** `include/cache.h`, `src/cache.c`

### Cache de Instruções

Idêntica à cache de dados, mas **independente**:

- Não interfere com acessos de dados
- Otimizada para fetch sequencial de instruções
- Melhora desempenho em loops

**Arquivo:** `include/mic1.h`, `src/mic1.c`

---

## Montador Assembly

O ArchSim-MIC1 inclui um montador assembly completo para 8 instruções básicas.

### Conjunto de Instruções

#### 1. LODD (Load Direct)
```asm
LODD addr    ; AC ← Memory[addr]
```
- **Opcode:** 0x00
- **Formato:** 16 bits (0x00 + addr)
- **Exemplo:** `LODD 100` → carrega Memory[100] no AC

#### 2. STOD (Store Direct)
```asm
STOD addr    ; Memory[addr] ← AC
```
- **Opcode:** 0x01
- **Formato:** 16 bits (0x01 + addr)
- **Exemplo:** `STOD 200` → armazena AC em Memory[200]

#### 3. ADDD (Add Direct)
```asm
ADDD addr    ; AC ← AC + Memory[addr]
```
- **Opcode:** 0x02
- **Formato:** 16 bits (0x02 + addr)
- **Exemplo:** `ADDD 150` → AC = AC + Memory[150]

#### 4. SUBD (Subtract Direct)
```asm
SUBD addr    ; AC ← AC - Memory[addr]
```
- **Opcode:** 0x03
- **Formato:** 16 bits (0x03 + addr)
- **Exemplo:** `SUBD 175` → AC = AC - Memory[175]

#### 5. LOCO (Load Constant)
```asm
LOCO const   ; AC ← const
```
- **Opcode:** 0x04
- **Formato:** 16 bits (0x04 + const)
- **Exemplo:** `LOCO 42` → AC = 42

#### 6. JUMP (Jump Unconditional)
```asm
JUMP addr    ; PC ← addr
```
- **Opcode:** 0x05
- **Formato:** 16 bits (0x05 + addr)
- **Exemplo:** `JUMP LOOP` → desvia para label LOOP

#### 7. JZER (Jump if Zero)
```asm
JZER addr    ; if (AC == 0) PC ← addr
```
- **Opcode:** 0x06
- **Formato:** 16 bits (0x06 + addr)
- **Exemplo:** `JZER END` → desvia se AC == 0

#### 8. HALT (Halt)
```asm
HALT         ; Para execução
```
- **Opcode:** 0x07
- **Formato:** 16 bits (0x0700)

### Sintaxe Assembly

**Comentários:**
```asm
; Isto é um comentário (linha inteira)
LODD 100  ; Comentário no final da linha
```

**Labels:**
```asm
LOOP:   LODD 100    ; Define label "LOOP"
        ADDD 101
        JZER END
        JUMP LOOP
END:    HALT
```

**Operandos:**
```asm
LODD 100        ; Decimal
LODD 0x64       ; Hexadecimal
LOCO 42         ; Constante decimal
JUMP LABEL      ; Referência a label
```

### API do Montador

**Função principal:**
```c
int assemble(const char *source, uint16_t *output, int *size);
```

**Parâmetros:**
- `source`: String com código assembly
- `output`: Array para armazenar código binário
- `size`: Ponteiro para retornar tamanho do código

**Retorno:**
- `0`: Sucesso
- `-1`: Erro de sintaxe

**Exemplo de uso:**
```c
#include "assembler.h"

const char *code = 
    "START:  LOCO 0\n"
    "        LODD 100\n"
    "        ADDD 101\n"
    "        STOD 102\n"
    "        HALT\n";

uint16_t output[MAX_INSTRUCTIONS];
int size;

if (assemble(code, output, &size) == 0) {
    printf("Montagem bem-sucedida: %d instruções\n", size);
}
```

**Arquivo:** `include/assembler.h`, `src/assembler.c`

---

## Ciclo de Execução

O MIC-1 executa um ciclo de 4 estágios por microinstrução:

### 1. Fetch (Busca)

```c
// Ler microinstrução da control memory
MIR = control_memory[MPC];
```

### 2. Decode (Decodificação)

```c
// Extrair sinais de controle do MIR
AMUX    = (MIR >> 31) & 0x01;
COND    = (MIR >> 29) & 0x03;
JUMP    = (MIR >> 26) & 0x07;
JAM     = (MIR >> 23) & 0x07;
C_ADDR  = (MIR >> 19) & 0x0F;
B_ADDR  = (MIR >> 15) & 0x0F;
A_ADDR  = (MIR >> 11) & 0x0F;
SH      = (MIR >>  9) & 0x03;
ENC     = (MIR >>  8) & 0x01;
F0      = (MIR >>  7) & 0x01;
F1      = (MIR >>  6) & 0x01;
RD      = (MIR >>  5) & 0x01;
WR      = (MIR >>  4) & 0x01;
ADDR    = MIR & 0xFF;
```

### 3. Execute (Execução)

```c
// 1. Ler registradores
A = decoder_a(registers, A_ADDR);
B = decoder_b(registers, B_ADDR);

// 2. Carregar latches
latch_a = A;
latch_b = B;

// 3. Selecionar entrada da ALU
alu_input = amux(latch_a, MBR, AMUX);

// 4. Executar ALU
alu_result = alu(alu_input, latch_b, F0, F1, &N, &Z);

// 5. Shifter
shifted = shifter(alu_result, SH);

// 6. Escrever resultado
decoder_c(registers, C_ADDR, shifted, ENC);

// 7. Acesso à memória (se RD ou WR)
if (RD) MBR = memory_read(MAR);
if (WR) memory_write(MAR, MBR);
```

### 4. Update (Atualização)

```c
// Calcular próximo MPC usando MMUX
next_mpc = mmux(MPC, ADDR, JAM, COND, N, Z, IR);
MPC = next_mpc;
```

**Arquivo:** `include/mic1.h`, `src/mic1.c`

---

## Referência de Sinais

### Sinais de Controle (32 bits)

| Sinal | Bits | Valores | Descrição |
|-------|------|---------|-----------|
| AMUX | 31 | 0=Latch A, 1=MBR | Seleção de entrada da ALU |
| COND | 30-29 | 00=None, 01=Z, 10=N | Condição de desvio |
| JUMP | 28-26 | 000-111 | Tipo de desvio |
| JAM | 25-23 | 000-111 | Modo de cálculo do MPC |
| C | 22-19 | 0x0-0xF | Endereço destino |
| B | 18-15 | 0x0-0xF | Endereço operando B |
| A | 14-11 | 0x0-0xF | Endereço operando A |
| SH | 10-9 | 00=None, 01=Right, 10=Left | Shifter |
| ENC | 8 | 0=Disable, 1=Enable | Enable escrita C |
| F0 | 7 | Controle ALU bit 0 | |
| F1 | 6 | Controle ALU bit 1 | |
| RD | 5 | 0=No, 1=Yes | Read memory |
| WR | 4 | 0=No, 1=Yes | Write memory |
| MBR | 3-0 | Flags | Flags adicionais |
| ADDR | 7-0 | 0x00-0xFF | Endereço de desvio |

### Flags

| Flag | Nome | Descrição |
|------|------|-----------|
| N | Negative | Resultado da ALU é negativo (bit 15 = 1) |
| Z | Zero | Resultado da ALU é zero |

### Exemplo de Microinstrução

Microinstrução para `PC ← PC + 1`:

```
AMUX=0, COND=00, JUMP=000, JAM=000
C=0x0 (PC), B=0x5 (zero), A=0x0 (PC)
SH=00, ENC=1, F0=0, F1=1 (A+B)
RD=0, WR=0, ADDR=0x00
```

**Binário:**
```
0 00 000 000 0000 0101 0000 00 1 0 1 0 0 0000 00000000
```

**Hexadecimal:** `0x00500820`

---

## Compilação e Testes

### Compilação

```bash
# Compilar simulador
make all

# Compilar com debug
make debug

# Compilar e rodar testes
make test

# Limpar build
make clean
```

### Estrutura de Testes

```
tests/
├── test_assembler.c          # Testa montador (3 casos)
├── test_cache_separation.c   # Testa caches independentes
└── test_debug.c              # Debug geral
```

**Executar testes:**
```bash
make test
```

**Saída esperada:**
```
==========================================
Running all tests...
==========================================

>>> Running tests/test_assembler...
[PASS] Test 1: Simple program
[PASS] Test 2: Program with labels
[PASS] Test 3: File assembly
[PASS] tests/test_assembler

>>> Running tests/test_cache_separation...
[PASS] Test 1: Instruction cache cold/warm
[PASS] Test 2: Data cache independence
[PASS] Test 3: Statistics tracking
[PASS] tests/test_cache_separation

==========================================
All tests passed!
==========================================
```

---

## Referências

1. **Tanenbaum, A. S.** - *Structured Computer Organization* (6th Edition)
   - Capítulo 4: The Microarchitecture Level
   - Seção 4.1: An Example Microarchitecture (MIC-1)

2. **Patterson, D. A. & Hennessy, J. L.** - *Computer Organization and Design*
   - Hierarquia de memória e caches

3. **Especificação MIC-1 Original**
   - ISA e microprograma base

---

**Última Atualização:** 28/11/2025  
**Versão:** 1.0  
**Autor:** Projeto ArchSim-MIC1
