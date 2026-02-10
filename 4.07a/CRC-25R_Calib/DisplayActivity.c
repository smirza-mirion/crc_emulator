/**
 * \file
 * \details This file contains functions, which calculates and displays activity.
 */

/*********************************************************************
  MODULE:	DISPLAY ACTIVITY

  FILE:		   DisplayActivity.c

  DATE:         01/03/07

  ANALYSIS: 	calculates and displays activity, current time
				and decayed activity if requested


  *************************************************************************/

//#define SIMULATE_HALF_LIFE_CALC_SOURCE 1

#ifdef SIMULATE_HALF_LIFE_CALC_SOURCE
	#define SIM_HALF_LIFE_INITIAL_VOLTAGE 33000
	#define SIM_HALF_LIFE_RATE	(30)
	//#define SIMULATE_HALF_LIFE_CALC_NOISE 1
	//#define SIMULATE_HALF_LIFE_GAIN_CHANGE 1
#endif

#include "crc.h"
#include "screen.h"
#include "keyboard.h"
#include "i2c.h"
#include "chambfac.h"
#include "remote.h"
#include "uart.h"
#include "lowlevelsnoop.h"
#include "amulet.h"
#include "message.h"
#ifdef SIMULATE_DECAY_SOURCE
	#include "nuc.h"
	#include <stdlib.h>
#endif
#include <string.h>
#include <math.h>
#ifdef SIMULATE_HALF_LIFE_CALC_NOISE
	#include <stdlib.h>
#endif
#define MAX_ACT_CI_R    10.
#define MAX_ACT_CI_P    1000000.
#define MAX_ACT_CI_UNLIMIT    1000000.
volatile bool DisplayActivity_SendActivity = FALSE;
volatile bool DisplayActivity_SendActivity2 = FALSE;
volatile short DisplayActivity_addedDigits = 0;

	extern const float unitfact[],upci[],upci77t[],downci[],downci77t[],upbq[],downbq[];
	extern const short ndecci[],ndecbq[],ndec77tci[];
	extern CURRENT current;
	extern CHAMBER chamber[];
    extern REMOTE remote[];
	extern MEASUREMENT measurement[];
	extern HALFLIFECALC HalflifeCalc_test;
#ifdef SIMULATE_DECAY_SOURCE
	extern time_t clock_time;
#endif
#ifdef TERMINAL
	extern int qspi_sim_adc_gain[2];
#endif // #ifdef TERMINAL

    static void calc_disp_dec(float act);
    void trim_and_shrink(char *acByte);
    void send_to_amulet_string(uchar ucIndex, char message0[]);
/**
 * \details Convert the meas field in the MEASUREMENT structure into act0 (Floating point value of Activity) and actstr (String value of Activity)
 * \param ch_num Chamber number
 * \returns None
 */
