# Guia do Montador (Assembler) - ArchSim-MIC1

Este guia explica como usar o montador assembly do simulador MIC-1.

---

## Visão Geral

O montador converte programas em linguagem assembly MIC-1 para formato binário executável (16 bits por instrução).

**Características:**
- 8 instruções básicas
- Labels simbólicos
- Comentários (`;`)
- Operandos decimais e hexadecimais
- Two-pass assembler (coleta labels, gera código)

---

## Instruções Suportadas

### **Instruções de Memória (Direct Addressing)**

| Mnemônico | Sintaxe | Operação | Opcode |
|-----------|---------|----------|--------|
| **LODD** | `LODD addr` | `AC ← Memory[addr]` | 0x00 |
| **STOD** | `STOD addr` | `Memory[addr] ← AC` | 0x01 |
| **ADDD** | `ADDD addr` | `AC ← AC + Memory[addr]` | 0x02 |
| **SUBD** | `SUBD addr` | `AC ← AC - Memory[addr]` | 0x03 |

### **Instruções Imediatas**

| Mnemônico | Sintaxe | Operação | Opcode |
|-----------|---------|----------|--------|
| **LOCO** | `LOCO const` | `AC ← constant` | 0x04 |

### **Instruções de Controle de Fluxo**

| Mnemônico | Sintaxe | Operação | Opcode |
|-----------|---------|----------|--------|
| **JUMP** | `JUMP addr` | `PC ← addr` (incondicional) | 0x05 |
| **JZER** | `JZER addr` | `if (AC == 0) PC ← addr` | 0x06 |

### **Instruções de Sistema**

| Mnemônico | Sintaxe | Operação | Opcode |
|-----------|---------|----------|--------|
| **HALT** | `HALT` | Para execução | 0x07 |

---

## Sintaxe Assembly

### **Formato Básico**

```asm
[LABEL:]  MNEMONIC  [OPERAND]  [; COMMENT]
```

- **LABEL (opcional):** Identificador seguido de `:`
- **MNEMONIC (obrigatório):** Nome da instrução (LODD, STOD, etc.)
- **OPERAND (opcional):** Endereço, constante, ou label
- **COMMENT (opcional):** Texto após `;` (ignorado)

### **Regras**

1. **Case insensitive:** `LODD`, `lodd`, `Lodd` são equivalentes
2. **Labels devem terminar com `:` **
3. **Comentários começam com `;`**
4. **Linhas vazias são ignoradas**
5. **Operandos podem ser:**
   - Decimal: `100`, `42`, `255`
   - Hexadecimal: `0x64`, `0x2A`, `0xFF`
   - Labels: `START`, `LOOP`, `END`

---

## Exemplos

### **Exemplo 1: Programa Simples**

```asm
; Programa: Carrega constante e salva na memória
LOCO 42       ; AC ← 42
STOD 100      ; Memory[100] ← AC
HALT          ; Para
```

**Binário gerado:**
```
0x042A  ; LOCO 42
0x0164  ; STOD 100
0x0700  ; HALT
```

---

### **Exemplo 2: Soma de Dois Números**

```asm
; Soma: result = a + b
; Endereços: a=100, b=101, result=102

        LOCO 10      ; AC ← 10
        STOD 100     ; Memory[100] ← 10 (salva 'a')
        
        LOCO 20      ; AC ← 20
        STOD 101     ; Memory[101] ← 20 (salva 'b')
        
        LODD 100     ; AC ← Memory[100] (carrega 'a')
        ADDD 101     ; AC ← AC + Memory[101] (soma com 'b')
        STOD 102     ; Memory[102] ← AC (salva resultado)
        
        HALT         ; Para
```

---

### **Exemplo 3: Loop com Label**

```asm
; Programa: Contador decrescente
; Conta de 5 até 0

START:  LOCO 5       ; AC ← 5
LOOP:   SUBD 1       ; AC ← AC - 1
        JZER END     ; Se AC == 0, vai para END
        JUMP LOOP    ; Senão, volta para LOOP
END:    HALT         ; Para
```

**Funcionamento:**
1. `START`: AC recebe 5
2. `LOOP`: Subtrai 1 (AC = 4, 3, 2, 1, 0)
3. `JZER END`: Quando AC == 0, pula para END
4. `JUMP LOOP`: Senão, volta para LOOP
5. `END`: Termina

