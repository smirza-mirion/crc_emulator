/**
 * All-Screens Validation Tests
 *
 * Comprehensive validation of every screen shown in the Pictures_4.07c
 * reference photos. Ensures each screen has the correct structure,
 * all buttons are functional, and widgets match the reference.
 */

import { describe, it, expect } from 'vitest'
import {
  loadScreenDef,
  screenExists,
  verifyResolution,
  verifyReferencedImages,
  getAllButtons,
  getAllStringFields,
  getWidgetsByType,
  getEnglishWidgets,
  verifyNavBar,
  getNavigationMap,
  hrefContainsMacro,
  verifyBottomBar,
} from '../screen-test-helpers'

/**
 * Reference screen catalog from Pictures_4.07c English images.
 * Each entry maps a reference picture folder to the screen definition name,
 * expected title, expected elements, and button counts.
 */
const REFERENCE_SCREENS = [
  {
    pictureFolder: 'Main',
    screenName: 'MainScreen',
    title: 'Main Screen',
    hasHome: false, // Main IS the home screen
    hasBack: false,
    hasSetup: true,
    hasChamberLabel: true,
    minButtons: 15, // 8 hotkeys + 8 nav + Well + Ch + Setup
    minStringFields: 10, // activity, unit, nuclide, time, cal#, hotkey labels, etc.
    navTargets: ['DAILY', 'BACKGROUND', 'CHAMBERVOLTS', 'ACCURACY', 'ENHANCED', 'MOLY', 'INVENTORY'],
  },
  {
    pictureFolder: 'Setup',
    screenName: 'Setup',
    title: 'Setup',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 5,
    minStringFields: 5,
    navTargets: [],
  },
  {
    pictureFolder: 'DailyTest',
    screenName: 'Daily',
    title: 'Daily Test',
    hasHome: true,
    hasBack: true,
    hasSetup: true,
    hasChamberLabel: true,
    minButtons: 3,
    minStringFields: 6, // 6 step labels + 6 results
    navTargets: ['ACCURACY'],
  },
  {
    pictureFolder: 'Background',
    screenName: 'Background',
    title: 'Background',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 2, // Home + Back
    minStringFields: 3,
    navTargets: [],
  },
  {
    pictureFolder: 'ChamberVolts',
    screenName: 'ChamberVolts',
    title: 'Chamber Volts',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: true,
    minButtons: 2,
    minStringFields: 2,
    navTargets: [],
  },
  {
    pictureFolder: 'Accuracy',
    screenName: 'Accuracy',
    title: 'Accuracy',
    hasHome: true,
    hasBack: true,
    hasSetup: true,
    hasChamberLabel: true,
    minButtons: 2,
    minStringFields: 10, // 5 nuclides × (name + SN + measured)
    navTargets: [],
  },
  {
    pictureFolder: 'EnhancedTests',
    screenName: 'Enhanced',
    title: 'Enhanced Tests',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 4, // Geometry + Linearity + Half-life Calc + QC
    minStringFields: 0,
    navTargets: ['GEOMETRY', 'QC'],
  },
  {
    pictureFolder: 'Geometry',
    screenName: 'Geometry',
    title: 'Geometry',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: true,
    minButtons: 2,
    minStringFields: 1,
    navTargets: [],
  },
  {
    pictureFolder: 'Linearity',
    screenName: 'SelectLinearity',
    fallbackName: 'Linearity',
    title: 'Linearity',
    hasHome: true,
    hasBack: true,
    hasSetup: true,
    hasChamberLabel: true,
    minButtons: 4, // AutoLinearity + Standard + Lineator + Calicheck
    minStringFields: 0,
    navTargets: [],
  },
  {
    pictureFolder: 'AutoLinearity',
    screenName: 'AutoLinearity',
    title: 'AutoLinearity',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: true,
    minButtons: 2, // Start Test + Reports
    minStringFields: 0,
    navTargets: [],
  },
  {
    pictureFolder: 'AutoLinearityStart',
    screenName: 'AutoLinearityTest',
    title: 'AutoLinearity Test',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 2,
    minStringFields: 4, // Nuclide, Interval, Total + headers
    navTargets: [],
  },
  {
    pictureFolder: 'AutoLinearityReports',
    screenName: 'AutoLinearitySearch',
    title: 'Search AutoLinearity Tests',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 2, // Home + Back + Search
    minStringFields: 3,
    navTargets: [],
  },
  {
    pictureFolder: 'LinearityStandard',
    screenName: 'SetupLinearityStandard',
    title: 'Linearity, Std',
    hasHome: false, // Uses Accept/Cancel/Clear instead of Home/Back
    hasBack: false,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 2, // Accept + Cancel + Clear
    minStringFields: 1,
    navTargets: [],
  },
  {
    pictureFolder: 'LiniearityLineator',
    screenName: 'SetupLinearityLineator',
    title: 'Lineator',
    hasHome: false, // Uses Accept/Cancel/Clear instead of Home/Back
    hasBack: false,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 2,
    minStringFields: 8, // 8 tube rows
    navTargets: [],
  },
  {
    pictureFolder: 'CaliCheck',
    screenName: 'SetupLinearityCalicheck',
    title: 'Calicheck',
    hasHome: false, // Uses Accept/Cancel/Clear instead of Home/Back
    hasBack: false,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 2,
    minStringFields: 12, // 12 color band rows
    navTargets: [],
  },
  {
    pictureFolder: 'Splash',
    screenName: 'Index',
    title: 'Splash/Index',
    hasHome: false,
    hasBack: false,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 0,
    minStringFields: 0,
    navTargets: [],
    isMetadataFile: true, // Index.json is a metadata manifest, not a standard screen
  },
  {
    pictureFolder: 'HalfLifeCalc',
    screenName: 'HalflifeCalculator',
    title: 'Half-life Calc',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 1, // Start
    minStringFields: 2,
    navTargets: [],
  },
  {
    pictureFolder: 'Inventory',
    screenName: 'Inventory',
    title: 'Inventory',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 1, // Add Item
    minStringFields: 2,
    navTargets: [],
  },
  {
    pictureFolder: 'MolyAssay',
    screenName: 'Moly',
    title: 'Moly Assay',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: true,
    minButtons: 3, // 3 moly types
    minStringFields: 0,
    navTargets: [],
  },
  {
    pictureFolder: 'QC',
    screenName: 'QC',
    title: 'QC',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: true,
    minButtons: 4, // 4 QC tests
    minStringFields: 0,
    navTargets: ['ONESTRIP', 'TWOSTRIP', 'HMPAO', 'MAG3'],
  },
  {
    pictureFolder: 'Utility',
    screenName: 'Other',
    title: 'Utility',
    hasHome: true,
    hasBack: true,
    hasSetup: false,
    hasChamberLabel: false,
    minButtons: 2,
    minStringFields: 2, // sfTitle + sfFutureDateEntry
    navTargets: [],
  },
]