void display_activity(short ch_num){
	short ii, jj, kun, idec, idecmin, ndec, type, ch_type;
	float act_ci, act_bq, actfact, absactiv_ci, absactiv_bq, max_act_ci, absact0_ci, absact0_bq;
	bool zflag;
	char actstr[12];
	unsigned char ucChamber;
	float fVolts, fActFact, fResponse, fActivity, fContainerFactor, fAppliedContainerFactor;
	float fLocalVolts, fLocalResponse, fLocalContainerFactor;
#ifdef SIMULATE_DECAY_SOURCE
	struct tm begin;
	time_t decay_begin;
	int nuclideIndex;
	int value;
	float floatvalue;
#endif

	ch_type = chamber_type(ch_num);
	ucChamber = (unsigned char) ch_type;

	//only do this function if chamber is in measurement mode
	if(measurement[ch_num].mode != MEASMODE) return;

	if(measurement[ch_num].over_flag){
		//send data to remote structure if remote exists
		if(remote[ch_num].exists) remote_activity(ch_num);
		measurement[ch_num].display_flag_2 = TRUE;
		return;
	}

	//max activity for R or PET
	if(ch_type == R_CHAMB) max_act_ci = MAX_ACT_CI_R;
	else if(ch_type == P_CHAMB) max_act_ci = MAX_ACT_CI_P;
	else max_act_ci = MAX_ACT_CI_UNLIMIT;

	idec = measurement[ch_num].idec;
	idecmin = measurement[ch_num].idecmin;

#ifdef SIMULATE_DECAY_SOURCE
	if((current.demo_mode) && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER)){
		begin.tm_sec = 0;	// 0 - 59
		begin.tm_min = 0;	// 0 - 59
		begin.tm_hour = 0;	// 0 - 23
		begin.tm_mday = 20;
		begin.tm_mon = 11;	// 0 - 11
		begin.tm_year = 111;	// 1900
		begin.tm_wday = 0;
		begin.tm_yday = 351;
		begin.tm_isdst = 0;

		nuclideIndex = NuclideData_getIndexFromNameIncludingUser("F 18  ");
		decay_begin = mktime(&begin);
		act_ci = nucdecay(1.0, decay_begin, clock_time, NuclideData_getHalflife(nuclideIndex),NuclideData_getHalflifeUnit(nuclideIndex));
		srand(clock_time);
		value = rand();
		value = value % 65535;
		value = value - 32768;
		floatvalue = value;
		floatvalue = floatvalue / 32768.0 * .15;
		floatvalue = 1.0 + floatvalue;
		act_ci *= floatvalue;
	}
#else
#ifndef TERMINAL
	if((current.demo_mode) && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER)) act_ci = 0.;
#else // #ifndef TERMINAL
	if(current.demo_mode && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER) && qspi_sim_adc_gain[ch_num] == -1) act_ci = 0.;
