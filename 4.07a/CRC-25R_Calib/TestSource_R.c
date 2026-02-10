 /*********************************************************************
  MODULE:	 SETUP Test Sources  for CRC-25 R,W

  FILE:		TestSource_R.c

  DATE:		01/15/07
            08/22/07 -- removed constancy automation
            03/26/08 -- warning message if caltime > today
            03/27/08 -- added initialization of daily and constancy to none

  CALLED BY:
        Other Menu

    *************************************************************************/
#include "crc.h"
#include "menu.h"
#include "screen.h"
#include "i2c.h"
#include "keyboard.h"
#include "message.h"
#include "chambfac.h"
#include "daily.h"
#include "nuc.h"
#include <string.h>
static bool now_act_ok(STAND *stnd,float test_act,short index);
static bool	disp_stand(STAND *stnd, bool d_flag);
static bool input_source(STAND *stnd, float *test_act, short index,float max_act);
static bool	constancy_source(void);
static short num_daily_sources(void);
static void test_source(short ts_index);

    void test_source_menu(void)
    {
        short iret;

        for(;;)
        {    
        
            iret = display_menu(TS_MENU);

            if(home_set())
                return;
            
            switch (iret)
            {
            case -1:  //MENU key
                erase_screen();
                return;
            case 0: //Co57
            case 1: //Co60
            case 2: //Ba133
            case 3: //Cs137
            case 4: //Ra226 -- R only
                test_source(iret);
                break;
            case 6:
                constancy_source();
                break;
            }
            if(home_set())
                return;
        }    
    }


#define  NONE   -1

    static void test_source(short ts_index)
	{

		STAND stnd;
		float test_act;
		bool act_ok;
        char stndstr[8];
        float max_act = 1.0;
        bool yn;
        short ciso;

        get_stndstr(ts_index,stndstr);
        EE_READ(stand[ts_index],(uchar *)&stnd);

		erase_screen();

		display_text(2,0,stndstr,0,MEDIUM,NORMAL);

        if (disp_stand(&stnd,TRUE) || home_set())
            return;

        

        for(;;)  /* forever */
		{
			erase_screen();

			// Removed:
			//if(stnd.syst != (uchar)NONE)
			if(stnd.caldate != (time_t)0)
			{
				//DO YOU HAVE
                display_medium_message(TEST_SOURCE_1,2,0,NORMAL);
				display_text(2,16,stndstr,0,MEDIUM,NORMAL);
				//?
                display_medium_message(TEST_SOURCE_2,16,0,NORMAL);
                display_medium_message(YES_OR_NO,50,0,NORMAL);
                yn = yesorno();
                if(home_set())
                    return;
				if (!yn)
				{
					//* set to don't have
					// Removed:
					//stnd.syst = 0xff;
					stnd.caldate = (time_t)0;
                    stnd.daily = FALSE;
                    stnd.const_iso = FALSE;
                    EE_WRITE(stand[ts_index],(uchar *)&stnd);

					erase_screen();
					return;
				}
			}
            //initialize to not daily and not constancy
            stnd.daily = FALSE;
            stnd.const_iso = FALSE;
            

			if(!input_source(&stnd,&test_act,ts_index,max_act))
                return;

            act_ok = now_act_ok(&stnd,test_act,ts_index);

            if(act_ok)
            {
                erase_lines(10,54,0);
                //USE DAILY ?
                display_medium_message(TEST_SOURCE_3,22,0,NORMAL);
                display_medium_message(YES_OR_NO,46,0,NORMAL);
                stnd.daily = FALSE;
                yn = yesorno();
                if(home_set())
                    return;
                if (yn)
                    stnd.daily = TRUE;
            }
                
            if(act_ok)
            {
                erase_lines(10,54,0);
                yn = disp_stand(&stnd,TRUE);
                if(home_set())
                    return;
				if(yn)
				{
                    erase_text_line(5,0,MEDIUM);
					break;
				}
            }
		}/* end forever */

        //if daily source and no constancy source yet, make this the constancy source
		if (stnd.daily)
        {
            ciso = get_const_source();
            if(ciso == NONE)
                stnd.const_iso = TRUE;
        }        
        EE_WRITE(stand[ts_index],(uchar *)&stnd);
        

		erase_screen();
	}


	/* test current activity for minimum value */
    extern time_t clock_time;
    extern time_t low_clock_time;
