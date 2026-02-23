import React, { useEffect, useState } from 'react'
import { AmuletStateManager } from '../lib/amulet-state'
import { WebSocketManager } from '../lib/websocket'
import { loadScreenByPage, loadScreenMap, ScreenDef, WidgetDef } from '../lib/screen-loader'
import { executeActions as executeRemoteActions } from '../lib/action-resolver'
import { AmuletStringField } from './widgets/AmuletStringField'
import { AmuletButton } from './widgets/AmuletButton'
import { AmuletFunctionButton } from './widgets/AmuletFunctionButton'
import { AmuletBarGraph } from './widgets/AmuletBarGraph'
import { AmuletImage } from './widgets/AmuletImage'
import { AmuletRadioButton } from './widgets/AmuletRadioButton'
import { AmuletCheckBox } from './widgets/AmuletCheckBox'

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
  const [positionOverrides, setPositionOverrides] = useState<Record<string, { x?: number; y?: number }>>({})

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
        setPositionOverrides({})

        // Replay current byte values against triggers to set initial visibility.
        // The firmware may have already sent byte values (e.g. language byte 100)
        // before this screen loaded, so we need to evaluate triggers now.
        if (def.refreshTriggers) {
          const toShow = new Set<string>()
          const toHide = new Set<string>()

          // Helper to execute actions including forceUpdate chains.
          // evaluatedConditions tracks (onVar:trigger) pairs that have already
          // been evaluated. On the real Amulet display, repeated forceUpdate
          // evaluations of the same condition are no-ops (change-detection).
          // This prevents Show/Hide trigger pairs from conflicting at init.
          const allTriggers = def.refreshTriggers!
          const evaluatedConditions = new Set<string>()
          const initPosOverrides: Record<string, { x?: number; y?: number }> = {}
          const execActions = (
            actions: NonNullable<typeof allTriggers[0]['actions']>,
            depth: number,
          ) => {
            if (depth > 5) return
            for (const action of actions) {
              if (action.type === 'reappear' && action.target) {
                toShow.add(action.target)
                toHide.delete(action.target)
              } else if (action.type === 'disappear' && action.target) {
                toHide.add(action.target)
                toShow.delete(action.target)
              } else if (action.type === 'raw' && action.expression) {
                applyRawExpression(action.expression, stateManager, initPosOverrides)
              } else if (action.type === 'forceUpdate' && action.target) {
                const target = action.target
                let matched = false
                for (const t of allTriggers) {
                  if (t.name && (t.name === target || t.name.startsWith(target))) {
                    matched = true
                    if (!t.onVar || !t.trigger) continue
                    // Deduplicate: skip if this exact condition was already evaluated
                    const condKey = `${t.onVar}:${t.trigger}`
                    if (evaluatedConditions.has(condKey)) continue
                    evaluatedConditions.add(condKey)
                    // Support both byte() and word() triggers
                    const byteMatch = t.onVar.match(/byte\((\d+)\)/)
                    const wordMatch = t.onVar.match(/word\((\d+)\)/)
                    let curV: number
                    if (byteMatch) {
                      curV = stateManager.getByte(parseInt(byteMatch[1]))
                    } else if (wordMatch) {
                      curV = stateManager.getWord(parseInt(wordMatch[1]))
                    } else {
                      continue
                    }
                    const trgV = parseTriggerValue(t.trigger)
                    if (trgV !== null && trgV === curV && t.actions) {
                      execActions(t.actions, depth + 1)
                    }
                  }
                }
                if (!matched) {
                  toShow.add(target)
                  toHide.delete(target)
                }
              }
            }
          }

          // Only fire "toggle" triggers during initial replay.
          // Toggle triggers have a setValue action to reset the byte after firing
          // (e.g., byte(20)=0xFF → refreshChambString resets byte(20) to 0).
          // "State" triggers (e.g., byte(80)=0 for language) should NOT fire
          // during initial replay because both "show" and "hide" triggers match
          // the same byte value, and the last one would win incorrectly.
          // State triggers only fire via forceUpdate chains from toggle triggers.
          for (const trigger of def.refreshTriggers) {
            if (!trigger.onVar || !trigger.trigger || !trigger.actions) continue

            // Check if this is a toggle trigger (has setValue action)
            const isToggle = trigger.actions.some(a => a.type === 'setValue')
            if (!isToggle) continue

            // Support both byte() and word() triggers
            const byteMatch = trigger.onVar.match(/byte\((\d+)\)/)
            const wordMatch = trigger.onVar.match(/word\((\d+)\)/)
            let currentVal: number
            if (byteMatch) {
              currentVal = stateManager.getByte(parseInt(byteMatch[1]))
            } else if (wordMatch) {
              currentVal = stateManager.getWord(parseInt(wordMatch[1]))
            } else {
              continue
            }

            const triggerVal = parseTriggerValue(trigger.trigger)
            if (triggerVal === null || triggerVal !== currentVal) continue

            execActions(trigger.actions, 0)
          }

          // Also process forceUpdate/reappear/disappear from initActions.
          // These explicitly initialize widget visibility that may not be
          // reachable through toggle trigger chains (e.g., language triggers).
          if (def.initActions) {
            for (const action of def.initActions) {
              if (action.type === 'forceUpdate' || action.type === 'reappear' || action.type === 'disappear') {
                execActions([action], 0)
              }
            }
          }

          // Apply position overrides from raw setX/setY expressions
          if (Object.keys(initPosOverrides).length > 0) {
            setPositionOverrides(initPosOverrides)
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

        // Send firmware-side initActions (SetScreen macro, setValue, etc.)
        // via WebSocket. This tells the firmware which menu handler to
        // activate for this screen (critical handshake on page load).
        if (def.initActions) {
          const remoteActions = def.initActions.filter(a =>
            a.type === 'macro' || a.type === 'setValue'
          )
          if (remoteActions.length > 0) {
            executeRemoteActions(remoteActions, wsManager, stateManager)
          }
        }
      } else if (!cancelled) {
        setScreenDef(def)
      }
    }

    loadPage(pageIndex)

    return () => { cancelled = true }
  }, [pageIndex, stateManager, wsManager])

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

    const triggers = screenDef.refreshTriggers!

    /**
     * Execute a list of trigger actions. Handles:
     * - reappear/disappear: widget visibility
     * - forceUpdate: re-evaluate named triggers (prefix match) or refresh widgets
     * - setValue: update byte values (used to reset trigger flags)
     */
    const executeActions = (
      actions: NonNullable<typeof triggers[0]['actions']>,
      toShow: Set<string>,
      toHide: Set<string>,
      depth: number,
      posOverrides?: Record<string, { x?: number; y?: number }>,
    ) => {
      if (depth > 5) return // Prevent infinite recursion

      for (const action of actions) {
        if (action.type === 'reappear' && action.target) {
          toShow.add(action.target)
          toHide.delete(action.target)
        } else if (action.type === 'disappear' && action.target) {
          toHide.add(action.target)
          toShow.delete(action.target)
        } else if (action.type === 'raw' && action.expression && posOverrides) {
          applyRawExpression(action.expression, stateManager, posOverrides)
        } else if (action.type === 'forceUpdate' && action.target) {
          // forceUpdate on a named trigger: find triggers whose name starts
          // with the target, evaluate their conditions, and execute their actions
          // if conditions are met. This is how the Amulet display chains triggers.
          const target = action.target
          let matched = false
          for (const t of triggers) {
            if (t.name && (t.name === target || t.name.startsWith(target))) {
              matched = true
              if (!t.onVar || !t.trigger) continue
              // Support both byte() and word() triggers
              const byteMatch = t.onVar.match(/byte\((\d+)\)/)
              const wordMatch = t.onVar.match(/word\((\d+)\)/)
              let curVal: number
              if (byteMatch) {
                curVal = stateManager.getByte(parseInt(byteMatch[1]))
              } else if (wordMatch) {
                curVal = stateManager.getWord(parseInt(wordMatch[1]))
              } else {
                continue
              }
              const trigVal = parseTriggerValue(t.trigger)
              if (trigVal !== null && trigVal === curVal && t.actions) {
                executeActions(t.actions, toShow, toHide, depth + 1, posOverrides)
              }
            }
          }
          // If no trigger matched, treat as a widget forceUpdate (reappear)
          if (!matched) {
            toShow.add(target)
            toHide.delete(target)
          }
        }
        // setValue actions are handled by the firmware (it resets the byte);
        // we don't need to process them in the frontend
      }
    }

    const applyChanges = (toShow: Set<string>, toHide: Set<string>, posOverrides: Record<string, { x?: number; y?: number }>) => {
      if (toShow.size > 0 || toHide.size > 0) {
        setShownWidgets(prev => {
          const next = new Set(prev)
          for (const name of toShow) next.add(name)
          for (const name of toHide) next.delete(name)
          return next
        })
        setHiddenWidgets(prev => {
          const next = new Set(prev)
          for (const name of toShow) next.delete(name)
          for (const name of toHide) next.add(name)
          return next
        })
      }
      if (Object.keys(posOverrides).length > 0) {
        setPositionOverrides(prev => ({ ...prev, ...posOverrides }))
      }
    }

    const byteHandler = (byteIndex: number, byteValue: number) => {
      const toShow = new Set<string>()
      const toHide = new Set<string>()
      const posOverrides: Record<string, { x?: number; y?: number }> = {}

      for (const trigger of triggers) {
        if (!trigger.onVar || !trigger.trigger || !trigger.actions) continue

        // Only fire "toggle" triggers directly (those with setValue).
        // State triggers (like language byte(80)) fire via forceUpdate chains.
        const isToggle = trigger.actions.some(a => a.type === 'setValue')
        if (!isToggle) continue

        const varMatch = trigger.onVar.match(/byte\((\d+)\)/)
        if (!varMatch) continue
        const triggerByteIndex = parseInt(varMatch[1])
        if (triggerByteIndex !== byteIndex) continue

        const triggerValue = parseTriggerValue(trigger.trigger)
        if (triggerValue === null || triggerValue !== byteValue) continue

        executeActions(trigger.actions, toShow, toHide, 0, posOverrides)
      }

      applyChanges(toShow, toHide, posOverrides)
    }

    // Also listen for word changes to fire word-based triggers
    const wordHandler = (wordIndex: number, wordValue: number) => {
      const toShow = new Set<string>()
      const toHide = new Set<string>()
      const posOverrides: Record<string, { x?: number; y?: number }> = {}

      for (const trigger of triggers) {
        if (!trigger.onVar || !trigger.trigger || !trigger.actions) continue

        const isToggle = trigger.actions.some(a => a.type === 'setValue')
        if (!isToggle) continue

        const varMatch = trigger.onVar.match(/word\((\d+)\)/)
        if (!varMatch) continue
        const triggerWordIndex = parseInt(varMatch[1])
        if (triggerWordIndex !== wordIndex) continue

        const triggerValue = parseTriggerValue(trigger.trigger)
        if (triggerValue === null || triggerValue !== wordValue) continue

        executeActions(trigger.actions, toShow, toHide, 0, posOverrides)
      }

      applyChanges(toShow, toHide, posOverrides)
    }

    stateManager.onByteChange(byteHandler)
    stateManager.onWordChange(wordHandler)
    return () => {
      stateManager.offByteChange(byteHandler)
      stateManager.offWordChange(wordHandler)
    }
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
        // Get position from explicit x/y or CSS positions (mapped by div_id)
        const positions = getStaticImagePosition(img, screenDef)
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
        pointerEvents: 'none', zIndex: 0,
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
          positionOverrides={positionOverrides}
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
  positionOverrides: Record<string, { x?: number; y?: number }>
}

