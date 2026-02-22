# CRC-25R Calibrator Emulator

## Project Overview

Desktop emulator for the CRC-25R Radio Nuclide Calibrator firmware. Compiles the actual embedded C firmware (250+ source files) with a Hardware Abstraction Layer (HAL) and bridges the Amulet touchscreen protocol to a React/TypeScript web frontend via WebSocket.

## Tech Stack

| Component | Technology |
|-----------|-----------|
| Firmware | C99 (compiled from original CRC-25R firmware source) |
| HAL + Bridge | C99 (emulator-specific, 26 source files) |
| Frontend | React 18, TypeScript 5, Vite 5 |
| Build System | CMake 3.20+ (C), npm/Vite (frontend) |
| Communication | WebSocket (JSON messages, port 9876) |
| Database | SQLite3 (from firmware, runs as-is) |
| VCS | Git (GitHub, branch: `emulator`) |

## Key Directories

```
emulator/                   Emulator C code (HAL, bridge, simulation)
  hal/                      Hardware abstraction (14 .c files)
  amulet_bridge/            UART ↔ WebSocket protocol bridge
  simulation/               ADC generation, MCA spectra, scenarios
  platform/                 OS-specific timers (macOS, Windows)
  compat/                   Compiler compatibility macros
firmware/                   Actual CRC-25R firmware (250+ .c files)
  CRC-25R_Calib/            Source + Headers/
  CRCHtml_3.00a/            168 HTM screen definitions + assets
frontend/                   React/TypeScript web UI
  src/components/           DeviceDisplay, ScreenRenderer, ControlPanel
  src/lib/                  WebSocket, state manager, screen loader
  public/screens/           168 JSON screen definitions
  public/assets/images/     Button and UI images
tools/htm_parser/           Python tool: HTM → JSON converter
```

## Critical Source Files

| File | Purpose |
|------|---------|
| `emulator/main_emulator.c` | Entry point (replaces firmware `main()`) |
| `emulator/hal/hal.h` | HAL interface declarations |
| `emulator/hal/hal_uart.c` | UART ring buffers, Amulet protocol routing |
| `emulator/hal/hal_pit.c` | PIT timer → OS timer thread |
| `emulator/hal/hal_fatfs.c` | FatFS → POSIX file I/O |
| `emulator/amulet_bridge/amulet_bridge.c` | Protocol parser + state + WebSocket broadcast |
| `emulator/amulet_bridge/ws_server.c` | Embedded WebSocket server |
| `emulator/simulation/sim_chamber.c` | ADC value generation with decay + noise |
| `emulator/simulation/sim_scenarios.c` | 10 preset test scenarios |
| `frontend/src/components/ScreenRenderer.tsx` | Dynamic screen rendering + trigger engine |
| `frontend/src/lib/amulet-state.ts` | Amulet variable state mirror |
| `frontend/src/lib/websocket.ts` | WebSocket client with auto-reconnect |

## Build

```bash
# C emulator
cmake -B build -S .
cmake --build build

# Frontend
cd frontend && npm install && npx vite build
```

**One-command launch:** `./start.sh` (macOS) or `.\start.ps1` (Windows)

Key compiler defines: `EMULATOR=1`, `DEMO=1`, `MCA_SIMULATION=1`, `FASTSTART=1`, `WATCHDOG_OFF=1`, `CRC_R=1`, `PLATFORM_MACOS=1` / `PLATFORM_WINDOWS=1`

## Version Control

Git with GitHub remote at `smirza-mirion/crc_emulator`. Branch: `emulator`.

## Architecture

The emulator has 3 layers:

1. **Frontend** (React) — 800x600 device display + control panel in browser
2. **Bridge** (C) — Translates Amulet UART protocol ↔ WebSocket JSON
3. **Firmware** (C) — Actual CRC-25R code compiled as `libfirmware.a`

Data flow: Firmware → `SetAmuletByte()` → UART TX → Bridge → JSON WebSocket → React state → widget re-render. User clicks: React → JSON WebSocket → Bridge → UART RX → `service_amulet()` → menu handler.

## WebSocket Protocol

**Server → Client:** `setByte`, `setWord`, `setString`, `setPage`, `fullState`, `drawLine`, `fillRect`
**Client → Server:** `buttonPress`, `stringInput`, `wordChanged`, `loadScenario`, `configSource`

## Naming Conventions

- `hal_*.c` — HAL module (one per hardware subsystem)
- `sim_*.c` — Simulation engine module
- `Amulet*.tsx` — Frontend widget component matching Amulet widget type
- `*.json` (in `public/screens/`) — Parsed HTM screen definitions

## Additional Documentation

| Document | When to Reference |
|----------|------------------|
| [Architecture](.claude/docs/emulator/architecture.md) | System design, data flow, threading model |
| [HAL Layer](.claude/docs/emulator/hal_layer.md) | Hardware emulation details per subsystem |
| [Amulet Bridge](.claude/docs/emulator/amulet_bridge.md) | UART protocol, state management, WebSocket |
| [Simulation Engine](.claude/docs/emulator/simulation_engine.md) | ADC generation, decay model, scenarios |
| [Frontend](.claude/docs/emulator/frontend.md) | React components, state manager, widgets |
| [Build System](.claude/docs/emulator/build_system.md) | CMake config, dependencies, cross-platform |
| [WebSocket Protocol](.claude/docs/emulator/websocket_protocol.md) | Message format, examples, variable indices |
| [Screen Rendering](.claude/docs/emulator/screen_rendering.md) | JSON format, triggers, language support |

## Related

- **Implementation Plan:** `.claude/plans/snazzy-honking-nova.md` (full phase-by-phase plan)
- **Firmware Docs:** See `crc_french` repo's `.claude/docs/` for firmware internals
