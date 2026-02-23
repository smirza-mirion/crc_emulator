/**
 * Linearity Screen Tests
 *
 * Reference: Pictures_4.07c/Linearity/English Prototype - DSC00658.JPG
 *
 * The Linearity screen shows:
 * - Title: "Linearity" (top center)
 * - Home button (top left), Back button (top right)
 * - 4 navigation buttons (centered, stacked):
 *   - AutoLinearity
 *   - Standard
 *   - Lineator
 *   - Calicheck
 * - Footer: "Ch" button, "Ch: 1, R" label, Setup button
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  verifyResolution,
  verifyNavBar,
  getNavigationMap,
  verifyReferencedImages,
  hrefContainsMacro,
  verifyBottomBar,
} from '../screen-test-helpers'

describe('Linearity (SelectLinearity)', () => {
  // The linearity screen may be named SelectLinearity or Linearity
  let screen: ReturnType<typeof loadScreenDef>
  try {
    screen = loadScreenDef('SelectLinearity')
  } catch {
    screen = loadScreenDef('Linearity')
  }

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

  describe('Sub-Menu Navigation Buttons', () => {
    it('should have AutoLinearity button', () => {
      const allBtns = getAllButtons(screen)
      const autoLinBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('auto') ||
        (b.params?.href || '').includes('AUTOLINEARITY') ||
        (b.params?.href || '').includes('AutoLinearity')
      )
      expect(autoLinBtn).toBeDefined()
    })

    it('should have Standard button', () => {
      const allBtns = getAllButtons(screen)
      const stdBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('standard') ||
        (b.params?.href || '').includes('STANDARD') ||
        (b.params?.href || '').includes('Standard')
      )
      expect(stdBtn).toBeDefined()
    })

    it('should have Lineator button', () => {
      const allBtns = getAllButtons(screen)
      const lineatorBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('lineator') ||
        (b.params?.href || '').includes('LINEATOR') ||
        (b.params?.href || '').includes('Lineator')
      )
      expect(lineatorBtn).toBeDefined()
    })

    it('should have Calicheck button', () => {
      const allBtns = getAllButtons(screen)
      const caliBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('cali') ||
        (b.params?.href || '').includes('CALICHECK') ||
        (b.params?.href || '').includes('Calicheck')
      )
      expect(caliBtn).toBeDefined()
    })

    it('should have at least 4 navigation targets', () => {
      const allBtns = getAllButtons(screen)
      const navBtns = allBtns.filter(b => {
        const href = b.params?.href || ''
        return !href.includes('%Home') && !href.includes('%Back') &&
               href.length > 0 &&
               (!b.language || b.language.toLowerCase() === 'english')
      })
      expect(navBtns.length).toBeGreaterThanOrEqual(4)
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
