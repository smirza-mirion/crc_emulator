/*
 * hal_watchdog.c - Watchdog Timer Stub
 *
 * The firmware's watchdog timer resets the CPU if not serviced within ~8 seconds.
 * In the emulator, the watchdog is a no-op.
 */
#include "crc.h"
#include "coldfire.h"

void setup_watchdog(void)
{
    /* No-op: watchdog disabled in emulator */
}

/* service_watchdog is provided by KeyInput_25.c in libfirmware.a */
