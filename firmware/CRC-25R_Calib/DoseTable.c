/*********************************************************************
  MODULE:	DOSE TABLE

  FILE:		DoseTable.c

  DATE:		01/22/07

 *************************************************************************/
        
#include "crc.h"
#include "i2c.h"
#include "printer.h"
#include "screen.h"
#include "uart.h"
#include "keyboard.h"
#include "message.h"
#include "nuc.h"
#include <string.h>

	extern const float unitfact[];
    extern CHAMBER chamber[];
    extern MEASUREMENT measurement[];
    extern CURRENT current;
    extern time_t clock_time;
    extern time_t low_clock_time;

static void new_day(struct tm *tmdate);
static void enter_dose(float *activ, short *cal_unit);
static void prt_dose_table(short d_index,float act0, float dose, 
                           float vol, time_t act_time, char prtype);

typedef struct dosedelta DOSEDELTA;
struct dosedelta
{
    char    nucname[6];
    short   delta;
};

static const DOSEDELTA dose_delta[] =
    {
        "Tc99m",30,
        "In111",75,
        "Ga67",75,
        "I 123",75,
        "I 131",300,
        "Tl201",75
    };    
    
    void dose_table(void)
	{

		time_t act_time;
        time_t nowtime;
		float act0;
		float dose;
		float dose_in;
		float vol;
		short dose_unit;
        short ch_num = current.main_chamber;
        uchar prtype;
        //short kun;
        short i;
        char name[8];
        short d_index;
        short num;


        //is nuclide on dose_delta list?
        NuclideData_getName(chamber[ch_num].nuc_index,name);
        d_index = -1;
        for(i = 0; i < 6; i++)
        {
            num = strlen((char *)&dose_delta[i].nucname);
            if(strncmp(name,&dose_delta[i].nucname[0],num) == 0)
            {
                d_index = i;
                break;
            }    
        }

        if(d_index == -1)
        {
            //not a dose table nuclide
            beep();
            return;
        }    
        
		/* set up printing */
        //EE_READ(print[0],&prtype);
        prtype = current.printer;

		if(!start_printer(prtype,1,FALSE,PAPER))
		{
			 beep();
			 return;
		}

		 /* get current time */
		 read_clock(&nowtime);
         clock_time = nowtime;
         low_clock_time = clock_time;
         reset_minute_counter_with_seconds();
         act_time = nowtime;

		 /* starting activity is measured activity */
	 // Changed:
         //kun = measurement[ch_num].kun;
		 //act0 = measurement[ch_num].act / unitfact[kun - 1];
		 act0 = measurement[ch_num].act0;

		 erase_screen();
		/* get dose */		
		enter_dose(&dose_in,&dose_unit);
        if(home_set())
            return;
        
		dose = dose_in / unitfact[dose_unit - 1];

		/* get volume in ml */
        erase_screen();
        //display_text(0,12,"ENTER VOLUME",0,MEDIUM,NORMAL);
        //display_text(0,22,"IN ml",0,MEDIUM,NORMAL);
        display_medium_message(EFF_12,12,0,NORMAL);
        display_medium_message(LAB_15,22,0,NORMAL);
	    vol = inpflt(8,42,MEDIUM,0,4,1.0,100.0);
        if(home_set())
            return;
		
        erase_screen();
		//display_text(24,42,"PRINTING",0,MEDIUM,NORMAL);
        display_medium_message(DOSE_1,42,0,NORMAL);


		/* call routine to print the dose table */
		prt_dose_table(d_index,act0,dose,vol,act_time,prtype);

        erase_text_line(42,MEDIUM,0);
		nucinit(ch_num,TRUE);
		disp_nuclide();
        disp_cal(measurement[ch_num].response);
	
	}


static void enter_dose(float *activ, short *cal_unit)
	{

		short unit;
		float act;
		float act1;
		for(;;)  /* forever */
		{
            set_nokey();
			//display_text(16,2,"ENTER DOSE",0,MEDIUM,NORMAL);
            display_medium_message(DOSE_2,2,0,NORMAL);
			val_unit_msg();

            act = inpfl_u(8,30,5,&unit,4);
			act1 = act/unitfact[unit];
			// Locked: CI
			//if (current.system == CI && act1 > 10.01 ||
			//	current.system == BQ && act1 > 1.0e+12)
			if(act1 > 10.01)
			{
				beep();
				erase_screen();
				//display_text(8,32,"OUT OF RANGE",0,MEDIUM,NORMAL);
                display_medium_message(DOSE_3,32,0,NORMAL);
				contmsg();
				erase_screen();
				continue;
			}

			// Locked: CI
            //if(current.system == CI)
            //    *cal_unit = unit + 1;
            //else
            //    *cal_unit = unit + 5;
			*cal_unit = unit + 1;

			*activ = act;
			
			erase_screen();
			return;
		}
	}

