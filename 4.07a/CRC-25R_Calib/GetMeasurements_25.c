/**
 * \file
 * \details This file contains functions, which converts the raw ADC readings from the chamber to normalized current readings.
 */
/*********************************************************************
  MODULE:	GET MEASUREMENTS for CRC-25

  FILE:		GetMeasurements_25.c

  DATE:		10/04/06


  *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "daily.h"
#include "lowlevelsnoop.h"
#include <math.h>
	extern CHAMBER chamber[];
    extern MEASUREMENT measurement[];
    extern CURRENT current;
    extern short max_chambers;
#ifdef TERMINAL
    extern int qspi_sim_adc_gain[2];
#endif // #ifdef TERMINAL
    
static void clear_moving(float actbuf[], char gainbuf[]);
static void moving_average(short num, float actbuf[], float volts, char gainbuf[], char prevgain);
void acquire_halflife_calc_data(int ch_num);
/**
 * \details Master routine for coordinating all measurement foreground routines. This routine executes servicechamber, getchambervolts, low_act, low_mo_bkg, low_mo_act, read_bias, read_zero, ismeas, acquire_halflife_calc, display_activity
 * \returns None
 */
void get_measurements(void){ // Called by keypress loop
	bool chambok;
	short ch_num;
		
	for(ch_num = 0; ch_num < max_chambers; ch_num++){
		//skip if doesn't exist -- PET only
		if(!chamber[ch_num].exists) continue;

		//skip if not active -- PET only
		if(!chamber[ch_num].active) continue;

#ifdef SIMULATE_DECAY_SOURCE
		if(current.demo_mode && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER)) chambok = measurement[ch_num].valid_flag = TRUE;
#else
#ifndef TERMINAL
		if(current.demo_mode && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER)) chambok = measurement[ch_num].valid_flag;
#else // #ifndef TERMINAL
		if(current.demo_mode && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER) && qspi_sim_adc_gain[ch_num] == -1) chambok = measurement[ch_num].valid_flag;
#endif // #ifndef TERMINAL
#endif
		else{
			servicechamber(ch_num); //
			chambok = getchambervolts(ch_num);
		}
    
		if(chambok){
			//value no longer valid
			measurement[ch_num].valid_flag = FALSE;

			switch(measurement[ch_num].mode){
				case BKGMODE:
					low_act(ch_num);
                    break;

                case MOBKGMODE:
                    low_mo_bkg(ch_num);
                    break;

                case MOASSMODE:
                    low_mo_act(ch_num);
                    break;

                case TESTMODE:
                    read_bias(ch_num);
                    break;

                case ZEROMODE:
                    read_zero(ch_num);
                    break;

                default:
                    ismeas(ch_num);
                    acquire_halflife_calc_data(ch_num);
                    display_activity(ch_num);
        
                    if(measurement[ch_num].dogain > 0 && measurement[ch_num].autodu && measurement[ch_num].volts <= 0.1){
                    	++measurement[ch_num].ntimes;
                    	if(measurement[ch_num].ntimes == 7) nucinit(ch_num,FALSE);
                    }
          
                    if(measurement[ch_num].dogain == 0) doauto(ch_num);
			}
		}
	}
}
/**
 * \details Calculate boxcar average using the MEASUREMENT.actbuf[] data array and place the result into MEASUREMENT.meas
 * \returns None
 */
	void ismeas(short ch_num)
	{
		short i;
		short num;
		short avg_num;
		float diff;
		float sx;
		float avg;
	    short nmeas;

	    unsigned char ucChamber;
	    float lastvolts;
	    char lastgain;
	    float percentthres;
	    float absolutethres;
	    float threshold;
	    static char countZeroClear[8] = {0,0,0,0,0,0,0,0};

	    ucChamber = (unsigned char) ch_num;
			avg_num = 40;

		nmeas = measurement[ch_num].nmeas;

		if (nmeas != 0) {
			lastvolts = measurement[ch_num].actbuf[nmeas - 1];
			//printf("lastvolts: %f\n", lastvolts);
			lastgain = measurement[ch_num].gainbuf[nmeas -1];
			//printf("lastgain: %d\n", lastgain);
			//printf("volts: %f\n", measurement[ch_num].volts);
			if (!((measurement[ch_num].volts <= .1) && (lastvolts <= .1))) {
				percentthres = lastvolts * .05;
				if (lastgain == 0) {
					absolutethres = .03;
				}
				else {
					absolutethres = 3.0;
				}
				//printf("percentthres: %f\n", percentthres);
				//printf("absolutethres: %f\n", absolutethres);

				if (absolutethres > percentthres) threshold = absolutethres;
				else threshold = percentthres;

				//printf("threshold: %f\n", threshold);
				if (fabs(measurement[ch_num].volts - lastvolts) > threshold) {
					countZeroClear[ch_num] = 6;
				}
				//printf("countZeroClear: %d\n", countZeroClear);
			}

			if (countZeroClear[ch_num] != 0) {
				countZeroClear[ch_num] --;
				measurement[ch_num].nmeas = 0;
				nmeas = 0;
				clear_moving(&measurement[ch_num].actbuf[0], &measurement[ch_num].gainbuf[0]);
			}
		}
		if(nmeas < avg_num)
		{
			measurement[ch_num].actbuf[nmeas] = measurement[ch_num].volts;
			measurement[ch_num].gainbuf[nmeas] = measurement[ch_num].prevgain;
			++nmeas;
			num = nmeas;
			measurement[ch_num].nmeas = nmeas;
		}
		else
		{
			moving_average(avg_num,&measurement[ch_num].actbuf[0], measurement[ch_num].volts, &measurement[ch_num].gainbuf[0], measurement[ch_num].prevgain);
			num = avg_num;
		}

		sx = 0.;
		for(i = 0; i < num; i++)
			sx += measurement[ch_num].actbuf[i];
		avg = sx /(float)num;


		PushIsMeas(ucChamber, num, num, avg, 0, 0, 0, 0);
		if(measurement[ch_num].prevgain == 0)
		{
			measurement[ch_num].isnum = 0;
				measurement[ch_num].meas = avg;
			return;
		}

		measurement[ch_num].meas = measurement[ch_num].volts;
		measurement[ch_num].longavg = FALSE;
		if (measurement[ch_num].volts <= 0.05)
		{
			measurement[ch_num].doavg = FALSE;
			measurement[ch_num].lastmeas = measurement[ch_num].meas;
			measurement[ch_num].isnum = 0;
			return;
		}

		if(measurement[ch_num].isnum < 10)
		{
			measurement[ch_num].doavg = FALSE;
			++measurement[ch_num].isnum;
		}


		if (measurement[ch_num].doavg)
		{
			measurement[ch_num].meas = avg;
			return;
		}

		if(measurement[ch_num].isnum == 10)
		{
			diff = measurement[ch_num].lastmeas - measurement[ch_num].meas;
			if (diff > 0)
			{
				measurement[ch_num].doavg = TRUE;
				measurement[ch_num].actbuf[0] = measurement[ch_num].lastmeas;
				measurement[ch_num].actbuf[1] = measurement[ch_num].volts;
				measurement[ch_num].nmeas = 2;
				return;
			}
		}

		measurement[ch_num].lastmeas = measurement[ch_num].meas;
	}

				/* moving average for 10 or 40 values */
				/* this is long code but fast */
