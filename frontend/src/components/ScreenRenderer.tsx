import React, { useEffect, useState } from 'react'
import { AmuletStateManager } from '../lib/amulet-state'
import { WebSocketManager } from '../lib/websocket'
import { loadScreenByPage, loadScreenMap, ScreenDef, WidgetDef } from '../lib/screen-loader'
import { AmuletStringField } from './widgets/AmuletStringField'
import { AmuletButton } from './widgets/AmuletButton'
import { AmuletFunctionButton } from './widgets/AmuletFunctionButton'
import { AmuletBarGraph } from './widgets/AmuletBarGraph'
import { AmuletImage } from './widgets/AmuletImage'

interface Props {
  stateManager: AmuletStateManager
  wsManager: WebSocketManager
}

interface DrawCommand {
  type: string
  [key: string]: any
}

/**
 * ScreenRenderer dynamically loads and renders Amulet screen definitions
 * based on the current page index from the firmware.
 */
export function ScreenRenderer({ stateManager, wsManager }: Props) {
  const [screenDef, setScreenDef] = useState<ScreenDef | null>(null)
  const [pageIndex, setPageIndex] = useState(stateManager.getPage())
  const [hiddenWidgets, setHiddenWidgets] = useState<Set<string>>(new Set())
  const [shownWidgets, setShownWidgets] = useState<Set<string>>(new Set())
  const [drawCommands, setDrawCommands] = useState<DrawCommand[]>([])

  // Preload screen map on mount
  useEffect(() => {
    loadScreenMap()
  }, [])

  // Load screen definition when page changes
  useEffect(() => {
    let cancelled = false

    const loadPage = async (page: number) => {
      const def = await loadScreenByPage(page)
      if (!cancelled && def) {
        // Collect all widgets that start invisible
        const initiallyHidden = new Set<string>()
        for (const w of def.widgets) {
          if (w.params?.invisible?.toUpperCase() === 'TRUE') {
            initiallyHidden.add(w.name)
          }
        }

        setScreenDef(def)
        setHiddenWidgets(initiallyHidden)
        setShownWidgets(new Set())
        setDrawCommands([])

        // Replay current byte values against triggers to set initial visibility.
        // The firmware may have already sent byte values (e.g. language byte 100)
        // before this screen loaded, so we need to evaluate triggers now.
        if (def.refreshTriggers) {
          const toShow = new Set<string>()
          const toHide = new Set<string>()

          for (const trigger of def.refreshTriggers) {
            if (!trigger.onVar || !trigger.trigger || !trigger.actions) continue

            const varMatch = trigger.onVar.match(/byte\((\d+)\)/)
            if (!varMatch) continue
            const byteIdx = parseInt(varMatch[1])
            const currentVal = stateManager.getByte(byteIdx)

            const triggerVal = parseTriggerValue(trigger.trigger)
            if (triggerVal === null || triggerVal !== currentVal) continue

            for (const action of trigger.actions) {
              if (action.type === 'reappear' && action.target) {
                toShow.add(action.target)
                toHide.delete(action.target)
              } else if (action.type === 'disappear' && action.target) {
                toHide.add(action.target)
                toShow.delete(action.target)
              }
            }
          }

          if (toShow.size > 0 || toHide.size > 0) {
            setShownWidgets(toShow)
            setHiddenWidgets(prev => {
              const next = new Set(prev)
              for (const name of toShow) next.delete(name)
              for (const name of toHide) next.add(name)
              return next
            })
          }
        }
      } else if (!cancelled) {
        setScreenDef(def)
      }
    }

    loadPage(pageIndex)

    return () => { cancelled = true }
  }, [pageIndex, stateManager])

  // Listen for page changes from firmware
  useEffect(() => {
    const handler = (page: number) => {
      setPageIndex(page)
    }
    stateManager.onPageChangeAdd(handler)
    return () => {
      stateManager.offPageChange(handler)
    }
  }, [stateManager])

  // Listen for draw commands (lines, rectangles, fill rects)
  useEffect(() => {
    const handleDraw = (data: any) => {
      setDrawCommands(prev => [...prev, data])
    }
    const handlePage = () => {
      setDrawCommands([])
    }

    wsManager.on('drawLine', handleDraw)
    wsManager.on('fillRect', handleDraw)
    wsManager.on('setPage', handlePage)

    return () => {
      wsManager.off('drawLine', handleDraw)
      wsManager.off('fillRect', handleDraw)
      wsManager.off('setPage', handlePage)
    }
  }, [wsManager])

  // Process refresh triggers when byte values change
  useEffect(() => {
    if (!screenDef?.refreshTriggers) return

    const handler = (byteIndex: number, byteValue: number) => {
      for (const trigger of screenDef.refreshTriggers!) {
        if (!trigger.onVar || !trigger.trigger || !trigger.actions) continue

        // Parse onVar: "internalRAM.byte(N)"
        const varMatch = trigger.onVar.match(/byte\((\d+)\)/)
        if (!varMatch) continue
        const triggerByteIndex = parseInt(varMatch[1])
        if (triggerByteIndex !== byteIndex) continue

        // Parse trigger value
        const triggerValue = parseTriggerValue(trigger.trigger)
        if (triggerValue === null || triggerValue !== byteValue) continue

        // Execute display-side actions
        for (const action of trigger.actions) {
          if (action.type === 'reappear' && action.target) {
            setShownWidgets(prev => new Set([...prev, action.target!]))
            setHiddenWidgets(prev => {
              const next = new Set(prev)
              next.delete(action.target!)
              return next
            })
          } else if (action.type === 'disappear' && action.target) {
            setHiddenWidgets(prev => new Set([...prev, action.target!]))
            setShownWidgets(prev => {
              const next = new Set(prev)
              next.delete(action.target!)
              return next
            })
          }
        }
      }
    }

    stateManager.onByteChange(handler)
    return () => stateManager.offByteChange(handler)
  }, [screenDef, stateManager])

  if (!screenDef) {
    return (
      <div style={{ width: '100%', height: '100%', position: 'relative' }}>
        {/* Fallback: show basic hardcoded layout while loading */}
        <FallbackDisplay stateManager={stateManager} drawCommands={drawCommands} />
      </div>
    )
  }

  return (
    <div style={{ width: '100%', height: '100%', position: 'relative' }}>
      {/* Static background images from the HTM file */}
      {screenDef.staticImages?.map((img, i) => {
        const src = normalizeImagePath(img.src)
        // Get position from CSS positions (mapped by div_id)
        const positions = getStaticImagePosition(img.div_id, screenDef)
        return (
          <img
            key={`static-${i}`}
            src={src}
            width={img.width}
            height={img.height}
            style={{
              position: 'absolute',
              left: positions.x,
              top: positions.y,
            }}
            draggable={false}
          />
        )
      })}

      {/* Draw commands (lines, fill rects) from firmware */}
      <svg style={{
        position: 'absolute', top: 0, left: 0, width: 800, height: 600,
        pointerEvents: 'none', zIndex: 1,
      }}>
        {drawCommands.map((cmd, i) => {
          if (cmd.type === 'drawLine') {
            const color = `rgb(${cmd.r || 0},${cmd.g || 0},${cmd.b || 0})`
            return (
              <line key={i}
                x1={cmd.x1} y1={cmd.y1} x2={cmd.x2} y2={cmd.y2}
                stroke={color} strokeWidth={cmd.weight || 1}
              />
            )
          }
          if (cmd.type === 'fillRect') {
            const color = `rgb(${cmd.r || 0},${cmd.g || 0},${cmd.b || 0})`
            return (
              <rect key={i}
                x={cmd.x} y={cmd.y} width={cmd.dx} height={cmd.dy}
                fill={color}
              />
            )
          }
          return null
        })}
      </svg>

      {/* Dynamic widgets from screen definition */}
      {screenDef.widgets.map((widget, i) => (
        <WidgetRenderer
          key={`${widget.name || `w${i}`}`}
          widget={widget}
          stateManager={stateManager}
          ws={wsManager}
          hiddenWidgets={hiddenWidgets}
          shownWidgets={shownWidgets}
        />
      ))}
    </div>
  )
}