void Amulet_printDoseTable(short nucindex, float act0, float dose, float vol, time_t act_time, time_t *timearray, float *volarray){
	char prtype;
	char str[20];
	char strng[90];
	char nucname[8];
	char actstr[12];
	time_t start_time;
	short nc;
	short index;
	char message[50];

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		//prt_dose_table(d_index, act0, dose, vol, act_time, prtype);

		start_time = act_time;

		prhead_languagesec(prtype);
        feed(1,prtype);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(TRUE);
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[4],"DOSE TABLE",10);
        //get_message(DOSE_4,&strng[4]);
		get_amulet_message(L_PR_DOSE_TABLE,message);    // "DOSE TABLE"
		strncpy(&strng[4],message,strlen(message));
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        feed(1,prtype);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(FALSE);

		/* print isotope */
        //strcpy(nucname,&dose_delta[d_index].nucname[0]);
		NuclideData_getName(nucindex, nucname);
        lininit(strng,TRUE,prtype);
		strncpy(&strng[5],nucname,strlen(nucname));
        pr_write(strng);

		/* print date / time */
        lininit(strng,TRUE,prtype);
        dateout_language(str,&act_time,4);
        strncpy(&strng[2],str,strlen(str));
        timeout(str,&start_time);
        strncpy(&strng[18],str,strlen(str));
        pr_write(strng);

        /* print Activity */
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[2],"Activity:",9);
        //get_message(DOSE_5,&strng[2]);
		get_amulet_message(L_ACTIVITY,message);    // "Activity"
		strcat(message,":");
		strncpy(&strng[2],message,strlen(message));
        // Locked: CI
        //format_activity(act0,current.system,actstr);
        //format_activity(act0, CI, actstr);
        format_activity_system(act0, actstr);
		strncpy(&strng[12],&actstr[0],9);
        pr_mu(&strng[18],prtype);
        pr_write(strng);

		/* print volume */
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[4],"Volume:",7);
        //get_message(DOSE_6,&strng[4]);
		get_amulet_message(L_VOLUME,message);    // "Volume:"
		strcpy(&strng[2],message);
        nc = sprintf(str,"% 6.1f",vol);
		strcat(strng,str);
		strcat(strng,"ml");
		strcat(strng,"\r\n");
		//strncpy(&strng[18 - nc],str,nc);
		//strncpy(&strng[18],"ml",2);
        pr_write(strng);

		/* print dose */
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[6],"Dose:",5);
        //get_message(DOSE_7,&strng[6]);
		get_amulet_message(L_DOSE,message);    // "Dose"
		strcat(message,": ");
		strncpy(&strng[2],message,strlen(message));
        // Locked: CI
        //format_activity(dose,current.system,actstr);
        //format_activity(dose, CI, actstr);
        format_activity_system(dose, actstr);
		strncpy(&strng[12],&actstr[0],9);
        pr_mu(&strng[18],prtype);
        pr_write(strng);
        feed(1,prtype);

		/* print heading */
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[2],"TIME",4);
		//strncpy(&strng[10],"VOL (ml)",8);
        //get_message(DOSE_8,&strng[2]);
		get_amulet_message(L_PR_TIME,message);    // "TIME"
		strncpy(&strng[2],message,strlen(message));
        //get_message(DOSE_9,&strng[10]);
		get_amulet_message(L_PR_VOLUME_HEADING,message);    // "VOLUME (ml)"
		strncpy(&strng[11],message,strlen(message));
		
        pr_write(strng);

        for(index=0; index<30; index++){
        	if((*timearray) == (time_t)0){
        		break;
        	}else{
        		lininit(strng,TRUE,prtype);
				timeout(str, timearray);
				strncpy(&strng[2],str,strlen(str));

				if((*volarray) == -999){
					//strncpy(&strng[10], "Exceeds Initial Vol", 19);
					get_amulet_message(L_PR_EXCEEDS_INITIAL,message);    // "Exceeds Initial Vol"
					strncpy(&strng[10],message,strlen(message));
					pr_write(strng);
					break;
				}else{
					nc = sprintf(str, "%6.2f", *volarray);
					strncpy(&strng[10], str, nc);
					pr_write(strng);
				}
				timearray++;
				volarray++;
        	}
        }

        formfeed(prtype);
	}
}

