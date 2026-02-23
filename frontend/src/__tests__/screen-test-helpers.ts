/**
 * Screen Test Helpers
 *
 * Shared utilities for loading and validating screen JSON definitions
 * against the reference pictures from Pictures_4.07c.
 *
 * These tests validate that the emulator's screen definitions match the
 * real CRC-25R calibrator device 1:1.
 */

import * as fs from 'fs'
import * as path from 'path'

// ------------------------------------------------------------------
// Types mirroring screen-loader.ts (duplicated to avoid import issues
// when running under vitest with node environment access)
// ------------------------------------------------------------------

export interface WidgetDef {
  type: string
  name: string
  x: number
  y: number
  width: number
  height: number
  params: Record<string, string>
  position_from?: string
  language?: string
  bindings?: { source: string; type: string; index: number; param: string }[]
}

export interface RefreshAction {
  type: string
  target?: string
  value?: number | string
  macro?: string
  args?: string
  expression?: string
}

export interface RefreshTrigger {
  name?: string
  interval?: string
  onVar?: string
  trigger?: string
  actions?: RefreshAction[]
}

export interface StaticImage {
  src: string
  x?: number
  y?: number
  width: number
  height: number
  div_id?: string
}

export interface ScreenDef {
  name: string
  filename: string
  resolution: { width: number; height: number }
  directives?: string[]
  memoryMap?: {
    bytes?: Record<string, string>
    words?: Record<string, string>
    strings?: Record<string, string>
  }
  widgets: WidgetDef[]
  staticImages?: StaticImage[]
  refreshTriggers?: RefreshTrigger[]
  initActions?: RefreshAction[]
  referencedImages?: string[]
  hasFrenchVariants?: boolean
  ramUsage?: Record<string, Record<string, number[]>>
}

// ------------------------------------------------------------------
// Path helpers
// ------------------------------------------------------------------

const SCREENS_DIR = path.resolve(__dirname, '../../public/screens')
const SCREEN_MAP_PATH = path.resolve(__dirname, '../../public/screen_map.json')
const ASSETS_DIR = path.resolve(__dirname, '../../public/assets/images')

// ------------------------------------------------------------------
// Loaders
// ------------------------------------------------------------------

export function loadScreenDef(screenName: string): ScreenDef {
  const filePath = path.join(SCREENS_DIR, `${screenName}.json`)
  if (!fs.existsSync(filePath)) {
    throw new Error(`Screen definition not found: ${filePath}`)
  }
  return JSON.parse(fs.readFileSync(filePath, 'utf-8'))
}

export function loadScreenMap(): Record<string, string> {
  return JSON.parse(fs.readFileSync(SCREEN_MAP_PATH, 'utf-8'))
}

export function screenExists(screenName: string): boolean {
  return fs.existsSync(path.join(SCREENS_DIR, `${screenName}.json`))
}

// ------------------------------------------------------------------
// Widget query helpers
// ------------------------------------------------------------------

/** Find a widget by name (case-insensitive partial match). */
export function findWidget(screen: ScreenDef, name: string): WidgetDef | undefined {
  return screen.widgets.find(w => w.name === name)
}

/** Find all widgets whose name contains the substring. */
export function findWidgetsByPartialName(screen: ScreenDef, partial: string): WidgetDef[] {
  return screen.widgets.filter(w => w.name.toLowerCase().includes(partial.toLowerCase()))
}

/** Get all widgets of a specific type. */
export function getWidgetsByType(screen: ScreenDef, type: string): WidgetDef[] {
  return screen.widgets.filter(w => w.type === type)
}

/** Get all visible English widgets (no language tag or language=english). */
export function getEnglishWidgets(screen: ScreenDef): WidgetDef[] {
  return screen.widgets.filter(w => !w.language || w.language.toLowerCase() === 'english')
}

/** Get all French widgets. */
export function getFrenchWidgets(screen: ScreenDef): WidgetDef[] {
  return screen.widgets.filter(w => w.language?.toLowerCase() === 'french')
}

/** Get all widgets marked as initially invisible. */
export function getInvisibleWidgets(screen: ScreenDef): WidgetDef[] {
  return screen.widgets.filter(w => w.params?.invisible?.toUpperCase() === 'TRUE')
}

/** Get all buttons (CustomButton + FunctionButton). */
export function getAllButtons(screen: ScreenDef): WidgetDef[] {
  return screen.widgets.filter(w => w.type === 'CustomButton' || w.type === 'FunctionButton')
}

