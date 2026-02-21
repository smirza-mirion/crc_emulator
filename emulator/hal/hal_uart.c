/*
 * hal_uart.c - UART Emulation for CRC-25R Emulator
 *
 * Replaces UartRoutines.c. Provides the same UART interface but uses
 * in-memory ring buffers instead of hardware UART registers.
 *
 * Channel 0 (U_PR): Printer - TX logged to file
 * Channel 1 (U_PC): PC communications - TX/RX available for debug
 * Channel 2: Amulet touchscreen - TX/RX bridged to WebSocket
 *
 * The Amulet firmware (Amulet.c) calls SendUart2Tx() to send commands
 * to the display, and reads from UART2 RX for touchscreen responses.
 * The emulator's Amulet bridge intercepts these.
 */
#include "compat.h"
#include "crc.h"
#include "coldfire.h"
#include "uart.h"
#include "hal.h"
#include <string.h>
#include <stdio.h>

/* Amulet mode flag - set to 1 to enable UART2 Amulet protocol processing.
 * In real firmware, this is defined in UartRoutines.c (excluded from emulator build). */
volatile unsigned char g_flgAmuletMode = 1;

/* Ring buffer size from firmware uart.h */
#ifndef UART_BUFFER_SIZE
#define UART_BUFFER_SIZE 1024
#endif

/* Ring buffer structure */
typedef struct {
    volatile char buffer[UART_BUFFER_SIZE];
    volatile unsigned short head;
    volatile unsigned short tail;
} RingBuffer;

/* Dual buffer for UART2 TX (encoded + raw) */
typedef struct {
    volatile char buffer[UART_BUFFER_SIZE];
    volatile char raw[UART_BUFFER_SIZE];
    volatile unsigned short head;
    volatile unsigned short tail;
} RingBuffer2;

/* UART 1 (Printer/PC) buffers */
static RingBuffer uart1_rx;
static RingBuffer uart1_tx;

/* UART 2 (Amulet) buffers */
static RingBuffer uart2_rx;
static RingBuffer2 uart2_tx;

/* PC communication state */
static volatile bool pc_char_waiting_flag = FALSE;

/* Printer output file */
static FILE *printer_file = NULL;

/* ---- Ring Buffer Operations ---- */

static unsigned short ring_count(unsigned short head, unsigned short tail)
{
    if (head >= tail)
        return head - tail;
    else
        return UART_BUFFER_SIZE - tail + head;
}

static char ring_push(RingBuffer *rb, char c)
{
    unsigned short next = (rb->head + 1) % UART_BUFFER_SIZE;
    if (next == rb->tail) return 0; /* full */
    rb->buffer[rb->head] = c;
    rb->head = next;
    return 1;
}

static char ring_pop(RingBuffer *rb, char *c)
{
    if (rb->head == rb->tail) return 0; /* empty */
    *c = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % UART_BUFFER_SIZE;
    return 1;
}

/* ---- UART Initialization ---- */

void init_uart(int BaudRate, int iu)
{
    (void)BaudRate;
    (void)iu;
    /* In the emulator, UART is pre-configured. Just clear buffers. */
    if (iu == U_PR || iu == 0) {
        memset((void *)&uart1_rx, 0, sizeof(RingBuffer));
        memset((void *)&uart1_tx, 0, sizeof(RingBuffer));
    }
    /* UART2 (Amulet) is initialized separately */
}

void hal_uart_init(void)
{
    memset((void *)&uart1_rx, 0, sizeof(RingBuffer));
    memset((void *)&uart1_tx, 0, sizeof(RingBuffer));
    memset((void *)&uart2_rx, 0, sizeof(RingBuffer));
    memset((void *)&uart2_tx, 0, sizeof(RingBuffer2));

    /* Open printer output file */
    printer_file = fopen("data/printer_output.txt", "a");
}

void hal_uart_shutdown(void)
{
    if (printer_file) {
        fclose(printer_file);
        printer_file = NULL;
    }
}

/* ---- Core UART Functions ---- */

void uart_putchar(int c, int iu)
{
    if (iu == U_PR) {
        /* Printer: log to file */
        if (printer_file) {
            fputc(c, printer_file);
            fflush(printer_file);
        }
    } else if (iu == U_PC) {
        /* PC comm: store in TX buffer for potential debug output */
        ring_push(&uart1_tx, (char)c);
    }
    /* UART2 (Amulet) is handled via SendUart2Tx/PushUart2Tx */
}

void uart_write(char *str, int iu)
{
    while (*str) {
        uart_putchar((int)*str, iu);
        str++;
    }
}

void uart_write_num(char *str, short num, int iu)
{
    short i;
    for (i = 0; i < num; i++) {
        uart_putchar((int)str[i], iu);
    }
}

