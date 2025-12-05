#!/bin/bash
# Executável para Linux/Mac - 1 clique para executar o simulador

echo "=========================================="
echo "  ArchSim-MIC1 - Executável"
echo "=========================================="
echo ""

if ! command -v docker &> /dev/null; then
    echo "❌ Docker não está instalado!"
    echo ""
    echo "Instale Docker:"
    echo "  Linux: https://docs.docker.com/engine/install/"
    echo "  Mac:   https://docs.docker.com/desktop/install/mac-install/"
    echo ""
    read -p "Pressione ENTER para sair..."
    exit 1
fi

echo "🔨 Construindo imagem Docker..."
docker build --platform linux/amd64 -t archsim-mic1:latest . 2>&1 | grep -E "(Step|ERROR|Successfully)" || true

if [ $? -ne 0 ]; then
    echo "❌ Erro ao construir imagem!"
    read -p "Pressione ENTER para sair..."
    exit 1
fi

echo ""
echo "✅ Imagem construída!"
echo ""
echo "🚀 Iniciando simulador..."
echo ""
echo "Controles:"
echo "  s - Step (executar 1 ciclo)"
echo "  r - Run/Pause"
echo "  x - Reset"
echo "  q - Sair"
echo ""
echo "=========================================="
echo ""

docker run --rm -it --platform linux/amd64 archsim-mic1:latest

