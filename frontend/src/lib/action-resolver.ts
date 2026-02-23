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

  // SET_CLEAR values from CRCHtml_3.00a/Amulet.h (%SetClear = UART.byte(14).setValue)
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

/**
 * Mapping from SET_CLEAR value (byte(14)) → page index for frontend local navigation.
 * Derived from the firmware's byte(14) handler in Amulet.c which calls
 * SetAmuletHTML(AmuletHTMLIndex[*_HTM]) for each case.
 */
const SET_CLEAR_TO_PAGE: Record<number, number> = {
  1:  0x07, // ADD_INV → INVENTORYADD_HTM
  2:  0x08, // WITHDRAW_INV → INVENTORYWITHDRAW_HTM
  3:  0x1B, // KIT_INV → INVENTORYKIT_HTM
  4:  0x1E, // INFO → INFO_HTM
  5:  0x19, // SETUP → SETUP_HTM
  6:  0x20, // SETUPSOURCES → SETUPSOURCES_HTM
  7:  0x21, // SETUPMOLY → SETUPMOLY_HTM
  8:  0x22, // SETUPNUCLIDE → SETUPNUCLIDE_HTM
  9:  0x23, // SETUPLINEARITY → SETUPLINEARITY_HTM
  10: 0x24, // SETUPREMOTE → SETUPREMOTE_HTM
  11: 0x25, // SETUPCALNUM → SETUPCALNUM_HTM
  12: 0x29, // SETUPLINEARITYSTANDARD → SETUPLINEARITYSTANDARD_HTM
  13: 0x2A, // SETUPLINEARITYLINEATOR → SETUPLINEARITYLINEATOR_HTM
  14: 0x2B, // SETUPLINEARITYCALICHECK → SETUPLINEARITYCALICHECK_HTM
  15: 0x2C, // DOSETABLE → DOSETABLE_HTM
  16: 0x2F, // WELLMAIN → WELLMAINSCREEN_HTM
  17: 0x32, // WELLAUTOCALIBRATE → WELLAUTOCALIBRATE_HTM
  18: 0x31, // WELLMEASUREMENT → WELLMEASUREMENT_HTM
  27: 0x10, // DAILY → DAILY_HTM
  28: 0x14, // ACCURACY → ACCURACY_HTM
  29: 0x3E, // AUTOCONSTANCY → AUTOCONSTANCY_HTM
  30: 0x15, // LINEARITY → LINEARITY_HTM
  31: 0x0C, // ONESTRIP → ONESTRIP_HTM
  32: 0x0D, // TWOSTRIP → TWOSTRIP_HTM
  33: 0x0E, // HMPAO → HMPAO_HTM
  34: 0x0F, // MAG3 → MAG3_HTM
  35: 0x17, // MOLY → MOLY_HTM
  36: 0x40, // FACTORY → FACTORY_HTM
  37: 0x41, // SETUPCHAMBER → SETUPCHAMBER_HTM
}

/**
 * Mapping from SET_CLEAR value (byte(14)) → SCREEN value (byte(3)) for menu registration.
 * The SCREEN values match MENU_* defines in the firmware's Headers/Amulet.h exactly.
 * On real hardware, when a page loads on the Amulet display, it sends byte(3) = SCREEN_*
 * to tell the firmware which menu handler to run.
 */