function WidgetRenderer({
  widget, stateManager, ws, hiddenWidgets, shownWidgets, positionOverrides,
}: WidgetRendererProps) {
  // Determine visibility from refresh trigger state
  const isHidden = hiddenWidgets.has(widget.name)
  const isForceShown = shownWidgets.has(widget.name)
  const visible = isForceShown || !isHidden

  // Apply position overrides from raw setX/setY expressions
  const posOverride = positionOverrides[widget.name]
  const effectiveWidget = posOverride
    ? { ...widget, x: posOverride.x ?? widget.x, y: posOverride.y ?? widget.y }
    : widget

  switch (effectiveWidget.type) {
    case 'StringField':
    case 'NumericField':
      return <AmuletStringField widget={effectiveWidget} stateManager={stateManager} visible={visible} />

    case 'CustomButton':
      return <AmuletButton widget={effectiveWidget} stateManager={stateManager} ws={ws} visible={visible} />

    case 'FunctionButton':
      return <AmuletFunctionButton widget={effectiveWidget} stateManager={stateManager} ws={ws} visible={visible} />

    case 'BarGraph':
      return <AmuletBarGraph widget={effectiveWidget} stateManager={stateManager} visible={visible} />

    case 'Image':
      return <AmuletImage widget={effectiveWidget} visible={visible} />

    case 'RadioButton':
      return <AmuletRadioButton widget={effectiveWidget} stateManager={stateManager} ws={ws} visible={visible} />

    case 'CheckBox':
      return <AmuletCheckBox widget={effectiveWidget} stateManager={stateManager} ws={ws} visible={visible} />

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
 * Get position for a static image. Uses explicit x/y from JSON if available,
 * otherwise falls back to known positions by div_id.
 */
function getStaticImagePosition(img: { x?: number; y?: number; div_id?: string }, _screenDef: ScreenDef): { x: number; y: number } {
  // Prefer explicit coordinates from the parser (resolved from HTM CSS)
  if (img.x !== undefined && img.y !== undefined && (img.x !== 0 || img.y !== 0)) {
    return { x: img.x, y: img.y }
  }

  const divId = img.div_id
  if (!divId) return { x: img.x ?? 0, y: img.y ?? 0 }

  // Fallback: common DIV positions from the HTM CSS
  const knownPositions: Record<string, { x: number; y: number }> = {
    topbar: { x: 0, y: 0 },
    top: { x: 0, y: 0 },
    bottombar: { x: 0, y: 537 },
    bottom: { x: 0, y: 556 },
    backgnd: { x: 0, y: 70 },
    backgnd1: { x: 0, y: 70 },
    gradient: { x: 0, y: 0 },
  }

  return knownPositions[divId] || { x: img.x ?? 0, y: img.y ?? 0 }
}

/**
 * Process raw Amulet expressions like:
 *   document.widgetName.setX(Amulet:internalRAM.word(N).value())
 *   document.widgetName.setY(Amulet:internalRAM.word(N).value())
 * Updates the posOverrides map with the new position values.
 */
function applyRawExpression(
  expression: string,
  stateManager: AmuletStateManager,
  posOverrides: Record<string, { x?: number; y?: number }>,
) {
  // Match: document.<widgetName>.setX(Amulet:internalRAM.word(<index>).value())
  const setXMatch = expression.match(/document\.(\w+)\.setX\(.*?word\((\d+)\)/)
  if (setXMatch) {
    const [, widgetName, wordIdx] = setXMatch
    const val = stateManager.getWord(parseInt(wordIdx))
    if (!posOverrides[widgetName]) posOverrides[widgetName] = {}
    posOverrides[widgetName].x = val
    return
  }
  const setYMatch = expression.match(/document\.(\w+)\.setY\(.*?word\((\d+)\)/)
  if (setYMatch) {
    const [, widgetName, wordIdx] = setYMatch
    const val = stateManager.getWord(parseInt(wordIdx))
    if (!posOverrides[widgetName]) posOverrides[widgetName] = {}
    posOverrides[widgetName].y = val
    return
  }
}