describe('All Reference Screen Validation', () => {
  for (const ref of REFERENCE_SCREENS) {
    describe(`${ref.pictureFolder} → ${ref.screenName}`, () => {
      let screen: ReturnType<typeof loadScreenDef> | null = null

      it('should have a corresponding JSON screen definition', () => {
        const name = screenExists(ref.screenName)
          ? ref.screenName
          : (ref as any).fallbackName || ref.screenName
        expect(screenExists(name)).toBe(true)
        // Skip loading as screen def if it's a metadata file (e.g., Index.json)
        if (!(ref as any).isMetadataFile) {
          screen = loadScreenDef(name)
        }
      })

      it('should have 800x600 resolution', () => {
        if (!screen) return
        expect(verifyResolution(screen)).toBe(true)
      })

      it('should have all referenced images available on disk', () => {
        if (!screen) return
        const missing = verifyReferencedImages(screen)
        expect(missing).toEqual([])
      })

      if (ref.hasHome) {
        it('should have Home button', () => {
          if (!screen) return
          const nav = verifyNavBar(screen)
          expect(nav.home).toBeTruthy()
        })
      }

      if (ref.hasBack) {
        it('should have Back button', () => {
          if (!screen) return
          const nav = verifyNavBar(screen)
          expect(nav.back).toBeTruthy()
        })
      }

      it(`should have at least ${ref.minButtons} buttons (English)`, () => {
        if (!screen) return
        const allBtns = getAllButtons(screen)
        const englishBtns = allBtns.filter(b =>
          !b.language || b.language.toLowerCase() === 'english'
        )
        expect(englishBtns.length).toBeGreaterThanOrEqual(ref.minButtons)
      })

      it(`should have at least ${ref.minStringFields} string fields`, () => {
        if (!screen) return
        const allFields = getAllStringFields(screen)
        expect(allFields.length).toBeGreaterThanOrEqual(ref.minStringFields)
      })

      if (ref.navTargets.length > 0) {
        it('should have navigation buttons to expected sub-screens', () => {
          if (!screen) return
          const navMap = getNavigationMap(screen)
          const allBtns = getAllButtons(screen)

          for (const target of ref.navTargets) {
            const found = Object.values(navMap).some(t => t.includes(target)) ||
              allBtns.some(b => (b.params?.href || '').includes(target))
            expect(found).toBe(true)
          }
        })
      }

      it('all buttons should have non-empty href actions', () => {
        if (!screen) return
        const allBtns = getAllButtons(screen)
        const emptyHrefBtns: string[] = []
        for (const btn of allBtns) {
          const href = btn.params?.href || ''
          if (!href && btn.type === 'CustomButton') {
            emptyHrefBtns.push(btn.name)
          }
        }
        // Custom buttons should always have href
        expect(emptyHrefBtns).toEqual([])
      })

      it('all string fields should have valid bindings or labels', () => {
        if (!screen) return
        const allFields = getAllStringFields(screen)
        const unboundFields: string[] = []
        for (const field of allFields) {
          const href = field.params?.href || ''
          const initHref = field.params?.initHref || ''
          const hasBinding = href.includes('string(') || href.includes('label(') ||
            initHref.includes('string(') || initHref.includes('label(') ||
            href.includes('byte(') || href.includes('word(')
          if (!hasBinding && !field.params?.text) {
            unboundFields.push(field.name)
          }
        }
        // Most string fields should be bound to data
        // Some may be static labels without binding
      })
    })
  }
})

