/*
 * hal_qspi.c - QSPI / ADC / DAC Emulation for CRC-25R Emulator
 *
 * Replaces Qspi_25.c and Semaphore.c. The QSPI module in the firmware
 * controls SPI communication with ADC chips (MAX132, MCP3550) that read
 * ionization chamber voltages, SPI communication with DAC for high-voltage
 * control, and the chamber selection multiplexer.
 *
 * In the emulator, actual SPI hardware is replaced by calls to
 * sim_get_adc_value() which returns simulated ionization chamber readings.
 * The ADC state machine is collapsed into a simple periodic update that
 * writes simulated values into the firmware's measurement[] struct.
 */
#include "compat.h"
#include "crc.h"
#include "coldfire.h"
#include "qspi.h"
#include "hal.h"
#include <string.h>

/* ---- Extern firmware globals ---- */
extern volatile unsigned long int g_ulTickCounter;
extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern CURRENT current;
extern short max_chambers;

/* ---- QSPI buffer globals ----
 * These are referenced by firmware code that logs ADC transactions
 * (LowLevelSnoop). In the emulator they are unused but must exist
 * for linking. */
volatile long QSPI_BUFFER0_PTR = -1;
volatile long QSPI_BUFFER0[500];
volatile long QSPI_BUFFER1_PTR = -1;
volatile long QSPI_BUFFER1[500];

/* ---- ADC state machine enum ----
 * Mirrors the enum declared locally in Qspi_25.c. Only adc_idle is
 * actually used in the emulator; the rest exist for completeness. */
enum adc_status
{
    adc_idle,
    adc_max132_start_conversion,
    adc_max132_read_status,
    adc_max132_wait_for_eoc,
    adc_max132_start_read_result1,
    adc_max132_end_read_result1,
    adc_max132_start_read_result0,
    adc_max132_end_read_result0,
    adc_mcp3550_conversion,
    adc_max132_conversion,
    adc_service_remote
};

/* ---- Module state ---- */
static enum adc_status adc_state = adc_idle;
static bool adc_enabled = FALSE;
static short chamber_number = 0;            /* currently selected chamber */
static unsigned long last_service_tick = 0;  /* tick of last ADC service */

/* ADC tick timer - firmware uses this to schedule chamber reads */
ushort adc_tick_time = 0;

/* SPI semaphore state (from Semaphore.c) */
static int currentLockedBy = 0;
static int lastLoaded = 0;
static uchar spiBaud[5];

/* ---- SPI Semaphore Functions ---- */

/* Clear the SPI semaphore and reset ADC state to idle */
void spiClearSemaphore(void)
{
    currentLockedBy = SPI_SEMA_NULL;
    lastLoaded = SPI_SEMA_NULL;
    spiBaud[SPI_SEMA_SDCARD] = 0xff;
    adc_state = adc_idle;
}

/* Return which device currently holds the SPI bus (0 = none) */
int spiGetSemaphore(void)
{
    return currentLockedBy;
}

/* Load QSPI registers for the given device (no-op in emulator) */
void spiSemaphoreLoadValues(int lockedBy)
{
    lastLoaded = lockedBy;
    (void)lockedBy;
}

/* Acquire or release the SPI semaphore. Returns 0 on success, 0xFF on failure */
unsigned char spiSemaphore(int acq_release, int lockedBy)
{
    if (((acq_release == SPI_SEMA_ACQUIRE) || (acq_release == SPI_SEMA_RELEASE))
        && (lockedBy != SPI_SEMA_NULL))
    {
        if (acq_release == SPI_SEMA_RELEASE) {
            if (currentLockedBy == lockedBy) {
                currentLockedBy = SPI_SEMA_NULL;
                return 0x00;
            }
            return 0xff;
        } else {
            /* Acquire */
            if (currentLockedBy == SPI_SEMA_NULL) {
                currentLockedBy = lockedBy;
                lastLoaded = lockedBy;
                return 0x00;
            } else if (currentLockedBy == lockedBy) {
                return 0x00;
            }
            return 0xff;
        }
    }
    return 0xff;
}

/* Set the SPI baud rate for a device (no-op in emulator) */
void spiSetBaud(int lockedBy, uchar baud)
{
    if (lockedBy >= 0 && lockedBy < 5)
        spiBaud[lockedBy] = baud;
}

/* ---- QSPI Initialization ---- */

/* Initialize the QSPI subsystem. In the real firmware this configures
 * ColdFire QSPI registers. In the emulator it is a no-op since there
 * is no SPI hardware to configure. */
void initSetQSPI(void)
{
    spiClearSemaphore();
}

/* Initialize QSPI GPIO for chip selects (no-op in emulator) */
void init_gpio_primary_funct_QSPI(void)
{
    /* No-op: no GPIO pins to configure */
}

/* Initialize QSPI DAC output (no-op in emulator) */
void init_qspi_dac(void)
{
    /* No-op: no DAC hardware */
}

/* ---- ADC Enable / Disable ---- */

/* Enable or disable the ADC subsystem. When disabled, service_adc
 * will not update measurement data. */
void set_adc_enabled(bool on)
{
    adc_enabled = on;
}

/* Return whether the ADC subsystem is enabled */
bool get_adc_enabled(void)
{
    return adc_enabled;
}

/* ---- ADC Mode ---- */

/* Set the conversion type for a chamber: ADC_READ_ZERO or ADC_READ_VOLTS.
 * This tells service_adc whether to read the zero-offset or the actual
 * ionization chamber voltage. */
void set_adc_mode(short ch_num, ushort mode)
{
    measurement[ch_num].conv_type = (mode == ADC_READ_ZERO) ? ADC_READ_ZERO : ADC_READ_VOLTS;
}

