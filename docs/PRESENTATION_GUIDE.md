# Guia Rápido de Apresentação - ArchSim MIC-1

## Preparação (Antes da Apresentação)

```bash
# 1. Compilar tudo
make re
make tui

# 2. Verificar que está tudo OK
make test
ls -lh mic1_simulator tui/archsim-tui
```

## Roteiro de Demonstração

### 1. Introdução (2 min)

"O ArchSim-MIC1 é um simulador educacional da microarquitetura MIC-1, implementado em C com uma interface visual em Go."

**Mostrar:**
- Estrutura de diretórios: `tree -L 1`
- Documentação: `ls docs/`

### 2. Montador Assembly (3 min)

"O projeto inclui um montador que converte assembly em código de máquina."

```bash
# Mostrar um exemplo simples
cat examples/constant.asm

# Executar testes do montador
./tests/test_assembler
```

**Explicar:**
- 8 instruções: LODD, STOD, ADDD, SUBD, LOCO, JUMP, JZER, HALT
- Suporte a labels
- Comentários com `;`

### 3. TUI - Exemplo Simples (5 min)

"A TUI permite visualizar a execução passo-a-passo."

```bash
# Carregar exemplo mais simples
./tui/archsim-tui examples/constant.asm
```

**Demonstrar na TUI:**
1. Mostrar os dois painéis (código | estado CPU)
2. Pressionar `h` para mostrar ajuda
3. Pressionar `h` novamente para fechar
4. Pressionar `s` três vezes (step-by-step)
   - Observar PC incrementando
   - Observar AC mudando para 42
   - Observar memória[200] recebendo 42
5. Pressionar `q` para sair

### 4. TUI - Exemplo com Operação (5 min)

```bash
# Exemplo com soma
./tui/archsim-tui examples/sum.asm
```

**Demonstrar:**
1. Pressionar `s` repetidamente
2. Apontar para:
   - Linha atual marcada com `->`
   - Registradores mudando (PC, AC)
   - Memória sendo escrita (0x100, 0x101, 0x102)
   - Contadores de ciclo e clock
   - Cache stats

### 5. Arquitetura do Projeto (3 min)

"O simulador implementa todos os componentes da microarquitetura."

**Mostrar código:**
```bash
# Estrutura modular
ls include/
ls src/

# Componentes principais
cat include/mic1.h | head -30
```

**Explicar:**
- Register Bank (16 registradores)
- ALU (4 operações)
- Cache dual (dados + instruções)
- Microprograma (79 microinstruções)

### 6. Testes Automatizados (2 min)

```bash
# Rodar suite de testes
make test
```

**Apontar:**
- 3 testes passando
- Assembler funcional
- Caches independentes
- Decode de microinstruções

### 7. Exemplos Disponíveis (2 min)

```bash
# Mostrar todos os exemplos
ls -1 examples/

# Contar linhas de cada um
wc -l examples/*.asm
```

**Mencionar:**
- 9 programas de exemplo
- Do mais simples (3 instruções) ao mais complexo (13 instruções)
- Demonstram diferentes aspectos da ISA

## Perguntas Comuns

### "O HALT funciona?"

"O HALT está implementado no montador, mas o microprograma não para completamente o MPC. Por isso, recomendamos usar o modo step-by-step (`s`) em vez do modo contínuo (`r`). Isso está documentado em `docs/KNOWN_ISSUES.md`."

### "Quantas instruções suporta?"

"8 instruções básicas: LODD, STOD, ADDD, SUBD, LOCO, JUMP, JZER, HALT. É suficiente para demonstrar conceitos de microprogramação e criar programas úteis."

### "Como funciona a cache?"

"Temos duas caches independentes: uma para dados e outra para instruções. Cada uma tem 8 linhas de 4 palavras, com mapeamento direto. Os testes em `tests/test_cache_separation` demonstram a independência."

### "Por que Go para a TUI?"

"Go com Bubble Tea permite criar TUIs elegantes rapidamente. A integração com C é feita via CGO, mantendo o backend de alto desempenho enquanto aproveitamos as bibliotecas Go para interface."

## Comandos Úteis

```bash
# Recompilar tudo
make re

# Rodar testes
make test

# TUI com exemplo
./tui/archsim-tui examples/constant.asm

# Ver ajuda do Makefile
make help

# Limpar build
make fclean
```

## Exemplos por Complexidade

**Iniciante:**
- constant.asm (3 inst.)
- double.asm (5 inst.)

**Intermediário:**
- subtract.asm (8 inst.)
- sum.asm (8 inst.)
- sequence.asm (7 inst.)

**Avançado:**
- jump.asm (7 inst.)
- zero_test.asm (10 inst.)
- maximum.asm (13 inst.)
- loop.asm (5 inst.)

## Tempo Total Sugerido: 22 minutos

- Introdução: 2 min
- Montador: 3 min
- TUI Simples: 5 min
- TUI Operação: 5 min
- Arquitetura: 3 min
- Testes: 2 min
- Exemplos: 2 min

Deixe tempo para perguntas!
