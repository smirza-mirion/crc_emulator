/**
 * Chamber Volts Screen Tests
 *
 * Reference: Pictures_4.07c/ChamberVolts/English Prototype - DSC00650.JPG
 *
 * The Chamber Volts screen shows:
 * - Title: "Chamber Volts" (top center)
 * - Home button (top left), Back button (top right)
 * - Step: 1) Check Chamber Voltage → 155.0V OK
 * - Footer: "Ch: 1, R" label (no Setup button per reference)
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllStringFields,
  verifyResolution,
  verifyNavBar,
  verifyReferencedImages,
  getTriggersByByte,
} from '../screen-test-helpers'

describe('ChamberVolts', () => {
  const screen = loadScreenDef('ChamberVolts')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
      expect(screen.name).toBe('ChamberVolts')
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

  describe('Voltage Display', () => {
    it('should have step label for checking voltage', () => {
      // In ChamberVolts.json, content fields are at x:0, y:70
      // (visibility-controlled by triggers, not spatially arranged)
      const allFields = getAllStringFields(screen)
      const stepLabel = allFields.find(w =>
        w.name.toLowerCase().includes('chambervolts') ||
        w.name.toLowerCase().includes('volt') ||
        (w.y >= 70 && w.y < 200)
      )
      expect(stepLabel).toBeDefined()
    })

    it('should have voltage result field', () => {
      // Reference: "155.0V OK"
      // Result fields are also at x:0, y:70 (visibility-controlled)
      const allFields = getAllStringFields(screen)
      const resultField = allFields.find(w =>
        w.name.toLowerCase().includes('result') ||
        w.name.toLowerCase().includes('value') ||
        (w.y >= 70 && w.y < 530 && w !== allFields.find(f => f.y < 70))
      )
      expect(resultField).toBeDefined()
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
  })

  describe('Refresh Triggers', () => {
    it('should have triggers for voltage update', () => {
      expect(screen.refreshTriggers).toBeDefined()
      expect(screen.refreshTriggers!.length).toBeGreaterThan(0)
    })

    it('should have chamber label refresh trigger on byte(20)', () => {
      const triggers = getTriggersByByte(screen, 20)
      expect(triggers.length).toBeGreaterThan(0)
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
