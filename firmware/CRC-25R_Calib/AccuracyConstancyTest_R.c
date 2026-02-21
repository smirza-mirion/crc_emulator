/*********************************************************************

  MODULE:	ACCURACY and CONSTANCY TEST for CRC-25R,W

  FILE:		AccuracyConstancyTest_R.c

  DATE:	    01/24/07
            08/29/07 -- removed automated constancy
	    11/3/08 -- changed font for USB printers

  ANALYSIS:	 controls accuracy test measurement for each standard source
             performs constancy test in Daily mode

  DESIGN: 	steps user through accuracy test, performs measurement


  PASSED PARAMETERS:  daily -- true if called by daily test

  CALLED BY:
		tests
		daily


  *************************************************************************/
#include "crc.h"
#include "i2c.h"
#include "screen.h"
#include "printer.h"
#include "keyboard.h"
#include "uart.h"
#include "chambfac.h"
#include "daily.h"
#include "message.h"
#include "remote.h"
#include "nuc.h"
#include <string.h>
#include <math.h>

void dispdev(float diff);
// Removed: Conversion function is not needed
//static	float convact(float act0);
static void constancy_test(void);
void prconstancysection_db(short printer, char *header_name);
//static void auto_constancy_test(void);
void disp_pract(short nacc, short ch_num);
short get_num_sources(bool daily);
void convert_printing_serial(char *strng0, char *strng);
void prchambersection_db(time_t test_time, char *chamber_sn, int chamber_type, bool two_stage, char *header_name);
void prinactive_db(char *inactive_reason, char pter, char *header_name);
void AmuletGenericYesNo_saveInDatabase(bool autoconstancy);
static bool read_stand(short niso);

	extern const char *unit_str[];
	extern CURRENT current;
	extern CHAMBER  chamber[];
    extern MEASUREMENT measurement[];
    extern const short max_chambers;

extern DAILY_TEST_RES  daily_res[];
extern short num_daily;
extern volatile bool AmuletPCQC_Abort;
extern CHAMBERACCURACYMEASUREMENT	AmuletChamberSearchMenu_AccuracyMeasurement[6];
extern CHAMBERAUTOCONSTANCY		AmuletChamberSearchMenu_AutoConstancy[12];
extern CHAMBERACCURACYTEST			AmuletChamberSearchMenu_AccuracyTest;
extern time_t clock_time;


ACCDATA acc_data[6];
static STAND stnd;
static MEASUREMENT last_measurement;

	short accuracy_test(bool daily)
	{


		bool okflag;
        short nacc;
		bool acc_flag;
		bool ynret;
		short nuc_index;
		bool meas_done;
        bool do_pr;
        char printer;
		short temp;
		short ciso;
		float diff;
		char ch;
        short acc_pass;
        short ch_num = current.main_chamber;


        //are there any standard sources?
        if(get_num_sources(FALSE) == 0)
        {
            if(chamber[ch_num].control == CONTROL_MAIN)
            {
                beep();
                erase_screen();
                //display_text(0,32,"NO SOURCE DATA",0,MEDIUM,NORMAL);
                display_medium_message(ACC_5,32,0,NORMAL);
                contmsg();
            }
            return -1;
        }

        //get number of daily sources
        num_daily = get_num_sources(TRUE);
        measurement[ch_num].future.dosetime = NO_TIME;

        // Removed: System is read at the beginning
        //EE_READ(syst, &current.system);

        if(daily)
        {

            if(chamber[ch_num].control == CONTROL_PC)
            {

                for(;;)
                {
                    if(get_pc_ret() == PC_NUM_DAILY)
                        break;
                    getkey();
                    if(AmuletPCQC_Abort) return -1;
                    if(home_set()) return -1;
                }
            }

            if(num_daily == 0)
            {
                beep();
                //display_text(8,22,"NO DAILY",0,MEDIUM,NORMAL);
                //display_text(8,32,"SOURCE DATA",0,MEDIUM,NORMAL);
                display_medium_message(ACC_1,22,0,NORMAL);
                display_medium_message(ACC_2,32,0,NORMAL);
                key_pc_continue();
                return -1;
            }

            //find constancy source
            ciso = get_const_source();

            get_acc_data(ciso,TRUE,ch_num);

            if(chamber[ch_num].control == CONTROL_PC)
            {

                for(;;)
                {
                    if(get_pc_ret() == PC_ACC_SOURCES)
                        break;
                    getkey();
                    if(AmuletPCQC_Abort) return -1;
                    if(home_set()) return -1;
                }
            }
        }
        else  //not Daily
        {
            get_acc_data(0,FALSE,ch_num);
        }


		acc_flag = FALSE;

        //measure each accuracy test source
        for(nacc = 0; nacc < 5; nacc++)
		{

            if(strlen(&acc_data[nacc].nucname[0]) == 0)
                break;      // no more sources

            if(daily && acc_data[nacc].daily[0] == FALSE)
                continue;

			erase_screen();
            set_meas_screen(FALSE);
			okflag = FALSE;
			//display_text(8,12,"MEASURE",0,MEDIUM,NORMAL);
            display_medium_message(ACC_3,12,0,NORMAL);
            display_text(72,12,&acc_data[nacc].nucname[0],0,MEDIUM,NORMAL);

			//display_text(8,32,"S/N:",0,MEDIUM,NORMAL);
            display_medium_message(ACC_4,32,0,NORMAL);
			//display_text(40,32,&stnd.sn[0],0,MEDIUM,NORMAL);
            display_text(40,32,&acc_data[nacc].sn[0],0,MEDIUM,NORMAL);
			if (daily)
                key_pc_continue();
			else
			{
				//display_text(8,42,"Y OR N ?",0,MEDIUM,NORMAL);
                display_medium_message(YES_OR_NO,48,0,NORMAL);
                ynret = yesorno();
                if(AmuletPCQC_Abort) return -1;
                if(home_set()) return -1;

                if(!ynret)
				{
					erase_screen();
					continue;
				}
			}
            acc_flag = TRUE;        //set accuracy test flag
			erase_screen();
            nuc_index = acc_data[nacc].nuc_index;
            set_nuclide_data(nuc_index,ch_num);
			disp_nuclide();

            disp_pract(nacc,ch_num);

            if(chamber[ch_num].control == CONTROL_MAIN)
            {
                //display_text(0,56,"ENTER to Accept",0,SMALL,NORMAL);
                display_small_message(AUTO_ZERO_8,56,0,NORMAL);
                set_nokey();
            }
			meas_done = FALSE;

			do
			{

                ch = getkey();
                if(AmuletPCQC_Abort) return -1;
                if(home_set()) return -1;


                if(get_pc_ret() == PC_ACCEPT && okflag)
                {
                    clear_pc_ret();
                    ch = OK;        //same as if ENTER pressed
                }


				switch (ch)
				{
				case NOKEY:
					break;

				case DOWN:
					downbut();
					break;

                case ZERO:
				case ONE:
				case TWO:
				case THREE:
				case FOUR:
				case FIVE:
				case SIX:
				case SEVEN:
				case EIGHT:
				case NINE:
					dokey(ch);
					break;

                case CLR:
                    keybuf_bsp();
					break;

				case OK:
					if (!okflag)
					{
						beep();
						break;
					}
                    set_idnum();
                    erase_text_line(56,0,SMALL);
					meas_done = TRUE;
					break;

				default:
					beep();
					break;


				}

				if(!meas_done)
				{
					/* display */
					if (measurement[ch_num].display_flag_2)
					{
                        activity_to_screen();
                        measurement[ch_num].display_flag_2 = FALSE;
                        if(measurement[ch_num].over_flag){
                        	diff = 0;
                        	acc_data[nacc].test_res[ch_num].ms_kun = 0;
                        	strcpy(acc_data[nacc].test_res[ch_num].msact, "           ");
                        	strcpy(acc_data[nacc].test_res[ch_num].var, "      ");
                        }else{
                        	diff = calc_dev(nacc,ch_num);
                        	dispdev(diff);
                        }
                        okflag = TRUE;
					}
				}
			}while(!meas_done);

			acc_data[nacc].test_res[ch_num].pr_kun = format_activity_system(acc_data[nacc].test_res[ch_num].pred_act, acc_data[nacc].test_res[ch_num].pract);
			acc_data[nacc].test_res[ch_num].diff = diff;
			acc_data[nacc].test_res[ch_num].measuredon = clock_time;
			acc_data[nacc].test_res[ch_num].resp0 = measurement[ch_num].resp0;
			acc_data[nacc].test_res[ch_num].over_flag = measurement[ch_num].over_flag;
			if(!acc_data[nacc].test_res[ch_num].over_flag) acc_data[nacc].test_res[ch_num].ms_kun = format_activity_system(acc_data[nacc].test_res[ch_num].ms_act, acc_data[nacc].test_res[ch_num].msact);

			if(acc_data[nacc].const_source){
				memcpy(&(measurement[8]), &last_measurement, sizeof(MEASUREMENT));
			}

			if (diff > 20. || diff < -20.)
			{
				display_text(70,40,"ERROR",0,BIG,NORMAL);
				temp = -1;
				key_pc_continue();
				if(AmuletPCQC_Abort) return -1;
                if(home_set()) return -1;
			}

            if(chamber[ch_num].control == CONTROL_MAIN)
            {
                if(daily && acc_data[nacc].const_source)
                {
                    if(temp == -1)
                        acc_pass = 0;
                    else
                    {
                        acc_pass = 1;
                        constancy_test();
                    }
                    if(AmuletPCQC_Abort) return -1;
                    if(home_set()) return -1;
                }
            }

		} /* end nacc loop */

        if(chamber[ch_num].control == CONTROL_PC){
        	AmuletGenericYesNo_saveInDatabase(FALSE);
        }

        //save accuracy test data to daily_res
        save_acc_to_daily_res(ch_num);

		if(daily)
		{

            if(chamber[ch_num].control == CONTROL_MAIN)
            {
                if(ciso == -1)
                {
                    beep();
                    erase_screen();
                    //display_text(8,22,"NO CONSTANCY",0,MEDIUM,NORMAL);
                    //display_text(8,32,"SOURCE",0,MEDIUM,NORMAL);
                    display_medium_message(CONST_1,22,0,NORMAL);
                    display_medium_message(CONST_2,32,0,NORMAL);
                    contmsg();
                    return 0;
                }
            }
            return acc_pass;

		}


        /* only gets here if not daily
           accuracy test done, do printing here */
        if(!acc_flag)   //no accuracy tests to print
            return 1;

        erase_screen();
        printer = current.printer;

        do
        {
            switch(printer)
            {
            case NONE_PRINTER:
                return 1;
            case ROLL_PRINTER:
                do_pr = TRUE;
                break;
            case SLIP_PRINTER:
                do_pr = insert_paper(PAPER,1);
                break;
            case OKI_PRINTER:
                do_pr = check_oki(PAPER);
                break;
            }

            if(do_pr)
            {
                prhead_language(printer);
                pr_set_linecnt(5);
                print_accuracy_test(printer);
                dosig(printer);
            }
            ynret = print_again();
            erase_screen();
        }while(ynret);

		return 1;
	}

