import { WebSocketManager } from './websocket'

type StateChangeHandler = (index: number, value: any) => void

export class AmuletStateManager {
  private bytes: Uint8Array = new Uint8Array(256)
  private words: Uint16Array = new Uint16Array(256)
  private strings: string[] = new Array(256).fill('')
  private currentPage: number = 0

  private byteHandlers: StateChangeHandler[] = []
  private wordHandlers: StateChangeHandler[] = []
  private stringHandlers: StateChangeHandler[] = []

  private pageHandlers: ((page: number) => void)[] = []

  // Legacy callback (kept for backward compat)
  onPageChange: ((page: number) => void) | null = null

  constructor(private ws: WebSocketManager) {
    ws.on('setByte', (data) => {
      this.bytes[data.index] = data.value
      this.byteHandlers.forEach(h => h(data.index, data.value))
    })

    ws.on('setWord', (data) => {
      this.words[data.index] = data.value
      this.wordHandlers.forEach(h => h(data.index, data.value))
    })

    ws.on('setString', (data) => {
      this.strings[data.index] = data.value
      this.stringHandlers.forEach(h => h(data.index, data.value))
    })

    ws.on('setPage', (data) => {
      this.currentPage = data.page
      this.onPageChange?.(data.page)
      this.pageHandlers.forEach(h => h(data.page))
    })

    ws.on('fullState', (data) => {
      if (data.bytes) {
        for (const [k, v] of Object.entries(data.bytes)) {
          this.bytes[parseInt(k)] = v as number
        }
      }
      if (data.words) {
        for (const [k, v] of Object.entries(data.words)) {
          this.words[parseInt(k)] = v as number
        }
      }
      if (data.strings) {
        for (const [k, v] of Object.entries(data.strings)) {
          this.strings[parseInt(k)] = v as string
        }
      }
      if (data.page !== undefined) {
        this.currentPage = data.page
        this.onPageChange?.(data.page)
        this.pageHandlers.forEach(h => h(data.page))
      }
    })
  }

  getByte(index: number): number { return this.bytes[index] }
  getWord(index: number): number { return this.words[index] }
  getString(index: number): string { return this.strings[index] }
  getPage(): number { return this.currentPage }

  onByteChange(handler: StateChangeHandler) { this.byteHandlers.push(handler) }
  onWordChange(handler: StateChangeHandler) { this.wordHandlers.push(handler) }
  onStringChange(handler: StateChangeHandler) { this.stringHandlers.push(handler) }

  offByteChange(handler: StateChangeHandler) {
    const idx = this.byteHandlers.indexOf(handler)
    if (idx >= 0) this.byteHandlers.splice(idx, 1)
  }
  offWordChange(handler: StateChangeHandler) {
    const idx = this.wordHandlers.indexOf(handler)
    if (idx >= 0) this.wordHandlers.splice(idx, 1)
  }
  offStringChange(handler: StateChangeHandler) {
    const idx = this.stringHandlers.indexOf(handler)
    if (idx >= 0) this.stringHandlers.splice(idx, 1)
  }

  onPageChangeAdd(handler: (page: number) => void) {
    this.pageHandlers.push(handler)
  }
  offPageChange(handler: (page: number) => void) {
    const idx = this.pageHandlers.indexOf(handler)
    if (idx >= 0) this.pageHandlers.splice(idx, 1)
  }

  // Send user interaction to firmware
  pressButton(byteIndex: number, value: number) {
    this.ws.send({ type: 'buttonPress', byteIndex, value })
  }

  sendString(stringIndex: number, value: string) {
    this.ws.send({ type: 'stringInput', stringIndex, value })
  }

  sendWord(wordIndex: number, value: number) {
    this.ws.send({ type: 'wordChanged', wordIndex, value })
  }
}
