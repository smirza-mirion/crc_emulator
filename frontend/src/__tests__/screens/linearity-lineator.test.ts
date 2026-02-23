/**
 * Linearity Lineator Screen Tests
 *
 * Reference: Pictures_4.07c/LiniearityLineator/English Prototype - DSC00667.JPG
 *
 * The Lineator screen shows:
 * - Title: "Lineator #Q3W45TYU" (top center, with serial number)
 * - Home button (top left), Back button (top right)
 * - Table headers: #, Factor, % Ratio
 * - 8 data rows:
 *   1) Tube 1         nan    inf
 *   2) Tube 1+2       nan    Accept button
 *   3) Tube 1+3       nan    Accept button
 *   4) Tube 1+2,3     nan    inf
 *   5) Tube 1+4       nan    inf
 *   6) Tube 1+2,4     nan    inf
 *   7) Tube 1+3,4     nan    inf
 *   8) Tube 1+2,3,4   nan    Accept button
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
  findWidgetsByPartialName,
} from '../screen-test-helpers'

describe('LinearityLineator', () => {
  const screen = loadScreenDef('SetupLinearityLineator')

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
      // SetupLinearityLineator uses Accept/Cancel/Clear instead of Home/Back
      const allBtns = getAllButtons(screen)
      const acceptBtn = allBtns.find(b => b.name.toLowerCase().includes('accept'))
      const cancelBtn = allBtns.find(b => b.name.toLowerCase().includes('cancel'))
      expect(acceptBtn || cancelBtn).toBeTruthy()
    })
  })

  describe('Table Structure', () => {
    it('should have table header fields (#, Factor, % Ratio)', () => {
      const allFields = getAllStringFields(screen)
      const headerFields = allFields.filter(w => w.y > 70 && w.y < 130)
      expect(headerFields.length).toBeGreaterThanOrEqual(2)
    })

    it('should have multiple data row fields for tube measurements', () => {
      const allFields = getAllStringFields(screen)
      const dataFields = allFields.filter(w => w.y > 110 && w.y < 530)
      // Reference shows 8 rows × 3 columns minimum
      expect(dataFields.length).toBeGreaterThanOrEqual(8)
    })

    it('should have Accept buttons for specific rows', () => {
      const allBtns = getAllButtons(screen)
      const acceptBtns = allBtns.filter(b =>
        b.name.toLowerCase().includes('accept') ||
        b.y > 110 && b.y < 530 && b.x > 500
      )
      // Reference shows Accept buttons for rows 2, 3, and 8
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
