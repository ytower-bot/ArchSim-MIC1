# Guia Docker - ArchSim-MIC1

Este guia explica como usar o Docker para executar o simulador MIC-1.

---

## Pré-requisitos

**Instalar Docker:**
- Linux: `sudo apt-get install docker.io`
- macOS: [Docker Desktop](https://www.docker.com/products/docker-desktop)
- Windows: [Docker Desktop](https://www.docker.com/products/docker-desktop)

---

## Início Rápido

### 1. Build da Imagem

```bash
cd ArchSim-MIC1
docker build -t archsim-mic1 .
```

**Saída esperada:**
```
[+] Building 15.2s (12/12) FINISHED
 => [internal] load build definition
 => => transferring dockerfile
 => [internal] load .dockerignore
 => exporting to image
 => => exporting layers
 => => writing image sha256:...
 => => naming to docker.io/library/archsim-mic1
```

### 2. Executar Container Interativo

```bash
docker run -it archsim-mic1
```

**Você verá:**
```
===================================
  ArchSim-MIC1 Simulator
===================================

Available tests:
  ./test_assembler         - Test assembler (3 cases)
  ./test_cache_separation  - Test data/instruction caches

Documentation:
  cat README.md            - Full project documentation

root@abc123:/app#
```

### 3. Rodar Testes

Dentro do container:

```bash
# Teste do montador
./test_assembler

# Teste de caches separadas
./test_cache_separation

# Ver documentação
cat README.md
```

---

## Comandos Úteis

### Executar Teste Único (Sem Shell Interativo)

```bash
# Testar apenas o montador
docker run archsim-mic1 ./test_assembler

# Testar apenas as caches
docker run archsim-mic1 ./test_cache_separation
```

### Montar Volume Local (Desenvolvimento)

Para editar arquivos localmente e testar no container:

```bash
docker run -it -v $(pwd):/app archsim-mic1
```

Agora mudanças locais aparecem no container em tempo real.

### Ver Logs de Build

```bash
docker build -t archsim-mic1 . --no-cache --progress=plain
```

### Listar Imagens

```bash
docker images | grep archsim
```

### Remover Imagem

```bash
docker rmi archsim-mic1
```

### Limpar Containers Parados

```bash
docker container prune
```

---

## Estrutura do Container

```
/app/                     # Working directory
├── include/              # Headers (.h)
├── src/                  # Source files (.c)
├── data/                 # Microcode files
├── test_assembler        # Compiled test executable
├── test_cache_separation # Compiled test executable
└── README.md             # Documentation
```

---

## Testando a Imagem

### Verificar Compilação

```bash
docker run archsim-mic1 ls -lh
```

Você deve ver:
```
-rwxr-xr-x test_assembler
-rwxr-xr-x test_cache_separation
drwxr-xr-x include/
drwxr-xr-x src/
drwxr-xr-x data/
```

### Verificar GCC

```bash
docker run archsim-mic1 gcc --version
```

Saída esperada:
```
gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
```

---

## Troubleshooting

### Erro: "Cannot connect to Docker daemon"

**Problema:** Docker não está rodando.

**Solução:**
```bash
# Linux
sudo systemctl start docker

# macOS/Windows
# Inicie o Docker Desktop
```

### Erro: "Permission denied"

**Problema:** Usuário não tem permissão para usar Docker.

**Solução (Linux):**
```bash
sudo usermod -aG docker $USER
# Logout e login novamente
```

### Build Lento

**Problema:** Primeira build baixa imagem base Ubuntu.

**Solução:** Aguarde. Builds subsequentes são rápidas (cache).

### Container Sai Imediatamente

**Problema:** Falta flag `-it` para modo interativo.

**Solução:**
```bash
docker run -it archsim-mic1  # Correto
docker run archsim-mic1      # Errado (sai imediatamente)
```

---

## Distribuição

### Exportar Imagem

```bash
docker save archsim-mic1 > archsim-mic1.tar
```

### Importar Imagem

```bash
docker load < archsim-mic1.tar
```

### Publicar no Docker Hub (Opcional)

```bash
docker tag archsim-mic1 username/archsim-mic1:v1.0
docker push username/archsim-mic1:v1.0
```

---

## Referências Adicionais

- [README.md](../README.md) - Documentação completa
- [ASSEMBLER_GUIDE.md](ASSEMBLER_GUIDE.md) - Guia do montador
- [ArchSim-MIC1.md](ArchSim-MIC1.md) - Documentação técnica detalhada
- [ROADMAP.md](ROADMAP.md) - Status do projeto

---

**Última Atualização:** 28/11/2025
