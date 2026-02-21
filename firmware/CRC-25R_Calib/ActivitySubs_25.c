/*********************************************************************
  MODULE:	CALIBRATOR ACTIVITY SUBROUTINES   FOR crc-25

  FILE:		ActivitySubs_25.c

  DATE:		10/04/06
            07/11/07 -- fixed bugs in upbut and nucinit for PET
            03/27/08  --removed use of mindec since it caused bad readings
            also removed lowresp so that all Cal#s sit at 0.00mCi (R-chamber) for background
            09/03/08 -- nucset:  always call nucinit

  ANALYSIS:		subroutines for activity measurement

  ROUTINES:
		downbut:    services DOWN key
        get_new_cal:    gets new calibration number
        get_lincor: returns linearity correction
        getiso:     user presses built-in isotope key
		getuser:	gets data when Ukey pressed
        lowresp:    low response correction
        mindec:     minimum decimal places
		nucinit:	initialization for nuclide
		nucset:		setup for new nuclide
        readvals:   reads in values from eeprom
        respcor:    response correction
		sysbut:		changes between Ci & Bq when Ci/Bq key pressed
		upbut:		services UP key
		user_key_index:	reads in user key data from eeprom


  *************************************************************************/
#include "crc.h"
#include "i2c.h"
#include "screen.h"
#include "keyboard.h"
#include "chambfac.h"
#include "nuc.h"
#include <string.h>


	extern CHAMBER chamber[];
    extern REMOTE remote[];
    extern MEASUREMENT measurement[];
	extern CURRENT current;

