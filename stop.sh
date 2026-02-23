#!/usr/bin/env bash
# CRC-25R Emulator - Stop Script
# Usage: ./stop.sh
# Cleanly shuts down the emulator backend and Vite frontend.

EMU_PORT=9876
VITE_PORT=3000
KILLED=0

echo "Stopping CRC-25R Emulator services..."

# Kill emulator backend
PIDS=$(lsof -ti :$EMU_PORT 2>/dev/null)
if [ -n "$PIDS" ]; then
    echo "  Stopping emulator (port $EMU_PORT, PIDs: $PIDS)..."
    echo "$PIDS" | xargs kill 2>/dev/null
    sleep 1
    # Force kill if still running
    PIDS=$(lsof -ti :$EMU_PORT 2>/dev/null)
    if [ -n "$PIDS" ]; then
        echo "$PIDS" | xargs kill -9 2>/dev/null
    fi
    KILLED=$((KILLED + 1))
fi

# Kill Vite dev server
PIDS=$(lsof -ti :$VITE_PORT 2>/dev/null)
if [ -n "$PIDS" ]; then
    echo "  Stopping Vite (port $VITE_PORT, PIDs: $PIDS)..."
    echo "$PIDS" | xargs kill 2>/dev/null
    sleep 1
    PIDS=$(lsof -ti :$VITE_PORT 2>/dev/null)
    if [ -n "$PIDS" ]; then
        echo "$PIDS" | xargs kill -9 2>/dev/null
    fi
    KILLED=$((KILLED + 1))
fi

# Kill any orphaned crc_emulator processes
PIDS=$(pgrep -f crc_emulator 2>/dev/null)
if [ -n "$PIDS" ]; then
    echo "  Stopping orphaned emulator processes ($PIDS)..."
    echo "$PIDS" | xargs kill 2>/dev/null
    KILLED=$((KILLED + 1))
fi

if [ "$KILLED" -eq 0 ]; then
    echo "  No running services found."
else
    echo ""
    echo "All services stopped."
fi
