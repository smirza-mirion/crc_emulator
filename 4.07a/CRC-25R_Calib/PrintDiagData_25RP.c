/*********************************************************************
  MODULE:	PRINT DIAGNOSTIC DATA for CRC-25R, Pet

  FILE:		PrintDiagData_25RP.c

  DATE: 	02/05/07

    CALLED BY:
		diagnostics

  *************************************************************************/
#include <stdlib.h>
#include "crc.h"
#include "printer.h"
#include "uart.h"
#include "i2c.h"
#include "qspi.h"
#include "message.h"
#include "chambfac.h"
#include "screen.h"
#include "keyboard.h"
#include "pit.h"
#include "nuc.h"
#include "mca.h"
#include "database.h"
#include "wipes.h"
#include "ff.h"
#include "bioassay.h"
#include <string.h>
#include <math.h>

static short format_hl(float hl, char *strng);
static void	pr_nuclide_data(short ii);
static void	pr_nuclide_data_well(short ii);
static void pr_nuclide_data_700(short ii);
static bool pr_stand(char *strng, STAND *stnd, char *name, bool constsource);
static void	pr_table_head(void);
static void	pr_table_head_well(void);
static void	pr_table_head_700(void);
static void pr_chamber_parameters(short ch_num);
static bool pr_test_sources(void);
static void pr_test_source_header(void);
static bool pr_user_keys(void);
static void print_nuc_data(void);
static void print_nuc_datasec(void);
static void print_nuc_data_well(void);
static void print_nuc_data_700(void);
static void pr_system_test(bool test_ok);
static void eps_div(float xpos);
static void pr_well_counter_parameters(void);
static void pr_SD_status(void);
bool system_test(void);
bool amulet_system_test(void);
void GetShortNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void trim(char *acByte);
void AmuletWellMainScreenMenu_getWellStandardSN(char *SN);
time_t AmuletWellMainScreenMenu_getWellStandardDate(void);
float AmuletWellMainScreenMenu_getWellStandardActivity(void);
void trim_and_shrink(char *acByte);
static void print_nuc_data_700_hp(void);
static void	pr_table_head_700_hp(short mode);
static void pr_nuclide_data_700_hp(short ii);
void print_page_num(void);
static void	pr_table_head_well_hp(short mode);
static void print_nuc_data_well_hp(void);
static void	pr_nuclide_data_well_hp(short ii);
static void print_nuc_data_hp(bool well_exists, bool probe_exists);
static void	pr_table_head_hp(short mode,bool well_exists, bool probe_exists);
static void	pr_nuclide_data_hp(short ii,bool well_exists, bool probe_exists);
static void pr_chamber_parameters_hp(short ch_num);
static void pr_well_counter_parameters_hp(bool t_flag);
void get_time_unit(short itime, char *unit);
	extern const char *timeunit[];
	extern CURRENT current;
    extern CHAMBER chamber[];
    extern CHAMBERVALS chamb_vals[];
    extern float I131Lower_keV, I131Upper_keV, I125Lower_keV, I125Upper_keV, I123Lower_keV, I123Upper_keV;
    extern PROBETHYROIDUPTAKENORMAL ThyroidUptake_normal[4];
    extern time_t clock_time;
    extern float RBCSurvival_minNormal, RBCSurvival_maxNormal;

    static char prtype;
    static short line;
short diag_page_num;
    //static PROBETHYROIDUPTAKEPROTOCOLACTIVENAME *ThyroidUptakeProtocolActiveName;
    //static PROBETHYROIDUPTAKEPROTOCOL ThyroidUptakeProtocol;


	void print_diagnostic_data(void)
	{
        bool pr_ok;
        bool test_ok;
        short ch_num = current.main_chamber;

        // Removed: System is read at the beginning
        //EE_READ(syst, &current.system);
        prtype = current.printer;
        
        //disable ADC
        set_adc_enabled(FALSE);
        erase_screen();
        //display_text(16, 0, "DIAGNOSTICS",0,MEDIUM,NORMAL);
        display_medium_message(DIAG_1,0,0,NORMAL);

        pr_ok = start_printer(prtype,1,FALSE,PAPER);
        if(home_set())
            return;
        
        if(pr_ok)
        {
            prhead_language(prtype);
            feed(1,prtype);
            
            line = 3;
            
            print_nuc_data();
            
            feed(2,prtype);
            ++line;
            ++line;
            
            pr_user_keys();
    
            feed(2,prtype);
            ++line;
            ++line;
            
            pr_test_sources();

            pr_chamber_parameters(ch_num);
        }

        test_ok = system_test();
        if(pr_ok)
        {    
        
            pr_system_test(test_ok);
			if(prtype == SLIP_PRINTER)
				formfeed(ROLL_PRINTER);
			else
				formfeed(prtype);
        }
        contmsg();

        //re-enable ADC
        if(current.num_chambers != 0)
            set_adc_enabled(TRUE);
        
    }

bool amulet_print_diagnostic_data(void){
	bool pr_ok;
	bool test_ok;
	short ch_num = current.main_chamber;
	bool adcstate, runsystemtest;
	short i;
	bool well_exists,probe_exists;
	bool t_flag;

	// Removed: System is read at the beginning
	//EE_READ(syst, &current.system);

	diag_page_num = 0;
	prtype = current.printer;
	if(prtype == USB_PRINTER)
	{
		pcl_cpi("12",TRUE);
		pcl_height("12",TRUE);
		pcl_line_spacing("6",TRUE);
	}

	//disable ADC
	adcstate = get_adc_enabled();
	set_adc_enabled(FALSE);
	erase_screen();
	if(prtype == NONE_PRINTER || prtype == USB_EPS_LABEL_PRINTER){
		pr_ok = FALSE;
		runsystemtest = TRUE;
	}else{
		pr_ok = start_printer(prtype, 1, FALSE, PAPER);
		runsystemtest = pr_ok;
	}

    if(pr_ok){
		if(prtype == USB_PRINTER)
		{	
			pr_set_linecnt(3);
			rawheader_languagesec(prtype, "",  clock_time);
		}
		else
    			prhead_languagesec(prtype);

    	feed(1,prtype);

    	line = 3;

		well_exists = probe_exists = FALSE;
		t_flag = TRUE;
		if((Mca_installedDetector == DET_WELL) ||(Mca_installedDetector == DET_WELL700))
			well_exists = TRUE;
		if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_DRILLEDPROBE700))
			probe_exists = TRUE;

    	if((Mca_installedDetector == DET_WELL) && (current.num_chambers == 0)){
			if(prtype == USB_PRINTER)
				print_nuc_data_well_hp();
			else
    				print_nuc_data_well();

    		feed(1,prtype);
			++line;
    	}else if(((Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_DRILLEDPROBE700)) && (current.num_chambers == 0)){
			if(prtype == USB_PRINTER)
				print_nuc_data_700_hp();
			else
    				print_nuc_data_700();

    		feed(1,prtype);
    		++line;
    	}else{
			if(prtype == USB_PRINTER)
			{
				t_flag = FALSE;
				print_nuc_data_hp(well_exists,probe_exists);
				pr_set_linecnt(3);
				rawheader_languagesec(prtype, "",  clock_time);
				pr_test_sources();
				print_page_num();

				pr_set_linecnt(3);
				rawheader_languagesec(prtype, "",  clock_time);
				if(current.num_chambers > 1)
				{
					for(i=0; i<8; i++)
					{	
						if(chamber[i].exists)
							pr_chamber_parameters_hp(i);
					}
				}else
				{
					pr_chamber_parameters_hp(ch_num);
				}
				print_page_num();
			}	
			else
			{	
    		print_nuc_datasec();

    		feed(2,prtype);
			++line;
			++line;

			pr_test_sources();

			if(current.num_chambers > 1){
				for(i=0; i<8; i++){
						if(chamber[i].exists)
							pr_chamber_parameters(i);
				}
			}else{
				pr_chamber_parameters(ch_num);
			}
    		}
    	}

    	if(Mca_installedDetector == DET_WELL || Mca_installedDetector == DET_WELL700 || Mca_installedDetector == DET_PROBE700 || Mca_installedDetector == DET_DRILLEDPROBE700){
			if(prtype == USB_PRINTER)
				pr_well_counter_parameters_hp(t_flag);
			else
    				pr_well_counter_parameters();
    	}

    	feed(1,prtype);
		++line;

    	// Print SD available and total memory
    	pr_SD_status();

    	feed(1,prtype);
		++line;
    }

    if(runsystemtest) test_ok = amulet_system_test();

    if(pr_ok){
    	pr_system_test(test_ok);
		if(prtype == USB_PRINTER)
			print_page_num();
		else
		{
			if(prtype == SLIP_PRINTER)
				formfeed(ROLL_PRINTER);
			else
    			formfeed(prtype);
		}	
    }

    //re-enable ADC
    set_adc_enabled(adcstate);

    return runsystemtest;
}

static void pr_nuclide_data_700(short ii){
	short kp, temp;
	char buff[20], strng[90], nucname[6];
	float primarypeak, halflife;
	NUCDATA  nucdata;

	NuclideData_getNuclide(ii,&nucdata);
	if(nucdata.hlunit == -1)  /* user nuclide -- no data */
		return;

	// PRINT NUCLIDE
	lininit(strng,TRUE,prtype);
	kp = 0;

	stringtofixed(nucname, &nucdata.name[0], 6);
	strncpy(&strng[kp], nucname, 6);

	// PRINT HALFLIFE
	halflife = nucdata.halflife;
	if((nucdata.hlunit == YEAR) && (halflife > 1.0e+6)) halflife /= 1.0e+6;
	temp = format_hl(halflife,buff);
	kp = 15;
	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
		kp = 18;
	strncpy(&strng[kp - temp], buff, temp);
	if((nucdata.hlunit == YEAR) && (nucdata.halflife > 1.0e+6)){
		//strng[kp + 1] = 'M';
		//strng[kp + 2] = 'Y';
		get_time_unit(4,buff);
		strncpy(&strng[kp + 1],buff,strlen(buff));
		
	}else
	{
		//strng[kp + 1] = *timeunit[nucdata.hlunit - 1];
		get_time_unit(nucdata.hlunit - 1,buff);
		strncpy(&strng[kp + 1],buff,strlen(buff));
		
	}	

	primarypeak = NuclideData_getEffectivePrimary(ii);

	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER){
		if(primarypeak > 0.0){
			if(primarypeak != NuclideData_getPrimary(ii)) insertfloat(strng, 30, 1, 10, "%.2fu", primarypeak);
			else insertfloat(strng, 29, 1, 9, "%.2f", primarypeak);
		}
	}else{
		if(primarypeak > 0.0){
			if(primarypeak != NuclideData_getPrimary(ii)) insertfloat(strng, 25, 1, 10, "%.2fu", primarypeak);
			else insertfloat(strng, 24, 1, 9, "%.2f", primarypeak);
		}
	}
	pr_write(strng);
	++line;

	// PRINT WELL700 EFF
	if(NuclideData_getEffectiveEff(ii, DET_WELL700) > 0.0){
		lininit(strng, TRUE, prtype);
		kp = 3;
		strncpy(&(strng[kp]), "Well EFF:", 9);
		sprintf(buff, "%.2f%%", NuclideData_getEffectiveEff(ii, DET_WELL700));
		kp = 19 - strlen(buff);
		strncpy(&(strng[kp]), buff, strlen(buff));
		pr_write(strng);
		++line;
	}

	// PRINT PROBE700 EFF
	if(NuclideData_getEffectiveEff(ii, DET_PROBE700) > 0.0){
		lininit(strng, TRUE, prtype);
		kp = 2;
		strncpy(&(strng[kp]), "Probe EFF:", 10);
		sprintf(buff, "%.2f%%", NuclideData_getEffectiveEff(ii, DET_PROBE700));
		kp = 19 - strlen(buff);
		strncpy(&(strng[kp]), buff, strlen(buff));
		pr_write(strng);
		++line;
	}
}

//for USB HP only
static void pr_nuclide_data_700_hp(short ii)
{
	short temp;
	char buff[20], strng[200], nucname[8];
	float primarypeak, halflife;
	NUCDATA  nucdata;
	char unit[4];

	NuclideData_getNuclide(ii,&nucdata);
	if(nucdata.hlunit == -1)  /* user nuclide -- no data */
		return;

	lininit3(strng, TRUE, prtype);

	
	// PRINT NUCLIDE

	stringtofixed(nucname, &nucdata.name[0], 6);
	nucname[6] = '\0';
	insertstring3(strng,0,0,0,"%s",nucname);

	
	// PRINT HALFLIFE
	halflife = nucdata.halflife;
	if((nucdata.hlunit == YEAR) && (halflife > 1.0e+6)) halflife /= 1.0e+6;
	temp = format_hl(halflife,buff);
	buff[temp] = '\0';
	insertstring3(strng,18,1,0,"%s",buff);
	if((nucdata.hlunit == YEAR) && (nucdata.halflife > 1.0e+6)){
		insertconst3(strng,20,0,0,"MY");
	}
	else
	{
		unit[0] = *timeunit[nucdata.hlunit - 1];
		unit[1] = '\0';
		insertstring3(strng,20,0,0,"%s",unit);
	}


	//primary peak
	primarypeak = NuclideData_getEffectivePrimary(ii);

	if(primarypeak > 0.0)
	{
		if(primarypeak != NuclideData_getPrimary(ii))
			insertfloat3(strng, 24, 0, 10, "%.2fu", primarypeak);
		else
			insertfloat3(strng, 24, 0, 9, "%.2f", primarypeak);
	}

	
	// PRINT WELL700 EFF
	if(NuclideData_getEffectiveEff(ii, DET_WELL700) > 0.0){
		insertfloat3(strng,36,0,0,"%.2f",NuclideData_getEffectiveEff(ii, DET_WELL700));
	}


	
	// PRINT PROBE700 EFF
	if(NuclideData_getEffectiveEff(ii, DET_PROBE700) > 0.0){
		insertfloat3(strng,50,0,0,"%.2f",NuclideData_getEffectiveEff(ii, DET_PROBE700));
	}
	
	pr_write3_language(strng,"DIAGNOSTICS");
	++line;
}

static void	pr_nuclide_data_well(short ii){
	short kp, temp;
	char buff[20], strng[90], nucname[6];
	float primarypeak, eff, halflife;
	NUCDATA  nucdata;

	NuclideData_getNuclide(ii,&nucdata);
	if(nucdata.hlunit == -1)  /* user nuclide -- no data */
		return;

	// PRINT NUCLIDE
	lininit(strng,TRUE,prtype);
	kp = 0;

	stringtofixed(nucname, &nucdata.name[0], 6);
	strncpy(&strng[kp], nucname, 6);

	// PRINT HALFLIFE
	halflife = nucdata.halflife;
	if((nucdata.hlunit == YEAR) && (halflife > 1.0e+6)) halflife /= 1.0e+6;
	temp = format_hl(halflife,buff);
	kp = 15;
	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
		kp = 18;
	strncpy(&strng[kp - temp], buff, temp);
	if((nucdata.hlunit == YEAR) && (nucdata.halflife > 1.0e+6)){
		strng[kp + 1] = 'M';
		strng[kp + 2] = 'Y';
	}else strng[kp + 1] = *timeunit[nucdata.hlunit - 1];

	// Modified
	primarypeak = NuclideData_getEffectivePrimary(ii);
	eff = NuclideData_getEffectiveEff(ii, DET_WELL);

	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER){
		if(primarypeak > 0.0){
			if(primarypeak != NuclideData_getPrimary(ii)) insertfloat(strng, 30, 1, 10, "%.2fu", primarypeak);
			else insertfloat(strng, 29, 1, 9, "%.2f", primarypeak);
		}
		if(eff > 0.0) insertfloat(strng, 39, 1, 9, "%.3f", eff);
	}else{
		if(primarypeak > 0.0){
			if(primarypeak != NuclideData_getPrimary(ii)) insertfloat(strng, 25, 1, 10, "%.2fu", primarypeak);
			else insertfloat(strng, 24, 1, 9, "%.2f", primarypeak);
		}
		if(eff > 0.0) insertfloat(strng, 33, 1, 9, "%.3f", eff);
	}
	pr_write(strng);
	++line;
}

