Aurora C demo - Build & Run

This folder contains a minimal C demo of the Aurora model and helper build scripts.

Files:
- aurora_c_demo.c         : Minimal, portable C demo (trits, trigate, tensor, synthesize)
- Makefile                : For POSIX or MinGW (run `make`)
- build.bat               : Windows batch (gcc/MinGW)
- build_msvc.bat          : Windows batch for MSVC (cl) - run inside Developer Command Prompt

Quick options to compile on Windows

1) Using MinGW-w64 (recommended for GCC on Windows)
- Install MinGW-w64 (https://winlibs.com/ or via MSYS2)
- Add the MinGW `bin` directory to your PATH (e.g. C:\mingw-w64\mingw32\bin)
- From PowerShell or cmd:
  cd C:\Users\p_m_a\Aurora\Trinity-3\newVersion
  build.bat

Or manually:
  gcc -std=c11 -O2 -Wall -Wextra -o aurora_c_demo aurora_c_demo.c
  .\aurora_c_demo

2) Using WSL (Windows Subsystem for Linux)
- Open WSL shell and navigate to the project path under /mnt/c/...
  cd /mnt/c/Users/p_m_a/Aurora/Trinity-3/newVersion
  make
  ./aurora_c_demo

3) Using MSVC (Visual Studio's cl)
- Open "Developer Command Prompt for VS" (or run vcvarsall.bat to set env)
- Run:
  build_msvc.bat

Notes & Troubleshooting
- If `gcc` is not found, install MinGW-w64 or use WSL as described above.
- If using MSVC, warnings about `snprintf` may appear; modern Visual Studio versions support snprintf, otherwise adjust the code.

Extending the demo
- The C demo is intentionally small. If you want more features (MD5 hash, CLI, file IO, or test harness), ask and I can extend the C source.
