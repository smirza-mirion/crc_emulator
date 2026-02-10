# Architectural Patterns

## Layered Architecture

The system follows a 4-tier layered model:

```
UI Layer (Amulet HTML + C controllers)
    |
Application Logic (Measurement, QC tests, Analysis)
    |
Hardware Abstraction (Database, MCA, Printer, Keyboard/Screen)
    |
Hardware Drivers (UART, I2C, QSPI, PIT, MMC, USB)
```

Each layer only calls downward. UI controllers in `Amulet*Menu.c` files call application logic, which calls HAL functions, which call hardware drivers.

## Phase-Based State Machine (Menu Controllers)

Every `Amulet*Menu.c` file implements the same state machine pattern using phase variables. This is the primary control flow mechanism for multi-step operations.

**Core variables** (`Amulet.c`):
- `m_iMenu` - Current active menu ID (see `Headers/Amulet.h:338-499`)
- `m_iPhase` - Current execution phase
- `m_iCurPhase` / `m_iNextPhase` - Phase transition control

**Phase constants** (`Headers/Amulet.h:501-503`):
- `PHASE_GENERIC_BEFORE_INIT (0)` - Initial state before screen setup
- `PHASE_GENERIC_NOP (-1)` - No operation / idle
- `PHASE_GENERIC_CUR_NOP (-2)` - Current phase no-op (triggers transition)

**Pattern:** Each menu function checks `m_iPhase` via switch/if-else and performs the appropriate step. Phase transitions advance the operation (e.g., "initialize screen" -> "wait for input" -> "process result" -> "display output"). Found in all 166 `Amulet*Menu.c` files.

**Dispatcher:** `amulet_menu()` at `Amulet.c:2649` routes `m_iMenu` to the correct handler via a large switch statement covering all 164 menu IDs.

## BookEnd Transaction Pattern (Database)

Database operations that need atomicity use the `bookEnd` parameter pattern. This appears across ~50+ DB functions in `Database.c`.

**Implementation** (`Database.c:13258-13307`):
- `DB_StartBookEnd()` at line 13258: Executes `BEGIN;`
- `DB_EndBookEnd()` at line 13301: Executes `COMMIT;` on success, `ROLLBACK;` on failure

**Usage pattern:** Functions accept `bool bookEnd` parameter:
- `bookEnd=TRUE`: Function wraps itself in BEGIN/COMMIT (standalone operation)
- `bookEnd=FALSE`: Function runs within an existing transaction (batched operation)

**Example** (`Database.c:13267-13296`): `DB_CreateChamberAutoConstancy()` shows the complete pattern - BEGIN if bookEnd, bind parameters, execute, COMMIT/ROLLBACK if bookEnd.

**Helper functions:** All DB operations use a consistent sequence:
1. `d_prepare_v2()` - Prepare SQL statement with named parameters
2. `d_bind_*()` - Bind values (`d_bind_int`, `d_bind_text`, `d_bind_double`, `d_bind_int64`)
3. `d_step()` - Execute
4. `d_finalize()` - Clean up

## Global Singleton State

The system uses C global structs as singletons, declared `extern` in consuming files.

**Primary globals** (defined in `Headers/crc.h`):
- `CURRENT current` (line 202) - System-wide settings: units, language, security, chamber count, printer, PC comm mode
- `CHAMBER chamber[]` (line 343) - Per-chamber: existence, calibration, nuclide data, background, ADC zero
- `MEASUREMENT measurement[]` (line 381) - Per-chamber: activity buffer, ADC values, gain, mode, display flags
- `DETECTOR detector[]` (line 480) - Detector config: serial, HV, threshold, channel count
- `AUTOCAL` (line 456) - Auto-calibration: gain, sigma, linearity factors

**Access pattern:** Headers declare structs, individual `.c` files declare `extern CURRENT current;` (e.g., `DisplayMessage.c:19`).

## Error Display Macros

Three macros provide uniform error/warning/notification handling across the entire codebase. Defined at `Headers/crc.h:57-59`:

