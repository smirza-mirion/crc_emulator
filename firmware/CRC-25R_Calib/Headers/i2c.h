#pragma once

//header for I2C Module and devices that use it
//i2c.h
//05/12/05

#include <stddef.h>     //for offsetof
	
bool i2c_write(int addr, uchar *data, int len,bool stop_flag);
bool i2c_read(int addr, uchar *data, int len0,bool restart_flag);
void initSetI2C(void);
void setup_i2c(void);
bool eeread(uchar *data, ushort offset, int nbytes, int iEEType);
bool eewrite(ushort offset, uchar *data, int nbytes, int iEEType);
short read_clock(time_t *nowtime);
bool set_clock(struct tm *tmt);
bool setup_rtc(void);
void write_dac6(uchar val);
bool get_i2c_in_use(void);
void SetVolume(unsigned char ucValue);
void ReadSN(char *sn);
void WriteSN(char *sn);

#define RTC_SLAVE_ADDRESS	    0xd0
#define EEPROM_SLAVE_ADDRESS    0xa0
#define DAC6_SLAVE_ADDRESS      0x60
#define IO_SLAVE_ADDRESS        0x40
#define EEPROM_MCA_SLAVE_ADDRESS    0xae
#define EEPROM_AUX_SLAVE_ADDRESS    0xac

enum{
	EE_CRC,
	EE_MCA,
	EE_AUX
};

//macro to get size of structure element
#define SIZEOF(type,id)   (((size_t)sizeof(((type *)1)->id)))
//macros to read and write to EEPROM using element name and source or destination
#define EE_READ(M,D) eeread(D,offsetof(EEVALS,M),SIZEOF(EEVALS,M),EE_CRC)
#define EE_WRITE(M,S) eewrite(offsetof(EEVALS,M),S,SIZEOF(EEVALS,M),EE_CRC)
