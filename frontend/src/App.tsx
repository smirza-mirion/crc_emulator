import React, { useEffect, useState } from 'react'
import { DeviceDisplay } from './components/DeviceDisplay'
import { ControlPanel } from './components/ControlPanel'
import { WebSocketManager } from './lib/websocket'
import { AmuletStateManager } from './lib/amulet-state'

const wsManager = new WebSocketManager()
const stateManager = new AmuletStateManager(wsManager)

export default function App() {
  const [connected, setConnected] = useState(false)
  const [currentPage, setCurrentPage] = useState(0)

  useEffect(() => {
    // Connect to emulator WebSocket
    const port = new URLSearchParams(window.location.search).get('port') || '9876'
    wsManager.connect(`ws://localhost:${port}/ws`)

    wsManager.onConnect = () => setConnected(true)
    wsManager.onDisconnect = () => setConnected(false)
    stateManager.onPageChange = (page: number) => setCurrentPage(page)

    return () => wsManager.disconnect()
  }, [])

  return (
    <div style={{
      display: 'flex',
      flexDirection: 'row',
      height: '100vh',
      background: '#1a1a2e',
      color: '#e0e0e0',
    }}>
      <div style={{
        flex: '0 0 820px',
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        padding: '10px',
      }}>
        <div style={{
          color: connected ? '#4ade80' : '#ef4444',
          fontSize: '12px',
          marginBottom: '8px',
        }}>
          {connected ? '\u25CF Connected' : '\u25CB Disconnected'} | Page: 0x{currentPage.toString(16).toUpperCase().padStart(2, '0')}
        </div>
        <DeviceDisplay stateManager={stateManager} wsManager={wsManager} />
      </div>
      <div style={{
        flex: 1,
        minWidth: '350px',
        maxWidth: '450px',
        borderLeft: '1px solid #333',
        overflow: 'auto',
      }}>
        <ControlPanel wsManager={wsManager} stateManager={stateManager} />
      </div>
    </div>
  )
}
