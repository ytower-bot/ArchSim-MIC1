# ROADMAP - ArchSim-MIC1

**Modelo:** Implementação por Fases
**Status:** Fases 0-5 Completas | Fases 6-8 Pendentes

---

## FASE 0 - CORREÇÃO DE BUG CRÍTICO ✅

**Objetivo:** Eliminar memory leak e garantir compilação

**Tasks:**
- [x] Remover `malloc()` em `init_register_bank()`
- [x] Validar compilação sem erros
- [x] Testar execução básica

**Arquivos:** `src/datapath.c`
**Tempo:** 15 minutos
**Commit:** `ffe0d4e`

---

## FASE 1 - FUNÇÕES DE CONVERSÃO ✅

**Objetivo:** Funções utilitárias bit ↔ int para toda arquitetura

**Tasks:**
- [x] `bits_to_int(bits[], size)` - Array variável → int
- [x] `int_to_bits(value, bits[], size)` - Int → array variável
- [x] `address_to_int(address[12])` - 12 bits → endereço (0-4095)
- [x] `int_to_address(addr, address[12])` - Int → 12 bits
- [x] `copy_data(dest[16], src[16])` - Cópia de array 16 bits
- [x] Testes unitários (9 casos)

**Arquivos:**
- `include/utils/conversions.h` (declarações)
- `src/utils/conversions.c` (implementação)
- `src/tests/test_conversions.c` (testes)

**Validação:** 9/9 testes passando
**Tempo:** 1.5 horas
**Commit:** `4611bef`

---

## FASE 2 - COMPONENTES DATAPATH ✅

**Objetivo:** Shifter, Decoders A/B/C, MAR/MBR funcionais

### Shifter
- [x] `run_shifter()` - Executa operação (control_sh[2])
- [x] `get_shifter_control_value()` - Converte control → int
- [x] Remover malloc desnecessário em `init_shifter()`

### Decoders A/B
- [x] `run_decoder()` - Já implementado
- [x] `select_register()` - Switch case 0-15 → ponteiro
- [x] Copiar registrador → latch

### Decoder C
- [x] `run_decoderC()` - Write-back para registrador
- [x] Verificar ENC=1 antes de escrever
- [x] Control C[4] seleciona registrador destino

### MAR/MBR
- [x] `run_mar()` - Carrega endereço do Latch B
- [x] `run_mbr()` - Leitura/escrita memória
- [x] RD=1: chama `m_read()`
- [x] WR=1: chama `m_write()`
- [x] MBR=1: copia shifter → MBR

**Arquivos:**
- `src/shifter.c` (+45 linhas)
- `src/datapath.c` (+45 linhas)
- `include/datapath.h` (+1 declaração)

**Validação:** Compilação + execução sem crash
**Tempo:** 30 minutos
**Commit:** (pendente)

---

## FASE 3 - SISTEMA DE MEMÓRIA ✅

**Objetivo:** Memória principal + Cache com hit/miss tracking

### Memória Principal
- [x] `init_memory()` - Zera 4096 × 16 bits
- [x] `m_read()` - Converte MAR[12] → índice, copia memory[addr] → MBR
- [x] `m_write()` - Converte MAR[12] → índice, copia MBR → memory[addr]
- [x] `load_program()` - Carrega arquivo binário
- [x] Validação de range (0-4095)

### Cache (8 linhas × 4 palavras)
- [x] `init_cache()` - Inicializa 8 linhas, valid=0
- [x] `decompose_address()` - Tag(7) | Line(3) | Word(2)
- [x] `compare_tags()` - Compara 7 bits
- [x] `cache_lookup()` - Verifica valid + tag, atualiza hits/misses
- [x] `cache_load_block()` - Carrega 4 palavras da memória
- [x] `cache_read()` - Lookup, se miss carrega bloco
- [x] `cache_write()` - Write-through: memória + cache
- [x] Estatísticas: `print_cache_stats()`, `get_hit_rate()`, `reset_cache_stats()`

