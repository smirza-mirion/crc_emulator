/*********************************************************************
  MODULE:		DAILY TEST 

  FILE:			DailyTest.c

  DATE:			01/24/07

  ANALYSIS:		steps user through daily test

  DESIGN:    	auto zero; background; bias voltage; accuracy test
				using daily source; constancy test
				prints report

  CALLED BY:
		Test menu


  *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "printer.h"
#include "uart.h"
#include "i2c.h"
#include "message.h"
#include "chambfac.h"
#include "keyboard.h"
#include "qspi.h"
#include "daily.h"
#include "remote.h"
#include "pit.h"
#include "sl811s.h"
#include "nuc.h"
#include "stdlib.h"
#include <string.h>

extern CHAMBER chamber[];
extern CURRENT current;
extern BKGDATA bkgdata[];
extern BIASDATA biasdata[];
extern ZERODATA zerodata[];
extern DAILY_TEST_RES  daily_res[];
extern short max_chambers;
extern volatile bool AmuletPCQC_Abort;
extern CHAMBERDAILYTEST				AmuletChamberSearchMenu_DailyTest;
extern CHAMBERZERO					AmuletChamberSearchMenu_Zero;
extern CHAMBERBACKGROUND			AmuletChamberSearchMenu_Background;
extern CHAMBERVOLTAGE				AmuletChamberSearchMenu_Voltage;
extern CHAMBERACCURACYTEST			AmuletChamberSearchMenu_AccuracyTest;
extern CHAMBERACCURACYMEASUREMENT	AmuletChamberSearchMenu_AccuracyMeasurement[6];
extern CHAMBERAUTOCONSTANCY		AmuletChamberSearchMenu_AutoConstancy[12];
static char printer;
short prdaily(bool okdata,short acc_ret);
void trim(char *acByte);
void AmuletChamberSearch_PrintSNDescString(char *serial_number, int chamber_type, bool two_stage, char *output);
void praccuracysection_db(short prtype, char *header_name, bool print_chamber_section);
static bool data_check(void);

static bool bias_delay_flag;
bool printed_margin;
    

extern time_t clock_time;
extern time_t low_clock_time;
    void daily(void)
	{
		bool okdata;
		bool ynret;
		short iret;
        short acc_ret;
        short ch_num = current.main_chamber;
        short ch_type;

        ch_type = chamber_type(ch_num);

        start_daily_res(ch_num);
        

        erase_screen();
        //initialize id entry
        id_init();

		// Measure Zero 
        measure_zero_offset(TRUE);
        
        if(AmuletPCQC_Abort) return;
        if(home_set()) return;
        

		/* measure background */
        measure_bkg(1);

        if(AmuletPCQC_Abort) return;
        if(home_set()) return;

		/* read bias voltage */
		erase_screen();
        bias_delay_flag = FALSE;
        measure_bias(1);
        if(!chamber_hv_adc(ch_num)){
        	delay_bias(ch_type);
        }

        if(AmuletPCQC_Abort) return;
        if(home_set()) return;

        
		/* data check */
		okdata = data_check();

		if(AmuletPCQC_Abort) return;
        if(home_set()) return;

		if (okdata)
        {
            /* accuracy test */
            acc_ret = accuracy_test(TRUE);

            if(AmuletPCQC_Abort) return;
            if (acc_ret > 1 || home_set()) return;
        }
		
        
        erase_screen();

        if(chamber[ch_num].control == CONTROL_PC)
            return;

		/* printing */

        //get printer
        printer = current.printer;

		do
		{
			iret = prdaily(okdata,acc_ret);
            if(iret == -1 || home_set())
				return;
			if(iret == 1)
                dosig(printer);

			ynret = print_again();
			erase_screen();
		}while(ynret);
	}

	bool print_again(void)
	{
		//display_text(8,22,"PRINT AGAIN?",0,MEDIUM,NORMAL);
        display_medium_message(PRINT_1,22,0,NORMAL);
		//Yes or No
        display_medium_message(YES_OR_NO,42,0,NORMAL);
        return(yesorno());
	}

	extern CURRENT current;