static void prt_dose_table(short d_index,float act0, float dose, 
						   float vol, time_t act_time, char prtype)

	{
		short delta;
        short start_mins;
		short now_mins;
		short add_mins;
        time_t start_time;
        time_t dose_time;
        struct tm tm_start;
        struct tm tm_act;
        struct tm tm_dose;
		char nucname[8];
		char actstr[12];
		char str[20];
		short i;
		float prt_act;
		float conc;
		float req_vol;
		char strng[90];
        short ch_num = current.main_chamber;
        short nc;
        
        start_time = act_time;
        memcpy(&tm_act,gmtime(&act_time),sizeof(tm_act));
        memcpy(&tm_start,gmtime(&start_time),sizeof(tm_start));        
        /* print heading */
		prhead_language(prtype);
        feed(1,prtype);
		
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(TRUE);
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[4],"DOSE TABLE",10);
        get_message(DOSE_4,&strng[4]);
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        feed(1,prtype);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(FALSE);

		/* print isotope */
        strcpy(nucname,&dose_delta[d_index].nucname[0]);
        lininit(strng,TRUE,prtype);
		strncpy(&strng[5],nucname,strlen(nucname));
        pr_write(strng);

		/* print date / time */
        lininit(strng,TRUE,prtype);
        dateout_language(str,&act_time,4);
        strncpy(&strng[2],str,strlen(str));
        timeout(str,&start_time);
        strncpy(&strng[18],str,strlen(str));
        pr_write(strng);
		
        start_mins = tm_act.tm_min;

		/* put minute interval into multiple of quarter hour */
		if(start_mins < 15)
			add_mins = 15;
		if(start_mins < 30 && start_mins >= 15)
			add_mins = 30;
		if(start_mins < 45 && start_mins >= 30)
			add_mins = 45;
		if(start_mins > 45)
			add_mins = 0;

		if(add_mins == 0)
		{
			
            if(tm_start.tm_hour == 23)
			{
                tm_start.tm_hour = 0;
				new_day(&tm_start);
			}
			else
				/* increment hours */
                ++tm_start.tm_hour;
		}
        tm_start.tm_min = add_mins;


		/* print Activity */
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[2],"Activity:",9);
        get_message(DOSE_5,&strng[2]);
        // Locked: CI
        //format_activity(act0,current.system,actstr);
        format_activity(act0, CI, actstr);
		strncpy(&strng[12],&actstr[0],9);
        pr_mu(&strng[18],prtype);
        pr_write(strng);

		/* print volume */
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[4],"Volume:",7);
        get_message(DOSE_6,&strng[4]);
        nc = sprintf(str,"%6.1f",vol);
		strncpy(&strng[18 - nc],str,nc);
		strncpy(&strng[18],"ml",2);
        pr_write(strng);

		/* print dose */
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[6],"Dose:",5);
        get_message(DOSE_7,&strng[6]);
        // Locked: CI
        //format_activity(dose,current.system,actstr);
        format_activity(dose, CI, actstr);
		strncpy(&strng[12],&actstr[0],9);
        pr_mu(&strng[18],prtype);
        pr_write(strng);
        feed(1,prtype);

		/* print heading */
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[2],"TIME",4);
		//strncpy(&strng[10],"VOL (ml)",8);
        get_message(DOSE_8,&strng[2]);
        get_message(DOSE_9,&strng[10]);
        pr_write(strng);

        delta = dose_delta[d_index].delta;
		
        dose_time = start_time;

		/* print the table */
		for(i = 0; i < 30; i++)
		{
			/* decay activity to dose_time */
			prt_act = nucdecay(act0,start_time,dose_time,
							   chamber[ch_num].nucdata.halflife,chamber[ch_num].nucdata.hlunit);

			conc = prt_act / vol;
			req_vol = dose / conc;

			if(req_vol > vol)
			{
                lininit(strng,TRUE,prtype);
				//strncpy(&strng[2],"Required volume greater",23);
                get_message(DOSE_10,&strng[2]);
                pr_write(strng);
                lininit(strng,TRUE,prtype);
				//strncpy(&strng[2],"than existing volume.",21);
                get_message(DOSE_11,&strng[2]);
                pr_write(strng);
				break;
			}

            lininit(strng,TRUE,prtype);
            timeout(str,&dose_time);
            strncpy(&strng[2],str,strlen(str));
            nc = sprintf(str,"%6.2f",req_vol);
			strncpy(&strng[10],str,nc);
            pr_write(strng);

			/* get new time */
            memcpy(&tm_dose,gmtime(&dose_time),sizeof(tm_dose));
            tm_dose.tm_isdst = -1;
            tm_dose.tm_min += delta;
            if(tm_dose.tm_min >= 60)
			{
                now_mins = tm_dose.tm_min % 60;
				tm_dose.tm_hour += tm_dose.tm_min / 60;
                tm_dose.tm_min = now_mins;
                if(tm_dose.tm_hour >= 24)
				{
                    tm_dose.tm_hour -= 24;
					new_day(&tm_dose);
				}
			
			}
            dose_time = mk_time(&tm_dose);

		}
        formfeed(prtype);


	}

    extern const short month_days[];
static void new_day(struct tm *tmdate)
	{
        short mval[13];
        short mon;
        short day;
        short year;

        memcpy(mval,month_days,sizeof(mval));

		mon = tmdate -> tm_mon;
		day = tmdate -> tm_mday;
		year = tmdate -> tm_year;

        mval[2] = 28;
		if(year % 4 == 0)
			mval[2] = 29;

		++day;
		if(day > mval[mon + 1])
		{
			++mon;
			day = 1;
			if(mon == 13)
			{
				mon = 1;
				++year;
			}
		}
        tmdate -> tm_mon = mon;
        tmdate -> tm_mday = day;
        tmdate -> tm_year = year;

	}




