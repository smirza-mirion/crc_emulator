/*********************************************************************
  MODULE:       MEASURE ZERO

  FILE:         Zero.c

  DATE:         03/25/08
                09/17/08 -- fixed adc_zero # of mesurements in read_zero

  ANALYSIS:     measures and displays Zero for calibrator

    *************************************************************************/


#include "crc.h"
#include "screen.h"
//#include "printer.h"
#include "uart.h"
#include "i2c.h"
#include "message.h"
#include "chambfac.h"
#include "keyboard.h"
#include "qspi.h"
#include "daily.h"
#include "remote.h"
#include "amulet.h"
#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>       //for fabs

#include "lowlevelsnoop.h"

#define ZMAX_15 160.
#define ZDRIFT_15 5.0
#define ZMAX_18 5120.
#define ZDRIFT_18 40.0

#define OP_AMP_ZERO_COUNT 107

    extern CHAMBER chamber[];
    extern CHAMBERVALS chamb_vals[];
    extern MEASUREMENT measurement[];
    extern CURRENT  current;
    extern ZERODATA zerodata[];
    extern REMOTE remote[];
    extern ushort chamber_checksum[];
    extern volatile bool AmuletPCQC_Abort;
    extern time_t clock_time;
    extern long long int AmuletDailyMenu_ChamberDailyTestID;


    static float zero_value(float zero);
    static void setup_for_zero(short ch_num);
    static void display_zero(short ch_num);
    static void zero_to_remote(short ch_num);

    ushort calc_chamber_checksum(short ch_num);
    void send_to_amulet_string(uchar ucIndex, char message0[]);
    void DB_CreateChamberZero(CHAMBERZERO *chamberzero, long long int *ChamberDailyTestID, bool bookEnd);
    void trim(char *acByte);
    void Sysset_updateMirror(void);

    bool measure_zero_offset(bool daily_flag)
    {

        char ch, *acMsg;
        short ch_num = current.main_chamber;
        char test_type = '0' + TEST_ZERO;
        CHAMBERZERO *chamberzero;


        if(!daily_flag)
            erase_screen();

        //AUTO ZERO
        display_medium_message(AUTO_ZERO_1,6,0,NORMAL);

        //send IN PROGRESS message to remote if started at Readout or PC
        if(daily_flag)
        {
            test_type = '0' + TEST_ZERO;
            remote_in_progress(ch_num,test_type);

            //NO SOURCES
            display_medium_message(AUTO_ZERO_2,40,0,NORMAL);

            //continue via any key or command from PC
            key_pc_continue();

        }


        if(current.demo_mode)
        {
            contmsg();
            erase_lines(40,24,0);
            display_medium_message(AUTO_ZERO_7,38,0,NORMAL);
            display_small_message(AUTO_ZERO_8,56,0,NORMAL);
            display_text(20,22,"    0.0mV",0,BIG,NORMAL);
            contmsg();
            erase_screen();
            return TRUE;


        }

        if(AmuletPCQC_Abort) return FALSE;
        if(home_set()) return FALSE;

        //PLEASE WAIT
        plwait(TRUE);

        //measure zero of ADC
        setup_for_zero_adc(ch_num);
        set_nokey();

        measurement[ch_num].display_flag = FALSE;

        do
        {

            (void)getkey();     //to trigger get_measurements

        }while(zerodata[ch_num].phase == 1);


        for(;;)
        {

            if(measurement[ch_num].display_flag)
            {
                display_zero(ch_num);
                measurement[ch_num].display_flag = FALSE;
            }

            do
            {
                ch = getkey();  //triggers servicing chambers

                if(zerodata[ch_num].status != TEST_WAIT)
                {

                    if(test_done(ch,ch_num,TEST_ZERO))
                    {
                        zerodata[ch_num].done = TRUE;
                        break;  //break from do loop
                    }

                    if(AmuletPCQC_Abort) return FALSE;
                    if(home_set()) return FALSE;

                }
                else
                {
                    if (ch != NOKEY)
                        beep();
                }
                if(!chamber[ch_num].connected_flag)
                    return FALSE;
            }while(!measurement[ch_num].valid_flag);


            if(zerodata[ch_num].status == ZERO_OUT_OF_RANGE)
            {
                erase_screen();
                //ERROR
                display_medium_message(INPUT_1,2,0,REV);
                //AUTO ZERO
                display_medium_message(AUTO_ZERO_1,20,0,NORMAL);
                //OUT OF RANGE
                display_medium_message(AUTO_ZERO_4,34,0,NORMAL);

                //key_pc_continue();
                //measurement[ch_num].dogain = 1;
                //erase_screen();
                if((chamber[ch_num].control != CONTROL_PC) && (chamber[ch_num].control != CONTROL_REMOTE)) return(FALSE);
                else{
                	if(zerodata[ch_num].done){
						chamberzero = (CHAMBERZERO *) malloc(sizeof(CHAMBERZERO));
						chamberzero->ChamberZeroID = 0;
						chamberzero->ChamberSerialNumber[6] = 0;
						strncpy(chamberzero->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
						chamberzero->ChamberType = chamb_vals[ch_num].chamb_type;
						chamberzero->TwoStageChamber = chamber_one_gain_relay(ch_num);
						chamberzero->ZeroStatus = zerodata[ch_num].status;
						get_amulet_message_with_language(L_ZERO_OUT_OF_RANGE, chamberzero->ZeroTextEnglish, ENGLISH);    // "Zero out of Range"
						get_amulet_message_with_language(L_ZERO_OUT_OF_RANGE, chamberzero->ZeroTextFrench, FRENCH);    // "Zero out of Range"
						chamberzero->ZeroValue = zerodata[ch_num].znew;
						chamberzero->MeasuredOn = clock_time;
						chamberzero->CreatedOn = 0;
						chamberzero->InactiveReason[0] = 0;
						chamberzero->Inactive = FALSE;
						if(chamber[ch_num].control == CONTROL_PC) DB_CreateChamberZero(chamberzero, &AmuletDailyMenu_ChamberDailyTestID, TRUE);
						else DB_CreateChamberZero(chamberzero, NULL, TRUE);
						free(chamberzero);

						measurement[ch_num].dogain = 1;
						erase_screen();
						measurement[ch_num].dogain = 1;

						//exit zeromode
						setchamber(ch_num,0);
						measurement[ch_num].nmeas = 0;
						measurement[ch_num].mode = MEASMODE;

						//release remote in progress
						if(daily_flag) remote_in_progress(ch_num,'0');

						return FALSE;
                	}
                }
            }else{
            	if(zerodata[ch_num].done)
            		break;
            }
        }

        if((chamber[ch_num].control == CONTROL_PC) || (chamber[ch_num].control == CONTROL_REMOTE)){
			chamberzero = (CHAMBERZERO *) malloc(sizeof(CHAMBERZERO));
			chamberzero->ChamberZeroID = 0;
			chamberzero->ChamberSerialNumber[6] = 0;
			strncpy(chamberzero->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
			chamberzero->ChamberType = chamb_vals[ch_num].chamb_type;
			chamberzero->TwoStageChamber = chamber_one_gain_relay(ch_num);
			chamberzero->ZeroStatus = zerodata[ch_num].status;
			chamberzero->ZeroTextEnglish[0] = 0;
			chamberzero->ZeroTextFrench[0] = 0;
			chamberzero->ZeroValue = zerodata[ch_num].znew;
			chamberzero->MeasuredOn = clock_time;
			chamberzero->CreatedOn = 0;
			chamberzero->InactiveReason[0] = 0;
			chamberzero->Inactive = FALSE;

			if(zerodata[ch_num].status == TEST_GOOD){
				strcpy(chamberzero->ZeroTextEnglish, measurement[ch_num].actstr);
				trim(chamberzero->ZeroTextEnglish);
				strcpy(chamberzero->ZeroTextFrench, chamberzero->ZeroTextEnglish);
			}else if (zerodata[ch_num].status == ZERO_DRIFT){
				acMsg = malloc(100);

				strcpy(chamberzero->ZeroTextEnglish, measurement[ch_num].actstr);
				strcat(chamberzero->ZeroTextEnglish, " ");
				get_amulet_message_with_language(L_ZERO_DRIFT, acMsg, ENGLISH);    // "Zero Drift"
				strcat(chamberzero->ZeroTextEnglish, acMsg);

				strcpy(chamberzero->ZeroTextFrench, measurement[ch_num].actstr);
				trim(chamberzero->ZeroTextFrench);
				strcat(chamberzero->ZeroTextFrench, " ");
				get_amulet_message_with_language(L_ZERO_DRIFT, acMsg, FRENCH);    // "Zero Drift"
				strcat(chamberzero->ZeroTextFrench, acMsg);

				free(acMsg);
			}else if (zerodata[ch_num].status == ZERO_OUT_OF_RANGE){
				get_amulet_message_with_language(L_ZERO_OUT_OF_RANGE, chamberzero->ZeroTextEnglish, ENGLISH);    // "Zero out of Range"
				get_amulet_message_with_language(L_ZERO_OUT_OF_RANGE, chamberzero->ZeroTextFrench, FRENCH);    // "Zero out of Range"
			}

			if(chamber[ch_num].control == CONTROL_PC) DB_CreateChamberZero(chamberzero, &AmuletDailyMenu_ChamberDailyTestID, TRUE);
			else DB_CreateChamberZero(chamberzero, NULL, TRUE);
			free(chamberzero);
		}

        measurement[ch_num].dogain = 1;
        erase_screen();
        measurement[ch_num].dogain = 1;
        chamber[ch_num].zero = zerodata[ch_num].znew;

        //calculate checksum & save
        chamber_checksum[ch_num] = calc_chamber_checksum(ch_num);
        Sysset_updateMirror();

        //exit zeromode
        setchamber(ch_num,0);
        measurement[ch_num].nmeas = 0;
        measurement[ch_num].mode = MEASMODE;

        //release remote in progress
        if(daily_flag)
            remote_in_progress(ch_num,'0');
        return TRUE;


    }

static float zero_value(float zero){
	short ch_num = current.main_chamber;

	if(chamber_one_gain_relay(ch_num)){
		return(zero * 4 * 512 / 262144);
	}else{
		return(zero * 4 * 512 / 32768);
	}
}


void setup_for_zero_adc(short ch_num){
	//measure zero of ADC
	//setup for ADC Zero
	set_adc_mode(ch_num, ADC_READ_ZERO);

	//set actstr to blank in case PC requests Zero too early
	strcpy(&measurement[ch_num].actstr[0],"         ");

	zerodata[ch_num].sum = 0;
	zerodata[ch_num].phase = 1;
	zerodata[ch_num].done = FALSE;
	zerodata[ch_num].count = 0;
	zerodata[ch_num].barvalue = 0;

	zerodata[ch_num].status = TEST_WAIT;
	remote[ch_num].mode = REMOTE_ZERO_MODE;
	measurement[ch_num].mode = ZEROMODE;
	measurement[ch_num].kun = 7;    //mV
}

static void setup_for_zero(short ch_num){
	//return to measuring volts
	set_adc_mode(ch_num, ADC_READ_VOLTS);

	//begin chamber zero measurement
	zerodata[ch_num].phase = 2;
	//set chamber for zero measurement
	set_zero(ch_num);
	zerodata[ch_num].count = 0;
	zerodata[ch_num].sum = 0.;
}


    void read_zero(short ch_num){
        float value;
        float adc_zero;
        float zdiff;
        float zdisp;
		float zmaxlimit, zdriftlimit;
        float zero = chamber[ch_num].zero;
        char strng[40], strng2[40];
        short nc;
        char str[10];
        char acMsg[25];
        double dValue;
        long lValue;

		if(chamber_one_gain_relay(ch_num)){
			zmaxlimit = ZMAX_18;
			zdriftlimit = ZDRIFT_18;
		}
		else{
			zmaxlimit = ZMAX_15;
			zdriftlimit = ZDRIFT_15;
		}

        //phase 1, adc zero
        if(zerodata[ch_num].phase == 1){
            if(zerodata[ch_num].count < 7){ //3)
                ++zerodata[ch_num].count;
                ++zerodata[ch_num].barvalue;
                zero_to_remote(ch_num);
                return;

            }

            if(zerodata[ch_num].count == 7){ //3)
                ++zerodata[ch_num].count;
                ++zerodata[ch_num].barvalue;
                zero_to_remote(ch_num);

                setup_for_zero(ch_num);
                return;
            }
        }


        //phase 2, measure Zero
        if(zerodata[ch_num].count < 8){ //< 4)
            ++zerodata[ch_num].count;
            ++zerodata[ch_num].barvalue;
            zero_to_remote(ch_num);
            return;
        }

        if(zerodata[ch_num].count > 7 && zerodata[ch_num].count < 19){
        	dValue = measurement[ch_num].adc_value;
        	value = dValue;
        	adc_zero = chamber[ch_num].adc_zero;
        	value = value - adc_zero;
            zerodata[ch_num].sum += value;
            PushZero(ch_num, value);
            ++zerodata[ch_num].count;
            ++zerodata[ch_num].barvalue;

	 if(zerodata[ch_num].count == 19){
            	lValue = zerodata[ch_num].count - 7;
                zerodata[ch_num].znew = zerodata[ch_num].sum / (float)lValue;
                //if(current.main_chamber == ch_num && chamber[ch_num].control == CONTROL_MAIN)
                if(current.main_chamber == ch_num && get_crc_mode() == CRC_CAL)
                    erase_lines(40,24,0);
                zerodata[ch_num].status = TEST_GOOD;
            }
            zero_to_remote(ch_num);
            return;
        }

        dValue = measurement[ch_num].adc_value;
        value = dValue;
        adc_zero = chamber[ch_num].adc_zero;
        value = value - adc_zero;

        //use moving average of last 10 values
        zerodata[ch_num].znew = .9 * zerodata[ch_num].znew + 0.1 * value;


        if (zerodata[ch_num].znew < (-1 * zmaxlimit) || zerodata[ch_num].znew > zmaxlimit){
            zerodata[ch_num].status = ZERO_OUT_OF_RANGE;
            strcpy(&strng[0],"       mV");
            strcpy(&measurement[ch_num].actstr[0],strng);
            zero_to_remote(ch_num);
            //SetAmuletString(150, "Zero out of Range");
            send_amulet_message(L_ZERO_OUT_OF_RANGE, 150);    // "Zero out of Range"
            return;
        }

        zdiff = zerodata[ch_num].znew - zero;
        if (zdiff > zdriftlimit || zdiff < (-1 * zdriftlimit))
            zerodata[ch_num].status = ZERO_DRIFT;
        else
            zerodata[ch_num].status = TEST_GOOD;

        strcpy(&strng[0],"       mV");
        zdisp = zero_value(zerodata[ch_num].znew);
        nc = sprintf(str,"%6.2f",zdisp);
        strncpy(&strng[6 - nc],str,nc);
        //save in measurement
        strcpy(&measurement[ch_num].actstr[0],strng);

        zerodata[ch_num].zerodisp = zdisp;
        zero_to_remote(ch_num);
        measurement[ch_num].display_flag = TRUE;

        if (zerodata[ch_num].status == TEST_GOOD) {
        	send_to_amulet_string(150, &(measurement[ch_num].actstr[0]));
        }
        else if (zerodata[ch_num].status == ZERO_DRIFT) {
        	strcpy(acMsg, &(measurement[ch_num].actstr[0]));
        	get_amulet_message(L_ZERO_DRIFT, strng2);    // "Zero Drift"
        	strcat(acMsg, " ");
        	strcat(acMsg, strng2);
        	//strcat(acMsg, " Zero Drift");
        	send_to_amulet_string(150, acMsg);
        }
        else if (zerodata[ch_num].status == ZERO_OUT_OF_RANGE) {
        	//SetAmuletString(150, "Zero out of Range");
        	send_amulet_message(L_ZERO_OUT_OF_RANGE, 150);    // "Zero out of Range"
        }
    }


    static void display_zero(short ch_num)
    {

        if(zerodata[ch_num].status == ZERO_DRIFT)
        {
            //CAUTION
            //ZERO DRIFT
            display_medium_message(AUTO_ZERO_5,6,0,NORMAL);
            display_medium_message(AUTO_ZERO_6,38,0,NORMAL);
        }
        else
        {
            //AUTO ZERO
            display_medium_message(AUTO_ZERO_1,6,0,NORMAL);
            //"    OK    "
            display_medium_message(AUTO_ZERO_7,38,0,NORMAL);
        }

        //ENTER to Accept -- if called via Daily Test from calibrator
        if(chamber[ch_num].control == CONTROL_MAIN)
            display_small_message(AUTO_ZERO_8,56,0,NORMAL);

        display_text(20,22,&measurement[ch_num].actstr[0],0,BIG,NORMAL);


    }

    static void zero_to_remote(short ch_num)
    {

        char test_type;

        test_type = '0' + TEST_ZERO;
        remote_value(ch_num,&measurement[ch_num].actstr[0],test_type,zerodata[ch_num].status);

    }


    bool test_done(char ch, short ch_num, char test_type)
    {

        switch(chamber[ch_num].control)
        {
        case CONTROL_MAIN:
            if (ch == OK)
                return(TRUE);
            if(home_set())
                return FALSE;
            if(ch != NOKEY)
                beep(); // key other than ENTER
            break;
        case CONTROL_PC:
            if(get_pc_ret() == PC_ACCEPT)
                return(TRUE);
            break;
        case CONTROL_REMOTE:
            if(test_accepted(ch_num,test_type))
                return(TRUE);
            break;
        }

        //none of the above criteria was satisfied
        return FALSE;
    }