short prdaily(bool okdata,short acc_ret){
	char strng[90], buffer[90];
	char str[10], tempSECTION[40], tempADDITIONAL[40], tempADDITIONAL2[40];
	short ch_num = current.main_chamber;
	short ch_type;

	printer = current.printer;
	ch_type = chamber_type(ch_num);

	if(!start_printer(printer, 1, FALSE, PAPER)) return 0;

	prhead_language(printer);
	feed(1, printer);

	if(printer == USB_PRINTER)
		pcl_cpi("10", TRUE);

	pr_set_linecnt(5);
	lininit(strng, TRUE, printer);

	if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER) bold(TRUE);
	get_amulet_message(L_DAILY_TEST2, tempSECTION);    // "DAILY TEST:"
	if(current.num_chambers > 1){
		get_amulet_message(L_CHAMBER, tempADDITIONAL);    // "Chamber"
		sprintf(buffer, "%s %s # %d, ", tempSECTION, tempADDITIONAL, ch_num + 1);
		if(ch_type == R_CHAMB) strcat(buffer, "R");
		else if(ch_type == P_CHAMB) strcat(buffer, "PET");
		else if(ch_type == B_CHAMB) strcat(buffer, "BT");
		else if(ch_type == ONE_DOT_EIGHT_CHAMB) strcat(buffer, "1.8 Atm");
		else if(ch_type == C_CHAMB) strcat(buffer, "HR");
		else if(ch_type == K_CHAMB) strcat(buffer, "1K");
		insertconst(strng, 0, 0, 0, buffer);
	}else{
		insertconst(strng, 0, 0, 0, tempSECTION);
	}
	pr_write2_language(strng, "Daily Test");
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);

	feed(1,printer);
		
	//zero
	get_amulet_message(L_ZERO, tempSECTION);    // "ZERO:"

	sprintf(str,"%#5.2f",zerodata[ch_num].zerodisp);
	strcpy(&daily_res[ch_num].zero[0],str);
	trim(str);

	if(zerodata[ch_num].status == ZERO_OUT_OF_RANGE) get_amulet_message(L_ERROR, tempADDITIONAL);    // "ERROR"
	else tempADDITIONAL[0] = 0;

	sprintf(buffer, "%s %s mV %s", tempSECTION, str, tempADDITIONAL);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language(strng, "Daily Test");

	//background
	get_amulet_message(L_BACKGROUND, tempSECTION);    // "BACKGROUND:"
	if(bkgdata[ch_num].latched_status != BKG_TOO_HIGH){
		format_bkg(ch_num, str);
		if (str[6] == MU){
			if(printer == USB_EPS_PRINTER) str[6] = 'u';
			else str[6] = MU_PRT;
		}
		trim(str);

		if(bkgdata[ch_num].latched_status == BKG_HIGH) get_amulet_message(L_HIGH, tempADDITIONAL);    // "HIGH"
		else tempADDITIONAL[0] = 0;
		sprintf(buffer, "%s %s %s", tempSECTION, str, tempADDITIONAL);
	}else{
		get_amulet_message(L_ERROR, tempADDITIONAL);    // "ERROR"
		sprintf(buffer, "%s %s", tempSECTION, tempADDITIONAL);
	}
	insertconst(strng, 0, 0, 0, buffer);
    pr_write2_language(strng, "Daily Test");

    //bias voltage
    get_amulet_message(L_CHAMBER_VOLTAGE, tempSECTION);    // "CHAMBER VOLTAGE:"

    sprintf(str,"%5.1f",biasdata[ch_num].measvolts);
    strcpy(&daily_res[ch_num].volts[0],str);
    daily_res[ch_num].v_err = '0';
    trim(str);
    get_amulet_message(L_VOLTS, tempADDITIONAL);    // "Volts"

    if(biasdata[ch_num].status != TEST_GOOD){
		get_amulet_message(L_ERROR, tempADDITIONAL2);    // "ERROR"
		daily_res[ch_num].v_err = '1';
    }else{
    	tempADDITIONAL2[0] = 0;
    }
    sprintf(buffer, "%s %s %s %s", tempSECTION, str, tempADDITIONAL, tempADDITIONAL2);
    insertconst(strng, 0, 0, 0, buffer);
    pr_write2_language(strng, "Daily Test");

    //data check
    get_amulet_message(L_DATA_CHECK_PRINTED, tempSECTION);    // "DATA CHECK:"

    if(okdata){
            daily_res[ch_num].data_ok = '1';
            get_amulet_message(L_CAPS_OK, tempADDITIONAL);    // "OK"
            sprintf(buffer, "%s %s", tempSECTION, tempADDITIONAL);
            insertconst(strng, 0, 0, 0, buffer);
            pr_write2_language(strng, "Daily Test");

			feed(1,printer);

            if(printer == NONE_PRINTER) return -1;
            
            if(acc_ret  == -1) return 1; //no accuracy test done

            print_accuracy_test(printer);
			
			//contancy test 
            if(acc_ret == 0){    //accuracy test done, no constancy test done
            	if(printer == NONE_PRINTER) return -1;
            	else return 1;
            }
			feed(2,printer);

			print_constancy_test(printer);
			
    }else{
            daily_res[ch_num].data_ok = '0';

			//strncpy(&strng[i0 + 12],"MEMORY ERROR",12);
            //get_message(DAILY_11,&strng[i0 + 12]);
            //get_amulet_message(L_MEMORY_ERROR, tempADDITIONAL);
            get_amulet_message(L_ERROR, tempADDITIONAL);    // "ERROR"
            sprintf(buffer, "%s %s", tempSECTION, tempADDITIONAL);
            insertconst(strng, 0, 0, 0, buffer);
            pr_write2_language(strng, "Daily Test");
    }

    if(printer == NONE_PRINTER) return -1;
    else return 1;
}

