/*
 * platform.h - Platform Abstraction for CRC-25R Emulator
 *
 * Provides OS-specific timer and threading for the emulator.
 * The firmware's 1ms PIT interrupt is replaced with an OS timer
 * that calls timed_interrupt() periodically.
 */
#ifndef EMULATOR_PLATFORM_H
#define EMULATOR_PLATFORM_H

/*
 * Start the emulator timer thread.
 * Creates an OS timer that fires every 1ms and calls the firmware's
 * timed_interrupt() handler. Also sets up a 1-second timer for
 * sec_interrupt().
 */
void platform_timer_start(void);

/*
 * Stop the emulator timer thread.
 */
void platform_timer_stop(void);

/*
 * Sleep for the specified number of milliseconds.
 * Used by delay_msec() in hal_pit.c.
 */
void platform_sleep_ms(int ms);

/*
 * Sleep for the specified number of microseconds.
 * Used by delay_micro_sec() in hal_pit.c.
 */
void platform_sleep_us(int us);

/*
 * Get current time in milliseconds (monotonic).
 * Used for timing calculations.
 */
unsigned long platform_get_time_ms(void);

/*
 * Create the data/ directory if it doesn't exist.
 */
void platform_ensure_data_dir(void);

/*
 * Yield CPU briefly to prevent 100% CPU busy-loops.
 * Called from service_watchdog() in the main polling loop.
 */
void platform_yield(void);

#endif /* EMULATOR_PLATFORM_H */
