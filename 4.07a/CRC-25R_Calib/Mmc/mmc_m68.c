/**
 * \file
 * \details This file contains functions, which calls Coldfire SPI Module and implements communications with the SD Card.
 */
#ifndef _MMC_C_
#define _MMC_C_

#include <string.h>
/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 * 05/04/2004: Ported to Capintec CCU Environment
 * 03/15/05 - reduced timeout for no card
 * 06/15/05 - added serviceing watchdog
 ***************************************************************************/

#include "coldfire.h"
#include "crc.h"
#include "keyboard.h"
#include "qspi.h"
#include "diskio.h"
#include "pit.h"
//#include "mmc_m68.h"
//#include "mmc.h"

extern time_t clock_time;
extern uchar m_ucEnumerated;
extern unsigned long int m_ulLastLogicalBlock;
extern unsigned long int m_ulLogicalBlockSize;
extern int (*spi_write_function)(unsigned long *data, unsigned long sector);
extern int (*spi_read_function) (unsigned long *data, unsigned long sector);

static int spiSDinit(void);
/****************************************************************************
 *
 * port defintions
 *
 ***************************************************************************/
#define fnPr iprintf
#define CLKIN SYSCLK

/****************************** SPI *************************************/

/***************************** PORT *************************************/
//#define PORTB_DIR cf.gpio.ddrb
//#define PORTB_DATA cf.gpio.portbp
//#define PORTB_ASSIGN cf.gpio.pbcdpar
//#define PORTQ_ASSIGN cf.gpio.pqspar
//#define SPIPORT_DIR PORTB_DIR
//#define SPIPORT PORTB_DATA
//#define SPIPORT_ASSIGN PORTQ_ASSIGN

#define SPIWP  (1<<0)  //write protect
#define SPICD  (1<<1)  //card detect


/****************************************************************************
 *
 * SD card info type structure specification
 *
 ***************************************************************************/
typedef struct{
	long initok;

	long CSD_STR;
	long SPEC_VERS;

	long TAAC;
	long NSAC;
	long TRANSPEED;
	long CCC;
	long R_BL_LEN;
	long R_BL_PART;
	long WR_BL_MIS;
	long RD_BL_MIS;
	long DSR_IMP;

	long CSIZE;

	long RC_MIN;
	long RC_MAX;
	long WC_MAX;
	long WC_MIN;
	long C_SIZE_M;
	long ERASE_G_S;
	long ERASE_G_M;
	long WP_G_S;
	long WP_G_EN;
	long DEF_ECC;

	long ERASE_BLK_EN;
	long ERASE_SECTOR_SIZE;
	long WP_GRP_SIZE;
	long WP_GRP_ENABLE;

	long R2W_FACT;
	long WR_BL_LEN;
	long WR_BL_PAR;

	long C_PR_APP;
	long F_FORMATG;
	long COPY;
	long P_WR_PROT;
	long T_WR_PROT;
	long F_FORMAT;
	long ECC;

	long CRC7;
} MMC_TYPE;

MMC_TYPE g_mmc_type;

unsigned long CRCtbl[ 256 ];
/**
 * \details Calculate the CRC16 value for a single byte
 * \param crc Initial CRC16 Value
 * \param c Data byte
 * \returns Calculated CRC16 value
 */
static unsigned long crc16( unsigned long crc, unsigned char c){
	long a;

	crc = crc ^ ((unsigned long) c << 8UL);
	for(a=0; a<8; a++){
		if(crc & 0x8000) crc = (crc << 1) ^ 0x1021;
		else crc = crc << 1;
	}
	return crc;
}

unsigned long rxdata=0;
unsigned long bitcou=0;
bool flgHC;

// Retrieve Receive Data and copy into rxdata
/**
 * \details Retrieve Receive Data and copy into rxdata
 * \returns None
 */
static void spirx(void){
	register int i;
	for(i=0,rxdata=0,cf.qspi.qar=0x10;i<=((cf.qspi.qwr&0xf0)>>4);rxdata|=((cf.qspi.qdr&0xff)<<((3-i++)<<3)));
	bitcou=i<<3;
	return;
}

// Send Byte
/**
 * \details Send Byte
 * \param ch Data byte to send
 * \returns None
 */
static void spitxbyte(unsigned char ch){
	cf.qspi.qar=0;				// Set start address to the base of Transmit RAM
	cf.qspi.qdr=ch;				// Copy byte to base of Transmit RAM
	cf.qspi.qwr=0x1000;			// Set CS to inactive high, Start Address = 0, End Address = 0
	cf.qspi.qir|=1;				// Reset Finish Flag
	cf.qspi.qdlyr|=0x8000;		// Enable QSPI
	while((cf.qspi.qir&1)==0) service_watchdog();	// Loop until transfer finishes
}

// Send Word
/**
 * \details Send Word
 * \param data16 Data word to send
 * \returns None
 */
static void spitxword(unsigned short data16){
	unsigned char *bptr;

	bptr = (unsigned char *) &data16;
	cf.qspi.qar=0;				// Set start address to the base of Transmit RAM
	cf.qspi.qdr = *bptr++;
	cf.qspi.qdr = *bptr;
	cf.qspi.qwr=0x1100;			// Set CS to inactive high, Start Address = 0, End Address = 1
	cf.qspi.qir|=1;				// Reset Finish Flag
	cf.qspi.qdlyr|=0x8000;		// Enable QSPI
	while((cf.qspi.qir&1)==0) service_watchdog();	// Loop until transfer finishes
}

// Send Long Word
/**
 * \details Send Long Word
 * \param data32 Data long to send
 * \returns None
 */
