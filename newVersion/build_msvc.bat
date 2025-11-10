@echo off
REM build_msvc.bat - compile aurora_c_demo.c using MSVC (cl)
REM Usage: run from "Developer Command Prompt for VS" or ensure cl is in PATH

where cl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo cl (MSVC) not found in PATH. Open "Developer Command Prompt for VS" and try again.
    pause
    exit /b 1
)

cl /EHsc /O2 /W3 /nologo aurora_c_demo.c /Feaurora_c_demo.exe
if %ERRORLEVEL% neq 0 (
    echo Compilation failed.
    pause
    exit /b %ERRORLEVEL%
)

echo Compilation succeeded. Running demo:
aurora_c_demo.exe
pause
