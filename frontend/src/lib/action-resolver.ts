/**
 * action-resolver.ts - Resolves Amulet HTM button actions to WebSocket messages
 *
 * When a user clicks a button in the emulator, we need to translate the
 * Amulet href action (from the parsed HTM JSON) into the appropriate
 * WebSocket message that the amulet_bridge.c can process.
 *
 * The Amulet display firmware normally resolves macros like %SetHTML()
 * and %SetClear() internally, then sends UART commands to the CRC firmware.
 * We replicate that translation here.
 */

import { WebSocketManager } from './websocket'
import { AmuletStateManager } from './amulet-state'
import { RefreshAction } from './screen-loader'

/**
 * Resolve and execute a list of actions from a button press.
 * Actions come from the parsed HTM widget's href param.
 *
 * In the real Amulet hardware, the display handles page navigation itself
 * when buttons are clicked, then sends the byte(3) command to the firmware.
 * In the emulator, we replicate this by:
 *   1. Changing the frontend page locally (via stateManager.setPageLocally)
 *   2. Sending byte(3) to the firmware
 */
export function executeActions(
  actions: RefreshAction[],
  ws: WebSocketManager,
  stateManager?: AmuletStateManager,
): void {
  for (const action of actions) {
    executeAction(action, ws, stateManager)
  }
}

function executeAction(action: RefreshAction, ws: WebSocketManager, stateManager?: AmuletStateManager): void {
  switch (action.type) {
    case 'macro':
      executeMacro(action, ws, stateManager)
      break

    case 'setValue':
      executeSetValue(action, ws)
      break

    case 'nop':
      // No operation
      break

    default:
      // For document actions (reappear, disappear, forceUpdate, etc.)
      // these are display-side actions that we handle via the refresh trigger system
      break
  }
}

function executeMacro(action: RefreshAction, ws: WebSocketManager, stateManager?: AmuletStateManager): void {
  const macro = action.macro || ''
  const args = action.args || ''

  switch (macro) {
    case 'SetHTML': {
      // %SetHTML(%%SET_HTML_XXX) -> sends byte(3) = resolved value AND changes page
      const value = resolveMacroArg(args)
      if (value !== null) {
        // Navigate the frontend to the target page (display-side responsibility)
        const targetPage = SET_HTML_TO_PAGE[value]
        if (targetPage !== undefined && stateManager) {
          stateManager.setPageLocally(targetPage)
        }
        ws.send({ type: 'buttonPress', byteIndex: 3, value })
      }
      break
    }
    case 'SetClear': {
      // %SetClear(%%SET_CLEAR_XXX) -> sends byte(3) = resolved value AND changes page
      const value = resolveMacroArg(args)
      if (value !== null) {
        // Navigate the frontend to the target page
        const targetPage = SET_HTML_TO_PAGE[value]
        if (targetPage !== undefined && stateManager) {
          stateManager.setPageLocally(targetPage)
        }
        ws.send({ type: 'buttonPress', byteIndex: 3, value })
      }
      break
    }
    case 'Home': {
      // %Home -> navigate to main screen
      if (stateManager) {
        stateManager.setPageLocally(0x09) // MAINSCREEN_HTM
      }
      ws.send({ type: 'buttonPress', byteIndex: 3, value: 16 }) // SET_HTML_MAINSCREEN
      break
    }
    case 'Back': {
      // %Back -> navigate back (firmware tracks back stack)
      ws.send({ type: 'buttonPress', byteIndex: 4, value: 1 }) // back command
      break
    }
    case 'Beep':
      // Audio feedback - ignore in emulator
      break
    case 'SetHotkeyNuclide': {
      const value = resolveMacroArg(args)
      if (value !== null) {
        ws.send({ type: 'buttonPress', byteIndex: 5, value })
      }
      break
    }
    case 'SetAccuracyAll':
    case 'SetAccuracyDaily':
    case 'ShowDoseTableButton':
    case 'ConfigTitle':
    case 'ConfigNuclide':
    case 'ClearNuclide':
    case 'SetNuclide':
    case 'ConfigKeypad':
    case 'SetKeypad':
    case 'ConfigMaxStrLen':
    case 'ConfigMaxFractionLen':
    case 'ConfigMinValue':
    case 'ConfigMaxValue':
    case 'ConfigPlusMinus_OFF':
    case 'ConfigPlusMinus_ON':
    case 'ConfigDateAndTime':
    case 'SetTime':
    case 'ConfigX':
    case 'ConfigY':
    case 'SelectMode':
    case 'ActiveStaff':
    case 'ConfigTestIdent':
    case 'ConfigInvalidate':
      // These are Amulet-side configuration macros
      // They configure dialog/input parameters before showing a dialog
      // For now, if they have a numeric arg, send as byte(3)
      {
        const value = resolveMacroArg(args)
        if (value !== null) {
          ws.send({ type: 'buttonPress', byteIndex: 3, value })
        }
      }
      break
    default:
      console.log('[action] Unknown macro:', macro, args)
  }
}

