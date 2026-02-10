/*********************************************************************
  MODULE:	CHANGE CALIBRATION NUMBER  for CRC-25 R,W

  FILE:		ChangeCalNum_R.c

  DATE:		11/10/06

  ANALYSIS:	allows user to change nuclide's calibration number

  CALLED BY:
		nuclide_menu

    *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "i2c.h"
#include "keyboard.h"
#include "message.h"
#include "chambfac.h"
#include "nuc.h"


    extern CURRENT current;
    extern USERCAL user_cal[];
	
static bool add_cal_num(short *index);
static void disp_cal0(float response);
static void disp_cal1(short cal_index, float resp);
static void enter_cal_num(short cal_index,NUCDATA *data, 
                          short *nuc_index);
static void restore_cal(short cal_index);
static short get_new_cal_index(short nuc_index);
    
    void change_cal_num(void)
	{
		short nuc_index;
		short cal_index;
		NUCDATA origdata;
        bool start_flag;
        char ch;
        bool yn;
        short ch_num;
        short ch_type;

        start_flag = TRUE;
        for(;;)
        {

    		if (start_flag)
            {

                display_medium_message(CHANGE_CAL_1,0,0,NORMAL);
                //choose_nuclide_msg(0,"");
                choose_nuclide_msg(0,-1);
            }

            do
            {
        		ch = keyin();
        		if (ch == MENUBUT || home_set())
        			return;

                start_flag = FALSE;
        		switch (ch)
        		{
        
        
        		/* isotope selection button pressed */
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

        		/* user button pressed */
        		case USER1:
        		case USER2:
        		case USER3:
        		case USER4:
                case USER5:
        			nuc_index = user_key_index(ch - USER1);
        			if (nuc_index == -1)
        				beep();
        			break;


        		case NUCBUT:
        			input_nuclide(&nuc_index);
                    if(home_set())
                        return;
        			if (nuc_index == -1)
                        start_flag = TRUE;
        			break;


        		default:
        			nuc_index = -1;
        			beep();
        			break;


        		}
            }while( nuc_index == -1);

            ch_num = current.main_chamber;
            ch_type = chamber_type(ch_num);

            NuclideData_getNuclide(nuc_index,&origdata);
            erase_screen();
            display_text(0,2,&origdata.name[0],0,MEDIUM,NORMAL);
            disp_cal0(origdata.response[ch_type]);
            
            cal_index = get_new_cal_index(nuc_index);

            if (cal_index == -1)  // no new cal # 
            {
                if(!add_cal_num(&cal_index))
                    continue;
                //display_text(0,48,"Change Cal #?",0,SMALL,NORMAL);
                display_medium_message(CHANGE_CAL_2,48,0,NORMAL);
                yn = yesorno();
                if(home_set())
                    return;
                if (yn)
                {    
                    enter_cal_num(cal_index,&origdata,&nuc_index);
                    if(home_set())
                        return;

                }
                    
            }



            else  // new cal # exists 
            {
                disp_cal1(cal_index,origdata.response[ch_type]);

                restore_cal(cal_index);


            }
		    erase_screen();
            start_flag = TRUE;
        } //end of forever
        
	}



static short get_new_cal_index(short nuc_index)
	{
	    short i;
        USERCAL ucal;
        short cal_index;

		cal_index = -1;
		for (i = 0; i < MAX_NEW_CAL_OBSOLETE; i++)
		{
            //EE_READ(usercal[i],(uchar *)&ucal);
			if (ucal.nuc_index == nuc_index)
			{
				cal_index = i;
				break;
			}
		}
        return(cal_index);
	}



   void input_nuclide(short *nuc_index)
	{

		for(;;)
		{
			erase_screen();
			specify_nuclide_msg();

			*nuc_index = get_nuclide_index(FALSE);
			erase_screen();
			switch (*nuc_index)
			{
			case -1:
				no_nuclide_msg();
				break;

			case -2:
				specify_further_msg();
				break;

			case -3:
				*nuc_index = -1;
				return;

			default:
				return;
			}
		}
	}
    
	

	
