# CRC-25R Calibrator Emulator

A desktop emulator for the CRC-25R Radio Nuclide Calibrator firmware. Compiles and runs the **actual embedded C firmware** (250+ source files) on macOS and Windows with a browser-based pixel-accurate reproduction of the 800x600 Amulet touchscreen display.

## What This Is

The CRC-25R is a medical/nuclear physics instrument used for measuring radioactive nuclide activity. Its firmware runs on a Motorola ColdFire MCF5282 microcontroller with a proprietary Amulet HTML-based touchscreen.

This emulator replaces all hardware dependencies with a software abstraction layer (HAL), allowing the firmware to run natively on a desktop. A WebSocket bridge translates the Amulet UART protocol into JSON messages consumed by a React frontend that reproduces all 168 touchscreen screens.

**Key capabilities:**
- Runs the actual firmware C code (not a reimplementation)
- 168 interactive touchscreen screens with full navigation
- 10 preset simulation scenarios (Daily QC, Tc-99m measurement, accuracy test, etc.)
- Custom source configuration per chamber
- Dual language support (English/French)
- SQLite database persistence
- Simulated radioactive decay, ADC readings, and MCA spectra

## Prerequisites

### macOS (Apple Silicon or Intel)
- **CMake** 3.20+ (`brew install cmake`)
- **Apple Clang** (comes with Xcode Command Line Tools: `xcode-select --install`)
- **Node.js** 18+ (`brew install node`)
- **npm** (bundled with Node.js)