short Amulet_printAccuracyAndConstancy(bool printconstancy){
	char printer;

	printer = current.printer;

	if(!start_printer(printer,1,FALSE,PAPER)) return 0;
	prhead_languagesec(printer);
	pr_set_linecnt(5);
	print_accuracy_test(printer);

	if(printconstancy){
		feed(2,printer);
		print_constancy_test(printer);
	}
	return 1;
}


    extern char smu[];
    void disp_pract(short nacc, short ch_num)
    {
        short kun;
        char str[4];
        char strng[12];


        display_text(72,0,&acc_data[nacc].test_res[ch_num].pract[0],0,SMALL,NORMAL);
        kun = acc_data[nacc].test_res[ch_num].pr_kun;
        strncpy(strng,unit_str[kun - 1],3);
        if (kun == UCI)
            write_special(smu,108,0, SMALL,0);
        else
        {
            str[0] = strng[0];
            str[1] = '\0';
            display_text(108,0,str,0,SMALL,NORMAL);
        }
        strncpy(str,&strng[1],2);
        str[2] = '\0';
        display_text(114,0,str,0,SMALL,NORMAL);
    }



void dispdev(float diff)
	{

        char buf[20];

        //display_text(0,40,"Deviation:",0,SMALL,NORMAL);
        display_small_message(ACC_6,40,0,NORMAL);

		if (diff > -20. && diff < 20.)
		{
            sprintf(buf,"%5.1f",diff);
            display_text(60,40,buf,0,BIG,NORMAL);

		}
		else
            display_text(60,40," --  ",0,BIG,NORMAL);

        display_text(110,40,"%",0,BIG,NORMAL);
	}



	// Removed: Conversion function to other unit is not needed
    //static	float convact(float act0)
	//{
	//	if (current.system == CI)
	//		return (act0 / BQFACTOR);
	//	else
	//		return (act0 * BQFACTOR);
	//}