short prdailysec(bool okdata,short acc_ret){
	char strng[90], buffer[90];
	char str[10], tempSECTION[40], tempADDITIONAL[40], tempADDITIONAL2[40];
	short ch_num = current.main_chamber;
	short ch_type;

	printer = current.printer;
	ch_type = chamber_type(ch_num);

	if(!start_printer(printer, 1, FALSE, PAPER)) return 0;

	prhead_languagesec(printer);
	feed(1, printer);

	if(printer == USB_PRINTER)
		pcl_cpi("10", TRUE);

	pr_set_linecnt(5);
	lininit(strng, TRUE, printer);

	if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER) bold(TRUE);
	get_amulet_message(L_DAILY_TEST2, tempSECTION);    // "DAILY TEST:"
	if(current.num_chambers > 1){
		get_amulet_message(L_CHAMBER, tempADDITIONAL);    // "Chamber"
		sprintf(buffer, "%s %s # %d, ", tempSECTION, tempADDITIONAL, ch_num + 1);
		if(ch_type == R_CHAMB) strcat(buffer, "R");
		else if(ch_type == P_CHAMB) strcat(buffer, "PET");
		else if(ch_type == B_CHAMB) strcat(buffer, "BT");
		else if(ch_type == ONE_DOT_EIGHT_CHAMB) strcat(buffer, "1.8 Atm");
		else if(ch_type == C_CHAMB) strcat(buffer, "HR");
		else if(ch_type == K_CHAMB) strcat(buffer, "1K");
		insertconst(strng, 0, 0, 0, buffer);
	}else{
		insertconst(strng, 0, 0, 0, tempSECTION);
	}
	pr_write2_languagesec(strng, "Daily Test");
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);

	feed(1,printer);
	lininit(strng, TRUE, printer);
		
	//zero
	get_amulet_message(L_ZERO, tempSECTION);    // "ZERO:"

	sprintf(str,"%#5.2f",zerodata[ch_num].zerodisp);
	strcpy(&daily_res[ch_num].zero[0],str);
	trim(str);

	if(zerodata[ch_num].status == ZERO_OUT_OF_RANGE) get_amulet_message(L_ERROR, tempADDITIONAL);    // "ERROR"
	else tempADDITIONAL[0] = 0;

	sprintf(buffer, "%s %s mV %s", tempSECTION, str, tempADDITIONAL);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_languagesec(strng, "Daily Test");

	//background
	get_amulet_message(L_BACKGROUND, tempSECTION);    // "BACKGROUND:"
	if(bkgdata[ch_num].latched_status != BKG_TOO_HIGH){
		format_bkg(ch_num, str);
		if (str[6] == MU){
			if(printer == USB_EPS_PRINTER) str[6] = 'u';
			else str[6] = MU_PRT;
		}
		trim(str);

		if(bkgdata[ch_num].latched_status == BKG_HIGH) get_amulet_message(L_HIGH, tempADDITIONAL);    // "HIGH"
		else tempADDITIONAL[0] = 0;
		sprintf(buffer, "%s %s %s", tempSECTION, str, tempADDITIONAL);
	}else{
		get_amulet_message(L_ERROR, tempADDITIONAL);    // "ERROR"
		sprintf(buffer, "%s %s", tempSECTION, tempADDITIONAL);
	}
	insertconst(strng, 0, 0, 0, buffer);
    pr_write2_languagesec(strng, "Daily Test");

    //bias voltage
    get_amulet_message(L_CHAMBER_VOLTAGE, tempSECTION);    // "CHAMBER VOLTAGE:"

    sprintf(str,"%5.1f",biasdata[ch_num].measvolts);
    strcpy(&daily_res[ch_num].volts[0],str);
    daily_res[ch_num].v_err = '0';
    trim(str);
    get_amulet_message(L_VOLTS, tempADDITIONAL);    // "Volts"

    if(biasdata[ch_num].status != TEST_GOOD){
		get_amulet_message(L_ERROR, tempADDITIONAL2);    // "ERROR"
		daily_res[ch_num].v_err = '1';
    }else{
    	tempADDITIONAL2[0] = 0;
    }
    sprintf(buffer, "%s %s %s %s", tempSECTION, str, tempADDITIONAL, tempADDITIONAL2);
    insertconst(strng, 0, 0, 0, buffer);
    pr_write2_languagesec(strng, "Daily Test");

    //data check
    get_amulet_message(L_DATA_CHECK_PRINTED, tempSECTION);    // "DATA CHECK:"

    if(okdata){
            daily_res[ch_num].data_ok = '1';
            get_amulet_message(L_CAPS_OK, tempADDITIONAL);    // "OK"
            sprintf(buffer, "%s %s", tempSECTION, tempADDITIONAL);
            insertconst(strng, 0, 0, 0, buffer);
            pr_write2_languagesec(strng, "Daily Test");

			feed(1,printer);

            if(printer == NONE_PRINTER) return -1;

            if(acc_ret  == -1) return 1; //no accuracy test done

            print_accuracy_test(printer);

			//contancy test
            if(acc_ret == 0){    //accuracy test done, no constancy test done
            	if(printer == NONE_PRINTER) return -1;
            	else return 1;
            }
			feed(2,printer);

			print_constancy_test(printer);

    }else{
            daily_res[ch_num].data_ok = '0';

			//strncpy(&strng[i0 + 12],"MEMORY ERROR",12);
            //get_message(DAILY_11,&strng[i0 + 12]);
            //get_amulet_message(L_MEMORY_ERROR, tempADDITIONAL);
            get_amulet_message(L_ERROR, tempADDITIONAL);    // "ERROR"
            sprintf(buffer, "%s %s", tempSECTION, tempADDITIONAL);
            insertconst(strng, 0, 0, 0, buffer);
            pr_write2_languagesec(strng, "Daily Test");
    }

    if(printer == NONE_PRINTER) return -1;
    else return 1;
}

