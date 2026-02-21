/*
 * hal_i2c.c - I2C + EEPROM Emulation for CRC-25R Emulator
 *
 * Replaces I2cRoutines.c and EepromRoutines.c.
 * Maintains in-memory byte arrays for each EEPROM device,
 * persisted to files on disk.
 *
 * EEPROM devices:
 *   0x50 (EE_CRC)  - Main calibrator EEPROM (32KB)
 *   0x53 (EE_MCA)  - MCA EEPROM (8KB)
 *   0x54 (EE_AUX)  - Auxiliary EEPROM (8KB)
 *   0x68 (RTC)     - Real-time clock (handled by hal_rtc.c)
 */
#include "compat.h"
#include "crc.h"
#include "coldfire.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* EEPROM sizes */
#define EEPROM_CRC_SIZE  32768
#define EEPROM_MCA_SIZE   8192
#define EEPROM_AUX_SIZE   8192

/* EEPROM I2C slave addresses (7-bit) */
#define EEPROM_CRC_ADDR   0x50
#define EEPROM_MCA_ADDR   0x53
#define EEPROM_AUX_ADDR   0x54
#define RTC_ADDR           0x68
/* DAC address for volume control */
#define DAC_ADDR           0x60

/* EEPROM types matching firmware defines */
#ifndef EE_CRC
#define EE_CRC  0
#define EE_MCA  1
#define EE_AUX  2
#endif

/* In-memory EEPROM storage */
static unsigned char eeprom_crc[EEPROM_CRC_SIZE];
static unsigned char eeprom_mca[EEPROM_MCA_SIZE];
static unsigned char eeprom_aux[EEPROM_AUX_SIZE];

/* I2C state */
static bool i2c_in_use = FALSE;
static int i2c_current_addr = 0;
static unsigned short i2c_write_offset = 0;
static unsigned char i2c_write_buf[256];
static int i2c_write_len = 0;

/* EEPROM mirror variables (from EepromRoutines.c) */
unsigned char m_ucVolumeMirror = 50;
unsigned char m_ucVolumeCurrent = 50;
uchar EepromRoutines_brightnessMirror = 80;
uchar EepromRoutines_brightnessCurrent = 80;
uchar EepromRoutines_sleepBrightnessMirror = 20;
uchar EepromRoutines_sleepBrightnessCurrent = 20;
uchar EepromRoutines_sleepTimeoutMirror = 10;
uchar EepromRoutines_sleepTimeoutCurrent = 10;
static unsigned long sleep_timeout_stamp = 0;

/* File paths */
static const char *eeprom_crc_path = "data/eeprom_crc.bin";
static const char *eeprom_mca_path = "data/eeprom_mca.bin";
static const char *eeprom_aux_path = "data/eeprom_aux.bin";

/* ---- File Persistence ---- */

static void load_eeprom(const char *path, unsigned char *buf, int size)
{
    FILE *f = fopen(path, "rb");
    if (f) {
        fread(buf, 1, size, f);
        fclose(f);
    } else {
        /* Initialize with 0xFF (erased state) */
        memset(buf, 0xFF, size);
    }
}

static void save_eeprom(const char *path, unsigned char *buf, int size)
{
    FILE *f = fopen(path, "wb");
    if (f) {
        fwrite(buf, 1, size, f);
        fclose(f);
    }
}

void hal_i2c_init(void)
{
    load_eeprom(eeprom_crc_path, eeprom_crc, EEPROM_CRC_SIZE);
    load_eeprom(eeprom_mca_path, eeprom_mca, EEPROM_MCA_SIZE);
    load_eeprom(eeprom_aux_path, eeprom_aux, EEPROM_AUX_SIZE);
}

void hal_i2c_shutdown(void)
{
    save_eeprom(eeprom_crc_path, eeprom_crc, EEPROM_CRC_SIZE);
    save_eeprom(eeprom_mca_path, eeprom_mca, EEPROM_MCA_SIZE);
    save_eeprom(eeprom_aux_path, eeprom_aux, EEPROM_AUX_SIZE);
}

/* ---- I2C Interface ---- */

void initialize_i2c(void)
{
    hal_i2c_init();
}

void initSetI2C(void)
{
    hal_i2c_init();
}

void setup_i2c(void)
{
    /* Reset I2C module - no-op, just reload EEPROMs */
}

