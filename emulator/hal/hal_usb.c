/*
 * hal_usb.c - USB Host Controller Stubs for CRC-25R Emulator
 *
 * Replaces USB/sl811h_101508.c, USB/sl811s.c, and the USB-related
 * portions of PrinterSubs_25.c. All functions are stubs since USB
 * host functionality (printer, firmware update) is not emulated.
 *
 * The firmware uses the Cypress SL811HS USB host controller to:
 *   - Connect to and print via USB printers
 *   - Transfer firmware images to the Amulet touchscreen
 *   - Detect USB device attachment/detachment
 *
 * In the emulator, printer output is captured via the UART HAL
 * (hal_uart.c) and USB devices are not simulated.
 */
#include "compat.h"
#include "crc.h"
#include "coldfire.h"
#include <stdio.h>

/* ================================================================
 * SL811HS USB Host Controller Functions
 * ================================================================ */

/*
 * initSetUSB - Initialize the USB subsystem
 *
 * In the real firmware, this is a static function in CalibratorMain_R.c
 * that calls InitializeSL811H(). In the emulator, it is a no-op.
 */
void initSetUSB(void)
{
    /* No-op: no USB hardware */
}

/* InitializeSL811H is provided by sl811h_101508_with_USB_DRIVE.c in libfirmware.a */

/*
 * ReinitializeSL811H - Re-initialize the SL811HS after a bus error
 */
void ReinitializeSL811H(void)
{
    /* No-op: no USB hardware */
}

/*
 * startup_811 - Start the SL811HS USB host controller
 *
 * Called from the firmware to power on and initialize the USB bus.
 * This is declared in SL811.h and called from various places.
 */
void startup_811(void)
{
    /* No-op: no USB hardware */
}

/* ================================================================
 * USB Printer Functions (from sl811h.h)
 * ================================================================ */

/* IsUSBPrinterConnected is provided by sl811h_101508_with_USB_DRIVE.c in libfirmware.a */

/*
 * IsUSBPrinterError - Check if the USB printer has an error
 *
 * Returns FALSE (no error) since there is no printer.
 */
bool IsUSBPrinterError(void)
{
    return FALSE;
}

/*
 * IsUSBPrinterPaperEmpty - Check if the USB printer is out of paper
 *
 * Returns FALSE since there is no printer.
 */
bool IsUSBPrinterPaperEmpty(void)
{
    return FALSE;
}

/*
 * IsUSBPrinterSelected - Check if the USB printer is selected/online
 *
 * Returns FALSE since there is no printer.
 */
bool IsUSBPrinterSelected(void)
{
    return FALSE;
}

/* usb_write, usb_write_num are provided by sl811h_101508_with_USB_DRIVE.c in libfirmware.a */
/* printer_init is provided by PrinterSubs_25.c in libfirmware.a */

/* ================================================================
 * USB Service / Interrupt Functions
 * ================================================================ */

/*
 * usb_service - Service the USB host controller
 *
 * Called periodically from the timed interrupt to handle USB
 * device enumeration and data transfers. No-op in emulator.
 */
void usb_service(void)
{
    /* No-op: no USB hardware */
}

/* ================================================================
 * Exception Handler
 * ================================================================ */

/*
 * handle_ex - ColdFire exception handler
 *
 * In the real firmware, this catches hardware exceptions and
 * attempts recovery. In the emulator, it prints a warning
 * and continues execution.
 */
void handle_ex(void)
{
    fprintf(stderr, "[EMU] handle_ex: ColdFire exception handler invoked (ignored in emulator)\n");
}