static bool okdata;
static char static_hex_str[10];
	static bool data_check(void)
	{

		char hex_str[10];
        ushort memval;

		//display_text(24,0,"DATA CHECK",0,MEDIUM,NORMAL);
        display_medium_message(DAILY_1,0,MEDIUM,NORMAL);

        okdata = NuclideData_testNuclideTable(&memval);
        sprintf(hex_str, "%04x", memval);
        strcpy(static_hex_str, hex_str);

		if (okdata)
			//display_text(32,32,"OK",0,MEDIUM,NORMAL);
        display_medium_message(AUTO_ZERO_7,32,0,NORMAL);

		else
		{
			display_text(24,20,"ERROR",0,MEDIUM,REV);
            sprintf(hex_str,"%04x",memval);
			display_text(32,32,hex_str,0,MEDIUM,NORMAL);
			display_text(32,44,"SEE MANUAL",0,MEDIUM,NORMAL);
		}

        key_pc_continue();
        
		erase_screen();

        return(okdata);
	}

    bool get_okdata(void)
    {
        return okdata;

    }

    char *get_hex_str(void){
    	return static_hex_str;
    }

     void seeman(void)
      {
            erase_lines(40,10,0);
            display_text(10,40,"SEE MANUAL",0,MEDIUM,NORMAL);
   
      }


    void plwait(bool flag)
    {
        erase_lines(40,10,0);
        if(flag)
            display_medium_message(PLEASE_WAIT,40,0,NORMAL);
    }

    void start_daily_res(short ch_num)
    {    

        short i;
        struct tm *tm_time;
        
        short ch_type;

		ch_type = chamber_type(ch_num);
        //start saving in daily_res for this chamber
        //get current date/time & save
        read_clock(&clock_time);
        low_clock_time = clock_time;
        //reset_minute_counter();
        reset_minute_counter_with_seconds();
        tm_time = gmtime(&clock_time);
        daily_res[ch_num].date[0] = (char)tm_time -> tm_year;
        daily_res[ch_num].date[1] = (char)tm_time -> tm_mon + 1;
        daily_res[ch_num].date[2] = (char)tm_time -> tm_mday;
        daily_res[ch_num].date[3] = (char)tm_time -> tm_hour;
        daily_res[ch_num].date[4] = (char)tm_time -> tm_min;
        daily_res[ch_num].date[5] = (char)tm_time -> tm_sec;
        
        //save chamber number & type in ASCII
        daily_res[ch_num].ch_num = (char)ch_num + '1';
        if(ch_type == R_CHAMB)
            daily_res[ch_num].ch_type = 'R';
        else if(ch_type == P_CHAMB)
            daily_res[ch_num].ch_type = 'P';
        else if(ch_type == B_CHAMB)
            daily_res[ch_num].ch_type = 'B';
        else if(ch_type == ONE_DOT_EIGHT_CHAMB)
            daily_res[ch_num].ch_type = '8';
        else if(ch_type == C_CHAMB)
            daily_res[ch_num].ch_type = 'C';
        else if(ch_type == K_CHAMB)
            daily_res[ch_num].ch_type = 'K';
        //set accuracy tests to none
        for(i = 0; i < 5; i++)
            daily_res[ch_num].acc_res[i].source[0] = '\0';
        //set constancy source to none
        daily_res[ch_num].const_source[0] = '\0';
        //make each constancy nuclide NULL
        for(i = 0; i < 20; i++)
            daily_res[ch_num].const_res[i].nuclide[0] = '\0';
    }

    void format_bkg(short ch_num, char *str)
    {
        float actfact;
        short index_57;
        float response;
        float act;
        float bkg = chamber[ch_num].bkg;

            
        short ch_type;

        ch_type = chamber_type(ch_num);
        actfact = get_gainfact(ch_type);
        
        if(current.system == BQ) actfact *= BQFACTOR;

        index_57 = NuclideData_getIndexFromName("Co57");
        response = NuclideData_getResponse(index_57, ch_type);
        
        act = bkg * actfact / response;

        format_activity(act,current.system,str);
    }