void print_accuracy_test(short prtype){
	short i,k;
	short kp;
	char head[5][19];
	char nc;
	char stndstr[8];
	char strng[90], strng1[200];
	char sn[11], tempSECTION[40];
	short ch_num = current.main_chamber;

	if(prtype == NONE_PRINTER) return;

	if(prtype == USB_PRINTER) pcl_cpi("12",TRUE);
	if(prtype == USB_EPS_PRINTER) eps_cpi(12);

	feed(2,prtype);

	if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
	lininit(strng,TRUE,prtype);
	//strncpy(&strng[4],"ACCURACY TEST:",14);
	//get_message(ACC_7,&strng[4]);
	get_amulet_message(L_ACCURACYTEST, tempSECTION);    // "ACCURACY TEST:"
	insertconst(strng, 0, 0, 0, tempSECTION);
	pr_write(strng);
	if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);

	feed(1,prtype);

	strncpy(&head[0][0],"                   ",19);
	//strncpy(&head[0][0],"Source:          ",17);
	//get_message(ACC_8,&head[0][0]);
	get_amulet_message(L_SOURCE, tempSECTION);    // "Source:"
	strncpy(&head[0][0], tempSECTION, strlen(tempSECTION));

	strncpy(&head[1][0],"                   ",19);
	if(prtype == ROLL_PRINTER || prtype == SLIP_PRINTER){
		//strncpy(&head[1][0],"S/N:             ",17);
		//get_message(ACC_10,&head[1][0]);
		get_amulet_message(L_SN2, tempSECTION);    // "S/N:"
	}else{
		//strncpy(&head[1][0],"Serial #:        ",17);
		//get_message(ACC_9,&head[1][0]);
		get_amulet_message(L_SERIALNUM, tempSECTION);    // "Serial #:"
	}
	strncpy(&head[1][0], tempSECTION, strlen(tempSECTION));

	strncpy(&head[2][0],"                   ",19);
	//strncpy(&head[2][0],"Stand. Source(S):",17);
	//get_message(ACC_11,&head[2][0]);
	get_amulet_message(L_STANDSOURCE, tempSECTION);    // "Stand. Source(S):"
	strncpy(&head[2][0], tempSECTION, strlen(tempSECTION));

	strncpy(&head[3][0],"                   ",19);
	//strncpy(&head[3][0],"Measured As  (M):",17);
	//get_message(ACC_12,&head[3][0]);
	get_amulet_message(L_MEASUREDAS, tempSECTION);    // "Measured As  (M):"
	strncpy(&head[3][0], tempSECTION, strlen(tempSECTION));

	strncpy(&head[4][0],"                   ",19);
	//strncpy(&head[4][0],"Deviation(M-S)/S:",17);
	//get_message(ACC_13,&head[4][0]);
	get_amulet_message(L_DEVIATIONMSS, tempSECTION);    // "Deviation(M-S)/S:"
	strncpy(&head[4][0], tempSECTION, strlen(tempSECTION));

	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER){
		/* print names */
		lininit(strng,TRUE,prtype);
		strncpy(&strng[0],&head[0][0],19);
		k = -1;
		for(i = 0; i < 5; i++)
		{
			if (acc_data[i].test_res[ch_num].ms_kun != -1)
			{
				++k;
				strcpy(stndstr,&acc_data[i].nucname[0]);
				strncpy(&strng[23 + 12 * k],stndstr,strlen(stndstr));
			}
		}
		pr_write(strng);

		// print sn
		lininit(strng,TRUE,prtype);
		strncpy(&strng[0],&head[1][0],19);
		k = -1;
		for(i = 0; i < 5; i++)
		{
			if (acc_data[i].test_res[ch_num].ms_kun != -1)
			{
				++k;
				stringtofixed(sn, acc_data[i].sn, 11);
				//strncpy(&strng[20 + 12 * k], sn, 10);
				strncpy(&strng[30 + (12 * k) - strlen(acc_data[i].sn)], sn, strlen(acc_data[i].sn));
			}
		}
		pr_write(strng);

		// predicted activity
		lininit(strng,TRUE,prtype);
		strncpy(&strng[0],&head[2][0],19);
		k = -1;
		for(i = 0; i < 5; i++)
		{
			if(acc_data[i].test_res[ch_num].ms_kun != -1)
			{
				++k;
				kp = 12 * k + 21;
				strncpy(&strng[kp],&acc_data[i].test_res[ch_num].pract[0],6);
				kp += 6;

				strncpy(&strng[kp],unit_str[acc_data[i].test_res[ch_num].pr_kun - 1],3);
				if(acc_data[i].test_res[ch_num].pr_kun == UCI)
					strng[kp] = MU_PRT;
				pr_mu(&strng[kp],prtype);

			}
		}
		pr_write(strng);

		// measured activity
		lininit(strng,TRUE,prtype);
		strncpy(&strng[0],&head[3][0],19);
		k = -1;
		for(i = 0; i < 5; i++)
		{
			if (acc_data[i].test_res[ch_num].ms_kun != -1)
			{
				++k;
				kp = 12 * k + 21;  //18;
				strncpy(&strng[kp],&acc_data[i].test_res[ch_num].msact[0],6);
				kp += 6;
				strncpy(&strng[kp],unit_str[acc_data[i].test_res[ch_num].ms_kun - 1],3);
				//if(acc_data[i].test_res[ch_num].ms_kun == UCI)
				//	strng[kp] = MU_PRT;
				pr_mu(&strng[kp],prtype);
			}
		}
		pr_write(strng);

		// variation
		lininit(strng,TRUE,prtype);
		strncpy(&strng[0],&head[4][0],19);
		k = -1;
		for(i = 0; i < 5; i++)
		{
			if (acc_data[i].test_res[ch_num].ms_kun != -1)
			{
				++k;
				nc = acc_data[i].test_res[ch_num].nc;
				if (nc == -1)
					//strncpy(&strng[20 + 12 * k],"ERROR",5);
					get_message(DAILY_5,&strng[22 + 12 * k]);
				else
				{
					strncpy(&strng[27 + 12 * k - nc],&acc_data[i].test_res[ch_num].var[0],nc);
					strng[27 + 12 * k] = '%';
				}
			}
		}
		pr_write(strng);

		if (acc_data[5].test_res[ch_num].ms_kun != -1){
			lininit(strng,TRUE,prtype);
			pr_write(strng);

			/* print names */
			lininit(strng,TRUE,prtype);
			strncpy(&strng[0],&head[0][0],19);
			k = -1;
			++k;
			strcpy(stndstr,&acc_data[5].nucname[0]);
			strncpy(&strng[23 + 12 * k],stndstr,strlen(stndstr));
			pr_write(strng);

			// print sn
			lininit(strng,TRUE,prtype);
			strncpy(&strng[0],&head[1][0],19);
			k = -1;
			++k;
			stringtofixed(sn, acc_data[5].sn, 11);
			strncpy(&strng[30 + (12 * k) - strlen(acc_data[5].sn)], sn, strlen(acc_data[5].sn));
			pr_write(strng);

			// predicted activity
			lininit(strng,TRUE,prtype);
			strncpy(&strng[0],&head[2][0],19);
			k = -1;
			++k;
			kp = 12 * k + 21;
			strncpy(&strng[kp],&acc_data[5].test_res[ch_num].pract[0],6);
			kp += 6;
			strncpy(&strng[kp],unit_str[acc_data[5].test_res[ch_num].pr_kun - 1],3);
			if(acc_data[5].test_res[ch_num].pr_kun == UCI)
				strng[kp] = MU_PRT;
			pr_mu(&strng[kp],prtype);
			pr_write(strng);

			// measured activity
			lininit(strng,TRUE,prtype);
			strncpy(&strng[0],&head[3][0],19);
			k = -1;
			++k;
			kp = 12 * k + 21;  //18;
			strncpy(&strng[kp],&acc_data[5].test_res[ch_num].msact[0],6);
			kp += 6;
			strncpy(&strng[kp],unit_str[acc_data[5].test_res[ch_num].ms_kun - 1],3);
			//if(acc_data[i].test_res[ch_num].ms_kun == UCI)
			//	strng[kp] = MU_PRT;
			pr_mu(&strng[kp],prtype);
			pr_write(strng);

			// variation
			lininit(strng,TRUE,prtype);
			strncpy(&strng[0],&head[4][0],19);
			k = -1;
			++k;
			nc = acc_data[5].test_res[ch_num].nc;
			if (nc == -1)
				//strncpy(&strng[20 + 12 * k],"ERROR",5);
				get_message(DAILY_5,&strng[22 + 12 * k]);
			else
			{
				strncpy(&strng[27 + 12 * k - nc],&acc_data[5].test_res[ch_num].var[0],nc);
				strng[27 + 12 * k] = '%';
			}
			pr_write(strng);
		}

		return;
	}

	// Roll Printer & Slip Printer
	for(i = 0; i < 6; i++)
	{
		if (acc_data[i].test_res[ch_num].ms_kun != -1)
		{
			//serial number
			lininit(strng,TRUE,prtype);
			strncpy(&strng[1],&head[0][0],7);
			strcpy(stndstr,&acc_data[i].nucname[0]);
			strncpy(&strng[9],stndstr,strlen(stndstr));
			strncpy(&strng[16],&head[1][0],4);
			stringtofixed(sn, acc_data[i].sn, 11);
			strncpy(&strng[21], sn, 10);
			convert_printing_serial(strng, strng1);
			uart_write(strng1,U_PR);
			feed(1,prtype);

			// predicted activity
			lininit(strng,TRUE,prtype);
			strncpy(&strng[1],&head[2][0],19);
			strncpy(&strng[22],&acc_data[i].test_res[ch_num].pract[0],6);
			strncpy(&strng[28],unit_str[acc_data[i].test_res[ch_num].pr_kun - 1],3);
			if(acc_data[i].test_res[ch_num].pr_kun == UCI) strng[28] = MU_PRT;
			convert_printing_serial(strng, strng1);
			uart_write(strng1,U_PR);

			// measured activity
			lininit(strng,TRUE,prtype);
			strncpy(&strng[1],&head[3][0],17);
			strncpy(&strng[22],&acc_data[i].test_res[ch_num].msact[0],6);
			strncpy(&strng[28],unit_str[acc_data[i].test_res[ch_num].ms_kun - 1],3);
			if(acc_data[i].test_res[ch_num].ms_kun == UCI) strng[28] = MU_PRT;
			convert_printing_serial(strng, strng1);
			uart_write(strng1,U_PR);

			// variation
			lininit(strng,TRUE,prtype);
			strncpy(&strng[1],&head[4][0],17);
			nc = acc_data[i].test_res[ch_num].nc;
			if (nc == -1){
				//strncpy(&strng[19],"ERROR",5);
				//get_message(DAILY_5,&strng[19]);
				get_amulet_message(L_ERROR, tempSECTION);    // "ERROR"
				strncpy(&strng[26], tempSECTION, strlen(tempSECTION));
			}else{
				strncpy(&strng[29 - nc],&acc_data[i].test_res[ch_num].var[0],nc);
				strng[30] = '%';
			}
			convert_printing_serial(strng, strng1);
			uart_write(strng1,U_PR);
			feed(2,prtype);
		}
	}
}