static void spitxlong(unsigned long data32){
	unsigned char *bptr;

	bptr = (unsigned char *) &data32;
	cf.qspi.qar=0;					// Set start address to the base of Transmit RAM
	cf.qspi.qdr = *bptr++;
	cf.qspi.qdr = *bptr++;
	cf.qspi.qdr = *bptr++;
	cf.qspi.qdr = *bptr;
	cf.qspi.qwr=0x1300;				// Set CS to inactive high, Start Address = 0, End Address = 3
	cf.qspi.qir|=1;					// Reset Finish Flag
	cf.qspi.qdlyr|=0x8000;			// Enable QSPI
	while((cf.qspi.qir&1)==0) service_watchdog();		// Loop until transfer finishes
}
/**
 * \details Get a bit value from the SPI connection
 * \returns Bit value
 */
static long spigetbit(void){
	if(!bitcou){
		spitxbyte(0xff);
		spirx();
	}

	if(bitcou){
		bitcou--;
		if(rxdata&0x80000000){
			rxdata<<=1;
			return 1;
		}
		rxdata<<=1;
		return 0;
	}
	else return 1;
}
/**
 * \details Set SPI Start
 * \returns None
 */
static void spistart(void){
	cf.qspi.qir |= 1;
	cf.qspi.qdlyr |= 0x8000;
	while((cf.qspi.qir&1)==0) service_watchdog();
}

/****************************************************************************
 *
 * spiInit
 *
 * Init SPI ports, directions
 *
 ***************************************************************************/
/**
 * \details Initialize Coldfire SPI registers
 * \returns None
 */
static void spiInit(void ){
	long a;

	for(a=0; a<256; a++){
		CRCtbl[a]=crc16(0, (unsigned char)a );
	}
	memset(&g_mmc_type, 0, sizeof(MMC_TYPE));
	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_SDCARD)) service_watchdog();
	spiSDinit();
	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_SDCARD)) service_watchdog();
}

/****************************************************************************
 *
 * spiDataBits
 *
 * Receive any bits on spi and calculates crc16 on incoming data
 *
 * INPUTS
 *
 * bits - how many bits need to be received
 * pcrc - pointer to crc
 *
 * RETURNS
 *
 * received any bits data
 *
 ***************************************************************************/
/**
 * \details Receive any bits on spi and calculates crc16 on incoming data
 * \param bits How many bits need to be received
 * \param pcrc Pointer to crc
 * \returns Received any bits data
 */
static long spiDataBits(long bits, unsigned long *pcrc){
	long a;
	long data=0;
	unsigned long crc=*pcrc;
	for(a=0; a<bits; a++){
		data<<=1;

		if(spigetbit()){
			crc ^= 0x8000;
			data |= 1;
		}
		if(crc & 0x8000) crc = (crc << 1) ^ 0x1021;
		else crc = crc << 1;
	}
	*pcrc=crc;
	return data;
}

/****************************************************************************
 *
 * spiWaitStartBit
 *
 * Wait data start bit on spi and send one more clock
 *
 * RETURNS
 *
 * 0 - if received
 * other if time out
 *
 ***************************************************************************/
/**
 * \details Wait data start bit on spi and send one more clock
 * \returns 0 = Received, Other = Timed out
 */
static int spiWaitStartBit(void) {
    long a= 50000;	//500000;
    bitcou=0;
    while(a--){
    	service_watchdog();
    	if(!spigetbit()) return 0;
    }
    return 1;
}

/****************************************************************************
 *
 * spiWaitBusy
 *
 * Wait until SPI datain line goes hi
 *
 * RETURNS
 *
 * 0 - if ok
 * other if time out
 *
 ***************************************************************************/
/**
 * \details Wait until SPI data in line goes hi
 * \returns 0 = OK, Other = Timed out
 */
static int spiWaitBusy(void){
	long a=50000;	//500000;
	while(--a){
		service_watchdog();
		if(spigetbit()) return 0;
	}
	return 1;
}

/****************************************************************************
 *
 * spiCmd
 *
 * Send command to SPI, it adds startbit and stopbit and calculates crc7 also
 * sent, and Wait response start bit on spi and r1 response
 *
 * INPUTS
 *
 * cmd - command to send
 * data - argument of the command
 *
 * RETURNS
 *
 * r1 if received or 0xff if any error
 *
 ***************************************************************************/
/**
 * \details Send command to SPI, it adds startbit and stopbit and calculates crc7 also sent, and Wait response start bit on spi and r1 response
 * \param cmd Command to send
 * \param data Argument of the Command
 * \param highest_response_bit 7 = Receive byte, Other = Receive 4 byte
 * \returns Received data
 */
static long long spiCmd(long cmd, long data, int highest_response_bit){
	register long a;
	register long crc7 = 0;
	unsigned char *bptr;
	long long result;

	cf.qspi.qar = 0;
	cf.qspi.qdr = 0xff;
	cf.qspi.qdr = 0xff;
	cf.qspi.qdr = 0xff;
	cmd = (cmd & 0xff) | 0x40;
	cf.qspi.qdr = cmd;
	bptr = (unsigned char *) &data;
	cf.qspi.qdr = *bptr++;
	cf.qspi.qdr = *bptr++;
	cf.qspi.qdr = *bptr++;
	cf.qspi.qdr = *bptr;

	// Calculate CRC for cmd
	for(a=0; a<8; a++){
		if(cmd & 0x80) crc7 ^= 0x40;
		cmd <<= 1;

		if(crc7 & 0x40) crc7 = (crc7 << 1) ^ 0x09;
		else crc7 = crc7 << 1;
	}

	// Calculate CRC for data
	for(a=0; a<32; a++){
		if(data & 0x80000000) crc7 ^= 0x40;
		data <<= 1;

		if( crc7 & 0x40 ) crc7 = (crc7 << 1) ^ 0x09;
		else crc7 = crc7 << 1;
	}
	crc7 <<= 1;  //set crc7 into correct position
	crc7 |= 1;   //add endbit

	cf.qspi.qdr = (unsigned char ) crc7;
	cf.qspi.qwr = 0x1800;
	cf.qspi.qir |= 1;
	cf.qspi.qdlyr |= 0x8000;
	while((cf.qspi.qir&1) == 0) service_watchdog();

	result = 0;
	if(spiWaitStartBit() == 0){
		for (a=0; a<highest_response_bit; a++){
			result <<= 1;
			result |= spigetbit();
		}
		if(highest_response_bit == 7) result &= 0xFF;
		return result;
	}
	if(highest_response_bit == 7) return 0xFF;
	else return 0xffffffffffffffff;
}

