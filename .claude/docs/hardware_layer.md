# Hardware Abstraction Layer

## Purpose

The hardware layer provides driver-level access to all MCF5282 peripherals and external hardware. These drivers are the lowest layer in the architecture and are called only by the hardware abstraction and application layers above them.

## Usage

Hardware initialization occurs sequentially in `main()` at `CalibratorMain_R.c:206-342`:

1. Disable interrupts: `initDisableInterrupts()` (line 227)
2. Chip selects: `initSetCS()` (line 230)
3. GPIO: `initSetGPIO()` (line 245)
4. I2C: `initSetI2C()` (line 248)
5. QSPI: `initSetQSPI()` (line 254)
6. SD card: `initSetSDCard()` (line 258)
7. Database: `initDB(FALSE)` (line 263)
8. EEPROM: `first_initialization()` (line 271)
9. Chambers: `initialize_chambers()`, `setup_chambers()` (lines 291-292)
10. Timer/PIT: `initSetTimer()` (line 299)
11. UART: Implicit via Amulet init (`initAmuletHTML()` at line 260)
12. USB: Post-timer initialization
13. MCA: Conditional on `DEMO` define (lines 334-338)

## Key Components

### UART Communication (`UartRoutines.c`)

Primary serial interface for:
- **Amulet display:** Touchscreen UI commands (GetByte, GetString, SetByte)
- **PC communication:** RS232 option (`Headers/crc.h:181` - `PC_COMM_RS232`)
- **Remote units:** External remote control panels

Configuration in `Headers/uart.h`. UART service called from `service_amulet()` at `Amulet.c:297`.

### I2C Bus (`I2cRoutines.c`)

Used for:
- **EEPROM access:** Read/write calibration data and configuration
- **Real-time clock:** Time/date operations (`RealTimeClock.c`)
- **Detector identification:** Reading detector EEPROM for serial/type

Configuration in `Headers/i2c.h`. Initialized at `CalibratorMain_R.c:248`.

### QSPI (`Qspi_25.c`)

Queued Serial Peripheral Interface for:
- **ADC communication:** Reading chamber voltage measurements
- **DAC control:** Setting output levels

Configuration in `Headers/qspi.h`. Initialized at `CalibratorMain_R.c:254`.

### PIT - Programmable Interrupt Timer (`PitRoutines_R.c`)

Drives the real-time measurement loop:
- Fixed-interval interrupt triggers ADC reads
- Feeds measurement data into `MEASUREMENT.adc_value` (`Headers/crc.h:385`)
- Manages gain-change countdown timers
- Services watchdog timer

Configuration in `Headers/pit.h`. Initialized at `CalibratorMain_R.c:299`.

### Chip Select IO (`ChipSelectIO_RP.c`)

GPIO-based chip select management for:
- MCA card selection
- ADC/DAC multiplexing
- External peripheral enable/disable
- Touch controller reset

Configuration in `Headers/cs.h`.

### EEPROM (`EepromRoutines.c`)

Legacy persistent storage using I2C EEPROM:
- `EE_READ(addr, data)` macro - Read data from EEPROM
- `EE_WRITE(addr, data)` macro - Write data to EEPROM
- Stores: printer config, system settings, chamber calibration data

Being migrated to SQLite database (see [Database API](database_api.md)).

### Real-Time Clock (`RealTimeClock.c`)

I2C-connected RTC providing:
- `read_clock(&clock_time)` - Read current time
- Timestamp generation for measurements and tests
- Time format support: mm/dd/yyyy, dd/mm/yyyy, yyyy/mm/dd (`current.time_format` at `Headers/crc.h:220`)

### MMC/SD Card (`Mmc/`)

FatFS-based SD card driver for MCF5282:

| File | Purpose |
|------|---------|
| `Mmc/mmc_m68.c` | M68K-specific MMC hardware driver |
| `Mmc/mmcapi.c` | High-level MMC API wrapper |
| `fatfs/ff.c` | FatFS file system implementation |