void praccuracysection_db(short prtype, char *header_name, bool print_chamber_section){
	short i, k, kp, length;
	char sn[11], head[5][19], stndstr[8], strng[90], strng1[200], tempSECTION[40], strng_empty[90];
	bool flg_first;

	lininit_db(strng, TRUE, prtype);
	if(prtype == NONE_PRINTER) return;

	if(print_chamber_section){
		prchambersection_db(AmuletChamberSearchMenu_AccuracyTest.CreatedOn, AmuletChamberSearchMenu_AccuracyTest.ChamberSerialNumber, AmuletChamberSearchMenu_AccuracyTest.ChamberType, AmuletChamberSearchMenu_AccuracyTest.TwoStageChamber, header_name);
		feed_db(1, prtype);
	}

	if(prtype == USB_PRINTER) pcl_cpi("12", TRUE);
	if(prtype == USB_EPS_PRINTER) eps_cpi(12);

	if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
	linupdate_db(strng, FALSE, TRUE);

	get_amulet_message(L_ACCURACYTEST, tempSECTION);    // "ACCURACY TEST:"
	insertconst(strng, 0, 0, 0, tempSECTION);
	pr_write2_language_dbsec(strng, header_name);

	if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);
	linupdate_db(strng, FALSE, FALSE);

	strncpy(&head[0][0], "                   ", 19);
	get_amulet_message(L_SOURCE, tempSECTION);    // "Source:"
	strncpy(&head[0][0], tempSECTION, strlen(tempSECTION));

	strncpy(&head[1][0], "                   ", 19);
	if(prtype == ROLL_PRINTER || prtype == SLIP_PRINTER) get_amulet_message(L_SN2, tempSECTION);    // "S/N:"
	else get_amulet_message(L_SERIALNUM, tempSECTION);    // "Serial #:"
	strncpy(&head[1][0], tempSECTION, strlen(tempSECTION));

	strncpy(&head[2][0], "                   ", 19);
	get_amulet_message(L_STANDSOURCE, tempSECTION);    // "Stand. Source(S):"
	strncpy(&head[2][0], tempSECTION, strlen(tempSECTION));

	strncpy(&head[3][0], "                   ", 19);
	get_amulet_message(L_MEASUREDAS, tempSECTION);    // "Measured As  (M):"
	strncpy(&head[3][0], tempSECTION, strlen(tempSECTION));

	strncpy(&head[4][0], "                   ", 19);
	get_amulet_message(L_DEVIATIONMSS, tempSECTION);    // "Deviation(M-S)/S:"
	strncpy(&head[4][0], tempSECTION, strlen(tempSECTION));

	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER){
		/* print names */
		strncpy(&strng[0], &head[0][0], 19);
		k = -1;
		for(i=0; i<5; i++){
			if(AmuletChamberSearchMenu_AccuracyMeasurement[i].ChamberAccuracyMeasurementID > 0){
				++k;
				strcpy(stndstr, AmuletChamberSearchMenu_AccuracyMeasurement[i].SourceNuclide);
				strncpy(&strng[23 + 12 * k], stndstr, strlen(stndstr));
			}
		}
		pr_write2_language_dbsec(strng, header_name);

		// print sn
		strncpy(&strng[0], &head[1][0], 19);
		k = -1;
		for(i=0; i<5; i++){
			if(AmuletChamberSearchMenu_AccuracyMeasurement[i].ChamberAccuracyMeasurementID > 0){
				++k;
				stringtofixed(sn, AmuletChamberSearchMenu_AccuracyMeasurement[i].SourceSerialNumber, 11);
				strncpy(&strng[30 + (12 * k) - strlen(AmuletChamberSearchMenu_AccuracyMeasurement[i].SourceSerialNumber)], sn, strlen(AmuletChamberSearchMenu_AccuracyMeasurement[i].SourceSerialNumber));
			}
		}
		pr_write2_language_dbsec(strng, header_name);

		// predicted activity
		strncpy(&strng[0], &head[2][0], 19);
		k = -1;
		for(i=0; i<5; i++){
			if(AmuletChamberSearchMenu_AccuracyMeasurement[i].ChamberAccuracyMeasurementID > 0){
				++k;
				kp = 12 * k + 30;
				strng1[0] = 0;
				if(current.language == ENGLISH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[i].CalculatedActivityTextEnglish);
				else if(current.language == FRENCH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[i].CalculatedActivityTextFrench);
				length = strlen(strng1);
				strncpy(&strng[kp-length], strng1, length);
			}
		}
		pr_write2_language_dbsec(strng, header_name);

		// measured activity
		strncpy(&strng[0], &head[3][0], 19);
		k = -1;
		for(i=0; i<5; i++){
			if(AmuletChamberSearchMenu_AccuracyMeasurement[i].ChamberAccuracyMeasurementID > 0){
				++k;
				kp = 12 * k + 30;
				strng1[0] = 0;
				if(current.language == ENGLISH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[i].MeasuredActivityTextEnglish);
				else if(current.language == FRENCH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[i].MeasuredActivityTextFrench);
				length = strlen(strng1);
				strncpy(&strng[kp-length], strng1, length);
			}
		}
		pr_write2_language_dbsec(strng, header_name);

		// variation
		strncpy(&strng[0], &head[4][0], 19);
		k = -1;
		for(i=0; i<5; i++){
			if(AmuletChamberSearchMenu_AccuracyMeasurement[i].ChamberAccuracyMeasurementID > 0){
				++k;
				kp = 12 * k + 28;
				strng1[0] = 0;
				if(current.language == ENGLISH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[i].DeviationTextEnglish);
				else if(current.language == FRENCH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[i].DeviationTextFrench);
				length = strlen(strng1);
				strncpy(&strng[kp - length], strng1, length);
			}
		}
		pr_write2_language_dbsec(strng, header_name);

		if(AmuletChamberSearchMenu_AccuracyMeasurement[5].ChamberAccuracyMeasurementID > 0){
			lininit_db(strng_empty, TRUE, prtype);
			pr_write2_language_dbsec(strng_empty, header_name);

			/* print names */
			strncpy(&strng[0], &head[0][0], 19);
			k = -1;
			++k;
			strcpy(stndstr, AmuletChamberSearchMenu_AccuracyMeasurement[5].SourceNuclide);
			strncpy(&strng[23 + 12 * k], stndstr, strlen(stndstr));
			pr_write2_language_dbsec(strng, header_name);

			// print sn
			strncpy(&strng[0], &head[1][0], 19);
			k = -1;
			++k;
			stringtofixed(sn, AmuletChamberSearchMenu_AccuracyMeasurement[5].SourceSerialNumber, 11);
			strncpy(&strng[30 + (12 * k) - strlen(AmuletChamberSearchMenu_AccuracyMeasurement[5].SourceSerialNumber)], sn, strlen(AmuletChamberSearchMenu_AccuracyMeasurement[5].SourceSerialNumber));
			pr_write2_language_dbsec(strng, header_name);

			// predicted activity
			strncpy(&strng[0], &head[2][0], 19);
			k = -1;
			++k;
			kp = 12 * k + 30;
			strng1[0] = 0;
			if(current.language == ENGLISH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[5].CalculatedActivityTextEnglish);
			else if(current.language == FRENCH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[5].CalculatedActivityTextFrench);
			length = strlen(strng1);
			strncpy(&strng[kp-length], strng1, length);
			pr_write2_language_dbsec(strng, header_name);

			// measured activity
			strncpy(&strng[0], &head[3][0], 19);
			k = -1;
			++k;
			kp = 12 * k + 30;
			strng1[0] = 0;
			if(current.language == ENGLISH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[5].MeasuredActivityTextEnglish);
			else if(current.language == FRENCH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[5].MeasuredActivityTextFrench);
			length = strlen(strng1);
			strncpy(&strng[kp-length], strng1, length);
			pr_write2_language_dbsec(strng, header_name);

			// variation
			strncpy(&strng[0], &head[4][0], 19);
			k = -1;
			++k;
			kp = 12 * k + 28;
			strng1[0] = 0;
			if(current.language == ENGLISH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[5].DeviationTextEnglish);
			else if(current.language == FRENCH) strcpy(strng1, AmuletChamberSearchMenu_AccuracyMeasurement[5].DeviationTextFrench);
			length = strlen(strng1);
			strncpy(&strng[kp - length], strng1, length);
			pr_write2_language_dbsec(strng, header_name);
		}
	}else{
		// Roll Printer & Slip Printer
		flg_first = TRUE;
		for(i=0; i<6; i++){
			if(AmuletChamberSearchMenu_AccuracyMeasurement[i].ChamberAccuracyMeasurementID > 0){
				if(flg_first) flg_first = FALSE;
				else feed_db(1, prtype);

				//serial number
				strncpy(&strng[0], &head[0][0], 7);
				strcpy(stndstr, AmuletChamberSearchMenu_AccuracyMeasurement[i].SourceNuclide);
				strncpy(&strng[9], stndstr, strlen(stndstr));
				strncpy(&strng[16], &head[1][0], 4);
				stringtofixed(sn, AmuletChamberSearchMenu_AccuracyMeasurement[i].SourceSerialNumber, 11);
				strncpy(&strng[21], sn, 10);
				pr_write2_language_dbsec(strng, header_name);

				// predicted activity
				strncpy(&strng[0], &head[2][0], 19);
				if(current.language == ENGLISH) strcpy(tempSECTION, AmuletChamberSearchMenu_AccuracyMeasurement[i].CalculatedActivityTextEnglish);
				else if(current.language == FRENCH) strcpy(tempSECTION, AmuletChamberSearchMenu_AccuracyMeasurement[i].CalculatedActivityTextFrench);
				length = strlen(tempSECTION);
				strncpy(&strng[31 - length], tempSECTION, length);
				pr_write2_language_dbsec(strng, header_name);

				// measured activity
				strncpy(&strng[0], &head[3][0], 17);
				if(current.language == ENGLISH) strcpy(tempSECTION, AmuletChamberSearchMenu_AccuracyMeasurement[i].MeasuredActivityTextEnglish);
				else if(current.language == FRENCH) strcpy(tempSECTION, AmuletChamberSearchMenu_AccuracyMeasurement[i].MeasuredActivityTextFrench);
				length = strlen(tempSECTION);
				strncpy(&strng[31 - length], tempSECTION, length);
				pr_write2_language_dbsec(strng, header_name);

				// variation
				strncpy(&strng[0], &head[4][0], 17);
				if(current.language == ENGLISH) strcpy(tempSECTION, AmuletChamberSearchMenu_AccuracyMeasurement[i].DeviationTextEnglish);
				else if(current.language == FRENCH) strcpy(tempSECTION, AmuletChamberSearchMenu_AccuracyMeasurement[i].DeviationTextFrench);
				length = strlen(tempSECTION);
				strncpy(&strng[31 - length], tempSECTION, length);
				pr_write2_language_dbsec(strng, header_name);
			}
		}
	}

	if(AmuletChamberSearchMenu_AutoConstancy[0].ChamberAutoConstancyID > 0){
		feed_db(1,prtype);
		prconstancysection_db(prtype, header_name);
	}

	if(print_chamber_section){
		if(AmuletChamberSearchMenu_AccuracyTest.Inactive){
			feed_db(1, prtype);
			prinactive_db(AmuletChamberSearchMenu_AccuracyTest.InactiveReason, prtype, header_name);
		}
	}
}



