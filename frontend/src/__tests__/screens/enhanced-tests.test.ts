/**
 * Enhanced Tests Screen Tests
 *
 * Reference: Pictures_4.07c/EnhancedTests/English Prototype - DSC00654.JPG
 *
 * The Enhanced Tests screen shows:
 * - Title: "Enhanced Tests" (top center)
 * - Home button (top left), Back button (top right)
 * - 4 navigation buttons (centered, stacked vertically):
 *   - Geometry
 *   - Linearity
 *   - Half-life Calculator
 *   - QC
 * - Footer: "Ch" button, "Ch: 1, R" label
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  verifyResolution,
  verifyNavBar,
  getNavigationMap,
  verifyReferencedImages,
  getSetHTMLTarget,
  getSetClearTarget,
  hrefContainsMacro,
} from '../screen-test-helpers'

describe('EnhancedTests', () => {
  const screen = loadScreenDef('Enhanced')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
      expect(screen.name).toBe('Enhanced')
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

  describe('Sub-Menu Navigation Buttons', () => {
    it('should have Geometry button', () => {
      const navMap = getNavigationMap(screen)
      const geoEntry = Object.entries(navMap).find(([_, target]) =>
        target.includes('GEOMETRY')
      )
      expect(geoEntry).toBeDefined()
    })

    it('should have Linearity button', () => {
      const navMap = getNavigationMap(screen)
      const linEntry = Object.entries(navMap).find(([_, target]) =>
        target.includes('LINEARITY') || target.includes('SELECT_LINEARITY')
      )
      if (!linEntry) {
        // May use SelectLinearity as intermediate
        const allBtns = getAllButtons(screen)
        const linBtn = allBtns.find(b =>
          b.name.toLowerCase().includes('linear') ||
          (b.params?.href || '').toLowerCase().includes('linear')
        )
        expect(linBtn || linEntry).toBeTruthy()
      }
    })

    it('should have Half-life Calculator button', () => {
      const allBtns = getAllButtons(screen)
      const halfLifeBtn = allBtns.find(b => {
        const href = b.params?.href || ''
        return href.includes('HALFLIFE') || href.includes('HalfLife') ||
               b.name.toLowerCase().includes('halflife') || b.name.toLowerCase().includes('half')
      })
      expect(halfLifeBtn).toBeDefined()
    })

    it('should have QC button', () => {
      const navMap = getNavigationMap(screen)
      const qcEntry = Object.entries(navMap).find(([_, target]) =>
        target.includes('QC')
      )
      expect(qcEntry).toBeDefined()
    })

    it('should have at least 4 navigation targets (Geometry, Linearity, HalfLife, QC)', () => {
      const allBtns = getAllButtons(screen)
      // Count buttons that navigate to sub-screens (not Home/Back)
      const navBtns = allBtns.filter(b => {
        const href = b.params?.href || ''
        return !href.includes('%Home') && !href.includes('%Back') &&
               href.length > 0 &&
               (!b.language || b.language.toLowerCase() === 'english')
      })
      expect(navBtns.length).toBeGreaterThanOrEqual(4)
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
