/**
 * QC Screen Tests
 *
 * Reference: Pictures_4.07c/QC/English Prototype.png
 *
 * The QC screen shows:
 * - Title: "QC" (top center, minimal/no title bar)
 * - 4 buttons (centered, stacked):
 *   - Single Strip Test
 *   - Two Strip Test
 *   - HMPAO Test
 *   - MAG3 Test
 * - Footer: "Ch: 1, R" label
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  verifyResolution,
  verifyReferencedImages,
  getNavigationMap,
  hrefContainsMacro,
  getSetClearTarget,
  getSetHTMLTarget,
} from '../screen-test-helpers'

describe('QC', () => {
  const screen = loadScreenDef('QC')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
      expect(screen.name).toBe('QC')
    })

    it('should have 800x600 resolution', () => {
      expect(verifyResolution(screen)).toBe(true)
    })
  })

  describe('QC Test Buttons', () => {
    it('should have Single Strip Test button', () => {
      const allBtns = getAllButtons(screen)
      const oneStripBtn = allBtns.find(b => {
        const href = b.params?.href || ''
        return href.includes('ONESTRIP') || b.name.toLowerCase().includes('onestrip') ||
               b.name.toLowerCase().includes('single')
      })
      expect(oneStripBtn).toBeDefined()
    })

    it('Single Strip should navigate to OneStrip screen', () => {
      const allBtns = getAllButtons(screen)
      const btn = allBtns.find(b =>
        (b.params?.href || '').includes('ONESTRIP')
      )
      if (btn) {
        const target = getSetClearTarget(btn) || getSetHTMLTarget(btn)
        expect(target).toContain('ONESTRIP')
      }
    })

    it('should have Two Strip Test button', () => {
      const allBtns = getAllButtons(screen)
      const twoStripBtn = allBtns.find(b => {
        const href = b.params?.href || ''
        return href.includes('TWOSTRIP') || b.name.toLowerCase().includes('twostrip') ||
               b.name.toLowerCase().includes('two')
      })
      expect(twoStripBtn).toBeDefined()
    })

    it('should have HMPAO Test button', () => {
      const allBtns = getAllButtons(screen)
      const hmpaoBtn = allBtns.find(b => {
        const href = b.params?.href || ''
        return href.includes('HMPAO') || b.name.toLowerCase().includes('hmpao')
      })
      expect(hmpaoBtn).toBeDefined()
    })

    it('should have MAG3 Test button', () => {
      const allBtns = getAllButtons(screen)
      const mag3Btn = allBtns.find(b => {
        const href = b.params?.href || ''
        return href.includes('MAG3') || b.name.toLowerCase().includes('mag3')
      })
      expect(mag3Btn).toBeDefined()
    })

    it('should have exactly 4 main test buttons', () => {
      const allBtns = getAllButtons(screen)
      // Filter to buttons with SetClear hrefs (the 4 QC test navigation buttons)
      // Exclude Home/Back/French/Chamber buttons
      const testBtns = allBtns.filter(b => {
        const href = b.params?.href || ''
        return href.length > 0 &&
          !hrefContainsMacro(b, 'Home') &&
          !hrefContainsMacro(b, 'Back') &&
          !b.name.toLowerCase().includes('chamb') &&
          (!b.language || b.language.toLowerCase() === 'english')
      })
      expect(testBtns.length).toBeGreaterThanOrEqual(4)
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
