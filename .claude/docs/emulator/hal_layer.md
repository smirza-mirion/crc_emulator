# Hardware Abstraction Layer (HAL)

## Purpose

The HAL replaces all MCF5282 hardware drivers with software emulation so that the actual firmware C code compiles and runs on macOS and Windows. Each HAL module provides drop-in replacements for the functions exported by the corresponding firmware driver file.

## Interface

All HAL modules are declared in `emulator/hal/hal.h`. Initialization and shutdown:

```c
void hal_init(void);      // Called from main_emulator.c, initializes ColdFire, UART, I2C
void hal_shutdown(void);   // Called on exit, persists EEPROM, closes resources
```

## Modules

### ColdFire Register Emulation (`hal_coldfire.c`)

**Replaces:** `Headers/coldfire.h` memory-mapped register access

The firmware accesses MCF5282 peripherals via a global `MCF5282_STRUCT cf` (normally at `MCF5282_BASE = 0x40000000`). In the emulator, `cf` is a regular global struct allocated in RAM.

**Key registers emulated:**
- `cf.uarts[0..2]` — UART status/data registers (used by `hal_uart.c`)
- QSPI registers — No-op (handled by `hal_qspi.c` at function level)
- PIT registers — No-op (handled by `hal_pit.c` at function level)
- I2C registers — No-op (handled by `hal_i2c.c` at function level)
- GPIO/EPORT — No-op
- `cf.cwsr` — Watchdog service register (writes ignored)

Also provides `vector_base` global (exception vector table stub).

### UART Emulation (`hal_uart.c`)

**Replaces:** `UartRoutines.c`

Implements ring buffers for UART channels 0-2:
- **Channel 0 (Printer):** TX buffer logged to `data/printer_output.txt`
- **Channel 1 (PC Comm):** TX/RX available for future TCP bridge
- **Channel 2 (Amulet):** TX routes to Amulet Bridge; RX fed by Bridge

**Key functions:**

| Function | Description |
|----------|-------------|
| `init_uart(baud, channel)` | Store config (no hardware setup) |
| `uart_char_waiting(channel)` | Check RX ring buffer |
| `uart_getchar(channel)` | Dequeue from RX ring buffer |
| `uart_putchar(channel, char)` | Enqueue to TX ring buffer |
| `PushUart2Tx(byte)` | Calls `amulet_bridge_process_tx()` |
| `SendUart2Tx(buf, len)` | Sends buffer via `PushUart2Tx()` |
| `PushUart2Rx(byte)` | Injects byte into Amulet RX (from Bridge) |
| `ClearUart2Rx/Tx()` | Reset ring buffers |
| `QueryPendingUart2Rx/Tx()` | Check buffer occupancy |

**Critical path:** `SetAmuletByte()` → `SendUart2Tx()` → `PushUart2Tx()` → `amulet_bridge_process_tx()`

### I2C + EEPROM Emulation (`hal_i2c.c`)

**Replaces:** `I2cRoutines.c`, `EepromRoutines.c`, `RealTimeClock.c` (partially)

Maintains in-memory byte arrays:
- `eeprom_crc[32768]` — Main EEPROM (address 0x50)
- `eeprom_mca[8192]` — MCA EEPROM (address 0x53)

**Persistence:** EEPROM data is saved to `data/eeprom_crc.bin` and `data/eeprom_mca.bin` on shutdown and loaded on startup. If files don't exist, arrays are zero-initialized.

**Key functions:**

| Function | Description |
|----------|-------------|
| `initSetI2C()` | Initialize I2C subsystem, load EEPROM files |
| `i2c_write(addr, data, len, stop)` | Write to emulated EEPROM |
| `i2c_read(addr, data, len, restart)` | Read from emulated EEPROM |
| `eewrite(data, offset, nbytes, type)` | High-level EEPROM write |
| `eeread(data, offset, nbytes, type)` | High-level EEPROM read |
| `VolumeMirror()` | Read volume from EEPROM into `current` |
| `BrightnessMirror()` | Read brightness from EEPROM |
| `SleepBrightnessMirror()` | Read sleep brightness |
| `SleepTimeoutMirror()` | Read sleep timeout |
| `SetVolume(level)` | Write volume to EEPROM |
| `ReadSN(buf)` / `WriteSN(buf)` | Serial number read/write |

**RTC address (0x68):** Returns system time in BCD format when read.

### QSPI / ADC / DAC Emulation (`hal_qspi.c`)

**Replaces:** `Qspi_25.c`

Instead of reading real ADC hardware, returns values from the simulation engine.

**Key functions:**

| Function | Description |
|----------|-------------|
| `initSetQSPI()` | No-op (QSPI hardware init) |
| `set_adc_enabled(bool)` | Enable/disable ADC reads |
| `service_adc()` | Called from timer interrupt; reads `sim_get_adc_value()` |
| `SetADCStateToIdle()` | Reset ADC state machine |
| `set_chamber_spi(channel)` | Select active chamber for reading |

**ADC data flow:** `timed_interrupt()` → `service_adc()` → `sim_get_adc_value(channel)` → `measurement[ch].adc_value`

### PIT Timer Emulation (`hal_pit.c`)

**Replaces:** `PitRoutines_R.c`

The firmware's 10ms PIT interrupt drives the measurement loop. Replaced with an OS timer thread:
- **macOS:** `dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, ...)` at 10ms
- **Windows:** `CreateThread()` with `WaitForSingleObject()` at 10ms