**Estrutura Cache:**
```c
typedef struct cache_line {
    int valid;              // Hit/Miss
    int tag[7];             // Identificador bloco
    int data[4][16];        // 4 palavras × 16 bits
} cache_line;

typedef struct cache {
    cache_line lines[8];    // Mapeamento direto
    int hits;
    int misses;
} cache;
```

**Arquivos:**
- `src/cache.c` (290 linhas - novo arquivo)
- `src/memory.c` (+104 linhas)
- `include/cache.h` (+1 forward declaration)

**Validação:** Memória + Cache funcionais
**Tempo:** 45 minutos
**Commit:** (pendente)

---

## FASE 4 - UNIDADE DE CONTROLE ✅

**Objetivo:** MIR, MPC, MMUX executando microinstruções

### MIR (Microinstruction Register)
- [x] `run_mir()` - Decodifica 32 bits em sinais de controle
- [x] Distribuir sinais para componentes:
  - [0]: AMUX
  - [1-2]: COND → MMUX
  - [3-4]: ALU → ALU
  - [5-6]: SH → Shifter
  - [7]: MBR → MBR
  - [8]: MAR → MAR
  - [9]: RD → MBR
  - [10]: WR → MBR
  - [11]: ENC → Decoder C
  - [12-15]: C → Decoder C
  - [16-19]: B → Decoder B
  - [20-23]: A → Decoder A
  - [24-31]: ADDR → MMUX
- [x] `load_mir()` - Carrega microinstrução da memória de controle

### MPC (Microprogram Counter)
- [x] `increment_mpc()` - MPC = MPC + 1
- [x] `load_mpc(address)` - Carrega novo endereço (desvio)
- [x] `run_mpc()` - Busca próxima microinstrução

### MMUX (Micro Address Multiplexer)
- [x] `run_mmux()` - Decide próximo endereço MPC
- [x] Lógica de desvio:
  - COND=00: MPC+1 (sequencial)
  - COND=01: Se N=1, ADDR; senão MPC+1
  - COND=10: Se Z=1, ADDR; senão MPC+1
  - COND=11: ADDR (incondicional)
- [x] Ler flags N/Z da ALU

### Microprograma Básico
- [x] Definir 10-20 microinstruções de teste
- [x] Implementar loader de microprograma
- [x] `init_control_memory()` - Carrega ROM

**Arquivos:**
- `src/control_unit.c` (novo - ~640 linhas)
- `src/mic1.c` (integração)
- `include/control_unit.h` (atualizar)

**Validação:**
- ✓ Executar microinstrução simples (ex: MAR ← PC)
- ✓ Testar desvio condicional
- ✓ Testar flags N/Z
- ✓ AMUX: 7/7 testes passando
- ✓ Control Memory: 9/9 testes passando

**Tempo Estimado:** 4-5 horas
**Tempo Real:** ~4 horas
**Prioridade:** ALTA
**Commits:** b5e7fbd, bf6dd10

---

## FASE 5 - CICLO DE EXECUÇÃO ✅

**Objetivo:** Pipeline completo fetch-decode-execute-update

### Ciclo MIC-1
- [x] `fetch_microinstruction()` - MPC → ROM → MIR
- [x] `decode_signals()` - MIR → todos os componentes (inline em execute_datapath)
- [x] `execute_datapath()` - Executar via de dados
  1. Decoder A → Latch A
  2. Decoder B → Latch B
  3. AMUX seleciona entrada ULA
  4. ALU executa operação
  5. Shifter processa resultado
  6. Decoder C escreve em registrador
  7. MAR/MBR acessam memória
- [x] `update_control()` - MMUX decide próximo MPC

### Integração
- [x] Conectar `run_mic1_cycle()` ao ciclo completo
- [x] Sincronização de componentes
- [x] Ordem de execução:
  1. Fetch (MPC → MIR)
  2. Decode (MIR → sinais)
  3. Execute (Datapath)
  4. Update (MMUX → MPC)
- [ ] `halt_mic1()` - Condição de parada (não necessário ainda)

### Testes
- [x] Executar NOP (no operation)
- [x] Executar MAR ← PC
- [x] Executar AC ← AC + 1
- [x] Testar 10 ciclos consecutivos