static bool now_act_ok(STAND *stnd,float test_act,short index)
	{
		short j;
		float now_act;
        time_t time0,time1;
		NUCDATA  nucdata;
        float min_act;
        char name[8];


		read_clock(&time1);
        clock_time = time1;
        low_clock_time = clock_time;
        reset_minute_counter_with_seconds();
        get_stndstr(index,name);  
        j = NuclideData_getIndexFromName(name);
		NuclideData_getNuclide(j,&nucdata);
		time0 = stnd -> caldate;

		now_act = nucdecay(test_act,time0,time1,
						nucdata.halflife,nucdata.hlunit);
        //Rad226: minimum = 10uCi
        //all others: minimum = 50uCi
        min_act = 50.e-6;
        if (index == 4)
            min_act = 10.e-6;
        if (now_act < min_act)
		{
			beep();
			erase_screen();
			//SOURCE
			//TOO WEAK
            display_medium_message(TEST_SOURCE_4,12,0,NORMAL);
            display_medium_message(TEST_SOURCE_5,32,0,NORMAL);
            
			contmsg();
			erase_screen();
			return FALSE;
		}
		return TRUE;
	}


static const char *m_ustr[] = 
{
    "$Ci",
    "mCi",
    " Ci",
    "   ",
    "MBq",
    "GBq"
};

	extern float unitfact[];
static bool	disp_stand(STAND *stnd, bool d_flag)
	{
		bool yn;
		float act,act1;
		char dactstr[10];
		short ndec;
		short kun;
        char strng[12];

        // Removed
        //if(stnd -> syst == (uchar)NONE)
        if(stnd -> caldate  == (time_t)0)
            //NO SOURCE
            display_medium_message(TEST_SOURCE_9,32,0,NORMAL);

		else
		{

			//S/N:
            display_medium_message(TEST_SOURCE_10,12,0,NORMAL);
			display_text(40,12,&stnd -> sn[0],0,MEDIUM,NORMAL);
            dateout_language(strng, &stnd -> caldate, 4);
			display_text(0,27,strng,0,MEDIUM,NORMAL);
			act1 = stnd -> act;
			// Removed:
			//ndec = getdec(act1,stnd -> syst,&kun);
			ndec = getdec(act1, CI, &kun);
			act = act1 * unitfact[kun - 1];
			act2str(act,dactstr,ndec,FALSE,kun);
            dactstr[6] = '\0';
			display_text(0,40,&dactstr[1],5,MEDIUM,NORMAL);
            strcpy(strng,m_ustr[kun - 1]);
			display_text(48,40,strng,0,MEDIUM,NORMAL);

			if(d_flag && stnd -> daily)
                //DAILY
                display_medium_message(TEST_SOURCE_11,40,0,NORMAL);
		}
		//OK?  Y or N
        display_medium_message(OK_MSG,52,0,NORMAL);
		yn = yesorno();
        
		return (yn);

	}