function executeSetValue(action: RefreshAction, ws: WebSocketManager): void {
  const target = action.target || ''

  // Parse target: "internalRAM.byte(N)" or "UART.byte(N)" or "UART.word(N)"
  const iramByteMatch = target.match(/internalRAM\.byte\((\d+)\)/)
  const iramWordMatch = target.match(/internalRAM\.word\((\d+)\)/)
  const iramStringMatch = target.match(/internalRAM\.string\((\d+)\)/)
  const uartByteMatch = target.match(/UART\.byte\((\d+)\)/)
  const uartWordMatch = target.match(/UART\.word\((\d+)\)/)

  if (iramByteMatch) {
    const index = parseInt(iramByteMatch[1])
    const value = parseNumericValue(action.value)
    if (value !== null) {
      ws.send({ type: 'buttonPress', byteIndex: index, value })
    }
  } else if (iramWordMatch) {
    const index = parseInt(iramWordMatch[1])
    const value = parseNumericValue(action.value)
    if (value !== null) {
      ws.send({ type: 'wordChanged', wordIndex: index, value })
    }
  } else if (uartByteMatch) {
    const index = parseInt(uartByteMatch[1])
    const value = parseNumericValue(action.value)
    if (value !== null) {
      ws.send({ type: 'buttonPress', byteIndex: index, value })
    }
  } else if (uartWordMatch) {
    const index = parseInt(uartWordMatch[1])
    const value = parseNumericValue(action.value)
    if (value !== null) {
      ws.send({ type: 'wordChanged', wordIndex: index, value })
    }
  } else if (iramStringMatch) {
    const index = parseInt(iramStringMatch[1])
    const value = String(action.value ?? '')
    ws.send({ type: 'stringInput', stringIndex: index, value })
  }
}

/**
 * Resolve a macro argument to a numeric value.
 * Arguments can be:
 * - "%%SYMBOL" -> look up in macro table
 * - "0xNN" -> hex value
 * - "N" -> decimal value
 */
function resolveMacroArg(arg: string): number | null {
  if (!arg) return null
  arg = arg.trim()

  // %%SYMBOL reference
  if (arg.startsWith('%%')) {
    const symbol = arg.substring(2)
    const value = MACRO_TABLE[symbol]
    return value !== undefined ? value : null
  }

  return parseNumericValue(arg)
}

function parseNumericValue(value: any): number | null {
  if (value === null || value === undefined) return null
  if (typeof value === 'number') return value
  const str = String(value).trim()
  if (str.startsWith('0x') || str.startsWith('0X')) {
    return parseInt(str, 16)
  }
  const n = parseInt(str, 10)
  return isNaN(n) ? null : n
}

/**
 * Parse an href string from an Amulet widget into a list of actions.
 * This handles the common patterns found in HTM button href attributes.
 */
