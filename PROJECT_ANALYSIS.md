# ArchSim-MIC1 - Análise e Plano de Estruturação

**Data:** 2025-11-25
**Status:** Em Desenvolvimento
**Objetivo:** Simulador completo da microarquitetura MIC-1

---

## 1. SITUAÇÃO ATUAL

### 1.1 Problema Identificado
- **Erro de Compilação:** `src/memoryini.c` está incompleto (função main sem fechamento)
- **Projeto não roda:** Necessário corrigir erros e implementar funcionalidades faltantes

### 1.2 Estrutura de Arquivos Existente

```
ArchSim-MIC1/
├── src/
│   ├── main.c           ✅ Completo (interface interativa)
│   ├── mic1.c           ⚠️  Stubs (estrutura pronta, lógica pendente)
│   ├── datapath.c       ⚠️  Incompleto
│   ├── memoryini.c      ❌ Erro de sintaxe
│   └── memoryread.c     ⚠️  Desconhecido
├── include/
│   ├── mic1.h           ✅ Estrutura principal definida
│   ├── datapath.h       ✅ Estruturas definidas
│   ├── alu.h            ✅ Estruturas definidas
│   ├── memory.h         ✅ Estruturas definidas
│   ├── shifter.h        ✅ Estruturas definidas
│   ├── control_unit.h   ✅ Estruturas definidas
│   ├── cache.h          ✅ Estruturas definidas
│   ├── connections.h    ⚠️  Não analisado
│   └── utils/
│       └── conversions.h ⚠️  Não analisado
└── Makefile             ✅ Estrutura correta

✅ Completo e funcional
⚠️  Parcial ou desconhecido
❌ Com problemas
```

---

## 2. COMPONENTES DO MIC-1

### 2.1 Via de Dados (Datapath)

#### Registradores (16 bits cada)
- **PC** - Program Counter
- **AC** - Accumulator
- **IR** - Instruction Register
- **TIR** - Temporary Instruction Register
- **SP** - Stack Pointer (inicial: 4096)
- **AMASK** - Address Mask (0000111111111111)
- **SMASK** - Sign Mask (0000000011111111)
- **0** - Constante 0
- **+1** - Constante 1
- **-1** - Constante -1
- **A, B, C, D, E, F** - Registradores de propósito geral

**Status:** ✅ Estruturas definidas em `datapath.h`

#### Latches A e B
- Armazenam valores dos registradores
- Latch A → AMUX → ULA entrada A
- Latch B → MAR e ULA entrada B

**Status:** ✅ Estruturas definidas

#### Decoders A, B e C
- Decoder A/B: Selecionam registrador fonte (4 bits de controle)
- Decoder C: Seleciona registrador destino (4 bits + ENC)

**Status:** ✅ Estruturas definidas, ⚠️ Lógica não implementada

#### MAR (Memory Address Register)
- 12 bits de endereço
- Recebe valor do Latch B quando control_mar = 1

**Status:** ✅ Estrutura definida, ⚠️ Lógica stub

#### MBR (Memory Buffer Register)
- 16 bits de dados
- Controles: RD, WR, MBR
- Interface com memória e AMUX

**Status:** ✅ Estrutura definida, ⚠️ Lógica stub

#### ULA (ALU)
- 4 operações: A+B, A AND B, A, NOT A
- 2 flags: N (negativo), Z (zero)
- Controle de 2 bits

**Status:** ✅ Estrutura definida, ⚠️ Operações não implementadas

#### Deslocador (Shifter)
- Operações: None, Left Shift, Right Shift
- Controle de 2 bits (SH)
- Entrada: saída da ULA

**Status:** ✅ Estrutura definida, ⚠️ Lógica não implementada

#### AMUX
- Multiplexador: Latch A ou MBR → ULA
- Controle de 1 bit

**Status:** ✅ Estrutura definida, ⚠️ Lógica stub

---

### 2.2 Unidade de Controle

#### MIR (Microinstruction Register)
- 32 bits divididos em sinais de controle:
  - [0]: AMUX
  - [1-2]: COND
  - [3-4]: ALU
  - [5-6]: SH
  - [7]: MBR
  - [8]: MAR
  - [9]: RD
  - [10]: WR
  - [11]: ENC
  - [12-15]: C (decoder C)
  - [16-19]: B (decoder B)
  - [20-23]: A (decoder A)
  - [24-31]: ADDR (endereço)

**Status:** ✅ Estrutura definida, ⚠️ Decodificação não implementada