//USB HP printer only
static void	pr_nuclide_data_well_hp(short ii){
	short temp;
	char buff[20], strng[200], nucname[8];
	float primarypeak, eff, halflife;
	NUCDATA  nucdata;
	char unit[4];

	NuclideData_getNuclide(ii,&nucdata);
	if(nucdata.hlunit == -1)  /* user nuclide -- no data */
		return;

	// PRINT NUCLIDE
	lininit3(strng,TRUE,prtype);

	stringtofixed(nucname, &nucdata.name[0], 6);
	nucname[6] = '\0';
	insertstring3(strng,0,0,0,"%s",nucname);

	// PRINT HALFLIFE
	halflife = nucdata.halflife;
	if((nucdata.hlunit == YEAR) && (halflife > 1.0e+6)) halflife /= 1.0e+6;
	temp = format_hl(halflife,buff);
	buff[temp] = '\0';
	insertstring3(strng,18,1,9,"%s",buff);
	if((nucdata.hlunit == YEAR) && (nucdata.halflife > 1.0e+6)){
		insertconst3(strng,20,0,0,"MY");
	}else
	{	
		unit[0] = *timeunit[nucdata.hlunit - 1];
		unit[1] = '\0';
		insertstring3(strng,20,0,0,"%s",unit);
	}	

	primarypeak = NuclideData_getEffectivePrimary(ii);
	eff = NuclideData_getEffectiveEff(ii, DET_WELL);

	if(primarypeak > 0.0)
	{
		if(primarypeak != NuclideData_getPrimary(ii))
			//insertfloat(strng, 30, 1, 10, "%.2fu", primarypeak);
			insertfloat3(strng, 24, 0, 10, "%.2fu", primarypeak);
		else
			//insertfloat(strng, 29, 1, 9, "%.2f", primarypeak);
			insertfloat3(strng, 24, 0, 9, "%.2f", primarypeak);
	}
	if(eff > 0.0)
		//insertfloat(strng, 39, 1, 9, "%.3f", eff);
		insertfloat3(strng, 36, 0, 0, "%.2f", eff);
/*	}else{
 - 		if(primarypeak > 0.0){
 - 			if(primarypeak != NuclideData_getPrimary(ii)) insertfloat(strng, 25, 1, 10, "%.2fu", primarypeak);
 - 			else insertfloat(strng, 24, 1, 9, "%.2f", primarypeak);
 - 		}
 - 		if(eff > 0.0) insertfloat(strng, 33, 1, 9, "%.3f", eff);*/
	//}
	pr_write3_language(strng,"DIAGNOSTICS");
	++line;
}

    static void	pr_nuclide_data(short ii)
	{
		short kp;
		short temp;
		char buff[20];
		NUCDATA  nucdata;
        char strng[90];
        char cal_str[10];
        float fUserResponse;
        //short ch_type;
        char nucname[6];
        float primarypeak, eff, halflife;
		short i;

        //ch_type = chamber_type(current.main_chamber);
        NuclideData_getNuclide(ii,&nucdata);
        if (nucdata.hlunit == -1)  /* user nuclide -- no data */
            return;

		lininit(strng,TRUE,prtype);
		kp = 0;
		stringtofixed(nucname, &nucdata.name[0], 6);
		strncpy(&strng[kp], nucname, 6);

		halflife = nucdata.halflife;
		if((nucdata.hlunit == YEAR) && (halflife > 1.0e+6)) halflife /= 1.0e+6;
		temp = format_hl(halflife,buff);
		kp = 15;
		if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
			kp = 18;
		strncpy(&strng[kp - temp], buff, temp);
		if((nucdata.hlunit == YEAR) && (nucdata.halflife > 1.0e+6)){
			//strng[kp+1] = 'M';
			//strng[kp+2] = 'Y';
			get_time_unit(4,buff);
			strncpy(&strng[kp + 1],buff,strlen(buff));
		}else
		{	
			//strng[kp + 1] = *timeunit[nucdata.hlunit - 1];
			//strng[kp + 1] = buff[0];
			get_time_unit(nucdata.hlunit - 1,buff);
			strncpy(&strng[kp + 1],buff,strlen(buff));
		}	
        
		fUserResponse = NuclideData_getUserResponse(ii, R_CHAMB);
		if(fUserResponse != 0.0) nucdata.response[0] = fUserResponse;
        if ((nucdata.response[0] != 0.0) || (fUserResponse != 0.0))
        {
//            if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
//                kp = 23;
//            else
//            {
//                //strncpy(&strng[19],"Cal#:",5);
//                get_message(DIAG_11,&strng[19]);
//                kp = 25;
//            }
        	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER) kp = 24;
        	else kp = 19;

            DisplayNucCal_getCalNumFromResponse(nucdata.response[0], R_CHAMB, cal_str);
            //if(cal_str[3] == DIVSIGN)
			for(i = 0; i < strlen(cal_str); i++)
			{	
				if(cal_str[i] == '/')
				{
					if(prtype == USB_EPS_PRINTER)
					{
						cal_str[i] = ' ';
						eps_div(3.125);
					}
					else
						cal_str[i] = DIV_PRT;
				}
			}
/*            if(cal_str[3] == '/')
 -             {    
 -                 if(prtype == USB_EPS_PRINTER)
 -                     {
 -                         cal_str[3] = ' ';
 -                         eps_div(3.125);
 -                     }    
 -                     
 -                 else
 -                     cal_str[3] = DIV_PRT;
 -             }    */
            trim(cal_str);
            strncpy(&strng[kp],cal_str,strlen(cal_str));

            if(fUserResponse!=0.0)
            {
                //kp += 8;
            	kp+=strlen(cal_str);
                strng[kp] = 'U';
            }
//            kp = 10;
//            if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
//                kp = 31;
        }

        fUserResponse = NuclideData_getUserResponse(ii, P_CHAMB);
		if(fUserResponse != 0.0) nucdata.response[1] = fUserResponse;
        if ((nucdata.response[1] != 0.0) || (fUserResponse != 0.0))
        {
//            if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
//                kp = 32;
//            else
//            {
//                //strncpy(&strng[19],"Cal#:",5);
//                //get_message(DIAG_11,&strng[19]);
//                kp = 34;
//            }
        	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER) kp = 32;
        	else kp=27;

            DisplayNucCal_getCalNumFromResponse(nucdata.response[1], P_CHAMB, cal_str);
            if(cal_str[3] == DIVSIGN)
            {
                if(prtype == USB_EPS_PRINTER)
                    {
                        cal_str[3] = ' ';
                        eps_div(3.125);
                    }

                else
                    cal_str[3] = DIV_PRT;
            }
            trim(cal_str);
            strncpy(&strng[kp],cal_str,strlen(cal_str));

            if(fUserResponse!=0.0)
            {
                //kp += 8;
            	kp+=strlen(cal_str);
                strng[kp] = 'U';
            }
//            kp = 10;
//            if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
//                kp = 31;
        }

            pr_write(strng);
            ++line;

        if(Mca_installedDetector == DET_WELL){
        	primarypeak = NuclideData_getEffectivePrimary(ii);

        	//Modified
        	eff = NuclideData_getEffectiveEff(ii, DET_WELL);
        	if((primarypeak > 0.0) && (eff > 0.0)){
        		lininit(strng, TRUE, prtype);
        		kp = 2;
        		strncpy(&(strng[kp]), "Pri:", 4);
        		if(primarypeak != NuclideData_getPrimary(ii)) sprintf(buff, "%.2fu keV", primarypeak);
        		else sprintf(buff, "%.2f keV", primarypeak);
        		kp = 17 - strlen(buff);
        		strncpy(&(strng[kp]), buff, strlen(buff));
        		kp = 22;
        		strncpy(&(strng[kp]), "EFF:", 4);
        		sprintf(buff, "%.2f%%", eff);
        		kp = 33 - strlen(buff);
        		strncpy(&(strng[kp]), buff, strlen(buff));
        		pr_write(strng);
        		++line;
        	}
        }else if((Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_DRILLEDPROBE700)){
        	primarypeak = NuclideData_getEffectivePrimary(ii);
        	if(primarypeak > 0.0){
				if(NuclideData_getEffectiveEff(ii, DET_WELL700) > 0.0 || (NuclideData_getEffectiveEff(ii, DET_PROBE700) > 0.0)){
					lininit(strng, TRUE, prtype);
					kp = 4;
					strncpy(&(strng[kp]), "Primary:", 8);
					if(primarypeak != NuclideData_getPrimary(ii)) sprintf(buff, "%.2fu keV", primarypeak);
					else sprintf(buff, "%.2f keV", primarypeak);

					kp = 24 - strlen(buff);
					strncpy(&(strng[kp]), buff, strlen(buff));
					pr_write(strng);
					++line;

					if(NuclideData_getEffectiveEff(ii, DET_WELL700) > 0.0){
						lininit(strng, TRUE, prtype);
						kp = 3;
						strncpy(&(strng[kp]), "Well EFF:", 9);
						sprintf(buff, "%.2f%%", NuclideData_getEffectiveEff(ii, DET_WELL700));
						kp = 19 - strlen(buff);
						strncpy(&(strng[kp]), buff, strlen(buff));
						pr_write(strng);
						++line;
					}

					if(NuclideData_getEffectiveEff(ii, DET_PROBE700) > 0.0){
						lininit(strng, TRUE, prtype);
						kp = 2;
						strncpy(&(strng[kp]), "Probe EFF:", 10);
						sprintf(buff, "%.2f%%", NuclideData_getEffectiveEff(ii, DET_PROBE700));
						kp = 19 - strlen(buff);
						strncpy(&(strng[kp]), buff, strlen(buff));
						pr_write(strng);
						++line;
					}
				}
        	}
        }
	}

	//USB HP printer only
	static void	pr_nuclide_data_hp(short ii,bool well_exists, bool probe_exists)
	{
		short temp;
		char buff[20];
		NUCDATA  nucdata;
        char strng[200];
        char cal_str[10];
        float fUserResponse;
        char nucname[8];
        float primarypeak, eff, halflife;
		char unit[4];
		short i;

        NuclideData_getNuclide(ii,&nucdata);
        if (nucdata.hlunit == -1)  /* user nuclide -- no data */
            return;

		lininit3(strng,TRUE,prtype);
		stringtofixed(nucname, &nucdata.name[0], 6);
		nucname[6] = '\0';
		insertstring3(strng,0,0,0,"%s",nucname);

		halflife = nucdata.halflife;
		if((nucdata.hlunit == YEAR) && (halflife > 1.0e+6)) halflife /= 1.0e+6;
		temp = format_hl(halflife,buff);
		buff[temp] = '\0';
		insertstring3(strng,16,1,0,"%s",buff);
		if((nucdata.hlunit == YEAR) && (nucdata.halflife > 1.0e+6)){
			//insertconst3(strng,18,0,0,"MY");
			get_time_unit(4,unit);
			insertstring3(strng,18,0,0,"%s",unit);
		}else
		{	
			//unit[0] = *timeunit[nucdata.hlunit - 1];
			//unit[1] = '\0';
			get_time_unit(nucdata.hlunit - 1,unit);
			insertstring3(strng,18,0,0,"%s",unit);
		}	
        
		if(chamber_77t()){
			fUserResponse = NuclideData_getUserResponse(ii, C_CHAMB);
			if(fUserResponse != 0.0) nucdata.response[C_CHAMB] = fUserResponse;

			if ((nucdata.response[C_CHAMB] != 0.0) || (fUserResponse != 0.0))
				{

					DisplayNucCal_getCalNumFromResponse(nucdata.response[C_CHAMB], C_CHAMB, cal_str);
					//if(cal_str[3] == '/')
					//	cal_str[3] = DIV_PRT;
					trim(cal_str);
					temp = strlen(cal_str);
					strncpy(buff,cal_str,temp);
					buff[temp] = '\0';
					if(fUserResponse!=0.0)
						strcat(buff,"U");
					insertstring3(strng,22,0,0,"%s",buff);

				}
		}else{
			fUserResponse = NuclideData_getUserResponse(ii, R_CHAMB);
			if(fUserResponse != 0.0) nucdata.response[R_CHAMB] = fUserResponse;

			if ((nucdata.response[R_CHAMB] != 0.0) || (fUserResponse != 0.0))
			{

				DisplayNucCal_getCalNumFromResponse(nucdata.response[R_CHAMB], R_CHAMB, cal_str);
				//if(cal_str[3] == '/')
				//	cal_str[3] = DIV_PRT;
				trim(cal_str);
				temp = strlen(cal_str);
				strncpy(buff,cal_str,temp);
				buff[temp] = '\0';
				if(fUserResponse!=0.0)
					strcat(buff,"U");
				insertstring3(strng,22,0,0,"%s",buff);

			}
		}

        if(!chamber_77t()){
			fUserResponse = NuclideData_getUserResponse(ii, P_CHAMB);
			if(fUserResponse != 0.0) nucdata.response[1] = fUserResponse;
			if ((nucdata.response[1] != 0.0) || (fUserResponse != 0.0))
			{

				DisplayNucCal_getCalNumFromResponse(nucdata.response[1], P_CHAMB, cal_str);
				for(i = 0; i < strlen(cal_str);i++)
				{
					if(cal_str[i] == '/')
						cal_str[i] = DIV_PRT;
				}
	/*            if(cal_str[3] == '/')
	 - 				cal_str[3] = DIV_PRT;*/
				trim(cal_str);
				temp = strlen(cal_str);
				strncpy(buff,cal_str,temp);
				buff[temp] = '\0';
				if(fUserResponse!=0.0)
					strcat(buff,"U");
				insertstring3(strng,32,0,0,"%s",buff);

			}	
        }

		if(well_exists || probe_exists)
		{
        	primarypeak = NuclideData_getEffectivePrimary(ii);
			if(primarypeak > 0.0)
			{
				if(primarypeak != NuclideData_getPrimary(ii))
					insertfloat3(strng, 42, 0, 10, "%.2fu", primarypeak);
				else
					insertfloat3(strng, 42, 0, 9, "%.2f", primarypeak);
			}
			else
			{
				pr_write3_language(strng,"DIAGNOSTICS");	//no primary peak, line finished
				return;
			}	
		}
		else
		{
			pr_write3_language(strng,"DIAGNOSTICS");	//no well or probe, line finished
			return;
		}	
		
        if(Mca_installedDetector == DET_WELL)
		{

        	eff = NuclideData_getEffectiveEff(ii, DET_WELL);
			if(eff > 0.0)
				insertfloat3(strng,52,0,0,"%.2f",eff);
        }
		else if((Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_DRILLEDPROBE700))
		{
			if(NuclideData_getEffectiveEff(ii, DET_WELL700) > 0.0 || (NuclideData_getEffectiveEff(ii, DET_PROBE700) > 0.0))
			{
				if(NuclideData_getEffectiveEff(ii, DET_WELL700) > 0.0){
					eff = NuclideData_getEffectiveEff(ii, DET_WELL700);
					insertfloat3(strng,52,0,0,"%.2f",eff);
				}

				if(NuclideData_getEffectiveEff(ii, DET_PROBE700) > 0.0){
					eff = NuclideData_getEffectiveEff(ii, DET_PROBE700);
					insertfloat3(strng,64,0,0,"%.2f",eff);
				}
			}
        }
		pr_write3_language(strng,"DIAGNOSTICS");
	}

extern time_t clock_time;