void dosig(char prtype){
	char strng[90], buffer[90];
	//short i0;

	feed(3, prtype);
	lininit(strng, TRUE, prtype);
	strncpy(&strng[0], "__________________________________", 34);
	pr_write(strng);

	feed(1,prtype);

	lininit(strng, TRUE, prtype);
	//strncpy(&strng[0],"signature",9);
	get_amulet_message(L_SIGNATURE, buffer);    // "signature"
	strncpy(&strng[0], buffer, strlen(buffer));

	/*if(id_set()){
		if(prtype == OKI_PRINTER || prtype == LX_PRINTER || prtype < 0) i0 = 30;
		else i0 = 13;
			//strncpy(&strng[i0],"BY:",3);
		get_message(DAILY_13,&strng[i0]);
		get_id_str(&strng[i0 + 4]);
	}*/

	pr_write(strng);
	formfeed(prtype);
}

static bool cnmsg(short nuc_index, bool *flag);
static void cnmsg1(void);
static void save_test(short ch_num,short nuc_index);


short test_num;
    /* constancy test data */
    typedef struct cntest_data CNTESTDATA;
    struct cntest_data
    {
        short nuc_index;
        char strng[10];
        short kun;
    };

CNTESTDATA test_data[20];


    static void constancy_test(void)
    {

        bool okflag;
        char ch;
        char ch1;
        short nuc_index;
        short ch_num = current.main_chamber;

        //initialize # of tests
        test_num = -1;


        erase_screen();
        //display_text(8,12,"Leave Source",0,MEDIUM,NORMAL);
        //display_text(8,22,"IN FOR",0,MEDIUM,NORMAL);
        //display_text(8,33,"Constancy Test",0,MEDIUM,NORMAL);
        display_medium_message(CONST_3,12,0,NORMAL);
        display_medium_message(CONST_4,22,0,NORMAL);
        display_medium_message(CONST_5,33,0,NORMAL);
        contmsg();
        if(home_set())
            return;
        erase_screen();

        set_meas_screen(TRUE);      //for display of nuclide name & cal#

        measurement[ch_num].ndec0 = 5;
        strcpy(&measurement[ch_num].actstr0[0],"xxxxxx");
        disp_cal(measurement[ch_num].resp0);
        display_chamber();
        disp_nuclide();
        nuc_index = chamber[ch_num].nuc_index;

        cnmsg1();
        okflag = FALSE;


        for(;;)  /* forever */
        {

            ch = getkey();
            if(home_set())
                return;
            switch (ch)
            {

            case NOKEY:
                break;


            case ISO1:
            case ISO2:
            case ISO3:
            case ISO4:
            case ISO5:
            case ISO6:
            case ISO7:
            case ISO8:
                if(cnmsg(nuc_index,&okflag))
                {
                    getiso(ch_num,ch - ISO1);
                    nuc_index = chamber[ch_num].nuc_index;

                    cnmsg1();
                }
                break;

            case USER1:
            case USER2:
            case USER3:
            case USER4:
            case USER5:
                ch1 = ch - USER1;
                if (user_key_index(ch1) == -1)
                    beep();
                else
                {
                    if(cnmsg(nuc_index,&okflag))
                    {
                        getuser(ch_num,ch1);
                        nuc_index = chamber[ch_num].nuc_index;
                        cnmsg1();
                    }
                }
                break;



            case DOWN:
                downbut();
                break;

            case UP:
                upbut();
                break;

            case NUCBUT:
                if(cnmsg(nuc_index,&okflag))
                {
                    nucbut();
                    nuc_index = chamber[ch_num].nuc_index;
                }
                cnmsg1();
                break;

            case ZERO:
            case ONE:
            case TWO:
            case THREE:
            case FOUR:
            case FIVE:
            case SIX:
            case SEVEN:
            case EIGHT:
            case NINE:
                //dokey(ch,1);
                break;

            case CLR:
                //keybuf_bsp();
                break;

            case OK:
                if(!okflag)
                {
                    beep();
                    break;
                }
                if(test_num < 19)
                    cnmsg(nuc_index,&okflag);
                //set_idnum();
                return;

            default:
                beep();
                break;

            }  /* end of main switch */

            if (measurement[ch_num].display_flag)
            {
                activity_to_screen();
                measurement[ch_num].display_flag = FALSE;

                okflag = TRUE;
            }
        }


    }


