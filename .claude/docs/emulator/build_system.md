# Build System

## Overview

The emulator uses CMake for the C backend and Vite/npm for the TypeScript frontend. The C build produces a single executable (`crc_emulator`) that links the firmware as a static library.

## CMake Structure

```
CMakeLists.txt              # Root: project settings, compiler defines, platform detection
├── firmware/CMakeLists.txt # Builds libfirmware.a from 250+ firmware .c files
└── emulator/CMakeLists.txt # Builds crc_emulator executable from HAL + bridge + sim
```

## Root CMakeLists.txt

**Location:** `/CMakeLists.txt`

### Settings
- **C Standard:** C99
- **Minimum CMake:** 3.20

### Compiler Defines

| Define | Value | Purpose |
|--------|-------|---------|
| `EMULATOR` | 1 | Guard for emulator-specific code paths |
| `DEMO` | 1 | Demo mode with simulated data |
| `MCA_SIMULATION` | 1 | Simulated MCA hardware |
| `WATCHDOG_OFF` | 1 | Disable watchdog timer |
| `FASTSTART` | 1 | Skip hardware initialization delays |
| `INVERSION` | 1 | Current firmware build variant |
| `CRC_R` | 1 | Standard CRC-R build |
| `PRINTF_ERRORS` | 1 | Print errors to stdout |
| `toupper` | `firmware_toupper` | Resolve name conflict with ctype.h |

### Platform Detection

```cmake
if(APPLE)
    add_definitions(-DPLATFORM_MACOS=1)
elseif(WIN32)
    add_definitions(-DPLATFORM_WINDOWS=1)
endif()
```

### Warning Suppressions

The firmware is legacy C code (originally for Green Hills Compiler) with many constructs that generate warnings on modern compilers. All suppressions are intentional:

```
-Wno-implicit-function-declaration  # Forward declarations omitted
-Wno-incompatible-pointer-types     # Void*/char* casts
-Wno-int-conversion                 # Integer/pointer mixing
-Wno-pointer-sign                   # Signed/unsigned char*
-Wno-format                         # printf format mismatches
-Wno-return-type                    # Missing return statements
-Wno-parentheses                    # Ambiguous operator precedence
-Wno-switch                         # Incomplete switch cases
-Wno-unused-variable                # Unused local variables
-Wno-deprecated-non-prototype       # Old-style function declarations
[... and more]
```

### Compiler Options

```cmake
-fno-common  # Each global gets own storage (required for ASAN)
```

## Firmware Library (`firmware/CMakeLists.txt`)

**Target:** `libfirmware.a` (static library)

### Source Collection

Collects all `*.c` files from `firmware/CRC-25R_Calib/` recursively, then removes files that are replaced by the emulator HAL.

### Excluded Files

These files are replaced by emulator equivalents:

| Excluded File | Replaced By |
|---------------|-------------|
| `CalibratorMain_R.c` | `main_emulator.c` |
| `Globals_25.c` | Globals defined in HAL |
| `UartRoutines.c` | `hal_uart.c` |
| `I2cRoutines.c` | `hal_i2c.c` |
| `PitRoutines_R.c` | `hal_pit.c` |
| `EepromRoutines.c` | `hal_i2c.c` |
| `Screen.c` | `hal_screen.c` |
| `Keyboard.c` | `hal_keyboard.c` |
| `Ethernet.c` | `hal_ethernet.c` |
| `ChipSelectIO_RP.c` | `hal_gpio.c` |
| All `USB/*.c` | `hal_usb.c` |
| All `Mmc/*.c` | `hal_fatfs.c` |

### Include Paths

```
firmware/CRC-25R_Calib/Headers/
firmware/CRC-25R_Calib/fatfs/src/
firmware/CRC-25R_Calib/USB/
```

### SQLite Configuration

SQLite3 is compiled from `sqlite3.c` (amalgamation) with these defines:

| Define | Purpose |
|--------|---------|
| `SQLITE_OS_OTHER=1` | Custom VFS (FatFS → POSIX via HAL) |
| `SQLITE_THREADSAFE=0` | Single-threaded (no mutex overhead) |
| `SQLITE_OMIT_LOAD_EXTENSION=1` | No dynamic library loading |

SQLite source file has additional warning suppressions applied.

## Emulator Executable (`emulator/CMakeLists.txt`)

**Target:** `crc_emulator` (executable)

### Source Files

