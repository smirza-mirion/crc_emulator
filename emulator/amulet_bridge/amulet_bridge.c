/*
 * amulet_bridge.c - Amulet Protocol Bridge Implementation
 *
 * Intercepts UART2 TX/RX traffic between the CRC-25R firmware and the
 * Amulet touchscreen display, converting between the Amulet UART protocol
 * and JSON messages sent over WebSocket to the web frontend.
 *
 * TX flow (firmware -> display -> WebSocket):
 *   1. Firmware calls SetAmuletByte/Word/String/HTML
 *   2. Those call SendUart2Tx() which calls PushUart2Tx() for each byte
 *   3. PushUart2Tx() calls amulet_bridge_process_tx() for each byte
 *   4. Bridge collects bytes into a packet buffer
 *   5. On null terminator (0x00), parse the complete packet
 *   6. Convert to JSON and broadcast via WebSocket
 *
 * RX flow (WebSocket -> display -> firmware):
 *   1. Web frontend sends JSON via WebSocket (button press, string input, etc.)
 *   2. ws_server.c receives it and calls our message callback
 *   3. Bridge parses JSON and constructs an Amulet protocol packet
 *   4. Packet bytes are pushed into UART2 RX via PushUart2Rx()
 *   5. Firmware's amulet_interrupt() reads them via service_amulet()
 *
 * Amulet protocol packet format:
 *   Regular commands: cmd_byte + hex_data + 0x00
 *     0xD5: SetByte  -> 0xD5 + hex(index,2) + hex(value,2) + 0x00  (5 bytes total)
 *     0xD6: SetWord  -> 0xD6 + hex(index,2) + hex(value,4) + 0x00  (7 bytes total)
 *     0xD7: SetString-> 0xD7 + hex(index,2) + string_data  + 0x00  (variable)
 *     0xD0: GetByte  -> 0xD0 + hex(index,2) + 0x00                 (3 bytes total)
 *     0xD2: GetString-> 0xD2 + hex(index,2) + 0x00                 (3 bytes total)
 *   HTML navigation: 0xA0 + 0x02 + page_hi + page_lo + checksum + 0x00  (binary)
 *
 * The hex encoding uses uppercase ASCII: 0xA3 -> "A3" (two chars 'A','3')
 */
#include "amulet_bridge.h"
#include "compat.h"
#include "crc.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ---- Forward declarations for ws_server.c ---- */
extern void ws_broadcast(const char *message, int len);
extern void ws_server_init(int port);
extern void ws_server_poll(void);
extern void ws_server_shutdown(void);

typedef void (*ws_message_callback)(const char *message, int len);
extern void ws_set_message_callback(ws_message_callback cb);

/* ---- Forward declarations for firmware message queue ---- */
/* PushMasterMessage() in Amulet.c: directly pushes a complete packet
 * into the master message queue that service_amulet() reads from.
 * This bypasses the UART2 RX interrupt handler framing logic. */
extern char PushMasterMessage(char *pcMessage);

/* ---- AmuletHTMLIndex for reverse page lookup ---- */
/* When the firmware calls SetAmuletHTML(page), it sends the board-specific
 * Amulet page index (e.g. 0x37 for Daily on board 1). The frontend uses
 * HTM enum indices (e.g. DAILY_HTM=0x10). We reverse-translate here. */
#define AMULET_HTML_ARRAY_SIZE 0xA8
extern unsigned char AmuletHTMLIndex[AMULET_HTML_ARRAY_SIZE];

/* ---- Amulet Display State ---- */
/* These mirror what the firmware has sent to the display */
static unsigned char  amulet_bytes[256];
static unsigned short amulet_words[256];
static char           amulet_strings[256][256];
static unsigned char  current_page;

/* ---- TX Packet Buffer ---- */
/* Collects bytes from PushUart2Tx until a complete packet (null terminator) */
#define TX_PACKET_MAX 512
static unsigned char tx_packet[TX_PACKET_MAX];
static int           tx_packet_len;
static int           tx_is_binary;  /* 1 if current packet is binary (HTML cmd) */

/* ---- JSON output buffer ---- */
#define JSON_MAX 1024
static char json_buf[JSON_MAX];

/* ---- Thread safety ---- */
static EMU_MUTEX bridge_mutex;

/* ---- Hex conversion helpers ---- */

/*
 * Convert a single hex character to its nibble value (0-15).
 * Returns 0 for invalid characters.
 */