static bool add_cal_num(short *index)
    {
        
        int i;
        short temp_nuc;
        short cal_index;

        /* is there room to add? */
        cal_index = -1;
        for (i = 0; i < MAX_NEW_CAL_OBSOLETE; i++)
        {
            //EE_READ(usercal[i].nuc_index,(uchar *)&temp_nuc);
            if (temp_nuc == -1)
            {
                cal_index = i;
                break;
            }
        }

        if (cal_index == -1)
        {
            //display_text(8,12,"NO ROOM TO ADD",0,MEDIUM,NORMAL);
            display_medium_message(NO_ROOM,12,0,NORMAL);
            contmsg();
            erase_screen();
            return FALSE;
        }

        *index = cal_index;
        return TRUE;



    }

static void enter_cal_num(short cal_index,NUCDATA *data, 
                          short *nuc_index)
    {
        
        float response;
        bool ok;
        USERCAL ucal;
        bool yn;

        do
        {
            ok = TRUE;
            response = get_calnum(TRUE,0);
            if(home_set())
                return;
            if (response != -1.)
            {
                erase_screen();
                display_text(0,2,&data -> name[0],0,MEDIUM,0);
                disp_cal0(data -> response[0]);
                disp_cal1(-1,response);
                //display_text(8,52,"OK? Y or N",0,MEDIUM,NORMAL);
                display_medium_message(OK_MSG,52,0,NORMAL);
                yn = yesorno();
                if(home_set())
                    return;
                if (yn)
                {
                    // save new usercal data 
                    ucal.response = response;
                    ucal.nuc_index = *nuc_index;
                    //EE_WRITE(usercal[cal_index],(uchar *)&ucal);
                    user_cal[cal_index] = ucal;
                    
                }
                else
                    ok = FALSE;
            }
        }while(!ok);
        
        erase_screen();
    }

        

static void disp_cal0(float response)
	{

    char strng[20];
    
        //display_text(0,24,"Orig Cal #: ",0,SMALL,NORMAL);
        display_small_message(CHANGE_CAL_8,24,0,NORMAL);
		if (response == 0.)
            //display_text(72,24,"NONE",0,SMALL,NORMAL);
            display_small_message(CHANGE_CAL_3,24,0,NORMAL);
		else
		{
			cal_from_response(strng,response);
			display_cal(strng,72,24,SMALL,0);

		}
	}

static void disp_cal1(short cal_index, float resp)
	{
		float response;
        char strng[20];

        //display_text(0,32," New Cal #: ",0,SMALL,NORMAL);
        display_small_message(CHANGE_CAL_4,32,0,NORMAL);
		if (cal_index == -1)
			response = resp;  
		else
            //EE_READ(usercal[cal_index].response,(uchar *)&response);
		cal_from_response(strng,response);
		display_cal(strng,72,32,SMALL,0);
	}



static void restore_cal(short cal_index)
    {
        
        short temp_nuc;
        bool yn;

        //restore by deleting entry for changed Cal#

        //display_text(0,48,"Restore Orig Cal # ?",0,SMALL,0);
        display_small_message(CHANGE_CAL_5,48,0,NORMAL);
        yn = yesorno();
        if(home_set())
            return;
        if (yn)
        {
            temp_nuc = -1;
            //EE_WRITE(usercal[cal_index].nuc_index,(uchar *)&temp_nuc);
            user_cal[cal_index].nuc_index = temp_nuc;
        }

    }

#include <string.h>
//void choose_nuclide_msg(short flag, char *str)
void choose_nuclide_msg(short flag, short msg_num)
    {
        //if (strlen(str) != 0)
        if(msg_num != -1)
            //display_text(0,0,str,0,MEDIUM,NORMAL);
            display_medium_message(msg_num,0,0,NORMAL);

        display_medium_message(CHOOSE_NUCLIDE_1,12,0,NORMAL);   //CHOOSE NUCLIDE
        display_small_message(CHOOSE_NUCLIDE_2,32,0,NORMAL);    //"Press NUCL or"
        display_small_message(CHOOSE_NUCLIDE_3,40,0,NORMAL);    //"Pre-set nuclide key"
        display_small_message(CHOOSE_NUCLIDE_4,48,0,NORMAL);    //"or User key"
        if(flag == 1)   //"or CAL# for Half-Life"
            display_small_message(CHOOSE_NUCLIDE_5,56,0,NORMAL);
        if(flag == 2)   //"or CAL# for All Chan"
            display_small_message(CHOOSE_NUCLIDE_6,56,0,NORMAL);
        
    }
