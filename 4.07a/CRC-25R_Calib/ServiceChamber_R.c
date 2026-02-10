/**
 * \file
 * \details This file contains functions, which auto-ranges the ionization chamber and detects over-range from the ADC reading from the iometer board.
 */
/*********************************************************************
  MODULE:	SERVICE CHAMBERfor CRC-25

  FILE:		ServiceChamber_R.c

  DATE:		12/06/06
          


  *************************************************************************/
#include "crc.h"
#include "qspi.h"
#include "screen.h"
#include "keyboard.h"
#include "remote.h"
#include "lowlevelsnoop.h"
#include "message.h"
#include "chambfac.h"
#include <string.h>

#define MIN_CHAMB_OLD	200
#define MIN_CHAMB_NEW	200
#define MIN_CHAMB_C   200    // 200000 / 1000
#define MIN_CHAMB_K   72    // 200000 / 2777
#define MAX_CHAMB_OLD	32000	//~500mV
#define MAX_CHAMB_NEW	256000 //~500mV
#define MAX_CHAMB_C   256000 //~500mV
#define MAX_CHAMB_K   256000 //~500mV

extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern CURRENT  current;
extern volatile bool DisplayActivity_SendActivity;

bool chamber_one_gain_relay(short ch);
bool chamber_stored_gain_factor(short ch);
double chamber_gain_factor_0(short ch);
double chamber_gain_factor_1(short ch);
double chamber_gain_factor_2(short ch);
void send_to_amulet_string(uchar ucIndex, char message0[]);

// chamber service routine
/**
 * \details Chamber service routine, which handles auto-ranging and over-range
 * \param ch Chamber number
 * \returns None
 */
void servicechamber(short ch){
	long lAdcZero;
	long chamb;
	long value;
	bool min_flag;
	bool max_flag;

	unsigned char ucChamber;
	long lChamberMax;
	long lChamberMin;
	unsigned char ucMinFlag;
	unsigned char ucMaxFlag;
	long lADCValue;
	long lChamberValue;
	long lADCZero;
	long lLocalChamberMax;
	long lLocalChamberMin;
	long lLocalADCValue;

	ucChamber = (unsigned char) ch;

	if (chamber_one_gain_relay(ch)){
		if(chamber_C()){
			lLocalChamberMax = MAX_CHAMB_C;
			lLocalChamberMin = MIN_CHAMB_C;
		}else if(chamber_K()){
			lLocalChamberMax = MAX_CHAMB_K;
			lLocalChamberMin = MIN_CHAMB_K;
		}else{
			lLocalChamberMax = MAX_CHAMB_NEW;
			lLocalChamberMin = MIN_CHAMB_NEW;
		}
	}else{
		lLocalChamberMax = MAX_CHAMB_OLD;
		lLocalChamberMin = MIN_CHAMB_OLD;
	}

	if(flgServiceChamber_lChamberMax[ch]) lLocalChamberMax = ServiceChamber_lChamberMax[ch];
	if(flgServiceChamber_lChamberMin[ch]) lLocalChamberMin = ServiceChamber_lChamberMin[ch];

	//assume not over range
	//measurement[ch].over_flag = FALSE;

	//if no valid measurement, just return
	if(!measurement[ch].valid_flag) return;

	if(measurement[ch].changed != 0) return;

	//assume not over range
	measurement[ch].over_flag = FALSE;

	// assume no gain change
	measurement[ch].gain_change = FALSE;

	// no gain changing if doing zero adjust or testing voltage
	if(measurement[ch].mode == ZEROMODE || measurement[ch].mode == TESTMODE) return;

	//get chamber reading
	lAdcZero = chamber[ch].adc_zero;
	if(flgServiceChamber_lADCZero[ch]) lAdcZero = ServiceChamber_lADCZero[ch];
	lADCZero = lAdcZero;
	lLocalADCValue = measurement[ch].adc_value;
	if (flgServiceChamber_lADCValue[ch]) lLocalADCValue = ServiceChamber_lADCValue[ch];
	chamb = lLocalADCValue - lAdcZero;
	if (flgServiceChamber_lChamberValue[ch]) chamb = ServiceChamber_lChamberValue[ch];
	lChamberValue = chamb;
	value = lLocalADCValue;
	lADCValue = value;
        
	//determine mininum or maximum conditions
	min_flag = max_flag = FALSE; //assume in range
	if(measurement[ch].polarity == 0){ //positive numbers
		if(chamber_one_gain_relay(ch)){
			if((value & 0x40000) != 0){
				max_flag = TRUE;
				measurement[ch].adc_value = 262143; //max positive value
			}else{
				if(chamb > lLocalChamberMax) max_flag = TRUE;
				if(chamb < lLocalChamberMin) min_flag = TRUE;
			}
		}else{
			if((value & 0x8000) != 0){
				max_flag = TRUE;
				measurement[ch].adc_value = 32767; //max positive value
			}else{
				if(chamb > lLocalChamberMax) max_flag = TRUE;
				if(chamb < lLocalChamberMin) min_flag = TRUE;
			}
		}
	}else{    //negative polarity, always below minimum
		if(chamb > lLocalChamberMax) max_flag = TRUE;
		if(chamb < lLocalChamberMin) min_flag = TRUE;
	}

	lChamberMax = lLocalChamberMax;
	lChamberMin = lLocalChamberMin;
	ucMinFlag = min_flag;
	ucMaxFlag = max_flag;

	PushServiceChamber(ucChamber, lChamberMax, lChamberMin, ucMinFlag, ucMaxFlag, lADCValue, lChamberValue, lADCZero);

	// max_flag, increment gain if possible
	if(max_flag){
		//if gain already 2, over-range
		if(measurement[ch].gain == 2){
			measurement[ch].over_flag = TRUE;
			if(measurement[ch].over_flag_count < 2){
				measurement[ch].over_flag_count++; // The over_flag_count is used for gain stage switching deglitching. ie The first overrange measurement might be due to a spike and not real
			}
			return;
		}

		// set Gain Change, increment gain, send to hardware
		if (chamber_one_gain_relay(ch)){
			measurement[ch].gain_change = 1;
			measurement[ch].gain = 2;
			setchamber(ch,measurement[ch].gain);
		}else{
			measurement[ch].gain_change = 1;
			++measurement[ch].gain;
			setchamber(ch,measurement[ch].gain);
		}
	}

	// if min_flag, decrement gain if possible
	if(min_flag){
		//can decrement gain if gain = 2 or dogain = 1
		if(measurement[ch].gain > 0){
			if(measurement[ch].dogain == 1){
				//set Gain Change, decrement gain,send to hardware 
				if(chamber_one_gain_relay(ch)){
					measurement[ch].gain_change = -1;
					measurement[ch].gain = 0;
					setchamber(ch,measurement[ch].gain);
				}else{
					measurement[ch].gain_change = -1;
					--measurement[ch].gain;
					setchamber(ch,measurement[ch].gain);
				}
			}
		}
	}
}

