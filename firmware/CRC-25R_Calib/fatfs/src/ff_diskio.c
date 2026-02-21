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

//#include "mmc_m68.h"
#include "mmc.h"
#include "coldfire.h"
#include "crc.h"
#include "keyboard.h"
#include "diskio.h"
#include "time.h"

extern time_t clock_time;

#define SIP

/****************************************************************************
 *
 * port defintions
 *
 ***************************************************************************/
#define fnPr iprintf

#ifdef SIP

#define CLKIN SYSCLK

/****************************** SPI *************************************/


/***************************** PORT *************************************/
#define PORTB_DIR cf.gpio.ddrb

#define PORTB_DATA cf.gpio.portbp

#define PORTB_ASSIGN cf.gpio.pbcdpar
#define PORTQ_ASSIGN cf.gpio.pqspar

#define SPIPORT_DIR PORTB_DIR
#define SPIPORT PORTB_DATA

#define SPIPORT_ASSIGN PORTQ_ASSIGN

#define SPIWP  (1<<0)  //write protect
#define SPICD  (1<<1)  //card detect

#else

#define CLKIN 25000000

#define MBAR 0x10000000

#define CPTQP ((cf.qspi.qwr&0xf0)>>4)

/***************************** PORT *************************************/
#define PORTA_CTRL (*(volatile unsigned long *)(MBAR+0x80))
#define PORTD_CTRL (*(volatile unsigned long *)(MBAR+0x98))
#define PORTA_DIR (*(volatile unsigned short *)(MBAR+0x84))
#define PORTA_DATA (*(volatile unsigned short *)(MBAR+0x86))

#define SPIPORT_CTRL PORTA_CTRL
#define SPIPORT_DIR PORTA_DIR
#define SPIPORT PORTA_DATA

#define SPIWP  (1<<11)  //write protect
#define SPICD  (1<<7)  //card detect

#endif
//#define spiCS



/****************************************************************************
 *
 * SD card info type structure specification
 *
 ***************************************************************************/