/****************************************************************************
 *
 * spiGetDataBlock
 *
 * Getting a datablock from SD
 *
 * INPUTS
 *
 * dwaddr - long pointer where to store received data (512bytes)
 * addr - sector where to start read card, it has to be sector aligned
 *
 * RETURNS
 *
 * 0 - if successful
 * other if any error (crc,timeouts)
 *
 ***************************************************************************/
/**
 * \details Getting a datablock from SD
 * \param dwaddr Long pointer where to store received data (512bytes)
 * \param sector Sector where to start read card, it has to be sector aligned
 * \returns 0 = OK, 1 = Command Error, 2 = Timeout, 3 = CRC Error
 */
int spiGetDataBlock(unsigned long *dwaddr, unsigned long sector){
	unsigned char *bptr;
	unsigned short int *wptr;
	register unsigned long crc;
	unsigned short int crcvalue;

	wptr = (unsigned short int *) dwaddr;
	bptr = (unsigned char *) dwaddr;
	if(!g_mmc_type.initok) return 17;	//card is not initialized

	//if((SPIPORT) & SPICD) return 18;	//card is missing
	//if (!mmc_present) return 18;

	if(!flgHC) sector *= 512;

	if(spiCmd(17, sector, 7)){
		g_mmc_type.initok = 0;
		return 1;
	}

	if(spiWaitStartBit()){
		return 2;
	}

	crc=0;
	{
		register long a, b;
		register unsigned long *crctable = CRCtbl;

		cf.qspi.qmr &= 0xdfff;		// Set 16 bits transfer size
		cf.qspi.qar = 0x00;
		for(a=0; a<16; a++) cf.qspi.qdr = 0xffff;
		cf.qspi.qwr = 0x1f00;

		for(a=0; a<16; a++){
			spistart();
			cf.qspi.qar = 0x10;
			for(b=0; b<16; b++) *wptr++ = cf.qspi.qdr;
		}

		cf.qspi.qwr = 0x1000;
		spistart();
		cf.qspi.qar = 0x10;
		crcvalue = cf.qspi.qdr;
		cf.qspi.qmr |= 0x2000;		// Set 8 bit transfer size

		for(a=0; a<512; a++){
			crc = (crc << 8UL) ^ crctable[((crc >> 8UL) ^ (*bptr++)) & 0xff];
		}
		bptr = (unsigned char *) &crcvalue;
		crc = (crc << 8UL) ^ crctable[((crc >> 8UL) ^ (*bptr++)) & 0xff];
		crc = (crc << 8UL) ^ crctable[((crc >> 8UL) ^ (*bptr)) & 0xff];
		crc &= 0xffff;
	}

	if(crc){
		return 3; //crc error
	}
	return 0;
}
/**
 * \details Read 32 bit value from SPI
 * \returns Data value
 */
long spigetbit32(void){
	long ret=rxdata;
	long oldbitcou=bitcou;

      cf.qspi.qar=0;
      cf.qspi.qdr=0xffff;
      cf.qspi.qdr=0xffff;
      cf.qspi.qdr=0xffff;
      cf.qspi.qdr=0xffff;
      //cf.qspi.qmr |= 0x2000;		// Set 8 bit transfer size
      cf.qspi.qwr=0x1300;
      cf.qspi.qir|=1;
//   cf.qspi.qdlyr=0x8000;
   cf.qspi.qdlyr|=0x8000;
      while((cf.qspi.qir&1)==0);
   spirx();

   ret|=rxdata>>oldbitcou;
   rxdata<<=32-oldbitcou;
   bitcou=oldbitcou;
   return ret;
}

/**
 * \details Getting a datablock from SD with slow routine, read one bit at a time
 * \param dwaddr Long pointer where to store received data (512bytes)
 * \param sector Sector where to start read card, it has to be sector aligned
 * \returns 0 = OK, 1 = Command Error, 2 = Timeout, 3 = CRC Error
 */
int spiGetDataBlockSlow(unsigned long *dwaddr, unsigned long sector) {
register unsigned long crc;
   if (!g_mmc_type.initok) { return 17; }//card is not initialized

   if(!flgHC) sector *= 512;
   //if ((SPIPORT) & SPICD) { return 18; }//card is missing
   //if (!mmc_present) return 18;

   //spiCSlow();
   if (spiCmd(17, sector, 7)) {
      //spiCShi();
      g_mmc_type.initok=0;
      return 1;
   }

   if (spiWaitStartBit()) {
      //spiCShi();
      return 2;
   }

   crc=0;
   {
      register long a;
      register unsigned long *crctable=CRCtbl;
      for (a=0; a<128; a++) {  //get data 128long 512bytes
         register unsigned long data=spigetbit32();

         *dwaddr++=data;

         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>24)) & 0x0FFUL  ];
         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>16)) & 0x0FFUL  ];
         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>8)) & 0x0FFUL  ];
         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ data) & 0x0FFUL  ];
      }

      for (a=0; a<16; a++) {   //check crc in next two bytes
         if ( spigetbit() ) {
            crc^=0x8000;
         }

         if( crc & 0x8000 ) crc = (crc << 1) ^ 0x1021;
         else crc = crc << 1;
      }
   }

   //spiCShi();
   if (crc)
   {
     return 3; //crc error
   }

   return 0;
}