export function parseHrefActions(href: string): RefreshAction[] {
  if (!href) return []

  const actions: RefreshAction[] = []
  const parts = splitActionChain(href)

  for (const part of parts) {
    const trimmed = part.trim()
    if (!trimmed) continue

    // Macro: %MacroName or %MacroName(args)
    const macroMatch = trimmed.match(/^%(\w+)(?:\(([^)]*)\))?$/)
    if (macroMatch) {
      actions.push({
        type: 'macro',
        macro: macroMatch[1],
        args: macroMatch[2] || undefined,
      })
      continue
    }

    // Amulet:internalRAM.TYPE(N).setValue(V)
    const iramSetMatch = trimmed.match(
      /^Amulet:(?:i|I)nternal(?:RAM|Ram|ram)\.(\w+)\((\d+)\)\.setValue\(([^)]*)\)$/i,
    )
    if (iramSetMatch) {
      actions.push({
        type: 'setValue',
        target: `internalRAM.${iramSetMatch[1]}(${iramSetMatch[2]})`,
        value: iramSetMatch[3],
      })
      continue
    }

    // Amulet:UART.TYPE(N).setValue(V)
    const uartSetMatch = trimmed.match(
      /^Amulet:UART\.(\w+)\((\d+)\)\.setValue\(([^)]*)\)$/i,
    )
    if (uartSetMatch) {
      actions.push({
        type: 'setValue',
        target: `UART.${uartSetMatch[1]}(${uartSetMatch[2]})`,
        value: uartSetMatch[3],
      })
      continue
    }

    // Amulet:document.WIDGET.METHOD(args)
    const docMatch = trimmed.match(
      /^Amulet:document\.(\w+)\.(\w+)\(([^)]*)\)$/i,
    )
    if (docMatch) {
      actions.push({
        type: docMatch[2],
        target: docMatch[1],
        value: docMatch[3] || undefined,
      })
      continue
    }

    // Amulet:NOP()
    if (/^Amulet:NOP\(\)$/i.test(trimmed)) {
      actions.push({ type: 'nop' })
      continue
    }
  }

  return actions
}

function splitActionChain(chain: string): string[] {
  const parts: string[] = []
  let depth = 0
  let current: string[] = []

  for (const ch of chain) {
    if (ch === '(') {
      depth++
      current.push(ch)
    } else if (ch === ')') {
      depth--
      current.push(ch)
    } else if (ch === ',' && depth === 0) {
      parts.push(current.join(''))
      current = []
    } else {
      current.push(ch)
    }
  }
  if (current.length) parts.push(current.join(''))
  return parts
}

/**
 * Macro value lookup table.
 * Generated from Amulet.h #define values.
 */