typedef struct {
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

MMC_TYPE g_mmc_type_2;

#ifdef spi2CS
/****************************************************************************
 *
 * spiCSlow
 *
 * Set ChipSelect to low active
 *
 ***************************************************************************/
#define spiCSlow fn_spiCSlow
static void fn_spiCSlow() {
//   *SPIPORT &= ~DATA(SPICS);
//  cf.qspi.qwr=0;
}

/****************************************************************************
 *
 * spiCShi
 *
 * Set ChipSelect to high inactive
 *
 ***************************************************************************/

#define spiCShi fn_spiCShi
static void fn_spiCShi() {
//   *SPIPORT |= DATA(SPICS);
//  cf.qspi.qwr=0x1000;
}
#else
#define spi2CSlow()
#define spi2CShi()
#endif

unsigned long CRCtbl_2[ 256 ];
unsigned long crc16_2( unsigned long crc, unsigned char c)
{
long a;

    crc = crc ^ ((unsigned long) c << 8UL);

    for( a=0; a<8; a++ ) {

        if( crc & 0x8000 )
            crc = (crc << 1) ^ 0x1021;
        else
            crc = crc << 1;
    }
   return crc;
}

unsigned long rxdata_2=0;
unsigned long bitcou_2=0;

static void spi2Rx(void) {
	register int i;
      for (i=0,rxdata_2=0,cf.qspi.qar=0x10;i<=((cf.qspi.qwr&0xf0)>>4);rxdata_2|=((cf.qspi.qdr&0xff)<<((3-i++)<<3)));
      bitcou_2=i<<3;
      return;
}

static void spi2TxByte(unsigned char data8) {
   cf.qspi.qar=0;
   cf.qspi.qdr=data8;
   cf.qspi.qwr=0x1000;
   cf.qspi.qir|=1;
   cf.qspi.qdlyr|=0x8000;
   while((cf.qspi.qir&1)==0);
}


static void spi2TxWord(unsigned short data16) {
   cf.qspi.qar=0;
   cf.qspi.qdr=data16>>8;
   cf.qspi.qdr=data16&0xff;
   cf.qspi.qwr=0x1100;
   cf.qspi.qir|=1;
   cf.qspi.qdlyr|=0x8000;
   while((cf.qspi.qir&1)==0);
}

static void spi2TxLong(unsigned long data32) {
   cf.qspi.qar=0;
   cf.qspi.qdr=data32>>24;
   cf.qspi.qdr=(data32>>16)&0xff;
   cf.qspi.qdr=(data32>>8)&0xff;
   cf.qspi.qdr=data32&0xff;
   cf.qspi.qwr=0x1300;
   cf.qspi.qir|=1;
   cf.qspi.qdlyr|=0x8000;
   while((cf.qspi.qir&1)==0);
}


static long spi2GetBit(void) {
   if (!bitcou_2) {
      cf.qspi.qar=0;
      cf.qspi.qdr=0xff;
      cf.qspi.qwr=0x1000;
      cf.qspi.qir|=1;
      cf.qspi.qdlyr|=0x8000;
      while((cf.qspi.qir&1)==0);
      spi2Rx();
   }
   if (bitcou_2) {
      bitcou_2--;
      if (rxdata_2&0x80000000) {
         rxdata_2<<=1;
         return 1;
      }
      rxdata_2<<=1;
      return 0;
   }
   else return 1;
}

static long spi2GetBit32() {
	long ret=rxdata_2;
	long oldbitcou=bitcou_2;

	cf.qspi.qar=0;
	cf.qspi.qdr=0xff;
	cf.qspi.qdr=0xff;
	cf.qspi.qdr=0xff;
	cf.qspi.qdr=0xff;
	cf.qspi.qwr=0x1300;
	cf.qspi.qir|=1;
	cf.qspi.qdlyr|=0x8000;
	while((cf.qspi.qir&1)==0);
	spi2Rx();

	ret|=rxdata_2>>oldbitcou;
	rxdata_2<<=32-oldbitcou;
	bitcou_2=oldbitcou;
	return ret;
}

/****************************************************************************
 *
 * spiInit
 *
 * Init SPI ports, directions
 *
 ***************************************************************************/
static void spi2Init(void)
{
  long a;

  for ( a = 0; a < 256; a++ )
  {
    CRCtbl_2[a]=crc16_2(0, (unsigned char)a );
  }

  memset(&g_mmc_type_2,0,sizeof(MMC_TYPE));

#ifdef SIP
  SPIPORT_ASSIGN=0x7f; 	/* set PORTQ to QSPI Primary OPeration, Use CS0-3 */
//  SPIPORT_DIR&=~(SPICD+SPIWP);	/* set WP+CD as inputs on the proper port */
  cf.qspi.qwr=0x9000;		/* stop + active low */
  cf.qspi.qmr=0xC0FF;		/* Master mode */
// Clive: added delay like we are using for ADC
  cf.qspi.qdlyr=0x2020;		// Delay = 0x20 for CS to CLK, Delay after Transfer
//  cf.qspi.qdlyr=0;
  cf.qspi.qar=0x20;
// Clive: Changed to Use QCR QSPI_CS (Bits 8-11) Set to 1000 to select CS1 -- prototype
//Marcia: changed to use QCR QSPI_CS Bits 8-11) set to 0x0e00 to select Chip Selects
  cf.qspi.qdr=0x8e00;
  cf.qspi.qdr=0x8e00;
  cf.qspi.qdr=0x8e00;
  cf.qspi.qdr=0x8e00;		/* set commands */

#else
//  SPIPORT_CTRL&=~((3*(SPICD<<1))+(3*(SPIWP<<1)));	/* set WP+CD as general port on the proper port */
//  SPIPORT_DIR&=~(SPICD+SPIWP);		/* set WP+CD as inputs on the proper port */
  cf.qspi.qwr=0x9000;		/* stop + active low */
  cf.qspi.qmr=0xC0FF;		/* Master mode */
  cf.qspi.qdlyr=0;
  cf.qspi.qar=0x20;
  cf.qspi.qdr=0x8e00;
  cf.qspi.qdr=0x8e00;
  cf.qspi.qdr=0x8e00;
  cf.qspi.qdr=0x8e00;
#endif
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

static long spi2DataBits(long bits, unsigned long *pcrc) {
long a;
long data=0;
unsigned long crc=*pcrc;
   for (a=0; a<bits; a++) {
      data<<=1;

      if ( spi2GetBit() ) {
         crc^=0x8000;
         data|=1;
      }

      if( crc & 0x8000 ) crc = (crc << 1) ^ 0x1021;
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
static int spi2WaitStartBit(void){
    long a= 50000; //500000;
   bitcou_2=0;
   while (a--)
   {
       service_watchdog();
      if (!spi2GetBit()) return 0;
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

static int spi2WaitBusy(void) {
    long a=50000; //500000;
   while(--a)
   {
       service_watchdog();

       if (spi2GetBit()) return 0;
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

static long spi2Cmd(long cmd, long data) {
register long a;
register long crc7=0;

   cmd|=0xffffff40; //add start bit and 1
   spi2TxLong(cmd);  //send cmd and some fake bits before it

   for (a=0; a<8; a++) {     //send start and cmd

      if (cmd & 0x80) {
         crc7^=0x40;
      }

      cmd<<=1;

      if( crc7 & 0x40 ) crc7 = (crc7 << 1) ^ 0x09;
      else crc7 = crc7 << 1;
   }

   spi2TxLong(data);  //send data

   for (a=0; a<32; a++) {   //send 32bit argument

      if (data & 0x80000000) {
         crc7^=0x40;
      }

      data<<=1;

      if( crc7 & 0x40 ) crc7 = (crc7 << 1) ^ 0x09;
      else crc7 = crc7 << 1;
   }

   crc7<<=1;  //set crc7 into correct position
   crc7|=1;   //add endbit

   spi2TxByte((unsigned char)crc7); //send crc7 and endbit

   if (spi2WaitStartBit()==0)
   {
     for (a=0;a<7;a++)
     {
       data<<=1;
       data|=spi2GetBit();
     }
     return(data);
   }
   return 0xff;
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
 * addr - address where to start read card, it has to be sector aligned
 *
 * RETURNS
 *
 * 0 - if successful
 * other if any error (crc,timeouts)
 *
 ***************************************************************************/

static int spi2GetDataBlock(unsigned long *dwaddr, unsigned long addr) {
register unsigned long crc;
   if (!g_mmc_type_2.initok) { return 17; }//card is not initialized

   if ((SPIPORT) & SPICD) { return 18; }//card is missing
   //if (!mmc_present) return 18;

   spi2CSlow();
   if (spi2Cmd(17,addr)) {
      spi2CShi();
      g_mmc_type_2.initok=0;
      return 1;
   }

   if (spi2WaitStartBit()) {
      spi2CShi();
      return 2;
   }

   crc=0;
   {
      register long a;
      register unsigned long *crctable=CRCtbl_2;
      for (a=0; a<128; a++) {  //get data 128long 512bytes
         register unsigned long data=spi2GetBit32();

         *dwaddr++=data;

         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>24)) & 0x0FFUL  ];
         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>16)) & 0x0FFUL  ];
         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>8)) & 0x0FFUL  ];
         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ data) & 0x0FFUL  ];
      }

      for (a=0; a<16; a++) {   //check crc in next two bytes
         if ( spi2GetBit() ) {
            crc^=0x8000;
         }

         if( crc & 0x8000 ) crc = (crc << 1) ^ 0x1021;
         else crc = crc << 1;
      }
   }

   spi2CShi();
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
static int spi2SetDataBlock(unsigned long *dwaddr, unsigned long addr) {
register unsigned long crc;

   if (!g_mmc_type_2.initok) return 17;//card is not initialized

   if ((SPIPORT) & SPICD) return 18;//card is missing
//   if ((SPIPORT) & SPIWP) return 4; //write protected!!
   //if (!mmc_present) return 18;

// NOTE: CLIVE:  No Way to detect Write Protect Yet!!

   spi2CSlow();
   if (spi2Cmd(24,addr)) {
      spi2CShi();
      g_mmc_type_2.initok=0;
      return 13;
   }


   spi2TxLong(0xfffffffe); //startbit
   crc=0;
   {
      register long a;
      register unsigned long *crctable=CRCtbl_2;
      register unsigned long data;

      for (a=0; a<128; a++) { //sends 128long 512bytes
         data=*dwaddr++;

         spi2TxLong(data);

         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>24)) & 0x0FFUL  ];
         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>16)) & 0x0FFUL  ];
         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ (data>>8)) & 0x0FFUL  ];
         crc = ( crc << 8UL ) ^ crctable[ ( (crc >> 8UL) ^ data) & 0x0FFUL  ];
      }

      spi2TxWord((unsigned short)crc);

      if (spi2WaitStartBit()) {
         spi2CShi();
         return 1;
      }

      data=0;
      for (a=0; a<4; a++) {   //get data ack
         data<<=1;
         if (spi2GetBit()) data|=1;
      }

      data &= 0x1f; //keep valid bits

      if (data != 0x05) { //read accepted?
         spi2CShi();
         return (int)((data<<8)+1);
      }
   }


   if (spi2WaitBusy()) {
      spi2CShi();
      return 8;
   }

   spi2CShi();

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