static bool pr_stand(char *strng, STAND *stnd, char *name, bool constsource)
	{
		short j,kp;
        time_t time0;
        time_t nowtime;
		float pract;
		char dactstr[10];
		//short kun;
		NUCDATA	 nucdata;
		//short ndec;
		//float act;
        char dstr[12];
        char sn[10];
		char message[10];
		short len;

        if (prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
			kp = 30;
		else
			kp = 10;

        dateout_language(dstr,&stnd -> caldate,4);
        strncpy(&strng[kp],dstr,strlen(dstr));
        // Removed:
		//ndec = getdec(stnd -> act,stnd -> syst,&kun);
        //ndec = getdec(stnd -> act, CI, &kun);
		//act = stnd -> act * unitfact[kun - 1];
		//act2str(act,dactstr,ndec,FALSE,kun);
        //kun = format_activity_system2(stnd -> act, dactstr);
	format_activity_system2(stnd -> act, dactstr);
        if(prtype == USB_EPS_PRINTER) replace(dactstr, '$', 'u');
		else replace(dactstr, '$', MU_PRT);

        if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
			kp = 44;
		else
			kp = 22;
        strncpy(&strng[kp], dactstr, strlen(dactstr));
		//strncpy(&strng[kp],&dactstr[1],5);
        //strncpy(&strng[kp + 5],unit_str[kun - 1],3);
        //pr_mu(&strng[kp + 5],prtype);
        if (prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
			kp = 16;
		else
		{
            //uart_write(strng,U_PR);
			pr_write(strng);
			lininit(strng,TRUE,prtype);
			kp = 2;
			//strncpy(&strng[17],"Now:",4);
            //get_message(DIAG_47,&strng[17]);
			get_amulet_message(L_NOW,message);    // "Now:"
			len = strlen(message);
			strncpy(&strng[20 - len],message,len);
			
		}
        stringtofixed(sn, &stnd->sn[0], 10);
		strncpy(&strng[kp], sn, 10);
        j = NuclideData_getIndexFromName(name);
        NuclideData_getNuclide(j,&nucdata);
        nowtime = clock_time;
        time0 = stnd -> caldate;
		pract = nucdecay(stnd -> act,time0,nowtime,
				nucdata.halflife,nucdata.hlunit);
		// Removed:
		//ndec = getdec(pract,stnd -> syst,&kun);
		//ndec = getdec(pract, CI, &kun);
		//act = pract * unitfact[kun - 1];
		//act2str(act,dactstr,ndec,FALSE,kun);
		//kun = format_activity_system2(pract, dactstr);
		format_activity_system2(pract, dactstr);
		if(prtype == USB_EPS_PRINTER) replace(dactstr, '$', 'u');
		else replace(dactstr, '$', MU_PRT);

		if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
			kp = 59;
		else
			kp = 22;
		strncpy(&strng[kp], dactstr, strlen(dactstr));
		//strncpy(&strng[kp],&dactstr[1],5);
        //strncpy(&strng[kp + 5],unit_str[kun - 1],3);
        
        pr_mu(&strng[kp + 5],prtype);
        if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
            kp = 69;
        else
            kp = 32;
        if (stnd -> daily)
            strng[kp] = 'D';
        if (constsource)
            strng[kp + 1] = 'C';
		return(TRUE);
	}

	/* format half-life */
static	short format_hl(float hl, char *strng)
	{
		short ndec;
		short nc;
        char fmtstr[8];

		ndec = 3;
		if (hl >= 10.)
			ndec = 2;
		if (hl >= 100.)
			ndec = 1;
		if (hl > 500.)
			ndec = 0;
        strcpy(fmtstr,"%#5. f");
        fmtstr[4] = (uchar)ndec + '0';
        nc = sprintf(strng,fmtstr,hl);
		if (ndec < 3)
		{
			strng[nc] = ' ';
			++nc;
		}

		if (ndec < 2)
		{
			strng[nc] = ' ';
			++nc;
		}

		if(ndec < 1)
		{
			strng[nc] = ' ';
			++nc;
		}
		return(nc);
	}




    static bool pr_user_keys(void)
	{
		short ii,i0;
		short index;
		short nc;
		char str[4];
		NUCDATA  nucdata;
        short maxuser;
        char strng[90];


        short ch_type;

        ch_type = chamber_type(current.main_chamber);
        if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
            bold(TRUE);
        lininit(strng,TRUE,prtype);
        //strncpy(&strng[4],"USER KEYS:",10);
        get_message(USER_KEYS_MSG,&strng[4]);
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        ++line;

        if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
            bold(FALSE);

		if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
            lininit(strng,TRUE,prtype);

        if (ch_type == P_CHAMB) /* Review */
            maxuser = 9;
        else
            maxuser = 5;
        
        for(ii = 0; ii < maxuser; ii++)
		{

			if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
			{
				if(ii < 5)
					i0 = 12 * ii;
				else 
					i0 = 12 * (ii - 5);
			}
			else
			{
                lininit(strng,TRUE,prtype);
				i0 = 8;
			}
			strncpy(&strng[i0],"U  :",4);
            nc = sprintf(str,"%d",ii + 1);
			strncpy(&strng[i0 + 1],str,nc);
            EE_READ(user_keys[0].keys[ii],(uchar *)&index);
			if(index == -1)
				//strncpy(&strng[i0 + 5],"None",4);
                get_message(DIAG_32,&strng[i0 + 5]);
			else
			{
				NuclideData_getNuclide(index,&nucdata);
				strncpy(&strng[i0 + 5],&nucdata.name[0],6);
			}
            if(prtype == ROLL_PRINTER || prtype == SLIP_PRINTER)
			{
                //uart_write(strng,U_PR);
				pr_write(strng);
			}
			else
			{
				if(ii == 4 || ii == 8)
				{
                    pr_write(strng);
                    ++line;
                    lininit(strng,TRUE,prtype);
				}
			}
		}

		if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER){
            pr_write(strng);
            ++line;
		}
		return(TRUE);
	}

static void	pr_table_head_700(void){
	char strng[90];

	feed(1,prtype);
	++line;

	if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);

	lininit(strng,TRUE,prtype);
	//strncpy(&strng[4],"NUCLIDE DATA:",13);
	get_message(DIAG_4,&strng[4]);
	if(prtype == USB_EPS_PRINTER) eps_bold(0.5,strng);
	else pr_write(strng);
	++line;

	if(prtype == USB_PRINTER) pcl_cpi("12",TRUE);

	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER){
		bold(FALSE);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[12],"Half-Life",9);
		get_message(DIAG_6,&strng[12]);
		insertconst(strng, 29, 1, 0, "Pri(keV)");
		pr_write(strng);
		++line;
	}else{
		lininit(strng,TRUE,prtype);
		insertconst(strng, 24, 1, 0, "Pri(keV)");
		pr_write(strng);
		++line;
	}
}

//for HP USB only
static void	pr_table_head_700_hp(short mode){
	char strng[200];
	
	++line;

	pcl_cpi("10",FALSE);
	lininit3(strng,TRUE,prtype);
	if(mode == 1)
		insertconst3(strng,4,0,0,"@1NUCLIDE DATA:@8");
	else
		insertconst3(strng,4,0,0,"@1USER NUCLIDES:@8");
	pr_write3_language(strng,"DIAGNOSTICS");
	feed(1,prtype);
	++line;

	pcl_cpi("12",TRUE);


	insertconst3(strng, 12, 0, 0, "Half-Life");
	insertconst3(strng, 24, 0, 0, "Pri(keV)");
	insertconst3(strng,36,0,0,"Well Eff(%)");
	insertconst3(strng,50,0,0,"Probe Eff(%)");
	pr_write3_language(strng,"DIAGNOSTICS");
}

//for HP USB only
static void	pr_table_head_well_hp(short mode){
	char strng[200];
	
	++line;

	pcl_cpi("10",FALSE);
	lininit3(strng,TRUE,prtype);
	if(mode == 1)
		insertconst3(strng,4,0,0,"@1NUCLIDE DATA:@8");
	else
		insertconst3(strng,4,0,0,"@1USER NUCLIDES:@8");
	pr_write3_language(strng,"DIAGNOSTICS");
	feed(1,prtype);
	++line;

	pcl_cpi("12",TRUE);


	//strncpy(&strng[12],"Half-Life",9);
	//get_message(DIAG_6,&strng[12]);
	insertconst3(strng, 12, 0, 0, "Half-Life");
	insertconst3(strng, 24, 0, 0, "Pri(keV)");
	insertconst3(strng,36,0,0,"Eff(%)");
	pr_write3_language(strng,"DIAGNOSTICS");
}

static void	pr_table_head_well(void){
	char strng[90];

	feed(1,prtype);
	++line;

	if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);

	lininit(strng,TRUE,prtype);
	//strncpy(&strng[4],"NUCLIDE DATA:",13);
	get_message(DIAG_4,&strng[4]);
	if(prtype == USB_EPS_PRINTER) eps_bold(0.5,strng);
	else pr_write(strng);
	++line;

	if(prtype == USB_PRINTER) pcl_cpi("12",TRUE);

	if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER){
		bold(FALSE);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[12],"Half-Life",9);
		get_message(DIAG_6,&strng[12]);
		insertconst(strng, 29, 1, 0, "Pri(keV)");
		insertconst(strng, 39, 1, 0, "Eff(%)");
		pr_write(strng);
		++line;
	}else{
		lininit(strng,TRUE,prtype);
		insertconst(strng, 24, 1, 0, "Pri(keV)");
		insertconst(strng, 33, 1, 0, "Eff(%)");
		pr_write(strng);
		++line;
	}
}

    static void	pr_table_head(void)
	{

        char strng[90];
		char message[50];

        feed(1,prtype);
        ++line;
   		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(TRUE);
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[4],"NUCLIDE DATA:",13);
        //get_message(DIAG_4,&strng[4]);
		get_amulet_message(L_PR_NUCLIDE_DATA,message);    // "NUCLIDE DATA:"
		strncpy(&strng[4],message,strlen(message));
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        ++line;
        if(prtype == USB_PRINTER)
            pcl_cpi("12",TRUE);
        
		if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER){
            bold(FALSE);
            
            lininit(strng,TRUE,prtype);
            //strncpy(&strng[12],"Half-Life",9);
            //get_message(DIAG_6,&strng[12]);
			get_amulet_message(L_PR_HALFLIFE,message);    // "Half-Life"
			strncpy(&strng[12],message,strlen(message));
            strncpy(&strng[24],"RCal #",6);
            strncpy(&strng[33],"PCal #",6);
            pr_write(strng);
            ++line;
		}else{
			lininit(strng,TRUE,prtype);
			strncpy(&strng[19],"RCal #",6);
			strncpy(&strng[27],"PCal #",6);
			pr_write(strng);
			++line;
		}
    }
    
	//USB HP Printer only
	static void	pr_table_head_hp(short mode,bool well_exists, bool probe_exists)
	{

        char strng[200];
		char message[50];
		char str[50];

		pcl_cpi("10",FALSE);
		lininit3(strng,TRUE,prtype);
		if(mode == 1)
		{	
			//insertconst3(strng,4,0,0,"@1NUCLIDE DATA:@8");
			get_amulet_message(L_PR_NUCLIDE_DATA,message);    // "NUCLIDE DATA:"
			strcpy(str,"@1");
			strcat(str,message);
			strcat(str,"@8");
			insertconst3(strng,4,0,0,str);
		}	
		else
		{	
			//insertconst3(strng,4,0,0,"@1USER NUCLIDES:@8");
			get_amulet_message(L_PR_USER_NUCLIDES,message);    // "USER NUCLIDES:"
			strcpy(str,"@1");
			strcat(str,message);
			strcat(str,"@8");
			insertconst3(strng,4,0,0,str);
		}	
		pr_write3_language(strng,"DIAGNOSTICS");
		feed(1,prtype);
        pcl_cpi("12",TRUE);

		if(well_exists || probe_exists)
		{
			if(well_exists)
				insertconst3(strng,52,0,0,"Well");
			if(probe_exists)
				insertconst3(strng,64,0,0,"Probe");
		}
		pr_write3_language(strng,"DIAGNOSTICS");
		
        
		//insertconst3(strng,10,0,0,"Half-Life");
		get_amulet_message(L_PR_HALFLIFE,message);    // "Half-Life"
		insertconst3(strng,10,0,0,message);
		
		if(chamber_77t())insertconst3(strng,22,0,0,"Cal # ");
		else insertconst3(strng,22,0,0,"RCal #");
		if(!chamber_77t()) insertconst3(strng,32,0,0,"PCal #");
		if(well_exists || probe_exists)
		{
			insertconst3(strng,42,0,0,"Pri(keV)");
			if(well_exists)
				insertconst3(strng,52,0,0,"Eff(%)");
			if(probe_exists)
				insertconst3(strng,64,0,0,"Eff(%)");
		}
		pr_write3_language(strng,"DIAGNOSTICS");
    }

    static void pr_chamber_parameters(short ch_num)
    {
        
        float resp;
        short nc;
        char strng[90];
        char dactstr[16];
        short kp;
		int i;
        CHAMBERVALS ch_vals;
		char message[30];

        if(current.demo_mode)
            memcpy(&ch_vals,&chamb_vals[ch_num],sizeof(ch_vals));
        else
            //read in values for chamber
            read_from_chamber(ch_num,&ch_vals);

        if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
            bold(TRUE);

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[2],"Chamber  :",18);
        //get_message(DIAG_24,&strng[2]);
		get_amulet_message(L_CHAMBER,message);    // "Chamber"
		strcpy(&strng[2],message);
        if(current.num_chambers > 1)
        {
			strcat(strng," # ");
			i = strlen(strng);
			strng[i] = (char)ch_num + '1';
			strng[i + 1] = '\0';
			//strng[10] = '#';
				//strng[12] = (char)ch_num + '1';
        }
		strcat(strng,":\r\n");
		
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        ++line;
        
        if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
            bold(FALSE);
        

        lininit(strng, TRUE, prtype);
        if(ch_vals.chamb_type == R_CHAMB)
		{	
			//strncpy(&strng[2], "R CHAMBER", 9);
			get_amulet_message(L_PR_R_CHAMBER,message);    // "R CHAMBER"
			strncpy(&strng[2],message,strlen(message));
		}	
        else if(ch_vals.chamb_type == P_CHAMB)
		{	
			//strncpy(&strng[2], "PET CHAMBER", 11);
			get_amulet_message(L_PR_PET_CHAMBER,message);    // "PET CHAMBER"
			strncpy(&strng[2],message,strlen(message));
		}else if(ch_vals.chamb_type == B_CHAMB)
		{
			get_amulet_message(L_PR_BT_CHAMBER,message);    // "BT CHAMBER"
			strncpy(&strng[2],message,strlen(message));
		}else if(ch_vals.chamb_type == ONE_DOT_EIGHT_CHAMB)
		{
			get_amulet_message(L_PR_18_CHAMBER,message);    // "1.8 Atm CHAMBER"
			strncpy(&strng[2],message,strlen(message));
		}else if(ch_vals.chamb_type == C_CHAMB)
		{
			get_amulet_message(L_PR_C_CHAMBER,message);    // "HR CHAMBER"
			strncpy(&strng[2],message,strlen(message));
		}else if(ch_vals.chamb_type == K_CHAMB)
		{
			get_amulet_message(L_PR_K_CHAMBER,message);    // "1K CHAMBER"
			strncpy(&strng[2],message,strlen(message));
		}
        pr_write(strng);
        ++line;

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[2],"S / N:",6);
        get_message(ACC_10,&strng[2]);
        strncpy(&strng[9],&ch_vals.sn[0],6);
        pr_write(strng);
        ++line;

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[2],"Response Correction:",20);
        //get_message(DIAG_26,&strng[2]);
		get_amulet_message(L_PR_RESPONSE_CORRECTION,message);    // "Response Correction:"
		strncpy(&strng[2],message,strlen(message));
        
        if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
            kp = 23;
        else
        {
            pr_write(strng);
            ++line;
            lininit(strng,TRUE,prtype);
            kp = 6;
        }    
        resp = 100. * ch_vals.resp_corr;
        //strncpy(&strng[kp],"     %",6);
        strcpy(&strng[kp],"     %  ");
        if(resp != 0.)
        {    
            //strncpy(&strng[kp + 8],"(Co60         )",15);
            if (resp >= 0.)
                //strncpy(&strng[kp + 18],"HIGH",4);
                //get_message(DIAG_27,&strng[kp + 14]);
				get_amulet_message(L_PR_WAS_HIGH,message);    // "(Co60 was HIGH)"
            if (resp < 0.)
            {
                //strncpy(&strng[kp + 18],"LOW ",4);
                //get_message(DIAG_28,&strng[kp + 14]);
				get_amulet_message(L_PR_WAS_LOW,message);    // "(Co60 was LOW )"
                resp = -resp;
            }
			strcat(strng,message);
        }    
        nc = sprintf(dactstr,"%5.2f",resp);
        strncpy(&strng[kp + 4 - nc],dactstr,nc);
		strcat(strng,"\r\n");
        pr_write(strng);
        ++line;
        
        lininit(strng,TRUE,prtype);
        //strncpy(&strng[2],"Nominal Voltage:       V",24);
        //get_message(DIAG_29,&strng[2]);
		get_amulet_message(L_PR_NOMINAL_VOLTAGE,message);    // "Nominal Voltage: "
		strcpy(&strng[2],message);
        //strng[25] = 'V';
        nc = sprintf(dactstr,"%6.1f",ch_vals.nomvolts);
        //kp = 24 - nc;
        //strncpy(&strng[kp],dactstr,nc);
		strcat(strng,dactstr);
		strcat(strng," V\r\n");
        pr_write(strng);
        ++line;

        if (((ch_vals.feature_flag1) & HV_ADC_MASK) != 0)
        {
			lininit(strng,TRUE,prtype);
        	//strncpy(&strng[2], "HV directly into ADC", 20);
			get_amulet_message(L_PR_HV_DIRECTLY_INTO_ADC,message);    // "HV directly into ADC"
			strcpy(&strng[2],message);
			strcat(strng,"\r\n");
        	pr_write(strng);
        	++line;
        }

        if (((ch_vals.feature_flag1) & ONE_GAIN_RELAY_MASK) != 0)
        {
			lininit(strng,TRUE,prtype);
        	//strncpy(&strng[2], "Single Gain Relay", 17);
			get_amulet_message(L_PR_SINGLE_GAIN_RELAY,message);    // "Single Gain Relay"
			strcpy(&strng[2],message);
			strcat(strng,"\r\n");
        	pr_write(strng);
        	++line;
        }

        if (((ch_vals.feature_flag1) & STORED_GAIN_FACTOR) != 0)
        {
			lininit(strng,TRUE,prtype);
        	//sprintf(&strng[2],"Gain: %4.6f", ch_vals.gainfactor0);
			get_amulet_message(L_PR_GAIN,message);    // "Gain: %4.6f"
        	sprintf(&strng[2],message, ch_vals.gainfactor0);
			for (i=2; i<90; i++)
			{
				if (strng[i]==0)
				{
					strng[i]=' ';
					i=90;
				}
			}
        	pr_write(strng);
        	++line;
        }
        feed(1,prtype);
        ++line;

    }

	//USB HP printer only
	static void pr_chamber_parameters_hp(short ch_num)
    {
        
        float resp;
        //short nc;
        char strng[200];
        //char dactstr[16];
        //short kp;
		//int i;
		char buff[10];
        CHAMBERVALS ch_vals;
		char message[50],str[50];

        if(current.demo_mode)
            memcpy(&ch_vals,&chamb_vals[ch_num],sizeof(ch_vals));
        else
            //read in values for chamber
            read_from_chamber(ch_num,&ch_vals);


        lininit3(strng,TRUE,prtype);
		//insertconst3(strng,3,0,0,"@1Chamber   :@8");
		get_amulet_message(L_CHAMBER,message);    // "Chamber"
		if(current.num_chambers == 1)
		{	
			strcpy(str,"@1");
			strcat(str,message);
			strcat(str,":@8");
			insertconst3(strng,3,0,0,str);
		}
		else
        {
			strcpy(str,"@1");
			strcat(str,message);
			strcat(str," #");
            buff[0] = (char)ch_num + '1';
			buff[1] = ':';
			buff[2] = '\0';
			strcat(str,buff);
			strcat(str,"@8");
			insertconst3(strng,3,0,0,str);
			//insertstring3(strng,11,0,0,"%s",buff);
        }
		pr_write3_language(strng,"DIAGNOSTICS");
        ++line;
        

		if(ch_vals.chamb_type == R_CHAMB)
		{	
			//insertconst3(strng,5,0,0,"R CHAMBER");
			get_amulet_message(L_PR_R_CHAMBER,message);    // "R CHAMBER"
			insertconst3(strng,5,0,0,message);
		}	
		else if(ch_vals.chamb_type == P_CHAMB)
		{	
			//insertconst3(strng,5,0,0,"PET CHAMBER");
			get_amulet_message(L_PR_PET_CHAMBER,message);    // "PET CHAMBER"
			insertconst3(strng,5,0,0,message);
		}
		else if(ch_vals.chamb_type == B_CHAMB)
		{
			get_amulet_message(L_PR_BT_CHAMBER,message);    // "BT CHAMBER"
			insertconst3(strng,5,0,0,message);
		}
		else if(ch_vals.chamb_type == ONE_DOT_EIGHT_CHAMB)
		{
			get_amulet_message(L_PR_18_CHAMBER,message);    // "1.8 Atm CHAMBER"
			insertconst3(strng,5,0,0,message);
		}
		else if(ch_vals.chamb_type == C_CHAMB)
		{
			get_amulet_message(L_PR_C_CHAMBER,message);    // "HR CHAMBER"
			insertconst3(strng,5,0,0,message);
		}
		else if(ch_vals.chamb_type == K_CHAMB)
		{
			get_amulet_message(L_PR_K_CHAMBER,message);    // "1K CHAMBER"
			insertconst3(strng,5,0,0,message);
		}

        strncpy(buff,&ch_vals.sn[0],6);
		buff[6] = '\0';
		insertstring3(strng,25,0,0,"S/N: %s",buff);
		pr_write3_language(strng,"DIAGNOSTICS");
		

        //lininit(strng,TRUE,prtype);
        //strncpy(&strng[2],"Response Correction:",20);
        //get_message(DIAG_26,&strng[2]);
        
        resp = 100. * ch_vals.resp_corr;
		if(resp == 0.0)
		{	
			//insertconst3(strng,5,0,0,"Response Correction: 0.00%");
			get_amulet_message(L_PR_RESPONSE_CORRECTION,message);    // "Response Correction:"
			strcat(message," 0.00%");
			insertconst3(strng,5,0,0,message);
		}	
		else
		{
			//insertconst3(strng,5,0,0,"Response Correction:");
			get_amulet_message(L_PR_RESPONSE_CORRECTION,message);    // "Response Correction:"
			insertconst3(strng,5,0,0,message);
			if(resp > 0.)
			{	
				//insertconst3(strng,26,0,0,"(Co60 HIGH)");
				get_amulet_message(L_PR_CO60_HIGH,message);    // "(Co60 HIGH)"
				insertconst3(strng,26,0,0,message);
			}	
			else
			{
				
				//insertconst3(strng,26,0,0,"(Co60 LOW )");
				get_amulet_message(L_PR_CO60_LOW,message);    // "(Co60 LOW )"
				insertconst3(strng,26,0,0,message);
				resp = -resp;
			}
			insertfloat3(strng,39,0,0,"%5.2f%%",resp);
		}
		pr_write3_language(strng,"DIAGNOSTICS");
/*        strncpy(&strng[kp],"     %",6);
 -         if(resp != 0.)
 -         {    
 -             strncpy(&strng[kp + 8],"(Co60         )",15);
 -             if (resp >= 0.)
 -                 //strncpy(&strng[kp + 18],"HIGH",4);
 -                 get_message(DIAG_27,&strng[kp + 14]);
 -             if (resp < 0.)
 -             {
 -                 //strncpy(&strng[kp + 18],"LOW ",4);
 -                 get_message(DIAG_28,&strng[kp + 14]);
 -                 resp = -resp;
 -             }
 -         }    
 -         nc = sprintf(dactstr,"%5.2f",resp);
 -         strncpy(&strng[kp + 4 - nc],dactstr,nc);
 -         pr_write(strng);
 -         ++line;*/
        
/*        lininit(strng,TRUE,prtype);
 -         //strncpy(&strng[2],"Nominal Voltage:       V",24);
 -         get_message(DIAG_29,&strng[2]);
 -         strng[25] = 'V';
 -         nc = sprintf(dactstr,"%6.1f",ch_vals.nomvolts);
 -         kp = 24 - nc;
 -         strncpy(&strng[kp],dactstr,nc);
 -         pr_write(strng);
 -         ++line;*/

		//insertfloat3(strng,5,0,0,"Nominal Voltage: %6.1f V",ch_vals.nomvolts);
		get_amulet_message(L_PR_NOMINAL_VOLTAGE,message);    // "Nominal Voltage: "
		strcat(message," %6.1f V");
		insertfloat3(strng,5,0,0,message,ch_vals.nomvolts);
		pr_write3_language(strng,"DIAGNOSTICS");

		if (((ch_vals.feature_flag1) & HV_ADC_MASK) != 0)
        {
			//insertconst3(strng,5,0,0,"HV directly into ADC");
			get_amulet_message(L_PR_HV_DIRECTLY_INTO_ADC,message);    // "HV directly into ADC"
			insertconst3(strng,5,0,0,message);
			pr_write3_language(strng,"DIAGNOSTICS");
        }

        if (((ch_vals.feature_flag1) & ONE_GAIN_RELAY_MASK) != 0)
        {
			//insertconst3(strng,5,0,0,"Single Gain Relay");
			get_amulet_message(L_PR_SINGLE_GAIN_RELAY,message);    // "Single Gain Relay"
			insertconst3(strng,5,0,0,message);
			//pr_write3_language(strng,"DIAGNOSTICS");
        }

        if (((ch_vals.feature_flag1) & STORED_GAIN_FACTOR) != 0)
        {
			//insertfloat3(strng,30,0,0,"Gain: %4.6f",ch_vals.gainfactor0);
			get_amulet_message(L_PR_GAIN,message);    // "Gain: %4.6f"
			insertfloat3(strng,30,0,0,message,ch_vals.gainfactor0);
        }
		pr_write3_language(strng,"DIAGNOSTICS");
        feed(1,prtype);
        ++line;

    }