const SET_CLEAR_TO_SCREEN: Record<number, number> = {
  1:  15, // ADD_INV → MENU_ADD_INVENTORY
  2:  21, // WITHDRAW_INV → MENU_WITHDRAW_INVENTORY
  3:  23, // KIT_INV → MENU_KIT_INVENTORY
  4:  25, // INFO → MENU_INFO
  5:  26, // SETUP → MENU_SETUP
  6:  27, // SETUPSOURCES → MENU_SETUP_SOURCES
  7:  28, // SETUPMOLY → MENU_SETUP_MOLY
  8:  29, // SETUPNUCLIDE → MENU_SETUP_NUCLIDE
  9:  30, // SETUPLINEARITY → MENU_SETUP_LINEARITY
  10: 31, // SETUPREMOTE → MENU_SETUP_REMOTE
  11: 33, // SETUPCALNUM → MENU_SETUP_CALNUM
  12: 37, // SETUPLINEARITYSTANDARD → MENU_SETUP_LINEARITY_STANDARD
  13: 38, // SETUPLINEARITYLINEATOR → MENU_SETUP_LINEARITY_LINEATOR
  14: 39, // SETUPLINEARITYCALICHECK → MENU_SETUP_LINEARITY_CALICHECK
  15: 40, // DOSETABLE → MENU_DOSE_TABLE
  16: 44, // WELLMAIN → MENU_WELL_MAIN_SCREEN
  17: 46, // WELLAUTOCALIBRATE → MENU_WELL_AUTOCALIBRATE
  18: 45, // WELLMEASUREMENT → MENU_WELL_MEASUREMENT
  19: 49, // WELLSETUPNUCLIDE → MENU_WELL_SETUP_NUCLIDE
  20: 50, // WELLSETUPSEALED → MENU_WELL_SETUP_SEALED
  21: 51, // WELLSETUPSEALED2 → MENU_WELL_SETUP_SEALED2
  22: 52, // WELLSETUPTESTSOURCE → MENU_WELL_SETUP_TEST_SOURCE
  23: 53, // WELLSETUPTRIGGERLEVEL → MENU_WELL_SETUP_TRIGGER_LEVEL
  24: 54, // WELLSETUPEFFICIENCIES → MENU_WELL_SETUP_EFFICIENCIES
  25: 55, // WELLEDITEFFICIENCIES → MENU_WELL_EDIT_EFFICIENCIES
  26: 57, // WELLMEASUREEFFICIENCY → MENU_WELL_MEASURE_EFFICIENCY
  27: 1,  // DAILY → MENU_DAILY
  28: 4,  // ACCURACY → MENU_ACCURACY
  29: 59, // AUTOCONSTANCY → MENU_AUTOCONSTANCY
  30: 6,  // LINEARITY → MENU_LINEARITY
  31: 7,  // ONESTRIP → MENU_ONESTRIP
  32: 8,  // TWOSTRIP → MENU_TWOSTRIP
  33: 9,  // HMPAO → MENU_HMPAO
  34: 10, // MAG3 → MENU_MAG3
  35: 11, // MOLY → MENU_MOLY
  36: 62, // FACTORY → MENU_FACTORY
  37: 63, // SETUPCHAMBER → MENU_SETUPCHAMBER
  38: 66, // WELLMEASUREMENTS → MENU_WELL_MEASUREMENTS
  39: 67, // WELLEDITFULLEFFICIENCY → MENU_WELL_EDITFULLEFFICIENCY
  40: 68, // EDITCALIBSERIAL → MENU_SETUPCALIBSERIAL
  41: 70, // WELLSCHILLING → MENU_WELL_SCHILLING
  42: 71, // WELLPLASMA → MENU_WELL_PLASMA
  43: 72, // WELLRBC → MENU_WELL_RBC
  44: 73, // WELLFACTORY → MENU_WELL_FACTORY
  45: 65, // WELLMDA → MENU_WELL_MDA_TEST
  46: 78, // WELLCHI → MENU_WELL_CHITEST
  47: 82, // SETUPRHOTKEYS → MENU_SETUP_RHOTKEYS
  48: 83, // SETUPPHOTKEYS → MENU_SETUP_PHOTKEYS
  49: 89, // LOWLEVEL → MENU_LOW_LEVEL
  50: 90, // WELLSTABILITYTEST → MENU_WELL_STABILITYTEST
  51: 91, // SETUPCOMMUNICATIONS → MENU_SETUP_COMMUNICATIONS
  52: 93, // ETHERNET → MENU_ETHERNET
  63: 103, // INACTIVATE → MENU_INACTIVATE
  67: 109, // TESTIDENT → MENU_TEST_IDENT
  75: 123, // LOGIN → MENU_LOGIN
  76: 124, // SECURITY → MENU_SECURITY
  77: 125, // ADDEDIT_USER → MENU_ADD_EDIT_USER
  105: 150, // AUTOLINEARITYTEST → MENU_AUTOLINEARITY_TEST
  107: 152, // AUTOLINEARITYSEARCH → MENU_AUTOLINEARITY_SEARCH
  108: 154, // CHAMBERDAILYTESTSEARCH → MENU_CHAMBER_SEARCH
  109: 154, // CHAMBERZEROSEARCH → MENU_CHAMBER_SEARCH
  110: 154, // CHAMBERBACKGROUNDSEARCH → MENU_CHAMBER_SEARCH
  111: 154, // CHAMBERVOLTAGESEARCH → MENU_CHAMBER_SEARCH
  112: 154, // CHAMBERACCURACYSEARCH → MENU_CHAMBER_SEARCH
  113: 161, // HALFLIFECALC → MENU_CHAMBER_HALFLIFE_CALC
  114: 162, // SETUPKEY → MENU_SETUP_KEY
  115: 163, // SETUPPASSWORD → MENU_SETUP_PASSWORD
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
  63: 59, // AUTOCONSTANCY → SCREEN_AUTOCONSTANCY
  65: 62, // FACTORY → SCREEN_FACTORY
  66: 63, // SETUPCHAMBER → SCREEN_SETUPCHAMBER
}
