/**
 * \file
 * \details This file contains functions, which calls the I2C layer and communicates with the Real Time Clock chip.
 */
/***************************************************************************************
  MODULE:  Real Time Clock

  FILE:    RealTimeClock.c

  DATE:    05/24/06

  Reads / Write to Real Time Clock:  Dallas 1338

	*********************************************************************************************/

#include "crc.h"
#include "i2c.h"
#include <string.h>
static uchar to_bcd(ushort num);
static ushort from_bcd(uchar bnum);
static bool write_rtc(uchar *bdata);
static short read_rtc(uchar *data);
/////////////////////////////////////////////////////////////
// Returns time in PC Format for MMC file system
// Short Int returned Format:
//	seconds = (t & 0x1F)       - Valid range: 0-30
//  minutes = (t & 0x7E0)>>5   - Valid range: 0-59
//  hour    = (t & 0xf800)>>11 - Valid range: 0-23
/////////////////////////////////////////////////////////////
/**
 * \details Read time from RTC Chip
 * \returns Time formatted as seconds = (t & 0x1F), minutes = (t & 0x7E0)>>5, hour = (t & 0xf800)>>11
 */
ushort gettime(void)
{
		ushort pctime;
		int iret;
		uchar data[10];
		ushort hours;
		ushort minutes;
		ushort seconds;
        ushort hr,min,sec;

		setup_i2c();

		//read the Real Time Clock into data
		iret = read_rtc(data);

		//good return
		if(iret == 1)
		{

			hours = from_bcd(data[2] & 0x3f);
			hours = (hours > 23) ? 23 : hours;

			minutes = from_bcd(data[1] & 0x7f);
			minutes = (minutes > 59) ? 59 : minutes;

			seconds = from_bcd(data[0] & 0x7f);
			seconds = (seconds > 59) ? 60 : seconds;
			seconds >>=1;    //dvides by 2 to make it 5 bits

            hr = (hours & 0x1f) << 11;
            min = (minutes & 0x3f) << 5;
            sec = seconds & 0x1f;
            

            pctime = hr | min | sec;

			return pctime;
		}

		return 0;
}
/////////////////////////////////////////////////////////////
// Returns date in PC Format for MMC file system
// Short Int returned Format:
//	day = (t & 0x1F)       - Valid range: 0-31
//  month = (t & 0x1E0)>>5   - Valid range: 1-12
//  year = (t & 0xf800)>>9 - Valid range: 0-119
/////////////////////////////////////////////////////////////
/**
 * \details Read Date from RTC chip
 * \returns Date formatted as day = (t & 0x1F), month = (t & 0x1E0)>>5, year = (t & 0xf800)>>9
 */
ushort getdate(void)
{
		ushort pcdate;
		int iret;
		uchar data[10];
		ushort days;
		ushort months;
		ushort years;

		setup_i2c();

		//read the Real Time Clock into data
		iret = read_rtc(data);

		//good return
		if(iret == 1)
		{
			days = from_bcd(data[4] & 0x3f);
			days = (days > 31) ? 31 : days;

			months = from_bcd(data[5] & 0x1f);
			months = (months > 12) ? 12 : months;

			years = from_bcd(data[6]);
            //assume 21st century -- # of years since 1980
            years += 20;    
			years = (years > 119) ? 119 : years;

			pcdate = (days & 0x1f) |
					 ((months & 0x1f) << 5) |
					 ((years & 0x7f) << 9);

			return pcdate;
		}

		return 0;
}


	/********
	read the real time clock
	convert data to timet_t format and send to nowtime
	********/
/**
 * \details Read DateTime from RTC chip
 * \param nowtime Pointer to time_t structure to receive the DateTime
 * \returns Status Code, 1 = OK, Other = Error
 */
short read_clock(time_t *nowtime)
	{
		uchar data[10];
		int iret;
		ushort year;
        struct tm  tmt;

        //don't read clock if i2c is in use
        if(get_i2c_in_use())
            return 1;
        
		setup_i2c();

		//read the Real Time Clock into data
		iret = read_rtc(data);


        if(iret != 1)
            return iret;
        
        tmt.tm_mon = from_bcd(data[5] & 0x1f) - 1; //mon strts at 0
        tmt.tm_mday = from_bcd(data[4] & 0x3f);
        year = from_bcd(data[6]);
        if(year < 70)
            tmt.tm_year = year + 100;  //21st cent
        else
            tmt.tm_year = year;  //20th cent
        tmt.tm_hour = from_bcd(data[2] & 0x3f);
        tmt.tm_min = from_bcd(data[1] & 0x7f);
        tmt.tm_sec = from_bcd(data[0] & 0x7f);
        
        *nowtime = mk_time(&tmt);
        return 1;
	}


	/**********
	set clock chip with data in tm structure
	*********/
