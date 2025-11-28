# Roadmap do Projeto

## Visão Geral

Desenvolvimento organizado em fases incrementais, desde componentes básicos até sistema completo.

**Status Atual:** Fases 0-6 completas, sistema funcional com montador e testes.

---

## Fases Completas

### Fase 0: Correção de Bug Crítico
- Removido malloc desnecessário na inicialização do datapath
- Eliminado memory leak
- **Commit:** ffe0d4e

### Fase 1: Funções de Conversão
- Implementadas funções utilitárias de conversão bit/int
- Funções: bits_to_int, int_to_bits, address_to_int, int_to_address, copy_data
- **Testes:** 9/9 passando
- **Commit:** 4611bef

### Fase 2: Componentes do Datapath
- Implementado Shifter com deslocamento esquerda/direita
- Implementados Decoders A/B/C para seleção de registradores
- Implementados MAR/MBR para acesso à memória

### Fase 3: Sistema de Memória
- Memória principal: 4096 palavras × 16 bits
- Cache: 8 linhas × 4 palavras, mapeamento direto, write-through
- Decomposição de endereço: Tag(7) | Line(3) | Word(2)
- Rastreamento de estatísticas (hits/misses)

### Fase 4: Unidade de Controle
- MIR: registrador de microinstrução de 32 bits com distribuição de sinais
- MPC: contador de microprograma de 8 bits
- MMUX: multiplexador de microendereço com desvios condicionais
- Carregador de memória de controle para microprogramas
- AMUX: multiplexador de entrada da ALU
- **Testes:** 7/7 AMUX, 9/9 memória de controle
- **Commits:** b5e7fbd, bf6dd10

### Fase 5: Ciclo de Execução
- Ciclo completo fetch-decode-execute-update
- Fetch: MPC → Memória de Controle → MIR
- Execute: Pipeline completo de operações do datapath
- Update: MMUX determina próximo MPC
- **Testes:** 9/9 passando
- **Commit:** d705ead

### Fase 6: Linguagem Assembly e Testes
- Montador completo com 8 instruções (LODD, STOD, ADDD, SUBD, LOCO, JUMP, JZER, HALT)
- Suporte a labels para endereçamento simbólico
- Suporte a comentários com ponto-e-vírgula
- Saída binária: formato de 16 bits (8 bits opcode + 8 bits operando)
- Caches separadas para instruções e dados
- Suite de testes abrangente
- Programas exemplo (soma, loop, máximo)
- **Testes:** 3/3 montador, 3/3 separação de cache, 1/1 debug

---

## Fases Pendentes

### Fase 7: ISA Estendida
Expandir conjunto de instruções para especificação completa MIC-1:
- Operações de pilha (PUSH, POP, CALL, RETN)
- Aritmética adicional (multiplicação, divisão via sub-rotinas)
- Modos de endereçamento local/indireto
- Desvios condicionais baseados em flags

### Fase 8: Recursos Avançados
Ferramentas de desenvolvimento e otimização:
- Debugger interativo com breakpoints
- Modo de execução passo-a-passo
- Ferramentas de inspeção de memória/registradores
- Profiling de performance
- Log de trace de execução

### Fase 9: Programas Adicionais
Programas exemplo demonstrando capacidades:
- Cálculo de sequência Fibonacci
- Computação de fatorial
- Algoritmos de ordenação de arrays
- Manipulação de strings
- Funções matemáticas

### Fase 10: Documentação e Refinamento
Documentação final e refinamentos:
- Documentação completa da API
- Guia detalhado da arquitetura
- Tutorial para escrita de programas
- Benchmarks de performance
- Limpeza e otimização de código

---

## Resumo de Progresso

**Milestone atual:** Simulador core completo e funcional  
**Próximo milestone:** Conjunto de instruções estendido e ferramentas avançadas

---

## Diretrizes de Desenvolvimento

### Formato de Mensagem de Commit
```
<tipo>: <descrição curta>

- Detalhe 1
- Detalhe 2

Arquivos: lista de arquivos modificados
Testes: resultados dos testes
```

Tipos: feat, fix, refactor, docs, test, chore

### Formato de Log de Fase
Ver `docs/archsim_fase_log/` para exemplos de documentação de fase.

### Requisitos de Teste
- Todas as novas funcionalidades devem incluir testes
- Manter taxa de 100% de aprovação nos testes
- Sem warnings com -Wall -Wextra -std=c99

---

## Dependências Entre Fases

```
Fase 0 (Bug fix)
    ↓
Fase 1 (Conversões) ──→ Fase 2 (Datapath)
    ↓                          ↓
Fase 3 (Memória) ←────────────┘
    ↓
Fase 4 (Unidade de Controle)
    ↓
Fase 5 (Ciclo de Execução)
    ↓
Fase 6 (Montador) ──→ Fase 7 (ISA Estendida)
    ↓                          ↓
Fase 8 (Ferramentas) ←─────────┤
    ↓                          ↓
Fase 9 (Programas) ────────────┘
    ↓
Fase 10 (Documentação)
```

Cada fase constrói sobre o trabalho anterior, mantendo compatibilidade retroativa.
