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
 * Renders an Amulet CheckBox widget.
 * CheckBox displays checked/unchecked state with optional images.
 * State is bound to a byte from `initHref` binding.
 * Clicking toggles the state and executes `href` action.
 */
export const AmuletCheckBox: React.FC<Props> = ({ widget, stateManager, ws, visible }) => {
  const params = widget.params || {}
  const label = params.label || ''
  const checkedValue = parseInt(params.checkedValue || '1')
  const unCheckedValue = parseInt(params.unCheckedValue || '0')

  // Parse the initHref binding to find which byte determines checked state
  const initBinding = widget.bindings?.find(b => b.param === 'initHref')
  const byteIndex = initBinding?.type === 'byte' ? initBinding.index : -1

  const [checked, setChecked] = useState(false)

  useEffect(() => {
    if (byteIndex < 0) return

    // Load initial value
    const currentVal = stateManager.getByte(byteIndex)
    setChecked(currentVal === checkedValue)

    const handler = (index: number, val: number) => {
      if (index === byteIndex) {
        setChecked(val === checkedValue)
      }
    }
    stateManager.onByteChange(handler)
    return () => stateManager.offByteChange(handler)
  }, [byteIndex, checkedValue, stateManager])

  if (!visible) return null

  const handleClick = () => {
    const newChecked = !checked
    // On the real Amulet display, clicking a checkbox directly updates
    // the bound byte in internal RAM. We replicate this locally.
    if (byteIndex >= 0) {
      const newVal = newChecked ? checkedValue : unCheckedValue
      stateManager.setByteLocally(byteIndex, newVal)
    }
    // Execute the href action (typically UART word write to firmware)
    const href = params.href || ''
    if (href) {
      const actions = parseHrefActions(href)
      executeActions(actions, ws, stateManager)
    }
  }

  const emptyImage = normalizeImagePath(params.emptyImage)
  const fullImage = normalizeImagePath(params.fullImage)
  const useImages = !!(emptyImage && fullImage)
  const fontSize = parseFontSize(params.fontSize)
  const fontColor = params.fontColor || '#000000'
  const alignLeft = params.boxAlign === 'LEFT'

  return (
    <div
      style={{
        position: 'absolute',
        left: widget.x,
        top: widget.y,
        width: widget.width,
        height: widget.height,
        display: 'flex',
        flexDirection: alignLeft ? 'row' : 'row-reverse',
        alignItems: 'center',
        gap: 6,
        cursor: 'pointer',
        userSelect: 'none',
      }}
      onClick={handleClick}
    >
      {useImages ? (
        <img
          src={checked ? fullImage : emptyImage}
          width={Math.min(widget.width, widget.height)}
          height={Math.min(widget.width, widget.height)}
          style={{ display: 'block' }}
          draggable={false}
        />
      ) : (
        <div
          style={{
            width: 18,
            height: 18,
            border: '2px solid #555',
            backgroundColor: '#fff',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            flexShrink: 0,
          }}
        >
          {checked && (
            <span style={{ fontSize: 14, color: '#333', lineHeight: 1 }}>✓</span>
          )}
        </div>
      )}
      {label && (
        <span
          style={{
            fontSize,
            color: fontColor,
            whiteSpace: 'nowrap',
          }}
        >
          {label}
        </span>
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

function parseFontSize(size: string | undefined): number {
  if (!size) return 14
  const n = parseInt(size)
  if (isNaN(n)) return 14
  if (n <= 10) return n * 2 + 4
  return n
}
