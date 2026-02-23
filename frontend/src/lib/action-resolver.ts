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
      // %SetHTML(%%SET_HTML_XXX) = UART.byte(255).setValue(N) from CRCHtml_3.00a/Amulet.h
      // On real hardware: Amulet navigates display, then page load sends byte(3) = SCREEN_*.
      // We navigate locally and send byte(3) with the correct SCREEN value for m_iMenu.
      const htmlValue = resolveMacroArg(args)
      if (htmlValue !== null) {
        // Navigate the frontend to the target page (display-side responsibility)
        const targetPage = SET_HTML_TO_PAGE[htmlValue]
        if (targetPage !== undefined && stateManager) {
          stateManager.setPageLocally(targetPage)
        }
        // Send byte(3) = SCREEN value for menu registration (sets m_iMenu)
        const screenValue = SET_HTML_TO_SCREEN[htmlValue]
        if (screenValue !== undefined) {
          ws.send({ type: 'buttonPress', byteIndex: 3, value: screenValue })
        }
      }
      break
    }
    case 'SetClear': {
      // %SetClear(%%SET_CLEAR_XXX) = UART.byte(14).setValue(N)
      // On real hardware: sends byte(14) to firmware, firmware navigates + sets m_ucClear,
      // then page load sends byte(3) = SCREEN_* to set m_iMenu.
      const clearValue = resolveMacroArg(args)
      if (clearValue !== null) {
        // Navigate the frontend locally for instant feedback
        const targetPage = SET_CLEAR_TO_PAGE[clearValue]
        if (targetPage !== undefined && stateManager) {
          stateManager.setPageLocally(targetPage)
        }
        // Send byte(14) = clear value to firmware (sets m_ucClear + navigates)
        ws.send({ type: 'buttonPress', byteIndex: 14, value: clearValue })
        // Send byte(3) = SCREEN value to firmware (sets m_iMenu for menu handler)
        const screenValue = SET_CLEAR_TO_SCREEN[clearValue]
        if (screenValue !== undefined) {
          ws.send({ type: 'buttonPress', byteIndex: 3, value: screenValue })
        }
      }
      break
    }
    case 'Home': {
      // %Home = UART.byte(0).setValue(0x16) from CRCHtml_3.00a/Amulet.h
      if (stateManager) {
        stateManager.setPageLocally(0x09) // MAINSCREEN_HTM
      }
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x16 })
      break
    }
    case 'Back': {
      // %Back = UART.byte(0).setValue(4) from CRCHtml_3.00a/Amulet.h
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 4 })
      break
    }
    case 'Beep':
      // Audio feedback - ignore in emulator
      break
    case 'SetScreen': {
      // SetScreen(%%SCREEN_*) = sends byte(3) to register the firmware menu handler.
      // On real hardware, each HTM page calls this on load to tell the firmware
      // which menu handler (m_iMenu) to activate.
      const value = resolveMacroArg(args)
      if (value !== null) {
        ws.send({ type: 'buttonPress', byteIndex: 3, value })
      }
      break
    }
    case 'SetHotkeyNuclide': {
      const value = resolveMacroArg(args)
      if (value !== null) {
        ws.send({ type: 'buttonPress', byteIndex: 5, value })
      }
      break
    }
    // Macros with fixed byte(0) values (no args) from CRCHtml_3.00a/Amulet.h
    case 'SetAccuracyAll':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 9 })
      break
    case 'SetAccuracyDaily':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 10 })
      break
    case 'ShowDoseTableButton':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x22 })
      break
    case 'HideDoseTableButton':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x23 })
      break
    case 'ConfigPlusMinus_ON':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 15 })
      break
    case 'ConfigPlusMinus_OFF':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 16 })
      break
    case 'ConfigDateAndTime':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x20 })
      break
    case 'ConfigDateOnly':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x21 })
      break
    case 'SelectMode':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x39 })
      break
    case 'ActiveStaff':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x3B })
      break
    case 'AllStaff':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x3A })
      break
    case 'SecurityMode':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x38 })
      break
    // Linearity run macros (from SetupLinearity sub-screens)
    case 'RunLinearityStandard':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x13 })
      break
    case 'RunLinearityLineator':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x14 })
      break
    case 'RunLinearityCalicheck':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x15 })
      break
    // Well counter / detector macros
    case 'LockDetector':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x24 })
      break
    case 'UnLockDetector':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x25 })
      break
    case 'AddUserNuclide':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x26 })
      break
    case 'EditUserNuclide':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x27 })
      break
    case 'EditEfficiencyWellDetector':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x28 })
      break
    case 'EditEfficiencyProbeDetector':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x29 })
      break
    case 'AddEditUserNuclideWellDetector':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x2A })
      break
    case 'AddEditUserNuclideProbeDetector':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x2B })
      break
    case 'MDATestFromInstalledDetector':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x2C })
      break
    case 'MDATestFromDrilledProbeAsWell':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x2D })
      break
    case 'MDATestFromDrilledProbeAsProbe':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x2E })
      break
    case 'ChiSquareTestFromInstalledDetector':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x2F })
      break
    case 'ChiSquareTestFromDrilledProbeAsWell':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x30 })
      break
    case 'ChiSquareTestFromDrilledProbeAsProbe':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x31 })
      break
    // Login macros
    case 'LoginAllowGuestLogin':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x32 })
      break
    case 'LoginBlockGuestLogin':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x33 })
      break
    case 'LoginMinRoleZero':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x34 })
      break
    case 'LoginMinRoleOne':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x35 })
      break
    case 'LoginMinRoleTwo':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x36 })
      break
    case 'LoginMinRoleThree':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x37 })
      break
    // Thyroid uptake macros
    case 'TUProtocolSetupMode':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x3C })
      break
    case 'TUProtocolSelectMode':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x3D })
      break
    // General measurement macros
    case 'GeneralMeasFromInstalledDetector':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x3E })
      break
    case 'GeneralMeasFromDrilledProbeAsWell':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x3F })
      break
    case 'GeneralMeasFromDrilledProbeAsProbe':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x40 })
      break
    // Wildcard / display macros
    case 'HideWildcardChar':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x41 })
      break
    case 'ShowWildcardChar':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x42 })
      break
    // Normal value entry/view macros
    case 'EnterTUNormal':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x43 })
      break
    case 'ViewTUNormal':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x44 })
      break
    case 'EnterRBCSurvivalNormal':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x45 })
      break
    case 'ViewRBCSurvivalNormal':
      ws.send({ type: 'buttonPress', byteIndex: 0, value: 0x46 })
      break
    // Macros with specific byte indices (take args)
    case 'ConfigMaxStrLen': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 1, value })
      break
    }
    case 'ConfigMaxFractionLen': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 2, value })
      break
    }
    case 'SetNuclide': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 6, value })
      break
    }
    case 'SetSpecialFunction': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 15, value })
      break
    }
    case 'ConfigKeypad': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 16, value })
      break
    }
    case 'SetKeyboard': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 18, value })
      break
    }
    case 'SetTime': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 20, value })
      break
    }
    case 'SetActivity': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 19, value })
      break
    }
    case 'ConfigActivity': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 23, value })
      break
    }
    case 'SetMeasurement': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 21, value })
      break
    }
    case 'SendCheckboxValue': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 24, value })
      break
    }
    case 'ConfigNuclide': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 26, value })
      break
    }
    case 'ConfigKeyboard': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 28, value })
      break
    }
    case 'SetWellMeasurement': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 30, value })
      break
    }
    case 'ClearNuclide': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 32, value })
      break
    }
    case 'ConfigGenericItems': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 35, value })
      break
    }
    case 'ConfigInvalidate': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 36, value })
      break
    }
    case 'ConfigTestIdent': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 37, value })
      break
    }
    case 'ConfigGenericYesNo': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 38, value })
      break
    }
    case 'SendPrintSummaryValue': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'buttonPress', byteIndex: 39, value })
      break
    }
    // Word-based macros
    case 'SetKeypad': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'wordChanged', wordIndex: 5, value })
      break
    }
    case 'ConfigX': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'wordChanged', wordIndex: 2, value })
      break
    }
    case 'ConfigY': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'wordChanged', wordIndex: 3, value })
      break
    }
    case 'ConfigWellTime_hold': {
      const value = resolveMacroArg(args)
      if (value !== null) ws.send({ type: 'wordChanged', wordIndex: 4, value })
      break
    }
    // String-based macros
    case 'ConfigTitle': {
      const value = String(args || '')
      ws.send({ type: 'stringInput', stringIndex: 1, value })
      break
    }
    case 'ConfigMinValue': {
      const value = String(args || '')
      ws.send({ type: 'stringInput', stringIndex: 2, value })
      break
    }
    case 'ConfigMaxValue': {
      const value = String(args || '')
      ws.send({ type: 'stringInput', stringIndex: 3, value })
      break
    }
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
 * Generated from Amulet.h #define values (all 1167 entries).
 */