//linearity correction values
#define LINCOR  0.005   //.5%
#define MIN_LIN  1650.0   //mininum value for correction: ~1/3 Ci Tc99m for R
#define LIN_BASE 5000.0  //correction = LINCOR here: ~1Ci Tc99m for R
/**
 * \details Calculate the voltage from the ADC reading by subtracting Zero, Background and adjusting for the Gain
 * \param ch Chamber number
 * \returns True = Valid Calculation, False = ADC is not valid, so no calculation
 */
bool getchambervolts(short ch){
	char gain1;	/* gain when measurement made
				before any gain change */
	float volts;
	float double_volts;
	double dValue;
	unsigned char ucChamber;
	float fADCValue;
	float fADCZero;
	float fAppliedADCZero;
	float fChamberZero;
	float fAppliedChamberZero;
	float fChamberZeroCorr;
	float fChamberZeroCorrApplied;
	float fApplied18Bit;
	unsigned char ucGain;
	float fGainFactor;
	float fAppliedGain;
	float fAppliedLinCor;
	float fChamberBkg;
	float fAppliedBkg;
	double dADCZero;
	float fLocalChamberZero;
	float fLocalChamberBkg;
	double dLocalGainFactor;
	double dLocalChamberZeroCorr;

	ucChamber = (unsigned char) ch;
	if(chamber[ch].connected_flag == FALSE) return FALSE;
	if(measurement[ch].valid_flag == FALSE) return FALSE;

	/* only use value if changed = 0 */
	switch(measurement[ch].changed){
		case 0:
			break;
		case 1:
		case 2:
		case 3:
			++measurement[ch].changed;
			return FALSE;
		case 4:
			measurement[ch].changed = 0;
			return FALSE;
	}

	gain1 = measurement[ch].gain;

	/* Gain was increased */
	if(measurement[ch].gain_change == 1){
		measurement[ch].changed = 1;    //added 4/28
		/* set dogain to minimum*/
		measurement[ch].dogain = 1;
		/* return gain1 to previous gain */
		if(chamber_one_gain_relay(ch)){
			gain1 = 0;
		}else{
			if(gain1==1 || gain1==2) --gain1;
		}
	}

	/* Gain was decreased */
	if(measurement[ch].gain_change == -1){
		measurement[ch].changed = 1;
		measurement[ch].nmeas = 0;
		if(chamber_one_gain_relay(ch)){
			gain1 = 2;
		}else{
			if(gain1==0 || gain1==1) ++gain1;
		}
	}

	/* measurement was Over Range */
	if(measurement[ch].over_flag) return TRUE;

	/* get volts from chamber reading and zero offset */
	dValue = measurement[ch].adc_value;
	if(flgChamberVolts_fADCValue[ch]) dValue = ChamberVolts_fADCValue[ch];
	fADCValue = (float) dValue;
	dADCZero = chamber[ch].adc_zero;
	if(flgChamberVolts_fADCZero[ch]) dADCZero = ChamberVolts_fADCZero[ch];
	fADCZero = (float) dADCZero;
	dValue = dValue - dADCZero;
	if(flgChamberVolts_fAppliedADCZero[ch]) dValue = ChamberVolts_fAppliedADCZero[ch];
	fAppliedADCZero = (float) dValue;
	volts = dValue;

	fLocalChamberZero = chamber[ch].zero;

	if(flgChamberVolts_fChamberZero[ch]) fLocalChamberZero = ChamberVolts_fChamberZero[ch];
	volts -= fLocalChamberZero;

	if(flgChamberVolts_fAppliedChamberZero[ch]) volts = ChamberVolts_fAppliedChamberZero[ch];
	fChamberZero = fLocalChamberZero;
	fAppliedChamberZero = volts;

	dLocalChamberZeroCorr = 0.0;
	if(flgChamberVolts_fChamberZeroCorr[ch]) dLocalChamberZeroCorr = ChamberVolts_fChamberZeroCorr[ch];

	// Removed Low Gain Op Amp Offset Correction
	//dValue = volts;
	//dValue = dValue - dLocalChamberZeroCorr;
	//volts = dValue;
	//if(flgChamberVolts_fAppliedChamberZeroCorr[ch]) volts = ChamberVolts_fAppliedChamberZeroCorr[ch];
	fChamberZeroCorr = dLocalChamberZeroCorr;
	fChamberZeroCorrApplied = volts;

	if(chamber_one_gain_relay(ch)) volts /= 8.;

	if(flgChamberVolts_fApplied18Bit[ch]) volts = ChamberVolts_fApplied18Bit[ch];
	fApplied18Bit = volts;

	if(flgChamberVolts_ucGain[ch]) gain1 = ChamberVolts_ucGain[ch];
        
	/* apply gain */
	ucGain = (unsigned char) gain1;
	if(chamber_one_gain_relay(ch)){
		if(gain1 == 0){
			if(chamber_stored_gain_factor(ch)){
				dLocalGainFactor = chamber_gain_factor_0(ch);
				if(flgChamberVolts_fGainFactor[ch]) dLocalGainFactor = ChamberVolts_fGainFactor[ch];
				double_volts = volts;
				double_volts /= dLocalGainFactor;
				volts = double_volts;
				fGainFactor = (float) dLocalGainFactor;
			}else{
				dLocalGainFactor = 1000;
				if (flgChamberVolts_fGainFactor[ch]) dLocalGainFactor = ChamberVolts_fGainFactor[ch];
				volts /= dLocalGainFactor;
				fGainFactor = (float) dLocalGainFactor;
			}
		}else{
			dLocalGainFactor = 1;
			if (flgChamberVolts_fGainFactor[ch]) dLocalGainFactor = ChamberVolts_fGainFactor[ch];
			volts /= dLocalGainFactor;
			fGainFactor = (float) dLocalGainFactor;
		}
	}else{
		if(chamber_stored_gain_factor(ch)){
			if(gain1 == 0){
				dLocalGainFactor = chamber_gain_factor_2(ch);
				if (flgChamberVolts_fGainFactor[ch]) dLocalGainFactor = ChamberVolts_fGainFactor[ch];
				double_volts = volts;
				double_volts /= dLocalGainFactor;
				volts = double_volts;
				fGainFactor = (float) dLocalGainFactor;
			}else if(gain1 == 1){
				dLocalGainFactor = chamber_gain_factor_1(ch);
				if (flgChamberVolts_fGainFactor[ch]) dLocalGainFactor = ChamberVolts_fGainFactor[ch];
				double_volts = volts;
				double_volts /= dLocalGainFactor;
				volts = double_volts;
				fGainFactor = dLocalGainFactor;
			}else{
				dLocalGainFactor = 1;
				if (flgChamberVolts_fGainFactor[ch]) dLocalGainFactor = ChamberVolts_fGainFactor[ch];
				volts /= dLocalGainFactor;
				fGainFactor = (float) dLocalGainFactor;
			}
		}else{
			if(gain1 == 0){
				dLocalGainFactor = 10000;
				if (flgChamberVolts_fGainFactor[ch]) dLocalGainFactor = ChamberVolts_fGainFactor[ch];
				volts /= dLocalGainFactor;
				fGainFactor = dLocalGainFactor;
			}else if(gain1 == 1){
				dLocalGainFactor = 100;
				if (flgChamberVolts_fGainFactor[ch]) dLocalGainFactor = ChamberVolts_fGainFactor[ch];
				volts /= dLocalGainFactor;
				fGainFactor = dLocalGainFactor;
			}else{
				dLocalGainFactor = 1;
				if (flgChamberVolts_fGainFactor[ch]) dLocalGainFactor = ChamberVolts_fGainFactor[ch];
				volts /= dLocalGainFactor;
				fGainFactor = (float) dLocalGainFactor;
			}
		}
	}
	if (flgChamberVolts_fAppliedGain[ch]) volts = ChamberVolts_fAppliedGain[ch];
	fAppliedGain = volts;

	/* apply linearity correction if volts >= MIN_LIN
		when volts = LIN_BASE, correction = LINCOR */
	if(!chamber_77t()){
		if (volts >= MIN_LIN) volts = volts * (1. + LINCOR * volts / LIN_BASE);
	}
	if (flgChamberVolts_fAppliedLinCor[ch]) volts = ChamberVolts_fAppliedLinCor[ch];
	fAppliedLinCor = volts;

	/* subtract background */
	fLocalChamberBkg = chamber[ch].bkg;

	if (gain1 == 0) fLocalChamberBkg = chamber[ch].bkg;
	else fLocalChamberBkg = 0;

	if (flgChamberVolts_fChamberBkg[ch]) fLocalChamberBkg = ChamberVolts_fChamberBkg[ch];
	volts -= fLocalChamberBkg;
	if (flgChamberVolts_fAppliedChamberBkg[ch]) volts = ChamberVolts_fAppliedChamberBkg[ch];
	fChamberBkg = fLocalChamberBkg;
	fAppliedBkg = volts;

	PushChamberVolts(ucChamber, fADCValue, fADCZero, fAppliedADCZero, fChamberZero, fAppliedChamberZero, fChamberZeroCorr, fChamberZeroCorrApplied, fApplied18Bit, ucGain, fGainFactor, fAppliedGain, fAppliedLinCor, fChamberBkg);
	PushChamberVolts2(ucChamber, fAppliedBkg);

	/* save in chamber structure */
	measurement[ch].volts = volts;
	measurement[ch].prevgain = gain1;

	return TRUE;
}

