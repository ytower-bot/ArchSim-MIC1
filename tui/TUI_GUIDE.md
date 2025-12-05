# Como Testar a TUI

## Executando a TUI

```bash
cd /Users/yuri/Code/uff/arq_comp/ArchSim-MIC1/tui
./tui ../examples/test_alu.asm
```

## Controles

### Execução
- **`s`** - Step (executar 1 ciclo)
- **`r`** - Run/Pause (executar continuamente)
- **`x`** - Reset CPU
- **`l`** - Load arquivo ASM
- **`m`** - Load microcode

### Visualização
- **`d`** - Toggle detailed memory view
- **`f`** - Toggle formato (hex/decimal/ASCII)
- **`c`** - Toggle cache state
- **`v`** - View microcode
- **`h`** - Help
- **`q`** - Quit

## O Que Observar

### test_registers.asm 
Deve funcionar perfeitamente!

1. Pressione `s` várias vezes para step through
2. Observe o **AC** mudando conforme o programa carrega valores
3. Observe a área **Results (0x100-0x10F)** sendo populada:
   ```
   [0x100]: 0x002A = 42 
   [0x101]: 0x0063 = 99 
   [0x102]: 0x004D = 77 
   ```
4. O marcador `` indica que o valor mudou

### test_alu.asm 
Deve funcionar! Testa operações aritméticas.

1. Step through com `s`
2. Observe cálculos acontecendo:
   - **10 + 20 = 30** → aparece em `[0x100]`
   - **15 - 10 = 5** → aparece em `[0x101]`
   - **25 + 25 = 50** → aparece em `[0x102]`
3. Veja os valores em **hex E decimal** simultaneamente
4. Use `d` para ver detailed memory view com Code e Stack regions

### test_stack.asm 
**AGORA FUNCIONA!** Bug do microcode foi corrigido!

1. Pressione `s` para executar step by step
2. Observe:
   - **SP muda** quando PUSH/POP executam!
   - SP começa em 1000 (inicializado pelo programa)
   - PUSH 10: SP vai para 999, Memory[999] = 10
   - PUSH 20: SP vai para 998, Memory[998] = 20
   - PUSH 30: SP vai para 997, Memory[997] = 30
   - POP: SP volta para 998, AC recebe 30
   - POP: SP volta para 999, AC recebe 20
   - POP: SP volta para 1000, AC recebe 10
3. Resultados finais em 0x100-0x102: 10, 20, 30 (ordem LIFO)

**Status**:  CONSERTADO! Ver `docs/MICROCODE_SP_FIX.md`

## Novas Funcionalidades

### 1. Visualização Intuitiva
Registradores agora mostram hex E decimal:
```
AC:  0x002A = 42
SP:  0x0FFF = 4095
```

### 2. Results Area
Mostra automaticamente valores não-zero em 0x100-0x10F:
```
Results (0x100-0x10F):
[0x100]: 0x001E = 30 
[0x101]: 0x0005 = 5 
(no data yet)
```

### 3. Detailed Memory View (tecla 'd')
- **Code Region**: 0x00-0x3F (onde instruções ficam)
- **Stack Region**: Dinâmica ao redor do SP
- **Removida**: Data Region (não era usada)

### 4. Compact Display
- Cache só aparece quando há espaço na tela (>30 linhas)
- Tudo respeita bounds do terminal
- Sem overflow de texto

## Troubleshooting

### TUI não inicia
```bash
# Rebuild completo
cd /Users/yuri/Code/uff/arq_comp/ArchSim-MIC1
make clean && make
ar rcs obj/libmic1.a obj/*.o obj/utils/*.o
cd tui && go build -o tui
```

### SIGBUS error
Normal se tentar executar TUI sem terminal interativo (ex: via pipe).
Use direto no terminal.

### Programa não executa
1. Verifique se arquivo .asm existe
2. Verifique se microcode foi carregado (mensagem no topo)
3. Use `x` para reset e tente novamente

## Exemplos Incluídos

- **test_registers.asm** - Operações básicas (LOCO, STOD, LODD)
- **test_alu.asm** - Aritmética (ADD, SUB)
- **test_stack.asm** - Stack ops (PUSH, POP) - BROKEN devido a bug microcode

## Próximos Passos

1. Consertar microcode PUSH/POP
2. Testar test_stack.asm novamente
3. Criar mais exemplos (loops, branches)
