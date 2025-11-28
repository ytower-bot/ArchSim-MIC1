FROM ubuntu:22.04

# Instalar dependências básicas
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*

# Diretório de trabalho
WORKDIR /app

# Copiar projeto
COPY . .

# Compilar projeto
RUN make clean && make

# Definir variável de ambiente para modo CI
ENV CI=true

# Comando padrão: executar simulador em modo não-interativo
CMD ["./mic1_simulator"]