Power control: `power_mmc(TRUE/FALSE)` at `CalibratorMain_R.c:235`.

### USB (`USB/`)

SL811S USB host controller support:

| File | Purpose |
|------|---------|
| `USB/sl811s.c` | SL811S hardware driver |
| `USB/sl811h_101508.c` | Alternative USB host implementation |
| `USB/eps_subs.c` | Epson printer driver over USB |

USB modes defined at `Headers/crc.h:178-186`:
- `PC_COMM_USB` - Standard USB communication
- `PC_COMM_USB_DEBUG_CHAMBER` / `_WELL` - Debug modes
- `PC_COMM_USB_UPDATE` - Firmware update mode
- `PC_COMM_USB_UPLOAD_FILE` - File transfer mode

### Printer Support (`PrinterSubs_25.c`)

152KB printer control module supporting:
- Epson thermal printers (via USB, `USB/eps_subs.c`)
- OKI printers (custom protocol)
- Print formatting for test reports, labels, and receipts
- Printer selection via `current.printer` (`Headers/crc.h:214`)

## API Reference

### ADC/Measurement Reading

| Component | Description |
|-----------|-------------|
| `MEASUREMENT.adc_value` | Raw ADC reading per chamber (`Headers/crc.h:385`) |
| `MEASUREMENT.volts` | Calculated voltage (`Headers/crc.h:387`) |
| `MEASUREMENT.polarity` | Signal polarity (`Headers/crc.h:386`) |
| `MEASUREMENT.gain` | Current gain setting 0/1/2 (`Headers/crc.h:406`) |
| `set_adc_enabled(bool)` | Enable/disable ADC reads (`CalibratorMain_R.c:295`) |

### Chamber Hardware

| Function | Location | Description |
|----------|----------|-------------|
| `initialize_chambers()` | `CalibratorMain_R.c:291` | Detect and configure connected chambers |
| `setup_chambers()` | `CalibratorMain_R.c:292` | Apply calibration to chambers |
| `chamber_type()` | `Headers/chambfac.h` | Identify chamber type from hardware |

Chamber hardware values in `CHAMBERVALS` struct (`Headers/crc.h:233-248`):
- `resp_corr` - Response correction factor
- `nomvolts` - Nominal voltage
- `gainfactor0/1/2` - Multi-stage gain factors
- `feature_flag1` - Hardware capability bits (HV ADC, gain stages, stored factors)

### Watchdog

- `service_watchdog()` - Must be called periodically to prevent reset
- Can be disabled with `WATCHDOG_OFF` define (`Headers/crc.h:29`)

## Edge Cases

- **SD card not present:** `initSetSDCard()` failure at startup prevents database initialization; system may run in degraded mode
- **EEPROM corruption:** `first_initialization()` reads EEPROM with minimal validation; corrupted data propagates to global state
- **Chamber disconnection:** `CHAMBER.connected_flag` tracks live connection; disconnected chambers produce zero readings
- **Gain switching transient:** After gain change, `MEASUREMENT.gainchangecountdown` counts down before readings are trusted (`Headers/crc.h:426`); `MEASUREMENT.changed` tracks post-change readings
- **ADC zero drift:** `MEASUREMENT.adc_zero` compensated per-chamber (`Headers/crc.h:350`); buffer of 21 samples for averaging (`ADC_ZERO_BUF_SIZE` at `Headers/crc.h:378`)
- **Power sequencing:** MMC power must be enabled before SD card init; touch reset must be managed during startup sequence

## Dependencies

- **MCF5282 register definitions:** `Headers/mcf5282.h`, `Headers/coldfire.h`
- **FatFS configuration:** `Headers/ffconf.h`
- **USB hardware:** `Headers/SL811.h`, `Headers/sl811s.h`, `Headers/sl811h.h`
- **Linker scripts:** `src/standalone_ram.ld`, `src/standalone_romcopy.ld`, `src/standalone_romrun.ld`
