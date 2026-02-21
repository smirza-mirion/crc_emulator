# CRC-25R Emulator - Stop Script (PowerShell)
# Usage: .\stop.ps1
# Cleanly shuts down the emulator backend and Vite frontend.

$EmuPort = 9876
$VitePort = 5173
$Killed = 0

Write-Host "Stopping CRC-25R Emulator services..."

# Kill emulator backend
$conns = Get-NetTCPConnection -LocalPort $EmuPort -ErrorAction SilentlyContinue
if ($conns) {
    $pids = $conns | Select-Object -ExpandProperty OwningProcess -Unique
    Write-Host "  Stopping emulator (port $EmuPort, PIDs: $($pids -join ', '))..."
    foreach ($pid in $pids) {
        Stop-Process -Id $pid -Force -ErrorAction SilentlyContinue
    }
    $Killed++
}

# Kill Vite dev server
$conns = Get-NetTCPConnection -LocalPort $VitePort -ErrorAction SilentlyContinue
if ($conns) {
    $pids = $conns | Select-Object -ExpandProperty OwningProcess -Unique
    Write-Host "  Stopping Vite (port $VitePort, PIDs: $($pids -join ', '))..."
    foreach ($pid in $pids) {
        Stop-Process -Id $pid -Force -ErrorAction SilentlyContinue
    }
    $Killed++
}

# Kill any orphaned crc_emulator processes
$procs = Get-Process -Name "crc_emulator" -ErrorAction SilentlyContinue
if ($procs) {
    Write-Host "  Stopping orphaned emulator processes ($($procs.Id -join ', '))..."
    $procs | Stop-Process -Force -ErrorAction SilentlyContinue
    $Killed++
}

if ($Killed -eq 0) {
    Write-Host "  No running services found."
} else {
    Write-Host ""
    Write-Host "All services stopped."
}
