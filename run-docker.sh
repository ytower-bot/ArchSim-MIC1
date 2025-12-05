#!/bin/bash

echo "=========================================="
echo "  ArchSim-MIC1 - Docker Runner"
echo "=========================================="
echo ""

if ! command -v docker &> /dev/null; then
    echo "❌ Docker não está instalado!"
    echo "   Instale Docker: https://docs.docker.com/get-docker/"
    exit 1
fi

echo "🔨 Construindo imagem Docker (sem cache)..."
docker build --platform linux/amd64 --no-cache -t archsim-mic1:latest . || exit 1

echo ""
echo "✅ Imagem construída com sucesso!"
echo ""
echo "🚀 Iniciando simulador..."
echo ""
echo "Controles:"
echo "  s - Step (executar 1 ciclo)"
echo "  r - Run/Pause (executar continuamente)"
echo "  x - Reset CPU"
echo "  q - Sair"
echo ""
echo "=========================================="
echo ""

docker run --rm -it --platform linux/amd64 archsim-mic1:latest