const MACRO_TABLE: Record<string, number> = {
  // SET_HTML navigation
  SET_HTML_ACCURACY: 1,
  SET_HTML_BACKGROUND: 2,
  SET_HTML_CHAMBERVOLTS: 3,
  SET_HTML_DAILY: 4,
  SET_HTML_GEOMETRY: 5,
  SET_HTML_GEOMETRYREPORT: 6,
  SET_HTML_HMPAO: 7,
  SET_HTML_INDEX: 8,
  SET_HTML_INVENTORY: 9,
  SET_HTML_INVENTORYADD: 10,
  SET_HTML_INVENTORYWITHDRAW: 11,
  SET_HTML_KEYBOARD2: 12,
  SET_HTML_KEYPAD2: 13,
  SET_HTML_LINEARITY: 14,
  SET_HTML_MAG3: 15,
  SET_HTML_MAINSCREEN: 16,
  SET_HTML_MEASUREACTIVITY: 17,
  SET_HTML_MOLY: 18,
  SET_HTML_NUCLIDE2: 19,
  SET_HTML_ONESTRIP: 20,
  SET_HTML_QC: 21,
  SET_HTML_SETACTIVITY: 22,
  SET_HTML_SETSTUDY: 23,
  SET_HTML_SETTIME2: 24,
  SET_HTML_SETUP: 25,
  SET_HTML_TWOSTRIP: 26,
  SET_HTML_INVENTORYDELETE: 27,
  SET_HTML_INVENTORYKIT: 28,
  SET_HTML_SETSTUDY2: 29,
  SET_HTML_ENHANCED: 30,
  SET_HTML_INFO: 31,
  SET_HTML_PSETUP: 32,
  SET_HTML_SETUPSOURCES: 33,
  SET_HTML_SETUPMOLY: 34,
  SET_HTML_SETUPNUCLIDE: 35,
  SET_HTML_SETUPLINEARITY: 36,
  SET_HTML_SETUPREMOTE: 37,
  SET_HTML_SETUPCALNUM: 38,
  SET_HTML_ERRORMSG: 39,
  SET_HTML_WARNINGMSG: 40,
  SET_HTML_NOTIFICATIONMSG: 41,
  SET_HTML_SETUPLINEARITYSTANDARD: 42,
  SET_HTML_SETUPLINEARITYLINEATOR: 43,
  SET_HTML_SETUPLINEARITYCALICHECK: 44,
  SET_HTML_DOSETABLE: 45,
  SET_HTML_SELECT_CHAMBER: 46,
  SET_HTML_GRADIENT: 47,
  SET_HTML_WELLMAINSCREEN: 48,
  SET_HTML_BETAMAINSCREEN: 49,
  SET_HTML_WELLMEASUREMENT: 50,
  SET_HTML_WELLAUTOCALIBRATE: 51,
  SET_HTML_WELLPEAKS: 52,
  SET_HTML_WELLADVANCEDSETUP: 53,
  SET_HTML_WELLSETUPNUCLIDE: 54,
  SET_HTML_WELLSETUPSEALED: 55,
  SET_HTML_WELLSETUPSEALED2: 56,
  SET_HTML_WELLSETUPTESTSOURCE: 57,
  SET_HTML_WELLSETUPTRIGGERLEVEL: 58,
  SET_HTML_WELLSETUPEFFICIENCIES: 59,
  SET_HTML_WELLEDITEFFICIENCIES: 60,
  SET_HTML_WELLMANUAL: 61,
  SET_HTML_WELLMEASUREEFFICIENCY: 62,
  SET_HTML_AUTOCONSTANCY: 63,
  SET_HTML_SLEEP: 64,
  SET_HTML_FACTORY: 65,
  SET_HTML_SETUPCHAMBER: 66,
  SET_HTML_INVENTORYDELETEALL: 67,
  SET_HTML_WELLMDATEST: 68,
  SET_HTML_WELLMEASUREMENTS: 69,
  SET_HTML_WELLEDITFULLEFFICIENCY: 70,
  SET_HTML_SETUPCALIBSERIAL: 71,

  // SET_CLEAR values (same numbering as SET_HTML for page navigation)
  SET_CLEAR_ACCURACY: 1,
  SET_CLEAR_DAILY: 4,
  SET_CLEAR_MOLY: 18,
  SET_CLEAR_SETUP: 25,
  SET_CLEAR_INFO: 31,
  SET_CLEAR_WELLMAIN: 48,
  SET_CLEAR_SECURITY: 127,
  SET_CLEAR_INACTIVATE: 108,
  SET_CLEAR_TESTIDENT: 114,
  SET_CLEAR_CHAMBERDAILYTESTSEARCH: 160,
  SET_CLEAR_CHAMBERACCURACYSEARCH: 164,
  SET_CLEAR_CHAMBERZEROSEARCH: 161,
  SET_CLEAR_CHAMBERBACKGROUNDSEARCH: 162,
  SET_CLEAR_CHAMBERVOLTAGESEARCH: 163,
  SET_CLEAR_CHAMBERAUTOCONSTANCYSEARCH: 165,
  SET_CLEAR_WELLSETUPADDLOCATION: 100,
  SET_CLEAR_WELLSETUPEDITLOCATION: 100,
  SET_CLEAR_WELLSETUPDELETELOCATION: 101,

  // SET_HOTKEY_NUCLIDE
  SET_HOTKEY_NUCLIDE_1: 1,
  SET_HOTKEY_NUCLIDE_2: 2,
  SET_HOTKEY_NUCLIDE_3: 3,
  SET_HOTKEY_NUCLIDE_4: 4,
  SET_HOTKEY_NUCLIDE_5: 5,
  SET_HOTKEY_NUCLIDE_6: 6,
  SET_HOTKEY_NUCLIDE_7: 7,
  SET_HOTKEY_NUCLIDE_8: 8,

  // CONFIG_NUCLIDE
  CONFIG_NUCLIDE_ONLY_CAL: 0,

  // CLEAR_NUCLIDE
  CLEAR_NUCLIDE_HIDE: 0,

  // SET_NUCLIDE
  SET_NUCLIDE_MEASURE: 0,

  // CONFIG_KEYPAD
  CONFIG_KEYPAD_DEFAULT: 0,
  CONFIG_KEYPAD_PASSWORD: 1,
  CONFIG_KEYPAD_CALNUM: 2,

  // SET_KEYPAD
  SET_KEYPAD_TIME_PASSWD: 0,
  SET_KEYPAD_SOURCES_PASSWD: 1,
  SET_KEYPAD_CALNUM: 2,

  // SCREEN
  SCREEN_MAIN_SCREEN: 0x09,
  SCREEN_DAILY: 0x10,
}