static void dodown(short ch_num, short ch_type);



	//user presses one of the U keys
    bool getuser(short ch_num, short but)
	{

		short nuc_index;


		nuc_index = user_key_index(but);
		if (nuc_index == -1)
		{
			beep();
			return(FALSE);
		}
        set_nuclide_data(nuc_index,ch_num);
		return(TRUE);
	}



    //minimum decimal places
    void mindec(short ch_num)
	{
		float response;

		short ch_type;

		ch_type = chamber_type(ch_num);
		response = measurement[ch_num].response;


        if(response >= .1)
			return;

		if (ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB)
            measurement[ch_num].idecmin = 1;
        else
            measurement[ch_num].idecmin = 0;


		if (response < .02)
        {
            if (ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB)
                measurement[ch_num].idecmin = 2;
            else
                measurement[ch_num].idecmin = 1;
        }

        if (ch_type == R_CHAMB)
        {
            if (response < 0.002)
                measurement[ch_num].idecmin = 2;
        }

        measurement[ch_num].idec = measurement[ch_num].idecmin;
	}



 //low response correction
    void lowresp(short ch_num)
	{


    	short ch_type;

    	ch_type = chamber_type(ch_num);
		if (measurement[ch_num].dogain == 0 && measurement[ch_num].response < 0.09)
		{
			measurement[ch_num].autodu = FALSE;
			dodown(ch_num,ch_type);
		}
	}



 //get user key index
    short user_key_index(short but)
	{
        short index;


        EE_READ(user_keys[0].keys[but],(uchar *)&index);
		return(index);
	}




    extern USERCAL user_cal[];
    //get new calibratiion number
    // return True is there is new cal num, else return False
	bool get_new_cal(float *response, short nuc_index,short ch_type)
	{
		short i;
       USERCAL ucal;

		for (i = 0; i < MAX_NEW_CAL_OBSOLETE; i++)
		{
            //EE_READ(usercal[i],(uchar *)&ucal);
            ucal = user_cal[i];
			if ((ucal.nuc_index == nuc_index))
			{
                if (ucal.response == 0.)
                    return FALSE;
                *response = ucal.response;
				return TRUE;
			}
		}
		return FALSE;
	}



    //nuc init
    void nucinit(short ch, bool mode)
	{
        if (mode)
		{
        	// Removed:
            //measurement[ch].syst0 = -1;
            measurement[ch].kun0 = 0;
            if(ch == current.main_chamber && get_meas_screen())
            {
                erase_lines(16,48,0);
                erase_lines(Y_FUT_TIME,64 - Y_FUT_TIME,1);
                id_init();
                measurement[ch].future.dosetime = NO_TIME;
            }
		}

        measurement[ch].idecmin = 0;
		measurement[ch].idec = measurement[ch].idecmin;
		measurement[ch].act0 = 0.;
		strcpy(&measurement[ch].actstr0[0],"xxxxxx");
        measurement[ch].valid_flag = FALSE;
        measurement[ch].display_flag = FALSE;
        remote[ch].valid_flag = FALSE;
        //for 18bit
            setchamber(ch,0);      //most sensitive
        measurement[ch].dogain = 2;
            measurement[ch].gain = 0;
        measurement[ch].mode = MEASMODE;
		measurement[ch].changed = 4;
		measurement[ch].wasover = FALSE;
		measurement[ch].doavg = FALSE;
		measurement[ch].longavg = FALSE;
		measurement[ch].autodu = FALSE;
	}


    /*UP arrow pressed
    does slash if entering time
     else changes sensitivity */

    void upbut(void)
	{
        short ch_num = current.main_chamber;


        short ch_type;

        ch_type = chamber_type(ch_num);
		if (input_time_set())
		{
            dokey(10);
			return;
		}
		if (measurement[ch_num].longavg)
		{
			measurement[ch_num].longavg = FALSE;
			measurement[ch_num].nmeas = 0;
			return;
		}


		switch(measurement[ch_num].dogain)
		{

        case 0:
            if(ch_type == R_CHAMB)
            {
                beep();
                return;
            }
            break;
        case 1:
            if(ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB)
                beep();
            else
            {
                measurement[ch_num].dogain = 0;
                nucinit(ch_num,FALSE);
            }
			break;

        case 2:
            if(ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB)
                nucinit(ch_num,FALSE);	/* sets dogain to 1 */
            else
            {
                measurement[ch_num].dogain = 1;
                // Locked: CI
                //measurement[ch_num].idecmin = get_decmin_1(ch_type,current.system);
                measurement[ch_num].idecmin = get_decmin_1(ch_type, CI);
                measurement[ch_num].idec = measurement[ch_num].idecmin;
            }
			break;
		}

	}



	//nuc setup
    void nucset(short ch_num)
	{


		measurement[ch_num].ntimes = 0;
		measurement[ch_num].wasover = FALSE;
		//if (measurement[ch_num].response >= 0.09)
			nucinit(ch_num, TRUE);   /*initialization*/
/*		else
 - 		{
 - 			measurement[ch_num].idec = measurement[ch_num].idecmin;
 - 			measurement[ch_num].act0 = 0.;
 - 			measurement[ch_num].syst0 = -1;
 - 			measurement[ch_num].kun0 = 0;
 - 			strcpy(&measurement[ch_num].actstr0[0],"xxxxxx");
 -             measurement[ch_num].future.dosetime = NO_TIME;
 -             measurement[ch_num].mode = MEASMODE;
 -
 -             if(ch_num == current.main_chamber && get_meas_screen())
 -             {
 -                 id_init();
 -                 erase_lines(16,48,0);
 -                 erase_lines(Y_FUT_TIME,64 - Y_FUT_TIME,1);
 -             }
 - 		}*/

        if(ch_num == current.main_chamber && get_meas_screen())
        {
            disp_cal(measurement[ch_num].resp0);
            display_chamber();
            if(!chamber[ch_num].calkey)
                disp_nuclide();
        }

		respcor(ch_num,&measurement[ch_num].response);
		//mindec(ch_num);

		// reset chamber connected flag
		chamber[ch_num].connected_flag = TRUE;

        //setup for displaying time
        set_display_time();

	}

    //built-in isotope key pressed
    void getiso(short ch_num, short but)
	{

		short nuc_index;

		short ch_type;

		ch_type = chamber_type(ch_num);
        nuc_index = NuclideData_getNucIndexFromButtonKey(but,ch_type);

        set_nuclide_data(nuc_index,ch_num);


	}

    //response correction
	void respcor(short ch_num,float *response)
	{
		float alpha;
		float rdelta;
		float resp;
        float resp57;
        float diffr;
        short index_57;
        short ch_type;

        ch_type = chamber_type(ch_num);
        resp = *response;

        index_57 = NuclideData_getIndexFromName("Co57");
        resp57 = NuclideData_getResponse(index_57, ch_type);

        diffr = 1. - resp57;


		rdelta = chamber_response_correction(ch_num);

		alpha = (resp - resp57) / diffr;

		resp = resp * (1. + alpha * rdelta);

		*response = resp;
	}


    //system button -- Ci/Bq
    /* void sysbut(void)
	{


    	short ch_type;
        int i;
        bool syslocked;

        EE_READ(syslock,(uchar *)&syslocked);

        //system change is for all chambers

        if (current.lock || syslocked)
        {
            beep();
            return;
        }

        // Locked: CI
		//if (current.system == CI)
		//	current.system = BQ;
		//else
		//	current.system = CI;
		current.system = CI;
        EE_WRITE(syst,&current.system);

        for(i = 0; i < max_chambers; i++)
        {
            if(!chamber[i].exists)
                continue;

            ch_type = chamber_type(i);
            if (measurement[i].dogain == 0)
            	// Locked: CI
                //measurement[i].idecmin = get_decmin(ch_type,current.system);
            	measurement[i].idecmin = get_decmin(ch_type, CI);
            measurement[i].act0 = 0.;
            measurement[i].kun0 = 0;
            measurement[i].idec = measurement[i].idecmin;
        }
	}*/

