/************************************************************************************
  MODULE:  GLOBALS -- CRC-25

  FILE:    Globals_25.c

  DATE:    08/24/06

  This file contains all the globals for all CRC-25 products

  *************************************************************************************/
#include  "crc.h"
#include "coldfire.h"
#include "sl811s.h"
#include "daily.h"
#include "counter.h"


//unit conversion factors for activity
const float unitfact[6] = {1.e+6,1.e+3,1.0,1.e-3,1.e-6,1.e-9};

const float upci[9] = {19.99e-6,199.9e-6,999.e-6,
            1.999e-3,19.99e-3,199.9e-3,
            999.e-3,1.999,8.0};
const float downci[9] = {14.0e-6,140.e-6,.6e-3,1.4e-3,
              14.e-3,140.e-3,.6,1.4,8.0};
const float upbq[8] = {1.999e+6,19.99e+6,199.9e+6,
         1999e+6,19.99e+9,199.9e+9,37000.e+9,0};
const float downbq[8] = {1.40e+6,14.0e+6,140e+6,1.4e+9,
           14.0e+9,140.e+9,37000.e+9,0};

const float upci77t[8] = { 1.999e-3,
            			   19.99e-3,
						   199.9e-3,
						   999.0e-3,
						   1.999e+0,
						   19.99e+0,
						   199.9e+0,
						   1000.e+0
						 };
const float downci77t[8] = { 1.400e-3,
						     14.00e-3,
							 140.0e-3,
							 600.0e-3,
							 1.400e+0,
							 14.00e+0,
							 140.0e+0,
							 1000.e+0
						   };

const short month_days[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};


const short ndecbq[] = {3,2,1,0,2,1,0,0,0,0};
const short ndecci[] = {2,1,0,3,2,1,0,3,2,0};
const short ndec77tci[] = {2,2,1,0,3,2,1,0};
//EEPROM initialization sting
const char cap_str[] = "Capintec";
const char mir_str[] = "Mirion";

const char rev_num_str1_0[] = "Rev1.0";
const char rev_num_str1_1[] = "Rev1.1";
//unit strings
const char *unit_str[] =
    {
        "$Ci",  //microCurie
        "mCi",
        " Ci",
        "kBq",
        "MBq",
        "GBq"
    };

//time units
const char *timeunit[] =
    {
        "M",
        "H",
        "D",
        "Y"
    };

//RAM
//duration of tone for click or beep
short tone_duration = 0;

//standard linearity measurement, 12 possible measurements for each chamber
STANDLIN standlin[8][12];

short num_daily;    //number of daily sources
short adc_wait_time;    //interval between adc conversions -- function of # of chambers
time_t clock_time;  //time read from Real Time Clock
time_t prev_clock_time;   //previously displayed clock time

//user calibration numbers
USERCAL user_cal[MAX_NEW_CAL];
//container factors
CALCORR calcorr[MAX_CAL_COR];


//test data Well
C_TEST c_test;

//data for current daily test for each chamber
DAILY_TEST_RES  daily_res[8];
volatile unsigned char g_ucRelease = 0;
volatile unsigned char g_ucStartFlag = 0;

//ColdFire Register structure
#pragma ghs section bss="cfsect"
#pragma pack(1)
volatile mcf5282 cf;
#pragma pack()

#pragma pack()
//Interrupt Vectors
#pragma ghs section bss="vectsect"
vectors vector_base;
#pragma ghs section bss="default"


