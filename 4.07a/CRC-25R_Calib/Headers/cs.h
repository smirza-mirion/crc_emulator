/*****************************************************************************************
  MODULE:   Header for Chip Select IO routines  for CRC-25

  FILE:     cs.h

  DATE:     09/05/06
  ********************************************************************************************/

ushort debugPulse(void);
void initSetCS(void);
void init_chip_selects(void);
void write_lcd_cmd(uchar cmd);
void write_lcd_data(uchar data);
uchar read_lcd_data(void);
void write_keyboard(uchar data);
uchar read_keyboard(void);
void start_tone(void);
void stop_tone(void);
void set_backlight(bool ON);
bool mmc_in(void);
void power_mmc(bool ON);
void clear_counters(void);
void enable_counting(bool on);
void load_counters(void);
ulong read_counter(short cnum);
ulong read_error_counter(void);

//usb routines
void set_usb_register(uchar data);
uchar read_usb_register(void);
void write_usb_register(uchar data);
void set_usb_power(bool on);

bool usb_pc_connected(void);
bool battery_low(void);
short well_connection(void);











