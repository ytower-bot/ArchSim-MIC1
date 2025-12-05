@echo off
echo ==========================================
echo   ArchSim-MIC1 - Docker Runner (Windows)
echo ==========================================
echo.

where docker >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERRO] Docker nao esta instalado!
    echo        Instale Docker Desktop: https://docs.docker.com/get-docker/
    pause
    exit /b 1
)

echo [INFO] Construindo imagem Docker...
docker build --platform linux/amd64 -t archsim-mic1:latest .
if %ERRORLEVEL% NEQ 0 (
    pause
    exit /b 1
)

echo.
echo [OK] Imagem construida com sucesso!
echo.
echo [INFO] Iniciando simulador...
echo.
echo Controles:
echo   s - Step (executar 1 ciclo)
echo   r - Run/Pause (executar continuamente)
echo   x - Reset CPU
echo   q - Sair
echo.
echo ==========================================
echo.

docker run --rm -it archsim-mic1:latest

pause