- `DISPLAY_ERROR(title, msg)` - Calls `AmuletError_display()`, returns from caller if dialog pushed
- `DISPLAY_WARNING(title, msg)` - Calls `AmuletWarning_display()`, returns from caller
- `DISPLAY_NOTIFICATION(title, msg)` - Calls `AmuletNotification_display()`, returns from caller

**Key behavior:** These macros contain `return` - they exit the calling function when the dialog is displayed. The display functions (`DisplayMessage.c:33`) deduplicate messages using an `oldMessages` array to prevent repeated popups.

## Chamber Factory / Type Abstraction

Six chamber types are supported, abstracted through factory functions in `Headers/chambfac.h:15-25`:

| Type | Enum | ID |
|------|------|----|
| Regular | `R_CHAMB` | 0 |
| PET | `P_CHAMB` | 1 |
| BT | `B_CHAMB` | 2 |
| 1.8 Atm | `ONE_DOT_EIGHT_CHAMB` | 3 |
| 100 Ci | `C_CHAMB` | 4 |
| 1000 Ci | `K_CHAMB` | 5 |

Factory functions (`Headers/chambfac.h:27-52`) compute type-specific values:
- `chamber_type()` - Identifies chamber type from hardware
- `calc_response()` - Type-specific response calculation
- `get_gainfact()` - Type-specific gain factor
- `calc_cnum()` - Calibration number computation
- `chamber_gain_factor_*()` - Multi-stage gain factor access

**Nuclide responses** are stored per-chamber-type: `NUCDATA.response[6]` holds one response value per chamber type (`Headers/crc.h:316`).

## Conditional Compilation for Variants

The firmware supports multiple build configurations through preprocessor defines in `Headers/crc.h:12-33`:

- **Hardware variants:** `CRC_R` (standard), chamber type flags
- **Debug/test:** `DEMO`, `DEMO_DATA`, `MCA_SIMULATION`, `WHITEBOX_TESTS`, `TESTING`
- **Feature toggles:** `ATTACHWELL`, `CALIB_ONLY`, `FASTSTART`, `INVERSION`
- **Mode:** `CRC_CAL` vs `CRC_WELL` runtime modes (`Headers/crc.h:74-76`)

Code blocks wrapped in `#ifdef DEMO`, `#ifdef MCA_SIMULATION`, etc. provide alternate paths throughout the codebase, particularly in `CalibratorMain_R.c` (initialization) and `Mca.c` (hardware vs simulated acquisition).

## Localization Pattern

All user-visible strings go through the localization system:

- `LanguageStrings.c` (224KB) - String table with English/Spanish pairs
- `LANG_MAP` struct maps language IDs to string pairs
- Language selected via `current.language` (`Headers/crc.h:223`: 0=English, 1=Spanish)
- UI strings referenced by `L_*` constants, resolved at runtime

## Measurement Loop Architecture

The main measurement cycle is interrupt-driven with foreground processing:

1. **PIT interrupt** (`PitRoutines_R.c`) triggers at fixed intervals
2. **ADC reads** chamber voltages into `MEASUREMENT.adc_value`
3. **Foreground loop** (`CalibratorMain_R.c:206+`) calls `service_amulet()` which:
   - Processes UART commands from touchscreen (`Amulet.c:297`)
   - Dispatches to active menu handler (`Amulet.c:2649`)
   - Menu handler reads measurement data and updates display
4. **Moving average** in `MEASUREMENT.actbuf[40]` smooths readings (`Headers/crc.h:384`)
5. **Gain switching** handled via `MEASUREMENT.gain`, `dogain`, `prevgain` fields

## EEPROM-to-Database Migration

The system maintains dual persistence:
- **EEPROM** (`EepromRoutines.c`) - Legacy storage, `EE_READ`/`EE_WRITE` macros
- **SQLite** (`Database.c`) - Primary storage, `capintec.db` on SD card

Many initialization paths read EEPROM first, then mirror to database. The `first_initialization()` function (`CalibratorMain_R.c:271`) loads EEPROM data, while `initDB()` (`Database.c:189`) initializes the database. This dual approach provides backward compatibility with older hardware revisions.