#### MPC (Microprogram Counter)
- 8 bits de endereço
- Aponta para próxima microinstrução

**Status:** ✅ Estrutura definida, ⚠️ Lógica stub

#### MMUX (Micro Address Multiplexer)
- Condições de desvio (COND):
  - 00: Nenhum desvio
  - 01: Desvia se N=1
  - 10: Desvia se Z=1
  - 11: Desvia sempre

**Status:** ✅ Estrutura definida, ⚠️ Lógica não implementada

#### Memória de Controle
- 79 microinstruções (MICROPROGRAM_SIZE)
- 32 bits cada

**Status:** ✅ Estrutura definida, ❌ Sem microprograma

---

### 2.3 Memória

#### Memória Principal
- 4096 posições
- 16 bits por palavra
- Endereçamento de 12 bits

**Status:** ⚠️ Estrutura básica, implementação incompleta

#### Cache
- **Tipo:** Mapeamento Direto
- **Linhas:** 8 (3 bits de índice)
- **Palavras por linha:** 4 (2 bits de offset)
- **Tag:** 7 bits
- **Política de escrita:** Write-through
- **Política de substituição:** Substituição direta

**Endereço (12 bits):**
```
| Tag (7 bits) | Linha (3 bits) | Palavra (2 bits) |
```

**Status:** ✅ Estrutura completa definida, ⚠️ Operações não implementadas

---

## 3. MAPEAMENTO: IMPLEMENTADO vs PENDENTE

### ✅ JÁ IMPLEMENTADO
1. **Estruturas de dados:** Todas definidas nos headers
2. **main.c:** Interface interativa completa
3. **Funções stub:** Esqueleto de todas as funções
4. **Makefile:** Sistema de build funcional
5. **Inicialização básica:** Estrutura de CPU

### ⚠️ PARCIALMENTE IMPLEMENTADO
1. **mic1.c:** Funções básicas, mas sem lógica real
2. **Cache stats:** Funções de estatísticas prontas
3. **Estruturas:** Definidas mas não funcionais

### ❌ NÃO IMPLEMENTADO
1. **Lógica da ULA:** Operações aritméticas/lógicas
2. **Lógica dos Decoders:** Seleção de registradores
3. **Operações de memória:** Leitura/escrita real
4. **Cache:** Lookup, load block, hit/miss
5. **Shifter:** Operações de deslocamento
6. **MAR/MBR:** Comunicação com memória
7. **Unidade de Controle:** Execução de microinstruções
8. **Ciclo de execução:** Fetch-decode-execute
9. **Microprograma:** Definição das 79 microinstruções
10. **Montador:** Tradução assembly → binário
11. **Loader:** Carregar programas na memória
12. **Conversões:** Binário ↔ inteiro para endereços

---

## 4. PROBLEMAS URGENTES

### 🔴 Crítico (Impede compilação)
1. **memoryini.c:** Função main incompleta
   - Falta: fechamento da função, return, liberação de memória

### 🟡 Importante (Funcionalidade)
2. **Falta lógica central:** Componentes definidos mas não funcionam
3. **Sem microprograma:** Sistema não pode executar instruções
4. **Conversões ausentes:** Não há tradução binário/inteiro

### 🟢 Desejável (Melhorias)
5. **Testes:** Nenhum teste implementado
6. **Documentação de código:** Comentários mínimos
7. **Validação de entrada:** Sem checks de erro
8. **Logging:** Debug limitado

---

## 5. DEPENDÊNCIAS ENTRE COMPONENTES

```
Execução de Programa
    ↓
Ciclo MIC-1
    ├── Unidade de Controle
    │   ├── MPC → Memória de Controle → MIR
    │   ├── MIR → Decodifica sinais
    │   └── MMUX → Decide próximo MPC
    │
    └── Via de Dados
        ├── Decoders A/B → Latches A/B
        ├── AMUX → seleciona entrada ULA
        ├── ULA → operação
        ├── Shifter → desloca resultado
        ├── Decoder C → grava em registrador
        └── MAR/MBR ↔ Cache ↔ Memória
```

**Ordem de implementação lógica:**
1. Utilitários (conversões)
2. Componentes básicos (ULA, Shifter)
3. Registradores e Decoders
4. Memória e Cache
5. Unidade de Controle
6. Ciclo de execução
7. Microprograma
8. Loader e Montador

---

## 6. PLANO DE AÇÃO RECOMENDADO

