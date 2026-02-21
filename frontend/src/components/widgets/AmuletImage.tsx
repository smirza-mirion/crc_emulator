import React from 'react'
import { WidgetDef } from '../../lib/screen-loader'

interface Props {
  widget: WidgetDef
  visible: boolean
}

/**
 * Renders an Amulet Image widget (named IMG elements from HTM).
 */
export const AmuletImage: React.FC<Props> = ({ widget, visible }) => {
  const params = widget.params || {}

  const isInvisible = params.invisible?.toUpperCase() === 'TRUE'
  if (!visible || isInvisible) return null

  const src = normalizeImagePath(params.src)
  if (!src) return null

  return (
    <img
      src={src}
      width={widget.width}
      height={widget.height}
      style={{
        position: 'absolute',
        left: widget.x,
        top: widget.y,
      }}
      draggable={false}
    />
  )
}

function normalizeImagePath(src: string | undefined): string | undefined {
  if (!src) return undefined
  if (src.startsWith('Images/')) {
    return `/assets/images/${src.substring(7)}`
  }
  return `/assets/images/${src}`
}