bool get_i2c_in_use(void)
{
    return i2c_in_use;
}

/*
 * i2c_write - Emulate I2C write transaction
 *
 * For EEPROM writes, the first 2 bytes are the 16-bit address,
 * followed by the data bytes.
 */
bool i2c_write(int addr, uchar *data, int len, bool stop_flag)
{
    int slave = (addr >> 1) & 0x7F; /* Extract 7-bit address */
    (void)stop_flag;

    i2c_in_use = TRUE;

    if (slave == EEPROM_CRC_ADDR || slave == EEPROM_MCA_ADDR || slave == EEPROM_AUX_ADDR) {
        /* EEPROM write: first 2 bytes are address */
        if (len >= 2) {
            unsigned short offset = ((unsigned short)data[0] << 8) | data[1];
            unsigned char *eeprom = NULL;
            int eeprom_size = 0;
            const char *path = NULL;

            if (slave == EEPROM_CRC_ADDR) {
                eeprom = eeprom_crc; eeprom_size = EEPROM_CRC_SIZE;
                path = eeprom_crc_path;
            } else if (slave == EEPROM_MCA_ADDR) {
                eeprom = eeprom_mca; eeprom_size = EEPROM_MCA_SIZE;
                path = eeprom_mca_path;
            } else {
                eeprom = eeprom_aux; eeprom_size = EEPROM_AUX_SIZE;
                path = eeprom_aux_path;
            }

            /* Write data bytes after the 2-byte address */
            int i;
            for (i = 2; i < len && (offset + i - 2) < eeprom_size; i++) {
                eeprom[offset + i - 2] = data[i];
            }

            /* Store offset for potential subsequent reads */
            i2c_current_addr = slave;
            i2c_write_offset = offset + (len - 2);

            /* Auto-save on write */
            save_eeprom(path, eeprom, eeprom_size);
        } else if (len == 2) {
            /* Address-only write (for read setup) */
            i2c_write_offset = ((unsigned short)data[0] << 8) | data[1];
            i2c_current_addr = slave;
        }
    } else if (slave == DAC_ADDR) {
        /* DAC volume write - store value */
        if (len >= 1) {
            m_ucVolumeCurrent = data[0];
        }
    } else if (slave == RTC_ADDR) {
        /* RTC write - handled by hal_rtc.c via separate interface */
        /* Store for later use */
        i2c_current_addr = slave;
        if (len > 0) {
            memcpy(i2c_write_buf, data, len < 256 ? len : 255);
            i2c_write_len = len;
        }
    }

    i2c_in_use = FALSE;
    return TRUE;
}

/*
 * i2c_read - Emulate I2C read transaction
 *
 * Reads from the EEPROM at the current address (set by preceding write).
 */
bool i2c_read(int addr, uchar *data, int len0, bool restart_flag)
{
    int slave = (addr >> 1) & 0x7F;
    (void)restart_flag;

    i2c_in_use = TRUE;

    if (slave == EEPROM_CRC_ADDR || slave == EEPROM_MCA_ADDR || slave == EEPROM_AUX_ADDR) {
        unsigned char *eeprom = NULL;
        int eeprom_size = 0;

        if (slave == EEPROM_CRC_ADDR) {
            eeprom = eeprom_crc; eeprom_size = EEPROM_CRC_SIZE;
        } else if (slave == EEPROM_MCA_ADDR) {
            eeprom = eeprom_mca; eeprom_size = EEPROM_MCA_SIZE;
        } else {
            eeprom = eeprom_aux; eeprom_size = EEPROM_AUX_SIZE;
        }

        int i;
        for (i = 0; i < len0; i++) {
            if ((i2c_write_offset + i) < eeprom_size) {
                data[i] = eeprom[i2c_write_offset + i];
            } else {
                data[i] = 0xFF;
            }
        }
        i2c_write_offset += len0;
    } else if (slave == RTC_ADDR) {
        /* RTC read - return system time in BCD format */
        /* This is handled more completely in hal_rtc.c */
        extern void hal_rtc_i2c_read(uchar *data, int len);
        hal_rtc_i2c_read(data, len0);
    } else {
        /* Unknown device - return 0xFF */
        memset(data, 0xFF, len0);
    }

    i2c_in_use = FALSE;
    return TRUE;
}

/* ---- Volume Control ---- */