static int mmc_readsector(void *data,unsigned long sector) {
int ret;

   ret=spi2GetDataBlock((unsigned long*)data,sector*512);

   //if (ret) fnPr("RDERROR: %d\n",ret);

   return ret;
}

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

static int mmc_writesector(void *data,unsigned long sector) {
int ret;

   ret=spi2SetDataBlock((unsigned long*)data,sector*512);

   //if (ret) fnPr("WRERROR: %d\n",ret);

   return ret;
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

static int spi2CSDBlock() {
unsigned long crc;
unsigned long time;

   spi2CSlow();
   if (spi2Cmd(9,0)) {
      spi2CShi();
      return 2; //r1 any error
   }

   if (spi2WaitStartBit()) {
      spi2CShi();
      return 3;
   }

   crc=0;

   g_mmc_type_2.CSD_STR  =spi2DataBits(2,&crc);
   g_mmc_type_2.SPEC_VERS=spi2DataBits(4,&crc);
            spi2DataBits(2,&crc); //reserved

   g_mmc_type_2.TAAC     =spi2DataBits(8,&crc);
   g_mmc_type_2.NSAC     =spi2DataBits(8,&crc);
   g_mmc_type_2.TRANSPEED=spi2DataBits(8,&crc);
   g_mmc_type_2.CCC      =spi2DataBits(12,&crc);
   g_mmc_type_2.R_BL_LEN =spi2DataBits(4,&crc);
   g_mmc_type_2.R_BL_PART=spi2DataBits(1,&crc);
   g_mmc_type_2.WR_BL_MIS=spi2DataBits(1,&crc);
   g_mmc_type_2.RD_BL_MIS=spi2DataBits(1,&crc);
   g_mmc_type_2.DSR_IMP  =spi2DataBits(1,&crc);
            spi2DataBits(2,&crc); //reserved

   g_mmc_type_2.CSIZE    =spi2DataBits(12,&crc);
   g_mmc_type_2.RC_MIN   =spi2DataBits(3,&crc);
   g_mmc_type_2.RC_MAX   =spi2DataBits(3,&crc);
   g_mmc_type_2.WC_MAX   =spi2DataBits(3,&crc);
   g_mmc_type_2.WC_MIN   =spi2DataBits(3,&crc);
   g_mmc_type_2.C_SIZE_M =spi2DataBits(3,&crc);
   g_mmc_type_2.ERASE_G_S=spi2DataBits(5,&crc);
   g_mmc_type_2.ERASE_G_M=spi2DataBits(5,&crc);
   g_mmc_type_2.WP_G_S   =spi2DataBits(5,&crc);
   g_mmc_type_2.WP_G_EN  =spi2DataBits(1,&crc);
   g_mmc_type_2.DEF_ECC  =spi2DataBits(2,&crc);
   g_mmc_type_2.R2W_FACT =spi2DataBits(3,&crc);
   g_mmc_type_2.WR_BL_LEN=spi2DataBits(4,&crc);
   g_mmc_type_2.WR_BL_PAR=spi2DataBits(1,&crc);
            spi2DataBits(4,&crc); //reserved

   g_mmc_type_2.C_PR_APP =spi2DataBits(1,&crc);
   g_mmc_type_2.F_FORMATG=spi2DataBits(1,&crc);
   g_mmc_type_2.COPY     =spi2DataBits(1,&crc);
   g_mmc_type_2.P_WR_PROT=spi2DataBits(1,&crc);
   g_mmc_type_2.T_WR_PROT=spi2DataBits(1,&crc);
   g_mmc_type_2.F_FORMAT =spi2DataBits(2,&crc);
   g_mmc_type_2.ECC      =spi2DataBits(2,&crc);
   g_mmc_type_2.CRC7     =spi2DataBits(7,&crc);
            spi2DataBits(1,&crc); //reserved

   spi2DataBits(16,&crc); //crc

   spi2CShi();

   if (crc) return 10;

   switch (g_mmc_type_2.TRANSPEED & 7) {
   case 0: time=1; break; //100K
   case 1: time=10;  break; //1M
   case 2: time=100;   break; //10M
   case 3: time=1000;   break;  //100M
   default: return 0;
   }
   switch ((g_mmc_type_2.TRANSPEED >> 3) &0x0f) {
   case 0: return 0;
   case 1: time*=10; break; //1.0
   case 2: time*=12; break; //1.2
   case 3: time*=13; break; //1.3
   case 4: time*=15; break; //1.5
   case 5: time*=20; break; //2.0
   case 6: time*=25; break; //2.5
   case 7: time*=30; break; //3.0
   case 8: time*=35; break; //3.5
   case 9: time*=40; break; //4.0
   case 10: time*=45; break; //4.5
   case 11: time*=50; break; //5.0
   case 12: time*=55; break; //5.5
   case 13: time*=60; break; //6.0
   case 14: time*=70; break; //7.0
   case 15: time*=80; break; //8.0
   }

   time<<=1;
   time=((CLKIN/10000)+(time-1))/time;
   if (time<2) time=2;
   if (time>0xff) time=0xff;
   cf.qspi.qmr=0xC000+time;

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

static int spi2SDInit(void) {
long a;
long retry;

   g_mmc_type_2.initok=0;

   cf.qspi.qmr=0xC0FF;


   spi2CSlow();
   for (a=0; a<1000; a++) spi2TxLong(0xffffffff); //wait some

   for (retry=0;; retry++) {
      if ((a=spi2Cmd(0,0))!=0xff) break;

      if (retry==10) {
         spi2CShi();
         return 1;
      }
   }

   spi2TxLong(0xffffffff); //some fake


   for (retry=0; ; retry++ ){

      if (retry==1000) {
         spi2CShi();
         return 1;
      }

      spi2CSlow();
      if (!spi2Cmd(1,0)) {
         spi2CShi();
         break;
      }

      spi2CShi();
   }

   spi2CSlow();

   if (spi2Cmd(16,512)) {; //set blk lenght 512 byte
      spi2CShi();
      return 13;
   }

   spi2TxLong(0xffffffff); //some fake
   spi2CShi();

   spi2CSlow();

   if (spi2Cmd(59,1)) {  //set crc on
      return 14;
   }

   spi2TxLong(0xffffffff); //some fake
   spi2CShi();

   if (spi2CSDBlock()) {
      spi2CShi();
      return 24;
   }

   g_mmc_type_2.initok=1;

   return 0;
}

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

static long mmc_getstatus() {
long state=0;
//   if ((SPIPORT) & SPIWP) state|=F_ST_WRPROTECT;
// NOTE: CLIVE:  No Way to detect Write Protect Yet!!

   if ((SPIPORT) & SPICD) {
      g_mmc_type_2.initok=0;
      state|=F_ST_MISSING;
   }
   else {

      if (!g_mmc_type_2.initok) {
          spi2SDInit();
      }

      spi2CSlow();
      if (spi2WaitBusy()==0)
      {
        //fnPr("*");
        if (spi2Cmd(13,0)) {
           //fnPr("!");
           state|=F_ST_CHANGED;
           spi2CShi();
           spi2SDInit();
        }
      }
      else
      {
         //fnPr("#");
         state|=F_ST_CHANGED;
         spi2CShi();
         spi2SDInit();
      }
      spi2CShi();
   }

   if (!g_mmc_type_2.initok) state|=F_ST_MISSING; //card is not initialized


   return state;
}

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

static int mmc_getphy(F_PHY *phy) {
int ret=spi2SDInit();

   if (ret) return ret;

   phy->number_of_cylinders=0;
   phy->sector_per_track=63;
   phy->number_of_heads=255;
   phy->number_of_sectors=((1UL<<(g_mmc_type_2.C_SIZE_M+2))*(1UL<<g_mmc_type_2.R_BL_LEN)*(g_mmc_type_2.CSIZE+1))/512UL;

   return 0;
}

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
 ***************************************************************************

static int mmc_initfunc(F_FUNC *func) {
   g_mmc_type_2.initok=0;

   spi2Init();

   func->readsector=mmc_readsector;
   func->writesector=mmc_writesector;
   func->getstatus=mmc_getstatus;
   func->getphy=mmc_getphy;
   return 0;
} */

/******************************************************************************
 *
 *  End of mmc.c
 *
 *****************************************************************************/

#endif //_MMC_C_

DSTATUS disk_initialize(BYTE drivenumber){
	g_mmc_type_2.initok = 0;
    spi2Init();
    if(spi2SDInit()) return STA_NOINIT;
    else return 0;
}

DSTATUS disk_status(BYTE drivenumber){
	if(g_mmc_type_2.initok) return 0;
	else return disk_initialize(drivenumber);
}

DWORD get_fattime(void){
	struct tm result;
	unsigned long sec, min, hr, day, month, year, returnvalue;

	gmtime_r(&clock_time, &result);
	sec = result.tm_sec;
	min = result.tm_min;
	hr = result.tm_hour;
	day = result.tm_mday;
	month = result.tm_mon + 1;
	year = result.tm_year - 80;

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

DRESULT disk_ioctl(BYTE drivenumber, BYTE command, void *buffer){
	DRESULT returnvalue;
	WORD *sectorsize;
	DWORD *sectorcount;

	switch(command){
		case CTRL_SYNC:
			if(g_mmc_type_2.initok) returnvalue = RES_OK; // There is no write back cache to flush
			else returnvalue = RES_NOTRDY;
			break;

		case GET_SECTOR_SIZE:
			if(g_mmc_type_2.initok){
				sectorsize = (WORD *) buffer;
				*sectorsize = 512;
				returnvalue = RES_OK;
			}else{
				returnvalue = RES_NOTRDY;
			}
			break;

		case GET_SECTOR_COUNT:
			if(g_mmc_type_2.initok){
				sectorcount = (DWORD *) buffer;
				returnvalue = RES_OK;
			}else{
				returnvalue = RES_NOTRDY;
			}
			break;

		case GET_BLOCK_SIZE:
			break;

		case CTRL_ERASE_SECTOR:
			break;

		default:
			returnvalue = RES_PARERR;
			break;
	}

	return returnvalue;
}
