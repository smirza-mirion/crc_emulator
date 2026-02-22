# WebSocket Protocol

## Overview

The emulator backend and frontend communicate over WebSocket using JSON messages. The backend runs a WebSocket server on port 9876 at path `/ws`. All messages are UTF-8 encoded JSON objects with a `type` field identifying the message kind.

## Connection

### Endpoint

```
ws://localhost:9876/ws
```

### Handshake

Standard RFC 6455 WebSocket upgrade over HTTP:
```
GET /ws HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: <base64-encoded-key>
Sec-WebSocket-Version: 13
```

### Capacity

Maximum 4 concurrent WebSocket clients. Additional connections are rejected.

### Auto-Reconnect

The frontend automatically reconnects every 2 seconds if the connection drops. On reconnect, the backend sends a `fullState` message to synchronize.

## Server → Client Messages

### setByte

Sent when the firmware changes a byte variable.

```json
{"type": "setByte", "index": 20, "value": 255}
```

| Field | Type | Range | Description |
|-------|------|-------|-------------|
| `index` | number | 0-255 | Byte variable index |
| `value` | number | 0-255 | New byte value |

### setWord

Sent when the firmware changes a word variable.

```json
{"type": "setWord", "index": 5, "value": 1234}
```

| Field | Type | Range | Description |
|-------|------|-------|-------------|
| `index` | number | 0-255 | Word variable index |
| `value` | number | 0-65535 | New word value |

### setString

Sent when the firmware changes a string variable.

```json
{"type": "setString", "index": 10, "value": "15.00"}
```

| Field | Type | Description |
|-------|------|-------------|
| `index` | number | String variable index (0-255) |
| `value` | string | New string value (up to 255 chars) |

### setPage

Sent when the firmware navigates to a new screen.

```json
{"type": "setPage", "page": 18}
```

| Field | Type | Description |
|-------|------|-------------|
| `page` | number | Page index (0-168, maps to HTM screen) |

### drawLine

Sent when the firmware draws a line on the display.

```json
{"type": "drawLine", "x1": 10, "y1": 70, "x2": 790, "y2": 70, "color": "#0000FF"}
```

| Field | Type | Description |
|-------|------|-------------|
| `x1`, `y1` | number | Start coordinates |
| `x2`, `y2` | number | End coordinates |
| `color` | string | CSS color (hex RGB) |

### fillRect

Sent when the firmware fills a rectangle on the display.

```json
{"type": "fillRect", "x": 0, "y": 0, "w": 800, "h": 69, "color": "#3333AA"}
```

| Field | Type | Description |
|-------|------|-------------|
| `x`, `y` | number | Top-left corner |
| `w`, `h` | number | Width and height |
| `color` | string | CSS color (hex RGB) |

### fullState

Sent on initial client connection and on reconnect. Contains the complete current state.

```json
{
  "type": "fullState",
  "bytes": {"0": 0, "3": 32, "20": 0, "80": 0, "100": 0, "189": 0},
  "words": {"5": 0},
  "strings": {"3": "Tc-99m", "10": "0.000", "80": "Daily Test"},
  "page": 9
}
```

| Field | Type | Description |
|-------|------|-------------|
| `bytes` | object | Non-zero byte variables as `{index: value}` |
| `words` | object | Non-zero word variables as `{index: value}` |
| `strings` | object | Non-empty string variables as `{index: value}` |
| `page` | number | Current page index |

## Client → Server Messages

### buttonPress

Sent when the user clicks a button in the browser. Equivalent to the Amulet SetByte command (0xD5).

```json
{"type": "buttonPress", "byteIndex": 189, "value": 22}
```

| Field | Type | Description |
|-------|------|-------------|
| `byteIndex` | number | Target byte variable index |
| `value` | number | Value to set (button action ID) |

### stringInput

Sent when the user submits text input. Equivalent to the Amulet SetString command (0xD7).

```json
{"type": "stringInput", "stringIndex": 3, "value": "112"}
```

| Field | Type | Description |
|-------|------|-------------|
| `stringIndex` | number | Target string variable index |
| `value` | string | Input text |

### wordChanged

Sent when a word value changes (e.g., slider moved). Equivalent to the Amulet SetWord command (0xD6).

```json
{"type": "wordChanged", "wordIndex": 5, "value": 42}
```