int uart_getchar(bool wait_flag, int iu)
{
    char c;
    if (iu == U_PR) {
        /* Not typically reading from printer */
        return -1;
    } else if (iu == U_PC) {
        if (wait_flag) {
            while (!ring_pop(&uart1_rx, &c)) {
                /* busy wait - in emulator, add small sleep */
#ifdef PLATFORM_MACOS
                usleep(1000);
#elif defined(PLATFORM_WINDOWS)
                Sleep(1);
#endif
            }
            return (int)(unsigned char)c;
        } else {
            if (ring_pop(&uart1_rx, &c))
                return (int)(unsigned char)c;
            return -1;
        }
    }
    return -1;
}

bool uart_char_waiting(int iu)
{
    if (iu == U_PR) return FALSE;
    if (iu == U_PC) return (uart1_rx.head != uart1_rx.tail) ? TRUE : FALSE;
    return FALSE;
}

bool is_pc_char_waiting(void)
{
    return pc_char_waiting_flag;
}

/* ---- PC Communication Stubs ---- */
/* from_pc, from_pc_cdc, get_pc_ret, clear_pc_ret, set_pc_start_flag
 * are provided by PCComm_25.c in libfirmware.a */

bool rs_enabled(void)
{
    return FALSE; /* RS-232 not available in emulator */
}

void from_remote(void)
{
    /* No remote communication in emulator */
}

/* ---- UART1 Ring Buffer Functions (Printer/PC) ---- */

void ClearUart1Rx(void)
{
    uart1_rx.head = 0;
    uart1_rx.tail = 0;
}

void ClearUart1Tx(void)
{
    uart1_tx.head = 0;
    uart1_tx.tail = 0;
}

short unsigned int QueryPendingUart1Rx(void)
{
    return ring_count(uart1_rx.head, uart1_rx.tail);
}

short unsigned int QueryPendingUart1Tx(void)
{
    return ring_count(uart1_tx.head, uart1_tx.tail);
}

char PushUart1Rx(char cInput)
{
    return ring_push(&uart1_rx, cInput);
}

char PushUart1Tx(char cInput)
{
    /* For printer output, also write to file */
    if (printer_file) {
        fputc(cInput, printer_file);
        fflush(printer_file);
    }
    return ring_push(&uart1_tx, cInput);
}

char PopUart1Rx(char *cOutput)
{
    return ring_pop(&uart1_rx, cOutput);
}

char PopUart1Tx(char *cOutput)
{
    return ring_pop(&uart1_tx, cOutput);
}

char SendUart1Tx(char *cOutput)
{
    /* Send a null-terminated string via UART1 (printer/PC) */
    unsigned short uiLength, uiFree;
    if (!cOutput || *cOutput == '\0') return 0;

    uiLength = (unsigned short)(strlen(cOutput) + 1);
    uiFree = UART_BUFFER_SIZE - 1 - QueryPendingUart1Tx();

    if (uiFree >= uiLength) {
        while (*cOutput != 0) {
            PushUart1Tx(*cOutput);
            cOutput++;
        }
        PushUart1Tx(0);
        return 0;
    }
    return -1;
}

void SendNAK1(void)
{
    /* NAK for printer channel - no-op */
}

void EnableTx1(void)
{
    /* TX always enabled in emulator */
}

/* ---- UART2 Ring Buffer Functions (Amulet) ---- */

void ClearUart2Rx(void)
{
    uart2_rx.head = 0;
    uart2_rx.tail = 0;
}

void ClearUart2Tx(void)
{
    uart2_tx.head = 0;
    uart2_tx.tail = 0;
}

short unsigned int QueryPendingUart2Rx(void)
{
    return ring_count(uart2_rx.head, uart2_rx.tail);
}

short unsigned int QueryPendingUart2Tx(void)
{
    return ring_count(uart2_tx.head, uart2_tx.tail);
}

char PushUart2Rx(char cInput)
{
    return ring_push(&uart2_rx, cInput);
}

char PushUart2Tx(char cInput, char raw)
{
    unsigned short next = (uart2_tx.head + 1) % UART_BUFFER_SIZE;
    if (next == uart2_tx.tail) return 0; /* full */
    uart2_tx.buffer[uart2_tx.head] = cInput;
    uart2_tx.raw[uart2_tx.head] = raw;
    uart2_tx.head = next;

    /* Feed the encoded byte to the Amulet bridge for WebSocket forwarding.
     * We pass cInput (the encoded/protocol byte) not raw.
     * For regular commands, cInput contains hex-encoded chars; raw is 0.
     * For HTML commands (0xA0 prefix), cInput contains binary bytes; raw is -1 (0xFF).
     * The bridge uses the raw flag to distinguish binary vs hex-encoded packets. */
    amulet_bridge_process_tx((unsigned char)cInput, (unsigned char)raw);


    return 1;
}