/****************************************************************************
 *
 * spiSetDataBlock
 *
 * Write data into SD
 *
 * INPUTS
 *
 * dwaddr - long pointer where original data is (512bytes)
 * addr - where to store in the card (address has to be sector aligned)
 *
 * RETURNS
 *
 * 0 - if successful
 * other if any error
 *
 ***************************************************************************/
/**
 * \details Write data into SD
 * \param dwaddr Long pointer where original data is (512bytes)
 * \param sector Where to store in the card (address has to be sector aligned)
 * \returns 0 = Success, Other = Error
 */
int spiSetDataBlock(unsigned long *dwaddr, unsigned long sector){
	register unsigned long crc;
	register long a, b;
	register unsigned long *crctable = CRCtbl;
	unsigned char *bptr;
	unsigned short int *wptr;

	if (!g_mmc_type.initok) return 17;//card is not initialized

//	if ((SPIPORT) & SPICD) return 18;//card is missing
//  if ((SPIPORT) & SPIWP) return 4; //write protected!!
	//if (!mmc_present) return 18;
//  NOTE: CLIVE:  No Way to detect Write Protect Yet!!

	// Calculate CRC

	if(!flgHC) sector *= 512;

	crc = 0;
	bptr = (unsigned char *) dwaddr;
	for(a=0; a<512; a++){
		crc = (crc << 8UL) ^ crctable[((crc >> 8UL) ^ (*bptr++)) & 0xff];
	}

	if(spiCmd(24, sector, 7)){
		g_mmc_type.initok=0;
		return 13;
	}

	spitxlong(0xfffffffe); //startbit
	{
		register unsigned long data;

		wptr = (unsigned short int *) dwaddr;

		cf.qspi.qmr &= 0xdfff;
		cf.qspi.qwr = 0x1f00;

		for(a=0; a<16; a++){ //sends 16 x 32 byte blocks
			cf.qspi.qar = 0x00;
			for(b=0; b<16; b++) cf.qspi.qdr = *wptr++;
			spistart();
		}

		cf.qspi.qwr = 0x1000;
		cf.qspi.qar = 0x00;
		cf.qspi.qdr = (unsigned short int) crc;
		spistart();

		cf.qspi.qmr |= 0x2000;

		if(spiWaitStartBit()){
			return 1;
		}

		data = 0;
		for(a=0; a<4; a++){   //get data ack
			data <<= 1;
			if(spigetbit()) data |= 1;
		}

		data &= 0x1f; //keep valid bits

		if(data != 0x05){ //read accepted?
			return (int)((data<<8)+1);
		}
	}

	if(spiWaitBusy()){
		return 8;
	}
	return 0;
}
/**
 * \details Write data into SD with slow routine, writes one bit at a time
 * \param dwaddr Long pointer where original data is (512bytes)
 * \param sector Where to store in the card (address has to be sector aligned)
 * \returns 0 = Success, Other = Error
 */
int spiSetDataBlockSlow(unsigned long *dwaddr, unsigned long sector){
	register unsigned long crc;

	if (!g_mmc_type.initok) return 17;//card is not initialized

	if(!flgHC) sector *= 512;

	//if ((SPIPORT) & SPICD) return 18;//card is missing
	//if ((SPIPORT) & SPIWP) return 4; //write protected!!
	//if (!mmc_present) return 18;

// NOTE: CLIVE:  No Way to detect Write Protect Yet!!

	if(spiCmd(24, sector, 7)){
		//spiCShi();
		g_mmc_type.initok=0;
		return 13;
	}

	spitxlong(0xfffffffe); //startbit
	crc=0;
	{
		register long a;
		register unsigned long *crctable=CRCtbl;
		register unsigned long data;

		for(a=0; a<128; a++){ //sends 128long 512bytes
			data=*dwaddr++;

			spitxlong(data);

			crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>24)) & 0x0FFUL  ];
			crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>16)) & 0x0FFUL  ];
			crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>8)) & 0x0FFUL  ];
			crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ data) & 0x0FFUL  ];
		}

		spitxword((unsigned short)crc);

		if(spiWaitStartBit()) {
			return 1;
		}

		data=0;
		for(a=0; a<4; a++){   //get data ack
			data<<=1;
			if(spigetbit()) data|=1;
		}

		data &= 0x1f; //keep valid bits

		if(data != 0x05) { //read accepted?
			return (int)((data<<8)+1);
		}
	}

	if(spiWaitBusy()){
		return 8;
	}
	return 0;
}


/****************************************************************************
 *
 * spiCSDBlock
 *
 * Get Card Specific Data
 *
 * RETURNS
 *
 * 0 - if successful
 * other if any error
 *
 ***************************************************************************/
/**
 * \details Get Card Specific Data
 * \returns 0 = OK, Other = Error
 */