/* ---- ADC State Control ---- */

/* Reset the ADC state machine to idle. Called when the SPI semaphore
 * is cleared or when a measurement cycle needs to restart. */
void SetADCStateToIdle(void)
{
    adc_state = adc_idle;
}

/* ---- MAX132 / MCP3550 Pin Control ---- */

/* Set the MAX132 output pins for gain switching. In the real firmware
 * this performs an SPI transaction to the MAX132 chip. In the emulator
 * it is a no-op since gain is handled by the simulation engine. */
void max132_set_output_pins(uchar pinstate)
{
    (void)pinstate;
    /* No-op: gain setting is handled in simulation */
}

/* Set the MCP3550 output pins for a specific channel. In the emulator
 * this is a no-op since there is no physical ADC chip. */
void mcp3550_set_output_pins(short ch, uchar pinstate)
{
    (void)ch;
    (void)pinstate;
    /* No-op: no physical MCP3550 chip */
}

/* ---- Chamber SPI Selection ---- */

/* Select a chamber and SPI mode (ADC, EEPROM, or Remote). In the real
 * firmware this configures chip select lines and mux control. In the
 * emulator we just record which chamber is selected. */
void set_chamber_spi(short ch_num, short mode)
{
    (void)mode;
    chamber_number = ch_num;
}

/* ---- ADC Service Routine ---- */

/* Advance to the next connected chamber, cycling through all chambers.
 * This mirrors the firmware's get_next_chamber() logic but simplified
 * for the emulator. */
static void get_next_chamber(void)
{
    short i;
    short start = chamber_number;

    for (i = 1; i <= max_chambers; i++) {
        short next = (start + i) % max_chambers;
        if (chamber[next].connected_flag) {
            chamber_number = next;
            return;
        }
    }
    /* No connected chambers found; stay on current */
}

/* Main ADC service routine. In the real firmware this is a multi-state
 * SPI transaction engine that reads ADC values over several interrupt
 * cycles. In the emulator, it periodically fetches simulated values
 * from the simulation engine and writes them into measurement[].
 *
 * Called from the PIT timer interrupt every millisecond in the firmware.
 * In the emulator, the timer thread calls this; we throttle updates
 * to approximately every 100ms to match the firmware's chamber cycling
 * rate (~500ms per chamber with multiple chambers). */
void service_adc(unsigned long int msec_tstamp)
{
    short ch = chamber_number;
    long sim_value;

    /* Throttle: only update every ~100 ticks (100ms) */
    if ((msec_tstamp - last_service_tick) < 100)
        return;
    last_service_tick = msec_tstamp;

    /* Only process connected chambers */
    if (ch < 0 || ch >= max_chambers || !chamber[ch].connected_flag) {
        get_next_chamber();
        return;
    }

    /* Get the simulated ADC value from the simulation engine */
    sim_value = sim_get_adc_value(ch);

    /* Update measurement struct based on conversion type */
    switch (measurement[ch].conv_type) {
        case ADC_READ_ZERO:
            /* Zero-offset reading */
            chamber[ch].adc_zero = (short)sim_value;
            break;

        case ADC_READ_VOLTS:
        default:
            /* Normal voltage reading from ionization chamber */
            measurement[ch].adc_value = sim_value;
            break;
    }

    /* Mark measurement as valid and record timestamp */
    measurement[ch].valid_flag = TRUE;
    measurement[ch].ulMilliSecStamp = msec_tstamp;

    /* Decrement gain change countdown if active */
    if (measurement[ch].gainchangecountdown)
        measurement[ch].gainchangecountdown--;

    /* Move to the next connected chamber */
    get_next_chamber();
}

/* ---- Chamber EEPROM on SPI bus (stubs) ---- */

/* Read from the on-chamber EEPROM via SPI. In the emulator this returns
 * zeroed data since chamber EEPROMs are not simulated. */
uchar *uc_eeprom_read(ushort address, ushort num_bytes)
{
    static uchar eeram[256];
    (void)address;
    (void)num_bytes;
    memset(eeram, 0, sizeof(eeram));
    return eeram;
}

/* Write to the on-chamber EEPROM via SPI (no-op in emulator) */
void uc_eeprom_write(ushort address, uchar *outbuffer, ushort num_bytes)
{
    (void)address;
    (void)outbuffer;
    (void)num_bytes;
    /* No-op: chamber EEPROM not simulated */
}

/* Disable writes to the on-chamber EEPROM (no-op in emulator) */
void uc_eeprom_wrdi(void)
{
    /* No-op */
}

/* Enable writes to the on-chamber EEPROM (no-op in emulator) */
void uc_eeprom_wren(void)
{
    /* No-op */
}

/* Read the status register of the on-chamber EEPROM. Returns 0 (ready) */
uchar uc_eeprom_rdsr(void)
{
    return 0; /* Always ready */
}

/* Write the status register of the on-chamber EEPROM (no-op in emulator) */
void uc_eeprom_wrsr(uchar status)
{
    (void)status;
    /* No-op */
}

/* read_from_chamber is provided by FactoryMenu_25.c in libfirmware.a */

/* Service remote display attached to a chamber (no-op in emulator).
 * The remote is a small LCD on the ionization chamber that receives
 * measurement data over SPI. */
void service_remote(short ch_num)
{
    (void)ch_num;
    /* No-op: remote displays not simulated */
}

/* Initialize MMC/SD card over SPI (no-op in emulator; handled by hal_fatfs) */
bool init_mmc_card(void)
{
    return TRUE;
}
