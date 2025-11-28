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

# Comando padrão: executar simulador
CMD ["./mic1_simulator"]