static int spiCSDBlock(void){
	unsigned long crc;
	unsigned long time;

	if(spiCmd(9, 0, 7)) return 2; //r1 any error

	if(spiWaitStartBit()) return 3;

	crc = 0;

	g_mmc_type.CSD_STR		= spiDataBits(2, &crc); // CSD Version: 0 = CSD Ver 1.0, 1 = CSD Ver 2.0
	if(g_mmc_type.CSD_STR == 0){
		spiDataBits(6, &crc);

		g_mmc_type.TAAC			=spiDataBits(8, &crc); // Async data access time: (2-0: 0=1ns, 1=10ns, 2=100ns, 3=1us, 4=10us, 5=100us, 6=1ms, 7=10ms); (6-3: 0=reserved, 1=1.0, 2=1.2, 3=1.3, 4=1.5, 5=2.0, 6=2.5, 7=3.0, 8=3.5, 9=4.0, 10=4.5, 11=5.0, 12=5.5, 13=6.0, 14=7.0, 15=8.0);(7:reserved)
		g_mmc_type.NSAC			=spiDataBits(8, &crc); // Worst Case Data Access Time
		g_mmc_type.TRANSPEED	=spiDataBits(8, &crc); // Maximum data transfer rate: (2-0: 0=100kbit/s, 1=1Mbit/s, 2=10Mbit/s, 3=100MBit/s);(6-3: 0=reserved, 1=1.0, 2=1.2, 3=1.3, 4=1.5, 5=2.0, 6=2.5, 7=3.0, 8=3.5, 9=4.0, 10=4.5, 11=5.0, 12=5.5, 13=6.0, 14=7.0, 15=8.0);(7:reserved)
		g_mmc_type.CCC			=spiDataBits(12,&crc); // Card Command Class
		g_mmc_type.R_BL_LEN		=spiDataBits(4, &crc); // Maximum Read Data Block Length
		g_mmc_type.R_BL_PART	=spiDataBits(1, &crc); // Partial Block Read Enabled
		g_mmc_type.WR_BL_MIS	=spiDataBits(1, &crc); // 0=Forbids writes across physical block boundaries, 1=Accepts writes across physical block boundaries
		g_mmc_type.RD_BL_MIS	=spiDataBits(1, &crc); // 0=Forbids reads across physical block boundaries, 1=Accepts reads across physical block boundaries
		g_mmc_type.DSR_IMP		=spiDataBits(1, &crc); // 0=No Driver Stage Register, 1=Driver Stage Register implemented
		spiDataBits(2, &crc); //reserved

		g_mmc_type.CSIZE		=spiDataBits(12,&crc); // Size = (CSIZE+1) * (2^(C_SIZE_M+2)) * (2^R_BL_LEN)
		g_mmc_type.RC_MIN		=spiDataBits(3, &crc); // Max Read Current at min Voltage
		g_mmc_type.RC_MAX		=spiDataBits(3, &crc); // Max Read Current at max Voltage
		g_mmc_type.WC_MIN		=spiDataBits(3, &crc); // Max Write Current at min Voltage
		g_mmc_type.WC_MAX		=spiDataBits(3, &crc); // Max Write Current at max Voltage
		g_mmc_type.C_SIZE_M		=spiDataBits(3, &crc); // Size = (CSIZE+1) * (2^(C_SIZE_M+2)) * (2^R_BL_LEN)
		g_mmc_type.ERASE_BLK_EN	=spiDataBits(1, &crc); // 0=Erase block according to byte address, 1=Erase block according to 512 byte sector
		g_mmc_type.ERASE_SECTOR_SIZE = spiDataBits(7, &crc); // Size of erasable sector, 0=1block, 1=2block, etc.
		g_mmc_type.WP_GRP_SIZE	=spiDataBits(7, &crc); //Size of write protect group, 0=1 erase sector, 1=2 erase sector
		g_mmc_type.WP_GRP_ENABLE=spiDataBits(1, &crc); // 0=No Group Write Protection, 1=Group Write Protection
		spiDataBits(2, &crc); //reserved

		g_mmc_type.R2W_FACT		=spiDataBits(3, &crc); // Write Time vs Read Time, 0 = 1x, 1 = 2x, 2 = 4x, 3 = 8x, 4 = 16x, 5 = 32x
		g_mmc_type.WR_BL_LEN	=spiDataBits(4, &crc); // Max Write Block Length, 9=512 bytes, 10=1024 bytes, 11=2048 bytes
		g_mmc_type.WR_BL_PAR	=spiDataBits(1, &crc); // 0=Valid Blocks in units of 512 to WR_BL_LEN, 1=Min block size can be one byte
		spiDataBits(5, &crc); //reserved

		g_mmc_type.F_FORMATG	=spiDataBits(1, &crc); // 0=See F_FORMAT below, 1=reserved
		g_mmc_type.COPY			=spiDataBits(1, &crc); // 0=Original, 1=Copied
		g_mmc_type.P_WR_PROT	=spiDataBits(1, &crc); // 1=Permanently protects the entire card
		g_mmc_type.T_WR_PROT	=spiDataBits(1, &crc); // 1=Temporarily protects the entire card
		g_mmc_type.F_FORMAT		=spiDataBits(2, &crc); // 0=Hard disk-like file system, 1=DOS FAT(floppy like), 2=Universal File Format, 3=Others/Unknown
		spiDataBits(2, &crc); //reserved

		g_mmc_type.CRC7			=spiDataBits(7, &crc);
		spiDataBits(1, &crc); //reserved

		spiDataBits(16, &crc); //crc
	}else if(g_mmc_type.CSD_STR == 1){
		spiDataBits(6, &crc);

		g_mmc_type.TAAC			=spiDataBits(8, &crc);
		g_mmc_type.NSAC			=spiDataBits(8, &crc);
		g_mmc_type.TRANSPEED	=spiDataBits(8, &crc);
		g_mmc_type.CCC			=spiDataBits(12, &crc);
		g_mmc_type.R_BL_LEN		=spiDataBits(4, &crc);
		g_mmc_type.R_BL_PART	=spiDataBits(1, &crc);
		g_mmc_type.WR_BL_MIS	=spiDataBits(1, &crc);
		g_mmc_type.RD_BL_MIS	=spiDataBits(1, &crc);
		g_mmc_type.DSR_IMP		=spiDataBits(1, &crc);
		spiDataBits(6, &crc);

		g_mmc_type.CSIZE		=spiDataBits(22, &crc);
		spiDataBits(1, &crc);

		g_mmc_type.ERASE_BLK_EN	=spiDataBits(1, &crc);
		g_mmc_type.ERASE_SECTOR_SIZE =spiDataBits(7, &crc);
		g_mmc_type.WP_GRP_SIZE	=spiDataBits(7, &crc);
		g_mmc_type.WP_GRP_ENABLE=spiDataBits(1, &crc);
		spiDataBits(2, &crc);

		g_mmc_type.R2W_FACT		=spiDataBits(3, &crc);
		g_mmc_type.WR_BL_LEN	=spiDataBits(4, &crc);
		g_mmc_type.WR_BL_PAR	=spiDataBits(1, &crc);
		spiDataBits(5, &crc);

		g_mmc_type.F_FORMATG	=spiDataBits(1, &crc);
		g_mmc_type.COPY			=spiDataBits(1, &crc);
		g_mmc_type.P_WR_PROT	=spiDataBits(1, &crc);
		g_mmc_type.T_WR_PROT	=spiDataBits(1, &crc);
		g_mmc_type.F_FORMAT		=spiDataBits(2, &crc);
		spiDataBits(2, &crc);

		g_mmc_type.CRC7			=spiDataBits(7, &crc);
		spiDataBits(1, &crc);

		spiDataBits(16, &crc); //crc
	}else{
		g_mmc_type.SPEC_VERS=spiDataBits(4,&crc);
		spiDataBits(2,&crc); //reserved

		g_mmc_type.TAAC     =spiDataBits(8,&crc);
		g_mmc_type.NSAC     =spiDataBits(8,&crc);
		g_mmc_type.TRANSPEED=spiDataBits(8,&crc);
		g_mmc_type.CCC      =spiDataBits(12,&crc);
		g_mmc_type.R_BL_LEN =spiDataBits(4,&crc);
		g_mmc_type.R_BL_PART=spiDataBits(1,&crc);
		g_mmc_type.WR_BL_MIS=spiDataBits(1,&crc);
		g_mmc_type.RD_BL_MIS=spiDataBits(1,&crc);
		g_mmc_type.DSR_IMP  =spiDataBits(1,&crc);
		spiDataBits(2,&crc); //reserved

		g_mmc_type.CSIZE    =spiDataBits(12,&crc);
		g_mmc_type.RC_MIN   =spiDataBits(3,&crc);
		g_mmc_type.RC_MAX   =spiDataBits(3,&crc);
		g_mmc_type.WC_MAX   =spiDataBits(3,&crc);
		g_mmc_type.WC_MIN   =spiDataBits(3,&crc);
		g_mmc_type.C_SIZE_M =spiDataBits(3,&crc);
		g_mmc_type.ERASE_G_S=spiDataBits(5,&crc);
		g_mmc_type.ERASE_G_M=spiDataBits(5,&crc);
		g_mmc_type.WP_G_S   =spiDataBits(5,&crc);
		g_mmc_type.WP_G_EN  =spiDataBits(1,&crc);
		g_mmc_type.DEF_ECC  =spiDataBits(2,&crc);
		g_mmc_type.R2W_FACT =spiDataBits(3,&crc);
		g_mmc_type.WR_BL_LEN=spiDataBits(4,&crc);
		g_mmc_type.WR_BL_PAR=spiDataBits(1,&crc);
		spiDataBits(4,&crc); //reserved

		g_mmc_type.C_PR_APP =spiDataBits(1,&crc);
		g_mmc_type.F_FORMATG=spiDataBits(1,&crc);
		g_mmc_type.COPY     =spiDataBits(1,&crc);
		g_mmc_type.P_WR_PROT=spiDataBits(1,&crc);
		g_mmc_type.T_WR_PROT=spiDataBits(1,&crc);
		g_mmc_type.F_FORMAT =spiDataBits(2,&crc);
		g_mmc_type.ECC      =spiDataBits(2,&crc);
		g_mmc_type.CRC7     =spiDataBits(7,&crc);
		spiDataBits(1,&crc); //reserved

		spiDataBits(16,&crc); //crc
	}

	if(crc) return 10;

	switch(g_mmc_type.TRANSPEED & 7){
		case 0: time=   1; break; //100K
		case 1: time=  10; break; //1M
		case 2: time= 100; break; //10M
		case 3: time=1000; break;  //100M
		default: return 0;
	}

	switch((g_mmc_type.TRANSPEED >> 3) &0x0f){
		case  0: return 0;
		case  1: time*=10; break; //1.0
		case  2: time*=12; break; //1.2
		case  3: time*=13; break; //1.3
		case  4: time*=15; break; //1.5
		case  5: time*=20; break; //2.0
		case  6: time*=25; break; //2.5
		case  7: time*=30; break; //3.0
		case  8: time*=35; break; //3.5
		case  9: time*=40; break; //4.0
		case 10: time*=45; break; //4.5
		case 11: time*=50; break; //5.0
		case 12: time*=55; break; //5.5
		case 13: time*=60; break; //6.0
		case 14: time*=70; break; //7.0
		case 15: time*=80; break; //8.0
	}

	time <<= 1;
	time = ((CLKIN/10000) + (time - 1)) / time;
	if(time < 2) time = 2;
	if(time > 0xff) time = 0xff;
	time &= 0xff;

	cf.qspi.qmr &= 0xff00;
	cf.qspi.qmr |= time;
	spiSetBaud(SPI_SEMA_SDCARD, time);
   return 0;
}

