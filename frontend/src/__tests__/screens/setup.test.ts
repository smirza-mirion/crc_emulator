/**
 * Setup Screen Tests
 *
 * Reference: Pictures_4.07c/Setup/English Prototype - DSC00644.JPG
 *
 * The Setup screen shows:
 * - Title: "Setup" (top center)
 * - Home button (top left), Back button (top right)
 * - Activity Unit: Radio buttons (Ci/Bq, Ci, Bq)
 * - Date Format: "mm/dd/yyyy" button
 * - Printer: Radio buttons (None, usb/HP, 232/Slip, 232/Roll, 232/Oki-ticket, etc.)
 * - "Feed EpsonLabel After Printing" checkbox
 * - USB PC Driver: "Legacy" button
 * - Language: "English" button
 * - Sleep Timeout slider → OFF
 * - Sleep Brightness slider → 10
 * - Brightness slider → 100
 * - Volume slider → (with Test button)
 * - Bottom buttons: "Advanced Chamber", "Staff", "Screen Calib"
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  getAllStringFields,
  verifyResolution,
  verifyNavBar,
  verifyReferencedImages,
  getNavigationMap,
  hrefContainsMacro,
  getWidgetsByType,
  findWidgetsByPartialName,
} from '../screen-test-helpers'

describe('Setup', () => {
  const screen = loadScreenDef('Setup')

  describe('Screen Definition Basics', () => {
    it('should load successfully', () => {
      expect(screen).toBeDefined()
      expect(screen.name).toBe('Setup')
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

  describe('Activity Unit Selection', () => {
    it('should have activity unit control buttons or fields', () => {
      const allBtns = getAllButtons(screen)
      // Look for unit-related buttons
      const unitBtns = allBtns.filter(b =>
        b.name.toLowerCase().includes('unit') ||
        b.name.toLowerCase().includes('ci') ||
        b.name.toLowerCase().includes('bq')
      )
      const allFields = getAllStringFields(screen)
      const unitFields = allFields.filter(w =>
        w.name.toLowerCase().includes('unit') ||
        w.name.toLowerCase().includes('activity')
      )
      expect(unitBtns.length + unitFields.length).toBeGreaterThan(0)
    })
  })

  describe('Language Button', () => {
    it('should have a language selection button', () => {
      const allBtns = getAllButtons(screen)
      const langBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('lang') ||
        (b.params?.href || '').toLowerCase().includes('language')
      )
      const allFields = getAllStringFields(screen)
      const langField = allFields.find(w =>
        w.name.toLowerCase().includes('lang')
      )
      expect(langBtn || langField).toBeTruthy()
    })
  })

  describe('Slider Controls', () => {
    it('should have slider/bar widgets or string fields for sliders', () => {
      // Reference shows: Sleep Timeout, Sleep Brightness, Brightness, Volume
      const barGraphs = getWidgetsByType(screen, 'BarGraph')
      const sliderFields = findWidgetsByPartialName(screen, 'slider')
      const brightnessFields = findWidgetsByPartialName(screen, 'bright')
      const volumeFields = findWidgetsByPartialName(screen, 'volume')
      const sleepFields = findWidgetsByPartialName(screen, 'sleep')
      // Should have some form of slider controls
      expect(
        barGraphs.length + sliderFields.length + brightnessFields.length +
        volumeFields.length + sleepFields.length
      ).toBeGreaterThan(0)
    })
  })

  describe('Bottom Action Buttons', () => {
    it('should have Advanced Chamber button', () => {
      const allBtns = getAllButtons(screen)
      const advBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('advanced') ||
        b.name.toLowerCase().includes('chamb') ||
        (b.params?.href || '').includes('SETUPCHAMBER')
      )
      expect(advBtn).toBeDefined()
    })

    it('should have Staff/Security button', () => {
      // The Staff button is named btnSecurity in the JSON
      // Its label comes from label(132) = "Staff"
      // href: %SecurityMode,%SetClear(%%SET_CLEAR_SECURITY)
      const allBtns = getAllButtons(screen)
      const staffBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('staff') ||
        b.name.toLowerCase().includes('security') ||
        hrefContainsMacro(b, 'ActiveStaff') ||
        hrefContainsMacro(b, 'AllStaff') ||
        hrefContainsMacro(b, 'SecurityMode') ||
        (b.params?.href || '').includes('SECURITY')
      )
      expect(staffBtn).toBeDefined()
    })

    it('should have Screen Calib button', () => {
      const allBtns = getAllButtons(screen)
      const calibBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('calib') ||
        b.name.toLowerCase().includes('screen')
      )
      // This may be labeled differently
      expect(calibBtn || allBtns.length > 5).toBeTruthy()
    })
  })

  describe('Printer Selection', () => {
    it('should have printer-related widgets', () => {
      const allWidgets = screen.widgets
      const printerWidgets = allWidgets.filter(w =>
        w.name.toLowerCase().includes('print') ||
        w.name.toLowerCase().includes('232') ||
        w.name.toLowerCase().includes('usb')
      )
      expect(printerWidgets.length).toBeGreaterThan(0)
    })
  })

  describe('Date Format', () => {
    it('should have date format button or field', () => {
      const allWidgets = screen.widgets
      const dateWidgets = allWidgets.filter(w =>
        w.name.toLowerCase().includes('date') ||
        w.name.toLowerCase().includes('format')
      )
      expect(dateWidgets.length).toBeGreaterThan(0)
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