### Fase 1: CORRIGIR PROBLEMAS URGENTES
**Objetivo:** Fazer o projeto compilar

**Tasks:**
1. Corrigir `memoryini.c`
2. Adicionar código de liberação de memória
3. Testar compilação

**Resultado esperado:** `make` compila sem erros

---

### Fase 2: IMPLEMENTAR COMPONENTES BÁSICOS
**Objetivo:** Componentes fundamentais funcionando

**Tasks:**
1. Implementar operações da ULA
   - Soma binária
   - AND lógico
   - Pass A
   - NOT A
   - Atualização de flags N/Z
2. Implementar Shifter
   - Left shift
   - Right shift
3. Implementar funções de conversão
   - Binário → inteiro
   - Inteiro → binário
   - Operações com arrays de bits
4. Implementar Decoders
   - Decoder A/B: seleção de registrador fonte
   - Decoder C: escrita em registrador destino

**Resultado esperado:** Componentes básicos testáveis isoladamente

---

### Fase 3: MEMÓRIA E CACHE
**Objetivo:** Sistema de memória funcionando

**Tasks:**
1. Implementar Memória Principal
   - Alocação de 4096 palavras
   - Read/Write
   - Inicialização
2. Implementar Cache
   - Decomposição de endereço (tag, linha, offset)
   - Cache lookup (hit/miss)
   - Cache load block
   - Write-through
3. Implementar MAR/MBR
   - Comunicação com cache
   - Sinais de controle RD/WR/MBR/MAR

**Resultado esperado:** Leitura/escrita em memória com cache funcional

---

### Fase 4: UNIDADE DE CONTROLE
**Objetivo:** Executar microinstruções

**Tasks:**
1. Implementar MIR
   - Decodificação de microinstrução em sinais
   - Distribuição de sinais para componentes
2. Implementar MPC
   - Incremento
   - Carregamento de novo endereço
3. Implementar MMUX
   - Lógica de desvio condicional
   - Leitura de flags da ULA
4. Criar microprograma básico
   - Definir algumas microinstruções de teste
   - Implementar loader de microprograma

**Resultado esperado:** Sistema executa microinstruções simples

---

### Fase 5: CICLO DE EXECUÇÃO
**Objetivo:** Simulador executando programas

**Tasks:**
1. Implementar ciclo completo MIC-1
   - Fetch microinstrução
   - Decode (MIR)
   - Execute (componentes)
   - Update (MPC)
2. Conectar todos os componentes
   - Datapath completo
   - Control unit completo
   - Sincronização
3. Implementar HALT
   - Condição de parada
   - Finalização limpa

**Resultado esperado:** Simulador executa sequência de microinstruções

---

### Fase 6: MICROPROGRAMA COMPLETO
**Objetivo:** Executar instruções macro

**Tasks:**
1. Definir conjunto de instruções macro
   - LOAD, STORE
   - ADD, SUB
   - AND, OR
   - JUMP, BRANCH
   - HALT
2. Implementar 79 microinstruções
   - Seguir especificação MIC-1
   - Documentar cada microinstrução
3. Implementar interpretação de IR
   - Fetch de instrução macro
   - Desvio para microrotina correta

**Resultado esperado:** Simulador executa instruções MIC-1 completas

---

### Fase 7: MONTADOR E LOADER
**Objetivo:** Carregar programas externos

**Tasks:**
1. Implementar loader binário
   - Ler arquivo binário
   - Carregar em memória
   - Validar formato
2. Implementar montador simples
   - Parser de assembly
   - Geração de binário
   - Tabela de símbolos
3. Criar programas de teste
   - Fibonacci
   - Fatorial
   - Soma de array

**Resultado esperado:** Executar programas assembly completos

---

### Fase 8: TESTES E REFINAMENTOS
**Objetivo:** Sistema robusto e validado

**Tasks:**
1. Criar suite de testes
   - Testes unitários por componente
   - Testes de integração
   - Testes de programas completos
2. Adicionar validações
   - Check de ponteiros nulos
   - Validação de endereços
   - Detecção de overflow
3. Melhorar interface
   - Comandos adicionais
   - Visualização de estado
   - Debug step-by-step
4. Documentação
   - Comentários de código
   - Manual de uso
   - Exemplos

**Resultado esperado:** Sistema completo, testado e documentado

---

## 7. ESTRUTURA DE ARQUIVOS PROPOSTA

