# PROJECT ANALYSIS - ArchSim-MIC1

**Data:** 28/11/2025
**Status:** Em Desenvolvimento - Fases 0-3 Completas
**Objetivo:** Simulador da Microarquitetura MIC-1

---

## O QUE É O PROJETO

Simulador completo da arquitetura MIC-1 (Micro-1) implementado em C, baseado na especificação do livro "Structured Computer Organization" de Andrew S. Tanenbaum.

**Propósito:**
- Simular execução de microinstruções MIC-1
- Implementar via de dados completa com 16 registradores
- Sistema de memória com cache (mapeamento direto)
- Unidade de controle microprogramada
- Interface interativa para debug e execução

---

## ESTADO ATUAL

### ✅ COMPLETO (Fases 0-3)

#### FASE 0 - Bug Fix Crítico
- Removido malloc desnecessário em `init_register_bank`
- Memory leak eliminado
- Projeto compila sem erros

#### FASE 1 - Funções de Conversão (9/9 testes)
- `bits_to_int()` - Array de bits → inteiro
- `int_to_bits()` - Inteiro → array de bits
- `address_to_int()` - 12 bits → endereço (0-4095)
- `int_to_address()` - Inteiro → 12 bits
- `copy_data()` - Cópia de arrays 16 bits

**Arquivos:** `include/utils/conversions.h`, `src/utils/conversions.c`

#### FASE 2 - Componentes Datapath
- **Shifter:** `run_shifter()`, `get_shifter_control_value()`
- **Decoders A/B:** `run_decoder()`, `select_register()`
- **Decoder C:** `run_decoderC()` com sinal ENC
- **MAR/MBR:** Integração com memória (RD/WR/MBR)

**Arquivos:** `src/shifter.c`, `src/datapath.c`

#### FASE 3 - Sistema de Memória
- **Memória Principal:** 4096 palavras × 16 bits
  - `init_memory()`, `m_read()`, `m_write()`
  - `load_program()` - carrega arquivo binário
- **Cache:** 8 linhas × 4 palavras (mapeamento direto)
  - `cache_lookup()` - hit/miss detection
  - `cache_read()` / `cache_write()` - write-through
  - Decomposição: tag[7] | line[3] | word[2]
  - Estatísticas: hits, misses, hit rate

**Arquivos:** `src/memory.c`, `src/cache.c`

### ✅ ESTRUTURAS DEFINIDAS

**Via de Dados:**
- 16 registradores × 16 bits (PC, AC, IR, TIR, SP, AMASK, SMASK, 0, +1, -1, A-F)
- Latches A/B, Decoders A/B/C
- ALU (4 operações), Shifter, AMUX
- MAR (12 bits), MBR (16 bits)

**Unidade de Controle:**
- MIR (32 bits), MPC (8 bits), MMUX
- Memória de controle (79 microinstruções)

**Headers:** Todos em `include/` (alu.h, cache.h, datapath.h, memory.h, shifter.h, control_unit.h, etc.)

### ⚠️ IMPLEMENTADO PARCIALMENTE

**ALU (src/alu.c):**
- ✅ `alu_add()` - Soma com complemento de 2
- ✅ `alu_and()` - AND bit a bit
- ✅ `alu_pass_a()` - Passa A
- ✅ `alu_not_a()` - NOT A
- ✅ `update_flags()` - N/Z flags
- ✅ `run_alu()` - Executa baseado em control[2]

**Shifter (src/shifter.c):**
- ✅ `lshift()`, `rshift()` - Deslocamento 1 bit
- ✅ `run_shifter()` - Executa baseado em control_sh[2]

**Interface (src/main.c):**
- ✅ Menu interativo completo
- ✅ Comandos: run, step, status, registers, memory, help, quit

### ❌ PENDENTE (Fases 4+)

**Unidade de Controle:**
- MIR: Decodificação de 32 bits → sinais
- MPC: Incremento e carregamento
- MMUX: Lógica de desvio condicional (N/Z flags)
- Microprograma: Definir 79 microinstruções

