import React, { useEffect, useState } from 'react'
import { AmuletStateManager } from '../lib/amulet-state'
import { WebSocketManager } from '../lib/websocket'
import { ScreenRenderer } from './ScreenRenderer'

interface Props {
  stateManager: AmuletStateManager
  wsManager: WebSocketManager
}

export function DeviceDisplay({ stateManager, wsManager }: Props) {
  const [, forceUpdate] = useState(0)

  // Force re-render when state changes
  useEffect(() => {
    const handler = () => forceUpdate(n => n + 1)
    stateManager.onByteChange(handler)
    stateManager.onStringChange(handler)
    stateManager.onWordChange(handler)
    return () => {
      stateManager.offByteChange(handler)
      stateManager.offStringChange(handler)
      stateManager.offWordChange(handler)
    }
  }, [stateManager])

  return (
    <div style={{
      width: '800px',
      height: '600px',
      position: 'relative',
      overflow: 'hidden',
      background: '#DDEEFF',
      border: '2px solid #555',
      borderRadius: '4px',
      boxShadow: '0 4px 20px rgba(0,0,0,0.5)',
    }}>
      <ScreenRenderer stateManager={stateManager} wsManager={wsManager} />
    </div>
  )
}
