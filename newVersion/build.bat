@echo off
REM build.bat - compile aurora_c_demo.c using gcc (MinGW)
REM Usage: open PowerShell or cmd, run build.bat

where gcc >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo gcc not found in PATH. Install MinGW-w64 or use WSL or Visual Studio.
    echo See README_C_DEMO.md for instructions.
    pause
    exit /b 1
)

gcc -std=c11 -O2 -Wall -Wextra -o aurora_c_demo aurora_c_demo.c
if %ERRORLEVEL% neq 0 (
    echo Compilation failed.
    pause
    exit /b %ERRORLEVEL%
)

echo Compilation succeeded. Running demo:
.\aurora_c_demo
pause