interface WidgetRendererProps {
  widget: WidgetDef
  stateManager: AmuletStateManager
  ws: WebSocketManager
  hiddenWidgets: Set<string>
  shownWidgets: Set<string>
}

function WidgetRenderer({
  widget, stateManager, ws, hiddenWidgets, shownWidgets,
}: WidgetRendererProps) {
  // Determine visibility from refresh trigger state
  const isHidden = hiddenWidgets.has(widget.name)
  const isForceShown = shownWidgets.has(widget.name)
  const visible = isForceShown || !isHidden

  switch (widget.type) {
    case 'StringField':
    case 'NumericField':
      return <AmuletStringField widget={widget} stateManager={stateManager} visible={visible} />

    case 'CustomButton':
      return <AmuletButton widget={widget} stateManager={stateManager} ws={ws} visible={visible} />

    case 'FunctionButton':
      return <AmuletFunctionButton widget={widget} stateManager={stateManager} ws={ws} visible={visible} />

    case 'BarGraph':
      return <AmuletBarGraph widget={widget} stateManager={stateManager} visible={visible} />

    case 'Image':
      return <AmuletImage widget={widget} visible={visible} />

    default:
      return null
  }
}

/**
 * Fallback display used when no screen definition is loaded yet.
 * Shows a basic hardcoded layout with the data available from the state manager.
 */
