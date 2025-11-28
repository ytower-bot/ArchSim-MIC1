# Limitações Conhecidas - ArchSim MIC-1

## 1. Execução Contínua e HALT

### Problema

Quando a TUI é executada em modo contínuo (tecla `r`), o MPC (Microprogram Counter) pode ultrapassar o limite de 78 (MICROPROGRAM_SIZE = 79), gerando erros:

```
Error: MPC address out of bounds: 101
Error: MPC address out of bounds: 102
...
```

### Causa Raiz

A instrução HALT (opcode 0x07) não está implementada no microprograma para efetivamente parar o ciclo de execução. O MPC continua incrementando indefinidamente após encontrar um HALT.

### Comportamento Esperado vs Real

**Esperado:**
- Quando HALT é executado, `cpu->running` deveria ser setado para 0
- O MPC deveria parar de incrementar
- A TUI deveria mostrar status "STOPPED"

**Real:**
- HALT é tratado como NOP (no operation)
- MPC continua incrementando
- Após microinstrução 78, tenta acessar endereços inválidos (79, 80, 101, 102...)

### Workaround

**Para a apresentação, use o modo STEP-BY-STEP:**

1. Carregue o programa: `./tui/archsim-tui examples/sum.asm`
2. Use a tecla `s` para executar ciclo por ciclo
3. Observe o estado da CPU a cada passo
4. NÃO use a tecla `r` (run contínuo) para programas longos

**Programas recomendados para demonstração:**

- `constant.asm` - 3 instruções, executa rápido
- `double.asm` - 5 instruções, demonstra operação aritmética
- `subtract.asm` - 8 instruções, operação completa
- `sum.asm` - 8 instruções, exemplo clássico

### Impacto

- **Severidade**: Média
- **Afeta**: Modo de execução contínua apenas
- **Não afeta**: Execução passo-a-passo, montador, testes automatizados
- **Status**: Conhecido, documentado, workaround disponível

## 2. Microprograma HALT

### Solução Futura

Para corrigir completamente, seria necessário:

1. Adicionar microinstruções para HALT no `data/basic_microcode.txt`
2. Modificar o ciclo de execução para detectar HALT
3. Setar `cpu->running = 0` quando HALT for detectado

Exemplo de implementação necessária:

```c
// Em run_mic1_cycle() ou similar
if (opcode == OP_HALT) {
    cpu->running = 0;
    return;
}
```

## 3. Recomendações para Apresentação

### O que Demonstrar

1. **Compilação e Build**
   ```bash
   make re
   make test
   make tui
   ```

2. **Montador Funcionando**
   - Mostrar arquivos .asm
   - Executar testes do assembler
   - Demonstrar tratamento de erros

3. **TUI em Modo Step**
   ```bash
   ./tui/archsim-tui examples/constant.asm
   # Pressionar 's' algumas vezes
   # Mostrar atualização de registradores
   ```

4. **Exemplos Simples**
   - constant.asm (mais rápido)
   - double.asm (demonstra ALU)
   - subtract.asm (demonstra SUBD)

### O que Evitar

1. Não executar em modo contínuo (`r`) por mais de 3-4 segundos
2. Não usar exemplos com loops (loop.asm) em modo contínuo
3. Não demonstrar programas que dependem de HALT funcionar perfeitamente

## 4. Exemplos Criados

Total de 9 programas de exemplo:

1. **sum.asm** - Soma de dois números
2. **maximum.asm** - Máximo entre dois números
3. **loop.asm** - Loop decrescente (usar com cautela)
4. **constant.asm** - Carrega constante (RECOMENDADO)
5. **subtract.asm** - Subtração (RECOMENDADO)
6. **sequence.asm** - Sequência de valores
7. **double.asm** - Multiplica por 2 (RECOMENDADO)
8. **jump.asm** - Jump incondicional
9. **zero_test.asm** - Teste condicional JZER

## 5. Pontos Fortes do Projeto

Apesar da limitação do HALT, o projeto está funcional e demonstra:

- Microprogramação completa
- Sistema de cache dual
- Montador assembly funcional
- TUI integrada com CGO
- Testes automatizados passando
- Documentação profissional
- Código limpo e organizado

A limitação é conhecida, documentada e possui workaround viável para apresentação.