//chamber not connected
short errchamb(short ch_num){
	char ch;

	if(ch_num != current.main_chamber) return 0;

	erase_screen();
	display_text(40,0,"ERROR",0,BIG,NORMAL);
	display_text(10,16,"CHAMBER NOT",0,BIG,NORMAL);
	display_text(10,36,"CONNECTED",0,BIG,NORMAL);

	for(;;){
		ch = keyin();
		switch(ch){
			case MENUBUT:
				return -1;

			case CHAMB_KEY:
				if(current.num_chambers > 1) return 1;
                //else falls thru to default

			default:
				beep();
				break;
		}
	}
}

extern CURRENT current;
/**
 * \details Display over-range message
 * \param ch_num Chamber number
 * \returns None
 */
void overflow(short ch_num){
	if(ch_num != current.main_chamber) return;

	if(measurement[ch_num].over_flag_count >= 2){
		if(DisplayActivity_SendActivity){
			send_amulet_message(L_OVERRANGE, 12);    // "OVER RANGE"
			send_amulet_message(L_DOSE_DECAY, 6);    // "Dose Decay"
			measurement[ch_num].future.dosetime = NO_TIME; // Only reset future dose when two consecutive overrange measurements. This is to deglitch the spike for going from high gain to low gain
		}
	}

	if(measurement[ch_num].wasover) return;

	measurement[ch_num].wasover = TRUE;
	erase_lines(16,22,0);
	display_text(10,22,"OVER RANGE",0,BIG,NORMAL);
	if(DisplayActivity_SendActivity){
		send_to_amulet_string(10, "");
		send_to_amulet_string(11, "");
		send_to_amulet_string(7, "");
	}

	// Removed:
	//measurement[ch_num].syst0 = -1;
	measurement[ch_num].kun0 = 0;
	measurement[ch_num].act0 = 0.;
	strcpy(&measurement[ch_num].actstr0[0],"xxxxxx");
}