#endif // #ifndef TERMINAL
#endif
	else{
		actfact = get_gainfact(ch_type);
		// Locked: CI
		//if(current.system == BQ)
		//    actfact *= BQFACTOR;

		fLocalVolts = measurement[ch_num].meas;
		if(flgDisplayActivity_fVolts[ch_num]) fLocalVolts = DisplayActivity_fVolts[ch_num];
		fLocalResponse = measurement[ch_num].response;
		if(flgDisplayActivity_fResponse[ch_num]) fLocalResponse = DisplayActivity_fResponse[ch_num];
		if(flgDisplayActivity_fActFact[ch_num]) actfact = DisplayActivity_fActFact[ch_num];
		act_ci =  fLocalVolts * actfact / fLocalResponse;
		if(flgDisplayActivity_fActivity[ch_num]) act_ci = DisplayActivity_fActivity[ch_num];
		fVolts = fLocalVolts;
		fActFact = actfact;
		fResponse = fLocalResponse;
		fActivity = act_ci;
	}

	//container factor
	type = chamber[ch_num].type;
	if(type != REF){
		fLocalContainerFactor = chamber[ch_num].calcor.corr[type];
		if(flgDisplayActivity_fContainerFactor[ch_num]) fLocalContainerFactor = DisplayActivity_fContainerFactor[ch_num];
		act_ci *= fLocalContainerFactor;
		fContainerFactor = fLocalContainerFactor;
	}else{
		fLocalContainerFactor = 1;
		if(flgDisplayActivity_fContainerFactor[ch_num]) fLocalContainerFactor = DisplayActivity_fContainerFactor[ch_num];
		act_ci *= fLocalContainerFactor;
		fContainerFactor = fLocalContainerFactor;
	}
	if (flgDisplayActivity_fAppliedContainerFactor[ch_num]) act_ci = DisplayActivity_fAppliedContainerFactor[ch_num];
	fAppliedContainerFactor = act_ci;
	PushDisplayActivity(ucChamber, fVolts, fActFact, fResponse, fActivity, fContainerFactor, fAppliedContainerFactor);

	absactiv_ci = fabs(act_ci);

	if (absactiv_ci > max_act_ci){
		measurement[ch_num].over_flag = TRUE;
		measurement[ch_num].display_flag_2 = TRUE;
		if(measurement[ch_num].over_flag_count < 2){
			measurement[ch_num].over_flag_count++; // The over_flag_count is used for gain stage switching deglitching. ie The first overrange measurement might be due to a spike and not real
		}
		return;
	}

	measurement[ch_num].over_flag_count = 0; // Reset the counter to zero when activity is valid.

	//turn off wasover
	measurement[ch_num].wasover = FALSE;

	kun = 0;
	/* CURIES */
	if(current.system == CI){
		absact0_ci = fabs(measurement[ch_num].act0);
		if(absactiv_ci > absact0_ci){
			if(chamber_77t()){
				jj = 7;
				for(ii=idec; ii<8; ii++){
					if(absactiv_ci<=upci77t[ii]){
						jj = ii;
						break;
					}
				}
			}else{
				jj = 8;
				for(ii=idec; ii<9; ii++){
					if(absactiv_ci<=upci[ii]){
						jj = ii;
						break;
					}
				}
			}
		}else{
			jj = idec;
			if(idec>idecmin){
				for(ii=idec-1; ii>=idecmin; ii--){
					if(chamber_77t()){
						if (absactiv_ci >= downci77t[ii]) break;
					}else{
						if (absactiv_ci >= downci[ii]) break;
					}
					jj = ii;
				}
			}
		}

		if(chamber_77t()) ndec = ndec77tci[jj];
		else ndec = ndecci[jj];
		if(chamber_77t()){
			kun = MCI;
			if (jj > 3) kun = _CI;
			zflag = jj == 0;
		}else{
			kun = UCI;
			if (jj > 2) kun = MCI;
			if (jj > 6) kun = _CI;
			zflag = jj == 0;
		}
		idec = jj;
	}

	/* BQ */
	if(current.system == BQ){
		act_bq = act_ci * BQFACTOR;
		absactiv_bq = fabs(act_bq);
		absact0_bq = measurement[ch_num].act0 * BQFACTOR;
		absact0_bq = fabs(absact0_bq);

		if(absactiv_bq > absact0_bq){
			jj = 6;
			for(ii=idec; ii<7; ii++){
				if(absactiv_bq<=upbq[ii]){
					jj = ii;
					break;
				}
			}
		}else{
			jj = idec;
			if(idec>idecmin){
				for(ii=idec-1; ii>=idecmin; ii--){
					if (absactiv_bq >= downbq[ii]) break;
					jj = ii;
				}
			}
		}
		ndec = ndecbq[jj];
		kun = MBQ;
		if (jj > 3) kun = GBQ;
		zflag = FALSE;
		idec = jj;
		}

	measurement[ch_num].idec = idec;
	measurement[ch_num].kun = kun;
	measurement[ch_num].act0 = act_ci;

	if(current.system == BQ){
		act_bq *= unitfact[kun - 1];
		if(zflag && act_bq >= 1.) zflag = FALSE;
		act2strnew(act_bq, actstr, ndec, zflag, kun);
	}else{
		act_ci *= unitfact[kun - 1];
		if(zflag && act_ci >= 1.) zflag = FALSE;
		act2strnew(act_ci, actstr, ndec, zflag, kun);
	}

	// Removed:
	//measurement[ch_num].act = act;

	strcpy(&measurement[ch_num].actstr[0],actstr);

	if(ndec != measurement[ch_num].ndec0) strcpy(&measurement[ch_num].actstr0[0],"xxxxxxxxx");

	// set display ready flag
	measurement[ch_num].display_flag = TRUE;
	measurement[ch_num].display_flag_2 = TRUE;

	//send data to remote structure if remote exists
	if(remote[ch_num].exists) remote_activity(ch_num);

	measurement[ch_num].ndec = ndec;
	// Locked: CI
	//measurement[ch_num].syst = current.system;
	// Removed:
	//measurement[ch_num].syst = CI;

	if(current.main_chamber != ch_num) return;

	if (!input_time_set() && (measurement[ch_num].future.dosetime != NO_TIME)) calc_disp_dec(measurement[ch_num].act0);
}

    extern time_t clock_time;
