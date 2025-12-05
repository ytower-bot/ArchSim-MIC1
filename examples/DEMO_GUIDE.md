# Guia de Apresentação (5 min) — demo_presentation.asm

Este roteiro ajuda a apresentar o programa `examples/demo_presentation.asm` em ~5 minutos, explicando o que o código faz, como executar e como demonstrar os resultados na memória.

---

## Objetivo do Programa

- Inicializa uma lista com 5 números na memória (endereços 10..14).
- Soma todos os elementos (resultado em `30`).
- Calcula a média inteira pelo método de subtrações sucessivas (resultado em `31`).
- Faz uma comparação simples: `resultado = (soma >= 100) ? 1 : 0` (em `32`).
- Copia a soma para `33` para facilitar visualização.

Instruções usadas: `LOCO`, `LODD`, `STOD`, `ADDD`, `SUBD`, `JUMP`, `JZER`, `HALT`.

---

## Roteiro (fala sugerida ~5 min)

1) Contexto rápido (30s)
- “Este é um programa em assembly para o simulador MIC‑1. Ele demonstra quase todas as instruções do nosso montador: operações de memória, imediatas, controle de fluxo e finalização.”
- “Como a ISA é simples (8 instruções), mostramos como construir lógica com loops e labels.”

2) Layout de memória (40s)
- “Dados da lista: endereços `10..14`.”
- “Tamanho (N): `20`.”
- “Acumuladores/resultados: `30` (soma), `31` (média inteira), `32` (flag/resultado da comparação), `40` (índice i).”

3) Inicialização (40s)
- “Usamos `LOCO` para carregar constantes e `STOD` para salvar na memória. Assim colocamos 12, 8, 25, 30, 5 em `10..14` e `N=5` em `20`.”
- “Zeramos `soma` em `30` e o índice `i` em `40`.”

4) Loop de soma (1m30s)
- “O label `LOOP` controla a iteração: carregamos `i` (`LODD 40`), comparamos com `N` (`SUBD 20`), e usamos `JZER END_SUM` para parar quando `i == N`.”
- “Sem endereçamento indireto, decidimos qual elemento somar usando uma pequena ‘tabela’ por labels (`IDX0..IDX4`). Para cada `i`, somamos `mem[10+i]` em `soma` com `ADDD` e salvamos com `STOD`.”
- “Incrementamos `i` com `ADDD 1` e `STOD 40`, e `JUMP LOOP` volta ao início.”
- “Demonstra o uso de `JUMP` e `JZER` para controle de fluxo, e de `LODD/ADDD/STOD` para trabalhar com dados.”

5) Média inteira (50s)
- “Sem instrução de divisão, implementamos média por subtrações sucessivas: copiamos `soma` para um temporário (`32`), e vamos subtraindo `N` de `temp` enquanto possível, incrementando `media` (`31`).”
- “Quando `temp - N == 0`, detectamos com `JZER`, incrementamos a média mais uma vez e encerramos.”

6) Comparação e saída (30s)
- “Comparamos `soma` com 100 via `SUBD 100` e `JZER` para o caso de igualdade; como não há flag de negativo, para apresentação guardamos um 0 ou 1 conforme o caso em `32`.”
- “Copiamos `soma` também para `33` e finalizamos com `HALT`.”

7) Encerramento (20s)
- “Este exemplo mostra como construir lógica expressiva mesmo com uma ISA mínima, e como labels e fluxo condicional permitem estruturar loops e decisões.”

---

## Como Executar

Escolha uma das formas conforme seu ambiente.

- Via Makefile (se disponível):
  - `make` para compilar o simulador.
  - Executar o binário seguindo as instruções do README/TUI.

- Via TUI (Go):
  - `cd tui`
  - `go run .` e seguir o guia `tui/README.md` para carregar o programa (se a TUI oferece essa opção).

- Via CLI C (se o simulador aceita arquivo):
  - Consulte `README.md` e `docs/DOCKER.md` para montar e executar.
  - Em muitos fluxos: montar `examples/demo_presentation.asm` e rodar no simulador.

Observação: a infraestrutura do projeto oferece várias formas (Docker/TUI/CLI). Use a que você já usa nas demos.

---

## O que Mostrar na Demonstração

Depois de rodar o programa, aponte os seguintes endereços na memória:

- `10..14`: os 5 elementos iniciais (12, 8, 25, 30, 5).
- `20`: `N = 5`.
- `30`: soma total (12+8+25+30+5 = 80).
- `31`: média inteira (80/5 = 16).
- `32`: comparação soma >= 100 (para estes dados, 0).
- `33`: cópia da soma (80), facilita visualizar sem alterar `30`.
- `40`: índice final (`i = N = 5`).

Explique, enquanto aponta os resultados:
- “O loop percorreu os 5 valores e acumulou 80 em `30`.”
- “A média inteira calculada por subtrações sucessivas ficou 16 em `31`.”
- “A comparação com 100 armazenou `0` em `32`.”

---

## Dicas de Apresentação

- Foque nas instruções-chave quando passar pelo código: `LOCO/LODD/STOD` (dados), `ADDD/SUBD` (aritmética), `JUMP/JZER` (controle), `HALT` (fim).
- Reforce a limitação de endereçamento e como contornamos com labels (`IDX0..IDX4`).
- Reforce a ausência de divisão e flags, e como construímos a média com subtrações e detecção de zero.
- Mantenha o ritmo: explique rapidamente cada fase e mostre os valores finais.

---

## Referências

- `docs/ASSEMBLER_GUIDE.md` — instruções suportadas e exemplos.
- `README.md`, `DOCKER.md` — execução com Docker/CLI.
- `tui/README.md`, `tui/TUI_GUIDE.md` — uso da TUI (se optar por apresentar com interface).