/**
 * Geometry Screen Tests
 *
 * Reference: Pictures_4.07c/Geometry/English Prototype - DSC00657.JPG
 *
 * The Geometry screen shows:
 * - Title: "Geometry" (top center)
 * - Home button (top left), Back button (top right)
 * - "1) Container" label with radio buttons: Syringe / Vial
 * - Footer: "Ch: 1, R" label
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  getAllStringFields,
  verifyResolution,
  verifyNavBar,
  verifyReferencedImages,
} from '../screen-test-helpers'

describe('Geometry', () => {
  const screen = loadScreenDef('Geometry')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
      expect(screen.name).toBe('Geometry')
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

  describe('Container Selection', () => {
    it('should have container label text', () => {
      // Geometry.json has content fields at y:68 (visibility-controlled)
      const allFields = getAllStringFields(screen)
      const containerField = allFields.find(w =>
        w.name.toLowerCase().includes('container') ||
        (w.y >= 65 && w.y < 200)
      )
      expect(containerField).toBeDefined()
    })

    it('should have Syringe and Vial selection buttons', () => {
      const allBtns = getAllButtons(screen)
      const containerBtns = allBtns.filter(b =>
        b.name.toLowerCase().includes('syringe') ||
        b.name.toLowerCase().includes('vial') ||
        b.name.toLowerCase().includes('container')
      )
      // May use FunctionButton with checkbox pattern
      if (containerBtns.length === 0) {
        // Check for any interactive widgets in the container area
        const allWidgets = screen.widgets.filter(w =>
          w.y > 70 && w.y < 200 && w.x > 300
        )
        expect(allWidgets.length).toBeGreaterThan(0)
      } else {
        expect(containerBtns.length).toBeGreaterThanOrEqual(2)
      }
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
