# ArchSim-MIC1

Simulador educacional da microarquitetura MIC-1 baseado na especificação de Andrew Tanenbaum.

## Visão Geral

O ArchSim-MIC1 é uma implementação completa em C da microarquitetura MIC-1 descrita no livro "Structured Computer Organization". O simulador foi desenvolvido para fins educacionais e permite o estudo detalhado dos componentes internos de um processador microprogramado.

### Características Principais

- **Execução de microprogramas** com ciclo fetch-decode-execute completo
- **Montador assembly** com suporte a 8 instruções básicas
- **Caches separadas** para dados e instruções (8 linhas, mapeamento direto)
- **Componentes explícitos** da microarquitetura (ALU, Shifter, Decoders, MAR/MBR, etc.)
- **Testes automatizados** para validação de todos os componentes

## Arquitetura Implementada

### Componentes da Via de Dados (Datapath)

| Componente | Descrição |
|------------|-----------|
| Register Bank | 16 registradores de 16 bits (PC, AC, SP, IR, TIR, constantes, etc.) |
| ALU | Unidade Lógica Aritmética com 4 operações (A, B, A+B, A AND B) |
| Shifter | Deslocador de 1 bit (esquerda/direita) |
| Decoder A/B | Seleção de registradores para Latch A/B |
| Decoder C | Escrita em registradores com sinal ENC |
| MAR/MBR | Acesso à memória (endereço + dados) |
| AMUX | Multiplexador de entrada da ALU (Latch A ou MBR) |
| Latch A/B | Latches de 16 bits |

### Unidade de Controle

| Componente | Descrição |
|------------|-----------|
| MIR | Registrador de microinstrução (32 bits) |
| MPC | Contador de microprograma (8 bits) |
| MMUX | Multiplexador de microendereço (desvios condicionais) |
| Control Memory | Memória de controle (79 microinstruções) |

### Sistema de Memória

| Componente | Descrição |
|------------|-----------|
| Memória Principal | 4096 palavras × 16 bits |
| Data Cache | 8 linhas × 4 palavras (mapeamento direto) |
| Instruction Cache | 8 linhas × 4 palavras (independente da data cache) |

### Montador (Assembler)

| Recurso | Descrição |
|---------|-----------|
| Instruções | LODD, STOD, ADDD, SUBD, LOCO, JUMP, JZER, HALT |
| Labels | Suporte a labels simbólicos |
| Comentários | Linhas com `;` são ignoradas |
| Formato binário | 16 bits (8-bit opcode + 8-bit operand) |

## Início Rápido

### Opção 1: Docker (Recomendado)

```bash
# Build da imagem
docker build -t archsim-mic1 .

# Executar container
docker run -it archsim-mic1

# Rodar testes
docker run -it archsim-mic1 make test
```

Veja [docs/DOCKER.md](docs/DOCKER.md) para instruções detalhadas.

### Opção 2: Compilação Local

**Requisitos:**
- GCC 7.0+
- Make

**Compilação e testes:**

```bash
# Compilar simulador
make all

# Rodar todos os testes
make test

# Compilar com debug
make debug

# Limpar build
make clean
```

## Interface Visual (TUI)

O projeto inclui uma Terminal User Interface (TUI) para visualização e execução interativa de programas.

### Compilando a TUI

```bash
# Compilar backend C e TUI
make all
make tui
```

### Executando

```bash
# Executar com um programa
./tui/archsim-tui examples/sum.asm

# Ou executar sem argumentos e carregar depois
./tui/archsim-tui
```

### Interface

A TUI é dividida em dois painéis:

**Painel Esquerdo - Código Fonte**
- Código assembly carregado
- Instrução atual marcada com `->`
- Numeração de linhas

**Painel Direito - Estado da CPU**
- Status de execução (RUNNING/STOPPED)
- Contadores de ciclos e clock
- Valores dos registradores (PC, AC, SP, IR, TIR)
- Estado da unidade de controle (MPC, flags N e Z)
- Conteúdo de memória em endereços relevantes
- Estatísticas de cache (taxa de hit)

### Controles

| Tecla | Ação |
|-------|------|
| `s` | Executar um ciclo |
| `r` | Executar continuamente / Pausar |
| `x` | Resetar CPU |
| `l` | Carregar arquivo padrão |
| `h` ou `?` | Mostrar/ocultar ajuda |
| `q` ou `Ctrl+C` | Sair |

Para mais detalhes, veja [tui/README.md](tui/README.md).

## Uso do Montador

### Sintaxe Assembly

```asm
; Comentário (linha ignorada)
LABEL:  LODD 100    ; Load direto: AC <- Memory[100]
        ADDD 101    ; Add direto:  AC <- AC + Memory[101]
        STOD 102    ; Store:       Memory[102] <- AC
        HALT        ; Para execução
```

### Instruções Suportadas

| Mnemônico | Formato | Operação | Opcode |
|-----------|---------|----------|--------|
| `LODD addr` | `LODD 100` | `AC <- Memory[addr]` | 0x00 |
| `STOD addr` | `STOD 100` | `Memory[addr] <- AC` | 0x01 |
| `ADDD addr` | `ADDD 100` | `AC <- AC + Memory[addr]` | 0x02 |
| `SUBD addr` | `SUBD 100` | `AC <- AC - Memory[addr]` | 0x03 |
| `LOCO const` | `LOCO 42` | `AC <- const` | 0x04 |
| `JUMP addr` | `JUMP LOOP` | `PC <- addr` | 0x05 |
| `JZER addr` | `JZER END` | `if (AC == 0) PC <- addr` | 0x06 |
| `HALT` | `HALT` | Para execução | 0x07 |