void strchop(char *input, char *output, int length, char delimiter){
	int index, size;

	if(strlen(input) < 23){
		strcpy(output, input);
		input[0] = 0;
	}else{
		index = length;
		while(input[index] != delimiter) index--;
		input[index] = 0;
		strcpy(output, input);
		index++;
		index++;
		size = index;
		while(input[index] != 0){
			input[index - size] = input[index];
			index++;
		}
		input[index - size] = 0;
	}
}

    static void pr_well_counter_parameters(void){
    	DETECTOR detectorMirror;
    	char strng[90];
    	char sn[20];
    	char buff[90];
    	char nuclidename[90];
    	char nuclideline[24];
    	int index, jndex;
    	float hv, hvcode;
    	WELLWIPELOCATION *location;
    	bool flgFound;

		if(prtype == SLIP_PRINTER)
			formfeed(ROLL_PRINTER);
		else		
			formfeed(prtype);
    	prhead_languagesec(prtype);
    	feed(1,prtype);
    	pr_set_linecnt(5);

		if(prtype == USB_PRINTER)
			pcl_cpi("12",TRUE);

    	lininit(strng, TRUE, prtype);

    	if(Mca_installedDetector == DET_WELL){
    		strncpy(&(strng[2]), "Well Counter", 12);
    		pr_write2_languagesec(strng, "DIAGNOSTICS");

    		memcpy(&detectorMirror, Mca_detectorMirror(), sizeof(DETECTOR));
    		for(index=0;index<6;index++) sn[index] = detectorMirror.snum[index];
    		sn[6] = 0;
    		sprintf(buff, "S/N: %s", sn);
    		strncpy(&(strng[4]), buff, strlen(buff));
    		pr_write2_languagesec(strng, "DIAGNOSTICS");

    		hvcode = detectorMirror.hv;
    		hv = (.158974 * hvcode) + 595.0;
    		sprintf(buff, "HV: %.0f Volts", hv);
    		strncpy(&(strng[4]), buff, strlen(buff));
    		pr_write2_languagesec(strng, "DIAGNOSTICS");

    		sprintf(buff, "Threshold: %d", detectorMirror.threshold);
    		strncpy(&(strng[4]), buff, strlen(buff));
    		pr_write2_languagesec(strng, "DIAGNOSTICS");

    		sprintf(buff, "Channels: %d", detectorMirror.num_of_channels);
    		strncpy(&(strng[4]), buff, strlen(buff));
    		pr_write2_languagesec(strng, "DIAGNOSTICS");

    		feed2(1,prtype);
    	}else{
    		memcpy(&detectorMirror, Mca_detectorWell700Mirror(), sizeof(DETECTOR));
    		if(detectorMirror.installed != DET_EMPTY){
    			if(detectorMirror.installed == DET_WELL700){
    				strncpy(&(strng[2]), "Well Counter", 12);
    				pr_write2_languagesec(strng, "DIAGNOSTICS");

    				for(index=0;index<6;index++) sn[index] = detectorMirror.snum[index];
					sn[6] = 0;
					sprintf(buff, "S/N: %s", sn);
					strncpy(&(strng[4]), buff, strlen(buff));
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					hvcode = detectorMirror.hv;
					hv = .263158 * hvcode;
					sprintf(buff, "HV: %.0f Volts", hv);
					strncpy(&(strng[4]), buff, strlen(buff));
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					sprintf(buff, "Threshold: %d", detectorMirror.threshold);
					strncpy(&(strng[4]), buff, strlen(buff));
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					sprintf(buff, "Channels: %d", detectorMirror.num_of_channels);
					strncpy(&(strng[4]), buff, strlen(buff));
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					feed2(1,prtype);
    			}
    		}

    		memcpy(&detectorMirror, Mca_detectorProbe700Mirror(), sizeof(DETECTOR));
    		if(detectorMirror.installed != DET_EMPTY){
    			flgFound = FALSE;
    			if(detectorMirror.installed == DET_PROBE700){
    				strncpy(&(strng[2]), "Probe Counter", 13);
    				flgFound = TRUE;
    			}else if(detectorMirror.installed == DET_DRILLEDPROBE700){
    				strncpy(&(strng[2]), "Drilled Probe Counter", 21);
    				flgFound = TRUE;
    			}

    			if(flgFound){
    				pr_write2_languagesec(strng, "DIAGNOSTICS");
    				for(index=0;index<6;index++) sn[index] = detectorMirror.snum[index];
					sn[6] = 0;
					sprintf(buff, "S/N: %s", sn);
					strncpy(&(strng[4]), buff, strlen(buff));
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					hvcode = detectorMirror.hv;
					hv = .263158 * hvcode;
					sprintf(buff, "HV: %.0f Volts", hv);
					strncpy(&(strng[4]), buff, strlen(buff));
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					sprintf(buff, "Threshold: %d", detectorMirror.threshold);
					strncpy(&(strng[4]), buff, strlen(buff));
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					sprintf(buff, "Channels: %d", detectorMirror.num_of_channels);
					strncpy(&(strng[4]), buff, strlen(buff));
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					feed2(1,prtype);
    			}
    		}
    	}

		insertconst(strng, 2, 0, 0, "DETECTOR TEST SOURCE");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		if(AmuletWellMainScreenMenu_getWellStandardActivity() > 0){
			insertconst(strng, 3, 0, 0, "Nuclide: Cs137");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			AmuletWellMainScreenMenu_getWellStandardSN(buff);
			insertstring(strng, 7, 0, 0, "S/N: %s", buff);
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			insertconst(strng, 6, 0, 0, "Date:");
			inserttime_t_language(strng, 12, 0, 0, AmuletWellMainScreenMenu_getWellStandardDate(), TRUE);
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			insertconst(strng, 2, 0, 0, "Activity:");
			if(current.system == CI) format_activity_system2(AmuletWellMainScreenMenu_getWellStandardActivity(), buff);
			else format_activity_system_kbq(AmuletWellMainScreenMenu_getWellStandardActivity(), buff);
			if(prtype == USB_EPS_PRINTER) replace(buff, '$', 'u');
			else replace(buff, '$', MU_PRT);
			insertconst(strng, 12, 0, 0, buff);
			pr_write2_languagesec(strng, "DIAGNOSTICS");
		}else{
			insertconst(strng, 2, 0, 0, "has not been defined");
			pr_write2_languagesec(strng, "DIAGNOSTICS");
		}

		feed2(1,prtype);

		insertconst(strng, 2, 0, 0, "Wipe Bkg Trigger:");

		if(current.system == CI){
			insertconst(strng, 33, 1, 0, "cpm");
			insertfloat(strng, 29, 1, 0, "%.1f", (WipesCurrentType(1))->Threshold);
		}else{
			insertconst(strng, 33, 1, 0, "cps");
			insertfloat(strng, 29, 1, 0, "%.1f", ((WipesCurrentType(1))->Threshold)/60.0);
		}
		pr_write2_languagesec(strng, "DIAGNOSTICS");



		// DEFAULT VALUES FOR WIPE TYPES

		insertconst(strng, 0, 0, 0, "==================================");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "DEFAULT WORK AREA");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "Trigger:");
		if(current.system == CI){
			insertconst(strng, 33, 1, 0, "dpm");
			insertfloat(strng, 29, 1, 0, "%.2f", (WipesCurrentType(2))->Threshold);
		}else{
			insertconst(strng, 33, 1, 0, "Bq");
			insertfloat(strng, 29, 1, 0, "%.3f", Mca_convertDpmToBq((WipesCurrentType(2))->Threshold));
		}
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 5, 0, 0, "Time:");
		insertconst(strng, 33, 1, 0, "sec");
		insertint(strng, 29, 1, 0, "%d", (WipesCurrentType(2))->CountTime);
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "Nuclide:");
		buff[0] = 0;
		for(index=0; index<10; index++){
			if((WipesCurrentType(2))->NuclideID[index] >= 0){
				NuclideData_getName((WipesCurrentType(2))->NuclideID[index], nuclidename);
				trim_and_shrink(nuclidename);
				strcat(nuclidename, ", ");
				strcat(buff, nuclidename);
			}
		}
		buff[strlen(buff) - 2] = 0;

		if(strlen(buff) > 0){
			strchop(buff, nuclideline, 23, ',');
			insertstring(strng, 11, 0, 0, "%s", nuclideline);
		}
		pr_write2_languagesec(strng, "DIAGNOSTICS");


		while(strlen(buff) != 0){
			strchop(buff, nuclideline, 23, ',');
			insertstring(strng, 11, 0, 0, "%s", nuclideline);
			pr_write2_languagesec(strng, "DIAGNOSTICS");
		}

		insertconst(strng, 0, 0, 0, "==================================");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "DEFAULT UNRESTRICTED AREA");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "Trigger:");
		if(current.system == CI){
			insertconst(strng, 33, 1, 0, "dpm");
			insertfloat(strng, 29, 1, 0, "%.2f", (WipesCurrentType(3))->Threshold);
		}else{
			insertconst(strng, 33, 1, 0, "Bq");
			insertfloat(strng, 29, 1, 0, "%.3f", Mca_convertDpmToBq((WipesCurrentType(3))->Threshold));
		}
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 5, 0, 0, "Time:");
		insertconst(strng, 33, 1, 0, "sec");
		insertint(strng, 29, 1, 0, "%d", (WipesCurrentType(3))->CountTime);
		pr_write2_languagesec(strng, "DIAGNOSTICS");


		insertconst(strng, 2, 0, 0, "Nuclide:");
		buff[0] = 0;
		for(index=0; index<10; index++){
			if((WipesCurrentType(3))->NuclideID[index] >= 0){
				NuclideData_getName((WipesCurrentType(3))->NuclideID[index], nuclidename);
				trim_and_shrink(nuclidename);
				strcat(nuclidename, ", ");
				strcat(buff, nuclidename);
			}
		}
		buff[strlen(buff) - 2] = 0;

		if(strlen(buff) > 0){
			strchop(buff, nuclideline, 23, ',');
			insertstring(strng, 11, 0, 0, "%s", nuclideline);
		}
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		while(strlen(buff) != 0){
			strchop(buff, nuclideline, 23, ',');
			insertstring(strng, 11, 0, 0, "%s", nuclideline);
			pr_write2_languagesec(strng, "DIAGNOSTICS");
		}

		insertconst(strng, 0, 0, 0, "==================================");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "DEFAULT SEALED SOURCE");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "Trigger:");
		if(current.system == CI){
			insertconst(strng, 33, 1, 0, "nCi");
			insertfloat(strng, 29, 1, 0, "%.3f", Mca_convertDpmToCi((WipesCurrentType(4))->Threshold) * 1e+9);
		}else{
			insertconst(strng, 33, 1, 0, "Bq");
			insertfloat(strng, 29, 1, 0, "%.3f", Mca_convertDpmToBq((WipesCurrentType(4))->Threshold));
		}
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 5, 0, 0, "Time:");
		insertconst(strng, 33, 1, 0, "sec");
		insertint(strng, 29, 1, 0, "%d", (WipesCurrentType(4))->CountTime);
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "Nuclide:");
		buff[0] = 0;
		for(index=0; index<10; index++){
			if((WipesCurrentType(4))->NuclideID[index] >= 0){
				NuclideData_getName((WipesCurrentType(4))->NuclideID[index], nuclidename);
				trim_and_shrink(nuclidename);
				strcat(nuclidename, ", ");
				strcat(buff, nuclidename);
			}
		}
		buff[strlen(buff) - 2] = 0;

		if(strlen(buff) > 0){
			strchop(buff, nuclideline, 23, ',');
			insertstring(strng, 11, 0, 0, "%s", nuclideline);
		}
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		while(strlen(buff) != 0){
			strchop(buff, nuclideline, 23, ',');
			insertstring(strng, 11, 0, 0, "%s", nuclideline);
			pr_write2_languagesec(strng, "DIAGNOSTICS");
		}

		insertconst(strng, 0, 0, 0, "==================================");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "DEFAULT PACKAGE");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "Trigger:");
		if(current.system == CI){
			insertconst(strng, 33, 1, 0, "dpm");
			insertfloat(strng, 29, 1, 0, "%.2f", (WipesCurrentType(5))->Threshold);
		}else{
			insertconst(strng, 33, 1, 0, "Bq");
			insertfloat(strng, 29, 1, 0, "%.3f", Mca_convertDpmToBq((WipesCurrentType(5))->Threshold));
		}
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 5, 0, 0, "Time:");
		insertconst(strng, 33, 1, 0, "sec");
		insertint(strng, 29, 1, 0, "%d", (WipesCurrentType(5))->CountTime);
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		insertconst(strng, 2, 0, 0, "Nuclide:");
		buff[0] = 0;
		for(index=0; index<10; index++){
			if((WipesCurrentType(5))->NuclideID[index] >= 0){
				NuclideData_getName((WipesCurrentType(5))->NuclideID[index], nuclidename);
				trim_and_shrink(nuclidename);
				strcat(nuclidename, ", ");
				strcat(buff, nuclidename);
			}
		}
		buff[strlen(buff) - 2] = 0;

		if(strlen(buff) > 0){
			strchop(buff, nuclideline, 23, ',');
			insertstring(strng, 11, 0, 0, "%s", nuclideline);
		}
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		while(strlen(buff) != 0){
			strchop(buff, nuclideline, 23, ',');
			insertstring(strng, 11, 0, 0, "%s", nuclideline);
			pr_write2_languagesec(strng, "DIAGNOSTICS");
		}

		insertconst(strng, 0, 0, 0, "==================================");
		pr_write2_languagesec(strng, "DIAGNOSTICS");

		if(ActiveWipeLocationCount() > 0){
			location = ActiveWipeLocations();

			feed2(1,prtype);

			insertconst(strng, 2, 0, 0, "WIPE LOCATIONS");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			for(index=0; index<ActiveWipeLocationCount(); index++){
				if(prtype == ROLL_PRINTER) delayloop(200);
				insertconst(strng, 0, 0, 0, "==================================");
				pr_write2_languagesec(strng, "DIAGNOSTICS");

				insertconst(strng, 9, 1, 0, "Name:");
				insertconst(strng, 11, 0, 0, location->Name);
				pr_write2_languagesec(strng, "DIAGNOSTICS");

				insertconst(strng, 9, 1, 0, "Type:");
				insertconst(strng, 11, 0, 0, location->WellWipeTypeName);
				pr_write2_languagesec(strng, "DIAGNOSTICS");

				insertconst(strng, 9, 1, 0, "Trigger:");
				if(current.system == CI){
					if(location->WellWipeTypeGroupID == 4){
						insertfloat(strng, 11, 0, 0, "%.2f nCi", Mca_convertDpmToCi(location->Threshold) * 1.0e+9);
					}else{
						insertfloat(strng, 11, 0, 0, "%.2f dpm", location->Threshold);
					}
				}else{
					insertfloat(strng, 11, 0, 0, "%.2f Bq", location->Threshold / 60.0);
				}
				pr_write2_languagesec(strng, "DIAGNOSTICS");

				insertconst(strng, 9, 1, 0, "Time:");
				insertint(strng, 11, 0, 0, "%d sec", location->CountTime);
				pr_write2_languagesec(strng, "DIAGNOSTICS");

				buff[0] = 0;
				for(jndex=0; jndex<10; jndex++){
					if(location->NuclideID[jndex] >= 0){
						NuclideData_getName(location->NuclideID[jndex], nuclidename);
						trim_and_shrink(nuclidename);
						strcat(nuclidename, ", ");
						strcat(buff, nuclidename);
					}
				}
				buff[strlen(buff) - 2] = 0;

				insertconst(strng, 2, 0, 0, "Nuclide:");
				if(strlen(buff) > 0){
					strchop(buff, nuclideline, 23, ',');
					insertstring(strng, 11, 0, 0, "%s", nuclideline);
				}
				pr_write2_languagesec(strng, "DIAGNOSTICS");

				while(strlen(buff) != 0){
					strchop(buff, nuclideline, 23, ',');
					insertstring(strng, 11, 0, 0, "%s", nuclideline);
					pr_write2_languagesec(strng, "DIAGNOSTICS");
				}
				location++;
			}

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2_languagesec(strng, "DIAGNOSTICS");
		}

		if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)){
			if(prtype == ROLL_PRINTER) delayloop(400);
			insertconst(strng, 0, 0, 0, "BIOASSAY");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			insertint(strng, 0, 0, 0, "    Count Time: %d sec", BioAssayCountTime());
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			insertint(strng, 0, 0, 0, "Probe Distance: %d cm", BioAssayProbeDistance());
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			pr_write2_languagesec(strng, "DIAGNOSTICS");

			insertconst(strng, 0, 0, 0, "I131 BioAssay Parameters");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Used in BioAssay
			if(BioAssayI131Active()){
				insertconst(strng, 2, 0, 0, "Used in BioAssay");
			}else{
				insertconst(strng, 2, 0, 0, "Not Used in BioAssay");
			}
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// ROI
			sprintf(buff, "ROI(keV): %.1f - %.1f", I131Lower_keV, I131Upper_keV);
			insertconst(strng, 2, 0, 0, buff);
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Efficiency
			if(BioAssayI131Efficiency() > 0) insertfloat(strng, 2, 0, 0, "Efficiency: %.3f %%", BioAssayI131Efficiency());
			else insertconst(strng, 2, 0, 0, "Efficiency: N/A");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Cross Contamination Factor
			insertconst(strng, 2, 0, 0, "Cross Contamination");
			pr_write2_languagesec(strng, "DIAGNOSTICS");
			insertfloat(strng, 2, 0, 0, "Factor: %.5f", BioAssayI131I125Contamination());
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Trigger Level
			if(current.system == CI) format_activity_system2(BioAssayI131Threshold(), buff);
			else format_activity_system_kbq(BioAssayI131Threshold(), buff);
			replace(buff, '$', 'u');
			insertstring(strng, 2, 0, 0, "Trigger: %s", buff);
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			pr_write2_languagesec(strng, "DIAGNOSTICS");

			insertconst(strng, 0, 0, 0, "I125 BioAssay Parameters");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Used in BioAssay
			if(BioAssayI125Active()){
				insertconst(strng, 2, 0, 0, "Used in BioAssay");
			}else{
				insertconst(strng, 2, 0, 0, "Not Used in BioAssay");
			}
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// ROI
			sprintf(buff, "ROI(keV): %.1f - %.1f", I125Lower_keV, I125Upper_keV);
			insertconst(strng, 2, 0, 0, buff);
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Efficiency
			if(BioAssayI125Efficiency() > 0) insertfloat(strng, 2, 0, 0, "Efficiency: %.3f %%", BioAssayI125Efficiency());
			else insertconst(strng, 2, 0, 0, "Efficiency: N/A");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Trigger Level
			if(current.system == CI) format_activity_system2(BioAssayI125Threshold(), buff);
			else format_activity_system_kbq(BioAssayI125Threshold(), buff);
			replace(buff, '$', 'u');
			insertstring(strng, 2, 0, 0, "Trigger: %s", buff);
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			pr_write2_languagesec(strng, "DIAGNOSTICS");

			insertconst(strng, 0, 0, 0, "I123 BioAssay Parameters");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Used in BioAssay
			if(BioAssayI123Active()){
				insertconst(strng, 2, 0, 0, "Used in BioAssay");
			}else{
				insertconst(strng, 2, 0, 0, "Not Used in BioAssay");
			}
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// ROI
			sprintf(buff, "ROI(keV): %.1f - %.1f", I123Lower_keV, I123Upper_keV);
			insertconst(strng, 2, 0, 0, buff);
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Efficiency
			if(BioAssayI123Efficiency() > 0) insertfloat(strng, 2, 0, 0, "Efficiency: %.3f %%", BioAssayI123Efficiency());
			else insertconst(strng, 2, 0, 0, "Efficiency: N/A");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Cross Contamination Factor
			insertconst(strng, 2, 0, 0, "Cross Contamination");
			pr_write2_languagesec(strng, "DIAGNOSTICS");
			insertfloat(strng, 2, 0, 0, "Factor: %.5f", BioAssayI123I125Contamination());
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			// Trigger Level
			if(current.system == CI) format_activity_system2(BioAssayI123Threshold(), buff);
			else format_activity_system_kbq(BioAssayI123Threshold(), buff);
			replace(buff, '$', 'u');
			insertstring(strng, 2, 0, 0, "Trigger: %s", buff);
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			/*protocol_count = DB_ActiveProbeThyroidUptakeProtocolCount();
			if(protocol_count > 0){
				if(prtype == ROLL_PRINTER) delayloop(1000);
				ThyroidUptakeProtocolActiveName = malloc(protocol_count * sizeof(PROBETHYROIDUPTAKEPROTOCOLACTIVENAME));
				DB_RetrieveActiveProbeThyroidUptakeProtocolNames(ThyroidUptakeProtocolActiveName);
				for(index=0; index<protocol_count; index++){
					ThyroidUptakeProtocol.ProbeTUProtocolID = ThyroidUptakeProtocolActiveName[index].ProbeTUProtocolID;
					DB_RetrieveProbeThyroidUptakeProtocol(&ThyroidUptakeProtocol);
					if(ThyroidUptakeProtocol.ProbeTUProtocolID <= 0){
						Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
						return;
					}

					insertconst(strng, 0, 0, 0, "Thyroid Uptake Protocol");
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// Thyroid Uptake Protocol Name
					insertstring(strng, 0, 0, 0, "Name: %s", ThyroidUptakeProtocol.ProtocolName);
					pr_write2_languagesec(strng, "DIAGNOSTICS");
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// Counting Method - Dose Decay or Reference
					if(ThyroidUptakeProtocol.DoseDecayCorrect) insertconst(strng, 0, 0, 0, "Decay Correct Administered Dose");
					else insertconst(strng, 0, 0, 0, "Measure Reference Dose");
					pr_write2_languagesec(strng, "DIAGNOSTICS");
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// Dose Measurement
					switch(ThyroidUptakeProtocol.DoseMeasurementMethod){
						case 1:
							insertconst(strng, 4, 0, 0, "Dose Measured is");
							pr_write2_languagesec(strng, "DIAGNOSTICS");
							insertconst(strng, 4, 0, 0, "Dose Administered");
							break;

						case 2:
							insertconst(strng, 4, 0, 0, "Measure Each Dose and");
							pr_write2_languagesec(strng, "DIAGNOSTICS");
							insertconst(strng, 4, 0, 0, "Add Activity");
							break;

						case 3:
							insertconst(strng, 4, 0, 0, "Multiply Number Administered");
							pr_write2_languagesec(strng, "DIAGNOSTICS");
							insertconst(strng, 4, 0, 0, "by One Measured Capsule");
							break;

						case 4:
							insertconst(strng, 4, 0, 0, "Measure Liquid and");
							pr_write2_languagesec(strng, "DIAGNOSTICS");
							insertconst(strng, 4, 0, 0, "Multipy by Factor");
							break;
					}
					pr_write2_languagesec(strng, "DIAGNOSTICS");
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// Isotope
					switch(ThyroidUptakeProtocol.NuclideID){
						case 40:
							insertconst(strng, 0, 0, 0, "Nuclide: I123");
							break;
						case 45:
							insertconst(strng, 0, 0, 0, "Nuclide: I131");
							break;
						case 84:
							insertconst(strng, 0, 0, 0, "Nuclide: Tc99m");
							break;
					}
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// Dose Form
					switch(ThyroidUptakeProtocol.DoseForm){
						case 1:
							insertconst(strng, 0, 0, 0, "Dose Form: Capsule");
							break;

						case 2:
							insertconst(strng, 0, 0, 0, "Dose Form: Liquid");
							break;
					}
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// PreDose
					if(ThyroidUptakeProtocol.PreDoseMeasurement) insertconst(strng, 0, 0, 0, "Pre-Dose Measurement: Yes");
					else  insertconst(strng, 0, 0, 0, "Pre-Dose Measurement: No");
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// Residual
					if(ThyroidUptakeProtocol.ResidualMeasurement) insertconst(strng, 0, 0, 0, "Measure Residual: Yes");
					else insertconst(strng, 0, 0, 0, "Measure Residual: No");
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// Counting Time
					insertint(strng, 0, 0, 0, "Count Time(sec): %d", ThyroidUptakeProtocol.DefaultCountTime);
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// Probe Distance
					insertint(strng, 0, 0, 0, "Probe Distance(cm): %d", ThyroidUptakeProtocol.DefaultDistance);
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// I123 ROI
					sprintf(buff, " I123 ROI(keV): %.1f - %.1f", ThyroidUptakeProtocol.I123LowerEV, ThyroidUptakeProtocol.I123UpperEV);
					insertconst(strng, 0, 0, 0, buff);
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// I131 ROI
					sprintf(buff, " I131 ROI(keV): %.1f - %.1f", ThyroidUptakeProtocol.I131LowerEV, ThyroidUptakeProtocol.I131UpperEV);
					insertconst(strng, 0, 0, 0, buff);
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					// Tc99m ROI
					sprintf(buff, "Tc99m ROI(keV): %.1f - %.1f", ThyroidUptakeProtocol.Tc99mLowerEV, ThyroidUptakeProtocol.Tc99mUpperEV);
					insertconst(strng, 0, 0, 0, buff);
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					insertconst(strng, 0, 0, 0, "==================================");
					pr_write2_languagesec(strng, "DIAGNOSTICS");
				}
				free(ThyroidUptakeProtocolActiveName);

				if((ThyroidUptake_normal[0].hour != -1) || (ThyroidUptake_normal[1].hour != -1) || (ThyroidUptake_normal[2].hour != -1) || (ThyroidUptake_normal[3].hour != -1)){
					insertconst(strng, 0, 0, 0, "Thyroid Uptake Normal Range");
					pr_write2_languagesec(strng, "DIAGNOSTICS");
					pr_write2_languagesec(strng, "DIAGNOSTICS");

					insertconst(strng, 0, 0, 0, " Hour      Min %      Max %");
					pr_write2_languagesec(strng, "DIAGNOSTICS");
					for(index=0; index<4; index++){
						if(ThyroidUptake_normal[index].hour != -1){
							insertint(strng, 3, 1, 0, "%d",  ThyroidUptake_normal[index].hour);
							insertint(strng, 13, 1, 0, "%d",  ThyroidUptake_normal[index].min);
							insertint(strng, 24, 1, 0, "%d",  ThyroidUptake_normal[index].max);
							pr_write2_languagesec(strng, "DIAGNOSTICS");
						}
					}
					insertconst(strng, 0, 0, 0, "==================================");
					pr_write2_languagesec(strng, "DIAGNOSTICS");
				}
			} */

			if((ThyroidUptake_normal[0].hour != -1) || (ThyroidUptake_normal[1].hour != -1) || (ThyroidUptake_normal[2].hour != -1) || (ThyroidUptake_normal[3].hour != -1)){
				insertconst(strng, 0, 0, 0, "Thyroid Uptake Normal Range");
				pr_write2_languagesec(strng, "DIAGNOSTICS");
				pr_write2_languagesec(strng, "DIAGNOSTICS");

				insertconst(strng, 0, 0, 0, " Hour      Min %      Max %");
				pr_write2_languagesec(strng, "DIAGNOSTICS");
				for(index=0; index<4; index++){
					if(ThyroidUptake_normal[index].hour != -1){
						insertint(strng, 3, 1, 0, "%d",  ThyroidUptake_normal[index].hour);
						insertint(strng, 13, 1, 0, "%d",  ThyroidUptake_normal[index].min);
						insertint(strng, 24, 1, 0, "%d",  ThyroidUptake_normal[index].max);
						pr_write2_languagesec(strng, "DIAGNOSTICS");
					}
				}
				insertconst(strng, 0, 0, 0, "==================================");
				pr_write2_languagesec(strng, "DIAGNOSTICS");
			}
		}

		if((RBCSurvival_minNormal > 0) && (RBCSurvival_maxNormal > 0)){
			insertconst(strng, 0, 0, 0, "RBC Survival Normal Range");
			pr_write2_languagesec(strng, "DIAGNOSTICS");
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			sprintf(buff, "Normal Range: %.1f - %.1f Days", RBCSurvival_minNormal, RBCSurvival_maxNormal);
			insertconst(strng, 0, 0, 0, buff);
			pr_write2_languagesec(strng, "DIAGNOSTICS");

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2_languagesec(strng, "DIAGNOSTICS");
		}
    }

	//for USB HP printer only
	static void pr_well_counter_parameters_hp(bool t_flag){
    	DETECTOR detectorMirror;
    	char strng[200];
    	char sn[20];
    	char buff[90];
    	char nuclidename[90];
    	int index, jndex;
    	float hv, hvcode;
    	WELLWIPELOCATION *location;
    	bool flgFound;
		char c_ustr[4],a_ustr[4];
		float trigger;
		short kp;
		short loc_page;
		bool page_printed;

		if(current.system == CI)
		{	
			strcpy(c_ustr,"cpm");
			strcpy(a_ustr,"dpm");
		}	
		else
		{	
			strcpy(c_ustr,"cps");
			strcpy(a_ustr,"Bq");
		}	

		pr_set_linecnt(3);
		rawheader_languagesec(prtype, "",  clock_time);

    	pr_set_linecnt(5);

		pcl_cpi("12",TRUE);

		line = 5;

    	lininit3(strng, TRUE, prtype);

    	if(Mca_installedDetector == DET_WELL){
			insertconst3(strng,0,0,0,"@1Well Counter@8");
    		pr_write3_language(strng, "DIAGNOSTICS");
			++line;

    		memcpy(&detectorMirror, Mca_detectorMirror(), sizeof(DETECTOR));
    		for(index=0;index<6;index++) sn[index] = detectorMirror.snum[index];
    		sn[6] = 0;
			insertstring3(strng,2,0,0,"S/N: %s",sn);

    		hvcode = detectorMirror.hv;
    		hv = (.158974 * hvcode) + 595.0;
			insertfloat3(strng,20,0,0,"HV: %0.f Volts",hv);

			insertint3(strng,40,0,0,"Threshold: %d",detectorMirror.threshold);

			insertint3(strng,60,0,0,"Channels: %d",detectorMirror.num_of_channels);
    		pr_write3_language(strng, "DIAGNOSTICS");
			++line;

    		feed2(1,prtype);
			++line;
    	}else{
    		memcpy(&detectorMirror, Mca_detectorWell700Mirror(), sizeof(DETECTOR));
    		if(detectorMirror.installed != DET_EMPTY){
    			if(detectorMirror.installed == DET_WELL700){
					insertconst3(strng,0,0,0,"@1Well Counter@8");
    				pr_write3_language(strng, "DIAGNOSTICS");
					++line;

    				for(index=0;index<6;index++) sn[index] = detectorMirror.snum[index];
					sn[6] = 0;
					insertstring3(strng,2,0,0,"S/N: %s",sn);

					hvcode = detectorMirror.hv;
					hv = .263158 * hvcode;
					insertfloat3(strng,20,0,0,"HV: %0.f Volts",hv);

					insertint3(strng,40,0,0,"Threshold: %d",detectorMirror.threshold);

					insertint3(strng,60,0,0,"Channels: %d",detectorMirror.num_of_channels);
					pr_write3_language(strng, "DIAGNOSTICS");
					++line;

					feed2(1,prtype);
					++line;
    			}
    		}

    		memcpy(&detectorMirror, Mca_detectorProbe700Mirror(), sizeof(DETECTOR));
    		if(detectorMirror.installed != DET_EMPTY){
    			flgFound = FALSE;
    			if(detectorMirror.installed == DET_PROBE700){
					insertconst3(strng,0,0,0,"@1Probe Counter@8");
    				flgFound = TRUE;
    			}else if(detectorMirror.installed == DET_DRILLEDPROBE700){
					insertconst3(strng,0,0,0,"@1Drilled Probe Counter@8");
    				flgFound = TRUE;
    			}

    			if(flgFound){
    				pr_write3_language(strng, "DIAGNOSTICS");
					++line;
					
    				for(index=0;index<6;index++) sn[index] = detectorMirror.snum[index];
					sn[6] = 0;
					insertstring3(strng,2,0,0,"S/N: %s",sn);

					hvcode = detectorMirror.hv;
					hv = .263158 * hvcode;
					insertfloat3(strng,20,0,0,"HV: %0.f Volts",hv);
					insertint3(strng,40,0,0,"Threshold: %d",detectorMirror.threshold);
					
					insertint3(strng,60,0,0,"Channels: %d",detectorMirror.num_of_channels);
					
					pr_write3_language(strng, "DIAGNOSTICS");
					++line;

					feed2(1,prtype);
					++line;
    			}
    		}
    	}

		if(t_flag)
		{	
			//Test Source
			insertconst3(strng, 0, 0, 0, "@1DETECTOR TEST SOURCE@8");
			pr_write3_language(strng, "DIAGNOSTICS");
	
			if(AmuletWellMainScreenMenu_getWellStandardActivity() > 0){
				insertconst3(strng, 3, 0, 0, "Nuclide: Cs137");
	
				AmuletWellMainScreenMenu_getWellStandardSN(buff);
				insertstring3(strng, 20, 0, 0, "S/N: %s", buff);
				pr_write3_language(strng, "DIAGNOSTICS");
	
				insertconst3(strng, 3, 0, 0, "Activity:");
				if(current.system == CI) format_activity_system2(AmuletWellMainScreenMenu_getWellStandardActivity(), buff);
				else format_activity_system_kbq(AmuletWellMainScreenMenu_getWellStandardActivity(), buff);
				replace(buff, '$', MU_PRT);
				insertconst3(strng, 13, 0, 0, buff);
				insertconst3(strng,24,0,0,"on");
				inserttime_t3_language(strng, 28, 0, 0, AmuletWellMainScreenMenu_getWellStandardDate(), TRUE);
				
				pr_write3_language(strng, "DIAGNOSTICS");
				
			}else{
				insertconst3(strng, 2, 0, 0, "has not been defined");
				pr_write3_language(strng, "DIAGNOSTICS");
				
			}
		}

		feed2(1,prtype);
			
		insertconst3(strng, 0, 0, 0, "@1Wipe Background@8");
		pr_write3_language(strng, "DIAGNOSTICS");
			++line;
		
		trigger = (WipesCurrentType(1))->Threshold;
		if(current.system == BQ)
			trigger /= 60.;
		insertconst3(strng,3,0,0,"Trigger:");
		insertfloat3(strng,18,1,0,"%.1f",trigger);
		insertconst3(strng,20,0,0,c_ustr);
		
		pr_write3_language(strng, "DIAGNOSTICS");

		feed2(1,prtype);

		// DEFAULT VALUES FOR WIPE TYPES

		insertconst3(strng, 0, 0, 0, "@1DEFAULT WORK AREA@8");
		pr_write3_language(strng, "DIAGNOSTICS");


		insertconst3(strng, 3, 0, 0, "Trigger :");
		if(current.system == CI){
			insertfloat3(strng, 19, 1, 0, "%.2f", (WipesCurrentType(2))->Threshold);
		}else{
			insertfloat3(strng, 19, 1, 0, "%.3f", Mca_convertDpmToBq((WipesCurrentType(2))->Threshold));
		}
		insertconst3(strng,21,0,0,a_ustr);
		insertconst3(strng, 30, 0, 0, "Time:");
		insertconst3(strng, 40, 0, 0, "sec");
		insertint3(strng, 38, 1, 0, "%d", (WipesCurrentType(2))->CountTime);
		pr_write3_language(strng, "DIAGNOSTICS");


		
		insertconst3(strng,3,0,0,"Nuclides:");
		for(index = 0; index < 5; index++)
		{	
			if((WipesCurrentType(2))->NuclideID[index] >= 0)
			{
				NuclideData_getName((WipesCurrentType(2))->NuclideID[index], nuclidename);
				kp = 10 * index + 15;
				insertstring3(strng,kp,0,0,"%s",nuclidename);
			}
		}
		pr_write3_language(strng, "DIAGNOSTICS");
		
		flgFound = FALSE;
		for(index = 5; index < 10; index++)
		{	
			if((WipesCurrentType(2))->NuclideID[index] >= 0)
			{
				NuclideData_getName((WipesCurrentType(2))->NuclideID[index], nuclidename);
				kp = 10 * (index - 5) + 15;
				insertstring3(strng,kp,0,0,"%s",nuclidename);
				flgFound = TRUE;
			}
		}
		pr_write3_language(strng, "DIAGNOSTICS");
		
		if(flgFound)
			feed2(1,prtype);
		
		insertconst3(strng, 0, 0, 0, "@1DEFAULT UNRESTRICTED AREA@8");
		pr_write3_language(strng, "DIAGNOSTICS");

		insertconst3(strng, 3, 0, 0, "Trigger :");
		if(current.system == CI){
			insertfloat3(strng, 19, 1, 0, "%.2f", (WipesCurrentType(3))->Threshold);
		}else{
			insertfloat3(strng, 19, 1, 0, "%.3f", Mca_convertDpmToBq((WipesCurrentType(3))->Threshold));
		}
		insertconst3(strng,21,0,0,a_ustr);
		insertconst3(strng, 30, 0, 0, "Time:");
		insertconst3(strng, 40, 0, 0, "sec");
		insertint3(strng, 38, 1, 0, "%d", (WipesCurrentType(3))->CountTime);
		pr_write3_language(strng, "DIAGNOSTICS");


		
		insertconst3(strng,3,0,0,"Nuclides:");
		for(index = 0; index < 5; index++)
		{	
			if((WipesCurrentType(3))->NuclideID[index] >= 0)
			{
				NuclideData_getName((WipesCurrentType(3))->NuclideID[index], nuclidename);
				kp = 10 * index + 15;
				insertstring3(strng,kp,0,0,"%s",nuclidename);
			}
		}
		pr_write3_language(strng, "DIAGNOSTICS");
		
		flgFound = FALSE;
		for(index = 5; index < 10; index++)
		{	
			if((WipesCurrentType(3))->NuclideID[index] >= 0)
			{
				NuclideData_getName((WipesCurrentType(3))->NuclideID[index], nuclidename);
				kp = 10 * (index - 5) + 15;
				insertstring3(strng,kp,0,0,"%s",nuclidename);
				flgFound = TRUE;
			}
		}
		pr_write3_language(strng, "DIAGNOSTICS");
		if(flgFound)
			feed2(1,prtype);

		insertconst3(strng, 0, 0, 0, "@1DEFAULT SEALED SOURCE@8");
		pr_write3_language(strng, "DIAGNOSTICS");

		insertconst3(strng, 3, 0, 0, "Trigger :");

		if(current.system == CI){
			insertconst(strng, 21, 0, 0, "nCi");
			insertfloat(strng, 19, 1, 0, "%.3f", Mca_convertDpmToCi((WipesCurrentType(4))->Threshold) * 1e+9);
		}else{
			insertconst(strng, 21, 0, 0, "Bq");
			insertfloat(strng, 19, 1, 0, "%.3f", Mca_convertDpmToBq((WipesCurrentType(4))->Threshold));
		}


		insertconst3(strng, 30, 0, 0, "Time:");
		insertconst3(strng, 40, 0, 0, "sec");
		insertint3(strng, 38, 1, 0, "%d", (WipesCurrentType(4))->CountTime);
		
		pr_write3_language(strng, "DIAGNOSTICS");

		insertconst3(strng,3,0,0,"Nuclides:");
		for(index = 0; index < 5; index++)
		{	
			if((WipesCurrentType(4))->NuclideID[index] >= 0)
			{
				NuclideData_getName((WipesCurrentType(4))->NuclideID[index], nuclidename);
				kp = 10 * index + 15;
				insertstring3(strng,kp,0,0,"%s",nuclidename);
			}
		}
		pr_write3_language(strng, "DIAGNOSTICS");
		
		flgFound = FALSE;
		for(index = 5; index < 10; index++)
		{	
			if((WipesCurrentType(4))->NuclideID[index] >= 0)
			{
				NuclideData_getName((WipesCurrentType(4))->NuclideID[index], nuclidename);
				kp = 10 * (index - 5) + 15;
				insertstring3(strng,kp,0,0,"%s",nuclidename);
				flgFound = TRUE;
			}
		}
		pr_write3_language(strng, "DIAGNOSTICS");
		if(flgFound)
			feed2(1,prtype);

		insertconst3(strng, 0, 0, 0, "@1DEFAULT PACKAGE@8");
		pr_write3_language(strng, "DIAGNOSTICS");

		insertconst3(strng, 3, 0, 0, "Trigger :");
		if(current.system == CI){
			insertfloat3(strng, 19, 1, 0, "%.2f", (WipesCurrentType(5))->Threshold);
		}else{
			insertfloat3(strng, 19, 1, 0, "%.3f", Mca_convertDpmToBq((WipesCurrentType(5))->Threshold));
		}
		insertconst3(strng,21,0,0,a_ustr);
		insertconst3(strng, 30, 0, 0, "Time:");
		insertconst3(strng, 40, 0, 0, "sec");
		insertint3(strng, 38, 1, 0, "%d", (WipesCurrentType(5))->CountTime);
		pr_write3_language(strng, "DIAGNOSTICS");


		insertconst3(strng,3,0,0,"Nuclides:");
		for(index = 0; index < 5; index++)
		{	
			if((WipesCurrentType(5))->NuclideID[index] >= 0)
			{
				NuclideData_getName((WipesCurrentType(5))->NuclideID[index], nuclidename);
				kp = 10 * index + 15;
				insertstring3(strng,kp,0,0,"%s",nuclidename);
			}
		}
		pr_write3_language(strng, "DIAGNOSTICS");
		
		flgFound = FALSE;
		for(index = 5; index < 10; index++)
		{	
			if((WipesCurrentType(5))->NuclideID[index] >= 0)
			{
				NuclideData_getName((WipesCurrentType(5))->NuclideID[index], nuclidename);
				kp = 10 * (index - 5) + 15;
				insertstring3(strng,kp,0,0,"%s",nuclidename);
				flgFound = TRUE;
			}
		}
		pr_write3_language(strng, "DIAGNOSTICS");
		if(flgFound)
			feed2(1,prtype);
			
			
		//WIPE LOCATIONS
		page_printed = FALSE;
		if(ActiveWipeLocationCount() > 0){
			location = ActiveWipeLocations();

			feed2(1,prtype);
			
			insertconst3(strng, 0, 0, 0, "@1WIPE LOCATIONS@8");
			pr_write3_language(strng, "DIAGNOSTICS");

			loc_page = 0;
			for(index=0; index<ActiveWipeLocationCount(); index++){
				if(index == 4 + 11 * loc_page)
				{
					++loc_page;
					print_page_num();
					pr_set_linecnt(3);
					rawheader_languagesec(prtype, "",  clock_time);
					lininit3(strng, TRUE, prtype);
					page_printed = TRUE;
					insertconst3(strng, 0, 0, 0, "@1WIPE LOCATIONS@8");
					pr_write3_language(strng, "DIAGNOSTICS");
				}
				else
					page_printed = FALSE;

				insertconst3(strng, 3, 0, 0, "Name:");
				insertconst3(strng, 9, 0, 0, location->Name);

				insertconst3(strng, 35, 0, 0, "Type:");
				insertconst3(strng, 41, 0, 0, location->WellWipeTypeName);
				pr_write3_language(strng, "DIAGNOSTICS");

				insertconst3(strng, 3, 0, 0, "Trigger :");
				if(current.system == CI){
					if(location->WellWipeTypeGroupID == 4){
						insertfloat(strng, 19, 1, 0, "%.2f", Mca_convertDpmToCi(location->Threshold) * 1.0e+9);
						insertconst3(strng, 21, 0, 0, "nCi");
					}else{
						insertfloat3(strng, 19, 1, 0, "%.2f", location->Threshold);
						insertconst3(strng, 21, 0, 0, "dpm");
					}
				}else{
					insertfloat3(strng, 19, 1, 0, "%.2f", location->Threshold / 60.0);
					insertconst3(strng, 21, 0, 0, "Bq");
				}

				insertconst3(strng, 30, 0, 0, "Time:");
				insertconst3(strng, 40, 0, 0, "sec");
				insertint3(strng, 38, 0, 0, "%d", location->CountTime);
				pr_write3_language(strng, "DIAGNOSTICS");

				insertconst3(strng,3,0,0,"Nuclides:");
				for(jndex = 0; jndex < 5; jndex++)
				{	
					if(location->NuclideID[jndex] >= 0)
					{
						NuclideData_getName(location->NuclideID[jndex], nuclidename);
						kp = 10 * jndex + 15;
						insertstring3(strng,kp,0,0,"%s",nuclidename);
					}
				}
				pr_write3_language(strng, "DIAGNOSTICS");
				
				flgFound = FALSE;
				for(jndex = 5; jndex < 10; jndex++)
				{	
					if(location->NuclideID[jndex] >= 0)
					{
						NuclideData_getName(location->NuclideID[jndex], nuclidename);
						kp = 10 * (jndex - 5) + 15;
						insertstring3(strng,kp,0,0,"%s",nuclidename);
						flgFound = TRUE;
					}
				}
				pr_write3_language(strng, "DIAGNOSTICS");
				if(flgFound)
					feed2(1,prtype);

				
				location++;
			}

		}

		feed2(1,prtype);
		if(!page_printed)
		{	
			print_page_num();
			pr_set_linecnt(3);
			rawheader_languagesec(prtype, "",  clock_time);
			lininit3(strng, TRUE, prtype);
		}
		
		if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)){
			insertconst3(strng, 0, 0, 0, "@1BIOASSAY@8");
			pr_write3_language(strng, "DIAGNOSTICS");

			insertint3(strng, 3, 0, 0, "Count Time: %d sec", BioAssayCountTime());

			insertint3(strng, 30, 0, 0, "Probe Distance: %d cm", BioAssayProbeDistance());
			pr_write3_language(strng, "DIAGNOSTICS");
			feed2(1,prtype);

			insertconst3(strng, 3, 0, 0, "@4I131 BioAssay Parameters@8");
			pr_write3_language(strng, "DIAGNOSTICS");

			// Used in BioAssay
			if(BioAssayI131Active()){
				insertconst3(strng, 3, 0, 0, "Used in BioAssay");
			}else{
				insertconst3(strng, 3, 0, 0, "Not Used in BioAssay");
			}
			pr_write3_language(strng, "DIAGNOSTICS");


			// ROI
			sprintf(buff, "ROI(keV): %.1f - %.1f", I131Lower_keV, I131Upper_keV);
			insertconst3(strng,3,0,0,buff);	

			// Efficiency
			if(BioAssayI131Efficiency() > 0)
				insertfloat3(strng, 40, 0, 0, "Efficiency: %.3f %%", BioAssayI131Efficiency());
			else
				insertconst3(strng, 40, 0, 0, "Efficiency: N/A");
			pr_write3_language(strng, "DIAGNOSTICS");


			// Cross Contamination Factor
			insertfloat3(strng, 3, 0, 0, "Cross Contamination Factor: %.5f", BioAssayI131I125Contamination());

			// Trigger Level
			if(current.system == CI) format_activity_system2(BioAssayI131Threshold(), buff);
			else format_activity_system_kbq(BioAssayI131Threshold(), buff);
			replace(buff, '$', MU_PRT);
			insertstring(strng, 40, 0, 0, "Trigger: %s", buff);
			pr_write3_language(strng, "DIAGNOSTICS");
			feed2(1,prtype);


			insertconst3(strng, 3, 0, 0, "@4I125 BioAssay Parameters@8");
			pr_write3_language(strng, "DIAGNOSTICS");
			

			// Used in BioAssay
			if(BioAssayI125Active()){
				insertconst3(strng, 3, 0, 0, "Used in BioAssay");
			}else{
				insertconst3(strng, 3, 0, 0, "Not Used in BioAssay");
			}
			pr_write3_language(strng, "DIAGNOSTICS");

			// ROI
			sprintf(buff, "ROI(keV): %.1f - %.1f", I125Lower_keV, I125Upper_keV);
			insertconst3(strng, 3, 0, 0, buff);

			// Efficiency
			if(BioAssayI125Efficiency() > 0)
				insertfloat3(strng, 40, 0, 0, "Efficiency: %.3f %%", BioAssayI125Efficiency());
			else
				insertconst3(strng, 40, 0, 0, "Efficiency: N/A");
			
			pr_write3_language(strng, "DIAGNOSTICS");

			// Trigger Level
			if(current.system == CI) format_activity_system2(BioAssayI125Threshold(), buff);
			else format_activity_system_kbq(BioAssayI125Threshold(), buff);
			replace(buff, '$', MU_PRT);
			insertstring3(strng, 40, 0, 0, "Trigger: %s", buff);
			pr_write3_language(strng, "DIAGNOSTICS");
			feed2(1,prtype);

			insertconst3(strng, 3, 0, 0, "@4I123 BioAssay Parameters@8");
			pr_write3_language(strng, "DIAGNOSTICS");

			// Used in BioAssay
			if(BioAssayI123Active()){
				insertconst3(strng, 3, 0, 0, "Used in BioAssay");
			}else{
				insertconst3(strng, 3, 0, 0, "Not Used in BioAssay");
			}
			pr_write3_language(strng, "DIAGNOSTICS");

			// ROI
			sprintf(buff, "ROI(keV): %.1f - %.1f", I123Lower_keV, I123Upper_keV);
			insertconst3(strng, 3, 0, 0, buff);

			// Efficiency
			if(BioAssayI123Efficiency() > 0)
				insertfloat3(strng, 40, 0, 0, "Efficiency: %.3f %%", BioAssayI123Efficiency());
			else
				insertconst3(strng, 40, 0, 0, "Efficiency: N/A");

			pr_write3_language(strng, "DIAGNOSTICS");

			// Cross Contamination Factor
			insertfloat3(strng, 3, 0, 0, "Cross Contamination Factor: %.5f", BioAssayI123I125Contamination());
			

			// Trigger Level
			if(current.system == CI) format_activity_system2(BioAssayI123Threshold(), buff);
			else format_activity_system_kbq(BioAssayI123Threshold(), buff);
			replace(buff, '$', MU_PRT);
			insertstring3(strng,40, 0, 0, "Trigger: %s", buff);
			pr_write3_language(strng, "DIAGNOSTICS");
			feed2(1,prtype);


			//Thyroid Uptake Normal Values
			if((ThyroidUptake_normal[0].hour != -1) || (ThyroidUptake_normal[1].hour != -1) || (ThyroidUptake_normal[2].hour != -1) || (ThyroidUptake_normal[3].hour != -1)){
				insertconst3(strng, 0, 0, 0, "@1Thyroid Uptake Normal Range@8");
				pr_write3_language(strng, "DIAGNOSTICS");

				insertconst3(strng, 2, 0, 0, "@4 Hour      Min %      Max %@8");
				pr_write3_language(strng, "DIAGNOSTICS");
				for(index=0; index<4; index++){
					if(ThyroidUptake_normal[index].hour != -1){
						insertint3(strng, 3, 1, 0, "%d",  ThyroidUptake_normal[index].hour);
						insertint3(strng, 13, 1, 0, "%d",  ThyroidUptake_normal[index].min);
						insertint3(strng, 24, 1, 0, "%d",  ThyroidUptake_normal[index].max);
						pr_write3_language(strng, "DIAGNOSTICS");
					}
				}
			}
		}
		
		if((RBCSurvival_minNormal > 0) && (RBCSurvival_maxNormal > 0)){
			feed2(1,prtype);
			insertconst3(strng, 0, 0, 0, "@1RBC Survival Normal Range@8");
			pr_write3_language(strng, "DIAGNOSTICS");

			sprintf(buff, "Normal Range: %.1f - %.1f Days", RBCSurvival_minNormal, RBCSurvival_maxNormal);
			insertconst3(strng, 5, 0, 0, buff);
			pr_write3_language(strng, "DIAGNOSTICS");
		}
	}

    static bool pr_test_sources(void)
    {
        short i;
        short i0;
        STAND stnd;
        char strng[90];
        char sstr[10];
        bool constsource;
        char constsourceindex;
        short constancyChMirror[12];
        bool found;
        char message[52];
        NUCDATA output_nuc;

        EE_READ(constancysource, (uchar *) &constsourceindex);
        pr_test_source_header();

        lininit(strng, TRUE, prtype);
        //strncpy(&(strng[2]), "Chamber", 7);
		get_amulet_message(L_CHAMBER,message);    // "Chamber"
		strncpy(&strng[2],message,strlen(message));
        pr_write(strng);
        ++line;

		for(i = 0; i < 6; i++)
		{
            if(i == constsourceindex - 1) constsource = TRUE;
            else constsource = FALSE;
            
            lininit(strng,TRUE,prtype);
                
			i0 = 2;
			if (prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
				i0 = 6;
            get_stndstr(i,sstr);   
            strncpy(&strng[i0],sstr,strlen(sstr));
            if(i < 5) EE_READ(stand[i],(uchar *)&stnd);
            else EE_READ(stand_ge68, (uchar *)&stnd);
			if (stnd.caldate != (time_t)0)
			{
				if(!pr_stand(strng,&stnd,sstr, constsource))
					return(FALSE);
			}
			else
			{
				i0 = 12;
				if (prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
					i0 = 30;
				//strncpy(&strng[i0],"No Source",9);
                //get_message(DIAG_33,&strng[i0]);
				get_amulet_message(L_PR_NO_SOURCE,message);    // "No Source"
				strncpy(&strng[i0],message,strlen(message));
			}
            pr_write(strng);
            ++line;
			feed(1,prtype);
			++line;
		}
        lininit(strng,TRUE,prtype);
        
		//strncpy(&strng[2],"D: Daily  C: Constancy Source",29);
        //get_message(DIAG_34,&strng[2]);
		get_amulet_message(L_PR_DAILY_CONSTANCY,message);    // "D: Daily  C: Constancy Source"
		strncpy(&strng[1],message,strlen(message));

        pr_write(strng);
        ++line;
		feed(1,prtype);
		++line;

		EE_READ(constancych, (uchar *) constancyChMirror);
		found = FALSE;
		for(i=0; i<12; i++){
			if(constancyChMirror[i]>=0){
				found = TRUE;
				break;
			}
		}

		if(found){
			if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
				bold(TRUE);
			lininit(strng,TRUE,prtype);
			//strncpy(&strng[2], "Constancy Channels:", 19);
			get_amulet_message(L_PR_CONSTANCY_CHANNELS,message);    // "Constancy Channels:"
			strncpy(&strng[2],message,strlen(message));
			if(prtype == USB_EPS_PRINTER)
				eps_bold(0.5,strng);
			else
				pr_write(strng);
			++line;
			feed(1, prtype);
			++line;

			if(prtype == OKI_PRINTER || prtype < 0 || prtype ==  LX_PRINTER)
				bold(FALSE);

			for(i=0; i<12; i++){
				if(constancyChMirror[i]>=0){
					lininit(strng, TRUE, prtype);
					GetShortNuclideInfo(constancyChMirror[i], &output_nuc, message);
					strncpy(&strng[2], message, strlen(message));
					pr_write(strng);
					++line;
					feed(1, prtype);
					++line;
				}
			}
		}

		if(Mca_installedDetector == DET_WELL){
			lininit(strng, TRUE, prtype);
			strncpy(&(strng[2]), "Well Counter", 12);
			pr_write(strng);
			++line;

			lininit(strng,TRUE,prtype);

			i0 = 2;
			if (prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
				i0 = 6;
            get_stndstr(3,sstr);
            strncpy(&strng[i0], sstr, strlen(sstr));
            DB_PopDetectorStandard(&stnd);
            stnd.daily = FALSE;
            if (stnd.caldate != (time_t)0)
			{
				if(!pr_stand(strng, &stnd, sstr, FALSE))
					return(FALSE);
			}
			else
			{
				i0 = 12;
				if (prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER)
					i0 = 30;
				//strncpy(&strng[i0],"No Source",9);
                get_message(DIAG_33,&strng[i0]);
			}
            pr_write(strng);
            ++line;
            feed(1, prtype);
            ++line;
		}

        return TRUE;

    }



    static void pr_test_source_header(void)
    {

        char strng[90];
		char message[50];
        
        if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
            bold(TRUE);
        lininit(strng,TRUE,prtype);
        //strncpy(&strng[2],"TEST SOURCE :",13);
        //get_message(DIAG_48,&strng[2]);
        //strng[13] = 'S';
		get_amulet_message(L_PR_TEST_SOURCES,message);    // "TEST SOURCES:"
		strncpy(&strng[2],message,strlen(message));
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        ++line;

        if(prtype == OKI_PRINTER || prtype < 0 || prtype ==  LX_PRINTER)
        {
            bold(FALSE);
            lininit(strng,TRUE,prtype);
            
     //"                                  CALIBRATION             CURRENT\r\n"
            //get_message(DIAG_49,&strng[34]);
			get_amulet_message(L_PR_CALIBRATION,message);    // "CALIBRATION"
			strncpy(&strng[30],message,strlen(message));
			strncpy(&strng[44],message,strlen(message));
            //get_message(DIAG_50,&strng[58]);
			get_amulet_message(L_PR_CURRENT,message);    // "CURRENT"
			strncpy(&strng[58],message,strlen(message));
			
            pr_write(strng);
            ++line;
            lininit(strng,TRUE,prtype);
            //get_message(DIAG_51,&strng[6]);
			get_amulet_message(L_PR_NUCLIDE,message);    // "NUCLIDE"
			strncpy(&strng[6],message,strlen(message));
            get_message(DIAG_52,&strng[16]);	//S/N
            //get_message(DIAG_53,&strng[30]);
			get_amulet_message(L_SETUP_SOURCES_DATE,message);    // "DATE"
			strncpy(&strng[30],message,strlen(message));
            //get_message(DIAG_54,&strng[44]);
			get_amulet_message(L_SETUP_SOURCES_ACTIVITY,message);    // "ACTIVITY"
			strncpy(&strng[44],message,strlen(message));
            //get_message(DIAG_54,&strng[58]);
			strncpy(&strng[58],message,strlen(message));
            pr_write(strng);
            ++line;
            feed(1,prtype);
            ++line;
            
  //("      NUCLIDE   S/N           DATE          ACTIVITY      ACTIVITY\r\n\n"
        }
    }   

static void print_nuc_data_700(void){
	short ii;
	bool flgUserNuc;
	char strng[90];

	pr_table_head_700();


	if(prtype == SLIP_PRINTER) line = 18;

	for(ii=0; ii<BUILTINNUC - 4; ii++){
		service_pc();

		if(prtype != ROLL_PRINTER && prtype != SLIP_PRINTER){
			if(line >= 52){
				formfeed(prtype);
/*				if(prtype == SLIP_PRINTER){
 - 					change_paper();
 - 					if(home_set())
 - 						return;
 - 				}*/
				prhead_languagesec(prtype);
				line = 3;
				pr_table_head_700();
			}
		}
		pr_nuclide_data_700(SortedBuiltInIndex[ii]);	// New
		if(prtype < 0 && bytes_remaining() != 0) return;
	}

	flgUserNuc = FALSE;
	for(ii = USERNUC + 10; ii < USERNUC + 20; ii++){
		if(NuclideData_getHalflifeUnit(ii) != -1 && (NuclideData_getEff(ii, DET_WELL700) > 0.0 || NuclideData_getEff(ii, DET_PROBE700) > 0.0)){
			flgUserNuc = TRUE;
			break;
		}
	}

	if(flgUserNuc){
		feed(2,prtype);
		line+=2;

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
		lininit(strng,TRUE,prtype);
		//USER NUCLIDES:
		get_message(USER_NUCLIDES,&strng[4]);
		if(prtype == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);
		++line;
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);

		for(ii = USERNUC + 10; ii < USERNUC + 20; ii++){
			pr_nuclide_data_700(ii); // New
		}
	}
}

//for USB HPonly
static void print_nuc_data_700_hp(void){
	short ii;
	bool flgUserNuc;
	char prtype = USB_PRINTER;
	
	pr_table_head_700_hp(1);




	for(ii=0; ii<BUILTINNUC - 4; ii++){
		service_pc();

		if(ii == 48)
		{	
			print_page_num();
			pr_set_linecnt(3);
			rawheader_languagesec(prtype, "",  clock_time);
			line = 3;
			pr_table_head_700_hp(1);
		}

		pr_nuclide_data_700_hp(SortedBuiltInIndex[ii]);	
		if(prtype < 0 && bytes_remaining() != 0) return;
	}
	print_page_num();

	flgUserNuc = FALSE;
	for(ii = USERNUC + 10; ii < USERNUC + 20; ii++){
		if(NuclideData_getHalflifeUnit(ii) != -1 && (NuclideData_getEff(ii, DET_WELL700) > 0.0 || NuclideData_getEff(ii, DET_PROBE700) > 0.0)){
			flgUserNuc = TRUE;
			break;
		}
	}

	if(flgUserNuc)
	{	

		pr_set_linecnt(3);
		rawheader_languagesec(prtype, "",  clock_time);
		pr_table_head_700_hp(2);
		
		++line;

		for(ii = USERNUC + 10; ii < USERNUC + 20; ii++){
			pr_nuclide_data_700_hp(ii); 
		}
		print_page_num();
	}
}

static void print_nuc_data_well(void){
	short ii;
	bool flgUserNuc;
	char strng[90];

	pr_table_head_well();


	if(prtype == SLIP_PRINTER) line = 18;

	for(ii=0; ii<BUILTINNUC - 4; ii++){
		service_pc();
            
		if(prtype != ROLL_PRINTER && prtype != SLIP_PRINTER){
			if(line >= 52){
				formfeed(prtype);
/*				if(prtype == SLIP_PRINTER){
 - 					change_paper();
 - 					if(home_set())
 - 						return;
 - 				}*/
				prhead_languagesec(prtype);
				line = 3;
				pr_table_head_well();
			}
		}
		pr_nuclide_data_well(SortedBuiltInIndex[ii]);
		if(prtype < 0 && bytes_remaining() != 0) return;
	}

	flgUserNuc = FALSE;
	for(ii = USERNUC + 10; ii < USERNUC + 20; ii++){
		if(NuclideData_getHalflifeUnit(ii) != -1 && NuclideData_getEff(ii, DET_WELL) > 0){
			flgUserNuc = TRUE;
			break;
		}
	}

	if(flgUserNuc){
		feed(2,prtype);
		line+=2;

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
		lininit(strng,TRUE,prtype);
		//USER NUCLIDES:
		get_message(USER_NUCLIDES,&strng[4]);
		if(prtype == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);
		++line;
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);

		for(ii = USERNUC + 10; ii < USERNUC + 20; ii++){
			pr_nuclide_data_well(ii);
		}
	}
}

//for USB HP printer only
static void print_nuc_data_well_hp(void){
	short ii;
	bool flgUserNuc;

	pr_table_head_well_hp(1);



	for(ii=0; ii<BUILTINNUC - 4; ii++){
		service_pc();
		if(ii == 48)
		{	
			print_page_num();
			pr_set_linecnt(3);
			rawheader_languagesec(prtype, "",  clock_time);
			line = 3;
			pr_table_head_well_hp(1);
		}
		
            
		pr_nuclide_data_well_hp(SortedBuiltInIndex[ii]);
		if(prtype < 0 && bytes_remaining() != 0) return;
	}

	print_page_num();

	flgUserNuc = FALSE;
	for(ii = USERNUC + 10; ii < USERNUC + 20; ii++){
		if(NuclideData_getHalflifeUnit(ii) != -1 && NuclideData_getEff(ii, DET_WELL) > 0){
			flgUserNuc = TRUE;
			break;
		}
	}

	if(flgUserNuc){

		pr_set_linecnt(3);
		rawheader_languagesec(prtype, "",  clock_time);
		pr_table_head_well_hp(2);
		
		//USER NUCLIDES:

		for(ii = USERNUC + 10; ii < USERNUC + 20; ii++){
			pr_nuclide_data_well(ii);
		}
		print_page_num();
	}
}

    static void print_nuc_data(void)
    {
        short ii;
        char strng[90];
        bool flgUserChamberNucFound, flgUserWellNucFound, flgUser700NucFound;
		char message[50];

        pr_table_head();


        if(prtype == SLIP_PRINTER)
            line = 18;

        for(ii = 0; ii < BUILTINNUC; ii++)
        {
            service_pc();

            if((NuclideData_getEffectiveResponse(SortedBuiltInIndex[ii], R_CHAMB) != 0) || (NuclideData_getEffectiveResponse(SortedBuiltInIndex[ii], P_CHAMB))){
				if(prtype != ROLL_PRINTER && prtype != SLIP_PRINTER)
				{
					if(line >= 52)
					{
						formfeed(prtype);
/*						if(prtype == SLIP_PRINTER)
 - 						{
 - 							change_paper();
 - 							if(home_set())
 - 								return;
 - 						}*/
						prhead_language(prtype);
						line = 3;
						pr_table_head();
					}
				}
				pr_nuclide_data(SortedBuiltInIndex[ii]);
				if(prtype < 0 && bytes_remaining() != 0) return;
            }
        }

        //User Nuclides
        if(prtype == ROLL_PRINTER || prtype == SLIP_PRINTER){
            feed(1,prtype);
			++line;
        }else
        {
            formfeed(prtype);
/*            if(prtype == SLIP_PRINTER)
 -             {
 -                 change_paper();
 -                 if(home_set())
 -                     return;
 -             }*/
            prhead_language(prtype);
            line = 3;
            //pr_table_head();
        }

        flgUserChamberNucFound = FALSE;
        for(ii=USERNUC; ii<USERNUC + 10; ii++){
        	if(NuclideData_getHalflifeUnit(ii) != -1){
        		flgUserChamberNucFound = TRUE;
        		break;
        	}
        }
        if(flgUserChamberNucFound){
			if(prtype != ROLL_PRINTER) pr_table_head();
			if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
			lininit(strng,TRUE,prtype);
			//USER NUCLIDES:
			//get_message(USER_NUCLIDES,&strng[4]);
			//strncpy(&strng[4], "USER CHAMBER NUCLIDES:", 22);
			get_amulet_message(L_PR_USER_CHAMBER_NUCLIDES,message);    // "USER CHAMBER NUCLIDES:"
			strncpy(&strng[4],message,strlen(message));
			if(prtype == USB_EPS_PRINTER)
				eps_bold(0.5,strng);
			else
				pr_write(strng);
			++line;
			if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);

			for(ii = 0; ii < 10; ii++) pr_nuclide_data(USERNUC + ii);
        }

        if(Mca_installedDetector == DET_WELL){
			flgUserWellNucFound = FALSE;
			for(ii=USERNUC+10; ii<USERNUC+20; ii++){
				if(NuclideData_getHalflifeUnit(ii) != -1 && NuclideData_getEff(ii, DET_WELL) > 0.0){
					flgUserWellNucFound = TRUE;
					break;
				}
			}

			if(flgUserWellNucFound){
				pr_table_head_well();

				if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
				lininit(strng,TRUE,prtype);
				//USER NUCLIDES:
				//get_message(USER_NUCLIDES,&strng[4]);
				strncpy(&strng[4], "USER WELL NUCLIDES:", 19);
				if(prtype == USB_EPS_PRINTER)
					eps_bold(0.5,strng);
				else
					pr_write(strng);
				++line;
				if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);

				for(ii=USERNUC+10; ii<USERNUC+20; ii++){
					pr_nuclide_data_well(ii);
				}
			}
        }else if(Mca_installedDetector == DET_BETA){
        }else if(Mca_installedDetector == DET_WELL700 || Mca_installedDetector == DET_PROBE700 || Mca_installedDetector == DET_DRILLEDPROBE700){
        	flgUser700NucFound = FALSE;
        	for(ii=USERNUC+10; ii<USERNUC+20; ii++){
        		if(NuclideData_getHalflifeUnit(ii) != -1 && (NuclideData_getEff(ii, DET_WELL700) > 0.0 || NuclideData_getEff(ii, DET_PROBE700) > 0.0)){
        			flgUser700NucFound = TRUE;
        			break;
        		}
        	}

        	if(flgUser700NucFound){
        		pr_table_head_700();

        		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
				lininit(strng,TRUE,prtype);

				strncpy(&strng[4], "USER DETECTOR NUCLIDES:", 23);
				if(prtype == USB_EPS_PRINTER)
					eps_bold(0.5,strng);
				else
					pr_write(strng);
				++line;
				if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);

				for(ii=USERNUC+10; ii<USERNUC+20; ii++){
					pr_nuclide_data_700(ii);
				}
        	}
        }
    }   


	//USB HP printer only
	static void print_nuc_data_hp(bool well_exists, bool probe_exists)
    {
        short ii, printed_index;
        bool flgUserChamberNucFound, flgUserWellNucFound, flgUser700NucFound;
        char flgPrint;

        pr_table_head_hp(1,well_exists,probe_exists);


		printed_index = 0;
        for(ii = 0; ii < BUILTINNUC; ii++)
        {
            service_pc();
			if(printed_index == 48)
			{	
				print_page_num();
				pr_set_linecnt(3);
				rawheader_languagesec(prtype, "",  clock_time);
				line = 3;
				pr_table_head_hp(1,well_exists,probe_exists);
			}
			
			flgPrint = 0;
            if(chamber_77t()){
            	if(NuclideData_getEffectiveResponse(SortedBuiltInIndex[ii], C_CHAMB)) flgPrint = 1;
            }else{
            	if((NuclideData_getEffectiveResponse(SortedBuiltInIndex[ii], R_CHAMB) != 0) || (NuclideData_getEffectiveResponse(SortedBuiltInIndex[ii], P_CHAMB))) flgPrint = 1;
            }
            if(flgPrint){
            	printed_index++;
				pr_nuclide_data_hp(SortedBuiltInIndex[ii],well_exists,probe_exists);
				if(prtype < 0 && bytes_remaining() != 0) return;
            }
        }

		print_page_num();


        //User Nuclides
		flgUserChamberNucFound = FALSE;
		flgUserWellNucFound = FALSE;
       	flgUser700NucFound = FALSE;

		//chamber nuclides
        for(ii=USERNUC; ii<USERNUC + 10; ii++){
        	if(NuclideData_getHalflifeUnit(ii) != -1){
        		flgUserChamberNucFound = TRUE;
        		break;
        	}
        }
        if(flgUserChamberNucFound)
		{
			pr_set_linecnt(3);
			rawheader_languagesec(prtype, "",  clock_time);
			line = 3;
			pr_table_head_hp(2,FALSE,FALSE);


			for(ii = 0; ii < 10; ii++)
				pr_nuclide_data_hp(USERNUC + ii,FALSE,FALSE);
			feed(2,prtype);
        }

		//WellNuclides
        if(Mca_installedDetector == DET_WELL){
			for(ii=USERNUC+10; ii<USERNUC+20; ii++){
				if(NuclideData_getHalflifeUnit(ii) != -1 && NuclideData_getEff(ii, DET_WELL) > 0.0){
					flgUserWellNucFound = TRUE;
					break;
				}
			}

			if(flgUserWellNucFound)
			{
				if(!flgUserChamberNucFound)
				{
					rawheader_languagesec(prtype, "",  clock_time);
					line = 3;
				}	
					
				pr_table_head_well_hp(2);


				for(ii=USERNUC+10; ii<USERNUC+20; ii++){
					pr_nuclide_data_well_hp(ii);
					feed(2,prtype);	
				}
			}
        }else if(Mca_installedDetector == DET_WELL700 || Mca_installedDetector == DET_PROBE700 || Mca_installedDetector == DET_DRILLEDPROBE700){
        	for(ii=USERNUC+10; ii<USERNUC+20; ii++){
        		if(NuclideData_getHalflifeUnit(ii) != -1 && (NuclideData_getEff(ii, DET_WELL700) > 0.0 || NuclideData_getEff(ii, DET_PROBE700) > 0.0)){
        			flgUser700NucFound = TRUE;
        			break;
        		}
        	}

        	if(flgUser700NucFound)
			{
				if(!flgUserChamberNucFound && !flgUserWellNucFound)
				{
					rawheader_languagesec(prtype, "",  clock_time);
					line = 3;
				}	
        		pr_table_head_700_hp(2);


				for(ii=USERNUC+10; ii<USERNUC+20; ii++){
					pr_nuclide_data_700_hp(ii);
				}
        	}
        }
		if(flgUserChamberNucFound || flgUserWellNucFound || flgUser700NucFound)
			print_page_num();
    }

    void change_paper(void)
    {

        read_screen(0);
        erase_screen();
        //display_text(10,20,"Change Paper",0,MEDIUM,NORMAL);
        display_medium_message(DIAG_30,20,0,NORMAL);
        contmsg();
        erase_screen();
        write_screen(0);


    }   

    static void pr_system_test(bool test_ok)
    {
        char strng[90];
		char message[50];

		if(prtype == USB_PRINTER)
			pcl_bold(TRUE);

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[0],"PROGRAM INTEGRITY:",18);
		get_amulet_message(L_PROGRAM_INTEGRITY,message);    // "PROGRAM INTEGRITY"
		strcat(message,": ");
		strcpy(strng,message);
			

        if(test_ok)
            //strncpy(&strng[19],"PASS",4);
			get_amulet_message(L_PASS2,message);    // "PASS"
        else
            //strncpy(&strng[19],"FAIL",4);
			get_amulet_message(L_FAIL2,message);    // "FAIL"

		strcat(strng,message);
		strcat(strng,"\r\n");

        pr_write(strng);
        ++line;
		if(prtype == USB_PRINTER)
			pcl_bold(FALSE);
    }    

