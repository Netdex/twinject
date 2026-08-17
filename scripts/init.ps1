
# init.ps1 – One-click dev environment bootstrap for twinject
#
# Usage (PowerShell as Administrator):
#   Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
#   .\scripts\init.ps1
#
# What it does:
#   1. Checks prerequisites (Git, VS 2019/2022 Build Tools)
#   2. Provides download links for DirectX SDK if missing
#   3. Clones submodules automatically
#   4. Offers to open solution in VS

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path $PSScriptRoot -Parent

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  twinject dev environment bootstrap" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# ---------- Step 1: check prerequisites ----------

Write-Host "[1/4] Checking prerequisites..." -ForegroundColor Yellow

# Check git
$gitOk = $false
try {
    git --version 2>&1 | Out-Null
    $gitOk = ($LASTEXITCODE -eq 0)
} catch { }
if (-not $gitOk) {
    Write-Host "  ERROR: git not found. Please install Git for Windows from https://git-scm.com/download/win" -ForegroundColor Red
    exit 1
}
Write-Host "  OK: git found" -ForegroundColor Green

# Check MSBuild / VS (vswhere handles VS2019 -> VS2026 and every edition)
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$msbuildExe = $null
$vsPath = $null
if (Test-Path $vsWhere) {
    $vsPath = & "$vsWhere" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath 2>$null | Select-Object -First 1
    $msbuildExe = & "$vsWhere" -latest -products * -requires Microsoft.Component.MSBuild `
        -find 'MSBuild\**\Bin\MSBuild.exe' 2>$null | Select-Object -First 1
}
if (-not $msbuildExe -or -not (Test-Path $msbuildExe)) {
    Write-Host "  WARNING: MSBuild not found via vswhere. You need Visual Studio 2019+ with" -ForegroundColor Yellow
    Write-Host "           'Desktop development with C++' workload installed." -ForegroundColor Yellow
    Write-Host "  Downloads: https://visualstudio.microsoft.com/downloads/" -ForegroundColor Yellow
    Write-Host ""
    $continue = Read-Host "  Continue without VS? (y/n)"
    if ($continue -ne 'y') { exit 1 }
    $msbuildExe = "msbuild"  # hope PATH works
} else {
    Write-Host "  OK: MSBuild found at $msbuildExe" -ForegroundColor Green
}

# Determine PlatformToolset: keep v142 (project default) if installed, else
# fall back to the newest v1xx toolset shipped with the detected VS.
$toolset = $null
if ($vsPath) {
    if (Test-Path (Join-Path $vsPath 'VC\Tools\MSVC\14.2*')) {
        $toolset = 'v142'
    } else {
        $latest = Get-ChildItem -Path (Join-Path $vsPath 'VC\Auxiliary\Build') -Directory -Filter 'v1*' -ErrorAction SilentlyContinue |
            Sort-Object Name -Descending | Select-Object -First 1
        if ($latest) { $toolset = $latest.Name }
    }
}
if ($toolset -eq 'v142') {
    Write-Host "  OK: v142 PlatformToolset (project default) is installed" -ForegroundColor Green
} elseif ($toolset) {
    Write-Host "  NOTE: v142 not installed; build will be retargeted to $toolset" -ForegroundColor Yellow
} else {
    Write-Host "  WARNING: Could not determine an MSVC C++ toolset. Make sure the C++ workload is installed." -ForegroundColor Yellow
}

# Check DirectX SDK
$dxSdkDir = "$repoRoot\dependencies\directx_2010"
if (Test-Path (Join-Path $dxSdkDir "Include\d3d9.h")) {
    Write-Host "  OK: DirectX SDK (June 2010) headers found" -ForegroundColor Green
} else {
    Write-Host "  WARNING: DirectX SDK (June 2010) not found at $dxSdkDir" -ForegroundColor Yellow
    Write-Host "  Download from: https://www.microsoft.com/en-us/download/details.aspx?id=6812" -ForegroundColor Yellow
    Write-Host "  After install, copy: C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include" -ForegroundColor Yellow
    Write-Host "     to: $dxSdkDir\Include" -ForegroundColor Yellow
    Write-Host "  And copy: C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86" -ForegroundColor Yellow
    Write-Host "     to: $dxSdkDir\Lib\x86" -ForegroundColor Yellow
    Write-Host ""
    $continue = Read-Host "  Continue anyway? (y/n)"
    if ($continue -ne 'y') { exit 1 }
}

Write-Host ""

# ---------- Step 2: restore submodules ----------

Write-Host "[2/4] Restoring git submodules..." -ForegroundColor Yellow

Push-Location $repoRoot
try {
    git submodule update --init --recursive
    if ($LASTEXITCODE -ne 0) { throw "git submodule update failed" }
    Write-Host "  OK: submodules restored" -ForegroundColor Green
} finally {
    Pop-Location
}

Write-Host ""

# ---------- Step 3: build ----------

Write-Host "[3/4] Building solution (Release x86)..." -ForegroundColor Yellow

Push-Location $repoRoot
try {
    $slnPath = Join-Path $repoRoot "twinject.sln"
    if (-not (Test-Path $slnPath)) {
        Write-Host "  ERROR: twinject.sln not found at $slnPath" -ForegroundColor Red
        exit 1
    }
    $buildArgs = @($slnPath, '/p:Configuration=Release', '/p:Platform=x86',
                   '/p:WindowsTargetPlatformVersion=10.0', '/m', '/v:minimal')
    if ($toolset -and $toolset -ne 'v142') { $buildArgs += "/p:PlatformToolset=$toolset" }
    & "$msbuildExe" $buildArgs
    if ($LASTEXITCODE -ne 0) { throw "Build failed (exit code $LASTEXITCODE)" }
    Write-Host "  OK: build succeeded" -ForegroundColor Green
} finally {
    Pop-Location
}

Write-Host ""

# ---------- Step 4: show output ----------

Write-Host "[4/4] Build artifacts:" -ForegroundColor Yellow
$releaseDir = Join-Path $repoRoot "Release"
if (Test-Path $releaseDir) {
    Get-ChildItem $releaseDir -Filter "*.exe" | ForEach-Object { Write-Host "  $($_.FullName)" -ForegroundColor White }
    Get-ChildItem $releaseDir -Filter "*.dll" | ForEach-Object { Write-Host "  $($_.FullName)" -ForegroundColor White }
} else {
    Write-Host "  WARNING: Release output directory not found. The build may have placed artifacts in the build target directory." -ForegroundColor Yellow
    Write-Host "  Check vcxproj output dirs for thsandbox.exe / twinhook.dll / twinject.exe" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Done! To deploy: copy twinject.exe + twinhook.dll + twinject.toml to your Touhou game directory." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan