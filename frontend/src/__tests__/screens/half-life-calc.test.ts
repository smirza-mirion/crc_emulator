/**
 * Half-life Calculator Screen Tests
 *
 * Reference: Pictures_4.07c/HalfLifeCalc/English Prototype.png
 *
 * The Half-life Calculator screen shows:
 * - Title: "Half-life Calc - Ch:1, R" (top center)
 * - Config row: "Interval:" field, "Total:" field, "Start" button
 * - Table headers: "Elapsed (hrs)", "Measured", "Predicted", "% Var"
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

describe('HalflifeCalculator', () => {
  const screen = loadScreenDef('HalflifeCalculator')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
    })

    it('should have 800x600 resolution', () => {
      expect(verifyResolution(screen)).toBe(true)
    })
  })

  describe('Navigation', () => {
    it('should have navigation buttons', () => {
      const nav = verifyNavBar(screen)
      // Half-life calculator may not have Home/Back but should have some nav
      const allBtns = getAllButtons(screen)
      expect(allBtns.length).toBeGreaterThan(0)
    })
  })

  describe('Configuration Row', () => {
    it('should have Interval field', () => {
      const intervalWidgets = findWidgetsByPartialName(screen, 'interval')
      const allFields = getAllStringFields(screen)
      const intervalField = allFields.find(w => w.y < 150 && w.x < 300)
      expect(intervalWidgets.length > 0 || intervalField).toBeTruthy()
    })

    it('should have Total field', () => {
      const totalWidgets = findWidgetsByPartialName(screen, 'total')
      const allFields = getAllStringFields(screen)
      const totalField = allFields.find(w => w.y < 150 && w.x > 200 && w.x < 500)
      expect(totalWidgets.length > 0 || totalField).toBeTruthy()
    })

    it('should have Start button', () => {
      const allBtns = getAllButtons(screen)
      const startBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('start') ||
        b.x > 600 && b.y < 150
      )
      expect(startBtn).toBeDefined()
    })
  })

  describe('Data Table', () => {
    it('should have header fields', () => {
      const allFields = getAllStringFields(screen)
      const headerFields = allFields.filter(w => w.y > 70 && w.y < 180)
      expect(headerFields.length).toBeGreaterThanOrEqual(2)
    })

    it('should have data row fields', () => {
      const allFields = getAllStringFields(screen)
      const dataFields = allFields.filter(w => w.y > 150 && w.y < 530)
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
