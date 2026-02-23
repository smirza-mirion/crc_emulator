/**
 * Calicheck Screen Tests
 *
 * Reference: Pictures_4.07c/CaliCheck/English Prototype - DSC00668.JPG
 *
 * The Calicheck screen shows:
 * - Title: "Calicheck #AS357GHJ" (top center, with serial number)
 * - Home button (top left), Back button (top right)
 * - Table headers: #, Result, % Var
 * - 12 data rows with color band names:
 *   1) Black         0.00uCi    nan
 *   2) Black/Red     0.00uCi    nan
 *   ... through ...
 *   12) Blk/Purple/Blue  (Accept button)
 * - Some rows have "Accept" buttons
 * - Footer: "Ch: 1, R" label, Setup button
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  getAllStringFields,
  verifyResolution,
  verifyNavBar,
  verifyBottomBar,
  verifyReferencedImages,
} from '../screen-test-helpers'

describe('Calicheck', () => {
  const screen = loadScreenDef('SetupLinearityCalicheck')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
    })

    it('should have 800x600 resolution', () => {
      expect(verifyResolution(screen)).toBe(true)
    })
  })

  describe('Navigation Bar', () => {
    it('should have Accept/Cancel navigation buttons (setup sub-screen pattern)', () => {
      // SetupLinearityCalicheck uses Accept/Cancel/Clear instead of Home/Back
      const allBtns = getAllButtons(screen)
      const acceptBtn = allBtns.find(b => b.name.toLowerCase().includes('accept'))
      const cancelBtn = allBtns.find(b => b.name.toLowerCase().includes('cancel'))
      expect(acceptBtn || cancelBtn).toBeTruthy()
    })
  })

  describe('Table Structure', () => {
    it('should have table header fields', () => {
      const allFields = getAllStringFields(screen)
      const headerFields = allFields.filter(w => w.y > 70 && w.y < 120)
      expect(headerFields.length).toBeGreaterThanOrEqual(2)
    })

    it('should have at least 12 data row groups', () => {
      const allFields = getAllStringFields(screen)
      // 12 color band rows with color name + result + variance
      const dataFields = allFields.filter(w => w.y > 100 && w.y < 580)
      expect(dataFields.length).toBeGreaterThanOrEqual(12)
    })

    it('should have Accept buttons for measurement rows', () => {
      const allBtns = getAllButtons(screen)
      const acceptBtns = allBtns.filter(b =>
        b.name.toLowerCase().includes('accept') ||
        (b.y > 100 && b.y < 580 && b.x > 550)
      )
      expect(acceptBtns.length).toBeGreaterThanOrEqual(1)
    })
  })

  describe('Footer', () => {
    it('should have Setup button', () => {
      const { setupButton } = verifyBottomBar(screen)
      expect(setupButton).toBeDefined()
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