**Arquivos:**
- `src/mic1.c` (+100 linhas - execute_datapath, run_mic1_cycle reescrito)
- `src/control_unit.c` (+55 linhas - fetch_microinstruction, update_control)
- `src/datapath.c` (bug fixes - init_decoder/init_decoderC)
- `include/mic1.h` (+1 declaração)
- `include/control_unit.h` (+2 declarações)
- `src/tests/test_execution_cycle.c` (novo - 160 linhas, 9 testes)

**Validação:**
- ✓ Executar microinstruções em sequência
- ✓ Step-by-step funcional
- ✓ Estado da CPU consistente
- ✓ 9/9 testes passando (100%)
- ✓ Compilação sem warnings

**Bugs Corrigidos:**
- Decoder initialization in init_mic1()
- malloc bug in init_decoder/init_decoderC
- Duplicate MPC increment in update_control()

**Tempo Estimado:** 4-6 horas
**Tempo Real:** ~5 horas
**Prioridade:** ALTA
**Commits:** (pending)

---

## FASE 6 - MICROPROGRAMA & ISA ⚠️

**Status:** FASE 6A Completa (Foundation) | FASE 6B-6E Pendentes

**Objetivo:** ISA completo MIC-1 + microprograma interpretado

### FASE 6A - Foundation (COMPLETA) ✅
- [x] Definir 24 opcodes (8 bits) em `include/opcodes.h`:
  - LODD, STOD, ADDD, SUBD (direct addressing)
  - LODL, STOL, ADDL, SUBL (local addressing)
  - LOCO (load constant)
  - JPOS, JZER, JNEG, JNZE (conditional jumps)
  - JUMP, CALL, RETN (unconditional control flow)
  - PSHI, PUSH, POP, POPA (stack operations)
  - SWAP, INSP, DESP (stack manipulation)
  - HALT (system)
- [x] Estruturar microprograma com fetch-decode-execute
- [x] Fetch comum: PC → MAR, MBR → IR (0x00-0x03)
- [x] Decodificação: IR[8 bits] → endereço microrotina (0x04)
- [x] Templates para 7 instruções chave
- [x] Documentar formato de instrução (16 bits: opcode+operand)
- [x] Macros helper (MAKE_INSTRUCTION, GET_OPCODE, GET_OPERAND)

### FASE 6B - Core Instructions (PENDENTE)
- [ ] Implementar microcode completo para LODD
- [ ] Implementar microcode completo para STOD
- [ ] Implementar microcode completo para ADDD
- [ ] Implementar microcode completo para LOCO
- [ ] Implementar microcode completo para JUMP
- [ ] Implementar microcode completo para HALT
- [ ] Criar programa de teste: soma dois números
- [ ] Validar fetch-decode-execute com ISA real

### FASE 6C-6E - Extended ISA (FUTURO)
- [ ] Conditional jumps (JPOS, JZER, JNEG, JNZE)
- [ ] Stack operations (CALL, RETN, PUSH, POP, etc.)
- [ ] Local addressing (LODL, STOL, ADDL, SUBL)
- [ ] Complete 256-entry microprograma

**Arquivos:**
- `include/opcodes.h` (novo - 142 linhas) ✅
- `data/mic1_microcode.txt` (novo - 220 linhas, templates) ✅
- `docs/archsim_fase_log/FASE_6.txt` (novo - documentação) ✅

**Validação (FASE 6A):**
- ✓ ISA architecture documented
- ✓ Microprograma structure designed
- ✓ Fetch-decode-execute framework
- ⚠ Not yet executable (needs FASE 6B)

**Tempo Estimado:** 8-12 horas total (6A: 2h ✅, 6B-6E: 6-10h pendentes)
**Tempo Real (FASE 6A):** ~2 horas
**Prioridade:** MÉDIA
**Commits:** (pending)

---

## FASE 7 - LOADER E MONTADOR

**Objetivo:** Carregar programas externos em memória

### Loader Binário
- [ ] `load_binary_file(filename)` - Lê arquivo binário
- [ ] Parser de formato:
  - Header: endereço inicial, tamanho
  - Dados: sequência de palavras 16 bits
