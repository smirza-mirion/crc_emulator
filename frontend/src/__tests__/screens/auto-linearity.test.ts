/**
 * AutoLinearity Screen Tests
 *
 * Reference: Pictures_4.07c/AutoLinearity/English Prototype.png
 *
 * The AutoLinearity screen shows:
 * - Title: "AutoLinearity" (top center)
 * - Home button (top left), Back button (top right)
 * - 2 navigation buttons (centered):
 *   - Start Test
 *   - Reports
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
  getSetClearTarget,
} from '../screen-test-helpers'

describe('AutoLinearity', () => {
  const screen = loadScreenDef('AutoLinearity')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
      expect(screen.name).toBe('AutoLinearity')
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

  describe('Action Buttons', () => {
    it('should have Start Test button', () => {
      const allBtns = getAllButtons(screen)
      const startBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('start') ||
        (b.params?.href || '').includes('AUTOLINEARITYTEST')
      )
      expect(startBtn).toBeDefined()
    })

    it('should have Reports button', () => {
      const allBtns = getAllButtons(screen)
      const reportsBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('report') ||
        (b.params?.href || '').includes('AUTOLINEAIRTYSEARCH') ||
        (b.params?.href || '').includes('AUTOLINEARITYSEARCH')
      )
      expect(reportsBtn).toBeDefined()
    })

    it('Start Test should navigate to AutoLinearityTest', () => {
      const allBtns = getAllButtons(screen)
      const startBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('start') ||
        (b.params?.href || '').includes('AUTOLINEARITYTEST')
      )
      if (startBtn) {
        const target = getSetClearTarget(startBtn)
        expect(target).toContain('AUTOLINEARITYTEST')
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
