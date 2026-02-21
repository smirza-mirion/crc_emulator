/**
 * \file
 * \details This file contains functions, which calls the Coldfire Chip Select Module
 */

/***********************************************************************************
  MODULE:  Chip Select IO  for CRC-25R,ET

  FILE:    ChipSelectIO_RP.c

  DATE:    08/25/06

  All routines using the chip select are here

  Chip Select 1:
      0:  Write Data to LCD
      1:  Write command to LCD and Read Screen Data
      2:  Read from / Write to keyboard
      3:  System Control Register for
              Tone Generator, lcd backlight, mmc card, USB power
      4:  slave USB set register
      5:  slave USB read / write register
      6:  host USB set register
      7:  host USB read / write register
  Chip Select 2:
      0:  Selects chamber to communicate with via SPI
      2:  Control Register for 6 channel MCA
  Chip Select 3:

  
      
  *************************************************************************************/ 
#include "crc.h"
#include "coldfire.h"
#include "cs.h"
#include "qspi.h"
#include "pit.h"

#define  BASEADDRESS0  0xffc00000
#define  BASEADDRESS1  0xA0000000
#define  BASEADDRESS2  0xB0000000
#define  BASEADDRESS3  0xA0020000   //BASEADDRESS1 + 0x20000

static volatile ushort *pCS0;
volatile uchar *pCS1;
volatile uchar *pUSBSlaveAddress;
volatile uchar *pUSBSlaveData;
static volatile uchar *pCS2;
static volatile ushort *pCS3;

static void write_register(uchar reg);

ushort debugPulse(void){
	ushort returnvalue;
	returnvalue = *pCS0;
	return returnvalue;
}
/**
 * \details Initialize Chip Select registers on the Coldfire chip
 * \returns None
 */
void initSetCS(void){
	// Chip Select 0
	// Set high address
	cf.cs[0].csar = (BASEADDRESS0 >> 16);
	// Set 16 bit
	cf.cs[0].cscr = 0x3d80;
	// Set valid bit in mask register
	cf.cs[0].csmr = 0x00000001;
	pCS0 = (ushort *)BASEADDRESS0;


	// Chip Select 1
	// Set high address
	cf.cs[1].csar = (BASEADDRESS1 >> 16);
	// Set 8 bit
	cf.cs[1].cscr = 0x3c40;
	// Set valid bit in mask register
	cf.cs[1].csmr = 0x00000001;
	pCS1 = (uchar *)BASEADDRESS1;
	pUSBSlaveAddress = pCS1 + 4;
	pUSBSlaveData = pCS1 + 5;

	// Chip Select 2
	// Set high address
	cf.cs[2].csar = (BASEADDRESS2 >> 16);
	// Set 8 bit
	cf.cs[2].cscr = 0x3c40;
	// Set Valid bit in mask register
	cf.cs[2].csmr = 0x00000001;
	pCS2 = (uchar *)BASEADDRESS2;

	// Chip Select 3
	// Set high address
	cf.cs[3].csar = (BASEADDRESS3 >> 16);
	// Set 16 bit
	cf.cs[3].cscr = 0x3c80;
	// Set Valid bit in mask register
	cf.cs[3].csmr = 0x00000001;
	pCS3 = (ushort *)BASEADDRESS3;

	// Initialize sysem control register
	pCS1[3] = 0;
}