**Key functions:**

| Function | Description |
|----------|-------------|
| `delay_msec(n)` | `usleep(n * 1000)` (macOS) / `Sleep(n)` (Windows) |
| `delay_micro_sec(n)` | `usleep(n)` / `Sleep(n/1000)` |
| `timed_interrupt()` | Called every 10ms; services ADC, increments counters |
| `sec_interrupt()` | Called every second; time-based tasks |
| `reset_sec_counter()` / `get_sec_counter()` | Second counter management |
| `reset_minute_counter()` / `get_minute_counter()` | Minute counter management |
| `delayloop()` | Firmware delay with re-entrancy guard |
| `init_screen_saver()` / `key_press_screen_save()` | Screen saver timer |

**Timer counters:**
- `g_csec` — Centisecond counter (incremented each tick)
- `g_ulTickCounter` — Total tick count
- Second and minute counters for timeout management

### FatFS → POSIX Wrapper (`hal_fatfs.c`)

**Replaces:** `fatfs/ff.c`, `Mmc/mmc_m68.c`

Maps FatFS API calls to native POSIX file operations. The firmware's SD card filesystem is mapped to the `data/` directory on the host.

**Key function mappings:**

| FatFS Function | POSIX Equivalent |
|----------------|-----------------|
| `f_mount(drive, fs)` | `mkdir("data", 0755)` |
| `f_open(fp, path, mode)` | `fopen(mapped_path, mode_string)` |
| `f_read(fp, buf, len, &br)` | `fread(buf, 1, len, fp->posix_fp)` |
| `f_write(fp, buf, len, &bw)` | `fwrite(buf, 1, len, fp->posix_fp)` |
| `f_close(fp)` | `fclose(fp->posix_fp)` |
| `f_lseek(fp, offset)` | `fseek(fp->posix_fp, offset, SEEK_SET)` |
| `f_mkdir(path)` | `mkdir(mapped_path, 0755)` |
| `f_stat(path, fno)` | `stat(mapped_path, &st)` |
| `f_unlink(path)` | `remove(mapped_path)` |
| `f_rename(old, new)` | `rename(old_path, new_path)` |
| `f_opendir(dp, path)` | `opendir(mapped_path)` |
| `f_readdir(dp, fno)` | `readdir(dp->posix_dir)` |

**Path mapping:** Firmware paths like `/data/capintec.db` are mapped via `map_path()` which resolves the SD card root set by `hal_fatfs_set_sd_root()`. The working directory is changed to `data/` during initialization.

### GPIO / Chip Select Stubs (`hal_gpio.c`)

**Replaces:** `ChipSelectIO_RP.c`

All GPIO operations are no-ops since there is no physical hardware:

| Function | Behavior |
|----------|----------|
| `initSetCS()` | No-op |
| `write_lcd_cmd/data()` | No-op (LCD not used; Amulet is display) |
| `start_tone() / stop_tone()` | No-op (no speaker) |
| `set_backlight(level)` | No-op (no hardware backlight) |
| `power_mmc(on)` | No-op (no SD card power control) |
| `set_touch_reset(on)` | No-op |
| `battery_low()` | Returns FALSE (always on AC power) |
| `usb_pc_connected()` | Returns FALSE (no USB) |
| `read_mca_port() / write_mca_port()` | No-op |

### USB Stub (`hal_usb.c`)

**Replaces:** `USB/sl811s.c` and related USB files

| Function | Behavior |
|----------|----------|
| `initSetUSB()` | No-op |
| `startup_811()` | Returns error (no USB hardware) |

### RTC (`hal_rtc.c`)

**Replaces:** `RealTimeClock.c`

Maps real-time clock functions to host system time:

| Function | Behavior |
|----------|----------|
| `setup_rtc()` | Returns TRUE |
| `read_clock(&time)` | `*time = time(NULL)` |
| `set_clock(time)` | No-op (can't set system clock) |
| `gettime(h, m, s)` | Extract from `localtime()` |
| `getdate(y, m, d)` | Extract from `localtime()` |

### Watchdog Stub (`hal_watchdog.c`)

| Function | Behavior |
|----------|----------|
| `setup_watchdog()` | No-op (`WATCHDOG_OFF` defined) |
| `service_watchdog()` | No-op |

### Screen Stub (`hal_screen.c`)

**Replaces:** `Screen.c`

LCD operations are no-ops since the Amulet WebSocket display is the primary output:

| Function | Behavior |
|----------|----------|
| `initialize_screen()` | Prints "[LCD] Screen initialized" |
| `erase_screen()` | No-op |
| `display_text(x, y, text, color, size, mode)` | No-op |

### Keyboard Stub (`hal_keyboard.c`)

**Replaces:** `Keyboard.c`

Physical keyboard scanning is not used; all input comes through the Amulet touchscreen (WebSocket):

| Function | Behavior |
|----------|----------|
| `scan_keyboard()` | Returns 0 (no key) |
| `init_keyboard()` | No-op |

### Ethernet Stub (`hal_ethernet.c`)

| Function | Behavior |
|----------|----------|
| Ethernet init/send/receive | No-op stubs |

### Miscellaneous Stubs (`hal_stubs.c`)

Catches any remaining undefined symbols from the firmware build:
- `firmware_toupper()` — Wraps standard `toupper()` (firmware defines its own conflicting version)
- Various GHS runtime stubs