static bool cnmsg(short nuc_index, bool *flag)
     {

        short ch_num = current.main_chamber;


        if(!*flag || test_num == 19)
        {
            beep();
            return(FALSE);
        }

        save_test(ch_num,nuc_index);

        *flag = FALSE;
        return(TRUE);
     }

 static void cnmsg1(void)
    {
        //display_text(72,0,"CONSTANCY",0,SMALL,NORMAL);
        //display_text(90,8,"TEST",0,SMALL,NORMAL);
         display_small_message(CONST_6,0,0,NORMAL);
         display_small_message(CONST_7,8,0,NORMAL);

    }


 extern const char *unit_str[];
    void print_constancy_test(short printer)
    {

        short num;
        short ciso;
        short kun;
        char stndstr[8];
        char strng[90];
        char str[20];
        char strng1[6];
        char tempSECTION[40];

        if (test_num == -1)
            return;

        lininit(strng,TRUE,printer);
        if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER)
            bold(TRUE);
        //strncpy(&strng[4],"CONSTANCY TEST:",15);
        //get_message(CONST_10,&strng[4]);
        get_amulet_message(L_CONSTANCYTEST, tempSECTION);    // "CONSTANCY TEST:"
        strncpy(strng, tempSECTION, strlen(tempSECTION));
        if(printer == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER)
            bold(FALSE);
        feed(1,printer);


        lininit(strng,TRUE,printer);
        //strncpy(&strng[1],"Source:",7);
        //get_message(ACC_8,&strng[1]);
        get_amulet_message(L_SOURCE, tempSECTION);    // "Source:"
        strncpy(&strng[1], tempSECTION, strlen(tempSECTION));
        ciso = get_const_source2();
        get_stndstr(ciso,stndstr);
        strncpy(&strng[9],stndstr,strlen(stndstr));

        //strncpy(&strng[16],"S/N:",4);
        //get_message(ACC_10,&strng[16]);
        get_amulet_message(L_SN2, tempSECTION);    // "S/N:"
        strncpy(&strng[16], tempSECTION, strlen(tempSECTION));
        if(ciso < 5) EE_READ(stand[ciso].sn,(uchar *)str);
        else EE_READ(stand_ge68.sn, (uchar *)str);
        strncpy(&strng[21],str,10);
        if(printer != NONE_PRINTER)
            pr_write(strng);
        feed(1,printer);

        lininit(strng,TRUE,printer);
        //strncpy(&strng[3],"Nuclide     Activity",20);
        //get_message(CONST_11,&strng[3]);
        get_amulet_message(L_NUCLIDE, tempSECTION);    // "Nuclide"
        strncpy(&strng[3], tempSECTION, strlen(tempSECTION));
        get_amulet_message(L_ACTIVITY, tempSECTION);    // "Activity"
        strncpy(&strng[15], tempSECTION, strlen(tempSECTION));

        if(printer != NONE_PRINTER)
            pr_write(strng);
        feed(1,printer);


        for(num = 0; num <= test_num; num++)
        {

            lininit(strng,TRUE,printer);


            NuclideData_getName(test_data[num].nuc_index,str);

            strncpy(&strng[3],str,strlen(str));
            stringtofixed(strng1, test_data[num].strng, 6);
            strncpy(&strng[13],strng1,6);
            kun = test_data[num].kun;
            strncpy(&strng[19],unit_str[kun - 1],3);
            //if(strng[19] == MU)
            //    strng[19] = MU_PRT;
            pr_mu(&strng[19],printer);
            if(printer != NONE_PRINTER)
                pr_write(strng);

        }
    }