function FallbackDisplay({
  stateManager,
  drawCommands,
}: {
  stateManager: AmuletStateManager
  drawCommands: DrawCommand[]
}) {
  const [strings, setStrings] = useState<Record<number, string>>({})

  useEffect(() => {
    const handler = (index: number, value: string) => {
      setStrings(prev => ({ ...prev, [index]: value }))
    }
    stateManager.onStringChange(handler)
    return () => stateManager.offStringChange(handler)
  }, [stateManager])

  return (
    <>
      <div style={{
        position: 'absolute', top: 0, left: 0, right: 0, bottom: 0,
        background: 'linear-gradient(180deg, #3333AA 0%, #3333AA 69px, #DDEEFF 70px)',
      }} />

      <svg style={{
        position: 'absolute', top: 0, left: 0, width: 800, height: 600,
        pointerEvents: 'none',
      }}>
        {drawCommands.map((cmd, i) => {
          if (cmd.type === 'drawLine') {
            const color = `rgb(${cmd.r || 0},${cmd.g || 0},${cmd.b || 0})`
            return <line key={i} x1={cmd.x1} y1={cmd.y1} x2={cmd.x2} y2={cmd.y2} stroke={color} strokeWidth={cmd.weight || 1} />
          }
          if (cmd.type === 'fillRect') {
            const color = `rgb(${cmd.r || 0},${cmd.g || 0},${cmd.b || 0})`
            return <rect key={i} x={cmd.x} y={cmd.y} width={cmd.dx} height={cmd.dy} fill={color} />
          }
          return null
        })}
      </svg>

      {/* Title */}
      <div style={{
        position: 'absolute', top: 5, left: 0, right: 0,
        textAlign: 'center', color: 'white', fontSize: 24, fontWeight: 'bold',
        fontFamily: '"Arial Black", sans-serif',
      }}>
        {strings[80] || strings[100] || 'CRC-25R Calibrator'}
      </div>

      {/* Activity */}
      <div style={{
        position: 'absolute', top: 200, left: 180, width: 428, height: 114,
        display: 'flex', alignItems: 'center', justifyContent: 'flex-end',
        fontSize: 72, fontWeight: 'bold',
        fontFamily: '"Franklin Gothic Medium", Arial, sans-serif',
        paddingRight: 10,
      }}>
        {strings[10] || '---'}
      </div>

      {/* Unit */}
      <div style={{
        position: 'absolute', top: 240, left: 620,
        fontSize: 24, fontFamily: '"Franklin Gothic Medium", sans-serif',
      }}>
        {strings[11] || ''}
      </div>

      {/* Nuclide */}
      <div style={{
        position: 'absolute', top: 75, left: 10,
        fontSize: 18, fontFamily: '"Franklin Gothic Medium", sans-serif',
      }}>
        {strings[101] || ''}
      </div>

      {/* Chamber */}
      <div style={{
        position: 'absolute', top: 545, left: 120,
        fontSize: 16, fontFamily: 'sans-serif', color: '#000',
      }}>
        {strings[9] || ''}
      </div>

      {/* Time */}
      <div style={{
        position: 'absolute', top: 5, right: 10,
        fontSize: 14, color: 'white', fontFamily: 'monospace',
      }}>
        {strings[2] || ''}
      </div>

      {/* Loading indicator */}
      <div style={{
        position: 'absolute', bottom: 70, left: 0, right: 0,
        textAlign: 'center', color: '#999', fontSize: 12,
      }}>
        Loading screen definition...
      </div>
    </>
  )
}

function parseTriggerValue(trigger: string): number | null {
  if (trigger.startsWith('0x') || trigger.startsWith('0X')) {
    return parseInt(trigger, 16)
  }
  const n = parseInt(trigger, 10)
  return isNaN(n) ? null : n
}

function normalizeImagePath(src: string): string {
  if (src.startsWith('Images/')) {
    return `/assets/images/${src.substring(7)}`
  }
  return `/assets/images/${src}`
}

/**
 * Get position for a static image based on its parent DIV ID.
 * The CSS positions are parsed from the HTM file and stored in the screen definition.
 */
function getStaticImagePosition(divId: string | undefined, _screenDef: ScreenDef): { x: number; y: number } {
  if (!divId) return { x: 0, y: 0 }

  // Common DIV positions from the HTM CSS (hardcoded for known layouts)
  const knownPositions: Record<string, { x: number; y: number }> = {
    topbar: { x: 0, y: 0 },
    bottombar: { x: 0, y: 537 },
    backgnd1: { x: 0, y: 70 },
    gradient: { x: 0, y: 0 },
  }

  return knownPositions[divId] || { x: 0, y: 0 }
}