/****************************************************************************
 *
 * spiSDinit
 *
 * inti Card protocols via spi
 *
 * RETURNS
 *
 * 0 - if successful
 * other if any error
 *
 ***************************************************************************/
/**
 * \details Initialize SD Card
 * \returns 0 = OK, Other = Error
 */
static int spiSDinit(void){
	long a;
	long long b;
	long retry;
	long long response;
	bool CMD41, flgACMD41FAILED;
	int status;
	char temp[512];

	g_mmc_type.initok=0;

	//cf.qspi.qmr=0xE0FF; // Enable Master Mode, 8 Bit, Set to largest divider, ie slowest baud
	spiSetBaud(SPI_SEMA_SDCARD, 0xff);
	cf.qspi.qmr |= 0x00ff;

	for(a=0; a<1000; a++) spitxlong(0xffffffff); //wait some

	for(retry=0;; retry++){
		if((b = spiCmd(0, 0, 7)) != 0xff) break;

		if(retry==10){
			return 1;
		}
	}

	spitxlong(0xffffffff); //some fake

	for(retry=0;; retry++){
		if(retry == 1000){
			return 2;
		}

		if(((response = spiCmd(8, 0x1aa, 39)) & 0x7a00000000) == 0){
			break;
		}
	}

	if((response & 0x400000000)) CMD41 = FALSE;
	else CMD41 = TRUE;

	spitxlong(0xffffffff); //some fake

	if(CMD41){
		for(retry=0;; retry++){
			if(retry == 1000000){
				flgACMD41FAILED = TRUE;
				break;
			}

			b = spiCmd(55, 0, 7);
			b = spiCmd(41, 0x40000000, 7);
			if(!b){
				flgACMD41FAILED = FALSE;
				break;
			}
		}

		if(flgACMD41FAILED){
			for(retry=0;; retry++){
				if(retry == 1000000){
					return 3;
				}

				b = spiCmd(1, 0, 7);
				if(!b) break;
			}
		}


		for(retry=0;; retry++){
			if(retry == 1000) return 4;

			b = spiCmd(58, 0xffffffff, 39);
			if((b & 0xff00000000) == 0){
				if((b & 0x80000000) != 0){
					break;
				}
			}
		}

		if((b & 0x40000000) != 0) flgHC = TRUE;
		else flgHC = FALSE;
	}else{
		flgHC = FALSE;

		for(retry=0;; retry++){
			if(retry == 1000){
				flgACMD41FAILED = TRUE;
				break;
			}

			//b = spiCmd(1, 0, 7);
			b = spiCmd(55, 0, 7);
			b = spiCmd(41, 0, 7);
			if(!b){
				flgACMD41FAILED = FALSE;
				break;
			}
		}

		if(flgACMD41FAILED){
			for(retry=0;; retry++){
				if(retry == 1000){
					return 1;
				}

				b = spiCmd(1, 0, 7);
				if(!b) break;
			}
		}
	}

	if(!flgHC){
		// Set blk lenght 512 byte
		spitxlong(0xffffffff); //some fake
		if(spiCmd(16, 512, 7)){
			return 13;
		}
	}

	spitxlong(0xffffffff); //some fake

	// Set CRC On
	if(spiCmd(59, 1, 7)){
		return 14;
	}

	spitxlong(0xffffffff); //some fake

	if(spiCSDBlock()){
		return 24;
	}

	g_mmc_type.initok=1;

	status = spiGetDataBlock((unsigned long *) temp , 0);
	if(status){
		spi_read_function = spiGetDataBlockSlow;
		spi_write_function = spiSetDataBlockSlow;
	}else{
		spi_read_function = spiGetDataBlock;
		spi_write_function = spiSetDataBlock;
	}
	return 0;
}