### Windows
- **CMake** 3.20+ (from [cmake.org](https://cmake.org/download/) or `winget install cmake`)
- **MSVC** (Visual Studio 2019+ with "Desktop development with C++") or **MinGW-w64**
- **Node.js** 18+ (from [nodejs.org](https://nodejs.org/) or `winget install OpenJS.NodeJS`)
- **npm** (bundled with Node.js)

## Quick Start

### One-Command Launch

**macOS / Linux:**
```bash
./start.sh
```

**Windows (PowerShell):**
```powershell
.\start.ps1
```

These scripts will:
1. Kill any stale emulator processes
2. Build the C emulator if the binary doesn't exist
3. Install frontend npm dependencies if `node_modules/` is missing
4. Start the emulator backend (WebSocket server on port 9876)
5. Start the Vite dev server (frontend on port 3000)
6. Open your browser to `http://localhost:3000`

Press `Ctrl+C` to stop all services.

### Manual Launch

If you prefer to start services individually:

```bash
# 1. Build the emulator
cmake -B build -S .
cmake --build build

# 2. Install frontend dependencies
cd frontend && npm install && cd ..

# 3. Start the emulator backend
./build/emulator/crc_emulator &

# 4. Start the frontend dev server
cd frontend && npx vite &

# 5. Open browser
open http://localhost:3000    # macOS
start http://localhost:3000   # Windows
```

### Stopping Services

**macOS / Linux:**
```bash
./stop.sh
```

**Windows (PowerShell):**
```powershell
.\stop.ps1
```

## Emulator Command-Line Options

```
./build/emulator/crc_emulator [options]

Options:
  --ws-port=PORT   WebSocket server port (default: 9876)
  --scenario=N     Load preset scenario (0-9, default: 0)
  --data-dir=DIR   Data directory (default: ./data)
  --help           Show help
```

## User Interface

The browser UI has two main panels:

### Device Display (Left, 800x600)
A pixel-accurate reproduction of the CRC-25R touchscreen. All buttons, text fields, images, and navigation work identically to the physical device. The display shows the current firmware screen with real-time updates from the emulated measurement engine.

### Simulation Control Panel (Right)
Three tabs for controlling the simulation:

**Scenarios Tab** — Select from 10 preset test configurations:

| ID | Scenario | Description |
|----|----------|-------------|
| 0 | Cold Start | No sources, fresh system (default) |
| 1 | Daily QC (Co-57) | Co-57 source at 5 uCi for daily QC |
| 2 | Tc-99m Measurement | Tc-99m at 15 mCi |
| 3 | Multi-Chamber | Tc-99m + F-18 across 2 chambers |
| 4 | MCA Well Detector | Cs-137 spectrum on well detector |
| 5 | Accuracy Test | 5 standard sources preconfigured |
| 6 | Linearity Test | High-activity Tc-99m (200 mCi) |
| 7 | Moly Assay | Mo-99/Tc-99m generator simulation |
| 8 | Background Only | No source, ambient background |
| 9 | Error Conditions | Over-range, chamber disconnect |

**Custom Tab** — Configure individual chamber sources:
- Enable/disable chambers
- Select nuclide (Tc-99m, Co-57, Ba-133, Cs-137, Co-60, F-18, I-131, Mo-99)
- Set activity with unit (uCi, mCi, Ci, kBq, MBq, GBq)

**Debug Tab** — Inspect internal Amulet variable state:
- Byte variables (indices 20, 21, 22, 23, 92, 100, 189-192)
- String variables (indices 0, 2, 3, 9-11, 80, 101)
- Current page index

## Screen Navigation

The emulator supports all 168 firmware screens. Primary navigation:

| Button | Screen | Description |
|--------|--------|-------------|
| Daily | Daily QC Test | 6-step quality control procedure |
| Background | Background Measurement | Measure ambient radiation |
| Chamber Volts | Chamber Voltage Check | Monitor chamber HV |
| Accuracy | Accuracy Test | Multi-source accuracy verification |
| Enhanced Tests | Enhanced Test Menu | Linearity, constancy, wipe tests |
| Moly | Moly Assay | Mo-99/Tc-99m generator testing |
| Inventory | Inventory Management | Source tracking and records |
| Util | Utilities | Reports, search, export |
| Setup | System Setup | Configuration, language, calibration |
| Home | Main Screen | Return to main measurement display |
| Back | Previous Screen | Navigate back one level |

Language switching: **Setup > Language > Francais/English**

## Architecture

```
Browser (React/TypeScript)          Emulator Backend (C)
┌─────────────────────────┐        ┌─────────────────────────────┐
│  DeviceDisplay (800x600)│◄──────►│  Amulet Bridge              │
│  ScreenRenderer         │  WS    │  ├─ Protocol Parser          │
│  Widget Components      │  JSON  │  ├─ State Manager            │
│  ControlPanel           │◄──────►│  └─ WebSocket Server (:9876) │
│  AmuletStateManager     │        │                               │
└─────────────────────────┘        │  Firmware (250+ .c files)     │
                                   │  ├─ service_amulet()          │
                                   │  ├─ amulet_menu() dispatcher  │
                                   │  ├─ 161 menu handlers         │
                                   │  ├─ SQLite3 database          │
                                   │  └─ All business logic        │
                                   │                               │
                                   │  HAL (Hardware Abstraction)   │
                                   │  ├─ UART emulation            │
                                   │  ├─ I2C / EEPROM              │
                                   │  ├─ QSPI / ADC / DAC          │
                                   │  ├─ PIT timer (OS thread)     │
                                   │  ├─ FatFS → POSIX             │
                                   │  └─ GPIO / USB stubs          │
                                   │                               │
                                   │  Simulation Engine            │
                                   │  ├─ Chamber ADC generator     │
                                   │  ├─ MCA spectrum simulator    │
                                   │  └─ 10 preset scenarios       │
                                   └─────────────────────────────┘
```

**Data flow:**
1. Firmware calls `SetAmuletByte/Word/String()` via UART channel 2
2. HAL intercepts UART TX, routes to Amulet Bridge
3. Bridge parses Amulet protocol, converts to JSON, broadcasts via WebSocket
4. React frontend updates widgets in real-time
5. User clicks a button in the browser
6. Frontend sends JSON via WebSocket to Bridge
7. Bridge injects Amulet protocol bytes into UART RX buffer
8. Firmware's `service_amulet()` processes the input

## Project Structure

```
crc_emulator/
├── CMakeLists.txt              # Top-level CMake (C99, emulator defines)
├── start.sh / start.ps1        # One-command launch (Bash / PowerShell)
├── stop.sh / stop.ps1          # One-command teardown
├── README.md                   # This file
│
├── emulator/                   # Emulator-specific C code
│   ├── CMakeLists.txt          # Emulator build config
│   ├── main_emulator.c         # Entry point (replaces firmware main)
│   ├── compat/
│   │   └── compat.h            # Platform compatibility macros
│   ├── hal/                    # Hardware Abstraction Layer
│   │   ├── hal.h               # HAL interface
│   │   ├── hal_coldfire.c      # MCF5282 register emulation
│   │   ├── hal_uart.c          # UART channels + ring buffers
│   │   ├── hal_i2c.c           # I2C bus + EEPROM emulation
│   │   ├── hal_qspi.c          # QSPI / ADC / DAC
│   │   ├── hal_pit.c           # PIT timer → OS timer thread
│   │   ├── hal_fatfs.c         # FatFS → POSIX file I/O
│   │   ├── hal_gpio.c          # GPIO / chip select stubs
│   │   ├── hal_usb.c           # USB stubs
│   │   ├── hal_rtc.c           # Real-time clock → system time
│   │   ├── hal_watchdog.c      # Watchdog (disabled)
│   │   ├── hal_screen.c        # LCD screen stubs
│   │   ├── hal_keyboard.c      # Keyboard stubs
│   │   ├── hal_ethernet.c      # Ethernet stubs
│   │   └── hal_stubs.c         # Miscellaneous stubs
│   ├── amulet_bridge/          # UART ↔ WebSocket bridge
│   │   ├── amulet_bridge.h     # Bridge API
│   │   ├── amulet_bridge.c     # Protocol parser + state manager
│   │   └── ws_server.c         # Embedded WebSocket server
│   ├── simulation/             # Measurement simulation
│   │   ├── sim_chamber.h       # Simulation API + data types
│   │   ├── sim_chamber.c       # ADC value generator
│   │   ├── sim_mca.c           # MCA spectrum generator
│   │   └── sim_scenarios.c     # 10 preset test scenarios
│   └── platform/               # OS-specific code
│       ├── platform.h          # Platform interface
│       ├── platform_macos.c    # macOS timer thread
│       └── platform_windows.c  # Windows timer thread
│
├── firmware/                   # Actual CRC-25R firmware source
│   ├── CMakeLists.txt          # Builds libfirmware.a
│   ├── CRC-25R_Calib/          # 250+ C source files + Headers/
│   └── CRCHtml_3.00a/          # 168 HTM screen definitions + assets
│
├── frontend/                   # React/TypeScript web UI
│   ├── package.json            # Dependencies (React 18, Vite 5)
│   ├── vite.config.ts          # Vite config (port 3000)
│   ├── tsconfig.json           # TypeScript config
│   ├── src/
│   │   ├── App.tsx             # Root component + WebSocket setup
│   │   ├── main.tsx            # React entry point
│   │   ├── components/
│   │   │   ├── DeviceDisplay.tsx    # 800x600 display container
│   │   │   ├── ScreenRenderer.tsx   # Dynamic screen + trigger engine
│   │   │   ├── ControlPanel.tsx     # Scenarios / Custom / Debug tabs
│   │   │   └── widgets/            # Amulet widget components
│   │   │       ├── AmuletButton.tsx
│   │   │       ├── AmuletFunctionButton.tsx
│   │   │       ├── AmuletStringField.tsx
│   │   │       ├── AmuletImage.tsx
│   │   │       ├── AmuletBarGraph.tsx
│   │   │       └── index.ts
│   │   └── lib/
│   │       ├── websocket.ts         # WebSocket client (auto-reconnect)
│   │       ├── amulet-state.ts      # State manager (bytes/words/strings)
│   │       ├── screen-loader.ts     # JSON screen definition loader
│   │       └── action-resolver.ts   # Widget action execution
│   └── public/
│       ├── screen_map.json          # Page index → screen name mapping
│       ├── screens/                 # 168 JSON screen definitions
│       └── assets/images/           # Button and UI images
│
├── tools/                      # Build-time tools
│   └── htm_parser/
│       ├── parse_htm.py        # Converts HTM → JSON screen defs
│       └── screen_defs/        # Generated JSON output
│
├── data/                       # Runtime data directory
│   └── capintec.db             # SQLite database (created at runtime)
│
└── docs/                       # Additional documentation
```

## Build System

The project uses CMake with two build targets:

1. **`libfirmware.a`** — Static library compiled from all firmware `.c` files (excluding hardware-specific files replaced by the HAL)
2. **`crc_emulator`** — Executable linking the HAL, bridge, simulation engine, and `libfirmware.a`

Key compiler defines:
- `EMULATOR=1` — Enables emulator-specific code paths
- `DEMO=1` — Demo mode with simulated data
- `MCA_SIMULATION=1` — Simulated MCA hardware
- `FASTSTART=1` — Skip hardware delays
- `WATCHDOG_OFF=1` — Disable watchdog timer
- `CRC_R=1` — Standard CRC-R build variant
- `PLATFORM_MACOS=1` or `PLATFORM_WINDOWS=1` — Platform selection

### Rebuild

```bash
cmake -B build -S .
cmake --build build
```

The emulator binary is output to `build/emulator/crc_emulator`.

## Ports and Services

| Service | Port | Protocol | Description |
|---------|------|----------|-------------|
| Emulator Backend | 9876 | WebSocket | Firmware ↔ Frontend bridge |
| Vite Dev Server | 3000 | HTTP | Frontend development server |

## Logs

| Log | Location | Contents |
|-----|----------|----------|
| Emulator | `/tmp/crc_emulator.log` | Firmware init, bridge messages, errors |
| Vite | `/tmp/crc_vite.log` | Frontend build and HMR output |
| Console | stdout (when run manually) | Real-time emulator debug output |

## Data Persistence

The emulator stores runtime data in the `data/` directory:
- `capintec.db` — SQLite database (test results, nuclide data, inventory)
- `eeprom_crc.bin` — Emulated main EEPROM (32 KB)
- `eeprom_mca.bin` — Emulated MCA EEPROM (8 KB)
- Subdirectories mirror the firmware's SD card filesystem

Data persists across emulator restarts. Delete `data/` to reset to factory defaults.

## Troubleshooting

### Emulator won't start
- Check that port 9876 is not in use: `lsof -i :9876`
- Run `./stop.sh` to kill stale processes
- Check logs: `cat /tmp/crc_emulator.log`

### Frontend shows "Disconnected"
- Verify the emulator backend is running: `lsof -i :9876`
- The frontend auto-reconnects every 2 seconds
- Check browser console for WebSocket errors

### Build errors
- Ensure CMake 3.20+ is installed: `cmake --version`
- On macOS, install Xcode CLI tools: `xcode-select --install`
- Clean build: `rm -rf build && cmake -B build -S . && cmake --build build`

### Blank screen in browser
- Wait for the emulator to fully initialize (check terminal output for "Initialization complete!")
- Hard-refresh the browser (`Cmd+Shift+R` / `Ctrl+Shift+R`)
- Check that `frontend/public/screens/` contains JSON screen definition files

### Port conflicts
- Change emulator port: `./build/emulator/crc_emulator --ws-port=9877`
- Change Vite port: edit `frontend/vite.config.ts` (`server.port`)

## Related Repositories

- **crc_french** — Firmware source repository (v4.07c, French localization)
- **Implementation Plan** — Full architectural details in `.claude/plans/`

## License

Proprietary. Internal use only.
