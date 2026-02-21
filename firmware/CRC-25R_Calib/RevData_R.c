/************************************************************************************
  MODULE:  Rev Data -- CRC-25R

  FILE:    RevData_R.c

  DATE:    11/03/08

  *************************************************************************************/
#include "crc.h"
#include "daily.h"
#include "chambfac.h"

//Constants
//Rev number
//const char *rev_num = "a.4143";
const char *rev_num = "4.07a";
//const char *rev_num = "1.00";

//Calibrator Name
const char *crcname = "CRC&-25R";   //& = registered trade mark
//const char *pr_crcname = "CRC-55t"; //for printing header
//const char *oki_crcname = "R   ";    //for OKI header
char pr_crcname[42];
char pr_crcname_large[42];


//2 chamber
const short max_chambers = 2;

//RAM

volatile unsigned long int g_ulTickCounter=0; // 1 ms counter used for calculating when to start reading a chamber(measurement.ulTickAlarm), rollover in 49 days.
volatile bool lowBattery1 = FALSE;
volatile bool lowBattery2 = FALSE;
volatile bool lowBattery3 = FALSE;
//measurement data
MEASUREMENT measurement[9];

//test data: background, bias voltage, zero
BKGDATA bkgdata[8];
BIASDATA biasdata[8];
ZERODATA zerodata[8];

//chamber values
//will be read in from the chamber
CHAMBERVALS chamb_vals[9];

ushort g_Init_USB_PCL;

unsigned char m_ucHotKeyNuclideID[UPPER_LIMIT_CHAMB][8];
unsigned char m_ucHotKeyNuclideID2[UPPER_LIMIT_CHAMB][20];

//SAVED VARIABLES
//values in savesect will be in SRAM and will not be initialized on powerup
// #pragma ghs section bss="savesect"
//chamber data
CHAMBER chamber[9];
ushort chamber_checksum[8];
//remote data
REMOTE remote[9];
CURRENT current;
BACKGND temp_backgnd;


#pragma ghs section bss="ethertx"
ETHERTXDESC ethertxdesc[10];
uchar ethertxbuffer[10][2032];

#pragma ghs section bss="etherrx"
ETHERRXDESC etherrxdesc[10];
uchar etherrxbuffer[10][2032];