/**
 * \details Add values to the FILO buffer used in calculating the boxcar average
 * \param num Size of the FILO buffer
 * \param actbuf Pointer to activity buffer
 * \param volts Value to insert into the FILO buffer
 * \param gainbuf Pointer to gain buffer
 * \param prevgain Value to insert into the gain buffer
 * \returns None
 */
static	void moving_average(short num, float actbuf[], float volts, char gainbuf[], char prevgain)
	{
		/* actbuf sent is chamber.actbuf[0]
		   volts sent is chamber.volts */

		actbuf[0] = actbuf[1];
		actbuf[1] = actbuf[2];
		actbuf[2] = actbuf[3];
		actbuf[3] = actbuf[4];
		actbuf[4] = actbuf[5];
		actbuf[5] = actbuf[6];
		actbuf[6] = actbuf[7];
		actbuf[7] = actbuf[8];
		actbuf[8] = actbuf[9];
		actbuf[9] = actbuf[10];

		actbuf[10] = actbuf[11];
		actbuf[11] = actbuf[12];
		actbuf[12] = actbuf[13];
		actbuf[13] = actbuf[14];
		actbuf[14] = actbuf[15];
		actbuf[15] = actbuf[16];
		actbuf[16] = actbuf[17];
		actbuf[17] = actbuf[18];
		actbuf[18] = actbuf[19];
		actbuf[19] = actbuf[20];

		actbuf[20] = actbuf[21];
		actbuf[21] = actbuf[22];
		actbuf[22] = actbuf[23];
		actbuf[23] = actbuf[24];
		actbuf[24] = actbuf[25];
		actbuf[25] = actbuf[26];
		actbuf[26] = actbuf[27];
		actbuf[27] = actbuf[28];
		actbuf[28] = actbuf[29];
		actbuf[29] = actbuf[30];

		actbuf[30] = actbuf[31];
		actbuf[31] = actbuf[32];
		actbuf[32] = actbuf[33];
		actbuf[33] = actbuf[34];
		actbuf[34] = actbuf[35];
		actbuf[35] = actbuf[36];
		actbuf[36] = actbuf[37];
		actbuf[37] = actbuf[38];
		actbuf[38] = actbuf[39];

		actbuf[39] = volts;



		gainbuf[0] = gainbuf[1];
		gainbuf[1] = gainbuf[2];
		gainbuf[2] = gainbuf[3];
		gainbuf[3] = gainbuf[4];
		gainbuf[4] = gainbuf[5];
		gainbuf[5] = gainbuf[6];
		gainbuf[6] = gainbuf[7];
		gainbuf[7] = gainbuf[8];
		gainbuf[8] = gainbuf[9];
		gainbuf[9] = gainbuf[10];
		gainbuf[10] = gainbuf[11];
		gainbuf[11] = gainbuf[12];
		gainbuf[12] = gainbuf[13];
		gainbuf[13] = gainbuf[14];
		gainbuf[14] = gainbuf[15];
		gainbuf[15] = gainbuf[16];
		gainbuf[16] = gainbuf[17];
		gainbuf[17] = gainbuf[18];
		gainbuf[18] = gainbuf[19];
		gainbuf[19] = gainbuf[20];
		gainbuf[20] = gainbuf[21];
		gainbuf[21] = gainbuf[22];
		gainbuf[22] = gainbuf[23];
		gainbuf[23] = gainbuf[24];
		gainbuf[24] = gainbuf[25];
		gainbuf[25] = gainbuf[26];
		gainbuf[26] = gainbuf[27];
		gainbuf[27] = gainbuf[28];
		gainbuf[28] = gainbuf[29];
		gainbuf[29] = gainbuf[30];
		gainbuf[30] = gainbuf[31];
		gainbuf[31] = gainbuf[32];
		gainbuf[32] = gainbuf[33];
		gainbuf[33] = gainbuf[34];
		gainbuf[34] = gainbuf[35];
		gainbuf[35] = gainbuf[36];
		gainbuf[36] = gainbuf[37];
		gainbuf[37] = gainbuf[38];
		gainbuf[38] = gainbuf[39];
		gainbuf[39] = prevgain;
	}