const MACRO_TABLE: Record<string, number> = {
  SCREEN_NONE: 0,
  SCREEN_DAILY: 1,
  SCREEN_BKG: 2,
  SCREEN_CHAMBERVOLTS: 3,
  SCREEN_ACCURACY: 4,
  SCREEN_GEOMETRY: 5,
  SCREEN_LINEARITY: 6,
  SCREEN_ONESTRIP: 7,
  SCREEN_TWOSTRIP: 8,
  SCREEN_HMPAO: 9,
  SCREEN_MAG3: 10,
  SCREEN_MOLY: 11,
  SCREEN_INVENTORY: 12,
  SCREEN_NUCLIDE: 13,
  SCREEN_TIME: 14,
  SCREEN_ADD_INVENTORY: 15,
  SCREEN_KEYPAD: 16,
  SCREEN_KEYBOARD: 17,
  SCREEN_STUDY: 18,
  SCREEN_ACTIVITY: 19,
  SCREEN_MEASURE: 20,
  SCREEN_WITHDRAW_INVENTORY: 21,
  SCREEN_DELETE_INVENTORY: 22,
  SCREEN_KIT_INVENTORY: 23,
  SCREEN_STUDY2: 24,
  SCREEN_INFO: 25,
  SCREEN_SETUP: 26,
  SCREEN_SETUP_SOURCES: 27,
  SCREEN_SETUP_MOLY: 28,
  SCREEN_SETUP_NUCLIDE: 29,
  SCREEN_SETUP_LINEARITY: 30,
  SCREEN_SETUP_REMOTE: 31,
  SCREEN_MAIN_SCREEN: 32,
  SCREEN_SETUP_CALNUM: 33,
  SCREEN_ERROR_MSG: 34,
  SCREEN_WARNING_MSG: 35,
  SCREEN_NOTIFICATION_MSG: 36,
  SCREEN_SETUP_LINEARITY_STANDARD: 37,
  SCREEN_SETUP_LINEARITY_LINEATOR: 38,
  SCREEN_SETUP_LINEARITY_CALICHECK: 39,
  SCREEN_DOSE_TABLE: 40,
  SCREEN_SELECT_CHAMBER: 41,
  SCREEN_INITIAL: 42,
  SCREEN_OTHER: 43,
  SCREEN_WELL_MAIN_SCREEN: 44,
  SCREEN_WELL_MEASUREMENT: 45,
  SCREEN_WELL_AUTOCALIBRATE: 46,
  SCREEN_WELL_PEAKS: 47,
  SCREEN_WELL_ADVANCED_SETUP: 48,
  SCREEN_WELL_SETUP_NUCLIDE: 49,
  SCREEN_WELL_SETUP_SEALED: 50,
  SCREEN_WELL_SETUP_SEALED2: 51,
  SCREEN_WELL_SETUP_TEST_SOURCE: 52,
  SCREEN_WELL_SETUP_TRIGGER_LEVEL: 53,
  SCREEN_WELL_SETUP_EFFICIENCIES: 54,
  SCREEN_WELL_EDIT_EFFICIENCIES: 55,
  SCREEN_WELL_MANUAL: 56,
  SCREEN_WELL_MEASURE_EFFICIENCY: 57,
  SCREEN_GEOMETRY_REPORT: 58,
  SCREEN_AUTOCONSTANCY: 59,
  SCREEN_SLEEP: 60,
  SCREEN_ENHANCED: 61,
  SCREEN_FACTORY: 62,
  SCREEN_SETUPCHAMBER: 63,
  SCREEN_DELETE_ALL_INVENTORY: 64,
  SCREEN_WELL_MDA_TEST: 65,
  SCREEN_WELL_MEASUREMENTS: 66,
  SCREEN_WELL_EDIT_FULL_EFFICIENCY: 67,
  SCREEN_EDIT_CALIB_SERIAL: 68,
  SCREEN_WELL_WIPE_REPORT: 69,
  SCREEN_WELL_SCHILLING: 70,
  SCREEN_WELL_PLASMA: 71,
  SCREEN_WELL_RBC: 72,
  SCREEN_WELL_FACTORY: 73,
  SCREEN_WELL_PLASMA_ANALYSIS: 74,
  SCREEN_WELL_RBC_ANALYSIS: 75,
  SCREEN_WELL_QC_TESTS: 76,
  SCREEN_WELL_SYS_TEST_RESULT: 77,
  SCREEN_WELL_CHI_TEST: 78,
  SCREEN_SCHILLING_ANALYSIS: 79,
  SCREEN_WELL_CHANGE_ROI: 80,
  SCREEN_WELL_GENERAL_ANALYSIS: 81,
  SCREEN_SETUP_RHOTKEYS: 82,
  SCREEN_SETUP_PHOTKEYS: 83,
  SCREEN_REMOTE: 84,
  SCREEN_DELETE_LINEATOR: 85,
  SCREEN_DELETE_CALICHECK: 86,
  SCREEN_SELECT_LINEARITY: 87,
  SCREEN_QC: 88,
  SCREEN_LOW_LEVEL: 89,
  SCREEN_WELL_STABILITY_TEST: 90,
  SCREEN_SETUP_COMMUNICATIONS: 91,
  SCREEN_PCQC: 92,
  SCREEN_ETHERNET: 93,
  SCREEN_WELL_SETUP_TYPES: 94,
  SCREEN_WELL_SETUP_LOCATIONS: 95,
  SCREEN_WELL_SETUP_ADDEDIT_LOC: 96,
  SCREEN_WELL_SETUP_DELETE_LOC: 97,
  SCREEN_WELL_WIPE_LIST: 98,
  SCREEN_ERROR_MSG_LARGE: 99,
  SCREEN_WELL_SPECTRUM: 100,
  SCREEN_WELL_SEARCH_WIPE: 101,
  SCREEN_GENERIC_ITEMS: 102,
  SCREEN_INACTIVATE: 103,
  SCREEN_WELL_SEARCH_SYSTEM_TEST: 104,
  SCREEN_WELL_SEARCH_MDA: 105,
  SCREEN_WELL_MDA_ANALYSIS: 106,
  SCREEN_WELL_SEARCH_CHI: 107,
  SCREEN_WELL_CHI_ANALYSIS: 108,
  SCREEN_TEST_IDENT: 109,
  SCREEN_WELL_SEARCH_SCHILLING: 110,
  SCREEN_WELL_SCHILLING_REPORT: 111,
  SCREEN_WELL_SEARCH_PLASMA: 112,
  SCREEN_WELL_PLASMA_REPORT: 113,
  SCREEN_WELL_SEARCH_RBC: 114,
  SCREEN_WELL_RBC_REPORT: 115,
  SCREEN_WELL_SEARCH_AUTOCAL: 116,
  SCREEN_WELL_AUTOCAL_REPORT: 117,
  SCREEN_WELL_FACTORY_DETECTORS: 118,
  SCREEN_WELL_SETUP_USER_NUCLIDES: 119,
  SCREEN_WELL_ADD_EDIT_USER_NUCLIDES: 120,
  SCREEN_WELL_LAB_TESTS: 121,
  SCREEN_WELL_REPORTS: 122,
  SCREEN_LOGIN: 123,
  SCREEN_SECURITY: 124,
  SCREEN_ADDEDIT_USER: 125,
  SCREEN_SETUP_BIOASSAY: 126,
  SCREEN_SETUP_BIOASSAY_ENTER_EFF: 127,
  SCREEN_SETUP_BIOASSAY_MEASURE_EFF: 128,
  SCREEN_WELL_BIOASSAY: 129,
  SCREEN_WELL_BIOASSAY_ANALYSIS: 130,
  SCREEN_WELL_SEARCH_BIOASSAY: 131,
  SCREEN_SETUP_BIOASSAY_ENTER_ROI: 132,
  SCREEN_SETUP_THYROID_UPTAKE_PROTOCOL: 133,
  SCREEN_ADDEDIT_THYROID_UPTAKE_PROTOCOL: 134,
  SCREEN_WELL_THYROID_UPTAKE: 135,
  SCREEN_ADDEDIT_THYROID_UPTAKE_TEST: 136,
  SCREEN_WELL_THYROID_UPTAKE_TEST: 137,
  SCREEN_WELL_THYROID_UPTAKE_ENTER_ADMIN: 138,
  SCREEN_WELL_THYROID_UPTAKE_MEAS_DOSE: 139,
  SCREEN_WELL_THYROID_UPTAKE_MEAS_PAT: 140,
  SCREEN_WELL_THYROID_UPTAKE_ENTER_NORMAL: 141,
  SCREEN_WELL_REPEATED_MEASUREMENT: 142,
  SCREEN_WELL_SEARCH_REPEATED_MEASUREMENT: 143,
  SCREEN_WELL_RBC_SURVIVAL: 144,
  SCREEN_ADDEDIT_RBC_SURVIVAL_TEST: 145,
  SCREEN_WELL_RBC_SURVIVAL_TEST: 146,
  SCREEN_WELL_RBC_SURVIVAL_MEASUREMENT: 147,
  SCREEN_WELL_RBC_SURVIVAL_ENTER_NORMAL: 148,
  SCREEN_AUTOLINEARITY: 149,
  SCREEN_AUTOLINEARITYTEST: 150,
  SCREEN_GENERIC_YES_NO: 151,
  SCREEN_AUTOLINEARITYSEARCH: 152,
  SCREEN_CHAMBER_REPORTS: 153,
  SCREEN_CHAMBER_SEARCH: 154,
  SCREEN_CHAMBER_DAILY_TEST_VIEW: 155,
  SCREEN_CHAMBER_ZERO_TEST_VIEW: 156,
  SCREEN_CHAMBER_BACKGROUND_TEST_VIEW: 157,
  SCREEN_CHAMBER_VOLTAGE_TEST_VIEW: 158,
  SCREEN_CHAMBER_ACCURACY_TEST_VIEW: 159,
  SCREEN_CHAMBER_AUTOCONSTANCY_TEST_VIEW: 160,
  SCREEN_CHAMBER_HALFLIFE_CALC: 161,
  SCREEN_SETUP_KEY: 162,
  SCREEN_SETUP_PASSWORD: 163,
  SET_NUCLIDE_MEASURE: 1,
  SET_NUCLIDE_ADD_INV: 2,
  SET_NUCLIDE_DECAY_CALC: 3,
  SET_NUCLIDE_HOTKEY: 4,
  SET_NUCLIDE_USER_CAL: 5,
  SET_NUCLIDE_REMOTE_R1: 6,
  SET_NUCLIDE_REMOTE_R2: 7,
  SET_NUCLIDE_REMOTE_R3: 8,
  SET_NUCLIDE_REMOTE_R4: 9,
  SET_NUCLIDE_REMOTE_R5: 10,
  SET_NUCLIDE_REMOTE_R6: 11,
  SET_NUCLIDE_REMOTE_R7: 12,
  SET_NUCLIDE_REMOTE_R8: 13,
  SET_NUCLIDE_REMOTE_P1: 14,
  SET_NUCLIDE_REMOTE_P2: 15,
  SET_NUCLIDE_REMOTE_P3: 16,
  SET_NUCLIDE_REMOTE_P4: 17,
  SET_NUCLIDE_REMOTE_P5: 18,
  SET_NUCLIDE_REMOTE_P6: 19,
  SET_NUCLIDE_REMOTE_P7: 20,
  SET_NUCLIDE_REMOTE_P8: 21,
  SET_NUCLIDE_DOSE_TABLE: 22,
  SET_NUCLIDE_CONST_C1: 23,
  SET_NUCLIDE_CONST_C2: 24,
  SET_NUCLIDE_CONST_C3: 25,
  SET_NUCLIDE_CONST_C4: 26,
  SET_NUCLIDE_CONST_C5: 27,
  SET_NUCLIDE_CONST_C6: 28,
  SET_NUCLIDE_CONST_C7: 29,
  SET_NUCLIDE_CONST_C8: 30,
  SET_NUCLIDE_CONST_C9: 31,
  SET_NUCLIDE_CONST_C10: 32,
  SET_NUCLIDE_CONST_C11: 33,
  SET_NUCLIDE_CONST_C12: 34,
  SET_NUCLIDE_LINEAR_STD: 35,
  SET_NUCLIDE_WIPE1: 36,
  SET_NUCLIDE_WIPE2: 37,
  SET_NUCLIDE_WIPE3: 38,
  SET_NUCLIDE_WIPE4: 39,
  SET_NUCLIDE_WIPE5: 40,
  SET_NUCLIDE_WIPE6: 41,
  SET_NUCLIDE_WIPE7: 42,
  SET_NUCLIDE_WIPE8: 43,
  SET_NUCLIDE_WIPE9: 44,
  SET_NUCLIDE_WIPE10: 45,
  SET_NUCLIDE_UNRESTRICTED1: 46,
  SET_NUCLIDE_UNRESTRICTED2: 47,
  SET_NUCLIDE_UNRESTRICTED3: 48,
  SET_NUCLIDE_UNRESTRICTED4: 49,
  SET_NUCLIDE_UNRESTRICTED5: 50,
  SET_NUCLIDE_UNRESTRICTED6: 51,
  SET_NUCLIDE_UNRESTRICTED7: 52,
  SET_NUCLIDE_UNRESTRICTED8: 53,
  SET_NUCLIDE_UNRESTRICTED9: 54,
  SET_NUCLIDE_UNRESTRICTED10: 55,
  SET_NUCLIDE_SEALED1: 56,
  SET_NUCLIDE_SEALED2: 57,
  SET_NUCLIDE_SEALED3: 58,
  SET_NUCLIDE_SEALED4: 59,
  SET_NUCLIDE_SEALED5: 60,
  SET_NUCLIDE_SEALED6: 61,
  SET_NUCLIDE_SEALED7: 62,
  SET_NUCLIDE_SEALED8: 63,
  SET_NUCLIDE_SEALED9: 64,
  SET_NUCLIDE_SEALED10: 65,
  SET_NUCLIDE_SEALED11: 66,
  SET_NUCLIDE_SEALED12: 67,
  SET_NUCLIDE_SEALED13: 68,
  SET_NUCLIDE_SEALED14: 69,
  SET_NUCLIDE_SEALED15: 70,
  SET_NUCLIDE_SEALED16: 71,
  SET_NUCLIDE_SEALED17: 72,
  SET_NUCLIDE_SEALED18: 73,
  SET_NUCLIDE_SEALED19: 74,
  SET_NUCLIDE_SEALED20: 75,
  SET_NUCLIDE_SEALED21: 76,
  SET_NUCLIDE_SEALED22: 77,
  SET_NUCLIDE_SEALED23: 78,
  SET_NUCLIDE_SEALED24: 79,
  SET_NUCLIDE_SEALED25: 80,
  SET_NUCLIDE_SEALED26: 81,
  SET_NUCLIDE_SEALED27: 82,
  SET_NUCLIDE_SEALED28: 83,
  SET_NUCLIDE_SEALED29: 84,
  SET_NUCLIDE_SEALED30: 85,
  SET_NUCLIDE_SEALED31: 86,
  SET_NUCLIDE_SEALED32: 87,
  SET_NUCLIDE_SEALED33: 88,
  SET_NUCLIDE_SEALED34: 89,
  SET_NUCLIDE_SEALED35: 90,
  SET_NUCLIDE_SEALED36: 91,
  SET_NUCLIDE_SEALED37: 92,
  SET_NUCLIDE_SEALED38: 93,
  SET_NUCLIDE_SEALED39: 94,
  SET_NUCLIDE_SEALED40: 95,
  SET_NUCLIDE_SEALED41: 96,
  SET_NUCLIDE_SEALED42: 97,
  SET_NUCLIDE_SEALED43: 98,
  SET_NUCLIDE_SEALED44: 99,
  SET_NUCLIDE_SEALED45: 100,
  SET_NUCLIDE_SEALED46: 101,
  SET_NUCLIDE_SEALED47: 102,
  SET_NUCLIDE_SEALED48: 103,
  SET_NUCLIDE_SEALED49: 104,
  SET_NUCLIDE_SEALED50: 105,
  SET_NUCLIDE_SEALED51: 106,
  SET_NUCLIDE_SEALED52: 107,
  SET_NUCLIDE_SEALED53: 108,
  SET_NUCLIDE_SEALED54: 109,
  SET_NUCLIDE_SEALED55: 110,
  SET_NUCLIDE_SEALED56: 111,
  SET_NUCLIDE_SEALED57: 112,
  SET_NUCLIDE_SEALED58: 113,
  SET_NUCLIDE_SEALED59: 114,
  SET_NUCLIDE_SEALED60: 115,
  SET_NUCLIDE_SEALED61: 116,
  SET_NUCLIDE_SEALED62: 117,
  SET_NUCLIDE_SEALED63: 118,
  SET_NUCLIDE_SEALED64: 119,
  SET_NUCLIDE_SEALED65: 120,
  SET_NUCLIDE_SEALED66: 121,
  SET_NUCLIDE_SEALED67: 122,
  SET_NUCLIDE_SEALED68: 123,
  SET_NUCLIDE_SEALED69: 124,
  SET_NUCLIDE_SEALED70: 125,
  SET_NUCLIDE_SEALED71: 126,
  SET_NUCLIDE_SEALED72: 127,
  SET_NUCLIDE_SEALED73: 128,
  SET_NUCLIDE_SEALED74: 129,
  SET_NUCLIDE_SEALED75: 130,
  SET_NUCLIDE_SEALED76: 131,
  SET_NUCLIDE_SEALED77: 132,
  SET_NUCLIDE_SEALED78: 133,
  SET_NUCLIDE_SEALED79: 134,
  SET_NUCLIDE_SEALED80: 135,
  SET_NUCLIDE_SEALED81: 136,
  SET_NUCLIDE_SEALED82: 137,
  SET_NUCLIDE_SEALED83: 138,
  SET_NUCLIDE_SEALED84: 139,
  SET_NUCLIDE_SEALED85: 140,
  SET_NUCLIDE_SEALED86: 141,
  SET_NUCLIDE_SEALED87: 142,
  SET_NUCLIDE_SEALED88: 143,
  SET_NUCLIDE_SEALED89: 144,
  SET_NUCLIDE_SEALED90: 145,
  SET_NUCLIDE_SEALED91: 146,
  SET_NUCLIDE_SEALED92: 147,
  SET_NUCLIDE_SEALED93: 148,
  SET_NUCLIDE_SEALED94: 149,
  SET_NUCLIDE_SEALED95: 150,
  SET_NUCLIDE_SEALED96: 151,
  SET_NUCLIDE_SEALED97: 152,
  SET_NUCLIDE_SEALED98: 153,
  SET_NUCLIDE_SEALED99: 154,
  SET_NUCLIDE_MDA: 155,
  SET_NUCLIDE_ROI: 156,
  SET_NUCLIDE_R_M1: 157,
  SET_NUCLIDE_R_M2: 158,
  SET_NUCLIDE_R_M3: 159,
  SET_NUCLIDE_R_M4: 160,
  SET_NUCLIDE_R_M5: 161,
  SET_NUCLIDE_R_M6: 162,
  SET_NUCLIDE_R_M7: 163,
  SET_NUCLIDE_R_M8: 164,
  SET_NUCLIDE_R_N1: 165,
  SET_NUCLIDE_R_N2: 166,
  SET_NUCLIDE_R_N3: 167,
  SET_NUCLIDE_R_N4: 168,
  SET_NUCLIDE_R_N5: 169,
  SET_NUCLIDE_R_N6: 170,
  SET_NUCLIDE_R_N7: 171,
  SET_NUCLIDE_R_N8: 172,
  SET_NUCLIDE_R_N9: 173,
  SET_NUCLIDE_R_N10: 174,
  SET_NUCLIDE_R_N11: 175,
  SET_NUCLIDE_R_N12: 176,
  SET_NUCLIDE_R_N13: 177,
  SET_NUCLIDE_R_N14: 178,
  SET_NUCLIDE_R_N15: 179,
  SET_NUCLIDE_R_N16: 180,
  SET_NUCLIDE_R_N17: 181,
  SET_NUCLIDE_R_N18: 182,
  SET_NUCLIDE_R_N19: 183,
  SET_NUCLIDE_R_N20: 184,
  SET_NUCLIDE_P_M1: 185,
  SET_NUCLIDE_P_M2: 186,
  SET_NUCLIDE_P_M3: 187,
  SET_NUCLIDE_P_M4: 188,
  SET_NUCLIDE_P_M5: 189,
  SET_NUCLIDE_P_M6: 190,
  SET_NUCLIDE_P_M7: 191,
  SET_NUCLIDE_P_M8: 192,
  SET_NUCLIDE_P_N1: 193,
  SET_NUCLIDE_P_N2: 194,
  SET_NUCLIDE_P_N3: 195,
  SET_NUCLIDE_P_N4: 196,
  SET_NUCLIDE_P_N5: 197,
  SET_NUCLIDE_P_N6: 198,
  SET_NUCLIDE_P_N7: 199,
  SET_NUCLIDE_P_N8: 200,
  SET_NUCLIDE_P_N9: 201,
  SET_NUCLIDE_P_N10: 202,
  SET_NUCLIDE_P_N11: 203,
  SET_NUCLIDE_P_N12: 204,
  SET_NUCLIDE_P_N13: 205,
  SET_NUCLIDE_P_N14: 206,
  SET_NUCLIDE_P_N15: 207,
  SET_NUCLIDE_P_N16: 208,
  SET_NUCLIDE_P_N17: 209,
  SET_NUCLIDE_P_N18: 210,
  SET_NUCLIDE_P_N19: 211,
  SET_NUCLIDE_P_N20: 212,
  SET_NUCLIDE_AUTOLIN: 213,
  SET_NUCLIDE_HALFLIFE_CALC: 214,
  SET_CLEAR_ADD_INV: 1,
  SET_CLEAR_WITHDRAW_INV: 2,
  SET_CLEAR_KIT_INV: 3,
  SET_CLEAR_INFO: 4,
  SET_CLEAR_SETUP: 5,
  SET_CLEAR_SETUPSOURCES: 6,
  SET_CLEAR_SETUPMOLY: 7,
  SET_CLEAR_SETUPNUCLIDE: 8,
  SET_CLEAR_SETUPLINEARITY: 9,
  SET_CLEAR_SETUPREMOTE: 10,
  SET_CLEAR_SETUPCALNUM: 11,
  SET_CLEAR_SETUPLINEARITYSTANDARD: 12,
  SET_CLEAR_SETUPLINEARITYLINEATOR: 13,
  SET_CLEAR_SETUPLINEARITYCALICHECK: 14,
  SET_CLEAR_DOSETABLE: 15,
  SET_CLEAR_WELLMAIN: 16,
  SET_CLEAR_WELLAUTOCALIBRATE: 17,
  SET_CLEAR_WELLMEASUREMENT: 18,
  SET_CLEAR_WELLSETUPNUCLIDE: 19,
  SET_CLEAR_WELLSETUPSEALED: 20,
  SET_CLEAR_WELLSETUPSEALED2: 21,
  SET_CLEAR_WELLSETUPTESTSOURCE: 22,
  SET_CLEAR_WELLSETUPTRIGGERLEVEL: 23,
  SET_CLEAR_WELLSETUPEFFICIENCIES: 24,
  SET_CLEAR_WELLEDITEFFICIENCIES: 25,
  SET_CLEAR_WELLMEASUREEFFICIENCY: 26,
  SET_CLEAR_DAILY: 27,
  SET_CLEAR_ACCURACY: 28,
  SET_CLEAR_AUTOCONSTANCY: 29,
  SET_CLEAR_LINEARITY: 30,
  SET_CLEAR_ONESTRIP: 31,
  SET_CLEAR_TWOSTRIP: 32,
  SET_CLEAR_HMPAO: 33,
  SET_CLEAR_MAG3: 34,
  SET_CLEAR_MOLY: 35,
  SET_CLEAR_FACTORY: 36,
  SET_CLEAR_SETUPCHAMBER: 37,
  SET_CLEAR_WELLMEASUREMENTS: 38,
  SET_CLEAR_WELLEDITFULLEFFICIENCY: 39,
  SET_CLEAR_EDITCALIBSERIAL: 40,
  SET_CLEAR_WELLSCHILLING: 41,
  SET_CLEAR_WELLPLASMA: 42,
  SET_CLEAR_WELLRBC: 43,
  SET_CLEAR_WELLFACTORY: 44,
  SET_CLEAR_WELLMDA: 45,
  SET_CLEAR_WELLCHI: 46,
  SET_CLEAR_SETUPRHOTKEYS: 47,
  SET_CLEAR_SETUPPHOTKEYS: 48,
  SET_CLEAR_LOWLEVEL: 49,
  SET_CLEAR_WELLSTABILITYTEST: 50,
  SET_CLEAR_SETUPCOMMUNICATIONS: 51,
  SET_CLEAR_ETHERNET: 52,
  SET_CLEAR_WELLSETUPBACKGROUNDTYPE: 53,
  SET_CLEAR_WELLSETUPWORKAREATYPE: 54,
  SET_CLEAR_WELLSETUPUNRESTRICTIVETYPE: 55,
  SET_CLEAR_WELLSETUPSEALEDTYPE: 56,
  SET_CLEAR_WELLSETUPPACKAGETYPE: 57,
  SET_CLEAR_WELLSETUPADDLOCATION: 58,
  SET_CLEAR_WELLSETUPEDITLOCATION: 59,
  SET_CLEAR_WELLSETUPDELETELOCATION: 60,
  SET_CLEAR_WELLWIPELIST: 61,
  SET_CLEAR_WELLSEARCHWIPE: 62,
  SET_CLEAR_INACTIVATE: 63,
  SET_CLEAR_WELLSEARCHSYSTEMTEST: 64,
  SET_CLEAR_WELLSEARCHMDA: 65,
  SET_CLEAR_WELLSEARCHCHI: 66,
  SET_CLEAR_TESTIDENT: 67,
  SET_CLEAR_WELLSEARCHSCHILLING: 68,
  SET_CLEAR_WELLSEARCHPLASMA: 69,
  SET_CLEAR_WELLSEARCHRBC: 70,
  SET_CLEAR_WELLSEARCHAUTOCAL: 71,
  SET_CLEAR_WELLFACTORYDETECTORS: 72,
  SET_CLEAR_WELLSETUPUSERNUCLIDES: 73,
  SET_CLEAR_WELLADDEDITUSERNUCLIDES: 74,
  SET_CLEAR_LOGIN: 75,
  SET_CLEAR_SECURITY: 76,
  SET_CLEAR_ADDEDIT_USER: 77,
  SET_CLEAR_WELLSETUPADVANCED: 78,
  SET_CLEAR_SETUPADVANCED: 79,
  SET_CLEAR_SETUPBIOASSAY: 80,
  SET_CLEAR_SETUPBIOASSAYENTEREFF: 81,
  SET_CLEAR_SETUPBIOASSAYMEASUREEFF: 82,
  SET_CLEAR_WELLBIOASSAY: 83,
  SET_CLEAR_WELLBIOASSAYANALYSIS: 84,
  SET_CLEAR_WELLSEARCHBIOASSAY: 85,
  SET_CLEAR_SETUPBIOASSAYENTERROI: 86,
  SET_CLEAR_SETUPTHYROIDUPTAKEPROTOCOL: 87,
  SET_CLEAR_ADDEDITTHYROIDUPTAKEPROTOCOL: 88,
  SET_CLEAR_WELLTHYROIDUPTAKE: 89,
  SET_CLEAR_ADDEDITTHYROIDUPTAKETEST: 90,
  SET_CLEAR_WELLTHYROIDUPTAKETEST: 91,
  SET_CLEAR_WELLTHYROIDUPTAKEENTERADMIN: 92,
  SET_CLEAR_WELLTHYROIDUPTAKEMEASDOSE: 93,
  SET_CLEAR_WELLTHYROIDUPTAKEMEASPAT: 94,
  SET_CLEAR_WELLTHYROIDUPTAKEENTERNORMAL: 95,
  SET_CLEAR_WELLTHYROIDUPTAKEREPORT: 96,
  SET_CLEAR_WELLREPEATEDMEASUREMENT: 97,
  SET_CLEAR_WELLSEARCHREPEATEDMEASUREMENT: 98,
  SET_CLEAR_WELLRBCSURVIVAL: 99,
  SET_CLEAR_ADDEDITRBCSURVIVALTEST: 100,
  SET_CLEAR_WELLRBCSURVIVALTEST: 101,
  SET_CLEAR_WELLRBCSURVIVALMEASUREMENT: 102,
  SET_CLEAR_WELLRBCSURVIVALREPORT: 103,
  SET_CLEAR_WELLRBCSURVIVALENTERNORMAL: 104,
  SET_CLEAR_AUTOLINEARITYTEST: 105,
  SET_CLEAR_AUTOLINEARITYVIEW: 106,
  SET_CLEAR_AUTOLINEAIRTYSEARCH: 107,
  SET_CLEAR_CHAMBERDAILYTESTSEARCH: 108,
  SET_CLEAR_CHAMBERZEROSEARCH: 109,
  SET_CLEAR_CHAMBERBACKGROUNDSEARCH: 110,
  SET_CLEAR_CHAMBERVOLTAGESEARCH: 111,
  SET_CLEAR_CHAMBERACCURACYSEARCH: 112,
  SET_CLEAR_HALFLIFECALC: 113,
  SET_CLEAR_SETUPKEY: 114,
  SET_CLEAR_SETUPPASSWORD: 115,
  SET_KEYPAD_NONE: 0,
  SET_KEYPAD_CALNUM: 1,
  SET_KEYPAD_ADD_INV__ID: 2,
  SET_KEYPAD_ADD_INV__VOL: 3,
  SET_KEYPAD_ADD_INV__MOTC: 4,
  SET_KEYPAD_KIT_INV__ID: 5,
  SET_KEYPAD_KIT_INV__VOL: 6,
  SET_KEYPAD_SETUP_PASSWD: 7,
  SET_KEYPAD_TIME_PASSWD: 8,
  SET_KEYPAD_SETUP_MOLY_MO_TC_LIMIT: 9,
  SET_KEYPAD_SETUP_NUC_RCALNUM1: 10,
  SET_KEYPAD_SETUP_NUC_RCALNUM2: 11,
  SET_KEYPAD_SETUP_NUC_RCALNUM3: 12,
  SET_KEYPAD_SETUP_NUC_RCALNUM4: 13,
  SET_KEYPAD_SETUP_NUC_RCALNUM5: 14,
  SET_KEYPAD_SETUP_NUC_RCALNUM6: 15,
  SET_KEYPAD_SETUP_NUC_RCALNUM7: 16,
  SET_KEYPAD_SETUP_NUC_RCALNUM8: 17,
  SET_KEYPAD_SETUP_NUC_RCALNUM9: 18,
  SET_KEYPAD_SETUP_NUC_RCALNUM10: 19,
  SET_KEYPAD_SETUP_NUC_PCALNUM1: 20,
  SET_KEYPAD_SETUP_NUC_PCALNUM2: 21,
  SET_KEYPAD_SETUP_NUC_PCALNUM3: 22,
  SET_KEYPAD_SETUP_NUC_PCALNUM4: 23,
  SET_KEYPAD_SETUP_NUC_PCALNUM5: 24,
  SET_KEYPAD_SETUP_NUC_PCALNUM6: 25,
  SET_KEYPAD_SETUP_NUC_PCALNUM7: 26,
  SET_KEYPAD_SETUP_NUC_PCALNUM8: 27,
  SET_KEYPAD_SETUP_NUC_PCALNUM9: 28,
  SET_KEYPAD_SETUP_NUC_PCALNUM10: 29,
  SET_KEYPAD_SETUP_CALC_RCALNUM: 30,
  SET_KEYPAD_SETUP_CALC_PCALNUM: 31,
  SET_KEYPAD_SETUP_LINSTD_TOTAL: 32,
  SET_KEYPAD_SETUP_LINSTD_TEST1: 33,
  SET_KEYPAD_SETUP_LINSTD_TEST2: 34,
  SET_KEYPAD_SETUP_LINSTD_TEST3: 35,
  SET_KEYPAD_SETUP_LINSTD_TEST4: 36,
  SET_KEYPAD_SETUP_LINSTD_TEST5: 37,
  SET_KEYPAD_SETUP_LINSTD_TEST6: 38,
  SET_KEYPAD_SETUP_LINSTD_TEST7: 39,
  SET_KEYPAD_SETUP_LINSTD_TEST8: 40,
  SET_KEYPAD_SETUP_LINSTD_TEST9: 41,
  SET_KEYPAD_SETUP_LINSTD_TEST10: 42,
  SET_KEYPAD_SETUP_LINSTD_TEST11: 43,
  SET_KEYPAD_SETUP_LINSTD_TEST12: 44,
  SET_KEYPAD_SETUP_LINCALI_TOTAL: 45,
  SET_KEYPAD_DOSE_TABLE_VOLUME: 46,
  SET_KEYPAD_DOSE_TABLE_INTERVAL: 47,
  SET_KEYPAD_SOURCES_PASSWD: 48,
  SET_KEYPAD_LINEARITY_PASSWD: 49,
  SET_KEYPAD_MOLY_PASSWD: 50,
  SET_KEYPAD_MCA_GAIN1: 51,
  SET_KEYPAD_MCA_GAIN2: 52,
  SET_KEYPAD_MCA_ZERO_OFFSET: 53,
  SET_KEYPAD_WELL_SETUP_PASSWD: 54,
  SET_KEYPAD_MCA_THRESHOLD: 55,
  SET_KEYPAD_MCA_TIME: 56,
  SET_KEYPAD_MCA_HV: 57,
  SET_KEYPAD_MCA_ENERGY1: 58,
  SET_KEYPAD_MCA_ENERGY2: 59,
  SET_KEYPAD_MCA_ENERGY3: 60,
  SET_KEYPAD_MCA_WELL_EFF: 61,
  SET_KEYPAD_MCA_BETA_EFF: 62,
  SET_KEYPAD_MOLY_VOLUME: 63,
  SET_KEYPAD_FACTORY: 64,
  SET_KEYPAD_CALIB_SERIAL: 65,
  SET_KEYPAD_CHAMB_SERIAL: 66,
  SET_KEYPAD_CHAMB_RESPCORR: 67,
  SET_KEYPAD_CHAMB_NOMVOLTS: 68,
  SET_KEYPAD_WIPE_BKG_THRESHOLD: 69,
  SET_KEYPAD_WIPE_WORK_THRESHOLD: 70,
  SET_KEYPAD_WIPE_UNRESTRICTED_THRESHOLD: 71,
  SET_KEYPAD_WIPE_SEALED_THRESHOLD: 72,
  SET_KEYPAD_WIPE_FULL_EFFICIENCY: 73,
  SET_KEYPAD_WIPE_SEALED_SOURCE: 74,
  SET_KEYPAD_WIPE_WIPE_SOURCE: 75,
  SET_KEYPAD_WIPE_UNRESTRICTED_SOURCE: 76,
  SET_KEYPAD_WELL_SERIAL: 77,
  SET_KEYPAD_WELL_VOLTAGE: 78,
  SET_KEYPAD_WELL_THRESHOLD: 79,
  SET_KEYPAD_WELL_SHILLING_URINE_VOL: 80,
  SET_KEYPAD_WELL_SHILLING_ALIQOT_VOL: 81,
  SET_KEYPAD_WELL_SHILLING_DIL_FACTOR: 82,
  SET_KEYPAD_WELL_PLASMA_DIL_FACTOR: 83,
  SET_KEYPAD_WELL_PLASMA_SAMPLE_VOL: 84,
  SET_KEYPAD_WELL_PLASMA_HEMATOCRIT: 85,
  SET_KEYPAD_WELL_PLASMA_WEIGHT: 86,
  SET_KEYPAD_WELL_RBC_DOSE_HEMATOCRIT: 87,
  SET_KEYPAD_WELL_RBC_PAT_HEMATOCRIT: 88,
  SET_KEYPAD_WELL_RBC_PAT_WEIGHT: 89,
  SET_KEYPAD_WELL_MDA_PRECISION: 90,
  SET_KEYPAD_WELL_MDA_CORRECTION: 91,
  SET_KEYPAD_WELL_CHI_TIME: 92,
  SET_KEYPAD_WELL_CHI_REPS: 93,
  SET_KEYPAD_WELL_MEASURE_START_ROI: 94,
  SET_KEYPAD_WELL_MEASURE_END_ROI: 95,
  SET_KEYPAD_WELL_WIPE_PASSWD: 96,
  SET_KEYPAD_WELL_TEST_SOURCE: 97,
  SET_KEYPAD_WELL_STABILITY_TIME: 98,
  SET_KEYPAD_WELL_STABILITY_REPS: 99,
  SET_KEYPAD_WIPE_PACKAGE_THRESHOLD: 100,
  SET_KEYPAD_WIPE_COUNT_TIME: 101,
  SET_KEYPAD_WIPE_LOCATION_WORKAREA_THRESHOLD: 102,
  SET_KEYPAD_WIPE_LOCATION_UNRESTRICTED_THRESHOLD: 103,
  SET_KEYPAD_WIPE_LOCATION_SEALED_THRESHOLD: 104,
  SET_KEYPAD_WIPE_LOCATION_PACKAGE_THRESHOLD: 105,
  SET_KEYPAD_WIPE_LOCATION_COUNT_TIME: 106,
  SET_KEYPAD_PROBE700_SERIAL: 107,
  SET_KEYPAD_WELL700_SERIAL: 108,
  SET_KEYPAD_PROBE700_VOLTAGE: 109,
  SET_KEYPAD_WELL700_VOLTAGE: 110,
  SET_KEYPAD_PROBE700_THRESHOLD: 111,
  SET_KEYPAD_WELL700_THRESHOLD: 112,
  SET_KEYPAD_WELL_ADD_EDIT_NUCLIDES_ENERGY1: 113,
  SET_KEYPAD_WELL_ADD_EDIT_NUCLIDES_ENERGY2: 114,
  SET_KEYPAD_WELL_ADD_EDIT_NUCLIDES_ENERGY3: 115,
  SET_KEYPAD_WELL_ADD_EDIT_NUCLIDES_WELL_EFF: 116,
  SET_KEYPAD_WELL_ADD_EDIT_NUCLIDES_PROBE_EFF: 117,
  SET_KEYPAD_WELL_BIOASSAY_COUNT_TIME: 118,
  SET_KEYPAD_WELL_BIOASSAY_DISTANCE: 119,
  SET_KEYPAD_WELL_BIOASSAY_I131EFF: 120,
  SET_KEYPAD_WELL_BIOASSAY_I125EFF: 121,
  SET_KEYPAD_WELL_BIOASSAY_I123EFF: 122,
  SET_KEYPAD_WELL_BIOASSAY_I131_I125_Factor: 123,
  SET_KEYPAD_WELL_BIOASSAY_I123_I125_Factor: 124,
  SET_KEYPAD_WELL_RESET_BIOASSAY_DISTANCE: 125,
  SET_KEYPAD_WELL_BIOASSAY_I131_STARTROI: 126,
  SET_KEYPAD_WELL_BIOASSAY_I131_ENDROI: 127,
  SET_KEYPAD_WELL_BIOASSAY_I125_STARTROI: 128,
  SET_KEYPAD_WELL_BIOASSAY_I125_ENDROI: 129,
  SET_KEYPAD_WELL_BIOASSAY_I123_STARTROI: 130,
  SET_KEYPAD_WELL_BIOASSAY_I123_ENDROI: 131,
  SET_KEYPAD_WELL_TU_DEFAULT_COUNTTIME: 132,
  SET_KEYPAD_WELL_TU_DEFAULT_DISTANCE: 133,
  SET_KEYPAD_WELL_TU_I123_STARTROI: 134,
  SET_KEYPAD_WELL_TU_I123_ENDROI: 135,
  SET_KEYPAD_WELL_TU_I131_STARTROI: 136,
  SET_KEYPAD_WELL_TU_I131_ENDROI: 137,
  SET_KEYPAD_WELL_TU_TC99M_STARTROI: 138,
  SET_KEYPAD_WELL_TU_TC99M_ENDROI: 139,
  SET_KEYPAD_WELL_TU_ENTER_COUNT_TIME: 140,
  SET_KEYPAD_WELL_TU_ENTER_DISTANCE: 141,
  SET_KEYPAD_WELL_TU_ENTER_MULTIPLIER_INT: 142,
  SET_KEYPAD_WELL_TU_ENTER_MULTIPLIER_FLOAT: 143,
  SET_KEYPAD_WELL_TU_ENTER_NORM_HOUR1: 144,
  SET_KEYPAD_WELL_TU_ENTER_NORM_HOUR2: 145,
  SET_KEYPAD_WELL_TU_ENTER_NORM_HOUR3: 146,
  SET_KEYPAD_WELL_TU_ENTER_NORM_HOUR4: 147,
  SET_KEYPAD_WELL_TU_ENTER_NORM_MIN1: 148,
  SET_KEYPAD_WELL_TU_ENTER_NORM_MIN2: 149,
  SET_KEYPAD_WELL_TU_ENTER_NORM_MIN3: 150,
  SET_KEYPAD_WELL_TU_ENTER_NORM_MIN4: 151,
  SET_KEYPAD_WELL_TU_ENTER_NORM_MAX1: 152,
  SET_KEYPAD_WELL_TU_ENTER_NORM_MAX2: 153,
  SET_KEYPAD_WELL_TU_ENTER_NORM_MAX3: 154,
  SET_KEYPAD_WELL_TU_ENTER_NORM_MAX4: 155,
  SET_KEYPAD_WELL_REPEATED_MEAS_COUNTTIME: 156,
  SET_KEYPAD_WELL_REPEATED_MEAS_INTERVAL: 157,
  SET_KEYPAD_WELL_REPEATED_MEASUREMENTS: 158,
  SET_KEYPAD_WELL_RBC_SURVIVAL_ENTER_COUNTTIME: 159,
  SET_KEYPAD_WELL_RBC_SURVIVAL_ENTER_HEMATOCRIT: 160,
  SET_KEYPAD_WELL_RBC_SURVIVAL_ENTER_NORM_MIN: 161,
  SET_KEYPAD_WELL_RBC_SURVIVAL_ENTER_NORM_MAX: 162,
  SET_KEYPAD_TU_TEST_MEAS_PAT_LEG1: 201,
  SET_KEYPAD_TU_TEST_MEAS_PAT_LEG2: 202,
  SET_KEYPAD_TU_TEST_MEAS_PAT_NECK1: 203,
  SET_KEYPAD_TU_TEST_MEAS_PAT_NECK2: 204,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT1_1: 205,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT1_2: 206,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT2_1: 207,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT2_2: 208,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT3_1: 209,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT3_2: 210,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT4_1: 211,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT4_2: 212,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT5_1: 213,
  SET_KEYPAD_TU_TEST_MEAS_DOSE_COUNT5_2: 214,
  SET_KEYPAD_AUTOLIN_INTERVAL: 215,
  SET_KEYPAD_AUTOLIN_TOTAL: 216,
  SET_KEYPAD_FUTURE_DATE: 217,
  SET_KEYPAD_HALFLIFECALC_INTERVAL: 218,
  SET_KEYPAD_HALFLIFECALC_TOTAL: 219,
  SET_KEYPAD_CURRENT_PASSWORD: 220,
  SET_KEYPAD_NEW_PASSWORD: 221,
  SET_KEYPAD_CONFIRM_PASSWORD: 222,
  CONFIG_KEYPAD_DEFAULT: 0,
  CONFIG_KEYPAD_CALNUM: 1,
  CONFIG_KEYPAD_FLOAT: 2,
  CONFIG_KEYPAD_TIME: 3,
  CONFIG_KEYPAD_PASSWORD: 4,
  SET_KEYBOARD_ADD_INV__LOT: 1,
  SET_KEYBOARD_KIT_INV__LOT: 2,
  SET_KEYBOARD_SETUP_SOURCE_CO57: 3,
  SET_KEYBOARD_SETUP_SOURCE_CO60: 4,
  SET_KEYBOARD_SETUP_SOURCE_BA133: 5,
  SET_KEYBOARD_SETUP_SOURCE_CS137: 6,
  SET_KEYBOARD_SETUP_SOURCE_NA22: 7,
  SET_KEYBOARD_SETUP_NUC1: 8,
  SET_KEYBOARD_SETUP_NUC2: 9,
  SET_KEYBOARD_SETUP_NUC3: 10,
  SET_KEYBOARD_SETUP_NUC4: 11,
  SET_KEYBOARD_SETUP_NUC5: 12,
  SET_KEYBOARD_SETUP_NUC6: 13,
  SET_KEYBOARD_SETUP_NUC7: 14,
  SET_KEYBOARD_SETUP_NUC8: 15,
  SET_KEYBOARD_SETUP_NUC9: 16,
  SET_KEYBOARD_SETUP_NUC10: 17,
  SET_KEYBOARD_SETUP_ELEMENT1: 18,
  SET_KEYBOARD_SETUP_ELEMENT2: 19,
  SET_KEYBOARD_SETUP_ELEMENT3: 20,
  SET_KEYBOARD_SETUP_ELEMENT4: 21,
  SET_KEYBOARD_SETUP_ELEMENT5: 22,
  SET_KEYBOARD_SETUP_ELEMENT6: 23,
  SET_KEYBOARD_SETUP_ELEMENT7: 24,
  SET_KEYBOARD_SETUP_ELEMENT8: 25,
  SET_KEYBOARD_SETUP_ELEMENT9: 26,
  SET_KEYBOARD_SETUP_ELEMENT10: 27,
  SET_KEYBOARD_SETUP_WELL_TEST_SOURCE_SN: 28,
  SET_KEYBOARD_SETUP_LINEATOR_SN: 29,
  SET_KEYBOARD_SETUP_CALICHECK_SN: 30,
  SET_KEYBOARD_SETUP_LOCATION: 31,
  SET_KEYBOARD_INACTIVATE_COMMENT: 32,
  SET_KEYBOARD_TESTIDENT_TESTID: 33,
  SET_KEYBOARD_TESTIDENT_PATIENTID: 34,
  SET_KEYBOARD_TESTIDENT_FIRSTNAME: 35,
  SET_KEYBOARD_TESTIDENT_LASTNAME: 36,
  SET_KEYBOARD_TESTIDENT_SEX: 37,
  SET_KEYBOARD_TESTIDENT_PHYSICIAN: 38,
  SET_KEYBOARD_TESTIDENT_TECHID: 39,
  SET_KEYBOARD_ERASE_PASSPHRASE: 40,
  SET_KEYBOARD_WELL_ADD_EDIT_NUCLIDES_NUCLIDE: 41,
  SET_KEYBOARD_WELL_ADD_EDIT_NUCLIDES_ELEMENT: 42,
  SET_KEYBOARD_LOGIN_USERNAME: 43,
  SET_KEYBOARD_LOGIN_PASSWORD: 44,
  SET_KEYBOARD_ADDEDIT_USERNAME: 45,
  SET_KEYBOARD_ADDEDIT_PASSWORD: 46,
  SET_KEYBOARD_ADDEDIT_PASSWORD_ECHO: 47,
  SET_KEYBOARD_ADDEDIT_FIRSTNAME: 48,
  SET_KEYBOARD_ADDEDIT_LASTNAME: 49,
  SET_KEYBOARD_BIOASSAY_COMMENT: 50,
  SET_KEYBOARD_TU_PROTOCOL_NAME: 51,
  SET_KEYBOARD_TU_SEARCH_LASTNAME: 52,
  SET_KEYBOARD_TU_SEARCH_PATID: 53,
  SET_KEYBOARD_TU_ENTER_FIRSTNAME: 54,
  SET_KEYBOARD_TU_ENTER_LASTTNAME: 55,
  SET_KEYBOARD_TU_ENTER_PATID: 56,
  SET_KEYBOARD_TU_ENTER_SEX: 57,
  SET_KEYBOARD_TU_ENTER_PHYS: 58,
  SET_KEYBOARD_TU_ENTER_TECHID: 59,
  SET_KEYBOARD_TU_ENTER_LOTNUM: 60,
  SET_KEYBOARD_TU_ENTER_COMMENT: 61,
  SET_KEYBOARD_RBC_SURVIVAL_SEARCH_LASTNAME: 62,
  SET_KEYBOARD_RBC_SURVIVAL_SEARCH_PATID: 63,
  SET_KEYBOARD_RBC_SURVIVAL_ENTER_FIRSTNAME: 64,
  SET_KEYBOARD_RBC_SURVIVAL_ENTER_LASTNAME: 65,
  SET_KEYBOARD_RBC_SURVIVAL_ENTER_PATID: 66,
  SET_KEYBOARD_RBC_SURVIVAL_ENTER_SEX: 67,
  SET_KEYBOARD_RBC_SURVIVAL_ENTER_PHYS: 68,
  SET_KEYBOARD_RBC_SURVIVAL_ENTER_TECHID: 69,
  SET_KEYBOARD_RBC_SURVIVAL_ENTER_LOTNUM: 70,
  SET_KEYBOARD_RBC_SURVIVAL_ENTER_COMMENT: 71,
  SET_KEYBOARD_AUTOLIN_COMMENT: 72,
  SET_KEYBOARD_HALFLIFE_CALC_LOTNUM: 73,
  SET_KEYBOARD_SETUP_KEY: 74,
  SET_KEYBOARD_SETUP_SOURCE_GE68: 75,
  CONFIG_KEYBOARD_LOCK_LOWER: 0,
  CONFIG_KEYBOARD_LOCK_UPPER: 1,
  CONFIG_KEYBOARD_TEMP_UPPER: 2,
  SET_TIME_NONE: 0,
  SET_TIME_CLOCK: 1,
  SET_TIME_ADD_INV: 2,
  SET_TIME_WITHDRAW_INV: 3,
  SET_TIME_INFO_DECAY_FROM_TIME: 4,
  SET_TIME_INFO_DECAY_TO_TIME: 5,
  SET_TIME_SETUP_SOURCE_CO57: 6,
  SET_TIME_SETUP_SOURCE_CO60: 7,
  SET_TIME_SETUP_SOURCE_BA133: 8,
  SET_TIME_SETUP_SOURCE_CS137: 9,
  SET_TIME_SETUP_SOURCE_NA22: 10,
  SET_TIME_WELL_MEASURE_EFF_CAL_ON: 11,
  SET_TIME_FUTURE_TIME: 12,
  SET_TIME_WELL_STANDARD_SOURCE: 13,
  SET_TIME_WIPE_SEARCH_FROM: 14,
  SET_TIME_WIPE_SEARCH_TO: 15,
  SET_TIME_SYSTEM_TEST_SEARCH_FROM: 16,
  SET_TIME_SYSTEM_TEST_SEARCH_TO: 17,
  SET_TIME_MDA_SEARCH_FROM: 18,
  SET_TIME_MDA_SEARCH_TO: 19,
  SET_TIME_CHI_SEARCH_FROM: 20,
  SET_TIME_CHI_SEARCH_TO: 21,
  SET_TIME_TEST_IDENT_DOB: 22,
  SET_TIME_SCHILLING_SEARCH_FROM: 23,
  SET_TIME_SCHILLING_SEARCH_TO: 24,
  SET_TIME_PLASMA_SEARCH_FROM: 25,
  SET_TIME_PLASMA_SEARCH_TO: 26,
  SET_TIME_RBC_SEARCH_FROM: 27,
  SET_TIME_RBC_SEARCH_TO: 28,
  SET_TIME_AUTOCAL_SEARCH_FROM: 29,
  SET_TIME_AUTOCAL_SEARCH_TO: 30,
  SET_TIME_BIOASSAY_MEASURE_I131: 31,
  SET_TIME_BIOASSAY_MEASURE_I125: 32,
  SET_TIME_BIOASSAY_MEASURE_I123: 33,
  SET_TIME_BIOASSAY_SEARCH_FROM: 34,
  SET_TIME_BIOASSAY_SEARCH_TO: 35,
  SET_TIME_TU_ENTER_DOB: 36,
  SET_TIME_TU_ENTER_CALDATE: 37,
  SET_TIME_TU_ENTER_ADMINDATE: 38,
  SET_TIME_REP_MEAS_FROM: 39,
  SET_TIME_REP_MEAS_TO: 40,
  SET_TIME_REP_MEAS_EXPORT_FROM: 41,
  SET_TIME_REP_MEAS_EXPORT_TO: 42,
  SET_TIME_RBC_SURVIVAL_ENTER_DOB: 43,
  SET_TIME_RBC_SURVIVAL_ENTER_CALDATE: 44,
  SET_TIME_RBC_SURVIVAL_ENTER_INJDATE: 45,
  SET_TIME_AUTOLINEARITY_SEARCH_FROM: 46,
  SET_TIME_AUTOLINEARITY_SEARCH_TO: 47,
  SET_TIME_CHAMBER_SEARCH_FROM: 48,
  SET_TIME_CHAMBER_SEARCH_TO: 49,
  SET_TIME_SETUP_SOURCE_GE68: 50,
  SET_ACTIVITY_ADD_INV: 1,
  SET_ACTIVITY_WITHDRAW_INV: 2,
  SET_ACTIVITY_KIT_INV: 3,
  SET_ACTIVITY_INFO_CONVERT_CIBQ: 4,
  SET_ACTIVITY_INFO_FROM_ACTIVITY: 5,
  SET_ACTIVITY_INFO_TO_ACTIVITY: 6,
  SET_ACTIVITY_SETUP_SOURCE_CO57: 7,
  SET_ACTIVITY_SETUP_SOURCE_CO60: 8,
  SET_ACTIVITY_SETUP_SOURCE_BA133: 9,
  SET_ACTIVITY_SETUP_SOURCE_CS137: 10,
  SET_ACTIVITY_SETUP_SOURCE_NA22: 11,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE1: 12,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE2: 13,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE3: 14,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE4: 15,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE5: 16,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE6: 17,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE7: 18,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE8: 19,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE9: 20,
  SET_ACTIVITY_SETUP_NUCLIDE_HALFLIFE10: 21,
  SET_ACTIVITY_DOSE_TABLE_INITIAL: 22,
  SET_ACTIVITY_DOSE_TABLE_DOSE: 23,
  SET_ACTIVITY_WELL_MEASURE_EFF_CAL: 24,
  SET_ACTIVITY_WELL_TEST_SOURCE: 25,
  SET_ACTIVITY_WELL_ADD_EDIT_NUCLIDES_HALFLIFE: 26,
  SET_ACTIVITY_WELL_BIOASSAY_I131_THRESHOLD: 27,
  SET_ACTIVITY_WELL_BIOASSAY_I125_THRESHOLD: 28,
  SET_ACTIVITY_WELL_BIOASSAY_I123_THRESHOLD: 29,
  SET_ACTIVITY_WELL_BIOASSAY_I131_MEASURE: 30,
  SET_ACTIVITY_WELL_BIOASSAY_I125_MEASURE: 31,
  SET_ACTIVITY_WELL_BIOASSAY_I123_MEASURE: 32,
  SET_ACTIVITY_WELL_TU_CAL_ACTIVITY: 33,
  SET_ACTIVITY_WELL_RBC_SURVIVAL_CAL_ACTIVITY: 34,
  SET_ACTIVITY_SETUP_SOURCE_GE68: 35,
  CONFIG_ACTIVITY_CI: 1,
  CONFIG_ACTIVITY_BQ: 2,
  CONFIG_ACTIVITY_CI_AND_BQ: 3,
  CONFIG_ACTIVITY_TIME: 4,
  CONFIG_ACTIVITY_SYSTEM: 5,
  SET_MEASUREMENT_ADD_INV: 1,
  SET_MEASUREMENT_WITHDRAW_INV: 2,
  SET_MEASUREMENT_KIT_INV: 3,
  SET_MEASUREMENT_LINEATOR_1: 4,
  SET_MEASUREMENT_LINEATOR_2: 5,
  SET_MEASUREMENT_LINEATOR_3: 6,
  SET_MEASUREMENT_LINEATOR_4: 7,
  SET_MEASUREMENT_LINEATOR_5: 8,
  SET_MEASUREMENT_LINEATOR_6: 9,
  SET_MEASUREMENT_LINEATOR_7: 10,
  SET_MEASUREMENT_LINEATOR_8: 11,
  SET_MEASUREMENT_CALICHECK_1: 12,
  SET_MEASUREMENT_CALICHECK_2: 13,
  SET_MEASUREMENT_CALICHECK_3: 14,
  SET_MEASUREMENT_CALICHECK_4: 15,
  SET_MEASUREMENT_CALICHECK_5: 16,
  SET_MEASUREMENT_CALICHECK_6: 17,
  SET_MEASUREMENT_CALICHECK_7: 18,
  SET_MEASUREMENT_CALICHECK_8: 19,
  SET_MEASUREMENT_CALICHECK_9: 20,
  SET_MEASUREMENT_CALICHECK_10: 21,
  SET_MEASUREMENT_CALICHECK_11: 22,
  SET_MEASUREMENT_CALICHECK_12: 23,
  SET_MEASUREMENT_DOSE_TABLE: 24,
  SET_MEASUREMENT_ACCURACY_1: 25,
  SET_MEASUREMENT_ACCURACY_2: 26,
  SET_MEASUREMENT_ACCURACY_3: 27,
  SET_MEASUREMENT_ACCURACY_4: 28,
  SET_MEASUREMENT_ACCURACY_5: 29,
  SET_MEASUREMENT_ACCURACY_6: 30,
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
  SET_HTML_WELL_MAINSCREEN: 48,
  SET_HTML_BETA_MAINSCREEN: 49,
  SET_HTML_WELL_MEASUREMENT: 50,
  SET_HTML_WELL_AUTOCALIBRATE: 51,
  SET_HTML_WELL_PEAKS: 52,
  SET_HTML_WELL_ADVANCED_SETUP: 53,
  SET_HTML_WELL_SETUP_NUCLIDE: 54,
  SET_HTML_WELL_SETUP_SEALED: 55,
  SET_HTML_WELL_SETUP_SEALED2: 56,
  SET_HTML_WELL_SETUP_TEST_SOURCE: 57,
  SET_HTML_WELL_SETUP_TRIGGER_LEVEL: 58,
  SET_HTML_WELL_SETUP_EFFICIENCIES: 59,
  SET_HTML_WELL_EDIT_EFFICIENCIES: 60,
  SET_HTML_WELL_MANUAL: 61,
  SET_HTML_WELL_MEASURE_EFFICIENCY: 62,
  SET_HTML_AUTOCONSTANCY: 63,
  SET_HTML_SLEEP: 64,
  SET_HTML_FACTORY: 65,
  SET_HTML_SETUPCHAMBER: 66,
  SET_HTML_INVENTORYDELETEALL: 67,
  SET_HTML_WELL_MDA_TEST: 68,
  SET_HTML_WELL_MEASUREMENTS: 69,
  SET_HTML_WELL_EDIT_FULL_EFF: 70,
  SET_HTML_EDIT_CALIB_SERIAL: 71,
  SET_HTML_WELL_WIPE_REPORT: 72,
  SET_HTML_WELL_LAB_TESTS: 73,
  SET_HTML_WELL_SCHILLING: 74,
  SET_HTML_WELL_PLASMA: 75,
  SET_HTML_WELL_RBC: 76,
  SET_HTML_WELL_FACTORY: 77,
  SET_HTML_WELL_PLASMA_ANALYSIS: 78,
  SET_HTML_WELL_RBC_ANALYSIS: 79,
  SET_HTML_WELL_QC_TESTS: 80,
  SET_HTML_WELL_SYS_TEST_RESULT: 81,
  SET_HTML_WELL_CHI_TEST: 82,
  SET_HTML_WELL_SCHILLING_ANALYSIS: 83,
  SET_HTML_WELL_CHANGE_ROI: 84,
  SET_HTML_WELL_GENERAL_ANALYSIS: 85,
  SET_HTML_SETUP_RHOTKEYS: 86,
  SET_HTML_SETUP_PHOTKEYS: 87,
  SET_HTML_REMOTE: 88,
  SET_HTML_DELETE_LINEATOR: 89,
  SET_HTML_DELETE_CALICHECK: 90,
  SET_HTML_SELECT_LINEARITY: 91,
  SET_HTML_LOW_LEVEL: 92,
  SET_HTML_WELL_STABILITY_TEST: 93,
  SET_HTML_SETUPCOMMUNICATIONS: 94,
  SET_HTML_PCQC: 95,
  SET_HTML_ETHERNET: 96,
  SET_HTML_SETUP_WIPES: 97,
  SET_HTML_SETUP_TYPES: 98,
  SET_HTML_SETUP_LOCATION: 99,
  SET_HTML_SETUP_ADDEDITLOCATION: 100,
  SET_HTML_SETUP_DELETELOCATION: 101,
  SET_HTML_WELL_WIPE_LIST: 102,
  SET_HTML_ERRORMSG_LARGE: 103,
  SET_HTML_WELL_SPECTRUM: 104,
  SET_HTML_WELL_REPORTS: 105,
  SET_HTML_WELL_SEARCH_WIPE: 106,
  SET_HTML_GENERIC_ITEMS: 107,
  SET_HTML_INACTIVATE: 108,
  SET_HTML_WELL_SEARCH_SYSTEM_TEST: 109,
  SET_HTML_WELL_SEARCH_MDA: 110,
  SET_HTML_WELL_MDA_ANALYSIS: 111,
  SET_HTML_WELL_SEARCH_CHI: 112,
  SET_HTML_WELL_CHI_ANALYSIS: 113,
  SET_HTML_TEST_IDENT: 114,
  SET_HTML_WELL_SEARCH_SCHILLING: 115,
  SET_HTML_WELL_SCHILLING_REPORT: 116,
  SET_HTML_WELL_SEARCH_PLASMA: 117,
  SET_HTML_WELL_PLASMA_REPORT: 118,
  SET_HTML_WELL_SEARCH_RBC: 119,
  SET_HTML_WELL_RBC_REPORT: 120,
  SET_HTML_WELL_SEARCH_AUTOCAL: 121,
  SET_HTML_WELL_AUTOCAL_REPORT: 122,
  SET_HTML_WELL_FACTORY_DETECTORS: 123,
  SET_HTML_WELL_SETUP_USER_NUCLIDES: 124,
  SET_HTML_WELL_ADD_EDIT_USER_NUCLIDES: 125,
  SET_HTML_LOGIN: 126,
  SET_HTML_SECURITY: 127,
  SET_HTML_ADDEDIT_USER: 128,
  SET_HTML_SETUP_BIOASSAY: 129,
  SET_HTML_SETUP_BIOASSAY_ENTER_EFF: 130,
  SET_HTML_SETUP_BIOASSAY_MEASURE_EFF: 131,
  SET_HTML_WELL_BIOASSAY: 132,
  SET_HTML_WELL_BIOASSAY_ANALYSIS: 133,
  SET_HTML_WELL_SEARCH_BIOASSAY: 134,
  SET_HTML_SETUP_BIOASSAY_ENTER_ROI: 135,
  SET_HTML_SETUP_THYROID_UPTAKE_PROTOCOL: 136,
  SET_HTML_ADDEDIT_THYROID_UPTAKE_PROTOCOL: 137,
  SET_HTML_WELL_THYROID_UPTAKE: 138,
  SET_HTML_ADDEDIT_THYROID_UPTAKE_TEST: 139,
  SET_HTML_WELL_THYROID_UPTAKE_TEST: 140,
  SET_HTML_WELL_THYROID_UPTAKE_ENTER_ADMIN: 141,
  SET_HTML_WELL_THYROID_UPTAKE_MEAS_DOSE: 142,
  SET_HTML_WELL_THYROID_UPTAKE_MEAS_PAT: 143,
  SET_HTML_WELL_THYROID_UPTAKE_ENTER_NORMAL: 144,
  SET_HTML_WELL_REPEATED_MEASUREMENT: 145,
  SET_HTML_WELL_SEARCH_REPEATED_MEASUREMENT: 146,
  SET_HTML_WELL_RBC_SURVIVAL: 147,
  SET_HTML_ADDEDIT_RBC_SURVIVAL_TEST: 148,
  SET_HTML_WELL_RBC_SURVIVAL_TEST: 149,
  SET_HTML_WELL_RBC_SURVIVAL_MEASUREMENT: 150,
  SET_HTML_WELL_RBC_SURVIVAL_ENTER_NORMAL: 151,
  SET_HTML_SETTIME2_EUR: 152,
  SET_HTML_SETTIME2_JAP: 153,
  SET_HTML_AUTOLINEARITY: 154,
  SET_HTML_AUTOLINEARITY_TEST: 155,
  SET_HTML_GENERIC_YES_NO: 156,
  SET_HTML_AUTOLINEARITY_SEARCH: 157,
  SET_HTML_CHAMBER_REPORTS: 158,
  SET_HTML_CHAMBER_SEARCH: 159,
  SET_HTML_CHAMBER_DAILY_TEST_VIEW: 160,
  SET_HTML_CHAMBER_ZERO_TEST_VIEW: 161,
  SET_HTML_CHAMBER_BACKGROUND_TEST_VIEW: 162,
  SET_HTML_CHAMBER_VOLTAGE_TEST_VIEW: 163,
  SET_HTML_CHAMBER_ACCURACY_TEST_VIEW: 164,
  SET_HTML_CHAMBER_AUTOCONSTANCY_TEST_VIEW: 165,
  SET_HTML_CHAMBER_HALFLIFE_CALC: 166,
  SET_HOTKEY_NUCLIDE_1: 1,
  SET_HOTKEY_NUCLIDE_2: 2,
  SET_HOTKEY_NUCLIDE_3: 3,
  SET_HOTKEY_NUCLIDE_4: 4,
  SET_HOTKEY_NUCLIDE_5: 5,
  SET_HOTKEY_NUCLIDE_6: 6,
  SET_HOTKEY_NUCLIDE_7: 7,
  SET_HOTKEY_NUCLIDE_8: 8,
  CONFIG_NUCLIDE_ALL: 1,
  CONFIG_NUCLIDE_ONLY_CAL: 2,
  CONFIG_NUCLIDE_ONLY_ORIGINAL: 3,
  CONFIG_NUCLIDE_R_ONLY_ORIGINAL: 4,
  CONFIG_NUCLIDE_P_ONLY_ORIGINAL: 5,
  CONFIG_NUCLIDE_ENERGY1_AND_WELLEFF_ONLY: 6,
  CONFIG_NUCLIDE_ENERGY1_ONLY: 7,
  CONFIG_NUCLIDE_R_ONLY_ALL: 8,
  CONFIG_NUCLIDE_P_ONLY_ALL: 9,
  CONFIG_NUCLIDE_ENERGY1_AND_BETAEFF_ONLY: 10,
  CONFIG_NUCLIDE_ENERGY1_AND_PROBE700EFF_ONLY: 11,
  CONFIG_NUCLIDE_ENERGY1_AND_WELL700EFF_ONLY: 12,
  CONFIG_NUCLIDE_ENERGY1_AND_ACTIVEDETECTEFF_ONLY: 13,
  CONFIG_NUCLIDE_ENERGY1_AND_ACTIVEDETECTEFF_ONLY_SKEW_WELL: 14,
  CONFIG_NUCLIDE_ENERGY1_AND_ACTIVEDETECTEFF_ONLY_SKEW_PROBE: 15,
  CONFIG_NUCLIDE_ALL_INCLUDE_EFF: 16,
  CLEAR_NUCLIDE_HIDE: 0,
  CLEAR_NUCLIDE_SHOW: 1,
  SET_WELLMEASUREMENT_GENERAL: 1,
  SET_WELLMEASUREMENT_BACKGROUND: 2,
  SET_WELLMEASUREMENT_EFFICIENCY: 3,
  SET_WELLMEASUREMENT_WIPE: 4,
  SET_WELLMEASUREMENT_UNRESTRICTED: 5,
  SET_WELLMEASUREMENT_SEALED: 6,
  SET_WELLMEASUREMENT_SCHILLING_STD: 7,
  SET_WELLMEASUREMENT_SCHILLING_ALIQOT: 8,
  SET_WELLMEASUREMENT_PLASMA_STD: 9,
  SET_WELLMEASUREMENT_PLASMA_WHOLE: 10,
  SET_WELLMEASUREMENT_PLASMA_PLASMA: 11,
  SET_WELLMEASUREMENT_RBC_WHOLE_STD: 12,
  SET_WELLMEASUREMENT_RBC_PLASMA_STD: 13,
  SET_WELLMEASUREMENT_RBC_WHOLE_SAMPLE: 14,
  SET_WELLMEASUREMENT_RBC_PLASMA_SAMPLE: 15,
  SET_WELLMEASUREMENT_SYSTEM_TEST: 16,
  SET_WELLMEASUREMENT_MDA_TEST: 17,
  SET_WELLMEASUREMENT_CHI_TEST: 18,
  SET_WELLMEASUREMENT_STABILITY_TEST: 19,
  SET_WELLMEASUREMENT_BIOASSAY_MEASURE_I131: 20,
  SET_WELLMEASUREMENT_BIOASSAY_MEASURE_I125: 21,
  SET_WELLMEASUREMENT_BIOASSAY_MEASURE_I123: 22,
  SET_WELLMEASUREMENT_BIOASSAY: 23,
  SET_WELLMEASUREMENT_TU_PAT_LEG1: 24,
  SET_WELLMEASUREMENT_TU_PAT_LEG2: 25,
  SET_WELLMEASUREMENT_TU_PAT_NECK1: 26,
  SET_WELLMEASUREMENT_TU_PAT_NECK2: 27,
  SET_WELLMEASUREMENT_TU_DOSE1_VALUE1: 28,
  SET_WELLMEASUREMENT_TU_DOSE1_VALUE2: 29,
  SET_WELLMEASUREMENT_TU_DOSE2_VALUE1: 30,
  SET_WELLMEASUREMENT_TU_DOSE2_VALUE2: 31,
  SET_WELLMEASUREMENT_TU_DOSE3_VALUE1: 32,
  SET_WELLMEASUREMENT_TU_DOSE3_VALUE2: 33,
  SET_WELLMEASUREMENT_TU_DOSE4_VALUE1: 34,
  SET_WELLMEASUREMENT_TU_DOSE4_VALUE2: 35,
  SET_WELLMEASUREMENT_TU_DOSE5_VALUE1: 36,
  SET_WELLMEASUREMENT_TU_DOSE5_VALUE2: 37,
  SET_WELLMEASUREMENT_RBC_SURVIVAL_VALUE1: 38,
  SET_WELLMEASUREMENT_RBC_SURVIVAL_VALUE2: 39,
  SET_SPECIALFUNCTION_BACKGROUND_THRESHOLD_KEYPAD: 1,
  SET_SPECIALFUNCTION_WORK_THRESHOLD_KEYPAD: 2,
  SET_SPECIALFUNCTION_UNRESTRICTED_THRESHOLD_KEYPAD: 3,
  SET_SPECIALFUNCTION_SEALED_THRESHOLD_KEYPAD: 4,
  SET_SPECIALFUNCTION_PACKAGE_THRESHOLD_KEYPAD: 5,
  SET_SPECIALFUNCTION_LOCATION_THRESHOLD_KEYPAD: 6,
  SET_GENERICITEMS_WIPE_SEARCH_WIPETYPE: 1,
  SET_GENERICITEMS_DETECTOR700: 2,
  SET_GENERICITEMS_BRANDING: 3,
  SET_GENERICITEMS_AUTOCAL_SEARCH_DEVTYPE: 4,
  SET_GENERICITEMS_SYSTEMTEST_SEARCH_DEVTYPE: 5,
  SET_GENERICITEMS_MDA_SEARCH_DEVTYPE: 6,
  SET_GENERICITEMS_CHISQUARE_SEARCH_DEVTYPE: 7,
  SET_GENERICITEMS_SET_TU_COUNTING_METHOD: 8,
  SET_GENERICITEMS_SET_TU_DOSE_MEASUREMENT: 9,
  SET_GENERICITEMS_SET_TU_ISOTOPE: 10,
  SET_GENERICITEMS_SET_TU_DOSE_FORM: 11,
  SET_GENERICITEMS_SET_TU_PRE_DOSE: 12,
  SET_GENERICITEMS_DELETE_TU_TEST: 13,
  SET_GENERICITEMS_SET_TU_RESIDUAL: 14,
  SET_GENERICITEMS_REACTIVATE_TU: 15,
  SET_GENERICITEMS_LOCK_NUCLIDE_KEV: 16,
  SET_GENERICITEMS_REACTIVATE_RBC_SURVIVAL: 17,
  SET_GENERICITEMS_DELETE_RBC_SURVIVAL: 18,
  SET_GENERICITEMS_CONFIRM_UPDATE: 19,
  SET_GENERICITEMS_OVERWRITE_WITH_DEFAULT: 20,
  SET_GENERICITEMS_DATE_FORMAT: 21,
  SET_GENERICITEMS_USB_DRIVER: 22,
  SET_GENERICITEMS_FUTURE_DATE_ENTRY: 23,
  SET_GENERICITEMS_LANGUAGE: 24,
  SET_INVALIDATE_WIPE: 1,
  SET_INVALIDATE_WELLSYSTEMTEST: 2,
  SET_INVALIDATE_WELLMDA: 3,
  SET_INVALIDATE_WELLCHI: 4,
  SET_INVALIDATE_WELLSCHILLING: 5,
  SET_INVALIDATE_WELLPLASMA: 6,
  SET_INVALIDATE_WELLRBC: 7,
  SET_INVALIDATE_PROBE_BIOASSAY: 8,
  SET_INVALIDATE_TU_TEST: 9,
  SET_INVALIDATE_RBC_SURVIVAL: 10,
  SET_INVALIDATE_AUTO_LINEARITY: 11,
  SET_INVALIDATE_CHAMBER_DAILY: 12,
  SET_INVALIDATE_CHAMBER_ZERO: 13,
  SET_INVALIDATE_CHAMBER_BKG: 14,
  SET_INVALIDATE_CHAMBER_VOLT: 15,
  SET_INVALIDATE_CHAMBER_ACC: 16,
  SET_TESTIDENT_SCHILLING: 1,
  SET_TESTIDENT_PLASMA: 2,
  SET_TESTIDENT_RBC: 3,
  SET_GENERICYESNO_RESUME_AUTOLIN: 1,
  SET_GENERICYESNO_HOME_PAUSE_AUTOLIN: 2,
  SET_GENERICYESNO_BACK_PAUSE_AUTOLIN: 3,
  SET_GENERICYESNO_ABORT_AUTOLIN: 4,
  SET_GENERICYESNO_SAVE_AUTOLIN: 5,
  // Not in Amulet.h but referenced by WellSetup.json
  SCREEN_WELL_SETUP: 164,
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
  55: 54,   // SET_HTML_WELL_SETUP_SEALED → WELLSETUPSEALED_HTM
  56: 55,   // SET_HTML_WELL_SETUP_SEALED2 → WELLSETUPSEALED2_HTM
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
  // Values 72-166: page = SET_HTML_VALUE - 1 (consistent pattern from firmware HTM ordering)
  72:  71,  // SET_HTML_WELL_WIPE_REPORT → WellWipeReport
  73:  72,  // SET_HTML_WELL_LAB_TESTS → WellLabTests
  74:  73,  // SET_HTML_WELL_SCHILLING → WellSchilling
  75:  74,  // SET_HTML_WELL_PLASMA → WellPlasma
  76:  75,  // SET_HTML_WELL_RBC → WellRBC
  77:  76,  // SET_HTML_WELL_FACTORY → WellFactory
  78:  77,  // SET_HTML_WELL_PLASMA_ANALYSIS → WellPlasmaAnalysis
  79:  78,  // SET_HTML_WELL_RBC_ANALYSIS → WellRBCAnalysis
  80:  79,  // SET_HTML_WELL_QC_TESTS → WellQATests
  81:  80,  // SET_HTML_WELL_SYS_TEST_RESULT → WellSystemTestAnalysis
  82:  81,  // SET_HTML_WELL_CHI_TEST → WellChiTest
  83:  82,  // SET_HTML_WELL_SCHILLING_ANALYSIS → WellSchillingAnalysis
  84:  83,  // SET_HTML_WELL_CHANGE_ROI → WellChangeROI
  85:  84,  // SET_HTML_WELL_GENERAL_ANALYSIS → WellGeneralAnalysis
  86:  85,  // SET_HTML_SETUP_RHOTKEYS → SetupRHotkeys
  87:  86,  // SET_HTML_SETUP_PHOTKEYS → SetupPHotkeys
  88:  87,  // SET_HTML_REMOTE → Remote
  89:  88,  // SET_HTML_DELETE_LINEATOR → DeleteLineatorSettings
  90:  89,  // SET_HTML_DELETE_CALICHECK → DeleteCalicheckSettings
  91:  90,  // SET_HTML_SELECT_LINEARITY → SelectLinearity
  92:  91,  // SET_HTML_LOW_LEVEL → LowLevel
  93:  92,  // SET_HTML_WELL_STABILITY_TEST → WellStabilityTest
  94:  93,  // SET_HTML_SETUPCOMMUNICATIONS → SetupCommunications
  95:  94,  // SET_HTML_PCQC → PCQC
  96:  95,  // SET_HTML_ETHERNET → Ethernet
  97:  96,  // SET_HTML_SETUP_WIPES → WellSetupWipes
  98:  97,  // SET_HTML_SETUP_TYPES → WellSetupTypes
  99:  98,  // SET_HTML_SETUP_LOCATION → WellSetupLocations
  100: 99,  // SET_HTML_SETUP_ADDEDITLOCATION → WellSetupAddEditLocation
  101: 100, // SET_HTML_SETUP_DELETELOCATION → WellSetupDeleteLocation
  102: 101, // SET_HTML_WELL_WIPE_LIST → WellWipeList
  103: 102, // SET_HTML_ERRORMSG_LARGE → ErrorMsgLarge
  104: 103, // SET_HTML_WELL_SPECTRUM → WellSpectrum
  105: 104, // SET_HTML_WELL_REPORTS → WellReports
  106: 105, // SET_HTML_WELL_SEARCH_WIPE → WellWipeSearch
  107: 106, // SET_HTML_GENERIC_ITEMS → GenericItems
  108: 107, // SET_HTML_INACTIVATE → Inactivate
  109: 108, // SET_HTML_WELL_SEARCH_SYSTEM_TEST → WellSystemTestSearch
  110: 109, // SET_HTML_WELL_SEARCH_MDA → WellMDASearch
  111: 110, // SET_HTML_WELL_MDA_ANALYSIS → WellMDAAnalysis
  112: 111, // SET_HTML_WELL_SEARCH_CHI → WellChiSearch
  113: 112, // SET_HTML_WELL_CHI_ANALYSIS → WellChiAnalysis
  114: 113, // SET_HTML_TEST_IDENT → TestIdent
  115: 114, // SET_HTML_WELL_SEARCH_SCHILLING → WellSchillingSearch
  116: 115, // SET_HTML_WELL_SCHILLING_REPORT → WellSchillingReport
  117: 116, // SET_HTML_WELL_SEARCH_PLASMA → WellPlasmaSearch
  118: 117, // SET_HTML_WELL_PLASMA_REPORT → WellPlasmaReport
  119: 118, // SET_HTML_WELL_SEARCH_RBC → WellRBCSearch
  120: 119, // SET_HTML_WELL_RBC_REPORT → WellRBCReport
  121: 120, // SET_HTML_WELL_SEARCH_AUTOCAL → WellAutoCalSearch
  122: 121, // SET_HTML_WELL_AUTOCAL_REPORT → WellAutoCalReport
  123: 122, // SET_HTML_WELL_FACTORY_DETECTORS → WellFactoryDetectors
  124: 123, // SET_HTML_WELL_SETUP_USER_NUCLIDES → WellSetupUserNuclides
  125: 124, // SET_HTML_WELL_ADD_EDIT_USER_NUCLIDES → WellAddEditUserNuclides
  126: 125, // SET_HTML_LOGIN → Login
  127: 126, // SET_HTML_SECURITY → Security
  128: 127, // SET_HTML_ADDEDIT_USER → AddEditUser
  129: 128, // SET_HTML_SETUP_BIOASSAY → WellSetupBioAssay
  130: 129, // SET_HTML_SETUP_BIOASSAY_ENTER_EFF → WellSetupBioAssayEnterEfficiency
  131: 130, // SET_HTML_SETUP_BIOASSAY_MEASURE_EFF → WellSetupBioAssayMeasureEfficiency
  132: 131, // SET_HTML_WELL_BIOASSAY → WellBioAssay
  133: 132, // SET_HTML_WELL_BIOASSAY_ANALYSIS → WellBioAssayAnalysis
  134: 133, // SET_HTML_WELL_SEARCH_BIOASSAY → WellBioAssaySearch
  135: 134, // SET_HTML_SETUP_BIOASSAY_ENTER_ROI → WellSetupBioAssayEnterROI
  136: 135, // SET_HTML_SETUP_THYROID_UPTAKE_PROTOCOL → WellSetupThyroidUptakeProtocol
  137: 136, // SET_HTML_ADDEDIT_THYROID_UPTAKE_PROTOCOL → AddEditThyroidUptakeProtocol
  138: 137, // SET_HTML_WELL_THYROID_UPTAKE → WellThyroidUptake
  139: 138, // SET_HTML_ADDEDIT_THYROID_UPTAKE_TEST → AddEditThyroidUptakeTest
  140: 139, // SET_HTML_WELL_THYROID_UPTAKE_TEST → WellThyroidUptakeTest
  141: 140, // SET_HTML_WELL_THYROID_UPTAKE_ENTER_ADMIN → WellThyroidUptakeEnterAdminDate
  142: 141, // SET_HTML_WELL_THYROID_UPTAKE_MEAS_DOSE → WellThyroidUptakeMeasureDose
  143: 142, // SET_HTML_WELL_THYROID_UPTAKE_MEAS_PAT → WellThyroidUptakeMeasurePatient
  144: 143, // SET_HTML_WELL_THYROID_UPTAKE_ENTER_NORMAL → WellThyroidUptakeEnterNormal
  145: 144, // SET_HTML_WELL_REPEATED_MEASUREMENT → Dummy1
  146: 145, // SET_HTML_WELL_SEARCH_REPEATED_MEASUREMENT → Dummy2
  147: 146, // SET_HTML_WELL_RBC_SURVIVAL → WellRBCSurvival
  148: 147, // SET_HTML_ADDEDIT_RBC_SURVIVAL_TEST → AddEditRBCSurvivalTest
  149: 148, // SET_HTML_WELL_RBC_SURVIVAL_TEST → WellRBCSurvivalTest
  150: 149, // SET_HTML_WELL_RBC_SURVIVAL_MEASUREMENT → WellRBCSurvivalMeasurement
  151: 150, // SET_HTML_WELL_RBC_SURVIVAL_ENTER_NORMAL → WellRBCSurvivalEnterNormal
  152: 151, // SET_HTML_SETTIME2_EUR → SetTime2Eur
  153: 152, // SET_HTML_SETTIME2_JAP → SetTime2Jap
  154: 153, // SET_HTML_AUTOLINEARITY → AutoLinearity
  155: 154, // SET_HTML_AUTOLINEARITY_TEST → AutoLinearityTest
  156: 155, // SET_HTML_GENERIC_YES_NO → GenericYesNo
  157: 156, // SET_HTML_AUTOLINEARITY_SEARCH → AutoLinearitySearch
  158: 157, // SET_HTML_CHAMBER_REPORTS → ChamberReports
  159: 158, // SET_HTML_CHAMBER_SEARCH → ChamberSearch
  160: 159, // SET_HTML_CHAMBER_DAILY_TEST_VIEW → ChamberDailyTestView
  161: 160, // SET_HTML_CHAMBER_ZERO_TEST_VIEW → ChamberZeroTestView
  162: 161, // SET_HTML_CHAMBER_BACKGROUND_TEST_VIEW → ChamberBackgroundTestView
  163: 162, // SET_HTML_CHAMBER_VOLTAGE_TEST_VIEW → ChamberVoltageTestView
  164: 163, // SET_HTML_CHAMBER_ACCURACY_TEST_VIEW → ChamberAccuracyTestView
  165: 164, // SET_HTML_CHAMBER_AUTOCONSTANCY_TEST_VIEW → ChamberAutoconstancyTestView
  166: 165, // SET_HTML_CHAMBER_HALFLIFE_CALC → HalflifeCalculator
}