static bool	input_source(STAND *stnd, float *test_act, short index, float max_act)
	{

        bool act_ok;
		short cal_unit;
		// Removed:
		//char stsyst;
		float act,act1;
        struct tm stdate;
        char stndstr[8];
        time_t nowtime;
        time_t caltime;
        bool date_ok;

        get_stndstr(index,stndstr); 

		erase_screen();
        display_text(2,0,stndstr,0,MEDIUM,NORMAL);


		//ENTER S/N
        display_medium_message(TEST_SOURCE_12,12,0,NORMAL);
        inpalnum(8,32,&stnd -> sn[0],10,FALSE);
        if(home_set())
            return FALSE;
        erase_lines(32,10,0);

        for(;;)
        {    

            //CALIBRATED
            display_medium_message(TEST_SOURCE_13,12,0,NORMAL);
            //display_text(30,56,"MMDDYYYY",0,SMALL,NORMAL);
            display_small_message(DATE_TIME_IN_6,56,0,NORMAL);
            
            if(!date_in(8,28,&stdate) || home_set())
                return FALSE;

            //make calibration beginning of day so "today" is always OK
            stdate.tm_sec = 0;
            stdate.tm_min = 0;
            stdate.tm_hour = 0;
            stdate.tm_isdst = 0;

            //if calibration > Now, give warning message & keep if user OKs it

            read_clock(&nowtime);
            caltime = mk_time(&stdate);
            if(caltime <= nowtime)
                break;

            //caltime > nowtime
            beep();
            erase_lines(56,8,0);
            display_text(0,40,"Date In Future",0,MEDIUM,NORMAL);
            display_medium_message(OK_MSG,52,0,NORMAL);
            date_ok = yesorno();
            erase_lines(12,52,0);
            if(home_set())
                return FALSE;
            if(date_ok)
                break;
        }    
            

		do
		{
            erase_lines(12,30,0);
			//CALIB ACTIVITY:
            display_medium_message(TEST_SOURCE_14,10,0,NORMAL);
			/*if(current.lockedbq)
			{
				//IN MBq
                display_medium_message(TEST_SOURCE_15,22,0,NORMAL);

				act = inpflt(8,42,MEDIUM,0,6,0,9999);
                if(home_set())
                    return FALSE;
				cal_unit = 3;
			}
			else
			{*/
				val_unit_msg();
                act = inpfl_u(8,30,6,&cal_unit,1);
                if(home_set())
                    return FALSE;
				++cal_unit;
			//}
			act_ok = TRUE;
			if(act == 0.)
				act_ok = FALSE;
			if (!act_ok)
				beep();

			if(act_ok)
			{
				switch(cal_unit)
				{
				case 1:
					act1 = act * 1.0e-6;    // from uCi to Ci
					*test_act = act1;
					// Removed:
					//stsyst = CI;
					break;
				case 2:
					act1 = act * 1.0e-3;		// from mCi to Ci 
					*test_act = act1;
					// Removed:
					//stsyst = CI;
					break;
				case 3:
					act1 = act * 1.0e+6; 		// from MBq to Bq 
					*test_act = act1 / BQFACTOR;
					// Removed:
					//stsyst = BQ;
					break;
				}

				if (*test_act > max_act)
				{
					beep();
					erase_lines(10,54,0);
					//SOURCE
					//TOO STRONG
                    display_medium_message(TEST_SOURCE_4,12,0,NORMAL);
                    display_medium_message(TEST_SOURCE_16,32,0,NORMAL);                    
					contmsg();
                    if(home_set())
                        return FALSE;
					act_ok = FALSE;
				}
			}
			else
				beep();
		}while(!act_ok);

        stnd -> caldate = mk_time(&stdate);
		stnd -> act = act1;
		// Removed:
		//stnd -> syst = stsyst;
        stnd -> nucnum = NuclideData_getIndexFromName(stndstr);

        return TRUE;

	}


static bool	constancy_source(void)
	{
		short num_iso;
        short i,k;
		STAND stnd;
		bool yn;
		short ciso;
		short nchoose;
        char buf[20];
        char stndstr[8];
        bool eebool;

		erase_screen();
		num_iso = num_daily_sources();
		if(num_iso == 0)
			return FALSE;



        ciso = get_const_source();
		//CONSTANCY TEST
        display_medium_message(TEST_SOURCE_17,0,0,NORMAL);

		if(num_iso == 1)  // only 1 possibliity 
		{
            for(i = 0; i < 5; i++)
			{
                EE_READ(stand[i],(uchar *)&stnd);
                // Removed:
				//if (stnd.syst == (uchar )NONE || stnd.daily == FALSE)
				if (stnd.caldate == (time_t)0 || stnd.daily == FALSE)
					continue;
				// found the only daily source 
				ciso = (uchar)i;
				break;
			}
		}

		if(ciso != NONE)
		{
            EE_READ(stand[ciso],(uchar *)&stnd);
			if(stnd.daily == FALSE)
				ciso = NONE;
		}

		for(;;)
		{
			if (ciso != NONE)
			{
				get_stndstr(ciso,stndstr);  
                display_text(32,12,stndstr,0,MEDIUM,NORMAL);
				if(num_iso == 1)
				{
					contmsg();
                    if(home_set())
                        return FALSE;
                    
					yn = TRUE;
				}
				else
				{
                    display_medium_message(OK_MSG,50,0,NORMAL);
					yn = yesorno();
                    if(home_set())
                        return FALSE;
                    
				}
				if(yn)
				{
                    //make sure only 1 constancy source
                    eebool = FALSE;
                    for(i = 0; i < 6; i++)
                        EE_WRITE(stand[i].const_iso,(uchar *)&eebool);

                    //make this the constancy source
                    eebool = TRUE;
                    EE_WRITE(stand[ciso].const_iso,(uchar *)&eebool);
					return TRUE;
				}
			}

            erase_lines(12,52,0);
			k = 0;
			for(i = 0; i < 5; i++)
			{
                EE_READ(stand[i],(uchar *)&stnd);
                // Removed:
				//if (stnd.syst == 0xff || stnd.daily == FALSE)
                if (stnd.caldate == (time_t)0 || stnd.daily == FALSE)
					continue;
				++k;
                sprintf(buf,"%d",k);
                strcat(buf,".");
                get_stndstr(i,stndstr);  
                strcat(buf,stndstr);
                display_text(0,10 * k + 2,buf,0,MEDIUM,NORMAL);

			}

			nchoose = choose(1,k);
            if(home_set())
                return FALSE;
            
			k = 0;
			for(i = 0; i < 5; i++)
			{
                EE_READ(stand[i],(uchar *)&stnd);
                // Removed:
				//if (stnd.syst == 0xff || stnd.daily == FALSE)
                if (stnd.caldate == (time_t)0 || stnd.daily == FALSE)
					continue;
				++k;
				if (nchoose == k)
				{
					ciso = (uchar)i;
					break;
				}
			}

			erase_screen();
            //CONSTANCY TEST
            display_medium_message(TEST_SOURCE_17,0,0,NORMAL);
		}

	}

