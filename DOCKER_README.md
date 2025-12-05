# Docker - ArchSim-MIC1

## Execução Rápida (1 Clique)

### Linux/macOS:
```bash
./run-docker.sh
```

### Windows:
```batch
run-docker.bat
```

Isso irá:
1. Construir a imagem Docker automaticamente
2. Compilar todo o projeto (C + Go)
3. Iniciar a TUI com o programa `test_stack.asm` já carregado

## Execução Manual

### Construir imagem:
```bash
docker build -t archsim-mic1:latest .
```

### Executar:
```bash
docker run --rm -it archsim-mic1:latest
```

### Executar com arquivo específico:
```bash
docker run --rm -it -v $(pwd)/examples:/app/examples:ro archsim-mic1:latest ../examples/test_alu.asm
```

## Usando Docker Compose

```bash
docker-compose up
```

## Controles da TUI

- `s` - Step (executar 1 ciclo)
- `r` - Run/Pause (executar continuamente)
- `x` - Reset CPU
- `l` - Carregar arquivo .asm
- `q` - Sair

## Requisitos

- Docker instalado
- ~500MB de espaço em disco

## Estrutura no Container

```
/app/
├── include/     # Headers C
├── src/         # Código fonte C
├── data/        # Microcódigo
├── examples/    # Programas exemplo
└── tui/         # Interface TUI (Go)
```

## Troubleshooting

### Erro: "docker: command not found"
Instale Docker: https://docs.docker.com/get-docker/

### Erro na compilação
Verifique se todas as dependências estão no Dockerfile

### TUI não aparece
Certifique-se de usar `-it` flags: `docker run --rm -it archsim-mic1:latest`

