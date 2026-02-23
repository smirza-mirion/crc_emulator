/**
 * Screen Map Tests
 *
 * Validates that screen_map.json correctly maps page indices to screen names,
 * and that all mapped screens have corresponding JSON definition files.
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenMap,
  screenExists,
  loadScreenDef,
  verifyResolution,
} from './screen-test-helpers'

describe('Screen Map', () => {
  const screenMap = loadScreenMap()

  describe('Map Completeness', () => {
    it('should have entries', () => {
      const entries = Object.keys(screenMap)
      expect(entries.length).toBeGreaterThan(100)
    })

    it('all mapped screens should have JSON files', () => {
      const missing: string[] = []
      for (const [pageIdx, screenName] of Object.entries(screenMap)) {
        if (!screenExists(screenName)) {
          missing.push(`Page ${pageIdx} → ${screenName}`)
        }
      }
      expect(missing).toEqual([])
    })

    it('all screen definitions should have 800x600 resolution', () => {
      const wrongRes: string[] = []
      for (const [_, screenName] of Object.entries(screenMap)) {
        try {
          const def = loadScreenDef(screenName)
          // Skip metadata files that aren't standard screen definitions
          if (!def.resolution) continue
          if (!verifyResolution(def)) {
            wrongRes.push(screenName)
          }
        } catch { /* skip if file doesn't load */ }
      }
      expect(wrongRes).toEqual([])
    })
  })

  describe('Key Screen Mappings', () => {
    const expectedMappings: Record<string, string> = {
      '0': 'Index',
      '9': 'MainScreen',
      '16': 'Daily',
      '17': 'Background',
      '18': 'ChamberVolts',
      '19': 'Geometry',
      '20': 'Accuracy',
      '21': 'Linearity',
      '22': 'QC',
      '23': 'Moly',
      '24': 'Inventory',
      '25': 'Setup',
      '29': 'Enhanced',
      '30': 'Info',
      '31': 'Other',
      '44': 'DoseTable',
      '47': 'WellMainScreen',
      '62': 'AutoConstancy',
      '64': 'Factory',
      '125': 'Login',
      '153': 'AutoLinearity',
      '154': 'AutoLinearityTest',
      '156': 'AutoLinearitySearch',
      '165': 'HalflifeCalculator',
    }

    for (const [pageIdx, expectedName] of Object.entries(expectedMappings)) {
      it(`page ${pageIdx} should map to ${expectedName}`, () => {
        expect(screenMap[pageIdx]).toBe(expectedName)
      })
    }
  })

  describe('Reference Picture Screens Exist', () => {
    // Every screen shown in Pictures_4.07c must exist in the screen map
    const referenceScreens = [
      'MainScreen',        // Main
      'Setup',             // Setup
      'Daily',             // DailyTest
      'Background',        // Background
      'ChamberVolts',      // ChamberVolts
      'Accuracy',          // Accuracy
      'Enhanced',          // EnhancedTests
      'Geometry',          // Geometry
      'QC',                // QC
      'Moly',              // MolyAssay
      'Inventory',         // Inventory
      'AutoLinearity',     // AutoLinearity
      'AutoLinearityTest', // AutoLinearityStart
      'AutoLinearitySearch', // AutoLinearityReports
      'HalflifeCalculator', // HalfLifeCalc
      'Index',             // Splash
      'Other',             // Utility
    ]

    for (const name of referenceScreens) {
      it(`${name} should exist as a screen definition`, () => {
        expect(screenExists(name)).toBe(true)
      })
    }

    // These are navigated to from the Linearity screen
    const linearityScreens = [
      'SetupLinearityStandard',
      'SetupLinearityLineator',
      'SetupLinearityCalicheck',
    ]

    for (const name of linearityScreens) {
      it(`${name} should exist as a screen definition`, () => {
        expect(screenExists(name)).toBe(true)
      })
    }
  })

  describe('SelectLinearity Screen', () => {
    it('should exist (intermediate linearity menu)', () => {
      expect(screenExists('SelectLinearity') || screenExists('Linearity')).toBe(true)
    })
  })
})
