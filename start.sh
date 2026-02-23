#!/usr/bin/env bash
# CRC-25R Emulator - Quick Start Script
# Usage: ./start.sh
# Starts the C emulator backend and Vite frontend, then opens the browser.

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
EMU_BIN="$SCRIPT_DIR/build/emulator/crc_emulator"
FRONTEND_DIR="$SCRIPT_DIR/frontend"
EMU_PORT=9876
VITE_PORT=3000

# ---- Cleanup any previous instances ----
echo "Stopping any existing processes..."
lsof -ti :$EMU_PORT 2>/dev/null | xargs kill -9 2>/dev/null || true
lsof -ti :$VITE_PORT 2>/dev/null | xargs kill -9 2>/dev/null || true
sleep 1

# ---- Build if needed ----
if [ ! -f "$EMU_BIN" ]; then
    echo "Emulator binary not found. Building..."
    cmake -B "$SCRIPT_DIR/build" -S "$SCRIPT_DIR" 2>&1
    cmake --build "$SCRIPT_DIR/build" 2>&1
fi

# ---- Install frontend deps if needed ----
if [ ! -d "$FRONTEND_DIR/node_modules" ]; then
    echo "Installing frontend dependencies..."
    (cd "$FRONTEND_DIR" && npm install)
fi

# ---- Start emulator backend ----
echo "Starting emulator backend on port $EMU_PORT..."
"$EMU_BIN" > /tmp/crc_emulator.log 2>&1 &
EMU_PID=$!

# Wait for WebSocket server to be ready
for i in $(seq 1 30); do
    if lsof -ti :$EMU_PORT >/dev/null 2>&1; then
        break
    fi
    sleep 0.2
done

if ! kill -0 $EMU_PID 2>/dev/null; then
    echo "ERROR: Emulator failed to start. Check /tmp/crc_emulator.log"
    exit 1
fi
echo "  Emulator running (PID $EMU_PID)"

# ---- Start Vite dev server ----
echo "Starting frontend on port $VITE_PORT..."
cd "$FRONTEND_DIR" && npx vite > /tmp/crc_vite.log 2>&1 &
VITE_PID=$!
cd "$SCRIPT_DIR"

# Wait for Vite to be ready
for i in $(seq 1 30); do
    if lsof -ti :$VITE_PORT >/dev/null 2>&1; then
        break
    fi
    sleep 0.2
done
echo "  Frontend running (PID $VITE_PID)"

# ---- Open browser ----
URL="http://localhost:$VITE_PORT"
echo ""
echo "Opening $URL ..."
if command -v open >/dev/null 2>&1; then
    open "$URL"
elif command -v xdg-open >/dev/null 2>&1; then
    xdg-open "$URL"
elif command -v start >/dev/null 2>&1; then
    start "$URL"
fi

echo ""
echo "=== CRC-25R Emulator Running ==="
echo "  Frontend:  $URL"
echo "  WebSocket: ws://localhost:$EMU_PORT/ws"
echo "  Logs:      /tmp/crc_emulator.log, /tmp/crc_vite.log"
echo ""
echo "Press Ctrl+C to stop all services."

# ---- Wait and cleanup on exit ----
trap 'echo ""; echo "Shutting down..."; kill $EMU_PID 2>/dev/null; kill $VITE_PID 2>/dev/null; lsof -ti :$VITE_PORT 2>/dev/null | xargs kill 2>/dev/null; echo "Done."; exit 0' INT TERM

wait $EMU_PID 2>/dev/null
