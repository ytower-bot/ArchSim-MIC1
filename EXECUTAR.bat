@echo off
REM Executável para Windows - 1 clique para executar o simulador

echo ==========================================
echo   ArchSim-MIC1 - Executavel
echo ==========================================
echo.

where docker >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERRO] Docker nao esta instalado!
    echo.
    echo Instale Docker Desktop:
    echo   https://docs.docker.com/desktop/install/windows-install/
    echo.
    pause
    exit /b 1
)

echo [INFO] Construindo imagem Docker...
docker build --platform linux/amd64 -t archsim-mic1:latest .

if %ERRORLEVEL% NEQ 0 (
    echo [ERRO] Falha ao construir imagem!
    pause
    exit /b 1
)

echo.
echo [OK] Imagem construida!
echo.
echo [INFO] Iniciando simulador...
echo.
echo Controles:
echo   s - Step (executar 1 ciclo)
echo   r - Run/Pause
echo   x - Reset
echo   q - Sair
echo.
echo ==========================================
echo.

docker run --rm -it --platform linux/amd64 archsim-mic1:latest

pause

