export type MessageHandler = (data: any) => void

export class WebSocketManager {
  private ws: WebSocket | null = null
  private url: string = ''
  private reconnectTimer: number | null = null
  private handlers: Map<string, MessageHandler[]> = new Map()

  onConnect: (() => void) | null = null
  onDisconnect: (() => void) | null = null

  connect(url: string) {
    this.url = url
    this.doConnect()
  }

  private doConnect() {
    try {
      this.ws = new WebSocket(this.url)

      this.ws.onopen = () => {
        console.log('[WS] Connected to', this.url)
        this.onConnect?.()
        if (this.reconnectTimer) {
          clearTimeout(this.reconnectTimer)
          this.reconnectTimer = null
        }
      }

      this.ws.onmessage = (event) => {
        try {
          const data = JSON.parse(event.data)
          const handlers = this.handlers.get(data.type) || []
          handlers.forEach(h => h(data))
          // Also notify wildcard handlers
          const wildcardHandlers = this.handlers.get('*') || []
          wildcardHandlers.forEach(h => h(data))
        } catch (e) {
          console.warn('[WS] Failed to parse message:', event.data)
        }
      }

      this.ws.onclose = () => {
        console.log('[WS] Disconnected')
        this.onDisconnect?.()
        // Auto-reconnect after 2 seconds
        this.reconnectTimer = window.setTimeout(() => this.doConnect(), 2000)
      }

      this.ws.onerror = (e) => {
        console.error('[WS] Error:', e)
      }
    } catch (e) {
      console.error('[WS] Connection failed:', e)
      this.reconnectTimer = window.setTimeout(() => this.doConnect(), 2000)
    }
  }

  disconnect() {
    if (this.reconnectTimer) {
      clearTimeout(this.reconnectTimer)
      this.reconnectTimer = null
    }
    if (this.ws) {
      this.ws.close()
      this.ws = null
    }
  }

  send(data: any) {
    if (this.ws?.readyState === WebSocket.OPEN) {
      this.ws.send(JSON.stringify(data))
    }
  }

  on(type: string, handler: MessageHandler) {
    if (!this.handlers.has(type)) {
      this.handlers.set(type, [])
    }
    this.handlers.get(type)!.push(handler)
  }

  off(type: string, handler: MessageHandler) {
    const handlers = this.handlers.get(type)
    if (handlers) {
      const idx = handlers.indexOf(handler)
      if (idx >= 0) handlers.splice(idx, 1)
    }
  }

  get isConnected() {
    return this.ws?.readyState === WebSocket.OPEN
  }
}