/**
 * \details Calculate the future activity with the parameter act, current time, and dose time (MEASUREMENT.future.dosetime). The result is saved in MEASUREMENT.future.act0
 * \returns None
 */
    static void calc_disp_dec(float act)
    {
        time_t time0,time1;
        short ch_num = current.main_chamber;
        float hl;
        short hlunit;

        read_clock(&clock_time);     // get current time
        low_clock_time = clock_time;
        //reset_minute_counter();
        reset_minute_counter_with_seconds();
        time0 = clock_time;
        measurement[ch_num].future.meastime = time0;       //save measurement time

        if(act >= 0.)
        {
            // calculate activity at dose time
            time1 = measurement[ch_num].future.dosetime;
            hl = chamber[ch_num].nucdata.halflife;
            hlunit = chamber[ch_num].nucdata.hlunit;

            measurement[ch_num].future.act0 = nucdecay(act, time0, time1, hl, hlunit);
        }else{
        	measurement[ch_num].future.act0 = -1.0;
        }
    }

//prefixes
static const char prefix[] = " $m kMG";    //$ = mu

static void disp_future_act(void);
/**
 * \details Separate the Activity String, which has both the number and activity unit into a number string and a activity units string
 * \param act Pointer to Null terminated activity string
 * \param numbers Pointer to byte array, which receives the number string
 * \param units Pointer to byte array, which receives the activity unit string
 * \returns None
 */
	void splitoutunits(char *act, char *numbers, char *units){
		short index;
		char *activitystring;
		char *numbersstring;
		char *unitstring;

		numbersstring = numbers;
		unitstring = units;
		for(index=0; index<10; index++){
			*numbersstring = 0;
			numbersstring++;
			*unitstring = 0;
			unitstring++;
		}

		activitystring = act;
		numbersstring = numbers;
		unitstring = units;
		for(index=0; index<10; index++){
			if(*activitystring == 0){
				break;
			}else if((*activitystring == '$') ||
			   (*activitystring == 'C') ||
			   (*activitystring == 'i') ||
			   (*activitystring == 'm') ||
			   (*activitystring == 'k') ||
			   (*activitystring == 'B') ||
			   (*activitystring == 'q') ||
			   (*activitystring == 'M') ||
			   (*activitystring == 'G')
			  ){
				*unitstring = *activitystring;
				unitstring++;
			}else{
				*numbersstring = *activitystring;
				numbersstring++;
			}
			activitystring++;
		}
	}

    //display measured activity on screen
