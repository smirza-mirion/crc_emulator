import React, { useEffect, useState } from 'react'
import { WidgetDef } from '../../lib/screen-loader'
import { AmuletStateManager } from '../../lib/amulet-state'
import { WebSocketManager } from '../../lib/websocket'
import { parseHrefActions, executeActions } from '../../lib/action-resolver'

interface Props {
  widget: WidgetDef
  stateManager: AmuletStateManager
  ws: WebSocketManager
  visible: boolean
}

/**
 * Renders an Amulet FunctionButton widget.
 *
 * FunctionButton has two modes:
 * 1. Transparent hit-area: invisible overlay on top of other widgets (no fillColor/label)
 * 2. Visible styled button: rendered with fill color, label, font styling (90+ screens)
 *
 * Label resolution:
 * - Static text: label="Accept"
 * - From initHref binding: label="FromInitHref(51)" → getString(bindingIndex), max 51 chars
 * - Empty: label="" → no label text
 */
export const AmuletFunctionButton: React.FC<Props> = ({ widget, stateManager, ws, visible }) => {
  const params = widget.params || {}
  const [pressed, setPressed] = useState(false)
  const [labelText, setLabelText] = useState('')

  // Determine if this is a visible button or transparent hit-area
  const fillColor = parseFillColor(params.fillColor)
  const isVisible = fillColor !== 'transparent'

  // Resolve label from initHref binding if needed
  const fromInitHrefMatch = params.label?.match(/^FromInitHref(?:\((\d+)\))?$/)
  const isFromInitHref = !!fromInitHrefMatch

  // Find the string index for the label binding
  const initBinding = widget.bindings?.find(b => b.param === 'initHref')
  const labelStringIndex = initBinding ? initBinding.index : -1

  useEffect(() => {
    if (!isFromInitHref || labelStringIndex < 0) {
      // Static label
      if (params.label && !isFromInitHref) {
        setLabelText(params.label)
      }
      return
    }

    // Dynamic label from string binding
    const maxLen = fromInitHrefMatch![1] ? parseInt(fromInitHrefMatch![1]) : 0

    const updateLabel = (val: string) => {
      setLabelText(maxLen > 0 ? val.substring(0, maxLen) : val)
    }

    // Load initial value
    const initial = stateManager.getString(labelStringIndex)
    if (initial) updateLabel(initial)

    const handler = (index: number, value: string) => {
      if (index === labelStringIndex) updateLabel(value)
    }
    stateManager.onStringChange(handler)
    return () => stateManager.offStringChange(handler)
  }, [labelStringIndex, isFromInitHref, stateManager])

  if (!visible) return null

  const handleClick = () => {
    const href = params.href || ''
    if (!href) return
    const actions = parseHrefActions(href)
    executeActions(actions, ws, stateManager)
  }

  // Transparent hit-area mode
  if (!isVisible && !labelText) {
    return (
      <div
        style={{
          position: 'absolute',
          left: widget.x,
          top: widget.y,
          width: widget.width,
          height: widget.height,
          cursor: 'pointer',
          userSelect: 'none',
          backgroundColor: pressed ? 'rgba(0,0,0,0.05)' : 'transparent',
          zIndex: 10,
        }}
        onMouseDown={() => setPressed(true)}
        onMouseUp={() => setPressed(false)}
        onMouseLeave={() => setPressed(false)}
        onClick={handleClick}
      />
    )
  }

  // Visible styled button mode
  const fontSize = parseFontSize(params.fontSize)
  const fontFamily = mapFont(params.font)
  const fontColor = params.fontColor || '#000000'
  const fontWeight = params.fontStyle?.toUpperCase() === 'BOLD' ? 'bold' : 'normal'
  const textAlign = mapAlign(params.horizontalAlign)
  const vertAlign = mapVertAlign(params.verticalAlign)

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
        backgroundColor: pressed ? darken(fillColor) : fillColor,
        border: isVisible ? '1px solid #999' : 'none',
        cursor: 'pointer',
        userSelect: 'none',
        overflow: 'hidden',
        whiteSpace: 'nowrap',
        padding: '0 8px',
        boxSizing: 'border-box',
        lineHeight: 1.2,
        zIndex: 10,
      }}
      onMouseDown={() => setPressed(true)}
      onMouseUp={() => setPressed(false)}
      onMouseLeave={() => setPressed(false)}
      onClick={handleClick}
    >
      {labelText}
    </div>
  )
}

function parseFontSize(size: string | undefined): string {
  if (!size) return '14px'
  const ptMatch = size.match(/^(\d+)pt$/)
  if (ptMatch) return `${parseInt(ptMatch[1])}px`
  const num = parseInt(size)
  if (!isNaN(num)) {
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

function parseFillColor(color: string | undefined): string {
  if (!color || color === 'transparent') return 'transparent'
  // 8-digit hex: #RRGGBBAA
  if (color.length === 9 && color.startsWith('#')) {
    const alpha = parseInt(color.substring(7, 9), 16)
    if (alpha === 0) return 'transparent'
    return color.substring(0, 7)
  }
  return color
}

function darken(color: string): string {
  if (color === 'transparent') return 'rgba(0,0,0,0.05)'
  if (color === '#FFFFFF' || color === '#ffffff') return '#E8E8E8'
  return color
}