- [ ] Validação: range, overflow
- [ ] `load_at_address(address, data[], size)` - Carrega em endereço específico

### Montador Assembly
- [ ] Definir sintaxe assembly:
  ```asm
  ; Comentário
  LABEL:  LODD 100    ; Load direto
          ADDD 101    ; Add direto
          STOD 102    ; Store direto
          HALT        ; Halt
  ```
- [ ] Lexer: tokenizar entrada
- [ ] Parser: assembly → IR
- [ ] Gerador: IR → binário
- [ ] Tabela de símbolos (labels)
- [ ] Pass 1: coletar labels
- [ ] Pass 2: gerar código
- [ ] `assemble_file(input.asm, output.bin)`

### Interface
- [ ] Comando `load <file.bin>`
- [ ] Comando `assemble <file.asm>`
- [ ] Feedback de erros

**Arquivos:**
- `src/loader.c` (novo - ~150 linhas)
- `src/assembler/` (novo diretório)
  - `lexer.c` (~200 linhas)
  - `parser.c` (~250 linhas)
  - `codegen.c` (~150 linhas)
- `include/loader.h` (novo)
- `include/assembler.h` (novo)

**Validação:**
- Carregar programa binário simples
- Montar programa assembly simples
- Executar programa carregado

**Tempo Estimado:** 6-8 horas
**Prioridade:** MÉDIA

---

## FASE 8 - PROGRAMAS EXEMPLO

**Objetivo:** Criar e testar programas MIC-1 funcionais

### Programa 1: Soma de Dois Números
```asm
; Soma dois números em memória
START:  LODD NUM1       ; AC ← NUM1
        ADDD NUM2       ; AC ← AC + NUM2
        STOD RESULT     ; RESULT ← AC
        HALT

NUM1:   .WORD 10        ; Dado 10
NUM2:   .WORD 20        ; Dado 20
RESULT: .WORD 0         ; Resultado
```

### Programa 2: Fibonacci (N=10)
```asm
; Calcula 10 primeiros números Fibonacci
START:  LOCO 0          ; AC ← 0
        STOD FIB_A      ; FIB_A ← 0
        LOCO 1          ; AC ← 1
        STOD FIB_B      ; FIB_B ← 1
        LOCO 10         ; AC ← 10
        STOD COUNT      ; COUNT ← 10

LOOP:   LODD FIB_A      ; AC ← FIB_A
        ADDD FIB_B      ; AC ← FIB_A + FIB_B
        STOD FIB_C      ; FIB_C ← AC
        LODD FIB_B      ; AC ← FIB_B
        STOD FIB_A      ; FIB_A ← FIB_B
        LODD FIB_C      ; AC ← FIB_C
        STOD FIB_B      ; FIB_B ← FIB_C
        LODD COUNT      ; AC ← COUNT
        SUBD ONE        ; AC ← COUNT - 1
        STOD COUNT      ; COUNT ← AC
        JPOS LOOP       ; Se AC > 0, goto LOOP
        HALT

FIB_A:  .WORD 0
FIB_B:  .WORD 0
FIB_C:  .WORD 0
COUNT:  .WORD 0
ONE:    .WORD 1
```

### Programa 3: Fatorial (N=5)
```asm
; Calcula fatorial de 5
START:  LOCO 5          ; AC ← 5
        STOD N          ; N ← 5
        LOCO 1          ; AC ← 1
        STOD RESULT     ; RESULT ← 1

LOOP:   LODD RESULT     ; AC ← RESULT
        PUSH            ; Empilha AC
        LODD N          ; AC ← N
        POPA            ; Desempilha → AC
        MULT            ; AC ← RESULT * N (via microrotina)
        STOD RESULT     ; RESULT ← AC
        LODD N          ; AC ← N
        SUBD ONE        ; AC ← N - 1
        STOD N          ; N ← AC
        JPOS LOOP       ; Se AC > 0, goto LOOP
        HALT

N:      .WORD 0
RESULT: .WORD 0
ONE:    .WORD 1
```

