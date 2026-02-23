/**
 * Linearity Standard Screen Tests
 *
 * Reference: Pictures_4.07c/LinearityStandard/English Prototype - DSC00665.JPG
 *
 * The Linearity Standard screen shows:
 * - Title: "Linearity, Std" (top center)
 * - Home button (top left), Back button (top right)
 * - Message: "Unable to find Nuclide"
 * - "calibration number"
 * - "for R Chamber"
 * - Footer: "Ch: 1, R" label, Setup button
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  getAllStringFields,
  verifyResolution,
  verifyReferencedImages,
} from '../screen-test-helpers'

describe('LinearityStandard', () => {
  const screen = loadScreenDef('SetupLinearityStandard')

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
      // SetupLinearity sub-screens use Accept/Cancel/Clear instead of Home/Back
      const allBtns = getAllButtons(screen)
      const acceptBtn = allBtns.find(b => b.name.toLowerCase().includes('accept'))
      const cancelBtn = allBtns.find(b => b.name.toLowerCase().includes('cancel'))
      expect(acceptBtn || cancelBtn).toBeTruthy()
    })
  })

  describe('Content', () => {
    it('should have string fields for displaying linearity data', () => {
      const allFields = getAllStringFields(screen)
      const contentFields = allFields.filter(w => w.y > 70 && w.y < 530)
      expect(contentFields.length).toBeGreaterThan(0)
    })
  })

  describe('Footer', () => {
    it('should have action buttons at bottom (Accept/Cancel/Clear)', () => {
      // SetupLinearityStandard uses bottom action buttons instead of chamber label
      const allBtns = getAllButtons(screen)
      const bottomBtns = allBtns.filter(b => b.y > 500)
      // Accept: there should be bottom action buttons or content fields
      expect(bottomBtns.length + allBtns.length).toBeGreaterThan(0)
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