/** Get all string fields. */
export function getAllStringFields(screen: ScreenDef): WidgetDef[] {
  return screen.widgets.filter(w => w.type === 'StringField' || w.type === 'NumericField')
}

// ------------------------------------------------------------------
// Href / Action helpers
// ------------------------------------------------------------------

/** Extract the href param from a widget. */
export function getWidgetHref(widget: WidgetDef): string {
  return widget.params?.href || ''
}

/** Check if a widget's href contains a specific macro call. */
export function hrefContainsMacro(widget: WidgetDef, macroName: string): boolean {
  const href = getWidgetHref(widget)
  return href.includes(`%${macroName}`)
}

/** Check if a widget navigates to Home. */
export function isHomeButton(widget: WidgetDef): boolean {
  return hrefContainsMacro(widget, 'Home')
}

/** Check if a widget navigates Back. */
export function isBackButton(widget: WidgetDef): boolean {
  return hrefContainsMacro(widget, 'Back')
}

/** Extract SetHTML target from an href. */
export function getSetHTMLTarget(widget: WidgetDef): string | null {
  const href = getWidgetHref(widget)
  const match = href.match(/%SetHTML\(%%([^)]+)\)/)
  return match ? match[1] : null
}

/** Extract SetClear target from an href. */
export function getSetClearTarget(widget: WidgetDef): string | null {
  const href = getWidgetHref(widget)
  const match = href.match(/%SetClear\(%%([^)]+)\)/)
  return match ? match[1] : null
}

/** Extract the string index from a StringField's href. */
export function getStringIndex(widget: WidgetDef): number | null {
  const href = widget.params?.href || widget.params?.initHref || ''
  // Try string(N) pattern
  const strMatch = href.match(/string\((\d+)\)/)
  if (strMatch) return parseInt(strMatch[1])
  // Try label(N) pattern
  const labelMatch = href.match(/label\((\d+)\)/)
  if (labelMatch) return parseInt(labelMatch[1])
  return null
}

/** Get the byte index from a button's UART/internalRAM setValue action. */
export function getByteSetValue(widget: WidgetDef): { byteIndex: number; value: number } | null {
  const href = getWidgetHref(widget)
  const match = href.match(/(?:UART|internalRAM)\.byte\((\d+)\)\.setValue\(([^)]+)\)/)
  if (!match) return null
  const byteIndex = parseInt(match[1])
  const value = match[2].startsWith('0x') ? parseInt(match[2], 16) : parseInt(match[2])
  return { byteIndex, value }
}

// ------------------------------------------------------------------
// Trigger helpers
// ------------------------------------------------------------------

/** Find triggers that monitor a specific byte index. */
export function getTriggersByByte(screen: ScreenDef, byteIdx: number): RefreshTrigger[] {
  return (screen.refreshTriggers || []).filter(t => {
    const m = t.onVar?.match(/byte\((\d+)\)/)
    return m && parseInt(m[1]) === byteIdx
  })
}

/** Find triggers by name (exact or prefix match). */
export function getTriggersByName(screen: ScreenDef, name: string): RefreshTrigger[] {
  return (screen.refreshTriggers || []).filter(
    t => t.name === name || (t.name && t.name.startsWith(name))
  )
}

/** Check if a trigger is a "toggle" trigger (has setValue action). */
export function isToggleTrigger(trigger: RefreshTrigger): boolean {
  return (trigger.actions || []).some(a => a.type === 'setValue')
}

/** Check if a trigger has a forceUpdate action. */
export function hasForceUpdate(trigger: RefreshTrigger): boolean {
  return (trigger.actions || []).some(a => a.type === 'forceUpdate')
}

/** Get all targets of reappear actions in a trigger. */
export function getReappearTargets(trigger: RefreshTrigger): string[] {
  return (trigger.actions || [])
    .filter(a => a.type === 'reappear' && a.target)
    .map(a => a.target!)
}

/** Get all targets of disappear actions in a trigger. */
export function getDisappearTargets(trigger: RefreshTrigger): string[] {
  return (trigger.actions || [])
    .filter(a => a.type === 'disappear' && a.target)
    .map(a => a.target!)
}

// ------------------------------------------------------------------
// Validation helpers (used in assertions)
// ------------------------------------------------------------------

/**
 * Verify a screen has the standard navigation bar (Home + Back buttons).
 * Returns { home, back } widget references or null if missing.
 *
 * Handles two button patterns:
 * 1. Macro-based: href contains %Home or %Back
 * 2. UART-based: btnHome/btnBack using Amulet:UART.word(0).setValue()
 */
