/*
 * hal_rtc.c - Real-Time Clock Emulation for CRC-25R Emulator
 *
 * Replaces RealTimeClock.c. Maps RTC chip access (Dallas DS1338 via
 * I2C) to the host system clock. The firmware reads/writes time
 * through I2C transactions to the DS1338; in the emulator, we
 * simply return the current system time.
 *
 * Functions provided:
 *   gettime()      - Return time in FAT packed format (hour|min|sec)
 *   getdate()      - Return date in FAT packed format (year|month|day)
 *   read_clock()   - Read system time into time_t
 *   set_clock()    - Set clock (no-op in emulator)
 *   setup_rtc()    - Initialize RTC (no-op)
 *   mk_time()      - Convert struct tm to time_t (firmware's custom mktime)
 *   hal_rtc_i2c_read() - Return system time in BCD for I2C reads
 */
#include "compat.h"
#include "crc.h"
#include <time.h>
#include <string.h>

/* ---- Extern firmware globals ---- */
/* clock_time and today_clock are used by set_today_clock() in KeyInput_25.c */

/* ---- BCD Conversion Helpers ---- */

/*
 * to_bcd - Convert an integer (0-99) to BCD format
 *
 * Uses the same formula as the firmware: high nibble = tens digit * 16,
 * low nibble = ones digit.
 */
static uchar to_bcd(ushort num)
{
    return (uchar)(((num / 10) << 4) | (num % 10));
}

/*
 * from_bcd - Convert a BCD byte back to integer
 */
static ushort from_bcd(uchar bnum)
{
    return (ushort)(((bnum >> 4) * 10) + (bnum & 0x0F));
}

/* Suppress unused-function warnings for from_bcd */
static ushort (*from_bcd_ptr)(uchar) = from_bcd;

/* ================================================================
 * RTC API Implementation
 * ================================================================ */

/*
 * gettime - Return current time in FAT packed format
 *
 * Format:
 *   bit[15:11] = hours (0-23)
 *   bit[10:5]  = minutes (0-59)
 *   bit[4:0]   = seconds/2 (0-29)
 *
 * This is called by the FatFS layer (via get_fattime) and by
 * firmware code that needs the current time for timestamps.
 */
ushort gettime(void)
{
    time_t now;
    struct tm tm_buf;
    ushort hr, min, sec;

    now = time(NULL);
    gmtime_r(&now, &tm_buf);

    hr  = (ushort)((tm_buf.tm_hour & 0x1F) << 11);
    min = (ushort)((tm_buf.tm_min  & 0x3F) << 5);
    sec = (ushort)((tm_buf.tm_sec / 2)     & 0x1F);

    return hr | min | sec;
}

/*
 * getdate - Return current date in FAT packed format
 *
 * Format:
 *   bit[15:9] = year since 1980 (0-127)
 *   bit[8:5]  = month (1-12)
 *   bit[4:0]  = day (1-31)
 *
 * The RTC stores 2-digit year (00-99 = 2000-2099). The firmware
 * adds 20 to convert to years-since-1980 (so 2000 = year 20,
 * 2024 = year 44).
 */
ushort crc_getdate(void)
{
    time_t now;
    struct tm tm_buf;
    ushort years, months, days;

    now = time(NULL);
    gmtime_r(&now, &tm_buf);

    /* tm_year is years since 1900. FAT wants years since 1980. */
    years  = (ushort)(tm_buf.tm_year - 80);
    if (years > 127) years = 127;

    months = (ushort)(tm_buf.tm_mon + 1);
    days   = (ushort)tm_buf.tm_mday;

    return (ushort)((days & 0x1F) |
                    ((months & 0x0F) << 5) |
                    ((years  & 0x7F) << 9));
}

/*
 * read_clock - Read the current time from the "RTC"
 *
 * In the real firmware, this performs an I2C read of the DS1338
 * registers, converts BCD to binary, and packs into time_t.
 * In the emulator, we simply return the system time.
 *
 * Returns 1 on success (matching firmware convention).
 */
short read_clock(time_t *nowtime)
{
    if (!nowtime)
        return 0;

    *nowtime = time(NULL);
    return 1;
}

/*
 * set_clock - Set the RTC to the given time
 *
 * In the real firmware, this writes BCD time data to the DS1338
 * via I2C. In the emulator, we cannot set the system clock, so
 * this is a no-op that returns success.
 */
