/*
 * amulet_bridge.h - Amulet Protocol Bridge for CRC-25R Emulator
 *
 * Intercepts the Amulet UART protocol on channel 2 and bridges it to a
 * WebSocket connection for the web-based frontend.
 *
 * The Amulet display protocol uses these command bytes:
 *   0xD0: Get Byte from display
 *   0xD2: Get String from display
 *   0xD5: Set Byte on display (firmware -> display)
 *   0xD6: Set Word on display (firmware -> display)
 *   0xD7: Set String on display (firmware -> display)
 *   0xA0 0x02: Navigate to page (SetAmuletHTML)
 *   0xD9: Draw line
 *   0xDA: Fill rectangle
 *
 * Data is hex-encoded (2 ASCII hex chars per byte value) in the TX stream,
 * except for HTML page navigation which uses binary encoding.
 * Packets are null-terminated.
 */
#ifndef AMULET_BRIDGE_H
#define AMULET_BRIDGE_H

#include "crc.h"

/* Initialize the bridge and start WebSocket server on the given port */
void amulet_bridge_init(int port);

/* Shut down the bridge and WebSocket server */
void amulet_bridge_shutdown(void);

/* Called by hal_uart.c when firmware sends a byte on UART2 TX.
 * encoded_byte: the protocol byte (hex-encoded char or binary for HTML cmds)
 * raw_flag: 0 for hex-encoded packets, 0xFF for binary HTML packets */
void amulet_bridge_process_tx(unsigned char encoded_byte, unsigned char raw_flag);

/* Called periodically to check for incoming WebSocket data and
 * inject user interactions into UART2 RX as Amulet protocol bytes */
void amulet_bridge_poll_rx(void);

/* Called by WebSocket server when a client connects; sends full
 * current state (all bytes, words, strings, current page) */
void amulet_bridge_send_full_state(void);

/* Inject the startup command the Amulet display would send on power-up */
void amulet_bridge_inject_startup(void);

/* State accessors for debugging */
unsigned char amulet_bridge_get_byte(int index);
unsigned short amulet_bridge_get_word(int index);
const char *amulet_bridge_get_string(int index);
unsigned char amulet_bridge_get_page(void);

#endif /* AMULET_BRIDGE_H */
