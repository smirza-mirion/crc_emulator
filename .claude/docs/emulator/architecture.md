# Emulator Architecture

## Overview

The CRC-25R Emulator is a 3-tier system that runs the actual embedded firmware C code on a desktop operating system. It replaces all MCF5282 hardware dependencies with a software abstraction layer and provides a browser-based reproduction of the Amulet touchscreen display.

## System Layers

```
┌──────────────────────────────────────────────────────────┐
│  Web Frontend (React/TypeScript)                         │
│  ├─ DeviceDisplay: 800x600 pixel-accurate display        │
│  ├─ ScreenRenderer: Parses JSON screen defs, triggers    │
│  ├─ Widget components: buttons, text fields, images      │
│  ├─ ControlPanel: Scenarios, custom config, debug        │
│  ├─ AmuletStateManager: bytes/words/strings state mirror │
│  └─ WebSocketManager: Auto-reconnect WS client           │
├──────────────────────────────────────────────────────────┤
│  WebSocket JSON transport (port 9876)                    │
├──────────────────────────────────────────────────────────┤
│  Emulator Backend (C)                                    │
│  ├─ Amulet Bridge: UART protocol ↔ WebSocket JSON        │
│  ├─ HAL: ColdFire registers, UART, I2C, QSPI, PIT, etc. │
│  ├─ Simulation: ADC generation, MCA spectra, scenarios   │
│  └─ Platform: OS-specific timers and threading           │
├──────────────────────────────────────────────────────────┤
│  Firmware (250+ C files, compiled as libfirmware.a)      │
│  ├─ service_amulet() / amulet_menu(): UI service loop    │
│  ├─ 161 menu handlers (Amulet*Menu.c)                    │
│  ├─ SQLite3 database (works as-is)                       │
│  ├─ LanguageStrings.c: 891 English/French strings        │
│  └─ All business logic, calculations, test procedures    │
└──────────────────────────────────────────────────────────┘
```

## Data Flow

### Firmware → Display

1. Firmware calls `SetAmuletByte(index, value)` (in `Amulet.c:4242`)
2. This calls `SendUart2Tx()` which calls `PushUart2Tx()` (in `hal_uart.c:314`)
3. `PushUart2Tx()` calls `amulet_bridge_process_tx()` for each byte
4. Bridge accumulates bytes until null terminator, then calls `handle_tx_packet()`
5. Packet is parsed (command 0xD5 = setByte, 0xD7 = setString, etc.)
6. Bridge updates its shadow state and broadcasts JSON via WebSocket:
   ```json
   {"type": "setByte", "index": 20, "value": 255}
   ```
7. `AmuletStateManager` in the frontend receives the message
8. Subscribed React components re-render with the new value

### User Input → Firmware

1. User clicks a button in the browser
2. `AmuletButton` component calls `stateManager.pressButton(byteIndex, value)`
3. WebSocket sends: `{"type": "buttonPress", "byteIndex": 189, "value": 22}`
4. Bridge receives, encodes as Amulet protocol bytes, injects into UART2 RX buffer
5. Firmware's `service_amulet()` processes the command on next iteration
6. Menu handler responds (screen transition, measurement start, etc.)

### Page Navigation

1. Firmware calls `SetAmuletHTML(AmuletHTMLIndex[SCREEN_HTM])`
2. This generates a binary UART2 TX packet (0xA0 0x02 + page index)
3. Bridge intercepts, extracts page index, updates `currentPage`
4. Bridge broadcasts: `{"type": "setPage", "page": 18}`
5. Frontend's `ScreenRenderer` loads the JSON screen definition for page 18
6. New screen's widgets, triggers, and initActions are rendered

## Key Design Decisions

### Why HAL Instead of Modifying Firmware

The firmware uses hardware registers via a global `cf` struct (memory-mapped MCF5282 peripherals). Rather than modifying 250+ firmware files with `#ifdef EMULATOR`, the HAL provides drop-in replacements for the ~15 hardware driver files. The firmware's business logic, UI handlers, database layer, and string tables compile and run without any changes.

**Files replaced by HAL** (excluded from `libfirmware.a`):
- `CalibratorMain_R.c` → `main_emulator.c`
- `UartRoutines.c` → `hal_uart.c`
- `I2cRoutines.c` → `hal_i2c.c`
- `PitRoutines_R.c` → `hal_pit.c`
- `EepromRoutines.c` → `hal_i2c.c`
- `Qspi_25.c` → `hal_qspi.c`
- `ChipSelectIO_RP.c` → `hal_gpio.c`
- `Screen.c` → `hal_screen.c`
- `Keyboard.c` → `hal_keyboard.c`
- `Ethernet.c` → `hal_ethernet.c`
- All `Mmc/*.c` → `hal_fatfs.c`
- All `USB/*.c` → `hal_usb.c`

