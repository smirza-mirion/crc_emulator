import React, { useEffect, useState } from 'react'
import { WidgetDef } from '../../lib/screen-loader'
import { AmuletStateManager } from '../../lib/amulet-state'

interface Props {
  widget: WidgetDef
  stateManager: AmuletStateManager
  visible: boolean
}

/**
 * Renders an Amulet BarGraph widget.
 * BarGraph displays a horizontal progress bar bound to a UART byte value.
 */
export const AmuletBarGraph: React.FC<Props> = ({ widget, stateManager, visible }) => {
  const params = widget.params || {}

  const hrefMatch = params.href?.match(/(?:UART|internalRAM)\.byte\((\d+)\)/)
  const byteIndex = hrefMatch ? parseInt(hrefMatch[1]) : -1

  const min = parseInt(params.min || '0')
  const max = parseInt(params.max || '100')

  const [value, setValue] = useState(0)

  useEffect(() => {
    if (byteIndex >= 0) {
      // Load initial value so the bar doesn't start at 0
      setValue(stateManager.getByte(byteIndex))

      const handler = (index: number, val: number) => {
        if (index === byteIndex) setValue(val)
      }
      stateManager.onByteChange(handler)
      return () => stateManager.offByteChange(handler)
    }
  }, [byteIndex, stateManager])

  if (!visible) return null

  const range = max - min
  const pct = range > 0 ? Math.min(100, Math.max(0, ((value - min) / range) * 100)) : 0

  return (
    <div
      style={{
        position: 'absolute',
        left: widget.x,
        top: widget.y,
        width: widget.width,
        height: widget.height,
        backgroundColor: '#e0e0e0',
        border: '1px solid #999',
        overflow: 'hidden',
      }}
    >
      <div
        style={{
          width: `${pct}%`,
          height: '100%',
          backgroundColor: '#4a90d9',
          transition: 'width 0.3s ease',
        }}
      />
    </div>
  )
}