void prconstancysection_db(short printer, char *header_name){
	int i, nuclide_length;
	short kp;
	char str[20], stndstr[8], strng[90], tempSECTION[40];

	lininit_db(strng, TRUE, printer);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(TRUE);
	linupdate_db(strng, FALSE, TRUE);

	get_amulet_message(L_CONSTANCYTEST, tempSECTION);    // "CONSTANCY TEST:"
	strncpy(strng, tempSECTION, strlen(tempSECTION));
	if(printer != NONE_PRINTER) pr_write2_language_dbsec(strng, header_name);
	//if(printer == USB_EPS_PRINTER) eps_bold(0.5, strng);
	//else pr_write(strng);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	linupdate_db(strng, FALSE, FALSE);

	get_amulet_message(L_SOURCE, tempSECTION);    // "Source:"
	strncpy(&strng[0], tempSECTION, strlen(tempSECTION));
	kp = strlen(tempSECTION) + 1;
	strcpy(stndstr, AmuletChamberSearchMenu_AccuracyTest.ConstancyNuclide);
	strncpy(&strng[kp], stndstr, strlen(stndstr));
	kp += strlen(stndstr);
	strncpy(&strng[kp], ", ", 2);
	kp += 2;

	get_amulet_message(L_SN2, tempSECTION);    // "S/N:"
	strncpy(&strng[kp], tempSECTION, strlen(tempSECTION));
	kp += strlen(tempSECTION) + 1;
	strcpy(str, AmuletChamberSearchMenu_AccuracyTest.ConstancyNuclideSerialNumber);
	strncpy(&strng[kp], str, strlen(str));
	if(printer != NONE_PRINTER) pr_write2_language_dbsec(strng, header_name);

	get_amulet_message(L_NUCLIDE, str);    // "Nuclide"
	nuclide_length = strlen(str);
	get_amulet_message(L_ACTIVITY, tempSECTION);    // "Activity"
	for(i=0; i<3; i++){
		if(i==0){
			kp = 27 * i;
			strncpy(&strng[kp], str, strlen(str));

			kp = 27 * i + 18;
			strncpy(&strng[kp - strlen(tempSECTION)], tempSECTION, strlen(tempSECTION));
		}else{
			if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER){
				kp = 27 * i;
				strncpy(&strng[kp], str, strlen(str));

				kp = 27 * i + 18;
				strncpy(&strng[kp - strlen(tempSECTION)], tempSECTION, strlen(tempSECTION));
			}
		}
	}
	if(printer != NONE_PRINTER) pr_write2_language_dbsec(strng, header_name);

	if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER){
		i = 0;
		do{
			if(i){
				if(!(i%3)){
					if(printer != NONE_PRINTER) pr_write2_language_dbsec(strng, header_name);
				}
			}

			kp = (27 * (i%3)) + nuclide_length - strlen(AmuletChamberSearchMenu_AutoConstancy[i].ConstancyNuclide);
			strncpy(&strng[kp], AmuletChamberSearchMenu_AutoConstancy[i].ConstancyNuclide, strlen(AmuletChamberSearchMenu_AutoConstancy[i].ConstancyNuclide));

			if(current.language == ENGLISH) strcpy(tempSECTION, AmuletChamberSearchMenu_AutoConstancy[i].ConstancyActivityTextEnglish);
			else if(current.language == FRENCH) strcpy(tempSECTION, AmuletChamberSearchMenu_AutoConstancy[i].ConstancyActivityTextFrench);

			kp = (27 * (i%3)) + 18 - strlen(tempSECTION);
			strncpy(&strng[kp], tempSECTION, strlen(tempSECTION));
			i++;
		}while((i<12) && (AmuletChamberSearchMenu_AutoConstancy[i].ChamberAutoConstancyID > 0));

		if(printer != NONE_PRINTER) pr_write2_language_dbsec(strng, header_name);
	}else{
		i=0;
		do{
			kp = nuclide_length - strlen(AmuletChamberSearchMenu_AutoConstancy[i].ConstancyNuclide);
			strncpy(&strng[kp], AmuletChamberSearchMenu_AutoConstancy[i].ConstancyNuclide, strlen(AmuletChamberSearchMenu_AutoConstancy[i].ConstancyNuclide));

			if(current.language == ENGLISH) strcpy(tempSECTION, AmuletChamberSearchMenu_AutoConstancy[i].ConstancyActivityTextEnglish);
			else if(current.language == FRENCH) strcpy(tempSECTION, AmuletChamberSearchMenu_AutoConstancy[i].ConstancyActivityTextFrench);

			kp = 18 - strlen(tempSECTION);
			strncpy(&strng[kp], tempSECTION, strlen(tempSECTION));

			if(printer != NONE_PRINTER) pr_write2_language_dbsec(strng, header_name);
			i++;
		}while((i<12) && (AmuletChamberSearchMenu_AutoConstancy[i].ChamberAutoConstancyID > 0));
	}
}

/*    static bool auto_const_done;
 -     static void auto_constancy_test(void)
 -     {
 -
 -         short nuc_index;
 -         short ch_num = current.main_chamber;
 -         short iso;
 -         AUTO_CON auto_const;
 -         short nmeas;
 -
 -         auto_const_done = FALSE;
 -
 -         //initialize # of tests
 -         test_num = -1;
 -
 -         erase_screen();
 -
 -         //read in atuotmation data
 -         EE_READ(auto_con,(uchar *)&auto_const);
 -
 -         set_meas_screen(TRUE);      //for display of nuclide name & cal#
 -
 -         measurement[ch_num].ndec0 = 5;
 -
 -
 -         cnmsg1();
 -
 -
 -         for(iso = 0; iso < auto_const.num; iso++)
 -         {
 -             nuc_index = auto_const.nuclides[iso];
 -             set_nuclide_data(nuc_index,ch_num);
 -
 -             cnmsg1();
 -
 -             nmeas = 0;
 -
 -             for(;;)
 -             {
 -                 getkey();       //to trigger servicing chamber
 -
 -                 if (measurement[ch_num].display_flag)
 -                 {
 -                     activity_to_screen();
 -                     measurement[ch_num].display_flag = FALSE;
 -                     ++nmeas;
 -                     if(nmeas == 20)
 -                     {
 -                         save_test(ch_num,nuc_index);
 -                         break;
 -                     }
 -                 }
 -             }
 -
 -         }
 -
 -         erase_screen();
 -         //display_text(8,10,"CONSTANCY TEST",0,MEDIUM,NORMAL);
 -         //display_text(8,24,"PASSED",0,MEDIUM,NORMAL);
 -         display_medium_message(CONST_8,10,0,NORMAL);
 -         display_medium_message(CONST_9,24,0,NORMAL);
 -         auto_const_done = TRUE;
 -         //contmsg();
 -         key_pc_continue();
 -
 -
 -     }*/

