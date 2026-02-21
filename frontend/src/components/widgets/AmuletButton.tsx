import React, { useState } from 'react'
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
 */
export const AmuletButton: React.FC<Props> = ({ widget, stateManager, ws, visible }) => {
  const params = widget.params || {}
  const [pressed, setPressed] = useState(false)

  // Visibility is managed by ScreenRenderer (hiddenWidgets/shownWidgets)
  if (!visible) return null

  const upImage = normalizeImagePath(params.upImage)
  const downImage = normalizeImagePath(params.downImage) || upImage

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
    </div>
  )
}

function normalizeImagePath(src: string | undefined): string | undefined {
  if (!src) return undefined
  // Images/ paths from HTM -> /assets/images/ in frontend
  if (src.startsWith('Images/')) {
    return `/assets/images/${src.substring(7)}`
  }
  return `/assets/images/${src}`
}
