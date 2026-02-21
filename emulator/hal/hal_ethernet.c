/*
 * hal_ethernet.c - Ethernet Stub for CRC-25R Emulator
 *
 * Replaces Ethernet.c. The firmware has Ethernet support via the
 * ColdFire FEC (Fast Ethernet Controller). In the emulator, Ethernet
 * communication is not needed - the WebSocket bridge handles
 * all communication.
 */
#include "crc.h"
#include "coldfire.h"
#include <string.h>

/* Ethernet state */
static bool ethernet_enabled = FALSE;

void init_ethernet(void)
{
    ethernet_enabled = FALSE;
}

bool ethernet_connected(void)
{
    return FALSE;
}

void ethernet_service(void)
{
    /* No-op */
}

void ethernet_send(char *data, int len)
{
    (void)data;
    (void)len;
}

int ethernet_recv(char *data, int maxlen)
{
    (void)data;
    (void)maxlen;
    return 0;
}

/* ClearLowLevel is provided by LowLevelSnoop.c in libfirmware.a */

void LowLevel_setFlag(bool flag)
{
    (void)flag;
}

bool LowLevel_getFlag(void)
{
    return FALSE;
}
