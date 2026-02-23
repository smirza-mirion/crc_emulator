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
 * Renders an Amulet RadioButton widget.
 * RadioButtons are mutually exclusive within a groupName.
 * The selected state is determined by matching `internalNumber` against the
 * byte referenced in `initHref` (e.g., internalRAM.byte(100).value()).
 * Clicking sends the `href` action (typically UART.word(0).setValue(N)).
 */
export const AmuletRadioButton: React.FC<Props> = ({ widget, stateManager, ws, visible }) => {
  const params = widget.params || {}
  const label = params.label || ''
  const internalNumber = parseInt(params.internalNumber || '0')
  const hrefOnHitOnly = params.hrefOnHitOnly === 'TRUE'

  // Parse the initHref binding to find which byte determines selected state
  const initBinding = widget.bindings?.find(b => b.param === 'initHref')
  const byteIndex = initBinding?.type === 'byte' ? initBinding.index : -1

  const [selected, setSelected] = useState(false)

  useEffect(() => {
    if (byteIndex < 0) return

    // Load initial value
    const currentVal = stateManager.getByte(byteIndex)
    setSelected(currentVal === internalNumber)

    const handler = (index: number, val: number) => {
      if (index === byteIndex) {
        setSelected(val === internalNumber)
      }
    }
    stateManager.onByteChange(handler)
    return () => stateManager.offByteChange(handler)
  }, [byteIndex, internalNumber, stateManager])

  if (!visible) return null

  const handleClick = () => {
    const href = params.href || ''
    if (!href) return
    const actions = parseHrefActions(href)
    executeActions(actions, ws, stateManager)
  }

  const fontSize = parseFontSize(params.fontSize)
  const isBold = params.fontStyle === 'BOLD'
  const fontColor = params.fontColor || 'BLACK'
  const alignLeft = params.buttonAlign === 'LEFT'

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
      {/* Radio circle */}
      <div
        style={{
          width: 18,
          height: 18,
          borderRadius: '50%',
          border: '2px solid #555',
          backgroundColor: '#fff',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          flexShrink: 0,
        }}
      >
        {selected && (
          <div
            style={{
              width: 10,
              height: 10,
              borderRadius: '50%',
              backgroundColor: '#333',
            }}
          />
        )}
      </div>
      {/* Label */}
      {label && (
        <span
          style={{
            fontSize,
            fontWeight: isBold ? 'bold' : 'normal',
            color: fontColor.toLowerCase(),
            whiteSpace: 'nowrap',
          }}
        >
          {label}
        </span>
      )}
    </div>
  )
}

function parseFontSize(size: string | undefined): number {
  if (!size) return 14
  const n = parseInt(size)
  if (isNaN(n)) return 14
  // Amulet sizes: small numeric values (5, 14, 16) or with "pt" suffix
  if (n <= 10) return n * 2 + 4 // scale small Amulet sizes
  return n
}