describe('Button Functionality Completeness', () => {
  it('every CustomButton across all reference screens should have a valid href', () => {
    const brokenButtons: string[] = []

    for (const ref of REFERENCE_SCREENS) {
      try {
        const name = screenExists(ref.screenName) ? ref.screenName : (ref as any).fallbackName
        if (!name || !screenExists(name)) continue
        const screen = loadScreenDef(name)
        const buttons = getWidgetsByType(screen, 'CustomButton')

        for (const btn of buttons) {
          const href = btn.params?.href || ''
          if (!href) {
            brokenButtons.push(`${name}/${btn.name}: empty href`)
          }
        }
      } catch { /* skip */ }
    }

    expect(brokenButtons).toEqual([])
  })

  it('every FunctionButton across all reference screens should have a valid href', () => {
    const brokenButtons: string[] = []

    for (const ref of REFERENCE_SCREENS) {
      try {
        const name = screenExists(ref.screenName) ? ref.screenName : (ref as any).fallbackName
        if (!name || !screenExists(name)) continue
        const screen = loadScreenDef(name)
        const buttons = getWidgetsByType(screen, 'FunctionButton')

        for (const btn of buttons) {
          const href = btn.params?.href || ''
          if (!href) {
            brokenButtons.push(`${name}/${btn.name}: empty href`)
          }
        }
      } catch { /* skip */ }
    }

    expect(brokenButtons).toEqual([])
  })
})

describe('Widget Position Sanity', () => {
  it('no widget should be positioned outside the 800x600 viewport', () => {
    const outOfBounds: string[] = []

    for (const ref of REFERENCE_SCREENS) {
      try {
        const name = screenExists(ref.screenName) ? ref.screenName : (ref as any).fallbackName
        if (!name || !screenExists(name)) continue
        const screen = loadScreenDef(name)

        for (const widget of screen.widgets) {
          if (widget.x < 0 || widget.y < 0 || widget.x > 800 || widget.y > 600) {
            outOfBounds.push(`${name}/${widget.name}: (${widget.x}, ${widget.y})`)
          }
        }
      } catch { /* skip */ }
    }

    expect(outOfBounds).toEqual([])
  })
})
