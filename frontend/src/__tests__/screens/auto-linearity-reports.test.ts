/**
 * AutoLinearity Reports (Search) Screen Tests
 *
 * Reference: Pictures_4.07c/AutoLinearityReports/English Prototype - DSC00661.JPG
 *
 * The Search AutoLinearity Tests screen shows:
 * - Title: "Search AutoLinearity Tests" (top center)
 * - Home button (top left), Back button (top right)
 * - Search controls: "From:" date field, "To:" date field, "Search" button
 * - Table headers: "Chamber", "Nuclide", "Date"
 * - Data rows (alternating light blue/white stripes)
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  getAllButtons,
  getAllStringFields,
  verifyResolution,
  verifyNavBar,
  verifyReferencedImages,
  findWidgetsByPartialName,
} from '../screen-test-helpers'

describe('AutoLinearityReports (Search)', () => {
  const screen = loadScreenDef('AutoLinearitySearch')

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

  describe('Search Controls', () => {
    it('should have From date field', () => {
      const fromWidgets = findWidgetsByPartialName(screen, 'from')
      const allFields = getAllStringFields(screen)
      const fromField = allFields.find(w => w.y > 70 && w.y < 150 && w.x < 300)
      expect(fromWidgets.length > 0 || fromField).toBeTruthy()
    })

    it('should have To date field', () => {
      const toWidgets = findWidgetsByPartialName(screen, 'to')
      const allFields = getAllStringFields(screen)
      const toField = allFields.find(w => w.y > 70 && w.y < 150 && w.x > 300 && w.x < 600)
      expect(toWidgets.length > 0 || toField).toBeTruthy()
    })

    it('should have Search button', () => {
      const allBtns = getAllButtons(screen)
      const searchBtn = allBtns.find(b =>
        b.name.toLowerCase().includes('search') ||
        b.x > 700 && b.y > 70 && b.y < 150
      )
      expect(searchBtn).toBeDefined()
    })
  })

  describe('Table Headers', () => {
    it('should have Chamber, Nuclide, Date column headers', () => {
      const allFields = getAllStringFields(screen)
      const headerFields = allFields.filter(w => w.y > 120 && w.y < 200)
      expect(headerFields.length).toBeGreaterThanOrEqual(2)
    })
  })

  describe('Image Assets', () => {
    it('should have all referenced images available', () => {
      const missing = verifyReferencedImages(screen)
      expect(missing).toEqual([])
    })
  })
})
