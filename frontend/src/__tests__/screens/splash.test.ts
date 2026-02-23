/**
 * Splash (Index) Screen Tests
 *
 * Reference: Pictures_4.07c/Splash/English Prototype.png
 *
 * The Splash screen shows:
 * - Capintec logo (top center)
 * - "CRC-55t" model name
 * - "Rev 4.07c" firmware version
 * - "Continue" button (center)
 * - Copyright text at bottom
 *
 * Note: Index.json is a metadata manifest listing all 180 screen definitions.
 * It is NOT a standard screen definition with widgets. The splash screen
 * displayed at startup is page 0, but the firmware handles the splash display
 * and auto-advances to MainScreen.
 */

import { describe, it, expect } from 'vitest'
import * as fs from 'fs'
import * as path from 'path'
import { screenExists } from '../screen-test-helpers'

const SCREENS_DIR = path.resolve(__dirname, '../../../public/screens')

describe('Splash (Index)', () => {
  it('should have an Index.json file for page 0', () => {
    expect(screenExists('Index')).toBe(true)
  })

  it('Index.json should be a valid JSON file', () => {
    const filePath = path.join(SCREENS_DIR, 'Index.json')
    const content = JSON.parse(fs.readFileSync(filePath, 'utf-8'))
    expect(content).toBeDefined()
  })

  it('Index.json should contain screen catalog metadata', () => {
    const filePath = path.join(SCREENS_DIR, 'Index.json')
    const content = JSON.parse(fs.readFileSync(filePath, 'utf-8'))
    // Index.json is a metadata manifest with either:
    // - A "screens" array listing all screen summaries, OR
    // - Standard screen properties (name, widgets, etc.)
    const hasScreens = Array.isArray(content.screens)
    const hasWidgets = Array.isArray(content.widgets)
    const hasName = typeof content.name === 'string'
    expect(hasScreens || hasWidgets || hasName).toBe(true)
  })

  it('should map to page index 0 in screen_map.json', () => {
    const mapPath = path.resolve(SCREENS_DIR, '../screen_map.json')
    const screenMap = JSON.parse(fs.readFileSync(mapPath, 'utf-8'))
    expect(screenMap['0']).toBe('Index')
  })
})