/****************************************************************************
 *
 * mmc_readsector
 *
 * read one sector from the card
 *
 * INPUTS
 *
 * data - pointer where to store data
 * sector - which sector is needed
 *
 * RETURNS
 *
 * 0 - if successful
 * other if any error
 *
 ***************************************************************************/
/*int mmc_readsector(void *data, unsigned long sector){
	int ret;
	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_SDCARD)) service_watchdog();
	ret = spiGetDataBlock((unsigned long*)data, sector*512);
	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_SDCARD)) service_watchdog();
	return ret;
}*/

/****************************************************************************
 *
 * mmc_writesector
 *
 * write one sector into the card
 *
 * INPUTS
 *
 * data - pointer where original data is
 * sector - which sector needs to be written
 *
 * RETURNS
 *
 * 0 - if successful
 * other if any error
 *
 ***************************************************************************/
/*int mmc_writesector(void *data, unsigned long sector){
	int ret;
	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_SDCARD)) service_watchdog();
	ret = spiSetDataBlock((unsigned long*)data, sector*512);
	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_SDCARD)) service_watchdog();
	return ret;
}*/

/****************************************************************************
 *
 * mmc_getstatus
 *
 * get status of card, missing or/and removed,changed,writeprotect
 *
 * RETURNS
 *
 * F_ST_xxx code for high level
 *
 ***************************************************************************/
/*static long mmc_getstatus(void){
	long state=0;
	long status;
	int busy;

	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_SDCARD)) service_watchdog();
	if(!g_mmc_type.initok) spiSDinit();

	busy = spiWaitBusy();
	if(busy == 0){
		//fnPr("*");
		status = spiCmd(13, 0, 7);
		if(status){
			//fnPr("!");
			state |= F_ST_CHANGED;
			spiSDinit();
		}
	}else{
		//fnPr("#");
		state |= F_ST_CHANGED;
		spiSDinit();
	}

	if(!g_mmc_type.initok) state |= F_ST_MISSING; //card is not initialized
	return state;
	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_SDCARD)) service_watchdog();
}*/

/****************************************************************************
 *
 * mmc_getphy
 *
 * determinate flash card physicals
 *
 * INPUTS
 *
 * phy - this structure has to be filled with physical information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
/*int mmc_getphy(F_PHY *phy){
	int ret;

	while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_SDCARD)) service_watchdog();
	ret = spiSDinit();
	while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_SDCARD)) service_watchdog();
	if(ret) return ret;

	phy->number_of_cylinders=0;
	phy->sector_per_track=63;
	phy->number_of_heads=255;
	phy->number_of_sectors=((1UL<<(g_mmc_type.C_SIZE_M+2))*(1UL<<g_mmc_type.R_BL_LEN)*(g_mmc_type.CSIZE+1))/512UL;
	return 0;
}*/

