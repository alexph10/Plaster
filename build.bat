@echo off
REM Quick build script for Plaster Engine
REM Usage: build.bat [Debug|Release] [run]

setlocal

set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Debug

echo ====================================
echo Plaster Engine Quick Build
echo Configuration: %CONFIG%
echo ====================================
echo.

cd build
cmake --build . --config %CONFIG%

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build failed!
    exit /b 1
)

echo.
echo [SUCCESS] Build complete!

if "%2"=="run" (
    echo.
    echo Running Plaster Engine...
    echo ====================================
    %CONFIG%\Plaster.exe
    echo ====================================
    echo Program exited with code: %ERRORLEVEL%
)

endlocal
