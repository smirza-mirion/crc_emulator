# CRC-25R Calibrator Firmware (v4.07a)

## Project Overview

Embedded C firmware for the **CRC-25R Radio Nuclide Calibrator** (Capintec) with Well Detector support. Used in medical/nuclear physics for measuring radioactive nuclide activity and performing quality control testing on radiation detection equipment. Targets Motorola ColdFire MCF5282 (32-bit M68K) running bare-metal.

## Tech Stack

| Component | Technology |
|-----------|-----------|
| Language | C (C89/C99), Green Hills Compiler (GHS) |
| Target CPU | Motorola ColdFire MCF5282 |
| Build System | Green Hills GBuild (`calib_r.gpj`) |
| Database | SQLite3 (embedded, single-file `sqlite3.c`) |
| File System | FatFS on SD/MMC cards |
| UI Framework | Amulet (HTML-based touchscreen UI via UART) |
| USB | SL811S host controller |
| VCS | Git (GitHub) |

## Key Directories

All paths relative to `4.07a/`:

```
CRC-25R_Calib/              Main firmware (250 C files, ~266K lines)
  Headers/                  61 header files - all struct/enum definitions
  Mmc/                      MMC/SD card drivers (M68K-specific)
  USB/                      SL811S USB host controller + printer drivers
  fatfs/                    FatFS file system implementation
  GHS/                      Green Hills compiler support files
  Images/                   115 bitmap assets for LCD display
  Fonts/                    17 custom screen fonts
CRCHtml_3.00a/              UI layer (197 .htm screen definitions + assets)
GEMstudioProject/           Amulet GEM Studio design project (339 pages)
```

## Critical Source Files

| File | Purpose |
|------|---------|
| `CalibratorMain_R.c:206` | `main()` entry point, initialization sequence |
| `Amulet.c:297` | `service_amulet()` - main UI service loop |
| `Amulet.c:2649` | `amulet_menu()` - menu state machine dispatcher |
| `Database.c:189` | `initDB()` - SQLite initialization, opens `/data/capintec.db` |
| `Mca.c:1001` | `Mca_startAcquisition()` - MCA spectrum acquisition |
| `Headers/crc.h:202` | `CURRENT` struct - global system state |
| `Headers/crc.h:343` | `CHAMBER` struct - per-chamber data |
| `Headers/crc.h:381` | `MEASUREMENT` struct - per-chamber measurements |
| `Headers/crc.h:57` | `DISPLAY_ERROR` macro - error handling pattern |
| `Headers/Amulet.h:1-169` | Screen HTM defines (168 screens) |
| `Headers/Amulet.h:338-499` | Menu ID defines (164 menus) |
| `Headers/Amulet.h:501-503` | Phase state machine constants |

## Build

**Build tool:** Green Hills GBuild
**Project file:** `4.07a/CRC-25R_Calib/calib_r.gpj`
**Output binary:** `crc_25r`

Key compiler defines (toggle in `Headers/crc.h:12-33`):
- `CRC_R` - Standard build (set in `calib_r.gpj:20`)
- `DEMO` / `DEMO_DATA` - Demo mode with simulated data
- `MCA_SIMULATION` - Simulate MCA hardware
- `WHITEBOX_TESTS` - Enable built-in unit tests
- `INVERSION` - Currently active (line 31)

Build variants: `CRC_CAL` (calibrator mode), `CRC_WELL` (well detector mode) - see `Headers/crc.h:73-76`

## Version Control

This project uses **Git** with a private GitHub remote at `Mirion-DataPlatform/crc_french`. Branch: `master`.

**Push after every update.** Each logical change (bug fix, feature addition, config tweak) should be its own commit pushed immediately. Do not accumulate uncommitted changes.

```
git add <files>             # Stage changed/new files
git commit -m "description" # Commit locally
git push                    # Push to origin/master
```

Repo-level git identity is already configured (Shahmeer Mirza / smirza@mirion.com). Legacy `.hg/` directories inside `4.07a/` subdirectories are Mercurial history from the original repo and are excluded via `.gitignore`.

## Naming Conventions

- `Amulet*Menu.c` - UI screen controllers (161 files), each maps to an `.htm` screen
- `*_25.c` / `*_R.c` - Hardware-revision-specific implementations
- `Headers/*.h` - All type definitions and function declarations
- `DB_*` functions - Database CRUD operations in `Database.c`
- `Mca_*` functions - Multi-Channel Analyzer operations in `Mca.c`
- `init*` functions - Initialization routines in `CalibratorMain_R.c`

## Global State

The system uses global structs declared `extern` across files:
- `CURRENT current` - System-wide settings (`Headers/crc.h:202`)
- `CHAMBER chamber[]` - Per-chamber configuration (`Headers/crc.h:343`)
- `MEASUREMENT measurement[]` - Per-chamber measurement data (`Headers/crc.h:381`)
- `DETECTOR detector[]` - Detector configuration (`Headers/crc.h:480`)
- `m_iMenu` / `m_iPhase` - UI navigation state (`Amulet.c`)

## Localization

The firmware is **dual-language**: index 0 = English, index 1 = French. This was migrated from English+Spanish in v4.07a.

| Component | Details |
|-----------|---------|
| Language enum | `ENGLISH=0`, `FRENCH=1` in `Headers/crc.h:195` |
| String table | 891 entries in `LanguageStrings.c` (`lang_str[]` + `lang_map[]`) |
| String reference | Commented `amulet_messages[]` in `DisplayMessage.c` (lines 24–5438) |
| String IDs | `amulet_strings_id` enum in `Headers/message.h` (891 `L_*` constants) |
| Runtime lookup | `get_amulet_message()` / `get_amulet_message_with_language()` in `DisplayMessage.c` |
| Database columns | Paired `*TextEnglish` / `*TextFrench` fields in 8 struct types |
| Button images | `french_*.png` in `CRCHtml_3.00a/Images/` (28 images) |
| Amulet fonts | Arial Black Bold and Franklin Gothic Book cover full Latin-1 (0x20–0xFF) for French accented characters |
| Extended ASCII | French uses: À É È Ô à ç è é ê ï ô û (all in Latin-1 / ISO 8859-1 range) |

## Additional Documentation

Consult these files for specialized topics:

| Document | When to Reference | Key Source Files |
|----------|------------------|-----------------|
| [Architectural Patterns](.claude/docs/architectural_patterns.md) | Understanding design decisions, state machines, transaction patterns | `Headers/crc.h`, `Headers/Amulet.h`, `Headers/chambfac.h` |
| [Database API](.claude/docs/database_api.md) | Working with SQLite layer, CRUD operations, bookEnd transactions | `Database.c`, `Headers/database.h`, `sqlite3.c` |
| [UI System](.claude/docs/ui_system.md) | Amulet screen navigation, menu handlers, HTML-to-C mapping | `Amulet.c`, `Headers/Amulet.h`, `Amulet*Menu.c`, `DisplayMessage.c` |
| [MCA Subsystem](.claude/docs/mca_subsystem.md) | Spectrum acquisition, peak finding, detector switching | `Mca.c`, `Headers/mca.h`, `Headers/err.h` |
| [Hardware Layer](.claude/docs/hardware_layer.md) | UART, I2C, QSPI, timers, EEPROM, chip selects | `CalibratorMain_R.c`, `UartRoutines.c`, `I2cRoutines.c`, `PitRoutines_R.c`, `EepromRoutines.c` |