**Ciclo de Execução:**
- Fetch → Decode → Execute → Update
- Sincronização entre componentes
- Pipeline completo

**Programas:**
- Loader binário funcional
- Montador (assembly → binário)
- Programas de teste (fibonacci, fatorial, etc.)

---

## COMO ESTÁ ESTRUTURADO

```
ArchSim-MIC1/
├── src/
│   ├── main.c              # Interface interativa
│   ├── mic1.c              # Core (init, reset, cycle)
│   ├── alu.c               # ALU completo
│   ├── shifter.c           # Shifter completo
│   ├── datapath.c          # Decoders, registers
│   ├── memory.c            # Memória principal
│   ├── memoryread.c        # Helper de leitura
│   ├── memoryini.c         # Inicialização antiga
│   ├── cache.c             # Cache completo
│   ├── utils/
│   │   └── conversions.c   # Conversões bit/int
│   └── tests/
│       ├── test_alu.c      # Testes ALU
│       └── test_conversions.c  # Testes conversões
│
├── include/
│   ├── mic1.h              # Estrutura principal CPU
│   ├── alu.h               # ALU interface
│   ├── cache.h             # Cache interface
│   ├── datapath.h          # Registradores, decoders
│   ├── memory.h            # Memória interface
│   ├── shifter.h           # Shifter interface
│   ├── control_unit.h      # MIR, MPC, MMUX
│   ├── connections.h       # Barramento C
│   └── utils/
│       └── conversions.h   # Utils
│
├── docs/
│   └── archsim_fase_log/   # Logs das fases 0-3
│
├── .github/workflows/
│   └── build.yml           # CI/CD GitHub Actions
│
├── Dockerfile              # Container para build
├── Makefile                # Build system
└── README.md               # Especificação completa
```

**Tamanho:** ~1762 linhas de código C

---

## COMO PODE SER TESTADO

### Compilação
```bash
make clean
make
```

### Execução
```bash
./mic1_simulator
```

### Comandos Disponíveis
```
> help         # Lista comandos
> status       # Estado da CPU
> registers    # Dump dos 16 registradores
> memory <start> <end>  # Faixa de memória
> step         # Executa 1 ciclo
> run          # Executa continuamente
> quit         # Sair
```

### Testes Unitários (Implementados)
```bash
# Testes de conversão (FASE 1)
./test_conversions
# 9/9 testes passando

# Testes ALU (parcial)
./test_alu
```

### Testes Manuais Possíveis
1. **Registradores:** Verificar inicialização (0, +1, -1, AMASK, SMASK)
2. **ALU:** Testar 4 operações com valores conhecidos
3. **Shifter:** Testar left/right shift
4. **Memória:** Escrever e ler endereços
5. **Cache:** Verificar hits/misses com acesso sequencial

---

## CARACTERÍSTICAS TÉCNICAS

### Microarquitetura MIC-1

**Via de Dados:**
- Largura: 16 bits
- Registradores: 16 (nomeados + genéricos)
- ALU: 4 operações (A+B, A AND B, A, NOT A)
- Shifter: 3 modos (none, left, right)
- Barramento C: conecta shifter → registradores

**Memória:**
- Principal: 4096 palavras × 16 bits (endereço 12 bits)
- Cache: 8 linhas × 4 palavras (mapeamento direto)
- Política: Write-through
- Decomposição endereço: Tag(7) | Line(3) | Word(2)

**Unidade de Controle:**
- Microprogramada (ROM)
- MIR: 32 bits
  - AMUX(1) | COND(2) | ALU(2) | SH(2) | MBR(1) | MAR(1) | RD(1) | WR(1) | ENC(1) | C(4) | B(4) | A(4) | ADDR(8)
- MPC: 8 bits (endereça 256 microinstruções)
- Conjunto: 79 microinstruções definidas

**Ciclo de Clock:**
- Fetch microinstrução (MPC → ROM → MIR)
- Decode sinais de controle (MIR → componentes)
- Execute operação (Datapath)
- Update MPC (MMUX decide próximo endereço)

