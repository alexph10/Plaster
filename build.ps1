#!/usr/bin/env pwsh
# Plaster Engine Build Script
# This script automates the CMake configuration and build process

param(
    [string]$Config = "Debug",
    [switch]$Clean,
    [switch]$Run,
    [switch]$Rebuild,
    [switch]$Help
)

# Color output functions
function Write-Success { param($Message) Write-Host $Message -ForegroundColor Green }
function Write-Info { param($Message) Write-Host $Message -ForegroundColor Cyan }
function Write-Error { param($Message) Write-Host $Message -ForegroundColor Red }
function Write-Warning { param($Message) Write-Host $Message -ForegroundColor Yellow }

# Show help
if ($Help) {
    Write-Info "Plaster Engine Build Script"
    Write-Host ""
    Write-Host "Usage: .\build.ps1 [-Config <Debug|Release>] [-Clean] [-Run] [-Rebuild] [-Help]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Config     Build configuration (Debug or Release). Default: Debug"
    Write-Host "  -Clean      Clean the build directory before building"
    Write-Host "  -Run        Run the executable after successful build"
    Write-Host "  -Rebuild    Clean and rebuild everything"
    Write-Host "  -Help       Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build.ps1                    # Build in Debug mode"
    Write-Host "  .\build.ps1 -Config Release    # Build in Release mode"
    Write-Host "  .\build.ps1 -Clean -Run        # Clean, build, and run"
    Write-Host "  .\build.ps1 -Rebuild           # Full rebuild"
    exit 0
}

# Get script directory
$ScriptDir = $PSScriptRoot
$BuildDir = Join-Path $ScriptDir "build"
$ExePath = Join-Path $BuildDir "$Config\Plaster.exe"

Write-Info "=== Plaster Engine Build Script ==="
Write-Info "Configuration: $Config"
Write-Info "Build Directory: $BuildDir"
Write-Host ""

# Clean build directory if requested
if ($Clean -or $Rebuild) {
    Write-Warning "Cleaning build directory..."
    if (Test-Path $BuildDir) {
        Remove-Item -Path $BuildDir -Recurse -Force -ErrorAction SilentlyContinue
        Write-Success "Build directory cleaned."
    }
}

# Create build directory if it doesn't exist
if (-not (Test-Path $BuildDir)) {
    Write-Info "Creating build directory..."
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Navigate to build directory
Push-Location $BuildDir

try {
    # Configure with CMake if needed
    if (-not (Test-Path "CMakeCache.txt") -or $Rebuild) {
        Write-Info "Configuring CMake..."
        
        $vcpkgToolchain = Join-Path $ScriptDir "vcpkg_installed\x64-windows\share\vcpkg\scripts\buildsystems\vcpkg.cmake"
        
        cmake .. `
            -DCMAKE_TOOLCHAIN_FILE="$vcpkgToolchain" `
            -DCMAKE_BUILD_TYPE=$Config
        
        if ($LASTEXITCODE -ne 0) {
            Write-Error "CMake configuration failed!"
            exit 1
        }
        Write-Success "CMake configuration successful."
        Write-Host ""
    }

    # Build the project
    Write-Info "Building project in $Config mode..."
    cmake --build . --config $Config
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed!"
        exit 1
    }
    
    Write-Success "Build successful!"
    Write-Host ""
    
    # Check if executable exists
    if (Test-Path $ExePath) {
        Write-Success "Executable created: $ExePath"
    } else {
        Write-Warning "Warning: Executable not found at expected path: $ExePath"
    }

    # Run the executable if requested
    if ($Run -and (Test-Path $ExePath)) {
        Write-Host ""
        Write-Info "Running Plaster Engine..."
        Write-Host "=========================================="
        & $ExePath
        Write-Host "=========================================="
        Write-Info "Program exited with code: $LASTEXITCODE"
    }

} finally {
    # Return to original directory
    Pop-Location
}

Write-Host ""
Write-Success "=== Build Script Complete ==="