static unsigned char hex_nibble(char c)
{
    if (c >= '0' && c <= '9') return (unsigned char)(c - '0');
    if (c >= 'A' && c <= 'F') return (unsigned char)(c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (unsigned char)(c - 'a' + 10);
    return 0;
}

/*
 * Convert a pair of hex ASCII characters to a byte value.
 * hex_to_byte('A', '3') -> 0xA3
 */
static unsigned char hex_to_byte(char hi, char lo)
{
    return (unsigned char)((hex_nibble(hi) << 4) | hex_nibble(lo));
}

/*
 * Convert a nibble (0-15) to an uppercase hex character.
 */
static char nibble_to_hex(unsigned char n)
{
    n &= 0x0F;
    if (n < 10) return (char)('0' + n);
    return (char)('A' + n - 10);
}

/*
 * Encode a byte as two uppercase hex characters into dst[0] and dst[1].
 * Does NOT null-terminate.
 */
static void byte_to_hex(unsigned char val, char *dst)
{
    dst[0] = nibble_to_hex((val >> 4) & 0x0F);
    dst[1] = nibble_to_hex(val & 0x0F);
}

/*
 * Encode a 16-bit word as four uppercase hex characters into dst[0..3].
 * Big-endian order (high byte first), matching the Amulet itoh() convention.
 * Does NOT null-terminate.
 */
static void word_to_hex(unsigned short val, char *dst)
{
    byte_to_hex((unsigned char)((val >> 8) & 0xFF), dst);
    byte_to_hex((unsigned char)(val & 0xFF), dst + 2);
}

/* ---- JSON Escape Helper ---- */

/*
 * Write a JSON-safe version of src into dst, escaping special characters.
 * Returns the number of characters written (excluding null terminator).
 * dst must be large enough to hold the escaped string.
 */
static int json_escape_string(char *dst, int dst_size, const char *src)
{
    int i = 0;
    while (*src && i < dst_size - 2) {
        char c = *src++;
        switch (c) {
            case '"':  if (i + 2 < dst_size) { dst[i++] = '\\'; dst[i++] = '"'; }  break;
            case '\\': if (i + 2 < dst_size) { dst[i++] = '\\'; dst[i++] = '\\'; } break;
            case '\n': if (i + 2 < dst_size) { dst[i++] = '\\'; dst[i++] = 'n'; }  break;
            case '\r': if (i + 2 < dst_size) { dst[i++] = '\\'; dst[i++] = 'r'; }  break;
            case '\t': if (i + 2 < dst_size) { dst[i++] = '\\'; dst[i++] = 't'; }  break;
            default:
                if ((unsigned char)c < 0x20) {
                    /* Control character: encode as \u00XX */
                    if (i + 6 < dst_size) {
                        i += snprintf(dst + i, dst_size - i, "\\u%04X", (unsigned char)c);
                    }
                } else {
                    dst[i++] = c;
                }
                break;
        }
    }
    dst[i] = '\0';
    return i;
}

/* ---- TX Packet Handlers ---- */

/*
 * Handle a SetByte command (0xD5).
 * Packet format: 0xD5 + hex(index,2) + hex(value,2) + 0x00
 * Total encoded length = 5 bytes (cmd + 4 hex chars).
 */
static void handle_set_byte(const unsigned char *pkt, int len)
{
    unsigned char index, value;

    if (len < 5) return;  /* Malformed */

    index = hex_to_byte((char)pkt[1], (char)pkt[2]);
    value = hex_to_byte((char)pkt[3], (char)pkt[4]);

    amulet_bytes[index] = value;

    snprintf(json_buf, JSON_MAX,
             "{\"type\":\"setByte\",\"index\":%d,\"value\":%d}",
             (int)index, (int)value);
    ws_broadcast(json_buf, (int)strlen(json_buf));
}

/*
 * Handle a SetWord command (0xD6).
 * Packet format: 0xD6 + hex(index,2) + hex(value,4) + 0x00
 * Total encoded length = 7 bytes (cmd + 6 hex chars).
 */
static void handle_set_word(const unsigned char *pkt, int len)
{
    unsigned char index;
    unsigned short value;

    if (len < 7) return;  /* Malformed */

    index = hex_to_byte((char)pkt[1], (char)pkt[2]);
    /* Word is big-endian: high byte first */
    value = (unsigned short)(hex_to_byte((char)pkt[3], (char)pkt[4]) << 8)
          | (unsigned short)hex_to_byte((char)pkt[5], (char)pkt[6]);

    amulet_words[index] = value;

    snprintf(json_buf, JSON_MAX,
             "{\"type\":\"setWord\",\"index\":%d,\"value\":%d}",
             (int)index, (int)value);
    ws_broadcast(json_buf, (int)strlen(json_buf));
}

/*
 * Handle a SetString command (0xD7).
 * Packet format: 0xD7 + hex(index,2) + string_data + 0x00
 * The string_data is NOT hex-encoded; it's the raw string characters.
 * Special escape sequences in the firmware:
 *   '$' is replaced with 0x75 ('u')
 *   0x02 + 0x80 = special marker (ctrl-C in source)
 *   0x02 + 0x04 = special marker (ctrl-D in source)
 *   0x02 + 0x01 = special marker (ctrl-E in source)
 */
static void handle_set_string(const unsigned char *pkt, int len)
{
    unsigned char index;
    char escaped[512];
    int str_len;

    if (len < 3) return;  /* Malformed: need at least cmd + 2 hex chars */

    index = hex_to_byte((char)pkt[1], (char)pkt[2]);

    /* String data starts at offset 3, up to the null terminator.
     * Copy it into amulet_strings[index]. */
    str_len = len - 3;
    if (str_len > 255) str_len = 255;
    memcpy(amulet_strings[index], &pkt[3], str_len);
    amulet_strings[index][str_len] = '\0';

    /* Build JSON with escaped string */
    json_escape_string(escaped, sizeof(escaped), amulet_strings[index]);
    snprintf(json_buf, JSON_MAX,
             "{\"type\":\"setString\",\"index\":%d,\"value\":\"%s\"}",
             (int)index, escaped);
    ws_broadcast(json_buf, (int)strlen(json_buf));
}

/*
 * Handle a GetByte command (0xD0).
 * The display would normally respond; we just log it.
 * Packet format: 0xD0 + hex(index,2) + 0x00
 */
static void handle_get_byte(const unsigned char *pkt, int len)
{
    unsigned char index;

    if (len < 3) return;

    index = hex_to_byte((char)pkt[1], (char)pkt[2]);

    snprintf(json_buf, JSON_MAX,
             "{\"type\":\"getByte\",\"index\":%d}",
             (int)index);
    ws_broadcast(json_buf, (int)strlen(json_buf));
}

/*
 * Handle a GetString command (0xD2).
 * The display would normally respond; we just log it.
 * Packet format: 0xD2 + hex(index,2) + 0x00
 */
static void handle_get_string(const unsigned char *pkt, int len)
{
    unsigned char index;

    if (len < 3) return;

    index = hex_to_byte((char)pkt[1], (char)pkt[2]);

    snprintf(json_buf, JSON_MAX,
             "{\"type\":\"getString\",\"index\":%d}",
             (int)index);
    ws_broadcast(json_buf, (int)strlen(json_buf));
}

/*
 * Reverse-translate a board-specific Amulet page index to the HTM enum index.
 * The firmware's SetAmuletHTML() sends board-specific indices (e.g., 0x37 for
 * Daily on board 1), but the frontend uses HTM enum indices (e.g., 0x10 for
 * DAILY_HTM). Scan the AmuletHTMLIndex[] array to find the matching enum.
 * Returns the Amulet page index unchanged if no reverse mapping found.
 */
static int reverse_page_lookup(unsigned char amulet_page)
{
    int i;
    for (i = 0; i < AMULET_HTML_ARRAY_SIZE; i++) {
        if (AmuletHTMLIndex[i] == amulet_page) {
            return i;
        }
    }
    return (int)amulet_page;  /* Fallback: return as-is */
}

/*
 * Handle a SetAmuletHTML page navigation command.
 * Binary packet format: 0xA0 + 0x02 + page_hi + page_lo + checksum + 0x00
 * The page number is (page_hi << 8) | page_lo.
 *
 * The firmware sends board-specific page indices (from AmuletHTMLIndex[]),
 * but the frontend expects HTM enum indices. We reverse-translate here.
 */
static void handle_set_page(const unsigned char *pkt, int len)
{
    unsigned short raw_page;
    int htm_page;

    if (len < 5) return;  /* Need at least A0 02 hi lo chk */

    raw_page = (unsigned short)((pkt[2] << 8) | pkt[3]);
    htm_page = reverse_page_lookup((unsigned char)(raw_page & 0xFF));
    current_page = (unsigned char)htm_page;

    snprintf(json_buf, JSON_MAX,
             "{\"type\":\"setPage\",\"page\":%d}",
             htm_page);
    ws_broadcast(json_buf, (int)strlen(json_buf));
}

/*
 * Handle a SetAmuletLine draw command (0xD9).
 * Packet format: 0xD9 + hex(x1,4) + hex(y1,4) + hex(x2,4) + hex(y2,4)
 *              + hex(blue,2) + hex(green,2) + hex(red,2) + hex(weight,2) + 0x00
 */
static void handle_draw_line(const unsigned char *pkt, int len)
{
    unsigned short x1, y1, x2, y2;
    unsigned char red, green, blue, weight;

    if (len < 25) return;  /* 1 cmd + 24 hex chars */

    x1 = (unsigned short)(hex_to_byte((char)pkt[1],  (char)pkt[2])  << 8)
       | (unsigned short)hex_to_byte((char)pkt[3],  (char)pkt[4]);
    y1 = (unsigned short)(hex_to_byte((char)pkt[5],  (char)pkt[6])  << 8)
       | (unsigned short)hex_to_byte((char)pkt[7],  (char)pkt[8]);
    x2 = (unsigned short)(hex_to_byte((char)pkt[9],  (char)pkt[10]) << 8)
       | (unsigned short)hex_to_byte((char)pkt[11], (char)pkt[12]);
    y2 = (unsigned short)(hex_to_byte((char)pkt[13], (char)pkt[14]) << 8)
       | (unsigned short)hex_to_byte((char)pkt[15], (char)pkt[16]);

    blue   = hex_to_byte((char)pkt[17], (char)pkt[18]);
    green  = hex_to_byte((char)pkt[19], (char)pkt[20]);
    red    = hex_to_byte((char)pkt[21], (char)pkt[22]);
    weight = hex_to_byte((char)pkt[23], (char)pkt[24]);

    snprintf(json_buf, JSON_MAX,
             "{\"type\":\"drawLine\",\"x1\":%d,\"y1\":%d,\"x2\":%d,\"y2\":%d,"
             "\"r\":%d,\"g\":%d,\"b\":%d,\"weight\":%d}",
             (int)x1, (int)y1, (int)x2, (int)y2,
             (int)red, (int)green, (int)blue, (int)weight);
    ws_broadcast(json_buf, (int)strlen(json_buf));
}

/*
 * Handle a SetAmuletFillRect command (0xDA).
 * Packet format: 0xDA + hex(x,4) + hex(y,4) + hex(dx,4) + hex(dy,4)
 *              + hex(blue,2) + hex(green,2) + hex(red,2) + hex(weight,2) + 0x00
 */
static void handle_fill_rect(const unsigned char *pkt, int len)
{
    unsigned short x, y, dx, dy;
    unsigned char red, green, blue, weight;

    if (len < 25) return;

    x  = (unsigned short)(hex_to_byte((char)pkt[1],  (char)pkt[2])  << 8)
       | (unsigned short)hex_to_byte((char)pkt[3],  (char)pkt[4]);
    y  = (unsigned short)(hex_to_byte((char)pkt[5],  (char)pkt[6])  << 8)
       | (unsigned short)hex_to_byte((char)pkt[7],  (char)pkt[8]);
    dx = (unsigned short)(hex_to_byte((char)pkt[9],  (char)pkt[10]) << 8)
       | (unsigned short)hex_to_byte((char)pkt[11], (char)pkt[12]);
    dy = (unsigned short)(hex_to_byte((char)pkt[13], (char)pkt[14]) << 8)
       | (unsigned short)hex_to_byte((char)pkt[15], (char)pkt[16]);

    blue   = hex_to_byte((char)pkt[17], (char)pkt[18]);
    green  = hex_to_byte((char)pkt[19], (char)pkt[20]);
    red    = hex_to_byte((char)pkt[21], (char)pkt[22]);
    weight = hex_to_byte((char)pkt[23], (char)pkt[24]);

    snprintf(json_buf, JSON_MAX,
             "{\"type\":\"fillRect\",\"x\":%d,\"y\":%d,\"dx\":%d,\"dy\":%d,"
             "\"r\":%d,\"g\":%d,\"b\":%d,\"weight\":%d}",
             (int)x, (int)y, (int)dx, (int)dy,
             (int)red, (int)green, (int)blue, (int)weight);
    ws_broadcast(json_buf, (int)strlen(json_buf));
}

/*
 * Handle a complete TX packet from the firmware.
 * Dispatches to the appropriate handler based on the command byte.
 */
static void handle_tx_packet(const unsigned char *pkt, int len)
{
    unsigned char cmd;

    if (len < 1) return;
    cmd = pkt[0];

    {
        static int tx_dbg_count = 0;
        if (tx_dbg_count < 50) {
            printf("[bridge_tx] cmd=0x%02X len=%d\n", (int)cmd, len);
            tx_dbg_count++;
        }
    }

    /* Check for binary HTML navigation packet: 0xA0 0x02 ... */
    if (cmd == 0xA0 && len >= 2 && pkt[1] == 0x02) {
        handle_set_page(pkt, len);
        return;
    }

    switch (cmd) {
        case 0xD5:  /* SetByte */
            handle_set_byte(pkt, len);
            break;

        case 0xD6:  /* SetWord */
            handle_set_word(pkt, len);
            break;

        case 0xD7:  /* SetString */
            handle_set_string(pkt, len);
            break;

        case 0xD0:  /* GetByte (display request) */
            handle_get_byte(pkt, len);
            break;

        case 0xD2:  /* GetString (display request) */
            handle_get_string(pkt, len);
            break;

        case 0xD9:  /* DrawLine */
            handle_draw_line(pkt, len);
            break;

        case 0xDA:  /* FillRect */
            handle_fill_rect(pkt, len);
            break;

        case 0xE0:  /* GetByte response (firmware -> display) */
        case 0xE2:  /* GetString response (firmware -> display) */
        case 0xE5:  /* SetByte ACK (firmware -> display) */
        case 0xE6:  /* SetWord ACK (firmware -> display) */
            /* ACK/response packets: log but don't forward to frontend */
            break;

        default:
            /* Unknown command byte - log for debugging */
            printf("[bridge_tx] Unknown TX cmd: 0x%02X (len=%d)\n",
                   (int)cmd, len);
            break;
    }
}

/* ---- RX Injection (WebSocket -> Firmware) ---- */

/*
 * Mapping from SET_HTML/SET_CLEAR values to HTM enum indices.
 * When byte(3) is set via a button press, the Amulet display changes pages.
 * This table lets the bridge track the current page for fullState responses.
 * Index: SET_HTML value (1-71), Value: HTM enum index.
 * Derived from the CallHTML() switch in Amulet.c.
 */
static const unsigned char set_html_to_htm[72] = {
    0xFF,       /* 0: unused (main screen is at m_iMenu=0) */
    0x14,       /* 1: SET_HTML_ACCURACY    → ACCURACY_HTM */
    0x11,       /* 2: SET_HTML_BACKGROUND  → BACKGROUND_HTM */
    0x12,       /* 3: SET_HTML_CHAMBERVOLTS → CHAMBERVOLTS_HTM */
    0x10,       /* 4: SET_HTML_DAILY       → DAILY_HTM */
    0x13,       /* 5: SET_HTML_GEOMETRY    → GEOMETRY_HTM */
    0x0A,       /* 6: SET_HTML_GEOMETRYREPORT → GEOMETRYREPORT_HTM */
    0x0E,       /* 7: SET_HTML_HMPAO       → HMPAO_HTM */
    0x00,       /* 8: SET_HTML_INDEX       → INDEX_HTM */
    0x18,       /* 9: SET_HTML_INVENTORY   → INVENTORY_HTM */
    0x07,       /* 10: SET_HTML_INVENTORYADD → INVENTORYADD_HTM */
    0x08,       /* 11: SET_HTML_INVENTORYWITHDRAW → INVENTORYWITHDRAW_HTM */
    0x03,       /* 12: SET_HTML_KEYBOARD2  → KEYBOARD2_HTM */
    0x02,       /* 13: SET_HTML_KEYPAD2    → KEYPAD2_HTM */
    0x15,       /* 14: SET_HTML_LINEARITY  → LINEARITY_HTM */
    0x0F,       /* 15: SET_HTML_MAG3       → MAG3_HTM */
    0x09,       /* 16: SET_HTML_MAINSCREEN → MAINSCREEN_HTM */
    0x06,       /* 17: SET_HTML_MEASUREACTIVITY → MEASUREACTIVITY_HTM */
    0x17,       /* 18: SET_HTML_MOLY       → MOLY_HTM */
    0x01,       /* 19: SET_HTML_NUCLIDE2   → NUCLIDE2_HTM */
    0x0C,       /* 20: SET_HTML_ONESTRIP   → ONESTRIP_HTM */
    0x16,       /* 21: SET_HTML_QC         → QC_HTM */
    0x05,       /* 22: SET_HTML_SETACTIVITY → SETACTIVITY_HTM */
    0x0B,       /* 23: SET_HTML_SETSTUDY   → SETSTUDY_HTM */
    0x04,       /* 24: SET_HTML_SETTIME2   → SETTIME2_HTM */
    0x19,       /* 25: SET_HTML_SETUP      → SETUP_HTM */
    0x0D,       /* 26: SET_HTML_TWOSTRIP   → TWOSTRIP_HTM */
    0x1A,       /* 27: SET_HTML_INVENTORYDELETE → INVENTORYDELETE_HTM */
    0x1B,       /* 28: SET_HTML_INVENTORYKIT → INVENTORYKIT_HTM */
    0x1C,       /* 29: SET_HTML_SETSTUDY2  → SETSTUDY2_HTM */
    0x1D,       /* 30: SET_HTML_ENHANCED   → ENHANCED_HTM */
    0x1E,       /* 31: SET_HTML_INFO       → INFO_HTM */
    0x1F,       /* 32: SET_HTML_PSETUP     → OTHER_HTM */
    0x20,       /* 33: SET_HTML_SETUPSOURCES → SETUPSOURCES_HTM */
    0x21,       /* 34: SET_HTML_SETUPMOLY  → SETUPMOLY_HTM */
    0x22,       /* 35: SET_HTML_SETUPNUCLIDE → SETUPNUCLIDE_HTM */
    0x23,       /* 36: SET_HTML_SETUPLINEARITY → SETUPLINEARITY_HTM */
    0x24,       /* 37: SET_HTML_SETUPREMOTE → SETUPREMOTE_HTM */
    0x25,       /* 38: SET_HTML_SETUPCALNUM → SETUPCALNUM_HTM */
    0x26,       /* 39: SET_HTML_ERRORMSG   → ERRORMSG_HTM */
    0x27,       /* 40: SET_HTML_WARNINGMSG → WARNINGMSG_HTM */
    0x28,       /* 41: SET_HTML_NOTIFICATIONMSG → NOTIFICATIONMSG_HTM */
    0x29,       /* 42: SET_HTML_SETUPLINEARITYSTANDARD */
    0x2A,       /* 43: SET_HTML_SETUPLINEARITYLINEATOR */
    0x2B,       /* 44: SET_HTML_SETUPLINEARITYCALICHECK */
    0x2C,       /* 45: SET_HTML_DOSETABLE  → DOSETABLE_HTM */
    0x2D,       /* 46: SET_HTML_SELECT_CHAMBER → CHAMBER_HTM */
    0x2E,       /* 47: SET_HTML_GRADIENT   → GRADIENT_HTM */
    0x2F,       /* 48: SET_HTML_WELLMAINSCREEN → WELLMAINSCREEN_HTM */
    0x30,       /* 49: SET_HTML_BETAMAINSCREEN → BETAMAINSCREEN_HTM */
    0x31,       /* 50: SET_HTML_WELLMEASUREMENT → WELLMEASUREMENT_HTM */
    0x32,       /* 51: SET_HTML_WELLAUTOCALIBRATE */
    0x33,       /* 52: SET_HTML_WELLPEAKS  → WELLPEAKS_HTM */
    0x34,       /* 53: SET_HTML_WELLADVANCEDSETUP */
    0x35,       /* 54: SET_HTML_WELLSETUPNUCLIDE */
    0x37,       /* 55: SET_HTML_WELLSETUPSEALED2 */
    0x37,       /* 56: SET_HTML_WELLSETUPSEALED2 */
    0x38,       /* 57: SET_HTML_WELLSETUPTESTSOURCE */
    0x39,       /* 58: SET_HTML_WELLSETUPTRIGGERLEVEL */
    0x3A,       /* 59: SET_HTML_WELLSETUPEFFICIENCIES */
    0x3B,       /* 60: SET_HTML_WELLEDITEFFICIENCIES */
    0x3C,       /* 61: SET_HTML_WELLMANUAL → WELLMANUAL_HTM */
    0x3D,       /* 62: SET_HTML_WELLMEASUREEFFICIENCY */
    0x3E,       /* 63: SET_HTML_AUTOCONSTANCY */
    0x3F,       /* 64: SET_HTML_SLEEP      → SLEEP_HTM */
    0x40,       /* 65: SET_HTML_FACTORY    → FACTORY_HTM */
    0x41,       /* 66: SET_HTML_SETUPCHAMBER */
    0x42,       /* 67: SET_HTML_INVENTORYDELETEALL */
    0x43,       /* 68: SET_HTML_WELLMDATEST */
    0x44,       /* 69: SET_HTML_WELLMEASUREMENTS */
    0x45,       /* 70: SET_HTML_WELLEDITFULLEFFICIENCY */
    0x46,       /* 71: SET_HTML_SETUPCALIBSERIAL */
};

/*
 * Inject a byte-changed event directly into the firmware's master message queue.
 * Packet format: 0xD5 + hex(index,2) + hex(value,2) + '\0'
 * Bypasses UART2 RX and the interrupt handler framing logic.
 *
 * When byte index 3 is set (navigation command), also update current_page
 * and broadcast a setPage message so the frontend can track page changes
 * without relying on the firmware to call SetAmuletHTML().
 */
static void inject_byte_event(unsigned char index, unsigned char value)
{
    char pkt[6];

    pkt[0] = (char)0xD5;
    byte_to_hex(index, &pkt[1]);
    byte_to_hex(value, &pkt[3]);
    pkt[5] = '\0';

    /* Track page changes when byte(3) is set */
    if (index == 3 && value > 0 && value < 72) {
        unsigned char htm_page = set_html_to_htm[value];
        if (htm_page != 0xFF) {
            current_page = htm_page;
            snprintf(json_buf, JSON_MAX,
                     "{\"type\":\"setPage\",\"page\":%d}",
                     (int)htm_page);
            ws_broadcast(json_buf, (int)strlen(json_buf));
        }
    } else if (index == 3 && value == 0) {
        /* value 0 = return to measurement screen (MainScreen) */
        current_page = 0x09; /* MAINSCREEN_HTM */
        snprintf(json_buf, JSON_MAX,
                 "{\"type\":\"setPage\",\"page\":%d}",
                 (int)current_page);
        ws_broadcast(json_buf, (int)strlen(json_buf));
    }

    PushMasterMessage(pkt);
}

/*
 * Inject a word-changed event directly into the firmware's master message queue.
 * Packet format: 0xD6 + hex(index,2) + hex(value,4) + '\0'
 */
static void inject_word_event(unsigned char index, unsigned short value)
{
    char pkt[8];

    pkt[0] = (char)0xD6;
    byte_to_hex(index, &pkt[1]);
    word_to_hex(value, &pkt[3]);
    pkt[7] = '\0';

    PushMasterMessage(pkt);
}

/*
 * Inject a string-input event directly into the firmware's master message queue.
 * Packet format: 0xD7 + hex(index,2) + string_data + '\0'
 */
static void inject_string_event(unsigned char index, const char *str)
{
    char pkt[256];
    int slen;

    pkt[0] = (char)0xD7;
    byte_to_hex(index, &pkt[1]);

    /* Copy string data starting at offset 3 */
    slen = (int)strlen(str);
    if (slen > 250) slen = 250;
    memcpy(&pkt[3], str, slen);
    pkt[3 + slen] = '\0';

    PushMasterMessage(pkt);
}

/* ---- Simple JSON Parser Helpers ---- */

/*
 * Find a JSON string value for a given key in a JSON object string.
 * Returns pointer to the value (after the opening quote), or NULL.
 * Writes the value (without quotes) into out_buf.
 */
static int json_get_string(const char *json, const char *key, char *out_buf, int out_size)
{
    char search[64];
    const char *p;
    int i;

    snprintf(search, sizeof(search), "\"%s\":", key);
    p = strstr(json, search);
    if (!p) return 0;

    p += strlen(search);
    /* Skip whitespace */
    while (*p == ' ' || *p == '\t') p++;

    if (*p != '"') return 0;
    p++;  /* Skip opening quote */

    i = 0;
    while (*p && *p != '"' && i < out_size - 1) {
        if (*p == '\\' && *(p + 1)) {
            p++;  /* Skip backslash, take next char */
            switch (*p) {
                case 'n':  out_buf[i++] = '\n'; break;
                case 'r':  out_buf[i++] = '\r'; break;
                case 't':  out_buf[i++] = '\t'; break;
                case '"':  out_buf[i++] = '"';  break;
                case '\\': out_buf[i++] = '\\'; break;
                default:   out_buf[i++] = *p;   break;
            }
        } else {
            out_buf[i++] = *p;
        }
        p++;
    }
    out_buf[i] = '\0';
    return 1;
}

/*
 * Find a JSON integer value for a given key.
 * Returns 1 if found, 0 otherwise.
 */
static int json_get_int(const char *json, const char *key, int *out_val)
{
    char search[64];
    const char *p;

    snprintf(search, sizeof(search), "\"%s\":", key);
    p = strstr(json, search);
    if (!p) return 0;

    p += strlen(search);
    /* Skip whitespace */
    while (*p == ' ' || *p == '\t') p++;

    *out_val = atoi(p);
    return 1;
}

/*
 * WebSocket message callback - called by ws_server.c when data arrives
 * from a connected client.
 *
 * Expected JSON messages from the frontend:
 *   {"type":"buttonPress","byteIndex":N,"value":V}
 *   {"type":"stringInput","stringIndex":N,"value":"..."}
 *   {"type":"wordChanged","wordIndex":N,"value":V}
 *   {"type":"requestState"}
 */
static void on_ws_message(const char *message, int len)
{
    char type_buf[32];
    int index_val, value_val;
    char str_val[256];

    (void)len;

    if (!json_get_string(message, "type", type_buf, sizeof(type_buf))) {
        return;  /* No type field */
    }

    EMU_MUTEX_LOCK(bridge_mutex);

    if (strcmp(type_buf, "buttonPress") == 0) {
        if (json_get_int(message, "byteIndex", &index_val) &&
            json_get_int(message, "value", &value_val)) {
            inject_byte_event((unsigned char)index_val, (unsigned char)value_val);
        }
    }
    else if (strcmp(type_buf, "stringInput") == 0) {
        if (json_get_int(message, "stringIndex", &index_val) &&
            json_get_string(message, "value", str_val, sizeof(str_val))) {
            inject_string_event((unsigned char)index_val, str_val);
        }
    }
    else if (strcmp(type_buf, "wordChanged") == 0) {
        if (json_get_int(message, "wordIndex", &index_val) &&
            json_get_int(message, "value", &value_val)) {
            inject_word_event((unsigned char)index_val, (unsigned short)value_val);
        }
    }
    else if (strcmp(type_buf, "requestState") == 0) {
        amulet_bridge_send_full_state();
    }

    EMU_MUTEX_UNLOCK(bridge_mutex);
}

/* ---- Public API ---- */

/*
 * Initialize the Amulet bridge and start the WebSocket server.
 */
void amulet_bridge_init(int port)
{
    /* Clear all state */
    memset(amulet_bytes, 0, sizeof(amulet_bytes));
    memset(amulet_words, 0, sizeof(amulet_words));
    memset(amulet_strings, 0, sizeof(amulet_strings));
    current_page = 0;

    /* Clear packet buffer */
    tx_packet_len = 0;
    tx_is_binary = 0;
    memset(tx_packet, 0, sizeof(tx_packet));

    EMU_MUTEX_INIT(bridge_mutex);

    /* Start WebSocket server and register our message callback */
    ws_server_init(port);
    ws_set_message_callback(on_ws_message);

    printf("[amulet_bridge] Initialized on port %d\n", port);
}

/*
 * Inject the startup command that the Amulet display would normally send.
 * On real hardware, the display sends a SetByte(3, 0) to tell the firmware
 * to enter the main measurement screen (MENU_NONE = 0).
 * Call this after firmware initialization is complete.
 */
void amulet_bridge_inject_startup(void)
{
    /* SetByte index=3 value=32: tells firmware to enter MENU_MAIN_SCREEN.
     * This triggers AmuletMainScreen_menu() which sends all the initial
     * display data (language, branding, nuclide buttons, activity, etc.).
     * On real hardware, the Amulet display sends this after loading
     * MainScreen.htm. MENU_MAIN_SCREEN = 32 from Headers/Amulet.h. */
    inject_byte_event(3, 32);

    /* Also notify the bridge state that we're on the main screen page */
    current_page = 0x09;  /* MAINSCREEN_HTM = 0x09 */

    printf("[amulet_bridge] Injected startup command (MENU_MAIN_SCREEN=32)\n");
}

/*
 * Shut down the bridge and WebSocket server.
 */
void amulet_bridge_shutdown(void)
{
    ws_server_shutdown();
    EMU_MUTEX_DESTROY(bridge_mutex);
    printf("[amulet_bridge] Shutdown complete\n");
}

/*
 * Process a single byte from the firmware's UART2 TX stream.
 *
 * Called by PushUart2Tx() in hal_uart.c for each byte the firmware sends.
 *
 * encoded_byte: the protocol byte (hex-encoded ASCII for regular commands,
 *               raw binary for HTML page navigation commands)
 * raw_flag: 0 for hex-encoded packets, 0xFF (255) for binary HTML packets
 *
 * Bytes are accumulated into tx_packet[] until a null terminator (0x00)
 * is received, at which point the complete packet is dispatched.
 */
void amulet_bridge_process_tx(unsigned char encoded_byte, unsigned char raw_flag)
{
    EMU_MUTEX_LOCK(bridge_mutex);

    /* Detect start of a new binary (HTML) packet */
    if (tx_packet_len == 0 && raw_flag == 0xFF) {
        tx_is_binary = 1;
    } else if (tx_packet_len == 0) {
        tx_is_binary = 0;
    }

    /* Null terminator signals end of packet */
    if (encoded_byte == 0x00) {
        if (tx_packet_len > 0) {
            handle_tx_packet(tx_packet, tx_packet_len);
        }
        tx_packet_len = 0;
        tx_is_binary = 0;
        EMU_MUTEX_UNLOCK(bridge_mutex);
        return;
    }

    /* Accumulate byte into packet buffer */
    if (tx_packet_len < TX_PACKET_MAX - 1) {
        tx_packet[tx_packet_len++] = encoded_byte;
    } else {
        /* Buffer overflow - discard packet */
        printf("[amulet_bridge] TX packet overflow, discarding\n");
        tx_packet_len = 0;
        tx_is_binary = 0;
    }

    EMU_MUTEX_UNLOCK(bridge_mutex);
}

/*
 * Poll for incoming WebSocket data and process it.
 * Called periodically from amulet_interrupt() in hal_uart.c.
 */
void amulet_bridge_poll_rx(void)
{
    ws_server_poll();
}

/*
 * Send the complete current display state to all connected WebSocket clients.
 * Called when a new client connects so it can render the current screen.
 */
void amulet_bridge_send_full_state(void)
{
    int i;
    char escaped[512];

    /* Send current page */
    snprintf(json_buf, JSON_MAX,
             "{\"type\":\"fullState\",\"page\":%d}",
             (int)current_page);
    ws_broadcast(json_buf, (int)strlen(json_buf));

    /* Send all non-zero bytes */
    for (i = 0; i < 256; i++) {
        if (amulet_bytes[i] != 0) {
            snprintf(json_buf, JSON_MAX,
                     "{\"type\":\"setByte\",\"index\":%d,\"value\":%d}",
                     i, (int)amulet_bytes[i]);
            ws_broadcast(json_buf, (int)strlen(json_buf));
        }
    }

    /* Send all non-zero words */
    for (i = 0; i < 256; i++) {
        if (amulet_words[i] != 0) {
            snprintf(json_buf, JSON_MAX,
                     "{\"type\":\"setWord\",\"index\":%d,\"value\":%d}",
                     i, (int)amulet_words[i]);
            ws_broadcast(json_buf, (int)strlen(json_buf));
        }
    }

    /* Send all non-empty strings */
    for (i = 0; i < 256; i++) {
        if (amulet_strings[i][0] != '\0') {
            json_escape_string(escaped, sizeof(escaped), amulet_strings[i]);
            snprintf(json_buf, JSON_MAX,
                     "{\"type\":\"setString\",\"index\":%d,\"value\":\"%s\"}",
                     i, escaped);
            ws_broadcast(json_buf, (int)strlen(json_buf));
        }
    }
}

/* ---- State Accessors ---- */

unsigned char amulet_bridge_get_byte(int index)
{
    if (index < 0 || index > 255) return 0;
    return amulet_bytes[index];
}

unsigned short amulet_bridge_get_word(int index)
{
    if (index < 0 || index > 255) return 0;
    return amulet_words[index];
}

const char *amulet_bridge_get_string(int index)
{
    if (index < 0 || index > 255) return "";
    return amulet_strings[index];
}

unsigned char amulet_bridge_get_page(void)
{
    return current_page;
}