---

## DEPENDÊNCIAS

### Build
- GCC ou Clang (C99)
- Make
- stdint.h, string.h, stdio.h, stdlib.h

### Runtime
- Linux/Unix (testado)
- macOS (compatível)
- Windows (via WSL ou MinGW)

### Docker (Opcional)
```dockerfile
FROM gcc:latest
WORKDIR /app
COPY . .
RUN make
CMD ["./mic1_simulator"]
```

---

## DECISÕES DE DESIGN

### Já Decididas
1. **Endianess:** Big-endian (bit[0] = MSB)
2. **Tamanho de palavra:** 16 bits
3. **Cache:** Mapeamento direto, write-through
4. **Registradores:** Estrutura nomeada (não array)
5. **Conversões:** Funções dedicadas (não macros)
6. **Interface:** CLI interativa (não GUI)

### Pendentes
1. **Microprograma:** Criar manualmente ou tool?
2. **Assembly syntax:** Definir formato exato
3. **Loader format:** Binário puro ou com header?
4. **Debug level:** Quanto logging?
5. **Testes:** Suite automática ou manual?

---

## MÉTRICAS

**Código Implementado:**
- Linhas: ~1762
- Arquivos: 12 .c + 9 .h
- Funções: ~80+
- Testes: 9 unitários (conversões)

**Cobertura Funcional:**
- Via de Dados: ~70%
- Memória: ~90%
- Unidade de Controle: ~10%
- Ciclo Execução: 0%
- Microprograma: 0%

**Status Geral:** ~45% completo

---

## PRÓXIMAS ETAPAS

**Imediato (Fase 4):**
1. Implementar `run_mir()` - distribuir sinais
2. Implementar `run_mpc()` - incremento/carregamento
3. Implementar `run_mmux()` - lógica de desvio
4. Definir microprograma básico (10-20 instruções)

**Curto Prazo (Fase 5):**
5. Ciclo completo MIC-1 (fetch-decode-execute)
6. Conectar todos os componentes
7. Testar execução de microinstruções simples

**Médio Prazo (Fases 6-7):**
8. Microprograma completo (79 instruções)
9. Loader binário
10. Montador assembly

**Longo Prazo (Fase 8):**
11. Suite de testes completa
12. Programas exemplo (fibonacci, fatorial)
13. Documentação final

---

## PROBLEMAS CONHECIDOS

### Resolvidos
- ✅ malloc desnecessário em datapath (FASE_0)
- ✅ Faltavam funções de conversão (FASE_1)
- ✅ Componentes datapath incompletos (FASE_2)
- ✅ Sistema de memória não funcional (FASE_3)

### Pendentes
- ⚠️ Unidade de controle não executa microinstruções
- ⚠️ Ciclo de clock não implementado
- ⚠️ Microprograma não definido
- ⚠️ Loader não funciona
- ⚠️ Sem montador assembly

---

## REFERÊNCIAS

**Livro Base:**
- "Structured Computer Organization" (6th ed.) - Andrew S. Tanenbaum
- Capítulo 4: The Microarchitecture Level

**Arquivos Chave:**
- `README.md` - Especificação completa da arquitetura
- `docs/archsim_fase_log/` - Logs de implementação
- `include/mic1.h` - Estrutura principal da CPU
- `PROJECT_ANALYSIS.md` - Este arquivo
- `ROADMAP.md` - Plano de implementação

**Commits Importantes:**
- `ffe0d4e` - FASE_0: Fix malloc bug
- `4611bef` - FASE_1: Conversion functions
- (pendente) - FASE_2: Datapath components
- (pendente) - FASE_3: Memory system

---

## CONCLUSÃO

**Estado:** Projeto em desenvolvimento ativo, ~45% completo.

**Ponto Forte:** Base sólida de estruturas de dados e componentes de baixo nível (ALU, Shifter, Memória, Cache).

**Próximo Milestone:** Implementar unidade de controle e executar primeira microinstrução completa.

**Estimativa:** 4-6 fases restantes para simulador completo funcional.
