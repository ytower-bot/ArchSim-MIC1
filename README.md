# ArchSim-MIC1

Simulador educacional da microarquitetura MIC-1 baseado na especificacao de Andrew Tanenbaum.

## Caracteristicas

- Implementacao completa da via de dados MIC-1
- Unidade de controle microprogramada
- Sistema de memoria com cache unificada
- Montador assembly para instrucoes da ISA Tanenbaum
- Modo headless com trace de execucao

## Compilacao

```bash
make all      # Compila simulador + assembler
make clean    # Remove artefatos
make fclean   # Remove tudo (binarios inclusos)
```

## Uso

### Montador

```bash
./mic1asm input.asm              # Gera input.bin
./mic1asm input.asm output.bin   # Gera output.bin
```

### Simulador

```bash
./mic1_simulator program.bin         # Executa 50 ciclos (padrao)
./mic1_simulator program.bin 1000    # Executa 1000 ciclos
```

O simulador imprime um trace de execucao mostrando:
- Estado dos registradores (PC, AC, SP, IR) por ciclo
- Instrucao decodificada e seu significado
- Estado final da memoria

### Verificacao

```bash
make verify   # Monta teste e executa simulador
```

## Instrucoes da ISA

### Com Operando de 12 bits (0x0-0xE)

| Opcode | Instrucao | Descricao |
|--------|-----------|-----------|
| 0x0 | LODD addr | AC <- M[addr] |
| 0x1 | STOD addr | M[addr] <- AC |
| 0x2 | ADDD addr | AC <- AC + M[addr] |
| 0x3 | SUBD addr | AC <- AC - M[addr] |
| 0x4 | JPOS addr | if AC > 0: PC <- addr |
| 0x5 | JZER addr | if AC == 0: PC <- addr |
| 0x6 | JUMP addr | PC <- addr |
| 0x7 | LOCO const | AC <- const |
| 0x8 | LODL offset | AC <- M[SP + offset] |
| 0x9 | STOL offset | M[SP + offset] <- AC |
| 0xA | ADDL offset | AC <- AC + M[SP + offset] |
| 0xB | SUBL offset | AC <- AC - M[SP + offset] |
| 0xC | JNEG addr | if AC < 0: PC <- addr |
| 0xD | JNZE addr | if AC != 0: PC <- addr |
| 0xE | CALL addr | SP <- SP - 1; M[SP] <- PC; PC <- addr |

### Instrucoes Especiais (0xF)

| Subop | Instrucao | Descricao |
|-------|-----------|-----------|
| 0xF0 | PSHI | Push M[AC] |
| 0xF2 | POPI | Pop to M[AC] |
| 0xF4 | PUSH | Push AC |
| 0xF6 | POP | Pop to AC |
| 0xF8 | RETN | Return |
| 0xFA | SWAP | Swap AC, SP |
| 0xFC | INSP n | SP <- SP + n |
| 0xFE | DESP n | SP <- SP - n |

## Arquitetura

### Registradores
- **PC** - Program Counter (16 bits)
- **AC** - Accumulator (16 bits)
- **SP** - Stack Pointer (16 bits)
- **IR** - Instruction Register (16 bits)

### Memoria
- 4096 palavras x 16 bits

### ALU
- Operacoes: A, B, A+B, A AND B
- Flags: N (negative), Z (zero)

## Estrutura do Projeto

```
ArchSim-MIC1/
├── src/           # Codigo C
│   ├── mic1.c     # CPU e ciclo de execucao
│   ├── main.c     # CLI e trace logger
│   ├── alu.c      # Unidade logica aritmetica
│   ├── memory.c   # Sistema de memoria
│   └── mic1asm.c  # Montador
├── include/       # Headers
├── tests/         # Programas de teste (.asm)
├── examples/      # Exemplos
├── docs/          # Documentacao
├── Makefile
└── Dockerfile
```

## Requisitos

- GCC (C99)
- Make

## Docker

```bash
make docker-build   # Constroi imagem
make docker-test    # Executa verificacao no container
make docker-shell   # Shell interativo
```

## Documentacao

- `docs/ArchSim-MIC1.md` - Especificacao tecnica
- `docs/ASSEMBLER_GUIDE.md` - Guia do montador
- `docs/MICROCODE.md` - Formato do microcodigo