/**
 * \details Display measured activity on screen, displays the MEASUREMENT.actstr to the screen
 * \returns None
 */
    void activity_to_screen(void)
    {
        short xpos;
        short ypos;
        short yposp;
        short xposp;
        char str[8];
        char actstr[10],actstr0[10];
        char actamulet[10];
        char actonlyamulet[10];
        char unitonlyamulet[10];
        char message[25];
        short ch_num = current.main_chamber;
        int i;
        char ch;
        bool erase_flag;

        ypos = 16;
        xposp = 84;
        strcpy(actstr,&measurement[ch_num].actstr[0]);

        strcpy(actamulet, actstr);
        trim_and_shrink(actamulet);

        if(DisplayActivity_SendActivity){
        	if(measurement[ch_num].over_flag){
        		send_to_amulet_string(10, "");
        		send_to_amulet_string(11, "");
        		send_to_amulet_string(7, "");
				if(measurement[ch_num].over_flag_count >= 2){
					send_amulet_message(L_OVERRANGE, 12);    // "OVER RANGE"
					send_amulet_message(L_DOSE_DECAY, 6);    // "Dose Decay"
					measurement[ch_num].future.dosetime = NO_TIME; // Only reset future dose when two consecutive overrange measurements. This is to deglitch the spike for going from high gain to low gain
				}
        	}else{
        		splitoutunits(actamulet, actonlyamulet, unitonlyamulet);
        		send_to_amulet_string(10, actonlyamulet);
        		send_to_amulet_string(11, unitonlyamulet);
        		send_to_amulet_string(12, "");

        		if(measurement[ch_num].future.dosetime != NO_TIME){
        			strcpy(message, measurement[ch_num].future.actstr);
        			trim_and_shrink(message);
        			send_to_amulet_string(7, message);
        		}else{
        			send_amulet_message(L_DOSE_DECAY, 6);    // "Dose Decay"
        			send_to_amulet_string(7,"");
        		}
        	}
        }

        strcpy(actstr0,&measurement[ch_num].actstr0[0]);

        erase_flag = FALSE;
        if(actstr0[0] == 'x')
        {
            erase_lines(ypos,22,0);
            erase_flag = TRUE;
        }

        str[1] = '\0';
        if(actstr[0] != actstr0[0])
        {
            xpos = 0;
            str[0] = ' ';
            if (actstr[0] == '-')
                str[0] = '-';
            display_text(xpos,ypos,str,0,VBIG,NORMAL);
        }
        xpos = -4;


        for(i = 1; i <= 5; i++)
        {
            xpos += 16;
            ch = actstr[i];
            if(ch != actstr0[i])
            {
                str[0] = ch;
                display_text(xpos,ypos,str,0,VBIG,NORMAL);
            }

            if(ch == '.')
                xpos -= 8;
        }

        //prefix
        xpos = xposp;
        if((measurement[ch_num].kun != measurement[ch_num].kun0 || erase_flag))
        {
            str[0] = prefix[measurement[ch_num].kun];
            yposp = ypos;
            if (measurement[ch_num].kun == 1)   //mu
                yposp += 2;
            display_text(xpos,yposp,str,0,VBIG,NORMAL);
        }

	// Removed:
        //if((measurement[ch_num].syst != measurement[ch_num].syst0) || erase_flag)
	if(erase_flag)
        {
            //Ci or Bq
            if(current.system == CI)
                strcpy(str,"Ci");
            else
                strcpy(str,"Bq");


            xpos += 16;
            display_text(xpos,ypos,str,0,VBIG,NORMAL);
            xpos += 14;
            display_text(xpos,ypos,&str[1],0,VBIG,NORMAL);
        }

        //display future activity if future time set
        if (!input_time_set() && measurement[ch_num].future.dosetime != NO_TIME)
            disp_future_act();

        //now make these values the previous values
        measurement[ch_num].ndec0 = measurement[ch_num].ndec;
        strncpy(&measurement[ch_num].actstr0[0],&measurement[ch_num].actstr[0],9);
        measurement[ch_num].kun0 = measurement[ch_num].kun;
	// Removed:
        //measurement[ch_num].syst0 = measurement[ch_num].syst;
    }

    extern const float unitfact[];
    //display future activity on screen
/**
 * \details Display future activity on screen, displays the MEASUREMENT.future.actstr to the screen
 * \returns None
 */
    static void disp_future_act(void)
    {
        float act;
        char ustr[4];
        char strng[12];
        bool over_flag;
        short ch_num = current.main_chamber;
        //float act0;

        //act0 = measurement[ch_num].act0;

        erase_lines(YPROMPT, 64 - YPROMPT,0);

        //if(act0 > 0.)
        //{
            act = measurement[ch_num].future.act0;
            over_flag = FALSE;

            // Locked: CI
            //if(current.system == CI && act > 10. ||
            //   current.system == BQ && act > 1.0e+12)
            //    over_flag = TRUE;
            if(act > 10.) over_flag = TRUE;

            if(over_flag)
            {
                //strcpy(measurement[ch_num].future.actstr," OVER ");
            	get_amulet_message(L_OVER, measurement[ch_num].future.actstr);    // " OVER "
                strcpy(ustr,"   ");
            }
            else{
            	// Locked: CI
                //format_activity(act,current.system,measurement[ch_num].future.actstr);
            	if(act >= 0.0) format_activity_system(act, measurement[ch_num].future.actstr);
            	else{
            		strncpy(measurement[ch_num].future.actstr, "            ", 12);
            		strcpy(measurement[ch_num].future.actstr, "******   ");
            	}
            }

            strcpy(strng,&measurement[ch_num].future.actstr[1]);

            display_text(X_FUT_ACT, Y_FUT_TIME, strng,0,MEDIUM,NORMAL);
        //}


        //display date if dose date != measurement date
        if(!same_day(&measurement[ch_num].future.meastime, &measurement[ch_num].future.dosetime))
        {
            dateout_language(strng, &measurement[ch_num].future.dosetime, 4);
            display_text(X_TIME - 48,Y_FUT_DATE,strng,0,MEDIUM,NORMAL);
        }

        timeout(strng,&measurement[ch_num].future.dosetime);
        display_text(X_TIME,Y_FUT_TIME,strng,0,MEDIUM,NORMAL);



    }
