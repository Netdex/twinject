# build.ps1 - Build twinject solution (Release x86)
# Usage: PowerShell .\scripts\build.ps1
#
# Notes:
#  * The solution defines its platforms as x86 / x64 (they map to the
#    project-level Win32 platform), so we must pass /p:Platform=x86 - NOT Win32.
#  * The .vcxproj files target PlatformToolset v142 (VS2019). If that toolset is
#    not installed we transparently retarget the command-line build to whatever
#    MSVC toolset the detected Visual Studio ships (e.g. v143/v144/v145), so a
#    plain VS2022/VS2026 install builds without editing any project files.

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path $PSScriptRoot -Parent

function Find-MSBuild {
    # 1. Preferred: ask vswhere (handles VS2019 -> VS2026 and every edition)
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWhere) {
        $msbuild = & $vsWhere -latest -products * -requires Microsoft.Component.MSBuild `
            -find 'MSBuild\**\Bin\MSBuild.exe' 2>$null | Select-Object -First 1
        if ($msbuild -and (Test-Path $msbuild)) { return $msbuild }
    }
    # 2. Fall back to PATH
    $cmd = Get-Command msbuild -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    throw "MSBuild not found. Install Visual Studio with the 'Desktop development with C++' workload."
}

function Get-PlatformToolset {
    # Keep the project default (v142) when it is actually installed, otherwise
    # use the newest v1xx toolset that ships with the detected VS.
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vsWhere)) { return $null }
    $vsPath = & $vsWhere -latest -products * -property installationPath 2>$null | Select-Object -First 1
    if (-not $vsPath) { return $null }

    $platformToolsets = Join-Path $vsPath 'MSBuild\Microsoft\VC'
    $toolset = Get-ChildItem -Path $platformToolsets -Directory -Filter 'v1*' -Recurse -ErrorAction SilentlyContinue |
        Where-Object { $_.Parent.Name -eq 'PlatformToolsets' } |
        Sort-Object Name -Descending |
        Select-Object -First 1
    if ($toolset) { return $toolset.Name }
    return $null
}

$msbuildExe = Find-MSBuild
$toolset    = Get-PlatformToolset

$sln = Join-Path $repoRoot "twinject.sln"
if (-not (Test-Path $sln)) { Write-Error "Solution file not found: $sln" }

$msbuildArgs = @(
    $sln,
    '/p:Configuration=Release',
    '/p:Platform=x86',
    '/p:WindowsTargetPlatformVersion=10.0',
    '/m', '/v:minimal', '/nologo'
)
if ($toolset -and $toolset -ne 'v142') {
    Write-Host "v142 toolset not found - retargeting build to $toolset" -ForegroundColor Yellow
    $msbuildArgs += "/p:PlatformToolset=$toolset"
}

Write-Host "MSBuild: $msbuildExe" -ForegroundColor DarkGray
Write-Host "Building $sln (Release|x86)..." -ForegroundColor Cyan
& $msbuildExe $msbuildArgs
if ($LASTEXITCODE -ne 0) { throw "MSBuild failed (exit code $LASTEXITCODE)" }

# Gather artifacts
$outDir = Join-Path $repoRoot "Release"
$dist = Join-Path $repoRoot "dist"
if (-not (Test-Path $dist)) { New-Item -ItemType Directory -Path $dist | Out-Null }
foreach ($f in @("twinject.exe", "twinhook.dll")) {
    $artifact = Join-Path $outDir $f
    if (-not (Test-Path $artifact)) { throw "Build artifact not found: $artifact" }
    Copy-Item -Path $artifact -Destination $dist -Force
}
$config = Join-Path $repoRoot "deploy\twinject.toml"
if (-not (Test-Path $config)) { throw "Injector config not found: $config" }
Copy-Item -Path $config -Destination $dist -Force
Write-Host "Artifacts copied to $dist" -ForegroundColor Green