void init_chip_selects(void){
	//chip select 0
	//set to high address
	cf.cs[0].csar = (BASEADDRESS0 >> 16);

	//set for 16 bit
	cf.cs[0].cscr = 0x3c80;

	//set valid bit in mask register
	cf.cs[0].csmr = 0x00000001;
        
	//8-Bit IO for chip select 1
	cf.cs[1].csar = (BASEADDRESS1 >> 16);
	cf.cs[1].cscr = 0x3c40;
	cf.cs[1].csmr = 0x00000001;
	pCS1 = (uchar *)BASEADDRESS1;

	//8-Bit IO for chip select 2
	cf.cs[2].csar = (BASEADDRESS2 >> 16);
	cf.cs[2].cscr = 0x3c40;
	cf.cs[2].csmr = 0x00000001;
	pCS2 = (uchar *)BASEADDRESS2;

	//16-bit IO for chip select 3
	cf.cs[3].csar = (BASEADDRESS3 >> 16);
	cf.cs[3].cscr = 0x3c80;
	cf.cs[3].csmr = 0x00000001;
	pCS3 = (ushort *)BASEADDRESS3;

	//initialize sysem control register
	pCS1[3] = 0;
}

    //========================================================================
    //LCD Read / Write Routines

    //write command to LCD
    void write_lcd_cmd(uchar cmd)
    {
        pCS1[1] = cmd;
    }

    //write data to LCD
    void write_lcd_data(uchar data)
    {
        pCS1[0] = data;
    }

    //read data from lcd
    uchar read_lcd_data(void)
    {
        return *(pCS1 + 1);
    }    

    //=============================================================================

    //Keyboard Read / Write Routines

    //write to keyboard
    void write_keyboard(uchar data)
    {
        pCS1[2] = data;
    }

    //read from keyboard
    uchar read_keyboard(void)
    {
        return *(pCS1 + 2);
    }
    
    
    //==========================================================================================

    //Routines using Register at address 3, Chip Select 1

    //read register
/**
 * \details Read register at address 3, Chip Select 1
 * \returns Register contents
 */
    static uchar read_register(void)
    {
        uchar reg;

        reg = *(pCS1 + 3);
        return reg;
    }


/**
 * \details Write register at address 3, Chip Select 1
 * \param reg Data value to write
 * \returns None
 */
    static void write_register(uchar reg)
    {
        pCS1[3] = reg;
    }    

    //Tone generation uses bit 7
    
    //Start Tone Generatrion
/**
 * \details Start beep by setting bit 7 to 1 on address 3, Chip Select 1
 * \returns None
 */
    void start_tone(void)
    {
        uchar reg;

        //get current value of register
        reg = read_register();
        //set bit 7 to 1
        reg |= 0x80;

        //write to register
        write_register(reg);
        
    }

    //Stop Tone Generation
/**
 * \details Start beep by setting bit 7 to 0 on address 3, Chip Select 1
 * \returns None
 */
    void stop_tone(void)
    {
        uchar reg;

        //get current value of register
        reg = read_register();
        //set bit 7 to 0
        reg &= 0x7f;

        //write to register
        write_register(reg);
        
    }

    //set backlight -- uses bit 6
    void set_backlight(bool ON)
    {
        uchar reg;

        //get current value of register
        reg = read_register();

        if(ON)
        // set bit 6 to 1
            reg |= 0x40;
        else
        //set bit 6 to 0
            reg &= 0xbf;

        //write to register
        write_register(reg);

    }

    //mmc card uses bit 5

    //bit 5 is 1 if inserted, else is 0
/**
 * \details Read the status of the power to the SD card, read bit 5 on address 3, Chip Select 1
 * \returns True = Power on, False = Power off
 */
    bool mmc_in(void)
    {
        uchar reg;

        //read register
        reg = read_register();

        if((reg & 0x20) == 0)   //not in
            return FALSE;

        //in
        return TRUE;

    }    

/**
 * \details Set the power to the SD card, write to bit 5 on address 3, Chip Select 1
 * \param on True = Power on, False = Power off
 * \returns None
 */
    void power_mmc(bool on)
    {
        uchar reg;

        //get current value of register
        reg = read_register();

        if(on)
        // set bit 5 to 1
            reg |= 0x20;
        else
        //set bit 5 to 0
            reg &= 0xdf;

        //write to register
        write_register(reg);

    }

    //power for slave usb is bit 4
/**
 * \details Set the power to the USB Slave connector, write to bit 4 on address 3, Chip Select 1
 * \param on True = USB Power on, False = USB Power off
 * \returns None
 */
    void set_usb_power(bool on)
    {
        uchar reg;

        //get current value of register
        reg = read_register();

        if(on)
        // set bit 4 to 1
            reg |= 0x10;
        else
        //set bit 4 to 0
            reg &= 0xef;

        //write to register
        write_register(reg);

    }

    //power for host usb is bit 3
