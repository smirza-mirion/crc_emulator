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
 * Renders an Amulet CustomButton widget.
 * CustomButton displays up/down images and executes actions on click.
 * Supports text overlays via the "label" param:
 *   - Static text: label="Accept"
 *   - Dynamic text from initHref binding: label="FromInitHref" or "FromInitHref(N)"
 */
export const AmuletButton: React.FC<Props> = ({ widget, stateManager, ws, visible }) => {
  const params = widget.params || {}
  const [pressed, setPressed] = useState(false)
  const [labelText, setLabelText] = useState('')

  // Resolve label from initHref binding if needed
  const fromInitHrefMatch = params.label?.match(/^FromInitHref(?:\((\d+)\))?$/)
  const isFromInitHref = !!fromInitHrefMatch

  // Find the string/label index for the binding
  const initBinding = widget.bindings?.find(b => b.param === 'initHref')
  const labelStringIndex = initBinding ? initBinding.index : -1

  useEffect(() => {
    if (!isFromInitHref || labelStringIndex < 0) {
      // Static label (not "FromInitHref")
      if (params.label && !isFromInitHref) {
        setLabelText(params.label)
      }
      return
    }

    // Dynamic label from string/label binding
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

  const upImage = normalizeImagePath(params.upImage)
  const downImage = normalizeImagePath(params.downImage) || upImage
  const hasText = !!labelText

  const handleClick = () => {
    const href = params.href || ''
    if (!href) return
    const actions = parseHrefActions(href)
    executeActions(actions, ws, stateManager)
  }

  // Text styling (used when label text is present)
  const fontSize = parseFontSize(params.fontSize)
  const fontFamily = mapFont(params.font)
  const fontColor = params.fontColor || '#000000'
  const fontWeight = params.fontStyle?.toUpperCase() === 'BOLD' ? 'bold' : 'normal'
  const hAlign = mapAlign(params.horizontalAlign)
  const vAlign = mapVertAlign(params.verticalAlign)

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
      }}
      onMouseDown={() => setPressed(true)}
      onMouseUp={() => setPressed(false)}
      onMouseLeave={() => setPressed(false)}
      onClick={handleClick}
    >
      {upImage && (
        <img
          src={pressed ? (downImage || upImage) : upImage}
          width={widget.width}
          height={widget.height}
          style={{ display: 'block', opacity: pressed ? 0.8 : 1 }}
          draggable={false}
        />
      )}
      {hasText && (
        <div
          style={{
            position: 'absolute',
            top: 0,
            left: 0,
            width: widget.width,
            height: widget.height,
            display: 'flex',
            alignItems: vAlign,
            justifyContent: hAlign === 'left' ? 'flex-start' : hAlign === 'right' ? 'flex-end' : 'center',
            fontFamily,
            fontSize,
            fontWeight,
            color: fontColor,
            overflow: 'hidden',
            whiteSpace: 'nowrap',
            padding: '0 4px',
            boxSizing: 'border-box',
            lineHeight: 1.2,
            pointerEvents: 'none',
          }}
        >
          {labelText}
        </div>
      )}
    </div>
  )
}

function normalizeImagePath(src: string | undefined): string | undefined {
  if (!src) return undefined
  if (src.startsWith('Images/')) {
    return `/assets/images/${src.substring(7)}`
  }
  return `/assets/images/${src}`
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
