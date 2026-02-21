import React, { useEffect, useState } from 'react'
import { AmuletStateManager } from '../lib/amulet-state'
import { WebSocketManager } from '../lib/websocket'

interface Props {
  stateManager: AmuletStateManager
  wsManager: WebSocketManager
}

interface DrawCommand {
  type: string
  [key: string]: any
}

export function ScreenRenderer({ stateManager, wsManager }: Props) {
  const [drawCommands, setDrawCommands] = useState<DrawCommand[]>([])
  const [strings, setStrings] = useState<Record<number, string>>({})

  useEffect(() => {
    // Listen for draw commands from firmware
    const handleDraw = (data: any) => {
      if (data.type === 'drawLine' || data.type === 'fillRect' || data.type === 'drawImage') {
        setDrawCommands(prev => [...prev, data])
      }
    }

    const handleString = (index: number, value: string) => {
      setStrings(prev => ({ ...prev, [index]: value }))
    }

    const handlePage = (data: any) => {
      // Clear draw commands on page change
      setDrawCommands([])
    }

    wsManager.on('drawLine', handleDraw)
    wsManager.on('fillRect', handleDraw)
    wsManager.on('setPage', handlePage)
    stateManager.onStringChange(handleString)

    return () => {
      wsManager.off('drawLine', handleDraw)
      wsManager.off('fillRect', handleDraw)
      wsManager.off('setPage', handlePage)
      stateManager.offStringChange(handleString)
    }
  }, [wsManager, stateManager])

  // Render string fields at their positions
  // The firmware sends strings with index numbers that map to screen positions
  // For now, render all non-empty strings
  const stringEntries = Object.entries(strings).filter(([, v]) => v.length > 0)

  return (
    <div style={{ width: '100%', height: '100%', position: 'relative' }}>
      {/* Background */}
      <div style={{
        position: 'absolute', top: 0, left: 0, right: 0, bottom: 0,
        background: 'linear-gradient(180deg, #3333AA 0%, #3333AA 69px, #DDEEFF 70px)',
      }} />

      {/* Draw commands (lines, rectangles) */}
      <svg style={{ position: 'absolute', top: 0, left: 0, width: '800px', height: '600px', pointerEvents: 'none' }}>
        {drawCommands.map((cmd, i) => {
          if (cmd.type === 'drawLine') {
            return <line key={i} x1={cmd.x1} y1={cmd.y1} x2={cmd.x2} y2={cmd.y2} stroke={cmd.color || '#000'} strokeWidth="1" />
          }
          if (cmd.type === 'fillRect') {
            return <rect key={i} x={cmd.x} y={cmd.y} width={cmd.w} height={cmd.h} fill={cmd.color || '#000'} />
          }
          return null
        })}
      </svg>

      {/* Title bar text */}
      <div style={{
        position: 'absolute', top: '5px', left: '0', right: '0',
        textAlign: 'center', color: 'white', fontSize: '24px', fontWeight: 'bold',
        fontFamily: '"Arial Black", "Franklin Gothic Book", sans-serif',
      }}>
        {strings[80] || 'CRC-25R Calibrator'}
      </div>

      {/* Activity display (large number) */}
      <div style={{
        position: 'absolute', top: '200px', left: '180px', width: '428px', height: '114px',
        display: 'flex', alignItems: 'center', justifyContent: 'flex-end',
        fontSize: '72px', fontWeight: 'bold',
        fontFamily: '"Franklin Gothic Book", "Arial", sans-serif',
        color: '#000', paddingRight: '10px',
      }}>
        {strings[10] || '---'}
      </div>

      {/* Unit display */}
      <div style={{
        position: 'absolute', top: '240px', left: '620px',
        fontSize: '24px', fontFamily: '"Franklin Gothic Book", sans-serif',
      }}>
        {strings[11] || ''}
      </div>

      {/* Nuclide display */}
      <div style={{
        position: 'absolute', top: '75px', left: '10px',
        fontSize: '18px', fontFamily: '"Franklin Gothic Book", sans-serif',
      }}>
        {strings[101] || ''}
      </div>

      {/* Chamber indicator */}
      <div style={{
        position: 'absolute', top: '545px', left: '120px',
        fontSize: '16px', fontFamily: 'sans-serif',
        color: '#000',
      }}>
        {strings[9] || ''}
      </div>

      {/* Time display */}
      <div style={{
        position: 'absolute', top: '5px', right: '10px',
        fontSize: '14px', color: 'white',
        fontFamily: 'monospace',
      }}>
        {strings[2] || ''}
      </div>

      {/* Navigation buttons bar */}
      <NavigationBar stateManager={stateManager} />

      {/* Status text lines */}
      {[170, 171, 172, 173, 174, 175].map(idx => (
        strings[idx] ? (
          <div key={idx} style={{
            position: 'absolute',
            top: `${350 + (idx - 170) * 25}px`,
            left: '20px',
            fontSize: '14px',
            fontFamily: '"Franklin Gothic Book", sans-serif',
          }}>
            {strings[idx]}
          </div>
        ) : null
      ))}
    </div>
  )
}

function NavigationBar({ stateManager }: { stateManager: AmuletStateManager }) {
  const language = stateManager.getByte(92) // 0=English, 1=French

  const buttons = [
    { label: language ? 'Quotidien' : 'Daily', byteIdx: 189, value: 1, color: '#4a90d9' },
    { label: language ? 'Fond' : 'Bkg', byteIdx: 189, value: 2, color: '#4a90d9' },
    { label: language ? 'Tension' : 'Ch Volts', byteIdx: 189, value: 3, color: '#4a90d9' },
    { label: language ? 'Exactitude' : 'Accuracy', byteIdx: 189, value: 4, color: '#4a90d9' },
    { label: language ? 'Tests avanc.' : 'Enhanced', byteIdx: 189, value: 5, color: '#4a90d9' },
    { label: 'Moly', byteIdx: 189, value: 6, color: '#4a90d9' },
    { label: language ? 'Inventaire' : 'Inventory', byteIdx: 189, value: 7, color: '#4a90d9' },
    { label: language ? 'Config' : 'Setup', byteIdx: 189, value: 22, color: '#d9534f' },
  ]

  return (
    <div style={{
      position: 'absolute', bottom: '0', left: '0', right: '0', height: '63px',
      display: 'flex', flexDirection: 'row', background: '#e8e8e8',
      borderTop: '1px solid #999',
    }}>
      {buttons.map((btn, i) => (
        <button
          key={i}
          onClick={() => stateManager.pressButton(btn.byteIdx, btn.value)}
          style={{
            flex: 1, border: 'none', cursor: 'pointer',
            background: btn.color, color: 'white',
            fontSize: '11px', fontWeight: 'bold', fontFamily: 'sans-serif',
            margin: '2px', borderRadius: '4px',
            display: 'flex', alignItems: 'center', justifyContent: 'center',
          }}
          onMouseDown={(e) => (e.currentTarget.style.opacity = '0.7')}
          onMouseUp={(e) => (e.currentTarget.style.opacity = '1')}
          onMouseLeave={(e) => (e.currentTarget.style.opacity = '1')}
        >
          {btn.label}
        </button>
      ))}
    </div>
  )
}
