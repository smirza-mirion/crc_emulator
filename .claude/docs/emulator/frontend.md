# Frontend (React/TypeScript)

## Purpose

The frontend provides a browser-based, pixel-accurate reproduction of the CRC-25R's 800x600 Amulet touchscreen display. It renders all 168 screens from JSON definitions, handles user interaction, and provides a simulation control panel.

## Tech Stack

| Component | Version | Purpose |
|-----------|---------|---------|
| React | 18.2 | UI framework |
| TypeScript | 5.x | Type safety |
| Vite | 5.x | Build tool and dev server |
| WebSocket (native) | - | Communication with emulator backend |

## Source Files

```
frontend/src/
├── App.tsx                          # Root component, WebSocket connection
├── main.tsx                         # React entry point
├── components/
│   ├── DeviceDisplay.tsx            # 800x600 display container
│   ├── ScreenRenderer.tsx           # Dynamic screen rendering + triggers
│   ├── ControlPanel.tsx             # Simulation control (3 tabs)
│   └── widgets/
│       ├── AmuletButton.tsx         # Touchscreen button
│       ├── AmuletFunctionButton.tsx # Function key button
│       ├── AmuletStringField.tsx    # Text display/input field
│       ├── AmuletImage.tsx          # Image widget
│       ├── AmuletBarGraph.tsx       # Bar graph / spectrum display
│       └── index.ts                 # Widget exports
└── lib/
    ├── websocket.ts                 # WebSocket client with auto-reconnect
    ├── amulet-state.ts              # Amulet variable state manager
    ├── screen-loader.ts             # JSON screen definition loader
    └── action-resolver.ts           # Widget action execution
```

## Component Architecture

### App (`App.tsx`)

Root component that manages the WebSocket connection and provides managers to children.

- Creates `WebSocketManager` and `AmuletStateManager` instances
- Connects to `ws://localhost:9876/ws`
- Layout: horizontal flex with `DeviceDisplay` (left, 820px) and `ControlPanel` (right, 350-450px)
- Displays connection status indicator (green dot = connected, red = disconnected)
- Shows current page index in header

### DeviceDisplay (`DeviceDisplay.tsx`)

Container for the 800x600 device display:
- Fixed dimensions matching the physical Amulet touchscreen
- Blue-gray background with shadow border
- Renders `ScreenRenderer` as the dynamic content area

### ScreenRenderer (`ScreenRenderer.tsx`)

The core rendering engine. Responsible for:

1. **Screen Loading:** When the firmware navigates to a new page (via `setPage` message), loads the corresponding JSON screen definition from `/screens/{name}.json` using the page-to-name mapping from `/screen_map.json`.

2. **Widget Rendering:** Iterates over the screen's `widgets[]` array and renders each as the appropriate React component (button, text field, image, etc.) at its absolute pixel position.

3. **Refresh Triggers:** Evaluates the screen's `refreshTriggers[]` when byte/word values change:
   - `onVar`: specifies the byte/word index to monitor (e.g., `byte(20)`)
   - `trigger`: the value that activates the trigger (e.g., `255`)
   - `actions`: what happens when triggered:
     - `reappear(target)`: show widget
     - `disappear(target)`: hide widget
     - `setValue(byte, index, value)`: reset a byte value
     - `forceUpdate(target)`: re-evaluate another trigger by prefix name match

4. **Initial Actions:** On screen load, executes `initActions[]` to set initial widget visibility.

5. **Language Switching:** Widgets can have `language: "english"` or `language: "french"` tags. Visibility is controlled by byte(100): 0 = show English, 1 = show French variants.

#### Trigger Chain System

The trigger system supports chained evaluation:

- **Toggle triggers** have a `setValue` action that resets their byte back to 0. These fire first during initial replay to set up the trigger chain.
- **State triggers** respond to current byte values (like language byte 80) and show/hide widgets accordingly.
- **forceUpdate** creates chains: when trigger A fires and its action includes `forceUpdate("refreshShowHome")`, the renderer finds all triggers whose `name` starts with `"refreshShowHome"` and evaluates them against current byte values.

