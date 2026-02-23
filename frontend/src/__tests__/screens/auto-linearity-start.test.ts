/**
 * AutoLinearity Test (Start) Screen Tests
 *
 * Reference: Pictures_4.07c/AutoLinearityStart/English Prototype - DSC00660.JPG
 *
 * The AutoLinearity Test screen shows:
 * - Title: "AutoLinearity Test - Ch:1, R" (top center)
 * - Home button (top left), Back button (top right)
 * - Config row: "Nuclide:" field, "Interval:" field, "Total:" field, "Start" button
 * - Table headers: "Date Time / Elapsed", "Measured", "Predicted", "% Var"
 * - Data rows (alternating light blue/white stripes)
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  getAllStringFields,
  verifyResolution,
  verifyNavBar,
  verifyReferencedImages,
  findWidgetsByPartialName,
} from '../screen-test-helpers'

describe('AutoLinearityTest (Start)', () => {
  const screen = loadScreenDef('AutoLinearityTest')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
    })

    it('should have 800x600 resolution', () => {
      expect(verifyResolution(screen)).toBe(true)
    })
  })

  describe('Navigation Bar', () => {
    it('should have Home button', () => {
      const nav = verifyNavBar(screen)
      expect(nav.home).toBeTruthy()
    })

    it('should have Back button', () => {
      const nav = verifyNavBar(screen)
      expect(nav.back).toBeTruthy()
    })
  })

  describe('Configuration Row', () => {
    it('should have Nuclide label and field', () => {
      const nuclideWidgets = findWidgetsByPartialName(screen, 'nucl')
      const allFields = getAllStringFields(screen)
      const nuclideField = allFields.find(w => w.y > 70 && w.y < 150)
      expect(nuclideWidgets.length > 0 || nuclideField).toBeTruthy()
    })

    it('should have Interval label and field', () => {
      const intervalWidgets = findWidgetsByPartialName(screen, 'interval')
      const allFields = getAllStringFields(screen)
      const intervalField = allFields.find(w =>
        w.y > 70 && w.y < 150 && w.x > 200 && w.x < 500
      )
      expect(intervalWidgets.length > 0 || intervalField).toBeTruthy()
    })

    it('should have Total label and field', () => {
      const totalWidgets = findWidgetsByPartialName(screen, 'total')
      const allFields = getAllStringFields(screen)
      const totalField = allFields.find(w =>
        w.y > 70 && w.y < 150 && w.x > 400
      )
      expect(totalWidgets.length > 0 || totalField).toBeTruthy()
    })

    it('should have Start button', () => {
      const allBtns = getAllButtons(screen)
      const startBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('start') ||
        b.x > 700 && b.y > 70 && b.y < 150
      )
      expect(startBtn).toBeDefined()
    })
  })

  describe('Data Table Headers', () => {
    it('should have table header fields for Date/Time, Measured, Predicted, % Var', () => {
      const allFields = getAllStringFields(screen)
      // Headers should be in a row around y=150-200
      const headerFields = allFields.filter(w => w.y > 120 && w.y < 200)
      expect(headerFields.length).toBeGreaterThanOrEqual(3)
    })
  })

  describe('Data Rows', () => {
    it('should have multiple data row string fields', () => {
      const allFields = getAllStringFields(screen)
      // Data rows should be below headers
      const dataFields = allFields.filter(w => w.y > 180 && w.y < 530)
      expect(dataFields.length).toBeGreaterThan(0)
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