static void pr_SD_status(void){
	char strng[90], buf[90];
	unsigned long long int totalsize, freesize;
	unsigned long int fre_clust;
	FATFS *fs;

	f_getfree("0:", &fre_clust, &fs);
	totalsize = fs->n_fatent - 2;
	totalsize *= fs->csize;
	totalsize /= 2;

	freesize = fre_clust;
	freesize *= fs->csize;
	freesize /= 2;

	if(prtype == USB_PRINTER)
		pcl_bold(TRUE);

	lininit(strng, TRUE, prtype);
	//insertconst(strng, 0, 0, 0, "MEMORY STATUS");
	get_amulet_message(L_PR_MEMORY_STATUS,buf);    // "MEMORY STATUS"
	insertconst(strng, 0, 0, 0,buf);
	pr_write(strng);
	++line;

	if(prtype == USB_PRINTER)
		pcl_bold(FALSE);

	lininit(strng, TRUE, prtype);
	//insertconst(strng, 9, 1, 0, "Available:");
	get_amulet_message(L_PR_AVAILABLE,buf);    // "Available:"
	insertconst(strng, 10, 1, 0, buf);
	insertconst(strng, 33, 1, 0, "kB");
	sprintf(buf, "%llu", freesize);
	insertconst(strng, 30, 1, 0, buf);
	pr_write(strng);
	++line;

	lininit(strng, TRUE, prtype);
	//insertconst(strng, 9, 1, 0, "Total:");
	get_amulet_message(L_PR_TOTAL,buf);    // "Total:"
	insertconst(strng, 10, 1, 0, buf);
	insertconst(strng, 33, 1, 0, "kB");
	sprintf(buf, "%llu", totalsize);
	insertconst(strng, 30, 1, 0, buf);
	pr_write(strng);
	++line;
}