void SetVolume(unsigned char ucValue)
{
    m_ucVolumeCurrent = ucValue;
    /* In real hardware, this writes to I2C DAC. In emulator, just store it. */
}

/* ---- Serial Number ---- */

void ReadSN(char *sn)
{
    /* Read serial number from EEPROM offset - typically at a fixed location */
    /* For the emulator, provide a default */
    strncpy(sn, "EMU000001", 10);
    sn[9] = '\0';
}

void WriteSN(char *sn)
{
    (void)sn;
    /* No-op in emulator - serial number is fixed */
}

/* ---- EEPROM Read/Write ---- */

bool eewrite(ushort offset, uchar *data, int nbytes, int iEEType)
{
    unsigned char *eeprom = NULL;
    int eeprom_size = 0;
    const char *path = NULL;

    switch (iEEType) {
    case EE_CRC:
        eeprom = eeprom_crc; eeprom_size = EEPROM_CRC_SIZE;
        path = eeprom_crc_path;
        break;
    case EE_MCA:
        eeprom = eeprom_mca; eeprom_size = EEPROM_MCA_SIZE;
        path = eeprom_mca_path;
        break;
    case EE_AUX:
        eeprom = eeprom_aux; eeprom_size = EEPROM_AUX_SIZE;
        path = eeprom_aux_path;
        break;
    default:
        return FALSE;
    }

    if (offset + nbytes > eeprom_size) return FALSE;

    memcpy(&eeprom[offset], data, nbytes);
    save_eeprom(path, eeprom, eeprom_size);
    return TRUE;
}

bool eeread(uchar *data, ushort offset, int nbytes, int iEEType)
{
    unsigned char *eeprom = NULL;
    int eeprom_size = 0;

    switch (iEEType) {
    case EE_CRC:
        eeprom = eeprom_crc; eeprom_size = EEPROM_CRC_SIZE;
        break;
    case EE_MCA:
        eeprom = eeprom_mca; eeprom_size = EEPROM_MCA_SIZE;
        break;
    case EE_AUX:
        eeprom = eeprom_aux; eeprom_size = EEPROM_AUX_SIZE;
        break;
    default:
        return FALSE;
    }

    if (offset + nbytes > eeprom_size) return FALSE;

    memcpy(data, &eeprom[offset], nbytes);
    return TRUE;
}

/* ---- EEPROM Mirror Functions ---- */

void VolumeMirror(void)
{
    /* Read volume from EEPROM and set DAC */
    m_ucVolumeMirror = m_ucVolumeCurrent;
}

void VolumeMirrorUpdate(void)
{
    if (m_ucVolumeCurrent != m_ucVolumeMirror) {
        m_ucVolumeMirror = m_ucVolumeCurrent;
    }
}

void BrightnessMirror(void)
{
    EepromRoutines_brightnessMirror = EepromRoutines_brightnessCurrent;
}

void BrightnessMirrorUpdate(void)
{
    if (EepromRoutines_brightnessCurrent != EepromRoutines_brightnessMirror) {
        EepromRoutines_brightnessMirror = EepromRoutines_brightnessCurrent;
    }
}

void SleepBrightnessMirror(void)
{
    EepromRoutines_sleepBrightnessMirror = EepromRoutines_sleepBrightnessCurrent;
}

void SleepBrightnessUpdate(void)
{
    if (EepromRoutines_sleepBrightnessCurrent != EepromRoutines_sleepBrightnessMirror) {
        EepromRoutines_sleepBrightnessMirror = EepromRoutines_sleepBrightnessCurrent;
    }
}

void SleepTimeoutMirror(void)
{
    EepromRoutines_sleepTimeoutMirror = EepromRoutines_sleepTimeoutCurrent;
}

void SleepTimeoutUpdate(void)
{
    if (EepromRoutines_sleepTimeoutCurrent != EepromRoutines_sleepTimeoutMirror) {
        EepromRoutines_sleepTimeoutMirror = EepromRoutines_sleepTimeoutCurrent;
    }
}

void SleepRefreshTimeout(void)
{
    extern volatile unsigned long int g_ulTickCounter;
    sleep_timeout_stamp = g_ulTickCounter;
}

bool EepromRoutines_sleepNow(void)
{
    /* In emulator, never go to sleep */
    return FALSE;
}

void EepromRoutines_sleepWake(void)
{
    SleepRefreshTimeout();
}
