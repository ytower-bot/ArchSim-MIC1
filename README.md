# ArchSim-MIC1

Simulador educacional da microarquitetura MIC-1 baseado na especificação de Andrew Tanenbaum.

## Características

- Implementação completa da via de dados MIC-1
- Unidade de controle microprogramada com 256 microinstruções
- Sistema de memória com cache unificada (8 linhas × 4 palavras)
- Montador assembly para instruções IJVM
- Interface TUI (Terminal User Interface) interativa em Go
- Suporte para todas as 23 instruções IJVM
- Integração C/Go via CGO

## Compilação

```bash
make clean       # Limpar artefatos de build
make all         # Compilar simulador + assembler
make tui         # Compilar interface TUI
```

## Verificação (Zero-Trust Protocol)

```bash
make verify      # Executa suite completa de testes (unit + integration + asm)
make test        # Executa testes tradicionais
```

## Uso

### TUI (Terminal User Interface)

```bash
make tui-run
```

Controles:
- `l` - Abrir seletor de arquivos .asm
- `s` - Executar um ciclo
- `r` - Executar continuamente / Pausar
- `x` - Resetar CPU
- `d` - Alternar visualização detalhada de memória
- `f` - Alternar formato (hex/decimal/ASCII)
- `h` - Ajuda
- `q` - Sair

### Montador

```bash
./mic1asm input.asm output.bin
```

## Instruções IJVM

### Instruções com Operando de 12 bits (0x0-0xE)
- LODD addr - Load Direct
- STOD addr - Store Direct
- ADDD addr - Add Direct
- SUBD addr - Subtract Direct
- JPOS addr - Jump if Positive
- JZER addr - Jump if Zero
- JUMP addr - Unconditional Jump
- LOCO const - Load Constant
- LODL offset - Load Local
- STOL offset - Store Local
- ADDL offset - Add Local
- SUBL offset - Subtract Local
- JNEG addr - Jump if Negative
- JNZE addr - Jump if Not Zero
- CALL addr - Call Procedure

### Instruções Especiais (0xF)
- PSHI - Push Indirect
- POPI - Pop Indirect
- PUSH - Push AC
- POP - Pop to AC
- RETN - Return
- SWAP - Swap AC and SP
- INSP - Increment SP
- DESP - Decrement SP

## Arquitetura

### Registradores
- PC - Program Counter
- AC - Accumulator
- SP - Stack Pointer
- IR - Instruction Register
- TIR - Temporary Instruction Register
- MPC - Microprogram Counter

### Memória
- 4096 palavras × 16 bits
- Cache unificada: 8 linhas × 4 palavras

### ALU
- 4 operações: A, B, A+B, A AND B
- Flags: N (negative), Z (zero)

## Documentação

- `docs/ArchSim-MIC1.md` - Especificação técnica completa
- `docs/ASSEMBLER_GUIDE.md` - Guia do montador
- `docs/DOCKER.md` - Instruções Docker
- `tui/README.md` - Documentação da TUI
- `tui/TUI_GUIDE.md` - Guia de uso da TUI

## Requisitos

- GCC ou Clang (C99)
- Go 1.24+ (para TUI)
- Make

## Estrutura do Projeto

```
ArchSim-MIC1/
├── src/          # Código fonte C (simulador core)
│   ├── mic1.c           # CPU principal e ciclo de execução
│   ├── datapath.c       # Via de dados (decoders, latches)
│   ├── control_unit.c   # Unidade de controle (MIR, MPC, MMUX)
│   ├── alu.c            # Unidade lógica aritmética
│   ├── shifter.c        # Deslocador
│   ├── memory.c         # Memória principal e cache
│   ├── assembler.c      # Montador IJVM
│   └── utils/           # Utilitários (conversões)
├── include/      # Headers C
├── data/         # Microcódigo (basic_microcode.txt)
├── examples/     # Programas exemplo em assembly
├── docs/         # Documentação técnica
└── tui/          # Interface TUI (Go + CGO)
    ├── cgo_wrapper.go   # Ponte C/Go
    ├── model.go         # Modelo de dados
    ├── view.go          # Renderização
    └── update.go        # Lógica de atualização
```

## Arquitetura Técnica

O projeto utiliza uma arquitetura híbrida C/Go:

1. **Backend C**: Implementa toda a lógica do simulador MIC-1
2. **CGO Wrapper**: Interface entre C e Go (`cgo_wrapper.go`)
3. **Frontend Go**: Interface TUI usando `bubbletea` e `lipgloss`

A comunicação ocorre através de funções C exportadas via CGO, permitindo que o Go chame funções C e acesse estruturas de dados do simulador.
