/*********************************************************************
  MODULE:	UTILITY CALCULATIONS for CRC-25 R,W

  FILE:		UtilCalcs_R.c

  DATE:		12/13/06

  ANALYSIS:  utility for calculating decay and Ci/Bq conversion
            nuclide decay routine

  CALLED BY:
		util

  
  *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "keyboard.h"
#include "i2c.h"
#include "chambfac.h"
#include "counter.h"
#include "message.h"
#include "nuc.h"
#include <string.h>
#define MAX_ACT_CI    99.

	extern const float unitfact[];

    //static time_t enter_time(char *str);

    void val_unit_msg(void)
	{
/*        display_text(12,20,"value",0,SMALL,NORMAL);
 -         display_text(72,20,"unit",0,SMALL,NORMAL);
 - 		display_text(6,48,"-> to change unit",0,SMALL,NORMAL);
 - 		display_text(6,56,"ENTER to accept",0,SMALL,NORMAL);*/

        display_small_message(VAL_UNIT_1,20,0,NORMAL);
        display_small_message(VAL_UNIT_2,20,0,NORMAL);
        display_small_message(VAL_UNIT_3,48,0,NORMAL);
        display_small_message(AUTO_ZERO_8,56,0,NORMAL);
	 }


 //conversion between Ci and Bq
 static void disp(char syst, float act, short ypos);
     void util_convert(void)
     {
    
         char sys0,sys1;
         short cal_unit;
         float act,act1;
         char kun;
         char ch;
    
         erase_screen();
         for(;;)  /* forever */
         {
             //display_text(8,0,"ENTER ACTIVITY",0,MEDIUM,NORMAL);
             display_medium_message(INV_14,0,0,NORMAL);
             val_unit_msg();
             //act = inpfl_u(8,27,6,&cal_unit,2);
             act = inpfl_u(8,27,5,&cal_unit,2);
             if(home_set())
                 return;
             /* if input out of range, -1 is returned */
             if (act < 0.)
             {
                 beep();
                 erase_screen();
                 continue;
             }
    
             switch(cal_unit)
             {
    
             case 0:
                 kun = UCI;
                 sys0 = CI;
                 sys1 = BQ;
                 break;
    
             case 1:
                 kun = MCI;
                 sys0 = CI;
                 sys1 = BQ;
                 break;
    
             case 2:
                 kun = _CI;
                 sys0 = CI;
                 sys1 = BQ;
                 break;
    
             case 3:
                 kun = MBQ;
                 sys0 = BQ;
                 sys1 = CI;
                 break;
    
             case 4:
                 kun = GBQ;
                 sys0 = BQ;
                 sys1 = CI;
                 break;
    
             }
    
             erase_screen();
             act1 = act / unitfact[kun - 1];
             if ((kun == _CI && act >= 100.) || (kun == MCI && act1 > 99.99) ||
                 
                 (kun == 6 && act > 3663.))
             {
                 beep();
                 continue;
             }
    
             disp(sys0,act1,10);
             if (sys0 == CI)
                 act1 *= BQFACTOR;
             else
                 act1 /= BQFACTOR;
             disp(sys1,act1,28);
             display_text(0,28,">",0,BIG,NORMAL);
    
             //display_text(6,48,"ENTER to continue",0,SMALL,NORMAL);
             //display_text(6,56,"MENU to end",0,SMALL,NORMAL);
             display_small_message(SIGNON_1,48,0,NORMAL);
             display_small_message(CALC_1,56,0,NORMAL);
    
             do
             {
                ch = keyin();
                if(home_set())
                    return;
                if (ch == MENUBUT)
                     return;
    
                if (ch == OK)
                    erase_screen();
                else
                     beep();
             }while(ch != OK);
    
         } /* end forever */
     }
    
    static void disp(char syst, float act, short ypos)
     {
         char actstr[10];
    
         format_activity(act,syst,actstr);
         display_text(10,ypos,&actstr[1],0,BIG,NORMAL);
     }


/*********************************************************************
  	NUCLIDE DECAY CALCULATION

    		

  ANALYSIS:	decays activity of specified nuclide between 2 given times

  DESIGN:  	calculates elapsed time and converts it to the same unit as
			the half-life
 INPUT VARIABLES:
		float act0			original activity
		time_t *time0		starting time
		time_t *time1		ending time
		float hl			half-life of nuclide
		uchar unit			unit of half-life (min, hour, day, year)

    OUTPUT VARIABLES:
		decay corrected activity

  *************************************************************************/