/**
 * Mapping from SET_CLEAR value (byte(14)) → page index for frontend local navigation.
 * Derived from the firmware's byte(14) handler in Amulet.c which calls
 * SetAmuletHTML(AmuletHTMLIndex[*_HTM]) for each case.
 */
const SET_CLEAR_TO_PAGE: Record<number, number> = {
  // Core CRC-25R screens (from screen_map.json page indices)
  1:  7,   // ADD_INV → InventoryAdd
  2:  8,   // WITHDRAW_INV → InventoryWithdraw
  3:  27,  // KIT_INV → InventoryKit
  4:  30,  // INFO → Info
  5:  25,  // SETUP → Setup
  6:  32,  // SETUPSOURCES → SetupSources
  7:  33,  // SETUPMOLY → SetupMoly
  8:  34,  // SETUPNUCLIDE → SetupNuclide
  9:  35,  // SETUPLINEARITY → SetupLinearity
  10: 36,  // SETUPREMOTE → SetupRemote
  11: 37,  // SETUPCALNUM → SetupCalNum
  12: 41,  // SETUPLINEARITYSTANDARD → SetupLinearityStandard
  13: 42,  // SETUPLINEARITYLINEATOR → SetupLinearityLineator
  14: 43,  // SETUPLINEARITYCALICHECK → SetupLinearityCalicheck
  15: 44,  // DOSETABLE → DoseTable
  16: 47,  // WELLMAIN → WellMainScreen
  17: 50,  // WELLAUTOCALIBRATE → WellAutoCalibrate
  18: 49,  // WELLMEASUREMENT → WellMeasurement
  19: 53,  // WELLSETUPNUCLIDE → WellSetupNuclide
  20: 54,  // WELLSETUPSEALED → WellSetupSealed
  21: 55,  // WELLSETUPSEALED2 → WellSetupSealed2
  22: 56,  // WELLSETUPTESTSOURCE → WellSetupTestSource
  23: 57,  // WELLSETUPTRIGGERLEVEL → WellSetupTriggerLevel
  24: 58,  // WELLSETUPEFFICIENCIES → WellSetupEfficiencies
  25: 59,  // WELLEDITEFFICIENCIES → WellEditEfficiencies
  26: 61,  // WELLMEASUREEFFICIENCY → WellMeasureEfficiency
  27: 16,  // DAILY → Daily
  28: 20,  // ACCURACY → Accuracy
  29: 62,  // AUTOCONSTANCY → AutoConstancy
  30: 21,  // LINEARITY → Linearity
  31: 12,  // ONESTRIP → OneStrip
  32: 13,  // TWOSTRIP → TwoStrip
  33: 14,  // HMPAO → HMPAO
  34: 15,  // MAG3 → Mag3
  35: 23,  // MOLY → Moly
  36: 64,  // FACTORY → Factory
  37: 65,  // SETUPCHAMBER → SetupChamber
  38: 68,  // WELLMEASUREMENTS → WellMeasurements
  39: 69,  // WELLEDITFULLEFFICIENCY → WellEditFullEfficiency
  40: 70,  // EDITCALIBSERIAL → SetupCalibSerialNum
  41: 73,  // WELLSCHILLING → WellSchilling
  42: 74,  // WELLPLASMA → WellPlasma
  43: 75,  // WELLRBC → WellRBC
  44: 76,  // WELLFACTORY → WellFactory
  45: 67,  // WELLMDA → WellMDATest
  46: 81,  // WELLCHI → WellChiTest
  47: 85,  // SETUPRHOTKEYS → SetupRHotkeys
  48: 86,  // SETUPPHOTKEYS → SetupPHotkeys
  49: 91,  // LOWLEVEL → LowLevel
  50: 92,  // WELLSTABILITYTEST → WellStabilityTest
  51: 93,  // SETUPCOMMUNICATIONS → SetupCommunications
  52: 95,  // ETHERNET → Ethernet
  53: 97,  // WELLSETUPBACKGROUNDTYPE → WellSetupTypes
  54: 97,  // WELLSETUPWORKAREATYPE → WellSetupTypes
  55: 97,  // WELLSETUPUNRESTRICTIVETYPE → WellSetupTypes
  56: 97,  // WELLSETUPSEALEDTYPE → WellSetupTypes
  57: 97,  // WELLSETUPPACKAGETYPE → WellSetupTypes
  58: 99,  // WELLSETUPADDLOCATION → WellSetupAddEditLocation
  59: 99,  // WELLSETUPEDITLOCATION → WellSetupAddEditLocation
  60: 100, // WELLSETUPDELETELOCATION → WellSetupDeleteLocation
  61: 101, // WELLWIPELIST → WellWipeList
  62: 105, // WELLSEARCHWIPE → WellWipeSearch
  63: 107, // INACTIVATE → Inactivate
  64: 108, // WELLSEARCHSYSTEMTEST → WellSystemTestSearch
  65: 109, // WELLSEARCHMDA → WellMDASearch
  66: 111, // WELLSEARCHCHI → WellChiSearch
  67: 113, // TESTIDENT → TestIdent
  68: 114, // WELLSEARCHSCHILLING → WellSchillingSearch
  69: 116, // WELLSEARCHPLASMA → WellPlasmaSearch
  70: 118, // WELLSEARCHRBC → WellRBCSearch
  71: 120, // WELLSEARCHAUTOCAL → WellAutoCalSearch
  72: 122, // WELLFACTORYDETECTORS → WellFactoryDetectors
  73: 123, // WELLSETUPUSERNUCLIDES → WellSetupUserNuclides
  74: 124, // WELLADDEDITUSERNUCLIDES → WellAddEditUserNuclides
  75: 125, // LOGIN → Login
  76: 126, // SECURITY → Security
  77: 127, // ADDEDIT_USER → AddEditUser
  78: 52,  // WELLSETUPADVANCED → WellAdvancedSetup
  80: 128, // SETUPBIOASSAY → WellSetupBioAssay
  81: 129, // SETUPBIOASSAYENTEREFF → WellSetupBioAssayEnterEfficiency
  82: 130, // SETUPBIOASSAYMEASUREEFF → WellSetupBioAssayMeasureEfficiency
  83: 131, // WELLBIOASSAY → WellBioAssay
  84: 132, // WELLBIOASSAYANALYSIS → WellBioAssayAnalysis
  85: 133, // WELLSEARCHBIOASSAY → WellBioAssaySearch
  86: 134, // SETUPBIOASSAYENTERROI → WellSetupBioAssayEnterROI
  87: 135, // SETUPTHYROIDUPTAKEPROTOCOL → WellSetupThyroidUptakeProtocol
  88: 136, // ADDEDITTHYROIDUPTAKEPROTOCOL → AddEditThyroidUptakeProtocol
  89: 137, // WELLTHYROIDUPTAKE → WellThyroidUptake
  90: 138, // ADDEDITTHYROIDUPTAKETEST → AddEditThyroidUptakeTest
  91: 139, // WELLTHYROIDUPTAKETEST → WellThyroidUptakeTest
  92: 140, // WELLTHYROIDUPTAKEENTERADMIN → WellThyroidUptakeEnterAdministrationDate
  93: 141, // WELLTHYROIDUPTAKEMEASDOSE → WellThyroidUptakeMeasureDose
  94: 142, // WELLTHYROIDUPTAKEMEASPAT → WellThyroidUptakeMeasurePatient
  95: 143, // WELLTHYROIDUPTAKEENTERNORMAL → WellThyroidUptakeEnterNormal
  96: 144, // WELLTHYROIDUPTAKEREPORT → Dummy1 (thyroid uptake report placeholder)
  97: 145, // WELLREPEATEDMEASUREMENT → Dummy2 (repeated measurement placeholder)
  98: 146, // WELLSEARCHREPEATEDMEASUREMENT → WellRBCSurvival (search repeated meas)
  99: 146, // WELLRBCSURVIVAL → WellRBCSurvival
  100: 147, // ADDEDITRBCSURVIVALTEST → AddEditRBCSurvivalTest
  101: 148, // WELLRBCSURVIVALTEST → WellRBCSurvivalTest
  102: 149, // WELLRBCSURVIVALMEASUREMENT → WellRBCSurvivalMeasurement
  103: 149, // WELLRBCSURVIVALREPORT → WellRBCSurvivalMeasurement
  104: 150, // WELLRBCSURVIVALENTERNORMAL → WellRBCSurvivalEnterNormal
  105: 154, // AUTOLINEARITYTEST → AutoLinearityTest
  106: 155, // AUTOLINEARITYVIEW → GenericYesNo
  107: 156, // AUTOLINEAIRTYSEARCH → AutoLinearitySearch
  108: 158, // CHAMBERDAILYTESTSEARCH → ChamberSearch
  109: 158, // CHAMBERZEROSEARCH → ChamberSearch
  110: 158, // CHAMBERBACKGROUNDSEARCH → ChamberSearch
  111: 158, // CHAMBERVOLTAGESEARCH → ChamberSearch
  112: 158, // CHAMBERACCURACYSEARCH → ChamberSearch
  113: 165, // HALFLIFECALC → HalflifeCalculator
  114: 166, // SETUPKEY → SetupKey
  115: 167, // SETUPPASSWORD → SetupPassword
}

