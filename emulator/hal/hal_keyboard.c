/*
 * hal_keyboard.c - Physical Keyboard Stub for CRC-25R Emulator
 *
 * Replaces Keyboard.c. The physical keyboard on the CRC-25R has:
 * - Nuclide hot keys
 * - Navigation keys (Home, Back, Print)
 * - Number keys
 * In the emulator, all input comes from the Amulet touchscreen
 * via WebSocket, so the physical keyboard is not used.
 */
#include "crc.h"
#include "coldfire.h"

/* Key codes */
#define NO_KEY 0

/* Keyboard state */
static short last_key = NO_KEY;
static bool key_pressed_flag = FALSE;

void init_keyboard(void)
{
    last_key = NO_KEY;
    key_pressed_flag = FALSE;
}

/* scan_keyboard is provided by KeyInput_25.c in libfirmware.a */

short get_key(void)
{
    return last_key;
}

bool key_pressed(void)
{
    return key_pressed_flag;
}

void clear_key(void)
{
    last_key = NO_KEY;
    key_pressed_flag = FALSE;
}

/* Called from timed_interrupt in real firmware */
void service_keyboard(void)
{
    /* No-op */
}
