/**
 * Accuracy Screen Tests
 *
 * Reference: Pictures_4.07c/Accuracy/English Prototype - DSC00652.JPG
 *
 * The Accuracy screen shows:
 * - Title: "Accuracy" (top center)
 * - Home button (top left), Back button (top right)
 * - 5 nuclide rows, each with:
 *   - Nuclide name (Co57, Co60, Ba133, Cs137, Na22)
 *   - S/N label (e.g., "S/N: Co57SN")
 *   - "Measured:" label with input field
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
  getTriggersByByte,
  getWidgetsByType,
} from '../screen-test-helpers'

describe('Accuracy', () => {
  const screen = loadScreenDef('Accuracy')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
      expect(screen.name).toBe('Accuracy')
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

  describe('Nuclide Rows', () => {
    it('should have string fields for 5 nuclide names', () => {
      // Reference: Co57, Co60, Ba133, Cs137, Na22
      const allFields = getAllStringFields(screen)
      const nuclideNameFields = allFields.filter(w =>
        w.y > 70 && w.y < 530 && w.x < 200
      )
      expect(nuclideNameFields.length).toBeGreaterThanOrEqual(5)
    })

    it('should have "Measured:" labels and input fields', () => {
      const allFields = getAllStringFields(screen)
      const measuredFields = allFields.filter(w =>
        w.y > 70 && w.y < 530 && w.x > 300
      )
      // At least 5 measurement result fields
      expect(measuredFields.length).toBeGreaterThanOrEqual(5)
    })

    it('should have S/N label fields for each nuclide', () => {
      const allFields = getAllStringFields(screen)
      // Serial number fields should be smaller text below the nuclide name
      const snFields = allFields.filter(w =>
        w.y > 80 && w.y < 530 && w.x < 250 &&
        w.height < 30
      )
      expect(snFields.length).toBeGreaterThanOrEqual(3)
    })
  })

  describe('Footer', () => {
    it('should have chamber label', () => {
      const allFields = getAllStringFields(screen)
      const chambField = allFields.find(w =>
        w.y > 530 && (
          w.name.toLowerCase().includes('chamb') ||
          (w.params?.href || '').includes('string(9)')
        )
      )
      expect(chambField).toBeDefined()
    })

    it('should have Setup button', () => {
      const { setupButton } = verifyBottomBar(screen)
      expect(setupButton).toBeDefined()
    })
  })

  describe('Refresh Triggers', () => {
    it('should have refresh triggers for measurement updates', () => {
      expect(screen.refreshTriggers).toBeDefined()
      expect(screen.refreshTriggers!.length).toBeGreaterThan(0)
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