bool set_clock(struct tm *tmt)
{
    (void)tmt;
    /* Cannot set system clock in emulator. Return success anyway. */
    return TRUE;
}

/*
 * setup_rtc - Initialize the DS1338 RTC
 *
 * In the real firmware, writes to the control register to configure
 * the square wave output. No-op in emulator.
 */
bool setup_rtc(void)
{
    return TRUE;
}

/*
 * mk_time - Convert struct tm to time_t
 *
 * The firmware provides its own mktime implementation to avoid
 * Green Hills DST problems. We replicate the same algorithm for
 * exact compatibility with the firmware's date calculations.
 *
 * This computes seconds since Jan 1, 1970 00:00:00 UTC.
 */
static const short fmon[] = {428,459,122,153,183,214,244,275,306,336,367,397};
#define FYEAR0 69.

time_t mk_time(struct tm *tmt)
{
    float fyear1;
    short mon1, day1, year1, sec1, hour1, min1;
    time_t yy, secs, mins, dday;

    mon1  = (short)(tmt->tm_mon + 1);
    day1  = (short)tmt->tm_mday;
    year1 = (short)tmt->tm_year;
    sec1  = (short)tmt->tm_sec;
    hour1 = (short)tmt->tm_hour;
    min1  = (short)tmt->tm_min;

    fyear1 = (float)year1;

    if (mon1 <= 2)
        fyear1 -= 1.0f;

    yy   = (time_t)((int)(365.25 * fyear1) - (int)(365.25 * FYEAR0));
    dday = day1 - 1 + fmon[mon1 - 1] - fmon[0] + yy;
    mins = 60 * hour1 + min1;
    secs = 86400 * dday + 60 * mins + sec1;

    /* Day of the week: days since 1/1/89 (a Sunday) mod 7 */
    tmt->tm_wday = (int)((dday - 6940) % 7);

    return secs;
}

/* set_today_clock is provided by KeyInput_25.c in libfirmware.a */

/*
 * hal_rtc_i2c_read - Return current system time in DS1338 BCD format
 *
 * Called by hal_i2c.c when the firmware performs an I2C read from
 * the RTC slave address (0x68). The DS1338 register layout is:
 *
 *   Register 0: Seconds (BCD, bit 7 = oscillator halt, 0 = running)
 *   Register 1: Minutes (BCD)
 *   Register 2: Hours   (BCD, 24-hour format)
 *   Register 3: Day of week (1-7, 1 = Sunday)
 *   Register 4: Day of month (BCD)
 *   Register 5: Month   (BCD)
 *   Register 6: Year    (BCD, 00-99)
 *   Register 7: Control (0x10 = square wave enabled)
 */
void hal_rtc_i2c_read(uchar *data, int len)
{
    time_t now;
    struct tm tm_buf;
    uchar rtc_regs[8];

    if (!data || len <= 0)
        return;

    now = time(NULL);
    gmtime_r(&now, &tm_buf);

    /* Pack system time into DS1338 register format */
    rtc_regs[0] = to_bcd((ushort)tm_buf.tm_sec);    /* seconds, bit 7 = 0 (osc running) */
    rtc_regs[1] = to_bcd((ushort)tm_buf.tm_min);    /* minutes */
    rtc_regs[2] = to_bcd((ushort)tm_buf.tm_hour);   /* hours (24-hour) */
    rtc_regs[3] = (uchar)(tm_buf.tm_wday + 1);      /* day of week (1-7) */
    rtc_regs[4] = to_bcd((ushort)tm_buf.tm_mday);   /* day of month */
    rtc_regs[5] = to_bcd((ushort)(tm_buf.tm_mon + 1)); /* month (1-12) */

    /* Year: DS1338 stores 2-digit year. tm_year is years since 1900.
     * For 21st century: tm_year >= 100, so year = tm_year - 100. */
    if (tm_buf.tm_year >= 100)
        rtc_regs[6] = to_bcd((ushort)(tm_buf.tm_year - 100));
    else
        rtc_regs[6] = to_bcd((ushort)tm_buf.tm_year);

    rtc_regs[7] = 0x10; /* Control register: square wave output enabled */

    /* Copy requested number of registers */
    if (len > 8) len = 8;
    memcpy(data, rtc_regs, (size_t)len);
}
