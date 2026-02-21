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
 * FunctionButton is a transparent clickable overlay that sits on top of
 * a StringField. When clicked, it executes href actions (navigation,
 * setValue, macros). It is NOT meant to be visually prominent - it's an
 * invisible hit area over existing text.
 */
export const AmuletFunctionButton: React.FC<Props> = ({ widget, stateManager, ws, visible }) => {
  const params = widget.params || {}
  const [pressed, setPressed] = useState(false)

  if (!visible) return null

  const handleClick = () => {
    const href = params.href || ''
    if (!href) return
    const actions = parseHrefActions(href)
    executeActions(actions, ws, stateManager)
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