#include "counter.h"
    extern C_TEST c_test[];

    //0 in ch_num indicates no chamber
    void init_daily_results(void)
    {

        short i;

        for(i = 0; i < max_chambers; i++)
            daily_res[i].ch_num = '0';

        //counters
        for(i = 0; i < 2; i++)
        {    
            c_test[i].nuclide = '0';
            c_test[i].fill = 0xaa;
        }    
        
    }    


    

    void delay_bias(short ch_type)
    {
        short i;
        short imax;
        short ch_num = current.main_chamber;

        imax = 120;
        if(ch_type == R_CHAMB  || ch_type == C_CHAMB || ch_type == K_CHAMB)
            return;
        bias_delay_flag = TRUE;
        chamber[ch_num].active = FALSE;
        erase_screen();
        //plwait(TRUE);
        display_text(8,10,"PLEASE WAIT",0,MEDIUM,NORMAL);
        display_text(8,24,"2 Minutes For",0,MEDIUM,NORMAL);
        display_text(8,40,"Stabilization",0,MEDIUM,NORMAL);
           

        //imax sec delay
        for(i = 0; i < imax; i++)
        {    
            getkey();   //service chambers, PC
            delay_msec(1000);
        }    

        bias_delay_flag = FALSE;
        erase_screen();
        chamber[ch_num].active = TRUE;

    }

    bool get_bias_delay_flag(void)
    {
        return (bias_delay_flag);
    }    

void prsetprinter_db(void){
	printer = current.printer;
	printed_margin = FALSE;
}

