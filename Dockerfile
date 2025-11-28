FROM ubuntu:22.04

# Evitar prompts interativos durante instalação
ENV DEBIAN_FRONTEND=noninteractive

# Instalar dependências
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    vim \
    && rm -rf /var/lib/apt/lists/*

# Criar diretório de trabalho
WORKDIR /app

# Copiar arquivos do projeto
COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/
COPY data/ ./data/
COPY examples/ ./examples/
COPY Makefile ./
COPY README.md ./

# Compilar projeto e testes
RUN make clean && make all && make test

# Criar script de boas-vindas
RUN echo '#!/bin/bash\n\
echo ""\n\
echo "==================================="\n\
echo "  ArchSim-MIC1 Simulator"\n\
echo "==================================="\n\
echo ""\n\
echo "Available commands:"\n\
echo "  make all             - Build simulator"\n\
echo "  make test            - Run all tests"\n\
echo "  ./mic1_simulator     - Run simulator"\n\
echo ""\n\
echo "Documentation:"\n\
echo "  cat README.md        - Project overview"\n\
echo "  cat docs/DOCKER.md   - Docker guide"\n\
echo ""\n\
/bin/bash\n\
' > /entrypoint.sh && chmod +x /entrypoint.sh

# Definir entrypoint
ENTRYPOINT ["/entrypoint.sh"]
