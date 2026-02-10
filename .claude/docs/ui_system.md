# Amulet UI System

## Purpose

The Amulet UI system provides touchscreen interaction for the CRC-25R calibrator. It uses an HTML-based rendering engine on a dedicated display controller that communicates with the MCF5282 firmware via UART. The firmware manages 168 screens and 164 menu controllers.

## Usage

### Screen Navigation

Screens are defined as HTML files in `CRCHtml_3.00a/` and identified by hex IDs in `Headers/Amulet.h:1-169`. To navigate to a screen:

1. Set the target screen: `SetAmuletHTML(AmuletHTMLIndex[SCREEN_HTM])`
2. Set the active menu: `m_iMenu = MENU_*`
3. Set initial phase: `m_iPhase = PHASE_GENERIC_BEFORE_INIT`

The dispatcher at `Amulet.c:2649` (`amulet_menu()`) routes to the handler matching `m_iMenu`.

### Creating a New Menu Handler

Each screen needs:
1. An `.htm` file in `CRCHtml_3.00a/`
2. A `#define *_HTM 0xNN` in `Headers/Amulet.h` (screen ID)
3. A `#define SET_HTML_* NN` in `Headers/Amulet.h` (set command)
4. A `#define MENU_* NN` in `Headers/Amulet.h` (menu ID)
5. A `void Amulet*_menu(void)` function (typically in its own `Amulet*Menu.c` file)
6. A `case MENU_*:` entry in the switch at `Amulet.c:2659`
7. Function declaration in `Headers/Amulet.h` (line 532+)

## Key Components

### Core Service Loop

| Function | Location | Role |
|----------|----------|------|
| `service_amulet()` | `Amulet.c:297` | UART communication handler - processes display commands (GetByte 0xD0, GetString 0xD2, SetByte 0xD5) |
| `amulet_menu()` | `Amulet.c:2649` | Menu dispatcher - routes `m_iMenu` to handler via switch |

### Phase State Machine

Every menu controller implements the phase pattern:

**Variables:**
- `m_iMenu` - Active menu ID (global)
- `m_iPhase` - Current phase (global)
- `m_iCurPhase` / `m_iNextPhase` - Transition control (global)

**Standard phases** (`Headers/Amulet.h:501-503`):
- `PHASE_GENERIC_BEFORE_INIT (0)` - Setup screen, initialize variables
- `PHASE_GENERIC_NOP (-1)` - Idle / waiting for input
- `PHASE_GENERIC_CUR_NOP (-2)` - Phase completed, ready to transition

**Custom phases:** Menu-specific phases are defined as local constants or enums within each `Amulet*Menu.c` file (e.g., `PHASE_ACCURACY_MEASURE_SOURCE1` through `PHASE_ACCURACY_FINISHED`).

### UART Protocol

The Amulet display controller communicates via command bytes:

| Command | Hex | Direction | Purpose |
|---------|-----|-----------|---------|
| GetByte | 0xD0 | Display -> MCU | Request byte value from firmware |
| GetString | 0xD2 | Display -> MCU | Request string value from firmware |
| SetByte | 0xD5 | Display -> MCU | Send byte value (button press, input) |

Handled in `service_amulet()` at `Amulet.c:297-400`.

### Screen ID Mapping

Screens are identified by two parallel systems:

1. **HTM defines** (`Headers/Amulet.h:1-169`): `MAINSCREEN_HTM = 0x09`, `DAILY_HTM = 0x10`, etc.
2. **SET_HTML defines** (`Headers/Amulet.h:171-337`): `SET_HTML_MAINSCREEN = 16`, `SET_HTML_DAILY = 4`, etc.

The `AmuletHTMLIndex[]` array maps HTM defines to the actual page addresses used by `SetAmuletHTML()`.

### Menu Controllers (166 files)

Each `Amulet*Menu.c` file contains one `void Amulet*_menu(void)` function. Major categories:

| Category | Examples | Count |
|----------|----------|-------|
| Calibrator QC | `AmuletDailyMenu.c`, `AmuletAccuracyMenu.c`, `AmuletLinearityMenu.c` | ~15 |
| Well Detector | `AmuletWellMeasurementMenu.c`, `AmuletWellMDAMenu.c` | ~30 |
| Setup/Config | `AmuletSetupMenu.c`, `AmuletSetupChamberMenu.c` | ~15 |
| Lab Tests | `AmuletWellSchillingMenu.c`, `AmuletWellPlasmaMenu.c` | ~20 |
| Inventory | `AmuletInventoryMenu.c`, `AmuletAddInventoryMenu.c` | ~8 |
| Reports/Search | `AmuletChamberReportsMenu.c`, `AmuletChamberSearchMenu.c` | ~15 |
| System | `AmuletErrorMsgMenu.c`, `AmuletLoginMenu.c`, `AmuletKeyboardMenu.c` | ~10 |

## API Reference

### Screen Control

| Function | Purpose |
|----------|---------|
| `SetAmuletHTML(page)` | Navigate to HTML page |
| `SetAmuletByte(widget, value)` | Set byte value on screen widget |
| `SetAmuletString(widget, str)` | Set string on screen widget |
| `GetAmuletByte(widget)` | Read byte value from screen widget |
| `GetAmuletString(widget, buf)` | Read string from screen widget |

### Navigation Helpers

| Function | Purpose |
|----------|---------|
| `AmuletPushPage(page)` | Push page onto navigation stack |
| `AmuletPopPage()` | Return to previous page |
| `AmuletSetMenu(menu, phase)` | Set active menu and initial phase |

### Error/Warning Display

Defined at `Headers/crc.h:57-59`, implemented in `DisplayMessage.c:33` and `AmuletWarning.c:31`:

| Macro | Function Called | Behavior |
|-------|----------------|----------|
| `DISPLAY_ERROR(title, msg)` | `AmuletError_display()` | Shows error dialog, returns from caller |
| `DISPLAY_WARNING(title, msg)` | `AmuletWarning_display()` | Shows warning dialog, returns from caller |
| `DISPLAY_NOTIFICATION(title, msg)` | `AmuletNotification_display()` | Shows notification, returns from caller |

All three deduplicate messages via `oldMessages[]` array (`DisplayMessage.c:33-58`).

## Edge Cases

- **Sleep mode:** `amulet_menu()` checks `EepromRoutines_sleepNow()` before dispatching - if true, forces `SLEEP_HTM` regardless of `m_iMenu` (`Amulet.c:2650`)
- **Phase transition race:** Phase only advances when `m_iPhase == m_iCurPhase` (`Amulet.c:2653`), preventing double-execution
- **Reentrant calls:** `service_amulet()` is called from the main loop, not from interrupts - no reentrancy protection needed
- **Screen stack overflow:** Navigation stack has fixed depth; deep navigation chains (>~8 levels) may overflow
- **UART buffer overflow:** Large string responses must fit UART buffer constraints

## Dependencies

- **Hardware:** UART peripheral for Amulet display communication (`UartRoutines.c`)
- **GEM Studio:** Screen layouts designed in GEM Studio (`GEMstudioProject/`)
- **HTML files:** 197 `.htm` files in `CRCHtml_3.00a/` define screen layouts
- **Fonts:** 50 font files in `CRCHtml_3.00a/Fonts/`
- **Images:** 145 image files in `CRCHtml_3.00a/Images/`