/**
 * Mapping from SET_CLEAR value (byte(14)) → SCREEN value (byte(3)) for menu registration.
 * The SCREEN values match MENU_* defines in the firmware's Headers/Amulet.h exactly.
 * On real hardware, when a page loads on the Amulet display, it sends byte(3) = SCREEN_*
 * to tell the firmware which menu handler to run.
 */
const SET_CLEAR_TO_SCREEN: Record<number, number> = {
  // SCREEN_* values from Amulet.h (sent as byte(3) for menu registration)
  1:  15, // ADD_INV → SCREEN_ADD_INVENTORY
  2:  21, // WITHDRAW_INV → SCREEN_WITHDRAW_INVENTORY
  3:  23, // KIT_INV → SCREEN_KIT_INVENTORY
  4:  25, // INFO → SCREEN_INFO
  5:  26, // SETUP → SCREEN_SETUP
  6:  27, // SETUPSOURCES → SCREEN_SETUP_SOURCES
  7:  28, // SETUPMOLY → SCREEN_SETUP_MOLY
  8:  29, // SETUPNUCLIDE → SCREEN_SETUP_NUCLIDE
  9:  30, // SETUPLINEARITY → SCREEN_SETUP_LINEARITY
  10: 31, // SETUPREMOTE → SCREEN_SETUP_REMOTE
  11: 33, // SETUPCALNUM → SCREEN_SETUP_CALNUM
  12: 37, // SETUPLINEARITYSTANDARD → SCREEN_SETUP_LINEARITY_STANDARD
  13: 38, // SETUPLINEARITYLINEATOR → SCREEN_SETUP_LINEARITY_LINEATOR
  14: 39, // SETUPLINEARITYCALICHECK → SCREEN_SETUP_LINEARITY_CALICHECK
  15: 40, // DOSETABLE → SCREEN_DOSE_TABLE
  16: 44, // WELLMAIN → SCREEN_WELL_MAIN_SCREEN
  17: 46, // WELLAUTOCALIBRATE → SCREEN_WELL_AUTOCALIBRATE
  18: 45, // WELLMEASUREMENT → SCREEN_WELL_MEASUREMENT
  19: 49, // WELLSETUPNUCLIDE → SCREEN_WELL_SETUP_NUCLIDE
  20: 50, // WELLSETUPSEALED → SCREEN_WELL_SETUP_SEALED
  21: 51, // WELLSETUPSEALED2 → SCREEN_WELL_SETUP_SEALED2
  22: 52, // WELLSETUPTESTSOURCE → SCREEN_WELL_SETUP_TEST_SOURCE
  23: 53, // WELLSETUPTRIGGERLEVEL → SCREEN_WELL_SETUP_TRIGGER_LEVEL
  24: 54, // WELLSETUPEFFICIENCIES → SCREEN_WELL_SETUP_EFFICIENCIES
  25: 55, // WELLEDITEFFICIENCIES → SCREEN_WELL_EDIT_EFFICIENCIES
  26: 57, // WELLMEASUREEFFICIENCY → SCREEN_WELL_MEASURE_EFFICIENCY
  27: 1,  // DAILY → SCREEN_DAILY
  28: 4,  // ACCURACY → SCREEN_ACCURACY
  29: 59, // AUTOCONSTANCY → SCREEN_AUTOCONSTANCY
  30: 6,  // LINEARITY → SCREEN_LINEARITY
  31: 7,  // ONESTRIP → SCREEN_ONESTRIP
  32: 8,  // TWOSTRIP → SCREEN_TWOSTRIP
  33: 9,  // HMPAO → SCREEN_HMPAO
  34: 10, // MAG3 → SCREEN_MAG3
  35: 11, // MOLY → SCREEN_MOLY
  36: 62, // FACTORY → SCREEN_FACTORY
  37: 63, // SETUPCHAMBER → SCREEN_SETUPCHAMBER
  38: 66, // WELLMEASUREMENTS → SCREEN_WELL_MEASUREMENTS
  39: 67, // WELLEDITFULLEFFICIENCY → SCREEN_WELL_EDIT_FULL_EFFICIENCY
  40: 68, // EDITCALIBSERIAL → SCREEN_EDIT_CALIB_SERIAL
  41: 70, // WELLSCHILLING → SCREEN_WELL_SCHILLING
  42: 71, // WELLPLASMA → SCREEN_WELL_PLASMA
  43: 72, // WELLRBC → SCREEN_WELL_RBC
  44: 73, // WELLFACTORY → SCREEN_WELL_FACTORY
  45: 65, // WELLMDA → SCREEN_WELL_MDA_TEST
  46: 78, // WELLCHI → SCREEN_WELL_CHI_TEST
  47: 82, // SETUPRHOTKEYS → SCREEN_SETUP_RHOTKEYS
  48: 83, // SETUPPHOTKEYS → SCREEN_SETUP_PHOTKEYS
  49: 89, // LOWLEVEL → SCREEN_LOW_LEVEL
  50: 90, // WELLSTABILITYTEST → SCREEN_WELL_STABILITY_TEST
  51: 91, // SETUPCOMMUNICATIONS → SCREEN_SETUP_COMMUNICATIONS
  52: 93, // ETHERNET → SCREEN_ETHERNET
  53: 94, // WELLSETUPBACKGROUNDTYPE → SCREEN_WELL_SETUP_TYPES
  54: 94, // WELLSETUPWORKAREATYPE → SCREEN_WELL_SETUP_TYPES
  55: 94, // WELLSETUPUNRESTRICTIVETYPE → SCREEN_WELL_SETUP_TYPES
  56: 94, // WELLSETUPSEALEDTYPE → SCREEN_WELL_SETUP_TYPES
  57: 94, // WELLSETUPPACKAGETYPE → SCREEN_WELL_SETUP_TYPES
  58: 96, // WELLSETUPADDLOCATION → SCREEN_WELL_SETUP_ADDEDIT_LOC
  59: 96, // WELLSETUPEDITLOCATION → SCREEN_WELL_SETUP_ADDEDIT_LOC
  60: 97, // WELLSETUPDELETELOCATION → SCREEN_WELL_SETUP_DELETE_LOC
  61: 98, // WELLWIPELIST → SCREEN_WELL_WIPE_LIST
  62: 101, // WELLSEARCHWIPE → SCREEN_WELL_SEARCH_WIPE
  63: 103, // INACTIVATE → SCREEN_INACTIVATE
  64: 104, // WELLSEARCHSYSTEMTEST → SCREEN_WELL_SEARCH_SYSTEM_TEST
  65: 105, // WELLSEARCHMDA → SCREEN_WELL_SEARCH_MDA
  66: 107, // WELLSEARCHCHI → SCREEN_WELL_SEARCH_CHI
  67: 109, // TESTIDENT → SCREEN_TEST_IDENT
  68: 110, // WELLSEARCHSCHILLING → SCREEN_WELL_SEARCH_SCHILLING
  69: 112, // WELLSEARCHPLASMA → SCREEN_WELL_SEARCH_PLASMA
  70: 114, // WELLSEARCHRBC → SCREEN_WELL_SEARCH_RBC
  71: 116, // WELLSEARCHAUTOCAL → SCREEN_WELL_SEARCH_AUTOCAL
  72: 118, // WELLFACTORYDETECTORS → SCREEN_WELL_FACTORY_DETECTORS
  73: 119, // WELLSETUPUSERNUCLIDES → SCREEN_WELL_SETUP_USER_NUCLIDES
  74: 120, // WELLADDEDITUSERNUCLIDES → SCREEN_WELL_ADD_EDIT_USER_NUCLIDES
  75: 123, // LOGIN → SCREEN_LOGIN
  76: 124, // SECURITY → SCREEN_SECURITY
  77: 125, // ADDEDIT_USER → SCREEN_ADDEDIT_USER
  78: 48,  // WELLSETUPADVANCED → SCREEN_WELL_ADVANCED_SETUP
  80: 126, // SETUPBIOASSAY → SCREEN_SETUP_BIOASSAY
  81: 127, // SETUPBIOASSAYENTEREFF → SCREEN_SETUP_BIOASSAY_ENTER_EFF
  82: 128, // SETUPBIOASSAYMEASUREEFF → SCREEN_SETUP_BIOASSAY_MEASURE_EFF
  83: 129, // WELLBIOASSAY → SCREEN_WELL_BIOASSAY
  84: 130, // WELLBIOASSAYANALYSIS → SCREEN_WELL_BIOASSAY_ANALYSIS
  85: 131, // WELLSEARCHBIOASSAY → SCREEN_WELL_SEARCH_BIOASSAY
  86: 132, // SETUPBIOASSAYENTERROI → SCREEN_SETUP_BIOASSAY_ENTER_ROI
  87: 133, // SETUPTHYROIDUPTAKEPROTOCOL → SCREEN_SETUP_THYROID_UPTAKE_PROTOCOL
  88: 134, // ADDEDITTHYROIDUPTAKEPROTOCOL → SCREEN_ADDEDIT_THYROID_UPTAKE_PROTOCOL
  89: 135, // WELLTHYROIDUPTAKE → SCREEN_WELL_THYROID_UPTAKE
  90: 136, // ADDEDITTHYROIDUPTAKETEST → SCREEN_ADDEDIT_THYROID_UPTAKE_TEST
  91: 137, // WELLTHYROIDUPTAKETEST → SCREEN_WELL_THYROID_UPTAKE_TEST
  92: 138, // WELLTHYROIDUPTAKEENTERADMIN → SCREEN_WELL_THYROID_UPTAKE_ENTER_ADMIN
  93: 139, // WELLTHYROIDUPTAKEMEASDOSE → SCREEN_WELL_THYROID_UPTAKE_MEAS_DOSE
  94: 140, // WELLTHYROIDUPTAKEMEASPAT → SCREEN_WELL_THYROID_UPTAKE_MEAS_PAT
  95: 141, // WELLTHYROIDUPTAKEENTERNORMAL → SCREEN_WELL_THYROID_UPTAKE_ENTER_NORMAL
  96: 142, // WELLTHYROIDUPTAKEREPORT → (no specific SCREEN)
  97: 142, // WELLREPEATEDMEASUREMENT → SCREEN_WELL_REPEATED_MEASUREMENT
  98: 143, // WELLSEARCHREPEATEDMEASUREMENT → SCREEN_WELL_SEARCH_REPEATED_MEASUREMENT
  99: 144, // WELLRBCSURVIVAL → SCREEN_WELL_RBC_SURVIVAL
  100: 145, // ADDEDITRBCSURVIVALTEST → SCREEN_ADDEDIT_RBC_SURVIVAL_TEST
  101: 146, // WELLRBCSURVIVALTEST → SCREEN_WELL_RBC_SURVIVAL_TEST
  102: 147, // WELLRBCSURVIVALMEASUREMENT → SCREEN_WELL_RBC_SURVIVAL_MEASUREMENT
  103: 147, // WELLRBCSURVIVALREPORT → SCREEN_WELL_RBC_SURVIVAL_MEASUREMENT
  104: 148, // WELLRBCSURVIVALENTERNORMAL → SCREEN_WELL_RBC_SURVIVAL_ENTER_NORMAL
  105: 150, // AUTOLINEARITYTEST → SCREEN_AUTOLINEARITYTEST
  107: 152, // AUTOLINEARITYSEARCH → SCREEN_AUTOLINEARITYSEARCH
  108: 154, // CHAMBERDAILYTESTSEARCH → SCREEN_CHAMBER_SEARCH
  109: 154, // CHAMBERZEROSEARCH → SCREEN_CHAMBER_SEARCH
  110: 154, // CHAMBERBACKGROUNDSEARCH → SCREEN_CHAMBER_SEARCH
  111: 154, // CHAMBERVOLTAGESEARCH → SCREEN_CHAMBER_SEARCH
  112: 154, // CHAMBERACCURACYSEARCH → SCREEN_CHAMBER_SEARCH
  113: 161, // HALFLIFECALC → SCREEN_CHAMBER_HALFLIFE_CALC
  114: 162, // SETUPKEY → SCREEN_SETUP_KEY
  115: 163, // SETUPPASSWORD → SCREEN_SETUP_PASSWORD
}