char PopUart2Rx(char *cOutput)
{
    return ring_pop(&uart2_rx, cOutput);
}

char PopUart2Tx(char *cOutput, char *raw)
{
    if (uart2_tx.head == uart2_tx.tail) return 0;
    *cOutput = uart2_tx.buffer[uart2_tx.tail];
    if (raw) *raw = uart2_tx.raw[uart2_tx.tail];
    uart2_tx.tail = (uart2_tx.tail + 1) % UART_BUFFER_SIZE;
    return 1;
}

/*
 * SendUart2Tx - Send a null-terminated command string to the Amulet display.
 *
 * This matches the firmware's UartRoutines.c:SendUart2Tx() behavior:
 * Takes a null-terminated string, pushes each byte via PushUart2Tx(),
 * then pushes a null terminator to signal end of packet.
 *
 * For binary HTML commands (0xA0 0x02 ...), the hex-encoded page bytes
 * are converted to binary before sending.
 */
char SendUart2Tx(char *cOutput)
{
    unsigned char *p = (unsigned char *)cOutput;
    unsigned char first, second;
    unsigned short uiLength, uiFree;

    if (!cOutput || *cOutput == '\0') return 0;

    first = p[0];
    second = p[1];

    /* Check for binary HTML page navigation: 0xA0 0x02 + hex-encoded page */
    if (first == 0xA0 && second == 0x02) {
        unsigned char cBinary[6];
        extern unsigned char hex_to_byte_fw(char hi, char lo);

        cBinary[0] = 0xA0;
        cBinary[1] = 0x02;
        /* Convert hex-encoded page number to binary */
        cBinary[2] = (unsigned char)(
            ((p[2] >= 'A' ? p[2] - 'A' + 10 : p[2] - '0') << 4) |
             (p[3] >= 'A' ? p[3] - 'A' + 10 : p[3] - '0'));
        cBinary[3] = (unsigned char)(
            ((p[4] >= 'A' ? p[4] - 'A' + 10 : p[4] - '0') << 4) |
             (p[5] >= 'A' ? p[5] - 'A' + 10 : p[5] - '0'));
        cBinary[4] = (unsigned char)(
            ((p[6] >= 'A' ? p[6] - 'A' + 10 : p[6] - '0') << 4) |
             (p[7] >= 'A' ? p[7] - 'A' + 10 : p[7] - '0'));

        uiLength = 6;
        uiFree = UART_BUFFER_SIZE - 1 - QueryPendingUart2Tx();
        if (uiFree >= uiLength) {
            PushUart2Tx((char)cBinary[0], -1);
            PushUart2Tx((char)cBinary[1], -1);
            PushUart2Tx((char)cBinary[2], -1);
            PushUart2Tx((char)cBinary[3], -1);
            PushUart2Tx((char)cBinary[4], -1);
            PushUart2Tx(0, -1);
            return 0;
        }
        return -1;
    }

    /* Regular command: push each byte + null terminator */
    uiLength = (unsigned short)(strlen(cOutput) + 1);
    uiFree = UART_BUFFER_SIZE - 1 - QueryPendingUart2Tx();

    if (uiFree >= uiLength) {
        while (*cOutput != 0) {
            PushUart2Tx(*cOutput, 0);
            cOutput++;
        }
        PushUart2Tx(0, 0);  /* Null terminator = end of packet */
        return 0;
    }

    return -1;  /* Buffer full */
}

void SendNAK2(void)
{
    /* NAK for Amulet channel */
    PushUart2Tx(0x15, 0x15); /* NAK byte */
}

void EnableTx2(void)
{
    /* TX always enabled in emulator */
}

void SendWakeUp(void)
{
    /* Send wake-up sequence to Amulet - in emulator, just signal the bridge */
    PushUart2Tx(0x00, 0x00);
}

void printUart2(void)
{
    /* Debug print of UART2 state - no-op in emulator */
}

/* ---- Interrupt Handlers (emulated) ---- */
/* These are called by the timer thread instead of hardware interrupts */

void uart1_interrupt(void)
{
    /* In real firmware, this handles UART1 TX/RX interrupts.
     * In the emulator, we handle TX/RX synchronously, so this
     * just needs to drain the TX buffer. */
    char c;
    while (PopUart1Tx(&c)) {
        if (printer_file) {
            fputc(c, printer_file);
        }
    }
}

void amulet_interrupt(void)
{
    /* In real firmware, this handles UART2 TX/RX interrupts.
     * In the emulator, TX is handled synchronously in PushUart2Tx.
     * RX is fed by the Amulet bridge. */

    /* Poll the bridge for incoming data */
    amulet_bridge_poll_rx();
}

/* ---- Terminal Functions (when TERMINAL is defined) ---- */
#ifdef TERMINAL
#include <stdarg.h>
void pf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void from_terminal(char key)
{
    (void)key;
}
#endif