```
emulator/main_emulator.c            # Entry point
emulator/hal/hal_coldfire.c          # ColdFire register emulation
emulator/hal/hal_uart.c              # UART channels
emulator/hal/hal_i2c.c              # I2C + EEPROM
emulator/hal/hal_qspi.c             # ADC/DAC simulation
emulator/hal/hal_pit.c              # Timer emulation
emulator/hal/hal_fatfs.c            # FatFS → POSIX
emulator/hal/hal_gpio.c             # GPIO stubs
emulator/hal/hal_usb.c              # USB stubs
emulator/hal/hal_rtc.c              # RTC → system time
emulator/hal/hal_watchdog.c         # Watchdog (disabled)
emulator/hal/hal_screen.c           # LCD stubs
emulator/hal/hal_keyboard.c         # Keyboard stubs
emulator/hal/hal_ethernet.c         # Ethernet stubs
emulator/hal/hal_stubs.c            # Miscellaneous stubs
emulator/amulet_bridge/amulet_bridge.c  # Protocol bridge
emulator/amulet_bridge/ws_server.c      # WebSocket server
emulator/simulation/sim_chamber.c   # ADC generation
emulator/simulation/sim_mca.c       # MCA spectra
emulator/simulation/sim_scenarios.c # Preset scenarios
emulator/platform/platform_macos.c  # macOS timer (conditional)
emulator/platform/platform_windows.c # Windows timer (conditional)
```

### Include Paths

```
emulator/hal/
emulator/compat/
emulator/amulet_bridge/
emulator/simulation/
emulator/platform/
firmware/CRC-25R_Calib/Headers/
firmware/CRC-25R_Calib/fatfs/src/
firmware/CRC-25R_Calib/USB/
```

### Link Libraries

| Platform | Libraries |
|----------|-----------|
| macOS | `CoreFoundation`, `pthread`, `m` |
| Windows | `ws2_32`, `winmm` |
| Linux | `pthread`, `m`, `rt` |
| All | `libfirmware.a` |

### Output

Binary: `build/emulator/crc_emulator`

## Frontend Build

### Dependencies (`package.json`)

**Runtime:**
- `react` ^18.2.0
- `react-dom` ^18.2.0
- `ws` ^8.19.0 (WebSocket)

**Dev:**
- `@types/react` ^18.2.0
- `@types/react-dom` ^18.2.0
- `@vitejs/plugin-react` ^4.0.0
- `typescript` ^5.0.0
- `vite` ^5.0.0

### Vite Configuration (`vite.config.ts`)

```typescript
export default defineConfig({
  plugins: [react()],
  server: { port: 3000 },
  build: { outDir: 'dist' }
})
```

### Commands

| Command | Description |
|---------|-------------|
| `npm install` | Install dependencies |
| `npx vite` | Start dev server (port 3000, HMR) |
| `npx vite build` | Production build → `frontend/dist/` |
| `npx tsc --noEmit` | Type-check without emitting |

## Build Commands

### Full Build (from repo root)

```bash
# Build C emulator
cmake -B build -S .
cmake --build build

# Build frontend
cd frontend && npm install && npx vite build
```

### Clean Build

```bash
rm -rf build
cmake -B build -S .
cmake --build build
```

### Debug Build

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Address Sanitizer (macOS)

```bash
cmake -B build -S . -DCMAKE_C_FLAGS="-fsanitize=address -fno-omit-frame-pointer"
cmake --build build
```

The `-fno-common` flag (set by default) is required for ASAN to detect global buffer overflows.

## HTM Parser (Build Tool)

**Location:** `tools/htm_parser/parse_htm.py`
**Language:** Python 3
**Dependencies:** BeautifulSoup4 (pip install)

Converts Amulet `.htm` files from `firmware/CRCHtml_3.00a/` into JSON screen definitions in `frontend/public/screens/`.

```bash
cd tools/htm_parser
python parse_htm.py
```

Output: 168 JSON files + `screen_map.json`

## Cross-Platform Notes

### macOS (Apple Silicon / Intel)
- Uses Apple Clang (from Xcode Command Line Tools)
- Timer: `dispatch_source_create()` via CoreFoundation
- Networking: BSD sockets

### Windows (x86 / ARM)
- MSVC or MinGW-w64
- Timer: `CreateThread()` + `WaitForSingleObject()`
- Networking: Winsock2 (`ws2_32.dll`)

### Linux (future)
- GCC or Clang
- Timer: pthreads + `clock_nanosleep()`
- Networking: BSD sockets
