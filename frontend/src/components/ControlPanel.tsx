import React, { useState } from 'react'
import { WebSocketManager } from '../lib/websocket'
import { AmuletStateManager } from '../lib/amulet-state'

interface Props {
  wsManager: WebSocketManager
  stateManager: AmuletStateManager
}

const SCENARIOS = [
  { id: 0, name: 'Cold Start', desc: 'No sources, fresh system' },
  { id: 1, name: 'Daily QC (Co-57)', desc: 'Co-57 source at 5 uCi' },
  { id: 2, name: 'Tc-99m Measurement', desc: 'Tc-99m at 15 mCi' },
  { id: 3, name: 'Multi-Chamber', desc: 'Tc-99m + F-18, 2 chambers' },
  { id: 4, name: 'MCA Well Detector', desc: 'Cs-137 spectrum' },
  { id: 5, name: 'Accuracy Test', desc: '5 standard sources' },
  { id: 6, name: 'Linearity Test', desc: 'High-activity Tc-99m' },
  { id: 7, name: 'Moly Assay', desc: 'Mo-99/Tc-99m generator' },
  { id: 8, name: 'Background Only', desc: 'No source, ambient' },
  { id: 9, name: 'Error Conditions', desc: 'Over-range, disconnect' },
]

export function ControlPanel({ wsManager, stateManager }: Props) {
  const [activeTab, setActiveTab] = useState<'scenarios' | 'custom' | 'debug'>('scenarios')
  const [selectedScenario, setSelectedScenario] = useState(0)

  return (
    <div style={{ padding: '16px', height: '100%', display: 'flex', flexDirection: 'column' }}>
      <h2 style={{ margin: '0 0 12px 0', fontSize: '18px', color: '#fff' }}>
        Simulation Control
      </h2>

      {/* Tab buttons */}
      <div style={{ display: 'flex', gap: '4px', marginBottom: '12px' }}>
        {(['scenarios', 'custom', 'debug'] as const).map(tab => (
          <button
            key={tab}
            onClick={() => setActiveTab(tab)}
            style={{
              flex: 1, padding: '8px', border: 'none', borderRadius: '4px',
              cursor: 'pointer', fontSize: '13px',
              background: activeTab === tab ? '#4a90d9' : '#333',
              color: activeTab === tab ? 'white' : '#aaa',
            }}
          >
            {tab.charAt(0).toUpperCase() + tab.slice(1)}
          </button>
        ))}
      </div>

      {/* Tab content */}
      <div style={{ flex: 1, overflow: 'auto' }}>
        {activeTab === 'scenarios' && (
          <div>
            <p style={{ fontSize: '13px', color: '#888', margin: '0 0 12px 0' }}>
              Select a preset scenario to configure the simulation:
            </p>
            {SCENARIOS.map(s => (
              <div
                key={s.id}
                onClick={() => {
                  setSelectedScenario(s.id)
                  wsManager.send({ type: 'loadScenario', scenario: s.id })
                }}
                style={{
                  padding: '10px 12px', marginBottom: '6px', borderRadius: '6px',
                  cursor: 'pointer',
                  background: selectedScenario === s.id ? '#2a4a7a' : '#252535',
                  border: selectedScenario === s.id ? '1px solid #4a90d9' : '1px solid #333',
                }}
              >
                <div style={{ fontSize: '14px', fontWeight: 'bold', color: '#ddd' }}>{s.name}</div>
                <div style={{ fontSize: '12px', color: '#888', marginTop: '2px' }}>{s.desc}</div>
              </div>
            ))}
          </div>
        )}

        {activeTab === 'custom' && (
          <div>
            <p style={{ fontSize: '13px', color: '#888', margin: '0 0 12px 0' }}>
              Configure individual chamber sources:
            </p>
            <SourceConfig channel={0} wsManager={wsManager} />
            <SourceConfig channel={1} wsManager={wsManager} />
          </div>
        )}

        {activeTab === 'debug' && (
          <DebugPanel stateManager={stateManager} />
        )}
      </div>
    </div>
  )
}