void prchambersection_db(time_t test_time, char *chamber_sn, int chamber_type, bool two_stage, char *header_name){
	char *strng, *buffer, *descSECTION;

	strng = malloc(90);
	buffer = malloc(90);
	descSECTION = malloc(40);

	lininit_db(strng, TRUE, printer);

	if(printer == USB_PRINTER) pcl_cpi("10", TRUE);

	if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER) bold(TRUE);

	linupdate_db(strng, TRUE, TRUE);

	get_amulet_message(L_DATE_COLON, buffer);    // "Date:"
	strcat(buffer, " ");
	dateout_language(descSECTION, &test_time, 4);
	strcat(buffer, descSECTION);
	strcat(buffer, " ");
	timeoutsec(descSECTION, &test_time);
	strcat(buffer, descSECTION);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);


	AmuletChamberSearch_PrintSNDescString(chamber_sn, chamber_type, two_stage, buffer);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	linupdate_db(strng, FALSE, FALSE);

	free(descSECTION);
	free(buffer);
	free(strng);
}

void prinactive_db(char *inactive_reason, char pter, char *header_name){
	char *strng, *buffer;

	strng = malloc(90);
	buffer = malloc(90);


	if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER) bold(TRUE);
	lininit_db(strng, TRUE, pter);
	linupdate_db(strng, FALSE, TRUE);
	get_amulet_message(L_INACTIVE, buffer);    // "Inactive:"
	strcat(buffer, " ");
	strcat(buffer, inactive_reason);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);
	if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER) bold(FALSE);
	linupdate_db(strng, FALSE, FALSE);

	free(buffer);
	free(strng);
}

void prdailysection_db(char *header_name){
	char *strng, *buffer;

	strng = malloc(90);
	buffer = malloc(90);

	lininit_db(strng, TRUE, printer);

	prchambersection_db(AmuletChamberSearchMenu_DailyTest.CreatedOn, AmuletChamberSearchMenu_DailyTest.ChamberSerialNumber, AmuletChamberSearchMenu_DailyTest.ChamberType, AmuletChamberSearchMenu_DailyTest.TwoStageChamber, header_name);

	feed_db(1,printer);

	if(printer == USB_PRINTER) pcl_cpi("12", TRUE);
	if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER) bold(TRUE);
	linupdate_db(strng, FALSE, TRUE);

	get_amulet_message(L_DAILY_TEST2, buffer);    // "DAILY TEST:"
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	linupdate_db(strng, FALSE, FALSE);

	if(current.language == ENGLISH){
		get_amulet_message(L_ZERO, buffer);    // "ZERO:"
		strcat(buffer, " ");
		strcat(buffer, AmuletChamberSearchMenu_Zero.ZeroTextEnglish);
		insertconst(strng, 0, 0, 0, buffer);
		pr_write2_language_dbsec(strng, header_name);

		get_amulet_message(L_BACKGROUND, buffer);    // "BACKGROUND:"
		strcat(buffer, " ");
		strcat(buffer, AmuletChamberSearchMenu_Background.BackgroundTextEnglish);
		insertconst(strng, 0, 0, 0, buffer);
		pr_write2_language_dbsec(strng, header_name);

		get_amulet_message(L_CHAMBER_VOLTAGE, buffer);    // "CHAMBER VOLTAGE:"
		strcat(buffer, " ");
		strcat(buffer, AmuletChamberSearchMenu_Voltage.VoltageTextEnglish);
		insertconst(strng, 0, 0, 0, buffer);
		pr_write2_language_dbsec(strng, header_name);

		get_amulet_message(L_DATA_CHECK_PRINTED, buffer);    // "DATA CHECK:"
		strcat(buffer, " ");
		strcat(buffer, AmuletChamberSearchMenu_DailyTest.DataCheckTextEnglish);
		insertconst(strng, 0, 0, 0, buffer);
		pr_write2_language_dbsec(strng, header_name);
	}else if(current.language == SPANISH){
		get_amulet_message(L_ZERO, buffer);    // "ZERO:"
		strcat(buffer, " ");
		strcat(buffer, AmuletChamberSearchMenu_Zero.ZeroTextSpanish);
		insertconst(strng, 0, 0, 0, buffer);
		pr_write2_language_dbsec(strng, header_name);

		get_amulet_message(L_BACKGROUND, buffer);    // "BACKGROUND:"
		strcat(buffer, " ");
		strcat(buffer, AmuletChamberSearchMenu_Background.BackgroundTextSpanish);
		insertconst(strng, 0, 0, 0, buffer);
		pr_write2_language_dbsec(strng, header_name);

		get_amulet_message(L_CHAMBER_VOLTAGE, buffer);    // "CHAMBER VOLTAGE:"
		strcat(buffer, " ");
		strcat(buffer, AmuletChamberSearchMenu_Voltage.VoltageTextSpanish);
		insertconst(strng, 0, 0, 0, buffer);
		pr_write2_language_dbsec(strng, header_name);


		get_amulet_message(L_DATA_CHECK_PRINTED, buffer);    // "DATA CHECK:"
		strcat(buffer, " ");
		strcat(buffer, AmuletChamberSearchMenu_DailyTest.DataCheckTextSpanish);
		if(printer == ROLL_PRINTER || printer == SLIP_PRINTER){
			if(strlen(buffer) > 34){
				get_amulet_message(L_DATA_CHECK_PRINTED, buffer);    // "DATA CHECK:"
				insertconst(strng, 0, 0, 0, buffer);
				pr_write2_language_dbsec(strng, header_name);
				strcpy(buffer, AmuletChamberSearchMenu_DailyTest.DataCheckTextSpanish);
			}
		}
		insertconst(strng, 0, 0, 0, buffer);
		pr_write2_language_dbsec(strng, header_name);

	}
	free(buffer);
	free(strng);

	if(AmuletChamberSearchMenu_AccuracyTest.CreatedOn != 0){
		feed_db(1, printer);
		praccuracysection_db(printer, header_name, FALSE);
	}

	if(AmuletChamberSearchMenu_DailyTest.Inactive){
		feed_db(1, printer);
		prinactive_db(AmuletChamberSearchMenu_DailyTest.InactiveReason, printer, header_name);
	}
}

