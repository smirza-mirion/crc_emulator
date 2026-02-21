/*
 * hal_pit.c - PIT Timer Emulation for CRC-25R Emulator
 *
 * Replaces PitRoutines_R.c. The Programmable Interrupt Timer (PIT) in the
 * MCF5282 ColdFire drives:
 *   - 1ms tick counter (g_ulTickCounter)
 *   - ADC servicing (via service_adc)
 *   - Tone duration countdown
 *   - Keyboard scanning
 *   - Screen saver timeout
 *   - Second and minute counters
 *
 * In the emulator, the PIT hardware is replaced by an OS timer thread
 * (managed by hal_timer_start/stop in hal.h) that calls timed_interrupt()
 * approximately every 1ms. The blocking delay functions use OS sleep
 * primitives instead of polling PIT registers.
 */
#include "compat.h"
#include "crc.h"
#include "coldfire.h"
#include "pit.h"
#include "hal.h"
#include <string.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

/* ---- Extern firmware globals ---- */
extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern CURRENT current;
extern short max_chambers;
extern short tone_duration;
extern time_t clock_time;
extern time_t low_clock_time;

/* ---- Extern firmware functions ---- */
extern void service_adc(unsigned long int msec_tstamp);
extern void service_amulet(void);
extern void service_watchdog(void);
extern bool get_adc_enabled(void);

/* ---- Global timer counters ---- */

/* Millisecond tick counter, incremented every 1ms by timed_interrupt().
 * Used throughout the firmware for timing measurements, scheduling
 * chamber reads, and general-purpose timing.
 * Defined in RevData_R.c (libfirmware.a); declared extern here. */
extern volatile unsigned long int g_ulTickCounter;

/* Centisecond timestamp, incremented every 10ms. Used by delayloop()
 * and other firmware timing functions. */
volatile unsigned long int g_csec_tstamp = 0;

/* Millisecond timestamp, incremented every 1ms. Passed to service_adc()
 * and used for measurement timestamping. */
static volatile unsigned long int msec_tstamp = 0;

/* ---- Local timer state ---- */
static volatile ushort minute_counter = 0;     /* counts ms, resets at 60000 (1 min) */
static volatile bool display_time = TRUE;       /* set every second or minute */
static volatile short tick_counter = 0;         /* counts 0-9 for centisecond ticks */
static volatile short sec_counter = 0;          /* counts 0-999 for seconds */
static volatile short iSecondCounter = 0;       /* second counter within timed_interrupt */

/* Screen saver state */
static short sscount = 0;                       /* minutes since last keypress */
static bool screen_on = TRUE;                   /* screen currently on */
static short maxscr = 0;                        /* minutes before sleep (0 = never) */

/* ---- Blocking Delay Functions ---- */

/* Blocking delay in milliseconds. In the real firmware this uses PIT0
 * to count down. In the emulator we use OS sleep. Also services the
 * watchdog on each iteration to prevent timeout. */
void delay_msec(short nmsec)
{
    short i;
    for (i = 0; i < nmsec; i++) {
        service_watchdog();
#ifdef PLATFORM_WINDOWS
        Sleep(1);
#else
        usleep(1000);
#endif
    }
}

/* Blocking delay in microseconds. In the real firmware this uses PIT0
 * with a shorter modulus. In the emulator we use OS sleep. */
void delay_micro_sec(short nusec)
{
#ifdef PLATFORM_WINDOWS
    /* Windows Sleep has 1ms minimum; for sub-ms just yield */
    (void)nusec;
    Sleep(0);
#else
    usleep((unsigned int)nusec);
#endif
}

/* ---- Timer Interrupt Reset ---- */

/* Reset the PIT2 timer that generates the 1ms interrupt. In the emulator
 * the timer thread runs continuously, so this is a no-op. */
void reset_timed_interrupt(void)
{
    /* No-op: OS timer thread always runs */
}

/* ---- Interrupt Initialization ---- */