| Field | Type | Description |
|-------|------|-------------|
| `wordIndex` | number | Target word variable index |
| `value` | number | New word value |

### loadScenario

Sent from the control panel to load a preset simulation scenario.

```json
{"type": "loadScenario", "scenario": 1}
```

| Field | Type | Range | Description |
|-------|------|-------|-------------|
| `scenario` | number | 0-9 | Scenario ID (see Simulation Engine docs) |

### configSource

Sent from the control panel to configure an individual chamber source.

```json
{
  "type": "configSource",
  "channel": 0,
  "enabled": true,
  "nuclide": "Tc-99m",
  "activity": 15.0,
  "unit": "mCi"
}
```

| Field | Type | Description |
|-------|------|-------------|
| `channel` | number | Chamber index (0-7) |
| `enabled` | boolean | Enable/disable chamber |
| `nuclide` | string | Nuclide name |
| `activity` | number | Activity value |
| `unit` | string | Activity unit (uCi, mCi, Ci, kBq, MBq, GBq) |

## Message Flow Examples

### Button Press (Daily Test)

```
1. User clicks "Daily" button
2. Frontend: pressButton(189, 1)
3. WS →  {"type":"buttonPress","byteIndex":189,"value":1}
4. Bridge: encode as Amulet 0xD5 packet, inject into UART2 RX
5. Firmware: service_amulet() reads command, sets m_iMenu=MENU_DAILY
6. Firmware: SetAmuletHTML(DAILY_HTM), which sends page via UART2 TX
7. Bridge: parse TX, broadcast setPage
8. WS ←  {"type":"setPage","page":16}
9. Frontend: loads Daily.json, renders Daily screen
```

### Real-Time Measurement Update

```
1. Timer thread fires (every 10ms)
2. hal_pit.c:timed_interrupt() calls hal_qspi.c:service_adc()
3. service_adc() calls sim_get_adc_value(0) → returns 15234
4. Firmware processes ADC, calculates activity string
5. Firmware: SetAmuletString(10, "15.00")
6. hal_uart.c routes via PushUart2Tx → amulet_bridge_process_tx
7. Bridge: parse 0xD7 packet, update state_strings[10]
8. WS ←  {"type":"setString","index":10,"value":"15.00"}
9. Frontend: AmuletStringField for index 10 re-renders with "15.00"
```

### Page Navigation with Widget Setup

```
1. Firmware: navigates to ChamberVolts (page 18)
2. WS ←  {"type":"setPage","page":18}
3. Frontend: loads ChamberVolts.json
4. Firmware: SetAmuletByte(80, current.language)  → byte(80) = 0
5. WS ←  {"type":"setByte","index":80,"value":0}
6. Firmware: SetAmuletByte(20, 0xFF)  → trigger fire
7. WS ←  {"type":"setByte","index":20,"value":255}
8. Frontend: evaluates refreshTriggers for byte(20)=255
   → triggers reappear(btnHome), reappear(btnBack), etc.
9. Firmware: SetAmuletByte(20, 0) → reset trigger
10. WS ←  {"type":"setByte","index":20,"value":0}
```

## Important Variable Indices

### Byte Variables

| Index | Purpose | Common Values |
|-------|---------|---------------|
| 3 | Page navigation command | SET_HTML_* values |
| 20 | Widget refresh trigger | 0xFF = fire, then reset to 0 |
| 21 | Secondary trigger | 0xFF = fire |
| 22 | Tertiary trigger | 0xFF = fire |
| 23 | Quaternary trigger | 0xFF = fire |
| 80 | Language for widget visibility | 0 = English, 1 = French |
| 92 | Mode indicator | Various mode IDs |
| 100 | System language setting | 0 = English, 1 = French |
| 101 | Generic trigger | 0xFF = fire |
| 189-192 | Button press / menu events | Menu-specific values |

### String Variables

| Index | Purpose |
|-------|---------|
| 0 | Status message |
| 2 | Nuclide name |
| 3 | User input field |
| 9 | Chamber label |
| 10 | Activity display (main reading) |
| 11 | Activity unit |
| 80 | Screen title |
| 101 | Secondary message |

## Error Handling

- Invalid JSON is silently dropped (logged to console)
- Unknown message types are ignored
- WebSocket frame errors cause client disconnect (auto-reconnect handles this)
- If the emulator backend crashes, the frontend shows "Disconnected" and retries
