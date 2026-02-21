/*
 * hal_coldfire.c - ColdFire MCF5282 Register Emulation
 *
 * Provides the global 'cf' struct (mcf5282) and 'vector_base' in normal RAM
 * instead of memory-mapped hardware registers. Also provides flash_mirror.
 *
 * The firmware accesses hardware via cf.uarts[], cf.i2c, cf.qspi, cf.pit[],
 * cf.gpio, etc. In the emulator, these are just regular struct fields that
 * the HAL modules read/write to simulate hardware behavior.
 */
#include "compat.h"
#include "crc.h"
#include "coldfire.h"
#include <string.h>

/* Allocate the ColdFire peripheral register struct in normal RAM.
 * In the real firmware, this is placed at MCF5282_BASE (0x40000000)
 * via a GHS linker pragma. Here it's just a global. */
volatile mcf5282 cf;

/* Interrupt vector table - used by firmware for exception handlers.
 * In the emulator, we don't actually use vectors, but the firmware
 * writes to this during initialization. */
vectors vector_base;

/* Flash mirror - declared extern in coldfire.h.
 * The firmware only references it in coldfire.h declaration.
 * Provide a minimal allocation. */
vudword flash_mirror[0x80000]; /* Match coldfire.h declaration */

/*
 * hal_coldfire_init - Initialize the emulated ColdFire registers
 *
 * Sets registers to safe default values that won't cause firmware
 * code to hang or behave incorrectly.
 */
void hal_coldfire_init(void)
{
    /* Zero out all registers */
    memset((void *)&cf, 0, sizeof(mcf5282));
    memset((void *)&vector_base, 0, sizeof(vectors));

    /* UART status registers: mark TX as always ready */
    cf.uarts[0].usr = 0x0C;  /* TXRDY | TXEMP */
    cf.uarts[1].usr = 0x0C;  /* TXRDY | TXEMP */
    cf.uarts[2].usr = 0x0C;  /* TXRDY | TXEMP */

    /* I2C status: bus idle */
    cf.i2c.sr = 0x00;

    /* QSPI: transfer complete */
    cf.qspi.qir = 0x0001; /* SPIF set */

    /* PIT: not running initially */
    cf.pit[0].pcsr = 0;
    cf.pit[1].pcsr = 0;
    cf.pit[2].pcsr = 0;
    cf.pit[3].pcsr = 0;

    /* Watchdog: not enabled */
    cf.wtm.wcr = 0;

    /* CCM: chip identification */
    cf.ccm.cir = 0x5282;

    /* Clock: PLL locked */
    cf.clock.synsr = 0x08; /* LOCK bit set */
}