#define MINCI 1.4e-3
#define MINBQ 14.0e+6

    void downbut(void)
	{
        float minact;
        float test_act;
        short ch_num = current.main_chamber;

        short ch_type;

        ch_type = chamber_type(ch_num);
        if(input_time_set())
        {
            dokey(10);
            return;
        }

		test_act = measurement[ch_num].act0;

		if (measurement[ch_num].dogain < 2)
		{

            minact = 1.4e-3;
            if(ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB)
                minact *= 10;

            // Locked: CI
            //if (current.system == BQ)
            //    minact *= 1.0e+10;
            //if ((current.system == CI && test_act >= MINCI) ||
			//	(current.system == BQ && test_act >= MINBQ))
            if(test_act >= MINCI)
			{
				beep();
				return;
			}
			else   /* activity ok */
			{
				if (measurement[ch_num].dogain == 1)
				{
					/* must wait until gain has been changed
					before setting dogain to 2 */
					if(measurement[ch_num].prevgain == 1)
					{
						beep();
						return;
					}

                    minact *= 0.1;
                    if(test_act >= minact)
					{
						measurement[ch_num].dogain = 2;
						measurement[ch_num].longavg = TRUE;
						measurement[ch_num].nmeas = 10;
						return;
					}
				}
			}

			measurement[ch_num].autodu = FALSE;
            dodown(ch_num,ch_type);
		}
		else  /* dogain = 2 */
		{
			if (measurement[ch_num].prevgain > 0 ||
				measurement[ch_num].longavg)
				beep();
			else
			{
				measurement[ch_num].longavg = TRUE;
				measurement[ch_num].nmeas = 10;
			}
		}
	}


    static void	dodown(short ch_num, short ch_type)
	{

		//measurement[ch_num].dodisp = TRUE;
		measurement[ch_num].act0 = 0.;

		if (measurement[ch_num].dogain == 0)
        {
            measurement[ch_num].dogain = 1;
            // Locked: CI
            //measurement[ch_num].idecmin = get_decmin_1(ch_type,current.system);
            measurement[ch_num].idecmin = get_decmin_1(ch_type, CI);
        }

        /* dogain = 1 */
        else
        {
            measurement[ch_num].dogain = 2;
            if(ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB)
                measurement[ch_num].idecmin = 1;
            else
                measurement[ch_num].idecmin = 0;
        }

		//mindec(ch_num);

	}



    void doauto(short ch_num)
	{

        float minact;
        float maxact;

        short ch_type;

        ch_type = chamber_type(ch_num);
        // Locked: CI
        //if (current.system == CI)
		//{
		//	minact = 0.05e-3;
        //    maxact = 0.8e-3;
        //}
        //else  //Bq
        //{
        //    minact = 1.85e+6;
        //    maxact = 8.0e+6;
        //}

        minact = 0.05e-3;
        maxact = 0.8e-3;

        if ((measurement[ch_num].act0 > minact &&
               measurement[ch_num].volts > 0.16) &&
              (measurement[ch_num].act0 < maxact ||
               measurement[ch_num].volts < 1.6))
        {
            if (measurement[ch_num].ntimes <= 5)
                ++measurement[ch_num].ntimes;
            if (measurement[ch_num].ntimes == 5)
            {
                measurement[ch_num].autodu = TRUE;
                measurement[ch_num].ntimes = 0;
                dodown(ch_num,ch_type);
            }
        }
        else
            measurement[ch_num].ntimes = 0;

	}



