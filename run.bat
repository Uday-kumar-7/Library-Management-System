@echo off
if not exist "library_system.exe" (
    echo library_system.exe not found. Building first...
    call build.bat
    if %ERRORLEVEL% neq 0 (
        echo Build failed. Exiting.
        pause
        exit /b 1
    )
)

echo Launching Library Management System...
library_system.exe
