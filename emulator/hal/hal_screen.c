/*
 * hal_screen.c - LCD Screen Stub for CRC-25R Emulator
 *
 * Replaces Screen.c and ScreenFonts_25.c. The firmware's LCD display
 * functions draw to a physical LCD via the chip select interface.
 * In the emulator, the display is the Amulet touchscreen (via WebSocket),
 * so these LCD functions are no-ops.
 *
 * Note: The LCD screen is a secondary display on the real hardware.
 * The primary UI is the Amulet touchscreen which is handled by
 * Amulet.c → UART2 → WebSocket bridge.
 */
#include "crc.h"
#include "coldfire.h"
#include <string.h>
#include <stdio.h>

/* Font size definitions matching Screen.c */
#ifndef SMALL
#define SMALL  0
#define MEDIUM 1
#define LARGE  2
#endif

#ifndef NORMAL
#define NORMAL  0
#define INVERSE 1
#endif

/* LCD display state - not actually rendered, but tracked for debugging */
static short lcd_cursor_x = 0;
static short lcd_cursor_y = 0;

/* Screen flags referenced by firmware - defined in ActMeasScreen_R.c */
extern bool meas_screen;

void initialize_screen(void)
{
    /* No-op: LCD initialization */
    printf("[LCD] Screen initialized\n");
}

void erase_screen(void)
{
    /* No-op: clear LCD */
}

/* display_text is provided by DisplayText.c in libfirmware.a */

void display_char(short x, short y, char c, short color, short size, short mode)
{
    (void)x; (void)y; (void)c; (void)color; (void)size; (void)mode;
}

void display_line(short x1, short y1, short x2, short y2, short color)
{
    (void)x1; (void)y1; (void)x2; (void)y2; (void)color;
}

void display_box(short x1, short y1, short x2, short y2, short color)
{
    (void)x1; (void)y1; (void)x2; (void)y2; (void)color;
}

void display_filled_box(short x1, short y1, short x2, short y2, short color)
{
    (void)x1; (void)y1; (void)x2; (void)y2; (void)color;
}

void display_pixel(short x, short y, short color)
{
    (void)x; (void)y; (void)color;
}

void set_cursor(short x, short y)
{
    lcd_cursor_x = x;
    lcd_cursor_y = y;
}

short get_cursor_x(void)
{
    return lcd_cursor_x;
}

short get_cursor_y(void)
{
    return lcd_cursor_y;
}

/* Font functions from ScreenFonts_25.c */
short get_char_width(char c, short size)
{
    (void)c;
    if (size == SMALL) return 6;
    if (size == MEDIUM) return 8;
    return 12; /* LARGE */
}

short get_char_height(short size)
{
    if (size == SMALL) return 8;
    if (size == MEDIUM) return 14;
    return 16; /* LARGE */
}

short get_string_width(char *str, short size)
{
    short w = 0;
    while (*str) {
        w += get_char_width(*str, size);
        str++;
    }
    return w;
}

void set_font(short size)
{
    (void)size;
}

/* Additional screen utility functions that may be referenced */
void clear_line(short y, short size)
{
    (void)y; (void)size;
}

void scroll_up(short lines)
{
    (void)lines;
}

void set_contrast(short level)
{
    (void)level;
}

void set_lcd_brightness(uchar brightness)
{
    (void)brightness;
}
