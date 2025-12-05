# ArchSim-MIC1

Simulador educacional da microarquitetura MIC-1 baseado na especificação de Andrew Tanenbaum.

## Características

- Implementação completa da via de dados MIC-1
- Unidade de controle microprogramada
- Sistema de memória com cache unificada
- Montador assembly para instruções IJVM
- Interface TUI (Terminal User Interface) interativa
- Suporte para todas as 23 instruções IJVM

## Compilação

```bash
make clean
make all
make tui
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
├── src/          # Código fonte C
├── include/      # Headers
├── data/         # Microcódigo
├── examples/     # Programas exemplo
├── docs/         # Documentação técnica
└── tui/          # Interface TUI (Go)
```