function SourceConfig({ channel, wsManager }: { channel: number; wsManager: WebSocketManager }) {
  const [enabled, setEnabled] = useState(channel === 0)
  const [nuclide, setNuclide] = useState('Tc-99m')
  const [activity, setActivity] = useState('15')
  const [unit, setUnit] = useState('mCi')

  const nuclides = ['Tc-99m', 'Co-57', 'Ba-133', 'Cs-137', 'Co-60', 'F-18', 'I-131', 'Mo-99']
  const units = ['uCi', 'mCi', 'Ci', 'kBq', 'MBq', 'GBq']

  const apply = () => {
    wsManager.send({
      type: 'configSource',
      channel,
      enabled,
      nuclide,
      activity: parseFloat(activity),
      unit,
    })
  }

  return (
    <div style={{
      padding: '12px', marginBottom: '8px', background: '#252535',
      borderRadius: '6px', border: '1px solid #333',
    }}>
      <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '8px' }}>
        <input type="checkbox" checked={enabled} onChange={e => setEnabled(e.target.checked)} />
        <span style={{ fontSize: '14px', fontWeight: 'bold' }}>Chamber {channel + 1}</span>
      </div>
      {enabled && (
        <div style={{ display: 'flex', flexDirection: 'column', gap: '6px' }}>
          <select value={nuclide} onChange={e => setNuclide(e.target.value)}
            style={{ padding: '6px', background: '#1a1a2e', color: '#ddd', border: '1px solid #555', borderRadius: '4px' }}>
            {nuclides.map(n => <option key={n} value={n}>{n}</option>)}
          </select>
          <div style={{ display: 'flex', gap: '4px' }}>
            <input type="number" value={activity} onChange={e => setActivity(e.target.value)}
              style={{ flex: 1, padding: '6px', background: '#1a1a2e', color: '#ddd', border: '1px solid #555', borderRadius: '4px' }} />
            <select value={unit} onChange={e => setUnit(e.target.value)}
              style={{ padding: '6px', background: '#1a1a2e', color: '#ddd', border: '1px solid #555', borderRadius: '4px' }}>
              {units.map(u => <option key={u} value={u}>{u}</option>)}
            </select>
          </div>
          <button onClick={apply}
            style={{ padding: '8px', background: '#4a90d9', color: 'white', border: 'none', borderRadius: '4px', cursor: 'pointer' }}>
            Apply
          </button>
        </div>
      )}
    </div>
  )
}

function DebugPanel({ stateManager }: { stateManager: AmuletStateManager }) {
  const [refreshKey, setRefreshKey] = useState(0)

  // Show current state of important Amulet variables
  const importantBytes = [20, 21, 22, 23, 92, 100, 189, 190, 191, 192]
  const importantStrings = [0, 2, 3, 9, 10, 11, 80, 101]

  return (
    <div>
      <button onClick={() => setRefreshKey(k => k + 1)}
        style={{ padding: '6px 12px', marginBottom: '12px', background: '#333', color: '#ddd', border: '1px solid #555', borderRadius: '4px', cursor: 'pointer' }}>
        Refresh State
      </button>

      <h4 style={{ margin: '8px 0 4px 0', fontSize: '13px', color: '#4a90d9' }}>Byte Variables</h4>
      <div key={`bytes-${refreshKey}`} style={{ fontFamily: 'monospace', fontSize: '12px' }}>
        {importantBytes.map(i => (
          <div key={i} style={{ padding: '2px 0' }}>
            <span style={{ color: '#888' }}>byte[{i.toString().padStart(3)}]</span>
            {' = '}
            <span style={{ color: '#4ade80' }}>0x{stateManager.getByte(i).toString(16).toUpperCase().padStart(2, '0')}</span>
            {' '}({stateManager.getByte(i)})
          </div>
        ))}
      </div>

      <h4 style={{ margin: '12px 0 4px 0', fontSize: '13px', color: '#4a90d9' }}>String Variables</h4>
      <div key={`strings-${refreshKey}`} style={{ fontFamily: 'monospace', fontSize: '12px' }}>
        {importantStrings.map(i => (
          <div key={i} style={{ padding: '2px 0' }}>
            <span style={{ color: '#888' }}>str[{i.toString().padStart(3)}]</span>
            {' = '}
            <span style={{ color: '#f0c674' }}>"{stateManager.getString(i)}"</span>
          </div>
        ))}
      </div>

      <h4 style={{ margin: '12px 0 4px 0', fontSize: '13px', color: '#4a90d9' }}>Current Page</h4>
      <div style={{ fontFamily: 'monospace', fontSize: '12px' }}>
        0x{stateManager.getPage().toString(16).toUpperCase().padStart(2, '0')}
      </div>
    </div>
  )
}