char const *cal_type_str[] = {"    ","SYR","VIAL","CAP "};


extern CALCORR calcorr[];
    void get_num_cal_corr(short ch_num, short nuc_index)
    {
        int i;
        short corr_index;
        short temp_nuc;
        short num_cal_corr;
        CALCORR calcor;

        num_cal_corr = 0;
        //init for case of no data for nuclide
        for(i= 0; i < 4; i++)
            calcor.corr[i] = 0.;

        corr_index = -1;
        for (i = 0; i < MAX_CAL_COR; i++)
        {
            //EE_READ(cal_corr[i].nuc_index,(uchar *)&temp_nuc);
            temp_nuc = calcorr[i].nuc_index;
            if (temp_nuc == nuc_index)
            {
                corr_index = i;
                break;
            }
        }

        if (corr_index != -1)
        {

            //EE_READ(cal_corr[corr_index],(uchar *)&calcor);
            calcor = calcorr[corr_index];
            for (i = 1; i < 4; i++)   //0 is REF
            {
                if (calcor.corr[i] != 0.)
                    ++num_cal_corr;
            }

        }
        chamber[ch_num].num_cal_corr = num_cal_corr;
        chamber[ch_num].type = REF;
        chamber[ch_num].calcor = calcor;


    }



    void change_cal_type(void)
    {
        uchar type;
        short ch_num = current.main_chamber;

        if(chamber[ch_num].num_cal_corr == 0)
        {
            beep();
            return;
        }

        type = chamber[ch_num].type;

        for(;;)
        {
            if (type == CAP)  //3
                type = REF;   //0
            else
                ++type;

            if(chamber[ch_num].calcor.corr[type] > 0.)
                break;
        }

        chamber[ch_num].type = type;
        disp_cal_type();
    }

    void set_cal_type(short ch_num, short type)
    {

        //if no factor for selected type, set type to REF
        if(type != REF && chamber[ch_num].calcor.corr[type] == 0.)
            type = REF;


        chamber[ch_num].type = type;
        if(ch_num == current.main_chamber && get_meas_screen())
            disp_cal_type();
    }


    void disp_cal_type(void)
    {
    	short ch_num = current.main_chamber;
        short index = chamber[ch_num].type;

        display_text(48,2,(char *)cal_type_str[index],0,MEDIUM,NORMAL);
    }



    void sources_key(void)
    {
        short s_index;
        STAND stnd;
        short nuc_index;
        char nucname[8];
        short num;
        short i;
        bool exists;
        short ch_num = current.main_chamber;
        short ch_type;

        ch_type = chamber_type(ch_num);
        //if no test sources, beep & exit
        exists = FALSE;
        for(i = 0; i < 6; i++)
        {
            EE_READ(stand[i],(uchar *)&stnd);
            // Removed:
            //if(stnd.syst != 0xff)
            if(stnd.caldate != (time_t) 0)
            {
                if(!(((ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB) && i == 4) ||
                   (ch_type == R_CHAMB && i == 5)))
                exists = TRUE;
                break;
            }
        }

        if(!exists)
        {
            beep();
            return;
        }


        //go to next source

        num = 5;    //Na22 not for R
        if(ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB)
            num = 6;
        s_index = current.source_index;
        for(;;)
        {
            ++s_index;
            if((ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB) && s_index == 4)
                continue;   //Ra226 not for PET
            if(s_index == num)
                s_index = 0;


            EE_READ(stand[s_index],(uchar *)&stnd);
            // Removed:
            //if (stnd.syst != 0xff)
            if (stnd.caldate != (time_t)0)
                break;
        }

        current.source_index = s_index;
        get_stndstr(s_index,nucname);
        nuc_index = NuclideData_getIndexFromName(nucname);

        set_nuclide_data(nuc_index, ch_num);

        return;
    }


    bool set_nuclide_data(short nuc_index, short ch_num)
    {
        float fUserResponse;
        float fOrigResponse;

        short ch_type;

        ch_type = chamber_type(ch_num);
        fUserResponse = NuclideData_getUserResponse(nuc_index, ch_type);
        fOrigResponse = NuclideData_getResponse(nuc_index, ch_type);
        if(fUserResponse == 0.0 && fOrigResponse == 0.0) return FALSE;

        chamber[ch_num].nuc_index = nuc_index;
        NuclideData_getNuclide(nuc_index, &chamber[ch_num].nucdata);
        if(fUserResponse != 0.0) measurement[ch_num].response = fUserResponse;
        else measurement[ch_num].response = chamber[ch_num].nucdata.response[ch_type];

        measurement[ch_num].resp0 = measurement[ch_num].response; //save original
        chamber[ch_num].calkey = FALSE;
        nucset(ch_num);

        get_num_cal_corr(ch_num, nuc_index);
        //lowresp(ch_num);

        return TRUE;
    }

    //select chamber to be Main Chamber
    bool select_chamber(void)
    {
        short j;

        j = current.main_chamber;
        if(j == 0)
            j = 1;
        else
            j = 0;

        if(measurement[j].mode != MEASMODE)
        {
            beep();
            read_screen(0);
            read_screen(1);
            erase_screen();
            display_text(8,10,"CHAMBER",0,MEDIUM,NORMAL);
            display_text(8,24,"MEASURING",0,MEDIUM,NORMAL);
            switch(measurement[j].mode)
            {
            case ZEROMODE:
                display_text(8,40,"ZERO",0,MEDIUM,NORMAL);
                break;
            case BKGMODE:
                display_text(8,40,"BACKGROUND",0,MEDIUM,NORMAL);
                break;
            case TESTMODE:
                display_text(8,40,"VOLTAGE",0,MEDIUM,NORMAL);
                break;
            }
            contmsg();
            erase_screen();
            write_screen(0);
            write_screen(1);
            return FALSE;
        }

        current.main_chamber = j;


        //initialize display
        measurement[j].idec = measurement[j].idecmin;
        measurement[j].act0 = 0.;
        // Removed:
        //measurement[j].syst0 = -1;
        measurement[j].kun0 = 0;
        strcpy(&measurement[j].actstr0[0],"xxxxxx");
        measurement[j].future.dosetime = NO_TIME;
        id_init();
        erase_screen();
        //disp_cal(measurement[j].response);
        disp_cal(measurement[j].resp0);
        measurement[j].wasover = FALSE;
        display_chamber();
        if(!chamber[j].calkey)
            disp_nuclide();

        //setup for displaying time
        set_display_time();
        return TRUE;


    }