static short num_daily_sources(void)
	{
		STAND stnd;
		short i;
		short num_iso;

		num_iso = 0;
		for(i = 0; i < 6; i++)
		{
            EE_READ(stand[i],(uchar *)&stnd);
            // Removed:
			//if (stnd.syst != 0xff && stnd.daily == TRUE)
			if (stnd.caldate != (time_t)0 && stnd.daily == TRUE)
				++num_iso;
		}

		if (num_iso == 0)
		{
			erase_screen();
			beep();
			//ERROR
            display_medium_message(INPUT_1,0,0,NORMAL);
			//NO DAILY SOURCE
			//WAS CHOSEN
            display_medium_message(TEST_SOURCE_18,22,0,NORMAL);
            display_medium_message(TEST_SOURCE_19,32,0,NORMAL);            
			contmsg();
			erase_screen();
		}
		return num_iso;
	}

/*static void wr_nuc(short i);
 - static short on_list(short nuc_index, short num);
 - static bool delete_from_list(short num, short list_index);
 - static AUTO_CON auto_const;
 - 
 -     static void constancy_automation(void)
 -     {
 - 
 -         short num;
 -         int i;
 -         bool ynret;
 -         short nuc_index;
 -         char ch,ch1;
 -         char nucname[8];
 -         short list_index;
 -         bool done;
 -         bool del;
 - 
 -         EE_READ (auto_con,(uchar *)&auto_const);
 -         num = auto_const.num;
 - 
 -         for(;;)
 -         {
 -             erase_screen();
 -             //display_text(8,0,"Constancy Test",0,MEDIUM,NORMAL);
 -             display_medium_message(TEST_SOURCE_23,0,0,NORMAL);
 -             
 -             if(num == 0)
 -                 //display_text(8,22,"Not Automated",0,MEDIUM,NORMAL);
 -                 display_medium_message(TEST_SOURCE_24,22,0,NORMAL);
 -             else
 -             {
 -                 for(i = 0; i < num; i++)
 -                     wr_nuc(i);
 -             }    
 -             
 -             //OK? Y OR N
 -             display_medium_message(OK_MSG,52,0,NORMAL);
 -             ynret = yesorno();
 -             if(home_set())
 -                 return;
 -             if(ynret)
 -             {
 -                 auto_const.num = num;
 -                 EE_WRITE(auto_con,(uchar *)&auto_const);
 -                 erase_screen();
 -                 return;
 -             }
 - 
 -             erase_screen();
 -             //display_text(8,14,"CHOOSE NUCLIDE",0,MEDIUM,NORMAL);
 -             display_medium_message(CHOOSE_NUCLIDE_1,2,0,NORMAL);
 -             
 -             for(i = 0; i < num; i++)
 -                 wr_nuc(i);
 - 
 -             done = FALSE;
 -             for(;;)
 -             {
 -                 
 -                 nuc_index = -1;
 -                 ch = keyin();
 -                 if(home_set())
 -                     return;
 -                 
 -                 switch(ch)
 -                 {
 -         
 -                 case OK:
 -                     done = TRUE;
 -                     break;
 -         
 -                 case ISO1:
 -                 case ISO2:
 -                 case ISO3:
 -                 case ISO4:
 -                 case ISO5:
 -                 case ISO6:
 -                 case ISO7:
 -                 case ISO8:
 -                     nuc_index = get_key_index(ch - ISO1,0);
 -                     break;
 -         
 -                 case USER1:
 -                 case USER2:
 -                 case USER3:
 -                 case USER4:
 -                 case USER5:
 -                     ch1 = ch - USER1;
 -                     nuc_index = user_key_index(ch1);
 -                     break;
 -         
 -                 case NUCBUT:
 -                     read_screen(0);
 -                     nuc_index = get_nuclide_index(TRUE);
 -                     if(home_set())
 -                         return;
 -                     write_screen(0);
 -                     break;
 -         
 -                 default:
 -                     break;
 -                 }
 - 
 -                 if(done)
 -                     break;
 -                 
 -                 if(nuc_index < 0)
 -                     beep();
 -                 else
 -                 {
 -                     get_nuclide_name(nuc_index,nucname);
 -                     list_index = on_list(nuc_index,num);
 -                     //if not on list, error message if list full, else add to list
 -                     if(list_index == -1)  
 -                     {
 -                         if(num == 8)
 -                         {
 -                              beep();
 -                              read_screen(0);
 -                              erase_lines(14,50,0);
 -                              //display_text(8,32,"LIST FULL",0,MEDIUM,0);
 -                              display_medium_message(REMOTES_2,32,0,NORMAL);
 -                              contmsg();
 -                             if(home_set())
 -                                 return;
 -                              
 -                              write_screen(0);
 -                          }
 -                          else
 -                          {
 -                              //add to list
 -                              auto_const.nuclides[num] = nuc_index;
 -                              ++num;
 -                             for(i = 0; i < num; i++)
 -                                 wr_nuc(i);
 -                              
 -                          }    
 -                     }
 -                     else    //on list, delete it
 -                     {   del = delete_from_list(num,list_index);
 -                         if(home_set())
 -                             return;
 -                         if(del)
 -                             --num;
 -                     }    
 -                 }
 -             }
 - 
 - 
 - 
 -             
 -         }
 -         
 - 
 -         
 - 
 - 
 -     }    
 - 
 -     
 - 
 - 
 -     static void wr_nuc(short i)
 -     {
 -         short xpos,ypos;
 -         char str[10];
 -         
 -         get_nuclide_name(auto_const.nuclides[i],str);
 -         xpos = 7 * 6 * (i % 3);
 -         ypos = 12 * (1 + (i/3)) + 2;
 -         display_text(xpos,ypos,str,0,SMALL,NORMAL);
 - 
 -     }    
 - 
 -     
 -     static short on_list(short nuc_index, short num)
 -     {
 -         short i;
 -         short list_index;
 - 
 -         if(num == 0)
 -             return -1;
 - 
 -         list_index = -1;   //not on list
 -         for(i = 0; i < num; i++)
 -         {    
 -             if(nuc_index == auto_const.nuclides[i])
 -             {
 -                 list_index = i;
 -                 break;
 -             }    
 -             
 -         }
 -         return list_index;
 -     }
 - 
 -     static bool delete_from_list(short num, short list_index)
 -     {
 -         short i;
 -         bool  ynret;
 -         short newn;
 - 
 -         read_screen(0);
 -         erase_lines(14,50,0);
 -         //display_text(6,40,"Already On List",0,SMALL,NORMAL);
 -         //display_text(6,48,"Delete? Y or N",0,SMALL,NORMAL);
 -         display_small_message(REMOTES_3,40,0,NORMAL);
 -         display_small_message(REMOTES_4,48,0,NORMAL);
 -         ynret = yesorno();
 -         if(!ynret)
 -         {    
 -             write_screen(0);
 -             return FALSE;
 -         }
 - 
 -         erase_lines(40,24,0);
 -         for(i = list_index; i < 7; i++)
 -             auto_const.nuclides[i] = auto_const.nuclides[i + 1];
 -         
 -         newn = num - 1;
 -         for(i = 0; i < newn; i++)
 -             wr_nuc(i);
 - 
 -         return TRUE;
 - 
 -      }
 - */