/* Initialize ColdFire interrupt controller, PIT, and UART interrupts.
 * In the emulator, interrupts are handled by the OS timer thread and
 * the Amulet bridge, so this is a no-op. The timer thread is started
 * separately via hal_timer_start(). */
void initialize_interrupts(void)
{
    /* No-op: timer thread handles all periodic processing */
}

/* Disable all ColdFire interrupts. In the emulator this is a no-op
 * since the timer thread handles its own synchronization. */
void disable_all_interrupts(void)
{
    /* No-op */
}

/* ---- Second Counter ---- */

/* Reset the second counter to zero. Used by firmware for USB timeout
 * and other second-resolution timing. */
void reset_sec_counter(void)
{
    sec_counter = 0;
}

/* Return the current second counter value (0-999, wraps every second) */
short get_sec_counter(void)
{
    return sec_counter;
}

/* ---- Minute Counter ---- */

/* Reset the minute counter to zero. Called when 60000ms have elapsed
 * to restart the minute cycle. */
void reset_minute_counter(void)
{
    minute_counter = 0;
}

/* Reset the minute counter but preserve the current seconds offset.
 * This synchronizes the minute boundary with the actual clock seconds. */
void reset_minute_counter_with_seconds(void)
{
    struct tm now;
    gmtime_r(&clock_time, &now);
    minute_counter = (ushort)(1000 * now.tm_sec);
    if (minute_counter >= 1000)
        minute_counter -= 1000;
    if (minute_counter >= 60000)
        minute_counter = 0;
}

/* Return the current minute counter value (0-59999, ms within minute) */
ushort get_minute_counter(void)
{
    return minute_counter;
}

/* ---- Display Time Flag ---- */

/* Return whether it is time to update the clock display. Set TRUE
 * every second by sec_interrupt() and every minute by timed_interrupt(). */
bool display_time_set(void)
{
    return display_time;
}

/* Clear the display time flag after the display has been updated */
void clear_display_time(void)
{
    display_time = FALSE;
}

/* Force the display time flag to TRUE */
void set_display_time(void)
{
    display_time = TRUE;
}

/* ---- Screen Saver ---- */

/* Initialize the screen saver. Reads the timeout from EEPROM (in real
 * firmware) and turns the screen on. In the emulator, the screen is
 * always on since there is no physical backlight. */
void init_screen_saver(void)
{
    sscount = 0;
    screen_on = TRUE;
    maxscr = 0; /* 0 = always on (no EEPROM read needed in emulator) */
}

/* Called after a keypress to reset the screen saver counter and
 * turn the screen back on if it was off. */
void key_press_screen_save(void)
{
    if (maxscr == 0)
        return;

    if (!screen_on) {
        screen_on = TRUE;
        /* In real firmware: set_backlight(TRUE); */
    }
    sscount = 0;
}

/* Service the screen saver (called every minute from timed_interrupt).
 * In the emulator this is mostly a no-op since we don't have a
 * physical backlight, but we maintain the state for firmware compatibility. */
static void service_screen_saver(void)
{
    if (!screen_on)
        return;
    if (maxscr == 0)
        return;

    ++sscount;
    if (sscount >= maxscr) {
        screen_on = FALSE;
        /* In real firmware: set_backlight(FALSE); */
    }
}

/* ---- Timed Interrupt (1ms tick handler) ---- */

/* Called by the OS timer thread approximately every 1ms. This is the
 * heart of the firmware's timing system. In the real firmware it runs
 * as PIT2's interrupt handler.
 *
 * Simplified for the emulator:
 *   1. Increment tick counter
 *   2. Update centisecond and millisecond stamps
 *   3. Periodically call service_adc() for ADC measurement updates
 *   4. Track minute boundaries for screen saver
 *
 * Keyboard scanning, USB polling, and tone handling are omitted since
 * all input comes from the Amulet touchscreen via WebSocket and there
 * is no physical speaker or USB host. */