Example chain (ChamberVolts screen):
```
byte(20) = 0xFF → refreshChambString triggers
  → forceUpdate("refreshShowHome")
    → matches refreshShowHomeEnglish (byte(80)=0) → reappear(btnHome)
    → matches refreshShowHomeFrench (byte(80)=1) → disappear(btnHome)
```

### ControlPanel (`ControlPanel.tsx`)

Three-tab control panel for simulation management:

**Scenarios Tab:**
- 10 preset scenario cards with name and description
- Click to load scenario (sends `loadScenario` message)
- Visual highlight on selected scenario

**Custom Tab:**
- Per-chamber `SourceConfig` sub-component (currently 2 chambers)
- Enable/disable checkbox
- Nuclide dropdown: Tc-99m, Co-57, Ba-133, Cs-137, Co-60, F-18, I-131, Mo-99
- Activity input with unit selector: uCi, mCi, Ci, kBq, MBq, GBq
- Apply button sends `configSource` message

**Debug Tab:**
- `DebugPanel` sub-component
- Refresh button to update displayed values
- Shows key byte variables (indices 20, 21, 22, 23, 92, 100, 189-192) with hex and decimal
- Shows key string variables (indices 0, 2, 3, 9-11, 80, 101)
- Shows current page index

## Widget Components

### AmuletButton

Renders a touchscreen button with up/down image states:
- Position: absolute, based on `x`, `y`, `width`, `height` from screen definition
- Images: `imageUp` (normal) and `imageDown` (pressed) PNGs from `/assets/images/`
- Interaction: on click, calls `stateManager.pressButton(byteIndex, value)`
- Visual feedback: image swap on mousedown/mouseup

### AmuletFunctionButton

Similar to AmuletButton but styled for function key rows (F1-F8):
- Typically rendered in the bottom navigation bar
- Same click-to-pressButton behavior

### AmuletStringField

Displays a text value bound to an Amulet string variable:
- Subscribes to `stateManager.onStringChange()` for its bound string index
- Updates in real-time as the firmware sends new string values
- Supports font, color, size, and alignment from the screen definition
- For input fields: accepts keyboard input and sends `stateManager.sendString()` on Enter

### AmuletImage

Renders a static or dynamic image:
- Loads from `/assets/images/` or `/public/screens/`
- Scales to widget dimensions
- Supports visibility toggling via refresh triggers

### AmuletBarGraph

Renders bar chart visualizations:
- Used for spectrum displays and activity graphs
- Updates based on word variable changes
- Supports horizontal and vertical orientations

## Library Modules

### WebSocketManager (`lib/websocket.ts`)

Manages the WebSocket connection to the emulator backend:

```typescript
class WebSocketManager {
  connect(url: string)     // Open connection (ws://localhost:9876/ws)
  disconnect()             // Close connection
  send(data: any)          // Send JSON message
  on(type: string, handler) // Register message type handler
  off(type: string, handler) // Unregister handler
  isConnected: boolean     // Connection status
  onConnect: () => void    // Connection callback
  onDisconnect: () => void // Disconnection callback
}
```

Features:
- Auto-reconnect every 2 seconds on disconnect
- JSON serialization/deserialization
- Wildcard handler (`*`) receives all message types
- Error-tolerant parsing (warns on invalid JSON)

### AmuletStateManager (`lib/amulet-state.ts`)

Mirrors the backend's Amulet variable state:

