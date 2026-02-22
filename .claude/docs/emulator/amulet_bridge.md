# Amulet Protocol Bridge

## Purpose

The Amulet Bridge intercepts the firmware's UART channel 2 traffic (Amulet display protocol) and translates it bidirectionally to JSON messages over WebSocket for the web frontend. It maintains a shadow copy of the entire display state and provides full-state synchronization for newly connected clients.

## Source Files

| File | Description |
|------|-------------|
| `emulator/amulet_bridge/amulet_bridge.h` | Public API declarations |
| `emulator/amulet_bridge/amulet_bridge.c` | Protocol parser, state manager, broadcast |
| `emulator/amulet_bridge/ws_server.c` | Embedded WebSocket server (BSD sockets) |

## Amulet Protocol

The Amulet display controller communicates with the MCF5282 via UART using a binary/hex-encoded protocol. Commands are null-terminated byte sequences.

### Command Bytes

| Command | Hex | Direction | Description |
|---------|-----|-----------|-------------|
| GetByte | 0xD0 | Display → Firmware | Request byte variable value |
| GetString | 0xD2 | Display → Firmware | Request string variable value |
| SetByte | 0xD5 | Firmware → Display | Set byte variable (0-255) |
| SetWord | 0xD6 | Firmware → Display | Set word variable (0-65535) |
| SetString | 0xD7 | Firmware → Display | Set string variable (text) |
| SetHTML | 0xA0 0x02 | Firmware → Display | Navigate to page |
| DrawLine | 0xD9 | Firmware → Display | Draw line on screen |
| FillRect | 0xDA | Firmware → Display | Fill rectangle on screen |

### Encoding

- **Hex-encoded packets (0xD5, 0xD6, 0xD7):** Data bytes are transmitted as 2 ASCII hex characters per value. Example: byte value 255 is sent as "FF".
- **Binary packets (0xA0 0x02):** Page index is sent as raw binary bytes.
- **Termination:** All packets end with a null byte (0x00).

## Bridge API

### Initialization

```c
void amulet_bridge_init(int port);    // Start WebSocket server on port
void amulet_bridge_shutdown(void);     // Stop server, clean up
```

### TX Processing (Firmware → Display)

```c
void amulet_bridge_process_tx(unsigned char encoded_byte, unsigned char raw_flag);
```

Called by `hal_uart.c:PushUart2Tx()` for each byte the firmware sends on UART2. The bridge accumulates bytes into a packet buffer. When a null terminator is received, `handle_tx_packet()` is called to parse and broadcast.

Parameters:
- `encoded_byte` — The protocol byte
- `raw_flag` — 0 for hex-encoded packets, 0xFF for binary HTML navigation packets

### RX Processing (Display → Firmware)

```c
void amulet_bridge_poll_rx(void);
```

Called periodically from the firmware's service loop. Checks for incoming WebSocket messages and injects user interactions into the UART2 RX buffer as properly-encoded Amulet protocol bytes.

### State Synchronization

```c
void amulet_bridge_send_full_state(void);
```

Called when a new WebSocket client connects. Sends the complete current state:

```json
{
  "type": "fullState",
  "bytes": {"0": 0, "3": 32, "20": 255, ...},
  "words": {"5": 1234, ...},
  "strings": {"3": "Tc-99m", "10": "15.00", ...},
  "page": 9
}
```

### Startup Injection

```c
void amulet_bridge_inject_startup(void);
```

Injects the startup command that the Amulet display would normally send on power-up. Sets byte(3) to `MENU_MAIN_SCREEN` (32), causing the firmware to enter the main measurement screen.

### State Accessors

```c
unsigned char  amulet_bridge_get_byte(int index);
unsigned short amulet_bridge_get_word(int index);
const char    *amulet_bridge_get_string(int index);
unsigned char  amulet_bridge_get_page(void);
```

## Shadow State

The bridge maintains a complete mirror of the Amulet display state:

```c
static unsigned char  state_bytes[256];    // Byte variables
static unsigned short state_words[256];    // Word variables
static char           state_strings[256][256]; // String variables
static unsigned char  state_page;          // Current page index
```

