/*
 * hal_gpio.c - GPIO / Chip Select Stubs for CRC-25R Emulator
 *
 * Replaces ChipSelectIO_RP.c and the static hardware initialization
 * functions from CalibratorMain_R.c. Most functions are no-ops since
 * there is no physical LCD, keyboard, speaker, or peripheral hardware
 * in the emulator.
 *
 * The firmware uses these functions to:
 *   - Write to the 128x64 LCD panel (replaced by Amulet touchscreen)
 *   - Scan the physical keypad (replaced by Amulet touchscreen)
 *   - Control the piezo speaker tone
 *   - Control the LCD backlight
 *   - Detect SD card insertion / manage SD card power
 *   - Control USB power and host controller registers
 *   - Detect battery low and USB PC connection
 *   - Read/write MCA DSP registers
 *   - Configure ColdFire GPIO ports, edge port, and interrupts
 *
 * In the emulator, all input comes from the Amulet touchscreen via
 * WebSocket, and there is no physical hardware to control.
 */
#include "compat.h"
#include "crc.h"
#include "coldfire.h"

/* ================================================================
 * Chip Select I/O Functions (from ChipSelectIO_RP.c / cs.h)
 * ================================================================ */

/*
 * initSetCS - Initialize chip select registers
 *
 * In the real firmware, configures the ColdFire chip select module
 * for LCD, keyboard, and peripheral access. No-op in emulator.
 */
void initSetCS(void)
{
    /* No-op: no chip select hardware */
}

/*
 * init_chip_selects - Initialize all chip select lines
 *
 * Called during firmware startup. No-op in emulator.
 */
void init_chip_selects(void)
{
    /* No-op: no chip select hardware */
}

/* ---- LCD Functions ---- */

/*
 * write_lcd_cmd - Write a command byte to the 128x64 LCD panel
 *
 * In the real firmware, this writes to the LCD controller's command
 * register via chip select CS1. No-op in emulator since the LCD
 * is not used (Amulet touchscreen is the primary display).
 */
void write_lcd_cmd(uchar cmd)
{
    (void)cmd;
}

/*
 * write_lcd_data - Write a data byte to the 128x64 LCD panel
 */
void write_lcd_data(uchar data)
{
    (void)data;
}

/*
 * read_lcd_data - Read a data byte from the 128x64 LCD panel
 *
 * Returns 0 since there is no LCD to read from.
 */
uchar read_lcd_data(void)
{
    return 0;
}

/* ---- Keyboard Functions ---- */

/*
 * write_keyboard - Write to the keyboard scan register
 */
void write_keyboard(uchar data)
{
    (void)data;
}

/*
 * read_keyboard - Read the keyboard scan result
 *
 * Returns 0xFF (no key pressed) since there is no physical keypad.
 */
uchar read_keyboard(void)
{
    return 0xFF;
}

/* ---- Tone / Speaker Functions ---- */

/*
 * start_tone - Enable the piezo speaker tone
 */
void start_tone(void)
{
    /* No-op: no speaker in emulator */
}

/*
 * stop_tone - Disable the piezo speaker tone
 */
void stop_tone(void)
{
    /* No-op: no speaker in emulator */
}

/* ---- Backlight Control ---- */

/*
 * set_backlight - Turn the LCD backlight on or off
 */
void set_backlight(bool ON)
{
    (void)ON;
    /* No-op: no physical backlight */
}

/* ---- SD Card / MMC Functions ---- */

/*
 * mmc_in - Check if an SD/MMC card is inserted
 *
 * Returns TRUE since the emulator always has a virtual SD card
 * (the local "data/" directory).
 */
bool mmc_in(void)
{
    return TRUE;
}

/*
 * power_mmc - Control SD/MMC card power
 */
void power_mmc(bool ON)
{
    (void)ON;
    /* No-op: no physical SD card power control */
}

/* ---- Counter Functions (Well Detector) ---- */

/*
 * clear_counters - Clear the hardware pulse counters
 */
void clear_counters(void)
{
    /* No-op: counters not simulated */
}

/*
 * enable_counting - Enable/disable the hardware pulse counters
 */
void enable_counting(bool on)
{
    (void)on;
    /* No-op: counters not simulated */
}

/*
 * load_counters - Latch current counter values for reading
 */
void load_counters(void)
{
    /* No-op: counters not simulated */
}

/*
 * read_counter - Read a hardware pulse counter value
 *
 * Returns 0 since counters are not simulated.
 */
ulong read_counter(short cnum)
{
    (void)cnum;
    return 0;
}

/*
 * read_error_counter - Read the error counter value
 */
ulong read_error_counter(void)
{
    return 0;
}

/*
 * debugPulse - Generate a debug pulse on a GPIO pin
 *
 * Returns 0 (no-op in emulator).
 */
