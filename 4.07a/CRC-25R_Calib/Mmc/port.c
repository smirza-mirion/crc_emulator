#ifndef _PORT_C_
#define _PORT_C_

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
 ***************************************************************************/

#include "fat.h"
#include "port.h"

extern unsigned short gettime(void);
extern unsigned short getdate(void);

/****************************************************************************
 *
 * f_getrand
 *
 * This function should be ported. It has to return a different 32bit
 * random number whenever it is called. Random number generator could be
 * get from system time, this algorith below is just a simple random
 * number generator
 *
 * INPUTS
 *
 * rand - a number which could be used for random number generator
 *
 * RETURNS
 *
 * 32bit random number
 *
 ***************************************************************************/

static unsigned long dwrand=0x729a8fb3;

unsigned long f_getrand(unsigned long rand) {
long a;

   dwrand^=f_gettime();

   for (a=0; a<32; a++) {
      if (rand&1) {
         dwrand^=0x34098bc2;
      }
      if (dwrand&0x8000000) {
         dwrand<<=1;
         dwrand|=1;
      }
      else dwrand<<=1;
      rand>>=1;
   }

   return dwrand;
}

/****************************************************************************
 *
 * f_getdate
 *
 * need to be ported depending on system, it retreives the
 * current date in DOS format
 *
 * RETURNS
 *
 * current date
 *
 ***************************************************************************/

unsigned short f_getdate(void) {
//unsigned short year=1980;
//unsigned short month=1;
//unsigned short day=1;

//unsigned short pcdate= (((year-1980) <<  F_CDATE_YEAR_SHIFT)  & F_CDATE_YEAR_MASK) |
//					   ((month		 <<  F_CDATE_MONTH_SHIFT) & F_CDATE_MONTH_MASK) |
//					   ((day		 <<  F_CDATE_DAY_SHIFT)   & F_CDATE_DAY_MASK);
//	return pcdate;

   return getdate();
}

/****************************************************************************
 *
 * f_gettime
 *
 * need to be ported depending on system, it retreives the
 * current time in DOS format
 *
 * RETURNS
 *
 * current time
 *
 ***************************************************************************/

unsigned short f_gettime(void) {
//unsigned short hour=12;
//unsigned short min=0;
//unsigned short sec=0;

//unsigned short pctime= ((hour     <<  F_CTIME_HOUR_SHIFT) & F_CTIME_HOUR_MASK) |
//					   ((min      <<  F_CTIME_MIN_SHIFT)  & F_CTIME_MIN_MASK) |
//					   (((sec>>1) <<  F_CTIME_SEC_SHIFT)  & F_CTIME_SEC_MASK);
//	return pctime;

	return gettime();
}

/****************************************************************************
 *
 * end of port.c
 *
 ***************************************************************************/

#endif /* _PORT_C_ */