void timed_interrupt(void)
{
    bool screen_flag = FALSE;
    short i;

    /* Update millisecond timestamp */
    msec_tstamp++;

    /* Track minute boundary (60000 ms = 1 minute) */
    ++minute_counter;
    if (minute_counter >= 60000) {
        reset_minute_counter();
        screen_flag = TRUE;
    }

    /* Track seconds for low_clock_time */
    ++iSecondCounter;
    if (iSecondCounter > 999) {
        low_clock_time++;
        iSecondCounter = 0;
    }

    /* Centisecond tick (every 10 ms) */
    if (++tick_counter >= 10) {
        g_csec_tstamp++;
        tick_counter = 0;
    }

    /* Second counter (wraps at 1000) */
    if (++sec_counter >= 1000) {
        sec_counter = 0;
    }

    /* Increment the global tick counter. Handle rollover the same way
     * as the firmware to keep measurement alarm ticks consistent. */
    g_ulTickCounter++;
    if (g_ulTickCounter > 0xFFFFE000) {
        g_ulTickCounter -= 0xFFFF0000;
        for (i = 0; i < max_chambers; i++) {
            if (chamber[i].connected_flag) {
                measurement[i].ulTickAlarm -= 0xFFFF0000;
            }
        }
    }

    /* Service ADC if enabled, using the same tick-based scheduling
     * as the firmware. adc_tick_time counts down from the inter-read
     * interval; when it reaches 0, service_adc() is called. */
    if (get_adc_enabled()) {
        extern ushort adc_tick_time;
        if (adc_tick_time > 0) {
            adc_tick_time--;
        } else {
            service_adc(msec_tstamp);
        }
    }

    /* Service screen saver on minute boundary */
    if (screen_flag)
        service_screen_saver();
}

/* ---- Second Interrupt ---- */

/* Called every second in the real firmware via PIT1. Updates the system
 * clock and sets the display time flag. In the emulator, the timer
 * thread can call this every 1000 ticks, or the timed_interrupt iSecondCounter
 * handles clock advancement via low_clock_time. */
void sec_interrupt(void)
{
    clock_time++;
    display_time = TRUE;
}

/* ---- Hardware Interrupt Stubs ---- */

/* IRQ5 interrupt handler for battery low / USB PC detection.
 * In the emulator there is no battery or physical USB-PC connection,
 * so this is a no-op. */
void irq5_interrupt(void)
{
    /* No-op: no battery or USB-PC hardware in emulator */
}

/* USB interrupt handler for SL811S host controller.
 * In the emulator there is no USB host hardware, so this is a no-op. */
void usb_interrupt(void)
{
    /* No-op: no USB host hardware in emulator */
}

/* ---- Delay Loop (centisecond-based) ---- */

/* Re-entrancy guard for delayloop. Prevents infinite recursion:
 *   delayloop → service_amulet → amulet_menu → menu_handler → delayloop
 * When already inside delayloop, nested calls just sleep without
 * calling service_amulet() again. */
static volatile int delayloop_depth = 0;

/* Blocking delay in centiseconds (10ms units). The firmware uses this
 * during initialization and other places where it needs to wait while
 * still servicing the Amulet display and ADC. In the emulator, we
 * use OS sleep for timing and call service_amulet() periodically to
 * keep the UI responsive — but only at the outermost call level. */
void delayloop(unsigned long int csec)
{
    unsigned long wait;

    delayloop_depth++;
    wait = g_csec_tstamp + csec;
    while (wait > g_csec_tstamp) {
        service_watchdog();
        /* Only call service_amulet at the outermost delayloop level
         * to prevent recursive re-entry into menu handlers */
        if (delayloop_depth == 1) {
            service_amulet();
        }
#ifdef PLATFORM_WINDOWS
        Sleep(10);
#else
        usleep(10000); /* 10ms sleep per iteration */
#endif
    }
    delayloop_depth--;
}

/* ---- Millisecond Counter ---- */

/* Clear the millisecond timestamp counter to zero. Used by firmware
 * to reset the timing base for certain operations. */
void clear_milli_sec_counter(void)
{
    msec_tstamp = 0;
}