/**
 * \details Compare the date part of two datetime variables
 * \param time0 First datetime for comparision
 * \param time1 Second datetime for comparison
 * \returns True = Date is the same, False = Date is different
 */
    bool same_day(time_t *time0, time_t *time1)
    {

        struct tm tm0,tm1;

        memcpy(&tm0,gmtime(time0),sizeof(tm0));
        memcpy(&tm1,gmtime(time1),sizeof(tm1));

        if (tm0.tm_mday!= tm1.tm_mday ||
            tm0.tm_mon != tm1.tm_mon ||
            tm0.tm_year != tm1.tm_year)
            return FALSE;

        return TRUE;



    }
/**
 * \details Update the HALFLIFECALC structure with the current time and activity
 * \returns None
 */
void acquire_halflife_calc_data(int ch_num){
#ifdef SIMULATE_HALF_LIFE_CALC_NOISE
	float random, random_limit;
#endif
#ifdef SIMULATE_HALF_LIFE_CALC_SOURCE
    float elapsedTimeFloatSec;
#endif
	int i, elapsedTimeSec, elapsedTimeMilli;
	int fillIndex = -1;
	unsigned long int elapsedTimeInMilli;

	if((HalflifeCalc_test.HalflifeCalcID == -2) && (ch_num == HalflifeCalc_test.ChamberNumber) && (!measurement[ch_num].gainchangecountdown)){
		if(HalflifeCalc_test.HalflifeCalcMeasurement[0].HalflifeCalcMeasurementID == -2){
#ifdef SIMULATE_HALF_LIFE_CALC_NOISE
			srand(HalflifeCalc_test.StartedOn);
#endif
			elapsedTimeInMilli = 0;
			elapsedTimeSec = 0;
			elapsedTimeMilli = 0;
			fillIndex = 0;
		}else if(HalflifeCalc_test.HalflifeCalcMeasurement[0].HalflifeCalcMeasurementID == -3){
			if(measurement[ch_num].ulMilliSecStamp <=  HalflifeCalc_test.HalflifeCalcMeasurement[0].MilliSecStamp){
				// Redo first measurement because the current first measurement was taken before clearing the MilliSecStamp
				elapsedTimeInMilli = 0;
				elapsedTimeSec = 0;
				elapsedTimeMilli = 0;
				fillIndex = 0;
				for(i=0; i<1209; i++){
					if(HalflifeCalc_test.HalflifeCalcMeasurement[i].HalflifeCalcMeasurementID == -3){
						HalflifeCalc_test.HalflifeCalcMeasurement[i].HalflifeCalcMeasurementID = -2;
					}
					HalflifeCalc_test.HalflifeCalcMeasurement[i].Status = -1;
				}
			}else{
				elapsedTimeInMilli = measurement[ch_num].ulMilliSecStamp - HalflifeCalc_test.HalflifeCalcMeasurement[0].MilliSecStamp;
				elapsedTimeSec = elapsedTimeInMilli / 1000;
				elapsedTimeMilli = elapsedTimeInMilli % 1000;
				for(i = 1; i<1209; i++){
					if(HalflifeCalc_test.HalflifeCalcMeasurement[i].HalflifeCalcMeasurementID == -2){
						if(
						   (HalflifeCalc_test.HalflifeCalcMeasurement[i].TargetElapsedSecond <= elapsedTimeSec) &&
							  (
							   ((HalflifeCalc_test.HalflifeCalcMeasurement[i+1].HalflifeCalcMeasurementID == -2) && (HalflifeCalc_test.HalflifeCalcMeasurement[i+1].TargetElapsedSecond > elapsedTimeSec)) ||
							   (HalflifeCalc_test.HalflifeCalcMeasurement[i+1].HalflifeCalcMeasurementID == -1)
							  )
						  ){
							fillIndex = i;
							break;
						}
					}
				}
			}
		}

		if(fillIndex != -1){
#ifdef SIMULATE_HALF_LIFE_GAIN_CHANGE
			if(fillIndex == 3){
				setchamber(ch_num, measurement[ch_num].gain);
			}
#endif
			if((fillIndex == 0) || (HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex - 1].MilliSecStamp != measurement[ch_num].ulMilliSecStamp)){
				if(fillIndex == 0) HalflifeCalc_test.InitialGain = measurement[ch_num].gain;

				if(measurement[ch_num].over_flag){
					HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Status = 1;
				}else{
#ifdef SIMULATE_HALF_LIFE_CALC_SOURCE
					elapsedTimeFloatSec = elapsedTimeMilli;
					elapsedTimeFloatSec /= 1000.0;
					elapsedTimeFloatSec += elapsedTimeSec;
					HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Voltage = SIM_HALF_LIFE_INITIAL_VOLTAGE * exp(-1 * (elapsedTimeFloatSec * log(2)) / SIM_HALF_LIFE_RATE);

#ifdef SIMULATE_HALF_LIFE_CALC_NOISE
					random = rand();
					random -= (0x7fff / 2);
					random_limit = (0x7fff/2);
					random /= random_limit;
					random *= .03;
					HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Voltage *= (1.0 + random);
#endif

					if(fillIndex == 0){
						if(chamber_type(HalflifeCalc_test.ChamberNumber) == R_CHAMB){
							// R Chamber
							if(chamber_one_gain_relay(HalflifeCalc_test.ChamberNumber)){
								// Two Stage
								if(HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Voltage < 25) HalflifeCalc_test.InitialGain = 0;
								else HalflifeCalc_test.InitialGain = 2;
							}else{
								// Three Stage
							}
						}else{
							// PET Chamber
							if(chamber_one_gain_relay(HalflifeCalc_test.ChamberNumber)){
								// Two Stage
								if(HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Voltage < 25) HalflifeCalc_test.InitialGain = 0;
								else HalflifeCalc_test.InitialGain = 2;
							}else{
								// Three Stage
							}
						}
					}
#else
					HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Voltage = measurement[ch_num].volts;
#endif

					if(HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Voltage > 0){
#ifdef SIMULATE_HALF_LIFE_CALC_SOURCE
						if(HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Voltage > 33000){
							HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Status = 1;
						}else{
							HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].CalcLnVoltage = log(HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Voltage);
							HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Status = 0;
						}
#else
						HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].CalcLnVoltage = log(HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Voltage);
						HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Status = 0;
#endif
					}else{
						HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].Status = 2;
					}
				}

				HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].MilliSecStamp = measurement[ch_num].ulMilliSecStamp;
				HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].ElapsedSecond = elapsedTimeSec;
				HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].ElapsedMilliSecond = elapsedTimeMilli;
				HalflifeCalc_test.HalflifeCalcMeasurement[fillIndex].HalflifeCalcMeasurementID = -3;
				HalflifeCalc_test.MeasurementCount = fillIndex + 1;
			}
		}
	}
}
