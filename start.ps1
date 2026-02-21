# CRC-25R Emulator - Quick Start Script (PowerShell)
# Usage: .\start.ps1
# Starts the C emulator backend and Vite frontend, then opens the browser.

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$EmuBin = Join-Path $ScriptDir "build\emulator\crc_emulator.exe"
$FrontendDir = Join-Path $ScriptDir "frontend"
$EmuPort = 9876
$VitePort = 5173

# ---- Cleanup any previous instances ----
Write-Host "Stopping any existing processes..."
Get-NetTCPConnection -LocalPort $EmuPort -ErrorAction SilentlyContinue |
    ForEach-Object { Stop-Process -Id $_.OwningProcess -Force -ErrorAction SilentlyContinue }
Get-NetTCPConnection -LocalPort $VitePort -ErrorAction SilentlyContinue |
    ForEach-Object { Stop-Process -Id $_.OwningProcess -Force -ErrorAction SilentlyContinue }
Start-Sleep -Seconds 1

# ---- Build if needed ----
if (-not (Test-Path $EmuBin)) {
    Write-Host "Emulator binary not found. Building..."
    cmake -B (Join-Path $ScriptDir "build") -S $ScriptDir
    cmake --build (Join-Path $ScriptDir "build")
}

# ---- Install frontend deps if needed ----
if (-not (Test-Path (Join-Path $FrontendDir "node_modules"))) {
    Write-Host "Installing frontend dependencies..."
    Push-Location $FrontendDir
    npm install
    Pop-Location
}

# ---- Start emulator backend ----
Write-Host "Starting emulator backend on port $EmuPort..."
$EmuProc = Start-Process -FilePath $EmuBin -NoNewWindow -PassThru -RedirectStandardOutput "$env:TEMP\crc_emulator.log" -RedirectStandardError "$env:TEMP\crc_emulator_err.log"

# Wait for WebSocket server to be ready
for ($i = 0; $i -lt 30; $i++) {
    $conn = Get-NetTCPConnection -LocalPort $EmuPort -ErrorAction SilentlyContinue
    if ($conn) { break }
    Start-Sleep -Milliseconds 200
}

if ($EmuProc.HasExited) {
    Write-Host "ERROR: Emulator failed to start. Check $env:TEMP\crc_emulator.log" -ForegroundColor Red
    exit 1
}
Write-Host "  Emulator running (PID $($EmuProc.Id))"

# ---- Start Vite dev server ----
Write-Host "Starting frontend on port $VitePort..."
Push-Location $FrontendDir
$ViteProc = Start-Process -FilePath "npx" -ArgumentList "vite" -NoNewWindow -PassThru -RedirectStandardOutput "$env:TEMP\crc_vite.log" -RedirectStandardError "$env:TEMP\crc_vite_err.log"
Pop-Location

# Wait for Vite to be ready
for ($i = 0; $i -lt 30; $i++) {
    $conn = Get-NetTCPConnection -LocalPort $VitePort -ErrorAction SilentlyContinue
    if ($conn) { break }
    Start-Sleep -Milliseconds 200
}
Write-Host "  Frontend running (PID $($ViteProc.Id))"

# ---- Open browser ----
$Url = "http://localhost:$VitePort"
Write-Host ""
Write-Host "Opening $Url ..."
Start-Process $Url

Write-Host ""
Write-Host "=== CRC-25R Emulator Running ===" -ForegroundColor Green
Write-Host "  Frontend:  $Url"
Write-Host "  WebSocket: ws://localhost:${EmuPort}/ws"
Write-Host "  Logs:      $env:TEMP\crc_emulator.log, $env:TEMP\crc_vite.log"
Write-Host ""
Write-Host "Press Ctrl+C to stop all services."

# ---- Wait and cleanup on exit ----
try {
    $EmuProc.WaitForExit()
} finally {
    Write-Host "`nShutting down..."
    if (-not $EmuProc.HasExited) { $EmuProc.Kill() }
    if (-not $ViteProc.HasExited) { $ViteProc.Kill() }
    Write-Host "Done."
}