**Files that work unchanged:**
- `Amulet.c` and all 161 `Amulet*Menu.c` handlers
- `Database.c` and `sqlite3.c`
- `LanguageStrings.c` and `DisplayMessage.c`
- `NuclideData.c` and all calculation files
- All business logic and test procedure files

### Why WebSocket Instead of Direct Integration

The Amulet display is a physically separate device connected via UART. The emulator preserves this boundary: the firmware still "thinks" it's talking to an Amulet display via UART. The bridge intercepts at the UART level and translates to WebSocket, which:

1. Preserves protocol fidelity — all timing and sequencing match real hardware
2. Enables multiple connected frontends (up to 4 WebSocket clients)
3. Allows the frontend to reconnect without restarting the firmware
4. Keeps the firmware process and display process decoupled

### Why JSON Screen Definitions

The Amulet `.htm` files use a proprietary format with custom HTML attributes. Rather than parsing these at runtime in the browser, a Python build tool (`tools/htm_parser/parse_htm.py`) converts them to JSON. Benefits:

1. Clean separation of parsing complexity from rendering
2. JSON is fast to load and type-safe in TypeScript
3. Supports all Amulet features: variable bindings, refresh triggers, visibility conditions, language variants

## Build Architecture

```
firmware/CRC-25R_Calib/*.c ──► libfirmware.a (static library)
                                    │
emulator/main_emulator.c ──┐       │
emulator/hal/*.c ──────────┤       │
emulator/amulet_bridge/*.c ┼──► crc_emulator (executable)
emulator/simulation/*.c ───┤       │
emulator/platform/*.c ─────┘       │
                                    │
                              Links: libfirmware.a, pthread, sqlite3, math
                              macOS: + CoreFoundation
                              Windows: + ws2_32, winmm
```

## Threading Model

The emulator runs three threads:

1. **Main thread** — Runs the firmware's `measurement_screen()` forever loop, which calls `service_amulet()` → `amulet_menu()` → individual menu handlers
2. **Timer thread** — Platform-specific (dispatch queue on macOS, CreateThread on Windows), fires at 10ms interval to simulate the PIT interrupt. Calls `timed_interrupt()` for ADC reads and `sec_interrupt()` for once-per-second tasks
3. **WebSocket thread** — Handles incoming client connections and message dispatch (integrated into the bridge polling)

## Source File Reference

| File | Lines | Purpose |
|------|-------|---------|
| `main_emulator.c` | 554 | Entry point, init sequence, main loop |
| `hal/hal.h` | 123 | HAL interface declarations |
| `hal/hal_coldfire.c` | ~100 | MCF5282 register struct emulation |
| `hal/hal_uart.c` | ~400 | UART ring buffers, protocol routing |
| `hal/hal_i2c.c` | ~350 | I2C bus + EEPROM arrays + persistence |
| `hal/hal_qspi.c` | ~200 | ADC/DAC simulation interface |
| `hal/hal_pit.c` | ~300 | Timer, delay, screen saver emulation |
| `hal/hal_fatfs.c` | ~400 | FatFS → POSIX file I/O wrapper |
| `hal/hal_gpio.c` | ~150 | GPIO, chip select, backlight stubs |
| `amulet_bridge/amulet_bridge.c` | ~900 | Protocol parser + state + broadcast |
| `amulet_bridge/ws_server.c` | ~600 | WebSocket server (BSD sockets) |
| `simulation/sim_chamber.c` | ~200 | ADC generation with decay + noise |
| `simulation/sim_scenarios.c` | ~300 | 10 preset scenario configurations |
| `simulation/sim_mca.c` | ~150 | MCA spectrum generation |

## Implementation Plan Reference

The full implementation plan with detailed phase-by-phase breakdown is at:
`.claude/plans/snazzy-honking-nova.md`

Phases:
1. Project scaffolding + CMake build system
2. HAL layer (ColdFire, UART, I2C, QSPI, PIT, FatFS, stubs)
3. Emulator main entry point
4. Amulet protocol bridge + WebSocket server
5. HTM parser tool (Python)
6. Web frontend (React + device display + widgets)
7. Measurement simulation engine
8. Simulation control panel
9. Tauri desktop wrapper (future)
10. Integration testing + cross-platform builds
