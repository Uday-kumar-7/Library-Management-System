@echo off
setlocal enabledelayedexpansion

echo =======================================================
echo   Building Library Management System (Windows)
echo =======================================================

REM Check if gcc is directly available in PATH
where gcc >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set CC=gcc
    goto COMPILE
)

REM Check if clang is directly available in PATH
where clang >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set CC=clang
    goto COMPILE
)

REM Check WinGet installed LLVM MinGW location
set WINGET_LLVM_DIR=%LOCALAPPDATA%\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20261006-ucrt-x86_64\bin
if exist "%WINGET_LLVM_DIR%\gcc.exe" (
    set "PATH=%WINGET_LLVM_DIR%;%PATH%"
    set CC=gcc
    goto COMPILE
)

REM Check common MinGW default installations
if exist "C:\MinGW\bin\gcc.exe" (
    set "PATH=C:\MinGW\bin;%PATH%"
    set CC=gcc
    goto COMPILE
)

if exist "C:\msys64\ucrt64\bin\gcc.exe" (
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
    set CC=gcc
    goto COMPILE
)

if exist "C:\msys64\mingw64\bin\gcc.exe" (
    set "PATH=C:\msys64\mingw64\bin;%PATH%"
    set CC=gcc
    goto COMPILE
)

echo [ERROR] No C compiler (gcc or clang) found on your system!
echo Please read README.md for easy compiler installation instructions.
exit /b 1

:COMPILE
echo [*] Compiler detected: %CC%
echo [*] Compiling source files...

%CC% -Wall -Wextra -std=c99 -Iinclude src\utils.c src\book.c src\bst.c src\stack.c src\queue.c src\issue.c src\file_io.c src\main.c -o library_system.exe

if %ERRORLEVEL% equ 0 (
    echo [SUCCESS] Compilation succeeded!
    echo Executable generated: library_system.exe
    echo Run the program using: library_system.exe or run.bat
) else (
    echo [ERROR] Compilation failed! Please inspect the errors above.
    exit /b 1
)

endlocal