/**
 * Mapping from SET_HTML value (byte(255)) → SCREEN value (byte(3)) for menu registration.
 * When %SetHTML navigates to a page, the page sends byte(3) = SCREEN_* on load.
 * SCREEN_* values match MENU_* defines 1:1 in the firmware.
 */
const SET_HTML_TO_SCREEN: Record<number, number> = {
  1:  4,  // ACCURACY → SCREEN_ACCURACY
  2:  2,  // BACKGROUND → SCREEN_BKG
  3:  3,  // CHAMBERVOLTS → SCREEN_CHAMBERVOLTS
  4:  1,  // DAILY → SCREEN_DAILY
  5:  5,  // GEOMETRY → SCREEN_GEOMETRY
  6:  58, // GEOMETRYREPORT → SCREEN_GEOMETRY_REPORT
  7:  9,  // HMPAO → SCREEN_HMPAO
  8:  42, // INDEX → SCREEN_INITIAL
  9:  12, // INVENTORY → SCREEN_INVENTORY
  10: 15, // INVENTORYADD → SCREEN_ADD_INVENTORY
  11: 21, // INVENTORYWITHDRAW → SCREEN_WITHDRAW_INVENTORY
  12: 17, // KEYBOARD2 → SCREEN_KEYBOARD
  13: 16, // KEYPAD2 → SCREEN_KEYPAD
  14: 6,  // LINEARITY → SCREEN_LINEARITY
  15: 10, // MAG3 → SCREEN_MAG3
  16: 32, // MAINSCREEN → SCREEN_MAIN_SCREEN
  17: 19, // MEASUREACTIVITY → SCREEN_ACTIVITY
  18: 11, // MOLY → SCREEN_MOLY
  19: 13, // NUCLIDE2 → SCREEN_NUCLIDE
  20: 7,  // ONESTRIP → SCREEN_ONESTRIP
  21: 88, // QC → SCREEN_QC
  22: 19, // SETACTIVITY → SCREEN_ACTIVITY
  23: 18, // SETSTUDY → SCREEN_STUDY
  24: 14, // SETTIME2 → SCREEN_TIME
  25: 26, // SETUP → SCREEN_SETUP
  26: 8,  // TWOSTRIP → SCREEN_TWOSTRIP
  27: 22, // INVENTORYDELETE → SCREEN_DELETE_INVENTORY
  28: 23, // INVENTORYKIT → SCREEN_KIT_INVENTORY
  29: 24, // SETSTUDY2 → SCREEN_STUDY2
  30: 61, // ENHANCED → SCREEN_ENHANCED
  31: 25, // INFO → SCREEN_INFO
  32: 43, // PSETUP → SCREEN_OTHER
  33: 27, // SETUPSOURCES → SCREEN_SETUP_SOURCES
  34: 28, // SETUPMOLY → SCREEN_SETUP_MOLY
  35: 29, // SETUPNUCLIDE → SCREEN_SETUP_NUCLIDE
  36: 30, // SETUPLINEARITY → SCREEN_SETUP_LINEARITY
  37: 31, // SETUPREMOTE → SCREEN_SETUP_REMOTE
  38: 33, // SETUPCALNUM → SCREEN_SETUP_CALNUM
  39: 34, // ERRORMSG → SCREEN_ERROR_MSG
  40: 35, // WARNINGMSG → SCREEN_WARNING_MSG
  41: 36, // NOTIFICATIONMSG → SCREEN_NOTIFICATION_MSG
  42: 37, // SETUPLINEARITYSTANDARD → SCREEN_SETUP_LINEARITY_STANDARD
  43: 38, // SETUPLINEARITYLINEATOR → SCREEN_SETUP_LINEARITY_LINEATOR
  44: 39, // SETUPLINEARITYCALICHECK → SCREEN_SETUP_LINEARITY_CALICHECK
  45: 40, // DOSETABLE → SCREEN_DOSE_TABLE
  46: 41, // SELECT_CHAMBER → SCREEN_SELECT_CHAMBER
  48: 44, // WELLMAINSCREEN → SCREEN_WELL_MAIN_SCREEN
  50: 45, // WELLMEASUREMENT → SCREEN_WELL_MEASUREMENT
  51: 46, // WELLAUTOCALIBRATE → SCREEN_WELL_AUTOCALIBRATE
  52: 47, // WELLPEAKS → SCREEN_WELL_PEAKS
  53: 48, // WELLADVANCEDSETUP → SCREEN_WELL_ADVANCED_SETUP
  54: 49, // WELL_SETUP_NUCLIDE → SCREEN_WELL_SETUP_NUCLIDE
  55: 50, // WELL_SETUP_SEALED → SCREEN_WELL_SETUP_SEALED
  56: 51, // WELL_SETUP_SEALED2 → SCREEN_WELL_SETUP_SEALED2
  57: 52, // WELL_SETUP_TEST_SOURCE → SCREEN_WELL_SETUP_TEST_SOURCE
  58: 53, // WELL_SETUP_TRIGGER_LEVEL → SCREEN_WELL_SETUP_TRIGGER_LEVEL
  59: 54, // WELL_SETUP_EFFICIENCIES → SCREEN_WELL_SETUP_EFFICIENCIES
  60: 55, // WELL_EDIT_EFFICIENCIES → SCREEN_WELL_EDIT_EFFICIENCIES
  61: 56, // WELL_MANUAL → SCREEN_WELL_MANUAL
  62: 57, // WELL_MEASURE_EFFICIENCY → SCREEN_WELL_MEASURE_EFFICIENCY
  63: 59, // AUTOCONSTANCY → SCREEN_AUTOCONSTANCY
  64: 60, // SLEEP → SCREEN_SLEEP
  65: 62, // FACTORY → SCREEN_FACTORY
  66: 63, // SETUPCHAMBER → SCREEN_SETUPCHAMBER
  67: 64, // INVENTORYDELETEALL → SCREEN_DELETE_ALL_INVENTORY
  68: 65, // WELL_MDA_TEST → SCREEN_WELL_MDA_TEST
  69: 66, // WELL_MEASUREMENTS → SCREEN_WELL_MEASUREMENTS
  70: 67, // WELL_EDIT_FULL_EFF → SCREEN_WELL_EDIT_FULL_EFFICIENCY
  71: 68, // EDIT_CALIB_SERIAL → SCREEN_EDIT_CALIB_SERIAL
  72: 69, // WELL_WIPE_REPORT → SCREEN_WELL_WIPE_REPORT
  73: 121, // WELL_LAB_TESTS → SCREEN_WELL_LAB_TESTS
  74: 70,  // WELL_SCHILLING → SCREEN_WELL_SCHILLING
  75: 71,  // WELL_PLASMA → SCREEN_WELL_PLASMA
  76: 72,  // WELL_RBC → SCREEN_WELL_RBC
  77: 73,  // WELL_FACTORY → SCREEN_WELL_FACTORY
  78: 74,  // WELL_PLASMA_ANALYSIS → SCREEN_WELL_PLASMA_ANALYSIS
  79: 75,  // WELL_RBC_ANALYSIS → SCREEN_WELL_RBC_ANALYSIS
  80: 76,  // WELL_QC_TESTS → SCREEN_WELL_QC_TESTS
  81: 77,  // WELL_SYS_TEST_RESULT → SCREEN_WELL_SYS_TEST_RESULT
  82: 78,  // WELL_CHI_TEST → SCREEN_WELL_CHI_TEST
  83: 79,  // WELL_SCHILLING_ANALYSIS → SCREEN_SCHILLING_ANALYSIS
  84: 80,  // WELL_CHANGE_ROI → SCREEN_WELL_CHANGE_ROI
  85: 81,  // WELL_GENERAL_ANALYSIS → SCREEN_WELL_GENERAL_ANALYSIS
  86: 82,  // SETUP_RHOTKEYS → SCREEN_SETUP_RHOTKEYS
  87: 83,  // SETUP_PHOTKEYS → SCREEN_SETUP_PHOTKEYS
  88: 84,  // REMOTE → SCREEN_REMOTE
  89: 85,  // DELETE_LINEATOR → SCREEN_DELETE_LINEATOR
  90: 86,  // DELETE_CALICHECK → SCREEN_DELETE_CALICHECK
  91: 87,  // SELECT_LINEARITY → SCREEN_SELECT_LINEARITY
  92: 89,  // LOW_LEVEL → SCREEN_LOW_LEVEL
  93: 90,  // WELL_STABILITY_TEST → SCREEN_WELL_STABILITY_TEST
  94: 91,  // SETUPCOMMUNICATIONS → SCREEN_SETUP_COMMUNICATIONS
  95: 92,  // PCQC → SCREEN_PCQC
  96: 93,  // ETHERNET → SCREEN_ETHERNET
  97: 94,  // SETUP_WIPES → SCREEN_WELL_SETUP_TYPES
  98: 94,  // SETUP_TYPES → SCREEN_WELL_SETUP_TYPES
  99: 95,  // SETUP_LOCATION → SCREEN_WELL_SETUP_LOCATIONS
  100: 96,  // SETUP_ADDEDITLOCATION → SCREEN_WELL_SETUP_ADDEDIT_LOC
  101: 97,  // SETUP_DELETELOCATION → SCREEN_WELL_SETUP_DELETE_LOC
  102: 98,  // WELL_WIPE_LIST → SCREEN_WELL_WIPE_LIST
  103: 99,  // ERRORMSG_LARGE → SCREEN_ERROR_MSG_LARGE
  104: 100, // WELL_SPECTRUM → SCREEN_WELL_SPECTRUM
  105: 122, // WELL_REPORTS → SCREEN_WELL_REPORTS
  106: 101, // WELL_SEARCH_WIPE → SCREEN_WELL_SEARCH_WIPE
  107: 102, // GENERIC_ITEMS → SCREEN_GENERIC_ITEMS
  108: 103, // INACTIVATE → SCREEN_INACTIVATE
  109: 104, // WELL_SEARCH_SYSTEM_TEST → SCREEN_WELL_SEARCH_SYSTEM_TEST
  110: 105, // WELL_SEARCH_MDA → SCREEN_WELL_SEARCH_MDA
  111: 106, // WELL_MDA_ANALYSIS → SCREEN_WELL_MDA_ANALYSIS
  112: 107, // WELL_SEARCH_CHI → SCREEN_WELL_SEARCH_CHI
  113: 108, // WELL_CHI_ANALYSIS → SCREEN_WELL_CHI_ANALYSIS
  114: 109, // TEST_IDENT → SCREEN_TEST_IDENT
  115: 110, // WELL_SEARCH_SCHILLING → SCREEN_WELL_SEARCH_SCHILLING
  116: 111, // WELL_SCHILLING_REPORT → SCREEN_WELL_SCHILLING_REPORT
  117: 112, // WELL_SEARCH_PLASMA → SCREEN_WELL_SEARCH_PLASMA
  118: 113, // WELL_PLASMA_REPORT → SCREEN_WELL_PLASMA_REPORT
  119: 114, // WELL_SEARCH_RBC → SCREEN_WELL_SEARCH_RBC
  120: 115, // WELL_RBC_REPORT → SCREEN_WELL_RBC_REPORT
  121: 116, // WELL_SEARCH_AUTOCAL → SCREEN_WELL_SEARCH_AUTOCAL
  122: 117, // WELL_AUTOCAL_REPORT → SCREEN_WELL_AUTOCAL_REPORT
  123: 118, // WELL_FACTORY_DETECTORS → SCREEN_WELL_FACTORY_DETECTORS
  124: 119, // WELL_SETUP_USER_NUCLIDES → SCREEN_WELL_SETUP_USER_NUCLIDES
  125: 120, // WELL_ADD_EDIT_USER_NUCLIDES → SCREEN_WELL_ADD_EDIT_USER_NUCLIDES
  126: 123, // LOGIN → SCREEN_LOGIN
  127: 124, // SECURITY → SCREEN_SECURITY
  128: 125, // ADDEDIT_USER → SCREEN_ADDEDIT_USER
  129: 126, // SETUP_BIOASSAY → SCREEN_SETUP_BIOASSAY
  130: 127, // SETUP_BIOASSAY_ENTER_EFF → SCREEN_SETUP_BIOASSAY_ENTER_EFF
  131: 128, // SETUP_BIOASSAY_MEASURE_EFF → SCREEN_SETUP_BIOASSAY_MEASURE_EFF
  132: 129, // WELL_BIOASSAY → SCREEN_WELL_BIOASSAY
  133: 130, // WELL_BIOASSAY_ANALYSIS → SCREEN_WELL_BIOASSAY_ANALYSIS
  134: 131, // WELL_SEARCH_BIOASSAY → SCREEN_WELL_SEARCH_BIOASSAY
  135: 132, // SETUP_BIOASSAY_ENTER_ROI → SCREEN_SETUP_BIOASSAY_ENTER_ROI
  136: 133, // SETUP_THYROID_UPTAKE_PROTOCOL → SCREEN_SETUP_THYROID_UPTAKE_PROTOCOL
  137: 134, // ADDEDIT_THYROID_UPTAKE_PROTOCOL → SCREEN_ADDEDIT_THYROID_UPTAKE_PROTOCOL
  138: 135, // WELL_THYROID_UPTAKE → SCREEN_WELL_THYROID_UPTAKE
  139: 136, // ADDEDIT_THYROID_UPTAKE_TEST → SCREEN_ADDEDIT_THYROID_UPTAKE_TEST
  140: 137, // WELL_THYROID_UPTAKE_TEST → SCREEN_WELL_THYROID_UPTAKE_TEST
  141: 138, // WELL_THYROID_UPTAKE_ENTER_ADMIN → SCREEN_WELL_THYROID_UPTAKE_ENTER_ADMIN
  142: 139, // WELL_THYROID_UPTAKE_MEAS_DOSE → SCREEN_WELL_THYROID_UPTAKE_MEAS_DOSE
  143: 140, // WELL_THYROID_UPTAKE_MEAS_PAT → SCREEN_WELL_THYROID_UPTAKE_MEAS_PAT
  144: 141, // WELL_THYROID_UPTAKE_ENTER_NORMAL → SCREEN_WELL_THYROID_UPTAKE_ENTER_NORMAL
  145: 142, // WELL_REPEATED_MEASUREMENT → SCREEN_WELL_REPEATED_MEASUREMENT
  146: 143, // WELL_SEARCH_REPEATED_MEASUREMENT → SCREEN_WELL_SEARCH_REPEATED_MEASUREMENT
  147: 144, // WELL_RBC_SURVIVAL → SCREEN_WELL_RBC_SURVIVAL
  148: 145, // ADDEDIT_RBC_SURVIVAL_TEST → SCREEN_ADDEDIT_RBC_SURVIVAL_TEST
  149: 146, // WELL_RBC_SURVIVAL_TEST → SCREEN_WELL_RBC_SURVIVAL_TEST
  150: 147, // WELL_RBC_SURVIVAL_MEASUREMENT → SCREEN_WELL_RBC_SURVIVAL_MEASUREMENT
  151: 148, // WELL_RBC_SURVIVAL_ENTER_NORMAL → SCREEN_WELL_RBC_SURVIVAL_ENTER_NORMAL
  152: 14,  // SETTIME2_EUR → SCREEN_TIME
  153: 14,  // SETTIME2_JAP → SCREEN_TIME
  154: 149, // AUTOLINEARITY → SCREEN_AUTOLINEARITY
  155: 150, // AUTOLINEARITY_TEST → SCREEN_AUTOLINEARITYTEST
  156: 151, // GENERIC_YES_NO → SCREEN_GENERIC_YES_NO
  157: 152, // AUTOLINEARITY_SEARCH → SCREEN_AUTOLINEARITYSEARCH
  158: 153, // CHAMBER_REPORTS → SCREEN_CHAMBER_REPORTS
  159: 154, // CHAMBER_SEARCH → SCREEN_CHAMBER_SEARCH
  160: 155, // CHAMBER_DAILY_TEST_VIEW → SCREEN_CHAMBER_DAILY_TEST_VIEW
  161: 156, // CHAMBER_ZERO_TEST_VIEW → SCREEN_CHAMBER_ZERO_TEST_VIEW
  162: 157, // CHAMBER_BACKGROUND_TEST_VIEW → SCREEN_CHAMBER_BACKGROUND_TEST_VIEW
  163: 158, // CHAMBER_VOLTAGE_TEST_VIEW → SCREEN_CHAMBER_VOLTAGE_TEST_VIEW
  164: 159, // CHAMBER_ACCURACY_TEST_VIEW → SCREEN_CHAMBER_ACCURACY_TEST_VIEW
  165: 160, // CHAMBER_AUTOCONSTANCY_TEST_VIEW → SCREEN_CHAMBER_AUTOCONSTANCY_TEST_VIEW
  166: 161, // CHAMBER_HALFLIFE_CALC → SCREEN_CHAMBER_HALFLIFE_CALC
}
