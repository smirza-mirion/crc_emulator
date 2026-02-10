/*********************************************************************
  MODULE:		MEASURE BACKGROUND

  FILE: 		MeasureBkg.c

  DATE:         03/25/08


  ANALYSIS:		measures and displays background for calibrator

    *************************************************************************/
#include "crc.h"
#include "i2c.h"
#include "screen.h"
#include "keyboard.h"
#include "uart.h"
#include "chambfac.h"
#include "daily.h"
#include "remote.h"
#include "message.h"
#include "amulet.h"
#include "nuc.h"
#include <string.h>
#include <stdlib.h>

    extern CHAMBER chamber[];
    extern CHAMBERVALS chamb_vals[];
    extern MEASUREMENT measurement[];
	extern CURRENT current;
    extern BKGDATA bkgdata[];
    extern REMOTE remote[];
    extern ushort chamber_checksum[];
    extern volatile bool AmuletPCQC_Abort;
    extern time_t clock_time;
    extern long long int AmuletDailyMenu_ChamberDailyTestID;

    static bool bad_bkg;

static void set_status(short ch_num);
static void bkg_to_remote(short ch_num);
ushort calc_chamber_checksum(short ch_num);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void DB_CreateChamberBackground(CHAMBERBACKGROUND *chamberbackground, long long int ChamberDailyTestID, bool bookEnd);
void trim(char *acByte);

	void measure_bkg(short daily_flag)
	{

        short ch_num = current.main_chamber;
        char ch;
        char test_type;
        char *acMsg;
        CHAMBERBACKGROUND *chamberbackground;


        //send IN PROGRESS message to remote if started at Readout or PC
        if(daily_flag != -1)
        {
            test_type = '0' + TEST_BKG;
            remote_in_progress(ch_num,test_type);
        }

        if (daily_flag == 0)
        {
            erase_screen();
            //display_text(32,2,"MEASURE",0,MEDIUM,0);
            //display_text(16,12,"BACKGROUND",0,MEDIUM,0);
            //display_text(16,40,"NO SOURCES",0,MEDIUM,0);
            display_medium_message(BKG_1,2,0,NORMAL);
            display_medium_message(BKG_2,12,0,NORMAL);
            display_medium_message(BKG_3,40,0,NORMAL);
            //key press or PC command
            key_pc_continue();

            if(AmuletPCQC_Abort) return;
            if(home_set()) return;
            erase_lines(0,22,0);
        }

        if(daily_flag == -1) //from Remote
            erase_screen();

        //display_text(16,6,"BACKGROUND",0,MEDIUM,0);
        display_medium_message(BKG_2,6,0,NORMAL);


        setup_for_background(ch_num);
        bad_bkg = FALSE;

        plwait(TRUE);

        set_nokey();

        measurement[ch_num].display_flag = FALSE;

        for(;;)
        {
            if(measurement[ch_num].display_flag)
            {
                display_low(ch_num);
                measurement[ch_num].display_flag = FALSE;
            }

            do
            {
                ch = getkey();  //triggers servicing chambers
                if(AmuletPCQC_Abort) return;
                if(home_set()) return;

                if(bkgdata[ch_num].status != TEST_WAIT)
                {
                    if(test_done(ch,ch_num,TEST_BKG))
                    {
                        bkgdata[ch_num].done = TRUE;
                        break;  //break from do loop
                    }
                }
                else
                {
                    if (ch != NOKEY)
                        beep();
                }
                if(!chamber[ch_num].connected_flag)
                    return;
            }while(!measurement[ch_num].valid_flag);


            //if (measurement[ch_num].gain > 0)
            if ((measurement[ch_num].meas + chamber[ch_num].bkg)>= 2.95679)
            {
                remote_value(ch_num,"         ",test_type,BKG_TOO_HIGH);
                erase_screen();
                display_text(10,0,"BACKGROUND",0,BIG,0);
                display_text(20,22,"TOO HIGH",0,BIG,0);
                bad_bkg = TRUE;
                seeman();
                //key_pc_continue();
                erase_screen();
                bkgdata[ch_num].status = BKG_TOO_HIGH;

                if((chamber[ch_num].control != CONTROL_PC) && (chamber[ch_num].control != CONTROL_REMOTE)) return;
                else{
                	if(bkgdata[ch_num].done){
                		chamberbackground = (CHAMBERBACKGROUND *) malloc(sizeof(CHAMBERBACKGROUND));
						chamberbackground->ChamberBackgroundID = 0;
						chamberbackground->ChamberSerialNumber[6] = 0;
						strncpy(chamberbackground->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
						chamberbackground->ChamberType = chamb_vals[ch_num].chamb_type;
						chamberbackground->TwoStageChamber = chamber_one_gain_relay(ch_num);
						chamberbackground->BackgroundStatus = BKG_TOO_HIGH;
						get_amulet_message_with_language(L_BACKGROUND_TOO_HIGH, chamberbackground->BackgroundTextEnglish, ENGLISH);    // "BACKGROUND TOO HIGH"
						get_amulet_message_with_language(L_BACKGROUND_TOO_HIGH, chamberbackground->BackgroundTextFrench, FRENCH);    // "BACKGROUND TOO HIGH"
						chamberbackground->BackgroundValue = measurement[ch_num].meas + chamber[ch_num].bkg;
						chamberbackground->MeasuredOn = clock_time;
						chamberbackground->InactiveReason[0] = 0;
						chamberbackground->Inactive = FALSE;
						if(chamber[ch_num].control == CONTROL_PC) DB_CreateChamberBackground(chamberbackground, AmuletDailyMenu_ChamberDailyTestID, TRUE);
						else DB_CreateChamberBackground(chamberbackground, 0, TRUE);
						free(chamberbackground);

						measurement[ch_num].nmeas = 0;
						//if started from Readout, release Remote keys
						if(daily_flag == 1) remote_in_progress(ch_num,'0');

						return;
                	}
                }
            }

            if(bkgdata[ch_num].done)
                break;

        }

		chamber[ch_num].bkg += measurement[ch_num].meas;

		if((chamber[ch_num].control == CONTROL_PC) || (chamber[ch_num].control == CONTROL_REMOTE)){
        	chamberbackground = (CHAMBERBACKGROUND *) malloc(sizeof(CHAMBERBACKGROUND));
        	chamberbackground->ChamberBackgroundID = 0;
        	chamberbackground->ChamberSerialNumber[6] = 0;
        	strncpy(chamberbackground->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
        	chamberbackground->ChamberType = chamb_vals[ch_num].chamb_type;
			chamberbackground->TwoStageChamber = chamber_one_gain_relay(ch_num);
			chamberbackground->BackgroundStatus = bkgdata[ch_num].status;
			strcpy(chamberbackground->BackgroundTextEnglish, measurement[ch_num].actstr);
			trim(chamberbackground->BackgroundTextEnglish);
			replace(chamberbackground->BackgroundTextEnglish, '$', 'u');
			strcpy(chamberbackground->BackgroundTextFrench, chamberbackground->BackgroundTextEnglish);
			if(chamberbackground->BackgroundStatus == BKG_HIGH){
				acMsg = malloc(40);
				strcat(chamberbackground->BackgroundTextEnglish, " ");
				get_amulet_message_with_language(L_HIGH, acMsg, ENGLISH);    // "HIGH"
				strcat(chamberbackground->BackgroundTextEnglish, acMsg);

				strcat(chamberbackground->BackgroundTextFrench, " ");
				get_amulet_message_with_language(L_HIGH, acMsg, FRENCH);    // "HIGH"
				strcat(chamberbackground->BackgroundTextFrench, acMsg);
				free(acMsg);
			}
			chamberbackground->BackgroundValue = chamber[ch_num].bkg;
			chamberbackground->MeasuredOn = clock_time;
			chamberbackground->InactiveReason[0] = 0;
			chamberbackground->Inactive = FALSE;
			if(chamber[ch_num].control == CONTROL_PC) DB_CreateChamberBackground(chamberbackground, AmuletDailyMenu_ChamberDailyTestID, TRUE);
			else DB_CreateChamberBackground(chamberbackground, 0, TRUE);
			free(chamberbackground);
        }

        //calculate checksum & save
        chamber_checksum[ch_num] = calc_chamber_checksum(ch_num);


        measurement[ch_num].nmeas = 0;
        //if started from Readout, release Remote keys
        if(daily_flag == 1)
            remote_in_progress(ch_num,'0');


		erase_screen();

	}

    void low_act(short ch_num)
    {
        float actfact;
        float response;
        char actstr[12];
        float lowbkg = bkgdata[ch_num].lowbkg;
        float activity;
        short ch_type;
        char message[51], strng[40];
        short kunit;

        ch_type = chamber_type(ch_num);
        //if(bad_bkg)
        //    return;

        if(bkgdata[ch_num].count < 10)
        {
            ++bkgdata[ch_num].count;
            bkg_to_remote(ch_num);
            getdec(measurement[ch_num].act0, current.system, &kunit);
            measurement[ch_num].kun = kunit;
            return;
        }

        if(bkgdata[ch_num].count >= 10 && bkgdata[ch_num].count < 50)
        {
            if(current.demo_mode && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER))
                measurement[ch_num].volts = 0.;

            bkgdata[ch_num].sum += measurement[ch_num].volts;
            measurement[ch_num].actbuf[bkgdata[ch_num].count - 10] = measurement[ch_num].volts;
            ++bkgdata[ch_num].count;
            measurement[ch_num].meas = bkgdata[ch_num].sum / (float)(bkgdata[ch_num].count - 10);

            response = measurement[ch_num].response;
            actfact = get_gainfact(ch_type);
            // Locked: CI
            //if(current.system == BQ)
            //    actfact *= BQFACTOR;
            // Changed:
            //measurement[ch_num].act = (measurement[ch_num].meas - lowbkg) * actfact / response;
            measurement[ch_num].act0 = (measurement[ch_num].meas - lowbkg) * actfact / response;

            activity = measurement[ch_num].act0;
            if(current.system == BQ) activity *= BQFACTOR;

            // Locked: CI
            //format_activity(measurement[ch_num].act,current.system,actstr);
            // Changed:
            //format_activity(measurement[ch_num].act, CI, actstr);
            //format_activity(measurement[ch_num].act0, CI, actstr);
            format_activity(activity, current.system, actstr);
            getdec(activity, current.system, &kunit);

            //send activity string to measurement
            strcpy(&measurement[ch_num].actstr[0],actstr);
            measurement[ch_num].kun = kunit;

            measurement[ch_num].display_flag = TRUE;
            bkg_to_remote(ch_num);

            return;
        }

        if(bkgdata[ch_num].count == 50)
        {
            measurement[ch_num].nmeas = 40;
            measurement[ch_num].longavg = TRUE;
            if(ch_num == current.main_chamber)
            {
                if(get_crc_mode() == CRC_CAL)
                    plwait(FALSE);
                if(chamber[ch_num].control == CONTROL_MAIN)
                {
                    set_nokey();
                    //display_text(18,56,"ENTER to Accept",0,SMALL,NORMAL);
                    display_small_message(AUTO_ZERO_8,56,0,NORMAL);
                }
            }
            //assume good
            bkgdata[ch_num].status = TEST_GOOD;
            bkgdata[ch_num].count = 51;
        }

        ismeas(ch_num);

        response = measurement[ch_num].response;
        actfact = get_gainfact(ch_type);
        // Locked: CI
        //if(current.system == BQ)
        //    actfact *= BQFACTOR;
        if(current.demo_mode  && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER))
        	// Changed:
        	//measurement[ch_num].act = 0.;
        	measurement[ch_num].act0 = 0.;
        else
        	// Changed:
        	//measurement[ch_num].act = (measurement[ch_num].meas - lowbkg) *
        	//    actfact / response;
        	measurement[ch_num].act0 = (measurement[ch_num].meas - lowbkg) *
					actfact / response;

        set_status(ch_num);
        // Locked: CI
        //format_activity(measurement[ch_num].act,current.system,actstr);
        // Changed:
        //format_activity(measurement[ch_num].act, CI, actstr);
        activity = measurement[ch_num].act0;
        if(current.system == BQ) activity *= BQFACTOR;
        //format_activity(measurement[ch_num].act0, CI, actstr);
        format_activity(activity, current.system, actstr);
        getdec(activity, current.system, &kunit);

        //send activity string to measurement
        strcpy(&measurement[ch_num].actstr[0],actstr);
        measurement[ch_num].kun = kunit;

        measurement[ch_num].display_flag = TRUE;

        //if (measurement[ch_num].gain > 0) {
        if ((measurement[ch_num].meas + chamber[ch_num].bkg) >= 2.95679) {
        	send_amulet_message(L_BACKGROUND_TOO_HIGH, 150);    // "BACKGROUND TOO HIGH"
        	bkgdata[ch_num].status = BKG_TOO_HIGH;
        }
        else {
        	if(bkgdata[ch_num].status == BKG_HIGH){
        		get_amulet_message(L_HIGH, strng);    // "HIGH"
        		sprintf(message, "%s %s", strng, &(measurement[ch_num].actstr[0]));
        		send_to_amulet_string(150, message);
        	}else{
        		send_to_amulet_string(150, &(measurement[ch_num].actstr[0]));
        	}
        }

        bkg_to_remote(ch_num);
    }

    void display_low(short ch_num)
    {


        if(bkgdata[ch_num].status == TEST_GOOD)
            display_text(40,38," OK ",0,BIG,NORMAL);
        if(bkgdata[ch_num].status == BKG_HIGH)
            display_text(40,38,"HIGH",0,BIG,NORMAL);

        display_text(20,22,measurement[ch_num].actstr,0,BIG,NORMAL);


    }

    static void bkg_to_remote(short ch_num)
    {

        char test_type;

        test_type = '0' + TEST_BKG;
        remote_value(ch_num,&measurement[ch_num].actstr[0],test_type,bkgdata[ch_num].status);

    }

    void setup_for_background(short ch_num)
    {
        short index_57;
        short ch_type;

        ch_type = chamber_type(ch_num);


        //set to background mode
        measurement[ch_num].mode = BKGMODE;

        bkgdata[ch_num].done = FALSE;
        bkgdata[ch_num].status = TEST_WAIT;

        //blank activity string for case of requesting background too early
        strcpy(&measurement[ch_num].actstr[0],"         ");

        //set to most sensitive gain
        setchamber(ch_num,0);
        measurement[ch_num].gain = 0;
        measurement[ch_num].dogain = 2;
        measurement[ch_num].idecmin = 0;
        measurement[ch_num].idec = measurement[ch_num].idecmin;

        index_57 = NuclideData_getIndexFromName("Co57");
        measurement[ch_num].response = NuclideData_getResponse(index_57, ch_type);

        bkgdata[ch_num].lowbkg = -chamber[ch_num].bkg;
        if(current.demo_mode  && (current.pccomm != PC_COMM_USB_DEBUG_CHAMBER))
            bkgdata[ch_num].lowbkg = 0.;
        bkgdata[ch_num].sum = 0.;
        bkgdata[ch_num].count = 0;

        remote[ch_num].mode = REMOTE_BKG_MODE;

        // Locked: CI
        //if(current.system == CI)
        //    measurement[ch_num].kun = 1;
        //else
        //    measurement[ch_num].kun = 5;
        measurement[ch_num].kun = 1;

        bad_bkg = FALSE;
    }

    static void set_status(short ch_num)
    {
        if((measurement[ch_num].meas + chamber[ch_num].bkg) < 0.1)
            bkgdata[ch_num].status = TEST_GOOD;
        else
            bkgdata[ch_num].status = BKG_HIGH;
    }