### Tasks
- [ ] Implementar diretiva `.WORD`
- [ ] Implementar labels simbólicos
- [ ] Criar 3 programas completos
- [ ] Testar e validar resultados
- [ ] Documentar formato assembly

**Arquivos:**
- `programs/examples/sum.asm`
- `programs/examples/fibonacci.asm`
- `programs/examples/factorial.asm`
- `docs/ASSEMBLY_GUIDE.md` (novo)

**Validação:**
- Soma: RESULT = 30
- Fibonacci: FIB_B = 55 (10º número)
- Fatorial: RESULT = 120 (5!)

**Tempo Estimado:** 4-5 horas
**Prioridade:** BAIXA

---

## FASE 9 - TESTES E VALIDAÇÃO

**Objetivo:** Suite completa de testes automatizados

### Testes Unitários
- [ ] `test_alu.c` - Todas operações ALU
- [ ] `test_shifter.c` - Left/right shift
- [ ] `test_memory.c` - Read/write memória
- [ ] `test_cache.c` - Hit/miss, load block
- [ ] `test_decoders.c` - Seleção registradores
- [ ] `test_control.c` - MIR, MPC, MMUX
- [ ] `test_conversions.c` - ✅ Já implementado

### Testes de Integração
- [ ] `test_datapath.c` - Via de dados completa
- [ ] `test_cycle.c` - Ciclo de execução
- [ ] `test_microcode.c` - Microinstruções
- [ ] `test_programs.c` - Programas completos

### Testes de Regressão
- [ ] Script `run_all_tests.sh`
- [ ] CI/CD: GitHub Actions (`.github/workflows/build.yml`)
- [ ] Cobertura de código (gcov)

### Validações
- [ ] Memory leaks (valgrind)
- [ ] Endereços inválidos
- [ ] Overflow/underflow
- [ ] Div by zero (se implementar)
- [ ] Stack overflow/underflow

**Arquivos:**
- `src/tests/` (expandir)
- `scripts/run_all_tests.sh` (novo)
- `.github/workflows/test.yml` (novo)

**Validação:**
- Todos os testes passando (100%)
- Zero memory leaks
- Zero warnings

**Tempo Estimado:** 6-8 horas
**Prioridade:** ALTA

---

## FASE 10 - DOCUMENTAÇÃO E REFINAMENTOS

**Objetivo:** Documentação completa e polimento final

### Documentação
- [ ] `ARCHITECTURE.md` - Detalhes da arquitetura
- [ ] `MICROCODE.md` - Todas 79 microinstruções
- [ ] `ASSEMBLY_GUIDE.md` - Sintaxe e exemplos
- [ ] `API.md` - Interface das funções
- [ ] `TESTING.md` - Como testar
- [ ] Comentários inline em todo código

### Interface
- [ ] Comando `disassemble` - Mostra microinstrução atual
- [ ] Comando `breakpoint <addr>` - Define breakpoint
- [ ] Comando `continue` - Executa até breakpoint
- [ ] Comando `trace` - Log detalhado de execução
- [ ] Visualização melhorada de registradores
- [ ] Visualização de cache state

### Performance
- [ ] Profiling (gprof)
- [ ] Otimizar hot paths
- [ ] Cache statistics detalhadas
- [ ] Benchmark suite

### Extras
- [ ] Modo verbose/quiet
- [ ] Output para arquivo (log)
- [ ] Carregar múltiplos programas
- [ ] Reset limpo sem restart

**Arquivos:**
- `docs/` (expandir)
- `src/main.c` (melhorar interface)
- `include/` (documentar headers)

**Validação:**
- Documentação 100% completa
- Interface polida e funcional
- Performance aceitável

**Tempo Estimado:** 6-8 horas
**Prioridade:** MÉDIA

---

## RESUMO DAS FASES