```typescript
class AmuletStateManager {
  // State arrays
  bytes: Uint8Array[256]
  words: Uint16Array[256]
  strings: string[256]
  currentPage: number

  // Getters
  getByte(index): number
  getWord(index): number
  getString(index): string
  getPage(): number

  // Change subscriptions
  onByteChange(handler)  / offByteChange(handler)
  onWordChange(handler)  / offWordChange(handler)
  onStringChange(handler) / offStringChange(handler)
  onPageChangeAdd(handler) / offPageChange(handler)

  // User interactions (sends to firmware)
  pressButton(byteIndex: number, value: number)
  sendString(stringIndex: number, value: string)
  sendWord(wordIndex: number, value: number)

  // Local state update
  setPageLocally(page: number)
}
```

Automatically processes these incoming message types:
- `setByte` → updates `bytes[]`, notifies byte handlers
- `setWord` → updates `words[]`, notifies word handlers
- `setString` → updates `strings[]`, notifies string handlers
- `setPage` → updates `currentPage`, notifies page handlers
- `fullState` → bulk update all state, notifies all handlers

### Screen Loader (`lib/screen-loader.ts`)

Loads and caches JSON screen definitions:

```typescript
interface ScreenDef {
  name: string
  filename: string
  resolution: { width: number; height: number }
  widgets: WidgetDef[]
  staticImages?: StaticImage[]
  refreshTriggers?: RefreshTrigger[]
  initActions?: RefreshAction[]
  hasFrenchVariants?: boolean
}

loadScreenMap(): Promise<Record<string, string>>    // Page index → name
loadScreen(name: string): Promise<ScreenDef | null>  // Load by name
loadScreenByPage(page: number): Promise<ScreenDef | null> // Load by page index
getScreenNameForPage(page: number): string | null    // Quick lookup
```

Features:
- Fetches from `/screens/{name}.json` and `/screen_map.json`
- Caches loaded screen definitions in memory
- Returns null for unknown pages/screens

### Action Resolver (`lib/action-resolver.ts`)

Handles complex widget action execution including:
- Button press event routing
- String input validation and submission
- Word/byte value manipulation
- Trigger condition evaluation
- Multi-step action sequences

## Static Assets

### Screen Definitions (`public/screens/`)

168 JSON files generated by `tools/htm_parser/parse_htm.py` from the Amulet `.htm` files. Each contains:
- Widget positions, sizes, and parameters
- Variable bindings (byte, word, string indices)
- Refresh triggers with conditions and actions
- Initial actions for screen setup
- Image references

### Screen Map (`public/screen_map.json`)

Maps firmware page indices (0x00-0xA8) to screen names:
```json
{
  "9": "MainScreen",
  "16": "Daily",
  "17": "Background",
  "18": "ChamberVolts",
  "20": "Accuracy",
  "25": "Setup"
}
```

### Images (`public/assets/images/`)

~150 PNG/GIF files from `CRCHtml_3.00a/Images/`:
- Navigation buttons (English and French variants)
- Background images (sky gradient)
- Tab icons and indicators
- `french_*.png` — French-language button variants

## Development

### Dev Server

```bash
cd frontend && npx vite
```

Runs on port 3000 with hot module replacement (HMR).

### Build

```bash
cd frontend && npx vite build
```

Outputs to `frontend/dist/` which can be served by the emulator's built-in HTTP server.

### Type Checking

```bash
cd frontend && npx tsc --noEmit
```

## Key Implementation Details

### Widget Visibility

Widget visibility is managed by a `Set<string>` of hidden widget names. The ScreenRenderer maintains `hiddenWidgets` state that is computed from:
1. Initial trigger evaluation on screen load
2. Runtime byte/word change trigger evaluation
3. Language-based visibility (byte 100)

### Connection Recovery

When the WebSocket reconnects (after backend restart), the backend sends a `fullState` message that synchronizes all byte/word/string values and the current page. The frontend re-renders the current screen with the correct state.

### Performance

- Screen definitions are cached after first load (no re-fetch on revisit)
- Byte/word/string change handlers are targeted (only fire for relevant components)
- Widget rendering uses absolute positioning (no layout recalculation)