/****************************************************************************
 *
 * mmc_initfunc
 *
 * this init function has to be passed for highlevel to initiate the
 * driver functions
 *
 * INPUTS
 *
 * func - structure, where to fill functions
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
/*int mmc_initfunc(F_FUNC *func){
	g_mmc_type.initok = 0;
	spiInit();
	func->readsector=mmc_readsector;
	func->writesector=mmc_writesector;
	func->getstatus=mmc_getstatus;
	func->getphy=mmc_getphy;
	return 0;
}*/

/******************************************************************************
 *
 *  End of mmc.c
 *
 *****************************************************************************/
/**
 * \details This function feeds into the ff module. It returns the current time
 * \returns Current time used by the ff module
 */
DWORD get_fattime(void){
	struct tm *result;
	unsigned long sec, min, hr, day, month, year, returnvalue;

	result = gmtime(&clock_time);
	sec = result->tm_sec;
	min = result->tm_min;
	hr = result->tm_hour;
	day = result->tm_mday;
	month = result->tm_mon + 1;
	year = result->tm_year - 80;

	sec /= 2;
	min <<= 5;
	hr <<= 11;
	day <<= 16;
	month <<= 21;
	year <<= 25;
	returnvalue = 0;
	returnvalue = returnvalue | sec;
	returnvalue = returnvalue | min;
	returnvalue = returnvalue | hr;
	returnvalue = returnvalue | day;
	returnvalue = returnvalue | month;
	returnvalue = returnvalue | year;

	return returnvalue;
}
/**
 * \details This function feeds into the ff module. Initialize SD Card or USB Flash Drive
 * \param drive 0 = SD card, 1 = USB Flash Drive
 * \returns Status code
 */
DSTATUS disk_initialize(BYTE drive){
	BYTE returnvalue;
	if(drive == 0){
		g_mmc_type.initok = 0;
		spiInit();
		returnvalue = 0;
	}else if(drive == 1){
		if(m_ucEnumerated == 2) returnvalue = 0;
		else returnvalue = STA_NODISK;
	}
	return returnvalue;
}
/**
 * \details This function feeds into the ff module. Read the status of the SD Card or the USB Flash Drive
 * \param drive 0 = SD card, 1 = USB Flash Drive
 * \returns Status code
 */
DSTATUS disk_status(BYTE drive){
	BYTE returnvalue;
	long status;

	returnvalue = 0;
	if(drive == 0){
		while(spiSemaphore(SPI_SEMA_ACQUIRE, SPI_SEMA_SDCARD)) service_watchdog();

		if(!g_mmc_type.initok) spiSDinit();
		if(spiWaitBusy() == 0){
			status = spiCmd(13, 0, 7);
			if(status){
				spiSDinit();
			}
		}else{
			spiSDinit();
		}

		if(!g_mmc_type.initok) returnvalue |= STA_NODISK; //card is not initialized

		while(spiSemaphore(SPI_SEMA_RELEASE, SPI_SEMA_SDCARD)) service_watchdog();
	}else if(drive == 1){
		if(m_ucEnumerated != 2) returnvalue |= STA_NODISK;
	}
	return returnvalue;
}
/**
 * \details This function feeds into the ff module. Read formatting of either the SD card or the USB Flash Drive
 * \param drive 0 = SD card, 1 = USB Flash Drive
 * \param command Format parameter
 * \param buffer Pointer to memory, which receives the info
 * \returns Status code
 */
DRESULT disk_ioctl(BYTE drive, BYTE command, void *buffer){
	DRESULT returnvalue;
	WORD *wptr;
	DWORD *dwptr;
	long long int c_size, c_size_m, r_bl_len, totalsize;

	returnvalue = RES_PARERR;
	if(drive == 0){
	if(g_mmc_type.initok){
		switch(command){
			case CTRL_SYNC:
				returnvalue = RES_OK;
				break;

			case GET_SECTOR_SIZE:
				wptr = (WORD *) buffer;
				*wptr = 512;
				returnvalue = RES_OK;
				break;

			case GET_SECTOR_COUNT:
					dwptr = (DWORD *) buffer;
				if(g_mmc_type.CSD_STR == 0){
					c_size = g_mmc_type.CSIZE + 1;
					c_size_m = 4 << g_mmc_type.C_SIZE_M;
					r_bl_len = 1 << g_mmc_type.R_BL_LEN;
					totalsize = c_size * c_size_m * r_bl_len;
					totalsize /= 512;
				}else if(g_mmc_type.CSD_STR == 1){
					c_size = g_mmc_type.CSIZE + 1;
					c_size *= 512 * 1024;
					totalsize = c_size / 512;
				}
				*dwptr = totalsize;
				returnvalue = RES_OK;
				break;

			case GET_BLOCK_SIZE:
				dwptr = (DWORD *) buffer;
				*dwptr = 1;
				returnvalue = RES_OK;
				break;

			case CTRL_ERASE_SECTOR:
				returnvalue = RES_OK;
				break;
		}
	}else{
		returnvalue = RES_NOTRDY;
	}

	}else if(drive == 1){
		if(m_ucEnumerated == 2){
			switch(command){
				case CTRL_SYNC:
					returnvalue = RES_OK;
					break;

				case GET_SECTOR_SIZE:
					wptr = (WORD *) buffer;
					*wptr = m_ulLogicalBlockSize;
					returnvalue = RES_OK;
					break;

				case GET_SECTOR_COUNT:
					dwptr = (DWORD *) buffer;
					*dwptr = m_ulLastLogicalBlock + 1;
					returnvalue = RES_OK;
					break;

				case GET_BLOCK_SIZE:
					dwptr = (DWORD *) buffer;
					*dwptr = 1;
					returnvalue = RES_OK;
					break;

				case CTRL_ERASE_SECTOR:
					returnvalue = RES_OK;
					break;
			}
		}else{
			returnvalue = RES_NOTRDY;
		}
	}
	return returnvalue;
}
#endif //_MMC_C_