---

### **Exemplo 4: Uso de Hexadecimal**

```asm
; Operandos em hexadecimal
LOCO 0xFF      ; AC ← 255
STOD 0x100     ; Memory[256] ← AC
LODD 0x64      ; AC ← Memory[100]
HALT
```

---

## Uso da API

### **Montar de String (Em Memória)**

```c
#include "assembler.h"

const char* program = 
    "LOCO 10\n"
    "ADDD 100\n"
    "STOD 101\n"
    "HALT\n";

uint16_t output[MAX_INSTRUCTIONS];
int size;

if (assemble_string(program, output, &size) == 0) {
    printf("Assembly successful: %d instructions\n", size);
    for (int i = 0; i < size; i++) {
        printf("[%04X] %04X\n", i * 2, output[i]);
    }
}
```

### **Montar de Arquivo**

```c
#include "assembler.h"

// Cria arquivo program.asm
FILE* fp = fopen("program.asm", "w");
fprintf(fp, "LOCO 5\n");
fprintf(fp, "ADDD 10\n");
fprintf(fp, "STOD 15\n");
fprintf(fp, "HALT\n");
fclose(fp);

// Monta para binário
if (assemble_file("program.asm", "program.bin") == 0) {
    printf("Assembly successful!\n");
}
```

---

## Formato Binário

### **Estrutura da Instrução (16 bits)**

```
|  15  14  13  12  11  10   9   8 |  7   6   5   4   3   2   1   0 |
|        OPCODE (8 bits)          |      OPERAND (8 bits)           |
```

### **Exemplos de Codificação**

| Assembly | Opcode | Operand | Binário | Hex |
|----------|--------|---------|---------|-----|
| `LOCO 10` | 0x04 | 0x0A | `0000010000001010` | `0x040A` |
| `LODD 100` | 0x00 | 0x64 | `0000000001100100` | `0x0064` |
| `ADDD 0xFF` | 0x02 | 0xFF | `0000001011111111` | `0x02FF` |
| `HALT` | 0x07 | 0x00 | `0000011100000000` | `0x0700` |

---

## Limitações

1. **Operandos 8-bit:** Valores 0-255 apenas
2. **Endereços de memória:** 0-255 (8 bits)
3. **Instruções limitadas:** Apenas 8 instruções básicas
4. **Sem macros ou diretivas:** Apenas instruções puras
5. **Sem suporte a dados:** `.WORD`, `.DATA` não implementados

---

## Erros Comuns

### **Erro: "Invalid opcode"**

```asm
ADD 100  ; ❌ Errado: não existe ADD
ADDD 100 ; ✅ Correto: ADDD (Add Direct)
```

### **Erro: "Missing operand"**

```asm
LODD     ; ❌ Errado: falta endereço
LODD 100 ; ✅ Correto
```

### **Erro: "Undefined label"**

```asm
JUMP MISSING  ; ❌ Label MISSING não foi definida

START:  LODD 10
        JUMP START  ; ✅ Correto: label definida
```

### **Erro: "Duplicate label"**

```asm
LOOP:   LODD 10
LOOP:   ADDD 11  ; ❌ LOOP já existe
```

### **Erro: "Out of range"**

```asm
LOCO 256  ; ❌ 256 > 255 (operando 8-bit)
LOCO 255  ; ✅ Máximo permitido
```

---

## Tabela de Referência Rápida

| Operação | Assembly | Exemplo |
|----------|----------|---------|
| **Carregar constante** | `LOCO n` | `LOCO 42` |
| **Carregar da memória** | `LODD addr` | `LODD 100` |
| **Salvar na memória** | `STOD addr` | `STOD 100` |
| **Somar** | `ADDD addr` | `ADDD 101` |
| **Subtrair** | `SUBD addr` | `SUBD 102` |
| **Pular** | `JUMP label` | `JUMP START` |
| **Pular se zero** | `JZER label` | `JZER END` |
| **Parar** | `HALT` | `HALT` |

---

## Referências Adicionais

- [README.md](../README.md) - Visão geral do projeto
- [DOCKER.md](DOCKER.md) - Instruções Docker
- [ArchSim-MIC1.md](ArchSim-MIC1.md) - Documentação técnica completa
- `tests/test_assembler.c` - Exemplos de uso do montador

---

**Última Atualização:** 28/11/2025
