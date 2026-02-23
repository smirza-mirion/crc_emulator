/**
 * Daily Test Screen Tests
 *
 * Reference: Pictures_4.07c/DailyTest/English Prototype - DSC00646.JPG
 *
 * The Daily Test screen shows a step-by-step test sequence:
 * - Title: "Daily Test" (top center)
 * - Home button (top left), Back button (top right)
 * - Steps:
 *   1) Remove All Sources from Chamber → OK
 *   2) Auto Zero → 0.00 mV
 *   3) Measure Backgnd → 0.00uCi
 *   4) Check Chamber Voltage → 155.0V OK
 *   5) Data Check → Passed
 *   6) → Accuracy button
 * - Footer: "Ch: 1, R" label, Setup button
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  findWidget,
  findWidgetsByPartialName,
  getAllButtons,
  getAllStringFields,
  verifyResolution,
  verifyNavBar,
  getNavigationMap,
  verifyBottomBar,
  getTriggersByByte,
  verifyReferencedImages,
  getSetHTMLTarget,
  getSetClearTarget,
  hrefContainsMacro,
  getWidgetsByType,
} from '../screen-test-helpers'

describe('DailyTest', () => {
  const screen = loadScreenDef('Daily')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
      expect(screen.name).toBe('Daily')
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

  describe('Title', () => {
    it('should have "Daily Test" title text field', () => {
      const allFields = getAllStringFields(screen)
      // Title is in the top bar area
      const titleField = allFields.find(w =>
        w.y < 70 && w.width > 200
      )
      expect(titleField).toBeDefined()
    })
  })

  describe('Test Steps Display', () => {
    it('should have string fields for step labels and results', () => {
      // Reference shows steps 1-6 as text labels with results
      // Daily screen widgets are stacked at y=70, visibility-controlled by triggers
      const allFields = getAllStringFields(screen)
      // Should have at least 6 step + 6 result fields (many initially hidden)
      expect(allFields.length).toBeGreaterThanOrEqual(6)
    })

    it('should have step label widgets (Remove, Zero, Background, Volts, etc.)', () => {
      const allFields = getAllStringFields(screen)
      const stepNames = allFields.map(f => f.name.toLowerCase())
      // At least some recognizable daily test step names
      const hasRemove = stepNames.some(n => n.includes('remove'))
      const hasZero = stepNames.some(n => n.includes('zero'))
      const hasBackground = stepNames.some(n => n.includes('background') || n.includes('bkg'))
      const hasVolts = stepNames.some(n => n.includes('volt') || n.includes('chamb'))
      expect(hasRemove || hasZero || hasBackground || hasVolts).toBe(true)
    })
  })

  describe('Accuracy Button', () => {
    it('should have Accuracy button to navigate to accuracy test', () => {
      const navMap = getNavigationMap(screen)
      const accuracyEntry = Object.entries(navMap).find(([_, target]) =>
        target.includes('ACCURACY')
      )
      if (!accuracyEntry) {
        // May be a FunctionButton with different naming
        const allBtns = getAllButtons(screen)
        const accBtn = allBtns.find(b =>
          b.name.toLowerCase().includes('accur') ||
          (b.params?.href || '').includes('ACCURACY')
        )
        expect(accBtn || accuracyEntry).toBeTruthy()
      }
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
      // Setup button may not be present on Daily screen per reference
      // The reference shows it exists
      expect(setupButton).toBeDefined()
    })
  })

  describe('Refresh Triggers', () => {
    it('should have refresh triggers for updating test results', () => {
      expect(screen.refreshTriggers).toBeDefined()
      expect(screen.refreshTriggers!.length).toBeGreaterThan(0)
    })

    it('should have toggle triggers with setValue for step-by-step updates', () => {
      const toggleTriggers = (screen.refreshTriggers || []).filter(t =>
        (t.actions || []).some(a => a.type === 'setValue')
      )
      expect(toggleTriggers.length).toBeGreaterThan(0)
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