ushort debugPulse(void)
{
    return 0;
}

/* ---- USB Hardware Control ---- */

/*
 * set_usb_register - Write a value to the SL811S USB host controller
 *                    address register via chip select
 */
void set_usb_register(uchar data)
{
    (void)data;
}

/*
 * read_usb_register - Read from the SL811S USB host controller
 *                     data register via chip select
 *
 * Returns 0 since there is no USB hardware.
 */
uchar read_usb_register(void)
{
    return 0;
}

/*
 * write_usb_register - Write to the SL811S USB host controller
 *                      data register via chip select
 */
void write_usb_register(uchar data)
{
    (void)data;
}

/*
 * set_usb_power - Control the USB bus power
 */
void set_usb_power(bool on)
{
    (void)on;
}

/*
 * set_host_usb_power - Control the USB host controller power
 */
void set_host_usb_power(bool on)
{
    (void)on;
}

/* ---- USB PC Connection ---- */

/*
 * usb_pc_connected - Check if a USB cable is connected to a PC
 *
 * Returns FALSE since there is no physical USB connection.
 */
bool usb_pc_connected(void)
{
    return FALSE;
}

/* ---- Battery ---- */

/*
 * battery_low - Check if the battery voltage is low
 *
 * Returns FALSE since the emulator runs on host power.
 */
bool battery_low(void)
{
    return FALSE;
}

/* ---- Well Detector Connection ---- */

/*
 * well_connection - Check well detector connection status
 *
 * Returns 0 (not connected) in the emulator.
 */
short well_connection(void)
{
    return 0;
}

/* ---- Amulet Touchscreen Power/Reset ---- */

/*
 * set_touch_power - Control power to the Amulet touchscreen module
 */
void set_touch_power(bool on)
{
    (void)on;
    /* No-op: touchscreen is emulated via WebSocket */
}

/*
 * set_touch_reset - Control the reset line to the Amulet touchscreen
 */
void set_touch_reset(bool on)
{
    (void)on;
    /* No-op: touchscreen is emulated via WebSocket */
}

/*
 * set_touch_program - Control the programming line to the Amulet touchscreen
 */
void set_touch_program(bool on)
{
    (void)on;
    /* No-op: touchscreen is emulated via WebSocket */
}

/* ---- MCA (Multi-Channel Analyzer) Port Access ---- */

/*
 * read_mca_port - Read a register from the MCA DSP
 *
 * In the real firmware, this reads from the MCA daughter board
 * via memory-mapped I/O. Returns 0 in the emulator since the
 * MCA is handled separately by the simulation engine.
 */
ushort read_mca_port(short offset)
{
    (void)offset;
    return 0;
}

/*
 * write_mca_port - Write to a register on the MCA DSP
 */
void write_mca_port(ushort offset, ushort value)
{
    (void)offset;
    (void)value;
    /* No-op: MCA handled by simulation engine */
}

/* ================================================================
 * ColdFire Hardware Initialization Stubs
 *
 * These functions are defined as static in the firmware's
 * CalibratorMain_R.c but are called during the init sequence.
 * In the emulator, the main entry point calls them non-statically,
 * so they need to be provided here as no-ops.
 * ================================================================ */

/*
 * initialize_edge_port - Initialize the ColdFire edge port module
 *
 * In the real firmware, configures edge port pins for IRQ detection
 * (battery low, USB PC connected, etc.). No-op in emulator.
 */
void initialize_edge_port(void)
{
    /* No-op: no edge port hardware */
}

/*
 * initSetEPORT - Initialize edge port registers
 */
void initSetEPORT(void)
{
    /* No-op: no edge port hardware */
}

/*
 * initSetInterrupt - Configure the ColdFire interrupt controller
 *
 * In the real firmware, sets up interrupt priorities and masks
 * for PIT, UART, edge port, and USB interrupts.
 */
void initSetInterrupt(void)
{
    /* No-op: interrupts handled by OS timer thread */
}

/*
 * initSetGPIO - Configure ColdFire GPIO port directions and functions
 *
 * In the real firmware, sets up GPIO pins for chip selects, status
 * LEDs, and peripheral control lines.
 */
void initSetGPIO(void)
{
    /* No-op: no GPIO hardware */
}

/*
 * initDisableInterrupts - Disable all ColdFire interrupts
 *
 * Called early in startup before peripherals are configured.
 */
void initDisableInterrupts(void)
{
    /* No-op: no hardware interrupts */
}

/*
 * initSetExceptions - Set up the ColdFire exception vector table
 *
 * In the real firmware, writes function pointers into the vector
 * table for each exception/interrupt source.
 */
void initSetExceptions(void)
{
    /* No-op: no vector table in emulator */
}
