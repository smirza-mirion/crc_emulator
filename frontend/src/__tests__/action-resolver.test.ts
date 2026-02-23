/**
 * Action Resolver Tests
 *
 * Tests the parseHrefActions function and macro resolution to ensure
 * all button click actions correctly translate to firmware commands.
 */

import { describe, it, expect } from 'vitest'
import { parseHrefActions } from '../lib/action-resolver'

describe('Action Resolver', () => {
  describe('parseHrefActions', () => {
    it('should parse %Home macro', () => {
      const actions = parseHrefActions('%Home')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('macro')
      expect(actions[0].macro).toBe('Home')
    })

    it('should parse %Back macro', () => {
      const actions = parseHrefActions('%Back')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('macro')
      expect(actions[0].macro).toBe('Back')
    })

    it('should parse %SetHTML with macro argument', () => {
      const actions = parseHrefActions('%SetHTML(%%SET_HTML_DAILY)')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('macro')
      expect(actions[0].macro).toBe('SetHTML')
      expect(actions[0].args).toBe('%%SET_HTML_DAILY')
    })

    it('should parse %SetClear with macro argument', () => {
      const actions = parseHrefActions('%SetClear(%%SET_CLEAR_SETUP)')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('macro')
      expect(actions[0].macro).toBe('SetClear')
      expect(actions[0].args).toBe('%%SET_CLEAR_SETUP')
    })

    it('should parse %SetHotkeyNuclide with number', () => {
      const actions = parseHrefActions('%SetHotkeyNuclide(%%SET_HOTKEY_NUCLIDE_1)')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('macro')
      expect(actions[0].macro).toBe('SetHotkeyNuclide')
      expect(actions[0].args).toBe('%%SET_HOTKEY_NUCLIDE_1')
    })

    it('should parse Amulet:internalRAM.byte setValue', () => {
      const actions = parseHrefActions('Amulet:internalRAM.byte(189).setValue(22)')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('setValue')
      expect(actions[0].target).toBe('internalRAM.byte(189)')
      expect(actions[0].value).toBe('22')
    })

    it('should parse Amulet:UART.byte setValue', () => {
      const actions = parseHrefActions('Amulet:UART.byte(3).setValue(0x09)')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('setValue')
      expect(actions[0].target).toBe('UART.byte(3)')
      expect(actions[0].value).toBe('0x09')
    })

    it('should parse Amulet:UART.word setValue', () => {
      const actions = parseHrefActions('Amulet:UART.word(0).setValue(0x0102)')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('setValue')
      expect(actions[0].target).toBe('UART.word(0)')
      expect(actions[0].value).toBe('0x0102')
    })

    it('should parse chained actions separated by comma', () => {
      const actions = parseHrefActions('%Beep,%SetHTML(%%SET_HTML_MAINSCREEN)')
      expect(actions).toHaveLength(2)
      expect(actions[0].macro).toBe('Beep')
      expect(actions[1].macro).toBe('SetHTML')
    })

    it('should parse Amulet:NOP()', () => {
      const actions = parseHrefActions('Amulet:NOP()')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('nop')
    })

    it('should parse document method calls', () => {
      const actions = parseHrefActions('Amulet:document.btnWidget.reappear()')
      expect(actions).toHaveLength(1)
      expect(actions[0].type).toBe('reappear')
      expect(actions[0].target).toBe('btnWidget')
    })

    it('should handle empty href', () => {
      const actions = parseHrefActions('')
      expect(actions).toHaveLength(0)
    })

    it('should handle complex chained action with nested parens', () => {
      const href = '%SetHTML(%%SET_HTML_ACCURACY),Amulet:UART.byte(3).setValue(4)'
      const actions = parseHrefActions(href)
      expect(actions).toHaveLength(2)
      expect(actions[0].macro).toBe('SetHTML')
      expect(actions[1].type).toBe('setValue')
    })
  })

  describe('Navigation Macros', () => {
    const navMacros = [
      'SET_HTML_ACCURACY',
      'SET_HTML_BACKGROUND',
      'SET_HTML_CHAMBERVOLTS',
      'SET_HTML_DAILY',
      'SET_HTML_GEOMETRY',
      'SET_HTML_INVENTORY',
      'SET_HTML_LINEARITY',
      'SET_HTML_MAINSCREEN',
      'SET_HTML_MOLY',
      'SET_HTML_QC',
      'SET_HTML_SETUP',
      'SET_HTML_ENHANCED',
    ]

    for (const macro of navMacros) {
      it(`%SetHTML(%%${macro}) should parse correctly`, () => {
        const actions = parseHrefActions(`%SetHTML(%%${macro})`)
        expect(actions).toHaveLength(1)
        expect(actions[0].type).toBe('macro')
        expect(actions[0].macro).toBe('SetHTML')
        expect(actions[0].args).toBe(`%%${macro}`)
      })
    }
  })

  describe('SetClear Macros', () => {
    const clearMacros = [
      'SET_CLEAR_DAILY',
      'SET_CLEAR_ACCURACY',
      'SET_CLEAR_LINEARITY',
      'SET_CLEAR_ONESTRIP',
      'SET_CLEAR_TWOSTRIP',
      'SET_CLEAR_HMPAO',
      'SET_CLEAR_MAG3',
      'SET_CLEAR_MOLY',
      'SET_CLEAR_SETUP',
      'SET_CLEAR_AUTOLINEARITYTEST',
      'SET_CLEAR_HALFLIFECALC',
    ]

    for (const macro of clearMacros) {
      it(`%SetClear(%%${macro}) should parse correctly`, () => {
        const actions = parseHrefActions(`%SetClear(%%${macro})`)
        expect(actions).toHaveLength(1)
        expect(actions[0].type).toBe('macro')
        expect(actions[0].macro).toBe('SetClear')
        expect(actions[0].args).toBe(`%%${macro}`)
      })
    }
  })

  describe('Special Macros', () => {
    const specialMacros = [
      'Beep',
      'SetAccuracyAll',
      'SetAccuracyDaily',
      'ShowDoseTableButton',
      'HideDoseTableButton',
      'SelectMode',
      'SecurityMode',
      'ActiveStaff',
      'AllStaff',
    ]

    for (const macro of specialMacros) {
      it(`%${macro} should parse as a macro`, () => {
        const actions = parseHrefActions(`%${macro}`)
        expect(actions).toHaveLength(1)
        expect(actions[0].type).toBe('macro')
        expect(actions[0].macro).toBe(macro)
      })
    }
  })

  describe('Macro with Arguments', () => {
    it('should parse %ConfigKeypad with argument', () => {
      const actions = parseHrefActions('%ConfigKeypad(%%CONFIG_KEYPAD_PASSWORD)')
      expect(actions).toHaveLength(1)
      expect(actions[0].macro).toBe('ConfigKeypad')
      expect(actions[0].args).toBe('%%CONFIG_KEYPAD_PASSWORD')
    })

    it('should parse %SetNuclide with argument', () => {
      const actions = parseHrefActions('%SetNuclide(%%SET_NUCLIDE_MEASURE)')
      expect(actions).toHaveLength(1)
      expect(actions[0].macro).toBe('SetNuclide')
      expect(actions[0].args).toBe('%%SET_NUCLIDE_MEASURE')
    })

    it('should parse %ConfigMaxStrLen with numeric argument', () => {
      const actions = parseHrefActions('%ConfigMaxStrLen(13)')
      expect(actions).toHaveLength(1)
      expect(actions[0].macro).toBe('ConfigMaxStrLen')
      expect(actions[0].args).toBe('13')
    })
  })
})
