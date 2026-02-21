/*
 * hal.h - Hardware Abstraction Layer for CRC-25R Emulator
 *
 * This header declares the HAL interface. Each hardware subsystem
 * (UART, I2C, QSPI, PIT, etc.) has its own implementation file
 * that replaces the corresponding firmware hardware driver.
 */
#ifndef EMULATOR_HAL_H
#define EMULATOR_HAL_H

#include "compat.h"
#include "crc.h"
#include "coldfire.h"

/* ---- HAL Initialization ---- */
void hal_init(void);
void hal_shutdown(void);

/* ---- ColdFire Register Emulation (hal_coldfire.c) ---- */
/* The global 'cf' struct and 'vector_base' are allocated in hal_coldfire.c */
/* They're declared extern in coldfire.h and used throughout firmware */

/* ---- UART Emulation (hal_uart.c) ---- */
/* Implements: init_uart, uart_putchar, uart_write, uart_write_num,
 *             uart_getchar, uart_char_waiting, is_pc_char_waiting,
 *             ClearUart1Rx/Tx, ClearUart2Rx/Tx,
 *             QueryPendingUart1Rx/Tx, QueryPendingUart2Rx/Tx,
 *             PushUart1Rx/Tx, PushUart2Rx/Tx,
 *             PopUart1Rx/Tx, PopUart2Rx/Tx,
 *             SendUart1Tx, SendUart2Tx, SendNAK1, SendNAK2,
 *             EnableTx1, EnableTx2, SendWakeUp, printUart2,
 *             uart1_interrupt, amulet_interrupt
 */

/* ---- I2C + EEPROM Emulation (hal_i2c.c) ---- */
/* Implements: initialize_i2c, initSetI2C, setup_i2c,
 *             i2c_write, i2c_read, get_i2c_in_use,
 *             SetVolume, ReadSN, WriteSN,
 *             eewrite, eeread, EE_READ, EE_WRITE,
 *             VolumeMirror, VolumeMirrorUpdate,
 *             BrightnessMirror, BrightnessMirrorUpdate,
 *             SleepBrightnessMirror, SleepBrightnessUpdate,
 *             SleepTimeoutMirror, SleepTimeoutUpdate,
 *             SleepRefreshTimeout, EepromRoutines_sleepNow,
 *             EepromRoutines_sleepWake
 */

/* ---- QSPI / ADC / DAC Emulation (hal_qspi.c) ---- */
/* Implements: initSetQSPI, spiClearSemaphore,
 *             set_adc_enabled, get_adc_enabled,
 *             set_adc_mode, service_adc, SetADCStateToIdle,
 *             max132_set_output_pins, mcp3550_set_output_pins,
 *             set_chamber_spi
 */

/* ---- PIT Timer Emulation (hal_pit.c) ---- */
/* Implements: delay_msec, delay_micro_sec,
 *             reset_timed_interrupt, initialize_interrupts,
 *             disable_all_interrupts,
 *             reset_sec_counter, get_sec_counter,
 *             reset_minute_counter, get_minute_counter,
 *             display_time_set, clear_display_time, set_display_time,
 *             reset_minute_counter_with_seconds,
 *             init_screen_saver, key_press_screen_save,
 *             delayloop, clear_milli_sec_counter,
 *             timed_interrupt, sec_interrupt,
 *             irq5_interrupt, usb_interrupt
 */

/* ---- FatFS → POSIX Wrapper (hal_fatfs.c) ---- */
/* Implements: f_mount, f_open, f_close, f_read, f_write,
 *             f_lseek, f_getfree, f_truncate, f_sync,
 *             f_opendir, f_readdir, f_stat, f_mkdir, f_unlink, f_rename
 */

/* ---- GPIO / Chip Select Stubs (hal_gpio.c) ---- */
/* Implements: initSetCS, init_chip_selects,
 *             write_lcd_cmd, write_lcd_data, read_lcd_data,
 *             write_keyboard, read_keyboard,
 *             start_tone, stop_tone, set_backlight,
 *             mmc_in, power_mmc, set_usb_power, set_host_usb_power,
 *             set_touch_power, set_touch_reset, set_touch_program,
 *             set_usb_register, read_usb_register, write_usb_register,
 *             usb_pc_connected, battery_low,
 *             read_mca_port, write_mca_port
 */

/* ---- USB Stub (hal_usb.c) ---- */
/* Implements: initSetUSB, startup_811 */

/* ---- RTC (hal_rtc.c) ---- */
/* Implements: gettime, getdate, read_clock, set_clock, setup_rtc */

/* ---- Watchdog Stub (hal_watchdog.c) ---- */
/* Implements: setup_watchdog, service_watchdog */

/* ---- Screen Stub (hal_screen.c) ---- */
/* Implements: initialize_screen, erase_screen, display_text, etc. */
/* The LCD screen functions are no-ops since the Amulet touchscreen
 * (via WebSocket) is the primary display in the emulator. */

/* ---- Keyboard Stub (hal_keyboard.c) ---- */
/* Implements: scan_keyboard, init_keyboard, etc. */
/* Physical keyboard is not used; all input comes from Amulet touchscreen. */

/* ---- Simulation Data Access ---- */
/* These functions are called by the HAL to get simulated ADC values */
long sim_get_adc_value(short channel);
void sim_set_source_activity(short channel, float activity_uci);

/* ---- Amulet Bridge ---- */
/* The bridge intercepts UART channel 2 traffic */
void amulet_bridge_init(int port);
void amulet_bridge_shutdown(void);
void amulet_bridge_process_tx(unsigned char encoded_byte, unsigned char raw_flag);
void amulet_bridge_poll_rx(void);

/* ---- Timer Thread Control ---- */
void hal_timer_start(void);
void hal_timer_stop(void);

#endif /* EMULATOR_HAL_H */