/*    bool get_auto_const_done(void)
 -     {
 -         return (auto_const_done);
 -     }    */

    static void save_test(short ch_num,short nuc_index)
    {

        ++test_num;
        test_data[test_num].nuc_index = nuc_index;

        strncpy(&test_data[test_num].strng[0],
            &measurement[ch_num].actstr[0],6);
        test_data[test_num].strng[6] = '\0';
        test_data[test_num].kun = measurement[ch_num].kun;
        erase_text_line(40,0,BIG);

    }

    short get_const_source(void)
    {
        short i;
        STAND stnd;
        short ciso;

        ciso = -1; //assume no constancy source

        for(i = 0; i < 6; i++)
        {
            EE_READ(stand[i],(uchar *)&stnd);
            if(stnd.const_iso)
            {
                ciso = i;
                break;
            }

        }
        return (ciso);


    }

    short get_const_source2(void){
    	char ret;
    	short retvalue;
    	short constancych[12];
    	bool found;
    	short index;
    	short ch_num = current.main_chamber;
    	short ch_type;


    	EE_READ(constancysource, (uchar *) &ret);

    	if(ret == 0) retvalue = - 1;
    	else{
    		ch_type = chamber_type(ch_num);
    		EE_READ(constancych, (uchar *) &(constancych[0]));
    		found = FALSE;

    		for(index=0; index<12; index++){
    			if(constancych[index]>=0){
    				if(NuclideData_getEffectiveResponse(constancych[index], ch_type)!=0.0){
    					found = TRUE;
    					break;
    				}
    			}
    		}

    		if(found) retvalue = ret - 1;
    		else retvalue = -1;
    	}
    	return retvalue;
    }

    short get_num_sources(bool daily)
    {

        short i;
        short niso;

        niso = 0;
        for(i = 0; i < 6; i++)
        {

            if(!read_stand(i))
                continue;
            // Removed:
            //if(stnd.syst != 0xff)
            if(stnd.caldate != (time_t) 0)
            {
                if(!daily)
                {
                    ++niso;
                    continue;
                }
                //daily source?
                if (stnd.daily == TRUE)
                    ++niso;
            }
        }

        return niso;
    }

    static bool read_stand(short niso)
    {
        if(niso < 5){
            EE_READ(stand[niso],(uchar *)&stnd);
        }else{
            EE_READ(stand_ge68,(uchar *)&stnd);
        }

        return TRUE;

    }


    void get_acc_data(short ciso, bool daily, short ch_num)
    {
        //NUCDATA nucdata;
        short nuc_index;
        time_t time0,time1;
        char stndstr[8];
        short niso;
        short nacc;
        short kun;
        char strng[12];
        //short ndec;
        //float act_disp;
        float pr_act;
        short j, i;

        // set all tests to none
        for(j = 0; j < 6; j++)
        {
        	for(i=0; i<max_chambers; i++){
        		acc_data[j].test_res[i].pr_kun = acc_data[j].test_res[i].ms_kun = -1;
        	}
            //assume not daily
            acc_data[j].daily[0] = FALSE;
        }

        nacc = -1;
        for(niso = 0; niso < 6; niso++)
        {
            //read in standard
            if(!read_stand(niso))
               continue;

            // Removed:
            //if(stnd.syst == 0xff)   //no data for standard
            if(stnd.caldate == (time_t) 0)   //no data for standard
                continue;

            //if(daily && stnd.daily == FALSE)
            if(daily && !stnd.daily)
                continue;

            ++nacc;
            //strncpy(&acc_data[nacc].sn[0],&stnd.sn[0],strlen(&stnd.sn[0]));
            strcpy(acc_data[nacc].sn, stnd.sn);
            get_stndstr(niso,stndstr);
            nuc_index = NuclideData_getIndexFromName(stndstr);
            acc_data[nacc].nuc_index = nuc_index;

            read_clock(&time1);
            time0 = stnd.caldate;
            acc_data[nacc].cal_date = time0;
            acc_data[nacc].cal_activity = stnd.act;

            pr_act = nucdecay(stnd.act, time0, time1, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
            // Removed: Convert to system activity if system units differs from standards units
            //if (current.system != stnd.syst)
            //    pr_act = convact(pr_act);
            acc_data[nacc].test_res[ch_num].pred_act = pr_act;

            // Locked: CI
            //ndec = getdec(pr_act,current.system,&kun);
            // Removed:
            //ndec = getdec(pr_act, CI, &kun);
            //act_disp = pr_act * unitfact[kun - 1];
            //act2str(act_disp, strng, ndec, FALSE, kun);
            kun = format_activity_system(pr_act, strng);

            strncpy(&acc_data[nacc].test_res[ch_num].pract[0],strng, 6);
            acc_data[nacc].test_res[ch_num].pr_kun = kun;

            NuclideData_getName(nuc_index, acc_data[nacc].nucname);
            acc_data[nacc].nucname[5] = '\0';
            acc_data[nacc].daily[0] = stnd.daily;

            //if(daily)
            //{
                if(ciso == niso)
                    acc_data[nacc].const_source = TRUE;
                else
                    acc_data[nacc].const_source = FALSE;

            //}
        }
    }

    float calc_dev(short nacc, short ch_num)
    {

        float diff;
        float act1;
        //short kun;
        float pract;
        short nc;
        char strng[10];

        memcpy(&last_measurement, &(measurement[ch_num]), sizeof(MEASUREMENT));

        pract = acc_data[nacc].test_res[ch_num].pred_act;

	// Changed:
	//kun = measurement[ch_num].kun;
        //act1 = measurement[ch_num].act / unitfact[kun - 1];
	act1 = measurement[ch_num].act0;
        diff = 100. * (act1 - pract) / pract;

        acc_data[nacc].test_res[ch_num].ms_act = act1;
        strncpy(&acc_data[nacc].test_res[ch_num].msact[0],
            measurement[ch_num].actstr,6);
        acc_data[nacc].test_res[ch_num].ms_kun = measurement[ch_num].kun;

        if(fabs(diff) > 20.)
        {
            nc = -1;
            strcpy(&acc_data[nacc].test_res[ch_num].var[0],"*****");
        }
        else
        {
            nc = sprintf(strng,"%5.1f",diff);
            strcpy(&acc_data[nacc].test_res[ch_num].var[0],strng);
        }
        acc_data[nacc].test_res[ch_num].nc = nc;
        acc_data[nacc].test_res[ch_num].over_flag = measurement[ch_num].over_flag;
        return diff;

    }


    void save_acc_to_daily_res(short ch_num)
    {

        short i;
        short len;
        char stndstr[10];
        short nc;

        for(i = 0; i < 6; i++)
        {
            //save in daily_res
            if (acc_data[i].test_res[ch_num].ms_kun != -1)
            {
                //source & serial number
                len = strlen(&acc_data[i].nucname[0]);
                strncpy(stndstr,&acc_data[i].nucname[0],len);
                strncpy(&daily_res[ch_num].acc_res[i].source[0],stndstr,len);
                daily_res[ch_num].acc_res[i].source[len] = '\0';
                strcpy(&daily_res[ch_num].acc_res[i].sn[0],&acc_data[i].sn[0]);

                /* predicted activity*/
                strncpy(&daily_res[ch_num].acc_res[i].stand[0],&acc_data[i].test_res[ch_num].pract[0],6);
                strncpy(&daily_res[ch_num].acc_res[i].stand[6],unit_str[acc_data[i].test_res[ch_num].pr_kun - 1],3);
                daily_res[ch_num].acc_res[i].stand[9] = '\0';

                /* measured activity */
                strncpy(&daily_res[ch_num].acc_res[i].meas[0],&acc_data[i].test_res[ch_num].msact[0],6);
                strncpy(&daily_res[ch_num].acc_res[i].meas[6],unit_str[acc_data[i].test_res[ch_num].ms_kun - 1],3);
                daily_res[ch_num].acc_res[i].meas[9] = '\0';

                /* variation */
                nc = acc_data[i].test_res[ch_num].nc;
                if (nc == -1)
                    //strncpy(&daily_res[ch_num].acc_res[i].var[0],"ERROR",5);
                    get_message(DAILY_5,&daily_res[ch_num].acc_res[i].var[0]);
                else
                {
                    strncpy(&daily_res[ch_num].acc_res[i].var[0],&acc_data[i].test_res[ch_num].var[0],nc);
                    strcpy(&daily_res[ch_num].acc_res[i].var[nc],"%");
                }
            }
        }


    }
