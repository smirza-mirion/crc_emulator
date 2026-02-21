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
 * FunctionButton is a text-based button whose label comes from a string variable.
 * It executes actions on click.
 */
export const AmuletFunctionButton: React.FC<Props> = ({ widget, stateManager, ws, visible }) => {
  const params = widget.params || {}
  const [pressed, setPressed] = useState(false)

  // FunctionButton label can come from initHref (bound to a string/label index)
  const labelMatch = params.initHref?.match(/(?:string|label)\((\d+)\)/)
  const labelIndex = labelMatch ? parseInt(labelMatch[1]) : -1

  const [label, setLabel] = useState(params.label === 'FromInitHref' ? '' : (params.label || widget.name))

  useEffect(() => {
    if (labelIndex >= 0) {
      const initial = stateManager.getString(labelIndex)
      if (initial) setLabel(initial)

      const handler = (index: number, value: string) => {
        if (index === labelIndex) setLabel(value)
      }
      stateManager.onStringChange(handler)
      return () => stateManager.offStringChange(handler)
    }
  }, [labelIndex, stateManager])

  const isInvisible = params.invisible?.toUpperCase() === 'TRUE'
  if (!visible || (isInvisible && !label)) return null

  const fontSize = params.fontSize?.replace('pt', 'px') || '16px'
  const fontColor = params.fontColor || 'blue'
  const fontWeight = params.fontStyle?.toUpperCase() === 'BOLD' ? 'bold' : 'normal'
  const textAlign = (params.horizontalAlign || 'CENTER').toLowerCase()

  const handleClick = () => {
    const href = params.href || ''
    if (!href) return
    const actions = parseHrefActions(href)
    executeActions(actions, ws)
  }

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
        display: 'flex',
        alignItems: 'center',
        justifyContent: textAlign === 'left' ? 'flex-start' : textAlign === 'right' ? 'flex-end' : 'center',
        fontFamily: '"Franklin Gothic Medium", Arial, sans-serif',
        fontSize,
        fontWeight,
        color: fontColor,
        backgroundColor: pressed ? '#e0e0e0' : 'transparent',
        border: label ? '1px solid #999' : 'none',
        borderRadius: '3px',
        padding: '0 4px',
        boxSizing: 'border-box',
      }}
      onMouseDown={() => setPressed(true)}
      onMouseUp={() => setPressed(false)}
      onMouseLeave={() => setPressed(false)}
      onClick={handleClick}
    >
      {label}
    </div>
  )
}