/**
 * \details Set DateTime in the RTC chip
 * \param tmt Pointer to tm structure, which holds the DateTime to write to the RTC
 * \returns True = Success, False = Error
 */
	bool set_clock(struct tm *tmt)
	{
		uchar bdata[8];
		ushort year;

		setup_i2c();

		// change to bcd
		bdata[0] = 0;					//seconds , oscillator set
		bdata[3] = to_bcd((ushort)tmt -> tm_wday + 1);     	//day of week
		bdata[1] = to_bcd((ushort)tmt -> tm_min);				// minutes
		bdata[2] = to_bcd((ushort)tmt -> tm_hour);   	//hours, set to 24 hr format
		bdata[4] = to_bcd((ushort)tmt -> tm_mday) ;				//day of month
		bdata[5] = to_bcd((ushort)tmt -> tm_mon + 1);			//month 0 to 11 in structure

		year = (ushort)tmt -> tm_year;
		if(year >= 100) //21st cent
			year -= 100;
        bdata[6] = to_bcd(year);		// 2 digit year


		// write to the clock
        if(write_rtc(bdata))
			return TRUE;

		//error return
		return FALSE;
	}

	// convert from integer to bcd
/**
 * \details Convert from integer to BCD (Binary Coded Decimal)
 * \param num Integer value
 * \returns BCD value
 */
    static uchar to_bcd(ushort num)
	{
		return ((16 * (num / 10)) + (num % 10));
	}



	// convert from bcd to integer
/**
 * \details Convert from BCD (Binary Coded Decimal) to integer
 * \param bnum BCD value
 * \returns Integer value
 */
    static ushort from_bcd(uchar bnum)
	{
		return (10 * (bnum / 16) + (bnum & 0x0f));
	}


	// read Dallas 1338 rtc
/**
 * \details Read Dallas 1338 RTC
 * \param data Pointer byte array, which receives data from the RTC chip
 * \returns 1 = OK, Other = Error
 */
    static short read_rtc(uchar *data)
	{
		uchar cmd[2];

		//point to 1st clock register
		cmd[0] = 0;
		if (!i2c_write(RTC_SLAVE_ADDRESS,cmd,1,TRUE))
			return 0;   //error return

		// read the data
		if(!i2c_read(RTC_SLAVE_ADDRESS,data,7,FALSE))
			return 0;

		/* if MSB of 1st register set, oscillator
			not set, return error so that caller knows
			to re-set the clock */
		if(data[0] & 0x80)
			return -1;



		return 1;

	}


	/*************
	write the data to the Dallas 1338 rtc
	data is in bcd format
	***********/
/**
 * \details Write the data to the Dallas 1338 RTC
 * \param bdata Pointer to byte array to write to RTC
 * \returns True = Success, False = Error
 */
    static bool write_rtc(uchar *bdata)
	{
		uchar cmd[10];

		// point to 1st register
		cmd[0] = 0;

		// copy data into cmd
		memcpy(&cmd[1],bdata,7);

		if (!i2c_write(RTC_SLAVE_ADDRESS,cmd,8,TRUE))
			return FALSE;

		return  TRUE;

	}


	//setup the Real Time Clock
/**
 * \details Setup the RTC registers
 * \returns True = Success, False = Error
 */
	bool setup_rtc()
	{
		uchar cmd[10];

		//point to register 7
		cmd[0] = 7;
		//bits 7 & 4 for Square Wave
		//bits 0 & 1 for 32.768 kHz
		//other bits are not used
		cmd[1] = 0x93;

		//write command to the RTC  & return write status
		if(!i2c_write(RTC_SLAVE_ADDRESS,cmd, 2,TRUE))
			return FALSE;

		return TRUE;
	}


    //routine to replace Green Hills mktime because of DST problems
    //gives # of seconds since Jan 1, 1970 
static const short fmon[] = {428,459,122,153,183,214,244,275,306,336,367,397};
#define     FYEAR0 69.
    time_t mk_time(struct tm *tmt)
    {

        float fyear1;
        short mon1,day1,year1,sec1,hour1,min1;
        time_t yy,secs,mins,dday;

        mon1 = tmt -> tm_mon + 1;
        day1 = tmt -> tm_mday;
        year1 = tmt -> tm_year;
        sec1 = tmt -> tm_sec;
        hour1 = tmt -> tm_hour;
        min1 = tmt -> tm_min;

        fyear1 = (float)year1;

        if(mon1 <= 2)
            fyear1 -= 1.;

        yy = (int)(365.25 * fyear1) - (int)(365.25 * FYEAR0);
        dday = day1 - 1 + fmon[mon1 - 1] - fmon[0] + yy;
        mins = 60 * hour1 + min1;
        secs = 86400 * dday + 60 * mins + sec1;

        //day of the week = days since 1/1/89 (a Sunday) mod 7
        tmt -> tm_wday = (dday - 6940) % 7;
        

        return(secs);

    }    