| Fase | Status | Objetivo | Tempo | Prioridade |
|------|--------|----------|-------|------------|
| 0 | ✅ | Bug fix crítico | 15min | Crítica |
| 1 | ✅ | Conversões bit/int | 1.5h | Alta |
| 2 | ✅ | Componentes datapath | 30min | Alta |
| 3 | ✅ | Memória + Cache | 45min | Alta |
| 4 | ✅ | Unidade de controle | 4h | Alta |
| 5 | ✅ | Ciclo de execução | 5h | Alta |
| 6 | ⚠️ | Microprograma & ISA | 2h/8-12h | Média |
| 7 | ⏳ | Loader + Montador | 6-8h | Média |
| 8 | ⏳ | Programas exemplo | 4-5h | Baixa |
| 9 | ⏳ | Testes completos | 6-8h | Alta |
| 10 | ⏳ | Documentação | 6-8h | Média |

**Legenda:**
- ✅ Completo
- 🔄 Em progresso
- ⏳ Pendente

---

## MILESTONE TRACKING

### Milestone 1: Componentes Básicos ✅
- Fases 0-3 completas
- Via de dados funcional
- Memória + Cache operacional

### Milestone 2: Controle e Execução ✅
- Fase 4: Unidade de controle ✅
- Fase 5: Ciclo completo ✅
- **Meta:** Executar primeira microinstrução ✅
- **Status:** 9/9 testes passando (100%)

### Milestone 3: Sistema Completo
- Fase 6: Microprograma
- Fase 7: Loader/Montador
- **Meta:** Executar programa assembly completo

### Milestone 4: Produção
- Fase 8: Programas exemplo
- Fase 9: Testes completos
- Fase 10: Documentação
- **Meta:** Sistema robusto e documentado

---

## ORDEM DE IMPLEMENTAÇÃO

**Próximas tarefas (em ordem):**
1. ~~Implementar `run_mir()` (FASE 4)~~ ✅
2. ~~Implementar `run_mpc()` (FASE 4)~~ ✅
3. ~~Implementar `run_mmux()` (FASE 4)~~ ✅
4. ~~Definir microprograma básico (FASE 4)~~ ✅
5. ~~Implementar ciclo completo (FASE 5)~~ ✅
6. ~~Testar execução de microinstruções (FASE 5)~~ ✅
7. Implementar microprograma completo 79 instruções (FASE 6)
8. Criar interpretador IR → microrotinas (FASE 6)

**Dependências críticas:**
- Fase 5 depende de Fase 4
- Fase 6 depende de Fase 5
- Fase 7 depende de Fase 6
- Fase 8 depende de Fase 7

**Bloqueadores:**
- Nenhum no momento
- Fases 0-3 completas fornecem base sólida

---

## ESTIMATIVA TOTAL

**Tempo já investido:** ~14 horas (Fases 0-5, 6A)
**Tempo restante:** ~28-38 horas (Fases 6B-10)
**Tempo total:** ~45-55 horas

**Progresso:** ~65% completo (Milestones 1 & 2 completos, ISA defined)

---

## INSTRUÇÕES FINAIS

**Para continuar o desenvolvimento:**
1. Leia este ROADMAP.md
2. Leia PROJECT_ANALYSIS.md
3. Comece pela FASE 4
4. Siga a ordem das fases
5. Faça commits ao final de cada fase
6. Atualize logs em `docs/archsim_fase_log/`

**Modelo de commit:**
```bash
feat: implement FASE_X - <descrição concisa>

- Task 1 completa
- Task 2 completa
- Task 3 completa

Validação: <como foi testado>
Tempo: <tempo real>
```

**Modelo de log de fase:**
```
═══════════════════════════════════════════════════════════════
FASE_X - <TÍTULO>
═══════════════════════════════════════════════════════════════

DATA: DD/MM/AAAA
STATUS: ✅ COMPLETA / ⚠️ PARCIAL / ❌ PENDENTE

OBJETIVO
--------
<descrição>

TASKS IMPLEMENTADAS
-------------------
- [x] Task 1
- [x] Task 2

VALIDAÇÃO
---------
✓ Item 1
✓ Item 2

COMMITS
-------
<hash> - <mensagem>

ARQUIVOS MODIFICADOS
--------------------
+ novo_arquivo.c
~ arquivo_modificado.c

TEMPO ESTIMADO: X horas
TEMPO REAL: Y horas

IMPACTO
-------
<impacto no projeto>
```

**Bom trabalho! 🚀**
