import React, { useEffect, useState } from 'react'
import { WidgetDef } from '../../lib/screen-loader'
import { AmuletStateManager } from '../../lib/amulet-state'

interface Props {
  widget: WidgetDef
  stateManager: AmuletStateManager
  visible: boolean
}

/**
 * Renders an Amulet StringField widget.
 * StringField displays a text value bound to an internalRAM string index.
 */
export const AmuletStringField: React.FC<Props> = ({ widget, stateManager, visible }) => {
  const params = widget.params || {}

  // Find the bound string index from the href param
  const hrefMatch = params.href?.match(/string\((\d+)\)/)
  const stringIndex = hrefMatch ? parseInt(hrefMatch[1]) : -1

  // Also check for label bindings
  const labelMatch = params.initHref?.match(/label\((\d+)\)/)
  const labelIndex = labelMatch ? parseInt(labelMatch[1]) : -1

  const [text, setText] = useState(params.initialCondition || '')
  const [invertedColor, setInvertedColor] = useState(false)

  useEffect(() => {
    if (stringIndex >= 0) {
      // Get initial value
      const initial = stateManager.getString(stringIndex)
      if (initial) setText(initial)

      // Subscribe to changes
      const handler = (index: number, value: string) => {
        if (index === stringIndex) setText(value)
      }
      stateManager.onStringChange(handler)
      return () => stateManager.offStringChange(handler)
    }
    if (labelIndex >= 0) {
      const initial = stateManager.getString(labelIndex)
      if (initial) setText(initial)

      const handler = (index: number, value: string) => {
        if (index === labelIndex) setText(value)
      }
      stateManager.onStringChange(handler)
      return () => stateManager.offStringChange(handler)
    }
  }, [stringIndex, labelIndex, stateManager])

  // Parse style params
  const fontSize = parseFontSize(params.fontSize)
  const fontFamily = mapFont(params.font)
  const fontColor = invertedColor
    ? (params.fontColorAlt || '#FF0000')
    : (params.fontColor || '#000000')
  const fillColor = params.fillColor || 'transparent'
  const fontWeight = params.fontStyle?.toUpperCase() === 'BOLD' ? 'bold' : 'normal'
  const textAlign = mapAlign(params.horizontalAlign)
  const vertAlign = mapVertAlign(params.verticalAlign)

  // Handle invisible
  const isInvisible = params.invisible?.toUpperCase() === 'TRUE'
  const shouldShow = visible && (!isInvisible || text !== '')

  if (!shouldShow) return null

  // Parse fill color (handle 8-digit hex with alpha)
  const bgColor = parseFillColor(fillColor)

  return (
    <div
      style={{
        position: 'absolute',
        left: widget.x,
        top: widget.y,
        width: widget.width,
        height: widget.height,
        display: 'flex',
        alignItems: vertAlign,
        justifyContent: textAlign === 'left' ? 'flex-start' : textAlign === 'right' ? 'flex-end' : 'center',
        fontFamily,
        fontSize,
        fontWeight,
        color: fontColor,
        backgroundColor: bgColor,
        overflow: 'hidden',
        whiteSpace: 'nowrap',
        padding: '0 2px',
        boxSizing: 'border-box',
        lineHeight: 1.2,
      }}
    >
      {text}
    </div>
  )
}

function parseFontSize(size: string | undefined): string {
  if (!size) return '14px'
  // Handle "24pt", "18pt", "5" (Amulet font size units)
  const ptMatch = size.match(/^(\d+)pt$/)
  if (ptMatch) return `${parseInt(ptMatch[1])}px`
  const num = parseInt(size)
  if (!isNaN(num)) {
    // Amulet font sizes: 5 = ~18px, 4 = ~16px, 3 = ~14px, etc.
    if (num <= 10) return `${Math.max(12, num * 3.5)}px`
    return `${num}px`
  }
  return '14px'
}

function mapFont(font: string | undefined): string {
  if (!font) return 'Arial, sans-serif'
  const lower = font.toLowerCase()
  if (lower.includes('franklin gothic')) return '"Franklin Gothic Medium", "Trebuchet MS", Arial, sans-serif'
  if (lower.includes('arial black')) return '"Arial Black", Gadget, sans-serif'
  if (lower.includes('arial')) return 'Arial, Helvetica, sans-serif'
  if (lower.includes('courier')) return '"Courier New", Courier, monospace'
  return `"${font}", Arial, sans-serif`
}

function mapAlign(align: string | undefined): string {
  if (!align) return 'left'
  switch (align.toUpperCase()) {
    case 'CENTER': return 'center'
    case 'RIGHT': return 'right'
    default: return 'left'
  }
}

function mapVertAlign(align: string | undefined): string {
  if (!align) return 'center'
  switch (align.toUpperCase()) {
    case 'TOP': return 'flex-start'
    case 'MIDDLE': return 'center'
    case 'BOTTOM': return 'flex-end'
    default: return 'center'
  }
}

function parseFillColor(color: string): string {
  if (!color || color === 'transparent') return 'transparent'
  // 8-digit hex: #RRGGBBAA
  if (color.length === 9 && color.startsWith('#')) {
    const alpha = parseInt(color.substring(7, 9), 16)
    if (alpha === 0) return 'transparent'
    const rgb = color.substring(0, 7)
    return rgb
  }
  return color
}