static void eps_div(float xpos)
    {    
            pr_write("\r");
            eps_horiz(xpos);
            pr_write(":");
            pr_write("\r");
            eps_horiz(xpos);
            pr_write("-");
            pr_write("\r");
            eps_horiz(0.5);
    }

void print_page_num(void)
{
	char strng[200];
	char formatstring[25];

	lininit3(strng,TRUE,current.printer);
	pcl_vertcial("2900");
	++diag_page_num;
	get_amulet_message(L_PAGE, formatstring);    // "Page %d"
	//insertint3(strng,60,0,0,"Page %d",diag_page_num);
	insertint3(strng,58,0,0,formatstring,diag_page_num);
	pr_write3_language(strng,"DIAGNOSTICS");
	formfeed(current.printer);


}

    static void print_nuc_datasec(void)
    {
        short ii;
        char strng[90];
        bool flgUserChamberNucFound, flgUserWellNucFound, flgUser700NucFound;
		char message[50];

        pr_table_head();


        if(prtype == SLIP_PRINTER)
            line = 18;

        for(ii = 0; ii < BUILTINNUC; ii++)
        {
            service_pc();

            if((NuclideData_getEffectiveResponse(SortedBuiltInIndex[ii], R_CHAMB) != 0) || (NuclideData_getEffectiveResponse(SortedBuiltInIndex[ii], P_CHAMB))){
				if(prtype != ROLL_PRINTER && prtype != SLIP_PRINTER)
				{
					if(line >= 52)
					{
						formfeed(prtype);
/*						if(prtype == SLIP_PRINTER)
 - 						{
 - 							change_paper();
 - 							if(home_set())
 - 								return;
 - 						}*/
						prhead_languagesec(prtype);
						line = 3;
						pr_table_head();
					}
				}
				pr_nuclide_data(SortedBuiltInIndex[ii]);
				if(prtype < 0 && bytes_remaining() != 0) return;
            }
        }

        //User Nuclides
        if(prtype == ROLL_PRINTER || prtype == SLIP_PRINTER){
            feed(1,prtype);
			++line;
        }else
        {
            formfeed(prtype);
/*            if(prtype == SLIP_PRINTER)
 -             {
 -                 change_paper();
 -                 if(home_set())
 -                     return;
 -             }*/
            prhead_languagesec(prtype);
            line = 3;
            //pr_table_head();
        }

        flgUserChamberNucFound = FALSE;
        for(ii=USERNUC; ii<USERNUC + 10; ii++){
        	if(NuclideData_getHalflifeUnit(ii) != -1){
        		flgUserChamberNucFound = TRUE;
        		break;
        	}
        }
        if(flgUserChamberNucFound){
			if(prtype != ROLL_PRINTER) pr_table_head();
			if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
			lininit(strng,TRUE,prtype);
			//USER NUCLIDES:
			//get_message(USER_NUCLIDES,&strng[4]);
			//strncpy(&strng[4], "USER CHAMBER NUCLIDES:", 22);
			get_amulet_message(L_PR_USER_CHAMBER_NUCLIDES,message);    // "USER CHAMBER NUCLIDES:"
			strncpy(&strng[4],message,strlen(message));
			if(prtype == USB_EPS_PRINTER)
				eps_bold(0.5,strng);
			else
				pr_write(strng);
			++line;
			if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);

			for(ii = 0; ii < 10; ii++) pr_nuclide_data(USERNUC + ii);
        }

        if(Mca_installedDetector == DET_WELL){
			flgUserWellNucFound = FALSE;
			for(ii=USERNUC+10; ii<USERNUC+20; ii++){
				if(NuclideData_getHalflifeUnit(ii) != -1 && NuclideData_getEff(ii, DET_WELL) > 0.0){
					flgUserWellNucFound = TRUE;
					break;
				}
			}

			if(flgUserWellNucFound){
				pr_table_head_well();

				if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
				lininit(strng,TRUE,prtype);
				//USER NUCLIDES:
				//get_message(USER_NUCLIDES,&strng[4]);
				strncpy(&strng[4], "USER WELL NUCLIDES:", 19);
				if(prtype == USB_EPS_PRINTER)
					eps_bold(0.5,strng);
				else
					pr_write(strng);
				++line;
				if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);

				for(ii=USERNUC+10; ii<USERNUC+20; ii++){
					pr_nuclide_data_well(ii);
				}
			}
        }else if(Mca_installedDetector == DET_BETA){
        }else if(Mca_installedDetector == DET_WELL700 || Mca_installedDetector == DET_PROBE700 || Mca_installedDetector == DET_DRILLEDPROBE700){
        	flgUser700NucFound = FALSE;
        	for(ii=USERNUC+10; ii<USERNUC+20; ii++){
        		if(NuclideData_getHalflifeUnit(ii) != -1 && (NuclideData_getEff(ii, DET_WELL700) > 0.0 || NuclideData_getEff(ii, DET_PROBE700) > 0.0)){
        			flgUser700NucFound = TRUE;
        			break;
        		}
        	}

        	if(flgUser700NucFound){
        		pr_table_head_700();

        		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
				lininit(strng,TRUE,prtype);

				strncpy(&strng[4], "USER DETECTOR NUCLIDES:", 23);
				if(prtype == USB_EPS_PRINTER)
					eps_bold(0.5,strng);
				else
					pr_write(strng);
				++line;
				if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);

				for(ii=USERNUC+10; ii<USERNUC+20; ii++){
					pr_nuclide_data_700(ii);
				}
        	}
        }
    }
