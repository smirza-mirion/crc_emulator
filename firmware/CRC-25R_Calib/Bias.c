/*********************************************************************
  MODULE:       BIAS VOLTAGE MEASUREMENT

  FILE:         Bias.c

  DATE:         10/13/06

  ANALYSIS:     steps user through Bias Measurement


  CALLED BY:
        Daily Test
        Test menu


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
#include "pit.h"
#include <string.h>
#include <stdlib.h>

extern CHAMBER chamber[];
extern CHAMBERVALS chamb_vals[];
extern CURRENT current;
extern MEASUREMENT measurement[];
extern BIASDATA biasdata[];
extern REMOTE remote[];
extern volatile bool AmuletPCQC_Abort;
extern time_t clock_time;
extern long long int AmuletDailyMenu_ChamberDailyTestID;

static void bias_to_remote(short ch_num);
static void display_bias(short ch_num);
void DB_CreateChamberVoltage(CHAMBERVOLTAGE *chambervoltage, long long int ChamberDailyTestID, bool bookEnd);
void trim(char *acByte);

    bool measure_bias(short daily_flag)
    {

        char ch;
        char test_type;
        short ch_num = current.main_chamber;
        CHAMBERVOLTAGE *chambervoltage;
        char *acMsg;

        //send IN PROGRESS message to remote if started at Readout or PC
        if(daily_flag != -1)
        {    
            test_type = '0' + TEST_BIAS;
            remote_in_progress(ch_num,test_type);
        }
        
        if(daily_flag != 1)
            erase_screen();
        
        //display_text(16,6,"BIAS VOLTAGE",0,MEDIUM,NORMAL);
        display_medium_message(BIAS_1,6,0,NORMAL);
        if(daily_flag == 0)
        {    
            //display_text(8,40,"REMOVE SOURCES",0,MEDIUM,NORMAL);
            display_medium_message(BIAS_2,40,0,NORMAL);
            key_pc_continue();
            if(AmuletPCQC_Abort) return FALSE;
            if(home_set()) return FALSE;
        }    

        setup_for_bias(ch_num);

		setchamber(ch_num, measurement[ch_num].gain);
        plwait(TRUE);

        measurement[ch_num].display_flag = FALSE;

        for(;;)  
        {
            if(measurement[ch_num].display_flag)
            {    
                display_bias(ch_num);
                measurement[ch_num].display_flag = FALSE;
            }    
            do
            {
                ch = getkey();
                if(AmuletPCQC_Abort) {
               		measurement[ch_num].dogain = 1;
					erase_screen();

					//release Test relay keeping same gain
					measurement[ch_num].mode = MEASMODE;
					setchamber(ch_num, measurement[ch_num].gain);
					if(measurement[ch_num].gain != 0){
						delay_msec(1000);
						setchamber(ch_num, 0);
					}
                    return FALSE;
                }

                if(home_set()) {
					measurement[ch_num].dogain = 1;
					erase_screen();

					//release Test relay keeping same gain
					measurement[ch_num].mode = MEASMODE;
					setchamber(ch_num, measurement[ch_num].gain);
					if(measurement[ch_num].gain != 0){
						delay_msec(1000);
						setchamber(ch_num, 0);
					}					
                    return FALSE;
				}
                if(biasdata[ch_num].status != TEST_WAIT)
                {    
                    if(test_done(ch,ch_num,TEST_BIAS))
                    {
                        biasdata[ch_num].done = TRUE;
                        break;  //break from do loop
                    }
                }    
                else
                {    
                    if(ch != NOKEY)
                        beep();  /* other key */
                }
                if (!chamber[ch_num].connected_flag)
                {
                    return FALSE;
                }
            }while(!measurement[ch_num].valid_flag);

            if(biasdata[ch_num].done)
                break;
        }

        if((chamber[ch_num].control == CONTROL_PC) || (chamber[ch_num].control == CONTROL_REMOTE)){
        	chambervoltage = (CHAMBERVOLTAGE *) malloc(sizeof(CHAMBERVOLTAGE));
        	acMsg = (char *) malloc(100);
        	chambervoltage->ChamberVoltageID = 0;
			chambervoltage->ChamberSerialNumber[6] = 0;
			strncpy(chambervoltage->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
			chambervoltage->ChamberType = chamb_vals[ch_num].chamb_type;
			chambervoltage->TwoStageChamber = chamber_one_gain_relay(ch_num);
			chambervoltage->NominalVoltage = chamber_nominal_volts(ch_num);
			chambervoltage->MinVoltage = biasdata[ch_num].minvolts;
			chambervoltage->MaxVoltage = biasdata[ch_num].maxvolts;
			chambervoltage->VoltageStatus = biasdata[ch_num].status;
			chambervoltage->VoltageValue = biasdata[ch_num].measvolts;
			chambervoltage->MeasuredOn = clock_time;
			chambervoltage->InactiveReason[0] = 0;
			chambervoltage->Inactive = FALSE;
			strcpy(chambervoltage->VoltageTextEnglish, measurement[ch_num].actstr);
			trim(chambervoltage->VoltageTextEnglish);
			strcpy(chambervoltage->VoltageTextFrench, chambervoltage->VoltageTextEnglish);
			*acMsg = 0;
			if(biasdata[ch_num].status == TEST_GOOD){
				get_amulet_message(L_CAPS_OK, acMsg);    // "OK"
			}else if(biasdata[ch_num].status == BIAS_TEST_FAIL){
				get_amulet_message_with_language(L_ERROR, acMsg, ENGLISH);    // "ERROR"
				strcat(chambervoltage->VoltageTextEnglish, " ");
				strcat(chambervoltage->VoltageTextEnglish, acMsg);

				get_amulet_message_with_language(L_ERROR, acMsg, FRENCH);    // "ERROR"
				strcat(chambervoltage->VoltageTextFrench, " ");
				strcat(chambervoltage->VoltageTextFrench, acMsg);
			}
			if(chamber[ch_num].control == CONTROL_PC) DB_CreateChamberVoltage(chambervoltage, AmuletDailyMenu_ChamberDailyTestID, TRUE);
			else DB_CreateChamberVoltage(chambervoltage, 0, TRUE);
			free(acMsg);
        	free(chambervoltage);
        }
            
        measurement[ch_num].dogain = 1;
        erase_screen();

        //release Test relay keeping same gain
        measurement[ch_num].mode = MEASMODE;
        setchamber(ch_num,measurement[ch_num].gain);
        if(measurement[ch_num].gain != 0)
        {
            delay_msec(1000);
            setchamber(ch_num,0);
        }   


        //if started from Readout, release Remote keys
        if(daily_flag == 1)
            remote_in_progress(ch_num,'0');
       

        if(biasdata[ch_num].status == BIAS_TEST_FAIL)
            return FALSE;
        return TRUE;


    }

    void setup_for_bias(short ch_num)
    {

        float err,nom;

        measurement[ch_num].mode = TESTMODE;
        biasdata[ch_num].status = TEST_WAIT;
        biasdata[ch_num].wait = TRUE;
        biasdata[ch_num].done = FALSE;
        //blank activity string for case of requesting Test result too early
        strcpy(&measurement[ch_num].actstr[0],"         ");
        
        err = 0.05;
        nom = chamber_nominal_volts(ch_num);
        
        biasdata[ch_num].minvolts = (1. - err) * nom;
        biasdata[ch_num].maxvolts = (1. + err) * nom;

        measurement[ch_num].gain = 2;


        //set up for Bias measurement
        set_bias(ch_num);
        measurement[ch_num].dogain = 1;
        biasdata[ch_num].count = 0;

        remote[ch_num].mode = REMOTE_BIAS_MODE;

        measurement[ch_num].kun = 8; //V


    }   

    void read_bias(short ch_num)
    {

        float sysvolts;
        char strng[30];
        char str[10];
        short nc;

        short ch_type;

        ch_type = chamber_type(ch_num);
        if(biasdata[ch_num].count <= 10)
        {
            ++biasdata[ch_num].count;
            bias_to_remote(ch_num);            
            return;
        }    

        if (biasdata[ch_num].wait && measurement[ch_num].prevgain == 2 && biasdata[ch_num].count > 10)
        {
            if(ch_num == current.main_chamber)
                if(get_crc_mode() == CRC_CAL)
                    plwait(FALSE);
            {    
                if(chamber[ch_num].control == CONTROL_MAIN)
                {
                    //display_text(18,56,"ENTER to Accept",0,SMALL,NORMAL);
                    display_small_message(AUTO_ZERO_8,56,0,NORMAL);
                }    
            }    
            
            biasdata[ch_num].wait = FALSE;
            return;
            
        }

        if(biasdata[ch_num].wait)
        {    
            ++biasdata[ch_num].count;
            if(biasdata[ch_num].count >= 16)
            {
                if(ch_num == current.main_chamber)
                {    
                    //plwait(FALSE);
                    if(chamber[ch_num].control == CONTROL_MAIN)
                    {    
                        plwait(FALSE);
                        //display_text(18,56,"ENTER to Accept",0,SMALL,NORMAL);
                        display_small_message(AUTO_ZERO_8,56,0,NORMAL);
                    }    
                }
                biasdata[ch_num].wait = FALSE;
            }
            return;
        }

        if(current.demo_mode)
            sysvolts = (biasdata[ch_num].minvolts + biasdata[ch_num].maxvolts) / 2.;
        else
        	if(chamber_one_gain_relay(ch_num)){
        		sysvolts = get_volts_factor(0) * measurement[ch_num].volts;
        	}else{
        		sysvolts = get_volts_factor(ch_type) * measurement[ch_num].volts;
        	}
        strcpy(strng,"      V  ");
        nc = sprintf(str,"%6.1f",sysvolts);
        strncpy(strng,str,nc);
        biasdata[ch_num].measvolts = sysvolts;

        //send data string to measurement structure
        strcpy(&measurement[ch_num].actstr[0],strng);

        
        if (sysvolts < biasdata[ch_num].minvolts || sysvolts > biasdata[ch_num].maxvolts)
            biasdata[ch_num].status = BIAS_TEST_FAIL;
        else
            biasdata[ch_num].status = TEST_GOOD;

        measurement[ch_num].display_flag = TRUE;
        bias_to_remote(ch_num);
    }
    

    static void bias_to_remote(short ch_num)
    {
        char test_type;
        
        //send to remote
        test_type = '0' + TEST_BIAS;
        remote_value(ch_num,&measurement[ch_num].actstr[0],test_type,biasdata[ch_num].status);
    }    



    static void display_bias(short ch_num)
    {

        display_text(16,22,&measurement[ch_num].actstr[0],0,BIG,NORMAL);

        if(biasdata[ch_num].status == BIAS_TEST_FAIL)
        {    
            display_text(88,22,"FAIL",0,BIG,NORMAL);
            seeman();
            if(chamber[ch_num].control == CONTROL_MAIN)
                //display_text(0,56,"ENTER to Acknowledge",0,SMALL,NORMAL);
                display_small_message(BIAS_3,56,0,NORMAL);
        }
        else
            display_text(10,38,"    OK     ",0,BIG,NORMAL);
        
    }    