void prdivider_db(char *header_name){
	int i;
	char *strng;

	strng = malloc(90);
	lininit_db(strng, TRUE, printer);
	for(i=0; i<90; i++){
		if(strng[i] == ' '){
			strng[i] = '=';
		}else{
			break;
		}
	}
	pr_write2_language_dbsec(strng, header_name);
	free(strng);
}

void prdaily_db(void){
	prsetprinter_db();

	if(!start_printer(printer, 1, FALSE, PAPER)) return;

	prhead_language_db(printer);
	feed(1, printer);

	pr_set_linecnt(5);

	prdailysection_db("");

	formfeed(printer);
}

void praccuracy_db(void){
	prsetprinter_db();

	if(!start_printer(printer, 1, FALSE, PAPER)) return;

	prhead_language_db(printer);
	feed(1, printer);

	pr_set_linecnt(5);

	praccuracysection_db(printer, "", TRUE);

	formfeed(printer);
}

void przerosection_db(char *header_name, bool space){
	char *strng, *buffer;

	strng = malloc(90);
	buffer = malloc(90);

	lininit_db(strng, TRUE, printer);

	prchambersection_db(AmuletChamberSearchMenu_Zero.MeasuredOn, AmuletChamberSearchMenu_Zero.ChamberSerialNumber, AmuletChamberSearchMenu_Zero.ChamberType, AmuletChamberSearchMenu_Zero.TwoStageChamber, header_name);

	if(space) feed_db(1,printer);

	if(printer == USB_PRINTER) pcl_cpi("12", TRUE);
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	linupdate_db(strng, FALSE, FALSE);

	get_amulet_message(L_ZERO, buffer);    // "ZERO:"
	strcat(buffer, " ");
	if(current.language == ENGLISH) strcat(buffer, AmuletChamberSearchMenu_Zero.ZeroTextEnglish);
	else if(current.language == SPANISH) strcat(buffer, AmuletChamberSearchMenu_Zero.ZeroTextSpanish);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);

	free(buffer);
	free(strng);

	if(AmuletChamberSearchMenu_Zero.Inactive){
		if(space) feed_db(1, printer);
		prinactive_db(AmuletChamberSearchMenu_Zero.InactiveReason, printer, header_name);
	}
}

void przero_db(void){
	char *header_name;

	prsetprinter_db();

	if(!start_printer(printer, 1, FALSE, PAPER)) return;

	prhead_language_db(printer);
	feed(1, printer);

	pr_set_linecnt(5);

	header_name = malloc(80);
	get_amulet_message(L_ZERO_2, header_name);    // "Zero"
	przerosection_db(header_name, TRUE);
	free(header_name);

	formfeed(printer);
}