/**
 * \details Clear the FILO buffer
 * \param actbuf Pointer to activity buffer
 * \param gainbuf Pointer to gain buffer
 * \returns None
 */
static void clear_moving(float actbuf[], char gainbuf[]) {
	actbuf[0] = 0;
	actbuf[1] = 0;
	actbuf[2] = 0;
	actbuf[3] = 0;
	actbuf[4] = 0;
	actbuf[5] = 0;
	actbuf[6] = 0;
	actbuf[7] = 0;
	actbuf[8] = 0;
	actbuf[9] = 0;
	actbuf[10] = 0;
	actbuf[11] = 0;
	actbuf[12] = 0;
	actbuf[13] = 0;
	actbuf[14] = 0;
	actbuf[15] = 0;
	actbuf[16] = 0;
	actbuf[17] = 0;
	actbuf[18] = 0;
	actbuf[19] = 0;
	actbuf[20] = 0;
	actbuf[21] = 0;
	actbuf[22] = 0;
	actbuf[23] = 0;
	actbuf[24] = 0;
	actbuf[25] = 0;
	actbuf[26] = 0;
	actbuf[27] = 0;
	actbuf[28] = 0;
	actbuf[29] = 0;
	actbuf[30] = 0;
	actbuf[31] = 0;
	actbuf[32] = 0;
	actbuf[33] = 0;
	actbuf[34] = 0;
	actbuf[35] = 0;
	actbuf[36] = 0;
	actbuf[37] = 0;
	actbuf[38] = 0;
	actbuf[39] = 0;

	gainbuf[0] = 0;
	gainbuf[1] = 0;
	gainbuf[2] = 0;
	gainbuf[3] = 0;
	gainbuf[4] = 0;
	gainbuf[5] = 0;
	gainbuf[6] = 0;
	gainbuf[7] = 0;
	gainbuf[8] = 0;
	gainbuf[9] = 0;
	gainbuf[10] = 0;
	gainbuf[11] = 0;
	gainbuf[12] = 0;
	gainbuf[13] = 0;
	gainbuf[14] = 0;
	gainbuf[15] = 0;
	gainbuf[16] = 0;
	gainbuf[17] = 0;
	gainbuf[18] = 0;
	gainbuf[19] = 0;
	gainbuf[20] = 0;
	gainbuf[21] = 0;
	gainbuf[22] = 0;
	gainbuf[23] = 0;
	gainbuf[24] = 0;
	gainbuf[25] = 0;
	gainbuf[26] = 0;
	gainbuf[27] = 0;
	gainbuf[28] = 0;
	gainbuf[29] = 0;
	gainbuf[30] = 0;
	gainbuf[31] = 0;
	gainbuf[32] = 0;
	gainbuf[33] = 0;
	gainbuf[34] = 0;
	gainbuf[35] = 0;
	gainbuf[36] = 0;
	gainbuf[37] = 0;
	gainbuf[38] = 0;
	gainbuf[39] = 0;
	}