/**
 * \details Set the power to the USB Host connector, write to bit 3 on address 3, Chip Select 1
 * \param on True = USB Power on, False = USB Power off
 * \returns None
 */
    void set_host_usb_power(bool on)
    {
        uchar reg;

        //get current value of register
        reg = read_register();

        if(on)
        // set bit 3 to 1
            reg |= 0x08;
        else
        //set bit 3 to 0
            reg &= 0xf7;

        //write to register
        write_register(reg);

    }
/**
 * \details Control power to the Amulet Chip
 * \param on True = Power on, False = Power off
 * \returns None
 */
void set_touch_power(bool on){
	uchar reg;

	reg = read_register();

	if(on) reg |= 0x04;
	else reg &= 0xfb;
	pCS1[9] = 0xA5;
	write_register(reg);
}
/**
 * \details Control the reset line to the Amulet Chip
 * \param on True = Enable reset, False = Disable reset
 * \returns None
 */
void set_touch_reset(bool on){
	uchar reg;

	reg = read_register();

	if(on) reg |= 0x02;
	else reg &= 0xfd;
	pCS1[9] = 0xA5;
	write_register(reg);
}
/**
 * \details Control the programming line to the Amulet Chip
 * \param on True = Enable programming, False = Disable programming
 * \returns None
 */
void set_touch_program(bool on){
	uchar reg;

	reg = read_register();

	if(on) reg |= 0x01;
	else reg &= 0xfe;
	pCS1[9] = 0xA5;
	write_register(reg);
}

//============================================================================
    //Selection of chamber to communicate with via SPI
    //selects ADC or EEPROM or REMOTE in the chamber
/**
 * \details Select chamber and device within the chamber to communicate
 * \param ch_num Chamber number
 * \param mode 0 = EEPROM, 1 = ADC, 2 = Remote
 * \returns None
 */
    void set_chamber_spi(short ch_num, short mode)
    {
        uchar wr_byte;

        wr_byte = (uchar)(ch_num);

        switch(mode)
        {
        case QSPI_ADC:
            break;
        case QSPI_EEPROM:    
            wr_byte |= 0x40;
            break;
        case QSPI_REMOTE:    
            wr_byte |= 0x80;
            break;
        }        

        pCS2[0] = wr_byte;

    }

//-----------------------------------------------------------------------------
//  USB communications
//  development in CS1 -- final version in CS0
//-----------------------------------------------------
#define USB_OFF_SET  4      //set register address
#define USB_OFF_RW   5     // reads from /writes to register
    
    //set the usb register address
/**
 * \details Set SL811 Slave USB register
 * \param data Register number
 * \returns None
 */
    void set_usb_register(uchar data)
    {
        pCS1[USB_OFF_SET] = data;
    }

    //read from usb register
/**
 * \details Read data from SL811 Slave USB register
 * \returns Data from SL811 register
 */
    uchar read_usb_register(void)
    {
        return *(pCS1 + USB_OFF_RW);
    }

    //write to the usb register
/**
 * \details Write data to SL811 Slave USB register
 * \param data Data to write to register
 * \returns None
 */
    void write_usb_register(uchar data)
    {    

        pCS1[USB_OFF_RW] = data;
    
    }

    

    



    //read CS2[0] for testing    
    uchar read_cs2(void)
    {
        uchar reg;

        reg = *(pCS2);
        return reg;
    }

    
    //Routines using Interrupt Register at address 8, Chip Select 1

    //read register
/**
 * \details Read interrupt register
 * \returns Interrupt line status
 */
    static uchar read_int_register(void)
    {
        uchar reg;

        reg = *(pCS1 + 8);
        return reg;
    }

/**
 * \details Test if PC is connected to the USB
 * \returns True = PC connected, False = PC not connected
 */
    bool usb_pc_connected(void)
    {

        uchar data;

        data = read_int_register();
        if((data & 0x1) == 0)
            return TRUE;
        return FALSE;

    }
/**
 * \details Test if the RTC battery is low
 * \returns True = Battery low, False = Battery OK
 */
    bool battery_low(void)
    {

        uchar data;

        data = read_int_register();
        if((data & 0x40) == 0)
            return FALSE;
        return TRUE;


    }    

//read mca port
ushort read_mca_port(short offset){
	ushort value;

	value = *(pCS3 + offset / 2);
	return(value);
}

//write to mca port
void write_mca_port(ushort offset, ushort value){
	pCS3[offset / 2] = value;
}