/**
 * Mapping from SET_HTML/SET_CLEAR values to HTM enum indices (page IDs).
 *
 * When the Amulet display executes a %SetHTML(%%SET_HTML_DAILY) macro,
 * it navigates to the DAILY HTM page AND sends byte(3)=4 to the firmware.
 * This table tells the frontend which page to display for each SET_HTML value.
 *
 * Derived from CallHTML() in Amulet.c and the HTM defines in Amulet.h.
 */
const SET_HTML_TO_PAGE: Record<number, number> = {
  1:  0x14, // SET_HTML_ACCURACY     → ACCURACY_HTM
  2:  0x11, // SET_HTML_BACKGROUND   → BACKGROUND_HTM
  3:  0x12, // SET_HTML_CHAMBERVOLTS → CHAMBERVOLTS_HTM
  4:  0x10, // SET_HTML_DAILY        → DAILY_HTM
  5:  0x13, // SET_HTML_GEOMETRY     → GEOMETRY_HTM
  6:  0x0A, // SET_HTML_GEOMETRYREPORT → GEOMETRYREPORT_HTM
  7:  0x0E, // SET_HTML_HMPAO        → HMPAO_HTM
  8:  0x00, // SET_HTML_INDEX        → INDEX_HTM
  9:  0x18, // SET_HTML_INVENTORY    → INVENTORY_HTM
  10: 0x07, // SET_HTML_INVENTORYADD → INVENTORYADD_HTM
  11: 0x08, // SET_HTML_INVENTORYWITHDRAW → INVENTORYWITHDRAW_HTM
  12: 0x03, // SET_HTML_KEYBOARD2    → KEYBOARD2_HTM
  13: 0x02, // SET_HTML_KEYPAD2      → KEYPAD2_HTM
  14: 0x15, // SET_HTML_LINEARITY    → LINEARITY_HTM
  15: 0x0F, // SET_HTML_MAG3         → MAG3_HTM
  16: 0x09, // SET_HTML_MAINSCREEN   → MAINSCREEN_HTM
  17: 0x06, // SET_HTML_MEASUREACTIVITY → MEASUREACTIVITY_HTM
  18: 0x17, // SET_HTML_MOLY         → MOLY_HTM
  19: 0x01, // SET_HTML_NUCLIDE2     → NUCLIDE2_HTM
  20: 0x0C, // SET_HTML_ONESTRIP     → ONESTRIP_HTM
  21: 0x16, // SET_HTML_QC           → QC_HTM
  22: 0x05, // SET_HTML_SETACTIVITY  → SETACTIVITY_HTM
  23: 0x0B, // SET_HTML_SETSTUDY     → SETSTUDY_HTM
  24: 0x04, // SET_HTML_SETTIME2     → SETTIME2_HTM
  25: 0x19, // SET_HTML_SETUP        → SETUP_HTM
  26: 0x0D, // SET_HTML_TWOSTRIP     → TWOSTRIP_HTM
  27: 0x1A, // SET_HTML_INVENTORYDELETE → INVENTORYDELETE_HTM
  28: 0x1B, // SET_HTML_INVENTORYKIT → INVENTORYKIT_HTM
  29: 0x1C, // SET_HTML_SETSTUDY2    → SETSTUDY2_HTM
  30: 0x1D, // SET_HTML_ENHANCED     → ENHANCED_HTM
  31: 0x1E, // SET_HTML_INFO         → INFO_HTM
  32: 0x1F, // SET_HTML_PSETUP       → OTHER_HTM
  33: 0x20, // SET_HTML_SETUPSOURCES → SETUPSOURCES_HTM
  34: 0x21, // SET_HTML_SETUPMOLY    → SETUPMOLY_HTM
  35: 0x22, // SET_HTML_SETUPNUCLIDE → SETUPNUCLIDE_HTM
  36: 0x23, // SET_HTML_SETUPLINEARITY → SETUPLINEARITY_HTM
  37: 0x24, // SET_HTML_SETUPREMOTE  → SETUPREMOTE_HTM
  38: 0x25, // SET_HTML_SETUPCALNUM  → SETUPCALNUM_HTM
  39: 0x26, // SET_HTML_ERRORMSG     → ERRORMSG_HTM
  40: 0x27, // SET_HTML_WARNINGMSG   → WARNINGMSG_HTM
  41: 0x28, // SET_HTML_NOTIFICATIONMSG → NOTIFICATIONMSG_HTM
  42: 0x29, // SET_HTML_SETUPLINEARITYSTANDARD → SETUPLINEARITYSTANDARD_HTM
  43: 0x2A, // SET_HTML_SETUPLINEARITYLINEATOR → SETUPLINEARITYLINEATOR_HTM
  44: 0x2B, // SET_HTML_SETUPLINEARITYCALICHECK → SETUPLINEARITYCALICHECK_HTM
  45: 0x2C, // SET_HTML_DOSETABLE    → DOSETABLE_HTM
  46: 0x2D, // SET_HTML_SELECT_CHAMBER → CHAMBER_HTM
  47: 0x2E, // SET_HTML_GRADIENT     → GRADIENT_HTM
  48: 0x2F, // SET_HTML_WELLMAINSCREEN → WELLMAINSCREEN_HTM
  49: 0x30, // SET_HTML_BETAMAINSCREEN → BETAMAINSCREEN_HTM
  50: 0x31, // SET_HTML_WELLMEASUREMENT → WELLMEASUREMENT_HTM
  51: 0x32, // SET_HTML_WELLAUTOCALIBRATE → WELLAUTOCALIBRATE_HTM
  52: 0x33, // SET_HTML_WELLPEAKS    → WELLPEAKS_HTM
  53: 0x34, // SET_HTML_WELLADVANCEDSETUP → WELLADVANCEDSETUP_HTM
  54: 0x35, // SET_HTML_WELLSETUPNUCLIDE → WELLSETUPNUCLIDE_HTM
  55: 0x37, // SET_HTML_WELLSETUPSEALED2 → WELLSETUPSEALED2_HTM
  56: 0x37, // SET_HTML_WELLSETUPSEALED2 → WELLSETUPSEALED2_HTM
  57: 0x38, // SET_HTML_WELLSETUPTESTSOURCE → WELLSETUPTESTSOURCE_HTM
  58: 0x39, // SET_HTML_WELLSETUPTRIGGERLEVEL → WELLSETUPTRIGGERLEVEL_HTM
  59: 0x3A, // SET_HTML_WELLSETUPEFFICIENCIES → WELLSETUPEFFICIENCIES_HTM
  60: 0x3B, // SET_HTML_WELLEDITEFFICIENCIES → WELLEDITEFFICIENCIES_HTM
  61: 0x3C, // SET_HTML_WELLMANUAL   → WELLMANUAL_HTM
  62: 0x3D, // SET_HTML_WELLMEASUREEFFICIENCY → WELLMEASUREEFFICIENCY_HTM
  63: 0x3E, // SET_HTML_AUTOCONSTANCY → AUTOCONSTANCY_HTM
  64: 0x3F, // SET_HTML_SLEEP        → SLEEP_HTM
  65: 0x40, // SET_HTML_FACTORY      → FACTORY_HTM
  66: 0x41, // SET_HTML_SETUPCHAMBER → SETUPCHAMBER_HTM
  67: 0x42, // SET_HTML_INVENTORYDELETEALL → INVENTORYDELETEALL_HTM
  68: 0x43, // SET_HTML_WELLMDATEST  → WELLMDATEST_HTM
  69: 0x44, // SET_HTML_WELLMEASUREMENTS → WELLMEASUREMENTS_HTM
  70: 0x45, // SET_HTML_WELLEDITFULLEFFICIENCY → WELLEDITFULLEFFICIENCY_HTM
  71: 0x46, // SET_HTML_SETUPCALIBSERIAL → SETUPCALIBSERIALNUM_HTM
}
