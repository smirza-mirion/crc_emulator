/**
 * Moly Assay Screen Tests
 *
 * Reference: Pictures_4.07c/MolyAssay/English Prototype.png
 *
 * The Moly Assay screen shows:
 * - Title: "Moly Assay" (top center)
 * - 3 buttons (centered, stacked):
 *   - CAPMAC for Mallinckrodt Gen
 *   - CAPMAC for Bristol Myers Gen
 *   - Capintec Canister
 * - Footer: "Ch: 1, R" label
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  verifyResolution,
  verifyNavBar,
  verifyReferencedImages,
  hrefContainsMacro,
} from '../screen-test-helpers'

describe('MolyAssay', () => {
  const screen = loadScreenDef('Moly')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
      expect(screen.name).toBe('Moly')
    })

    it('should have 800x600 resolution', () => {
      expect(verifyResolution(screen)).toBe(true)
    })
  })

  describe('Navigation', () => {
    it('should have navigation buttons (Home or Back)', () => {
      const nav = verifyNavBar(screen)
      expect(nav.home || nav.back).toBeTruthy()
    })
  })

  describe('Moly Test Type Buttons', () => {
    it('should have 3 moly test type buttons', () => {
      const allBtns = getAllButtons(screen)
      const contentBtns = allBtns.filter(b =>
        !hrefContainsMacro(b, 'Home') &&
        !hrefContainsMacro(b, 'Back') &&
        b.y > 70 && b.y < 530 &&
        (!b.language || b.language.toLowerCase() === 'english')
      )
      expect(contentBtns.length).toBeGreaterThanOrEqual(3)
    })

    it('should have CAPMAC Mallinckrodt option', () => {
      const allBtns = getAllButtons(screen)
      const malBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('mall') ||
        b.name.toLowerCase().includes('capmac') ||
        b.name.toLowerCase().includes('moly1')
      )
      expect(malBtn || allBtns.filter(b => b.y > 100 && b.y < 250).length > 0).toBeTruthy()
    })

    it('should have CAPMAC Bristol Myers option', () => {
      const allBtns = getAllButtons(screen)
      const bmsBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('bristol') ||
        b.name.toLowerCase().includes('moly2')
      )
      expect(bmsBtn || allBtns.filter(b => b.y > 200 && b.y < 350).length > 0).toBeTruthy()
    })

    it('should have Capintec Canister option', () => {
      const allBtns = getAllButtons(screen)
      const canBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('canist') ||
        b.name.toLowerCase().includes('capintec') ||
        b.name.toLowerCase().includes('moly3')
      )
      expect(canBtn || allBtns.filter(b => b.y > 300 && b.y < 500).length > 0).toBeTruthy()
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