### Exemplos de Programas

O diretório `examples/` contém programas assembly prontos:

```bash
examples/sum.asm         # Soma de dois números
examples/loop.asm        # Loop com contador
examples/maximum.asm     # Encontra valor máximo
```

### API do Montador

```c
#include "assembler.h"

// Montar código assembly
const char *code = "LODD 100\nADDD 101\nSTOD 102\nHALT\n";
uint16_t output[MAX_INSTRUCTIONS];
int size;

if (assemble(code, output, &size) == 0) {
    printf("Montagem bem-sucedida: %d instruções\n", size);
}
```

Veja [docs/ASSEMBLER_GUIDE.md](docs/ASSEMBLER_GUIDE.md) para documentação completa.

## Testes

O projeto inclui testes automatizados para todos os componentes principais:

```bash
# Rodar todos os testes
make test
```

**Testes disponíveis:**
- `test_assembler` - Testa montador (3 casos)
- `test_cache_separation` - Testa caches independentes (3 casos)
- `test_debug` - Testes de debug gerais

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

## Hierarquia de Memória

### Cache de Dados
- 8 linhas × 4 palavras
- Mapeamento direto
- Política de escrita: write-through
- Tracking de hits/misses

### Cache de Instruções
- 8 linhas × 4 palavras
- Independente da cache de dados
- Mapeamento direto
- Otimizada para fetch sequencial

### Decomposição de Endereço (12 bits)

```
[TAG: 7 bits][LINE: 3 bits][WORD: 2 bits]
```

- **TAG (7 bits):** Identificador do bloco na memória
- **LINE (3 bits):** Qual das 8 linhas da cache
- **WORD (2 bits):** Qual das 4 palavras dentro da linha

## Estrutura do Projeto

```
ArchSim-MIC1/
├── include/              # Headers
│   ├── alu.h
│   ├── assembler.h      # API do montador
│   ├── cache.h
│   ├── control_unit.h
│   ├── datapath.h
│   ├── memory.h
│   ├── mic1.h           # CPU principal
│   ├── shifter.h
│   └── utils/
│       └── conversions.h
├── src/                  # Implementação
│   ├── alu.c
│   ├── assembler.c      # Montador
│   ├── cache.c
│   ├── control_unit.c
│   ├── datapath.c
│   ├── main.c           # Programa principal
│   ├── memory.c
│   ├── mic1.c           # Ciclo de execução
│   ├── shifter.c
│   └── utils/
│       └── conversions.c
├── tui/                  # Terminal User Interface
│   ├── main.go          # Ponto de entrada TUI
│   ├── model.go         # Estado e lógica
│   ├── view.go          # Renderização
│   ├── update.go        # Handlers de eventos
│   ├── cgo_wrapper.go   # Integração C/Go
│   └── README.md        # Documentação TUI
├── tests/                # Testes automatizados
│   ├── test_assembler.c
│   ├── test_cache_separation.c
│   └── test_debug.c
├── data/
│   └── basic_microcode.txt  # Microprograma (79 microinstruções)
├── docs/
│   ├── archsim_fase_log/    # Logs de desenvolvimento
│   ├── ArchSim-MIC1.md      # Documentação técnica completa
│   ├── ASSEMBLER_GUIDE.md   # Guia do montador
│   ├── DOCKER.md            # Instruções Docker
│   └── ROADMAP.md           # Roadmap do projeto
├── examples/             # Programas assembly de exemplo
│   ├── sum.asm
│   ├── loop.asm
│   └── maximum.asm
├── Dockerfile            # Container Docker
├── Makefile              # Sistema de build
└── README.md             # Este arquivo
```

## Documentação

### Documentação Técnica

- **[ArchSim-MIC1.md](docs/ArchSim-MIC1.md)** - Documentação técnica completa
  - Arquitetura detalhada
  - Componentes do datapath
  - Unidade de controle
  - Sistema de memória e caches
  - Ciclo de execução
  - Referência de sinais

- **[ASSEMBLER_GUIDE.md](docs/ASSEMBLER_GUIDE.md)** - Guia completo do montador
  - Sintaxe assembly
  - Conjunto de instruções
  - Exemplos de programas
  - API do montador

- **[DOCKER.md](docs/DOCKER.md)** - Instruções para uso com Docker

- **[ROADMAP.md](docs/ROADMAP.md)** - Planejamento e fases do projeto

### Exemplos de Código

O diretório `examples/` contém programas assembly funcionais que demonstram o uso do simulador.

## Status do Projeto

### Implementado

- Via de dados completa (ALU, Shifter, Registradores, Decoders)
- Unidade de controle microprogramada (MIR, MPC, MMUX)
- Sistema de memória hierárquico (memória principal + caches)
- Montador assembly com 8 instruções
- Terminal User Interface (TUI) interativa
- Suite de testes automatizados
- Containerização Docker
- Documentação completa

### Melhorias Futuras

- Programas exemplo adicionais
- Ferramentas de debug avançadas
- Exportação de trace de execução

## Contribuindo

Este é um projeto educacional desenvolvido para fins acadêmicos. Contribuições são bem-vindas através de issues e pull requests.

## Referências

- **Tanenbaum, A. S.** - *Structured Computer Organization* (6th Edition)
  - Capítulo 4: The Microarchitecture Level
- **Patterson, D. A. & Hennessy, J. L.** - *Computer Organization and Design*

## Licença

Projeto educacional de código aberto para fins acadêmicos.

---

**Última Atualização:** 28/11/2025  
**Versão:** 1.0