```
ArchSim-MIC1/
├── src/
│   ├── main.c                 # [Existe] Interface
│   ├── mic1.c                 # [Existe] Core
│   ├── components/            # [CRIAR] Componentes
│   │   ├── alu.c
│   │   ├── shifter.c
│   │   ├── decoders.c
│   │   ├── mar_mbr.c
│   │   └── registers.c
│   ├── control/               # [CRIAR] Controle
│   │   ├── mir.c
│   │   ├── mpc.c
│   │   ├── mmux.c
│   │   └── microprogram.c
│   ├── memory/                # [CRIAR] Memória
│   │   ├── main_memory.c
│   │   ├── cache.c
│   │   └── memory_ops.c
│   ├── utils/                 # [CRIAR] Utilidades
│   │   ├── conversions.c
│   │   ├── binary_ops.c
│   │   └── file_io.c
│   └── assembler/             # [CRIAR] Montador
│       ├── parser.c
│       ├── assembler.c
│       └── loader.c
├── include/                   # [Existe] Headers
├── programs/                  # [CRIAR] Programas
│   ├── examples/
│   │   ├── fibonacci.asm
│   │   ├── factorial.asm
│   │   └── sum_array.asm
│   └── microcode/
│       └── mic1_microcode.bin
├── tests/                     # [CRIAR] Testes
│   ├── test_alu.c
│   ├── test_cache.c
│   ├── test_memory.c
│   └── test_integration.c
└── docs/                      # [CRIAR] Documentação
    ├── ARCHITECTURE.md
    ├── MICROCODE.md
    └── ASSEMBLY_GUIDE.md
```

---

## 8. ESTIMATIVA DE COMPLEXIDADE

| Fase | Complexidade | Tarefas | Prioridade |
|------|--------------|---------|------------|
| 1. Correções | 🟢 Baixa | 3 | 🔴 Crítica |
| 2. Componentes Básicos | 🟡 Média | 8-10 | 🔴 Alta |
| 3. Memória e Cache | 🟡 Média | 6-8 | 🔴 Alta |
| 4. Unidade Controle | 🟠 Alta | 8-10 | 🟡 Média |
| 5. Ciclo Execução | 🟠 Alta | 5-7 | 🟡 Média |
| 6. Microprograma | 🔴 Muito Alta | 79+ | 🟡 Média |
| 7. Montador/Loader | 🟡 Média | 6-8 | 🟢 Baixa |
| 8. Testes | 🟡 Média | 10+ | 🟢 Baixa |

---

## 9. PRÓXIMOS PASSOS IMEDIATOS

### Para começar AGORA:

1. **Corrigir memoryini.c** (15 min)
2. **Implementar ULA básica** (1-2h)
3. **Implementar conversões binárias** (30min)
4. **Testar componentes isolados** (30min)

### Depois de funcionar básico:

5. **Implementar Shifter** (1h)
6. **Implementar Decoders** (1-2h)
7. **Implementar memória principal** (1h)
8. **Testar leitura/escrita** (30min)

---

## 10. DECISÕES DE DESIGN PENDENTES

**Você precisa decidir:**

1. **Endianess:** Big-endian ou little-endian para arrays de bits?
2. **Microprograma:** Criar manualmente ou gerar automaticamente?
3. **Assembly syntax:** Seguir alguma sintaxe específica?
4. **Cache write policy:** Confirmar write-through ou considerar write-back?
5. **Registradores:** Manter estrutura nomeada ou usar array?
6. **Debug output:** Quanto detalhe de logging?

---

## 11. RECURSOS E REFERÊNCIAS

**Livro base:** "Structured Computer Organization" - Andrew S. Tanenbaum

**Especificação MIC-1:**
- 16 registradores de 16 bits
- 79 microinstruções
- Memória 4096 palavras
- 4 operações ALU
- Conjunto instrução macro baseado em stack

**Arquivos chave no projeto:**
- `README.md` - Especificação detalhada
- `include/*.h` - Estruturas definidas
- `src/main.c` - Interface pronta

---

## CONCLUSÃO

O projeto tem uma **base sólida de estruturas** mas necessita de **implementação da lógica**.

**Estratégia recomendada:**
1. Começar pequeno (corrigir erros)
2. Construir de baixo para cima (componentes → sistema)
3. Testar incrementalmente
4. Documentar decisões

**Primeira milestone:** Fazer ULA e Shifter funcionando com testes simples.

**Segunda milestone:** Sistema de memória (cache + main memory) funcional.

**Terceira milestone:** Executar primeira microinstrução completa.

Você está pronto para começar! 🚀