export function verifyNavBar(screen: ScreenDef): { home: WidgetDef | null; back: WidgetDef | null } {
  const allBtns = getAllButtons(screen)
  const home = allBtns.find(b =>
    (!b.language || b.language.toLowerCase() === 'english') &&
    (isHomeButton(b) || b.name === 'btnHome')
  )
  const back = allBtns.find(b =>
    (!b.language || b.language.toLowerCase() === 'english') &&
    (isBackButton(b) || b.name === 'btnBack')
  )
  return { home: home || null, back: back || null }
}

/**
 * Verify the screen has a title string field containing the expected text.
 * Checks for a StringField with label/initHref binding in the top header area (y < 70).
 */
export function findTitleWidget(screen: ScreenDef): WidgetDef | undefined {
  return screen.widgets.find(w =>
    (w.type === 'StringField' || w.type === 'NumericField') &&
    w.y < 70 && w.width > 200
  )
}

/**
 * Verify bottom status bar elements (Ch: X, R and Setup button).
 */
export function verifyBottomBar(screen: ScreenDef): {
  chamberLabel: WidgetDef | null
  setupButton: WidgetDef | null
} {
  const allFields = getAllStringFields(screen)
  const chamberLabel = allFields.find(w => w.y > 530 && w.name.toLowerCase().includes('chamb'))
  const allBtns = getAllButtons(screen)
  const setupButton = allBtns.find(b =>
    b.y > 530 && (getSetHTMLTarget(b) === 'SET_HTML_SETUP' || getSetClearTarget(b) === 'SET_CLEAR_SETUP')
  )
  return { chamberLabel: chamberLabel || null, setupButton: setupButton || null }
}

/**
 * Check that all referenced images actually exist on disk.
 */
export function verifyReferencedImages(screen: ScreenDef): string[] {
  const missing: string[] = []
  const images = screen.referencedImages || []
  for (const img of images) {
    // Normalize path
    let resolved = img
    if (img.startsWith('Images/')) {
      resolved = img.substring(7)
    }
    const fullPath = path.join(ASSETS_DIR, resolved)
    if (!fs.existsSync(fullPath)) {
      missing.push(img)
    }
  }
  return missing
}

/**
 * Verify that French variant widgets exist for all English widgets with language tag.
 */
export function verifyFrenchVariants(screen: ScreenDef): string[] {
  if (!screen.hasFrenchVariants) return []
  const englishNames = screen.widgets
    .filter(w => w.language?.toLowerCase() === 'english')
    .map(w => w.name)
  const frenchNames = new Set(
    screen.widgets
      .filter(w => w.language?.toLowerCase() === 'french')
      .map(w => w.name)
  )
  // Check that for each English widget there's a French counterpart
  const missing: string[] = []
  for (const name of englishNames) {
    // Common naming: btnDaily (eng) → btnDailyFrench (french)
    const frenchName = name + 'French'
    const altFrenchName = name.replace('English', 'French')
    if (!frenchNames.has(frenchName) && !frenchNames.has(altFrenchName) && !frenchNames.has(name)) {
      // Some widgets share the same name but with language tag
      const sameNameFrench = screen.widgets.find(w =>
        w.name === name && w.language?.toLowerCase() === 'french'
      )
      if (!sameNameFrench) {
        missing.push(name)
      }
    }
  }
  return missing
}

/**
 * Extract all navigation targets from a screen's buttons.
 * Returns a map of button name → target screen.
 */
export function getNavigationMap(screen: ScreenDef): Record<string, string> {
  const map: Record<string, string> = {}
  for (const btn of getAllButtons(screen)) {
    const setHTML = getSetHTMLTarget(btn)
    const setClear = getSetClearTarget(btn)
    if (setHTML) map[btn.name] = `SetHTML:${setHTML}`
    else if (setClear) map[btn.name] = `SetClear:${setClear}`
    else if (isHomeButton(btn)) map[btn.name] = 'Home'
    else if (isBackButton(btn)) map[btn.name] = 'Back'
  }
  return map
}

/**
 * Verify that the screen resolution is 800x600 (CRC-25R display size).
 */
export function verifyResolution(screen: ScreenDef): boolean {
  return screen.resolution?.width === 800 && screen.resolution?.height === 600
}

/**
 * Get a widget's font size from params.
 */
export function getWidgetFontSize(widget: WidgetDef): number | null {
  const fontSize = widget.params?.fontSize
  if (!fontSize) return null
  const match = fontSize.match(/(\d+)/)
  return match ? parseInt(match[1]) : null
}