void prbackgroundsection_db(char *header_name, bool space){
	char *strng, *buffer;

	strng = malloc(90);
	buffer = malloc(90);

	lininit_db(strng, TRUE, printer);

	prchambersection_db(AmuletChamberSearchMenu_Background.MeasuredOn, AmuletChamberSearchMenu_Background.ChamberSerialNumber, AmuletChamberSearchMenu_Background.ChamberType, AmuletChamberSearchMenu_Background.TwoStageChamber, header_name);

	if(space) feed_db(1,printer);

	if(printer == USB_PRINTER) pcl_cpi("12", TRUE);
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	linupdate_db(strng, FALSE, FALSE);

	get_amulet_message(L_BACKGROUND, buffer);    // "BACKGROUND:"
	strcat(buffer, " ");
	if(current.language == ENGLISH) strcat(buffer, AmuletChamberSearchMenu_Background.BackgroundTextEnglish);
	else if(current.language == SPANISH) strcat(buffer, AmuletChamberSearchMenu_Background.BackgroundTextSpanish);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);

	free(buffer);
	free(strng);

	if(AmuletChamberSearchMenu_Background.Inactive){
		if(space) feed_db(1, printer);
		prinactive_db(AmuletChamberSearchMenu_Background.InactiveReason, printer, header_name);
	}
}

void prbackground_db(void){
	char *header_name;

	prsetprinter_db();

	if(!start_printer(printer, 1, FALSE, PAPER)) return;

	prhead_language_db(printer);
	feed(1, printer);

	pr_set_linecnt(5);

	header_name = malloc(80);
	get_amulet_message(L_BACKGROUND2, header_name);    // "Background"
	prbackgroundsection_db(header_name, TRUE);
	free(header_name);

	formfeed(printer);
}

void prchambervoltagesection_db(char *header_name, bool space){
    char *strng, *buffer, *acMsg;

	strng = malloc(90);
	buffer = malloc(90);
	acMsg = malloc(40);

	lininit_db(strng, TRUE, printer);

	prchambersection_db(AmuletChamberSearchMenu_Voltage.MeasuredOn, AmuletChamberSearchMenu_Voltage.ChamberSerialNumber, AmuletChamberSearchMenu_Voltage.ChamberType, AmuletChamberSearchMenu_Voltage.TwoStageChamber, header_name);

	if(space) feed_db(1,printer);

	if(printer == USB_PRINTER) pcl_cpi("12", TRUE);
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	linupdate_db(strng, FALSE, FALSE);

	get_amulet_message(L_CHAMBER_VOLTAGE, buffer);    // "CHAMBER VOLTAGE:"
	strcat(buffer, " ");
	if(current.language == ENGLISH) strcat(buffer, AmuletChamberSearchMenu_Voltage.VoltageTextEnglish);
	else if(current.language == SPANISH) strcat(buffer, AmuletChamberSearchMenu_Voltage.VoltageTextSpanish);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);

	get_amulet_message(L_NOMINAL_VOLTAGE, buffer);    // "NOMINAL VOLTAGE:"
	strcat(buffer, " ");
	sprintf(acMsg, "%.1f V", AmuletChamberSearchMenu_Voltage.NominalVoltage);
	strcat(buffer, acMsg);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);

	get_amulet_message(L_MIN_VOLTAGE, buffer);    // "MIN VOLTAGE:"
	strcat(buffer, " ");
	sprintf(acMsg, "%.1f V", AmuletChamberSearchMenu_Voltage.MinVoltage);
	strcat(buffer, acMsg);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);

	get_amulet_message(L_MAX_VOLTAGE, buffer);    // "MAX VOLTAGE:"
	strcat(buffer, " ");
	sprintf(acMsg, "%.1f V", AmuletChamberSearchMenu_Voltage.MaxVoltage);
	strcat(buffer, acMsg);
	insertconst(strng, 0, 0, 0, buffer);
	pr_write2_language_dbsec(strng, header_name);

	free(acMsg);
	free(buffer);
	free(strng);

	if(AmuletChamberSearchMenu_Voltage.Inactive){
		if(space) feed_db(1, printer);
		prinactive_db(AmuletChamberSearchMenu_Voltage.InactiveReason, printer, header_name);
	}
}

void prchambervoltage_db(void){
	char *header_name;

	prsetprinter_db();

	if(!start_printer(printer, 1, FALSE, PAPER)) return;

	prhead_language_db(printer);
	feed(1, printer);

	pr_set_linecnt(5);

	header_name = malloc(80);
	get_amulet_message(L_CHAMBER_VOLTAGE_2, header_name);    // "Chamber Voltage"
	prchambervoltagesection_db(header_name, TRUE);
	free(header_name);

	formfeed(printer);
}