#include <math.h>
	
    float nucdecay(float act0,time_t time0,time_t time1,
					float hl,short unit)
	{

        double diff;	//time difference
		float dec;	//decay factor
		float act1;  //decayed activity

		//get time difference in seconds
        diff = (double)(time1 - time0);     

        //convert to minutes
        if(unit > 0) diff /= 60.;

		//convert diff to same units as hlunit
		if(unit > 1)
			diff /= 60.;
		if(unit > 2)
			diff /= 24.;
		if(unit > 3)
			diff /= 365.25;


		//get decay factor
		dec = exp(-M_LN2 * diff / hl);

		//activity is dec multiplied by current activity
		act1 = act0 * dec;

		return(act1);
	}

    time_t nucdecaytime(float fActivity0, float fActivity, time_t dtmTime0, float fHalflife, short iUnit){
    	double dHalfLifeInSeconds;
    	double dDeltaTimeInSeconds;
    	double dActivityRatio;
    	double dActivity, dActivity0, dTest, dTest2;
    	time_t dtmReturn;
    	unsigned long ulTest;
    	void *ptr;
	    unsigned long int *pulHigh;
	    unsigned long int *pulLow;
	    unsigned long int *pulRatioHigh;
	    unsigned long int *pulRatioLow;

    	dHalfLifeInSeconds = (double) fHalflife;
    	switch(iUnit){
			case MIN:
				dHalfLifeInSeconds *= 60.0;
				break;

			case HOUR:
				dHalfLifeInSeconds *= 3600.0;
				break;

			case DAY:
				dHalfLifeInSeconds *= 86400.0;
				break;

			case YEAR:
				dHalfLifeInSeconds *= 31557600.0;
				break;
    	}

    	dActivity = (double) fActivity;
    	dActivity0 = (double) fActivity0;
    	dActivityRatio = dActivity / dActivity0;

    	ptr = (void *) &dActivityRatio;
    	pulRatioHigh = ptr;
    	pulRatioLow = pulRatioHigh + 1;

    	dDeltaTimeInSeconds = (dHalfLifeInSeconds / M_LN2) * log(dActivityRatio);

    	ulTest = 0x80000000 - (unsigned long) dtmTime0;
    	dTest = ulTest;
    	dTest2 = -dDeltaTimeInSeconds;

    	ptr = (void *) &dDeltaTimeInSeconds;
    	pulHigh = ptr;
    	pulLow = pulHigh + 1;

    	if((*pulHigh == 0xfff00000) && (*pulLow == 0)) dtmReturn = 1;
    	else if((*pulRatioHigh == 0x7ff00000) && (*pulRatioLow == 0)) dtmReturn = 0;
    	else if(dTest2 >= dTest){
    		dtmReturn = 1;
    	}else{
    		ulTest = (unsigned long) dtmTime0;
    		ulTest -= 2;
    		dTest = ulTest;

    		if(dDeltaTimeInSeconds >= dTest) dtmReturn = 0;
    		else dtmReturn = dtmTime0 - (long)dDeltaTimeInSeconds;
    	}

    	return dtmReturn;
    }

    // calculation & display of decay
        
    /*void util_decay(void)
    {
        time_t time0,time1;
        short i;
        short index;
        uchar cal_system;
        short cal_unit;
        float act,calact,newact;
        char act_str[10];
        short ndec;
        short taunit;
        bool locked;
        char fmtstr[8];
        NUCDATA nucdata;
        char strng[40];

        EE_READ(syslock,(uchar *)&locked);

        for(;;)  // forever
        {
            erase_screen();

            // choose nuclide
            index = get_nuclide(TRUE,-1);
            if(index == -1)
                return;

            if(index == -2)
            {
                erase_screen();
                //display_text(8,2,"HALF-LIFE:",0,MEDIUM,NORMAL);
                display_medium_message(ADD_NUCLIDE_5,2,0,NORMAL);
                val_unit_msg();
                nucdata.halflife = inpfl_u(8,27,5,&taunit,0);
                if(home_set())
                    return;
                nucdata.hlunit = taunit + 1;
            }
            erase_screen();
            if(index != -2)
            {
                NuclideData_getNuclide(index,&nucdata);
                // display nuclide name
                display_text(0,0,&nucdata.name[0],0,MEDIUM,NORMAL);
            }
            // display half-life
            ndec = 3;
            if (nucdata.halflife >= 10.)
                ndec = 2;
            if (nucdata.halflife >= 100.)
                ndec = 1;
            if (nucdata.halflife > 500.)
                ndec = 0;
            strcpy(fmtstr,"%#5. f");
            fmtstr[4] = (char)ndec + '0';
            sprintf(strng,fmtstr,nucdata.halflife);
            display_text(67,0,strng,0,MEDIUM,NORMAL);
            i = nucdata.hlunit - 1;
            display_text(117,0,(char *)timeunit[i],0,MEDIUM,NORMAL);

            //display_text(8,11,"ENTER ACTIVITY",0,MEDIUM,NORMAL);
            display_medium_message(INV_14,11,0,NORMAL);
            val_unit_msg();
            if(locked)
                act = inpfl_u(8,28,6,&cal_unit,4);
            else
                act = inpfl_u(8,28,6,&cal_unit,2);
            if(home_set())
                return;
            
            ++cal_unit;
            if(locked)
            {
                cal_system = BQ;
                cal_unit += 3;
            }
            else
            {
                cal_system = CI;
                if(cal_unit  >  3)
                    cal_system = BQ;
            }
            switch(cal_unit)
            {
            case 1:
                calact = act * 1.0e-6;    // from uCi to Ci
                break;
            case 2:
                calact = act * 1.0e-3;      // from mCi to Ci
                break;
            case 3:                         // Ci
                calact= act;
                break;
            case 4:
                calact = act * 1.0e+6;      // from MBq to Bq
                break;
            case 5:
                calact = act * 1.0e+9;      // from GBq to  Bq
                break;
            }

            erase_lines(10,54,0);
            // input starting date/time
            time0 = enter_time("FROM:");
            if(home_set())
                return;

            // input ending date/time
            erase_lines(12,52,0);
            time1 = enter_time("TO:");
            if(home_set())
                return;

            // calculate new activity
            newact = nucdecay(calact,time0,time1,
                nucdata.halflife,nucdata.hlunit);

            // display results
            erase_lines(12,52,0);
            format_activity(calact,cal_system,act_str);

            //display starting activity & time 
            display_text(0,18,act_str,0,MEDIUM,NORMAL);
            timeout(strng,&time0);
            display_text(87,18,strng,0,MEDIUM,NORMAL);
            //display starting date
            dateout(strng,&time0,4);
            strng[6] = ' ';
            display_text(35,29,strng,0,MEDIUM,NORMAL);
            
            //format decayed activity
            if(((cal_system == CI) && (newact > MAX_ACT_CI)) ||
               ((cal_system == BQ) && (newact > MAX_ACT_CI * BQFACTOR)))
            {
                strcpy(act_str,"OVER");
            }
            else
                format_activity(newact,cal_system,act_str);
            
            //display ending activity & time 
            display_text(0,44,act_str,0,MEDIUM,NORMAL);
            timeout(strng,&time1);
            display_text(87,44,strng,0,MEDIUM,NORMAL);
            //display starting date
            dateout(strng,&time1,4);
            strng[6] = ' ';
            display_text(35,55,strng,0,MEDIUM,NORMAL);
            
            //wait for key press
            if(keyin() == MENUBUT)
                return;
            if(home_set())
                return;

        }
    } */

    //short get_nuclide(short flag, char *str)
    short get_nuclide(short flag, short msg_num)
    {

        short nuc_index = -1;
        char ch;
        short mode = get_crc_mode();

        choose_nuclide_msg(flag,msg_num);
        do
        {
            ch = keyin();
            if (home_set() || ch == MENUBUT)
                return -1;
            switch (ch)
            {


            case ISO1:
            case ISO2:
            case ISO3:
            case ISO4:
            case ISO5:
            case ISO6:
            case ISO7:
            case ISO8:
                nuc_index = NuclideData_getNucIndexFromButtonKey(ch - ISO1,0);
                break;
            case USER1:
            case USER2:
            case USER3:
            case USER4:
            case USER5:
                if(mode == CRC_CAL)
                    nuc_index = user_key_index(ch - USER1);
                else
                    nuc_index = get_counter_user_key(ch - USER1);
                if (nuc_index == -1)
                    beep();
                break;


            case CALBUT:
                switch(flag)
                {
                case 0:
                    beep();
                    break;
                case 1:
                case 3:    
                    return -2;
                case 2:
                    nuc_index = GENERIC_NUC;
                    break;
                }    
                break;

            case NUCBUT:
                input_nuclide(&nuc_index);
                if(home_set())
                    return -1;
                if (nuc_index == -1)
                {
                    erase_lines(10,54,0);
                    choose_nuclide_msg(flag,msg_num);
                }
                break;


            default:
                nuc_index = -1;
                beep();
                break;

            }
        }while(nuc_index == -1);
        return (nuc_index);
    }


    /*static time_t enter_time(char *str)
    {

        bool ok;
        time_t time0;
        struct tm dt;
    
        display_text(8,17,str,0,MEDIUM,NORMAL);
        //display_text(8,56,"ENTER for NOW",0,SMALL,NORMAL);
        display_small_message(DATE_TIME_IN_9,56,0,NORMAL);
        //display_text(12,48,"hhmm/DD/MM/YYYY",0,SMALL,NORMAL);
        display_small_message(DATE_TIME_IN_8,48,0,NORMAL);
        do
        {
            ok = decode_time_date(FALSE,&dt);
            if (!ok)
                beep();
        }while(!ok);
        time0 = mk_time(&dt);
        return (time0);

    }*/