This state is:
1. Updated whenever the firmware sends a Set command (TX path)
2. Sent in full to newly connected clients (fullState message)
3. Accessible via debug accessors

## WebSocket Protocol

### Server → Client Messages

| Type | Fields | Description |
|------|--------|-------------|
| `setByte` | `index`, `value` | Byte variable changed |
| `setWord` | `index`, `value` | Word variable changed |
| `setString` | `index`, `value` | String variable changed |
| `setPage` | `page` | Page navigation occurred |
| `drawLine` | `x1`, `y1`, `x2`, `y2`, `color` | Draw line command |
| `fillRect` | `x`, `y`, `w`, `h`, `color` | Fill rectangle command |
| `fullState` | `bytes`, `words`, `strings`, `page` | Complete state sync |

### Client → Server Messages

| Type | Fields | Description |
|------|--------|-------------|
| `buttonPress` | `byteIndex`, `value` | Button pressed (setByte) |
| `stringInput` | `stringIndex`, `value` | Text input submitted |
| `wordChanged` | `wordIndex`, `value` | Word value changed (slider, etc.) |
| `loadScenario` | `scenario` | Load preset scenario (0-9) |
| `configSource` | `channel`, `enabled`, `nuclide`, `activity`, `unit` | Configure chamber source |

### Examples

**Button press (user clicks "Daily" button):**
```json
Client → Server: {"type": "buttonPress", "byteIndex": 189, "value": 1}
```
This gets encoded as Amulet protocol: `0xD5` + hex(189) + hex(1) + `0x00`

**Firmware updates activity display:**
```json
Server → Client: {"type": "setString", "index": 10, "value": "15.00"}
```

**Page navigation to ChamberVolts:**
```json
Server → Client: {"type": "setPage", "page": 18}
```

## WebSocket Server (`ws_server.c`)

### Implementation

The server is implemented using raw BSD sockets (no external dependencies):
- **macOS/Linux:** `socket()`, `bind()`, `listen()`, `accept()`, `select()`
- **Windows:** Winsock2 (`WSAStartup`, same socket API)

### Features

- Supports up to 4 concurrent WebSocket clients
- RFC 6455 compliant WebSocket handshake (SHA-1 + Base64 for `Sec-WebSocket-Accept`)
- Text frame send/receive (JSON messages)
- Non-blocking I/O via `select()`
- Static file serving from `frontend/dist/` (for production builds)

### Connection Flow

1. Client sends HTTP Upgrade request
2. Server validates `Sec-WebSocket-Key`, computes accept hash
3. Server sends HTTP 101 Switching Protocols response
4. Connection upgraded to WebSocket
5. Server sends `fullState` message with current display state
6. Bidirectional JSON messaging begins

## Important Byte Indices

These byte indices are critical for UI behavior:

| Index | Purpose | Key Values |
|-------|---------|------------|
| 3 | Page navigation (SET_HTML) | `MENU_MAIN_SCREEN=32` |
| 20 | Widget refresh trigger | `0xFF` = trigger fire |
| 80 | Language selection | `0` = English, `1` = French |
| 92 | Mode indicator | Various mode IDs |
| 100 | Language state | `0` = English, `1` = French |
| 101 | Generic trigger | `0xFF` = trigger fire |
| 189-192 | Button press events | Menu-specific values |

## Packet Processing Detail

### TX Packet Parsing (`handle_tx_packet`)

```
Raw UART bytes: D5 31 38 39 30 31 00
                │  │  │  │  │  │  └─ null terminator
                │  └──┴──┴──┴──┘  hex chars "18901"
                └─ command byte (SetByte)

Decoded: SetByte, index=189 (hex "BD"→189), value=1 (hex "01"→1)
Broadcast: {"type":"setByte","index":189,"value":1}
```

### RX Injection (`inject_byte_event`)

When a `buttonPress` message arrives:
```
Input: byteIndex=189, value=22
Encode: 0xD5 + hex(189) + hex(22) + 0x00
        → D5 42 44 31 36 00
Inject each byte into UART2 RX ring buffer via PushUart2Rx()
```

The firmware's `service_amulet()` reads these bytes on its next iteration and processes the command.
