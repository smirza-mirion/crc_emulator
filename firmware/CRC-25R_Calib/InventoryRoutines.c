/*********************************************************************

  MODULE:	 INVENTORY ROUTINES 

  FILE:		InventoryRoutines.c

  DATE:		08/21/07

  ANALYSIS:	Routines for inventory

  CALLED BY:
		utilities


*************************************************************************/
#include "crc.h"
#include "menu.h"
#include "screen.h"
#include "i2c.h"
#include "printer.h"
#include "uart.h"
#include "keyboard.h"
#include "message.h"
#include "nuc.h"
#include <string.h>

static short get_study(bool none_flag);
float get_current_act(INVENTORY *inv, NUCDATA *nuc, time_t *nowtime);
static void enter_activity(float *activ, short *cal_unit);
static void	delete_from_inventory(void);
static void	withdraw_from_inventory(void);
static void	print_inventory(void);
static void	print_inventorysec(void);
static short inventory_list(bool tc_flag, bool del_flag);
static void display_1_inv(short *inv_list, short i,short line);
static void add_to_inventory(void);
static void make_kit(void);
static void display_inventory(INVENTORY *inv,NUCDATA *nuc, bool update_flag, bool date_flag);
void write_binary_to_Options_folder(char *acFilename, char *acData, long int lFilelength);
float update_motc(INVENTORY *inv);

extern time_t clock_time;
extern time_t low_clock_time;

//NOTE: this must be in memory which will not be initialized on power-up
//#pragma ghs section bss="savesect"
INVENTORY  inventory[MAX_INVENTORY];

    void inventory_menu(void)
	{
        short iret;

        for(;;)
        {    

            iret = display_menu(INVENTORY_MENU);
            if(home_set())
                return;
            
    
            switch (iret)
            {
            case -1:  //UTIL key
                return;
            case 1:
                add_to_inventory();
                break;
            case 2:
                make_kit();
                break;
            case 3:
                withdraw_from_inventory();
                break;
            case 4:
                print_inventory();
                break;
            case 5:
                delete_from_inventory();
                break;
    
            }
            
            if(home_set())
                return;
        }

	}


	extern CURRENT current;
    extern MEASUREMENT measurement[];
	extern const float unitfact[];
const char *study_type[] =
{
    "BONE ",
    "LUNG ",
    "HIDA ",
    "HEART",
    "RENAL",
    "LIVER",
    "BRAIN",
    "LYMPH"
};


static void	add_to_inventory(void)
	{
			
		short i;		
		INVENTORY inv;
		short index;
		short nuc_index;
		short study;   
		short id;
		float vol;
		float motc;
		short act_unit;
		bool date_flag;
        time_t nowtime;
        struct tm tm_date;
        NUCDATA nucdata;
        short tc_index;
        char nucname[8];
        //char kun;
        float act;
        short ch_num = current.main_chamber;
        bool yn;
        bool dret;
        char acNuclideName[7];
        
        erase_screen();
        for(;;)
		{
			/*
				find empty slot for inventory 
			if none
				error message, return
			*/
			index = -1;	 // assume no room
			for (i = 0; i < MAX_INVENTORY; i++)
			{
				if (inventory[i].nucnum == -1)
				{
					index = i;		/* is empty, use this one */
					break;
				}
			}
	
			if (index == -1)		/* no slot found */
			{
				/* error message -- no room */
				//display_text(8,12,"NO ROOM TO ADD",0,MEDIUM,NORMAL);
                display_medium_message(NO_ROOM,12,0,NORMAL);
				contmsg();
                if(home_set())
                    return;
				erase_screen();
				return;
			}


			/* user selects nuclide */
            //nuc_index = get_nuclide(FALSE,"INVENTORY");
            nuc_index = get_nuclide(FALSE,INV_6);

			if(nuc_index == -1)
				return;

			NuclideData_getNuclide(nuc_index,&nucdata);
			NuclideData_getName(nuc_index, acNuclideName);
            if(NuclideData_getResponse(nuc_index, 0) == 0.0)
            {
                no_calnum_msg();
                return;
            }    

			inv.nucnum = nuc_index;
            //get nuclide index for Tc99m
            strcpy(nucname,"Tc99m");
            tc_index = NuclideData_getIndexFromName(nucname);
           
			if(nuc_index == tc_index) 
			{
				study = get_study(TRUE);
                if(home_set())
                    return;

				if(study == 0)
					inv.type = -1 ;
				else
					inv.type = study - 1;
			}
			else
				inv.type = -1;

			/* enter optional ID */
			erase_screen();
			//display_text(0,2,"ENTER ID",0,MEDIUM,NORMAL);
			//display_text(0,12,"0 FOR NONE",0,MEDIUM,NORMAL);
            display_medium_message(INV_1,2,0,NORMAL);
            display_medium_message(INV_2,12,0,NORMAL);
            id = inpint(8,52,MEDIUM,0,2,0,99,'\0');
            if(home_set())
                return;
            
			if(id == 0)
				inv.id = -1;
			else
				inv.id = id;

			
			/* enter optional lot number */
			erase_screen();
			//display_text(0,2,"ENTER LOT",0,MEDIUM,NORMAL);
			//display_text(0,12,"NUMBER",0,MEDIUM,NORMAL);
            display_medium_message(INV_3,2,0,NORMAL);
            display_medium_message(INV_4,12,0,NORMAL);
            (void)inpalnum(8,42,&inv.lot[0],15,FALSE);
            if(home_set())
                return;

			/* enter volume */
            erase_screen();
            //display_text(0,12,"ENTER VOLUME",0,MEDIUM,NORMAL);
            //display_text(0,22,"IN ml",0,MEDIUM,NORMAL);
            display_medium_message(EFF_12,2,0,NORMAL);
            display_medium_message(LAB_15,12,0,NORMAL);
            vol = inpflt(8,42,MEDIUM,0,4,1.0,100.0);	
            if(home_set())
                return;
			inv.vol = vol;

			/* measure or enter activity */
			erase_screen();
			//display_text(8,12,"MEASURE",0,MEDIUM,NORMAL);
            //display_text(8,22,"ACTIVITY ?",0,MEDIUM,NORMAL);
            display_medium_message(ACC_3,12,0,NORMAL);
            display_medium_message(INV_5,22,0,NORMAL);
            display_medium_message(YES_OR_NO,34,0,NORMAL);
            yn = yesorno();
            if(home_set())
                return;
            
            read_clock(&nowtime);
			if(yn)
			{
				measure_activity(acNuclideName);
                if(home_set())
                    return;
                
                clock_time = nowtime;
                low_clock_time = clock_time;
                reset_minute_counter_with_seconds();
		// Changed:
                //kun = measurement[ch_num].kun;
                //act = measurement[ch_num].act;
				//inv.act = act / unitfact[kun - 1];
		inv.act = measurement[ch_num].act0;
				// Locked: CI
				//inv.syst = current.system;
				// Removed:
				//inv.syst = CI;
                inv.date = nowtime;
				date_flag = FALSE;
			}
			else
			{
				erase_screen();
				enter_activity(&act,&act_unit);
                if(home_set())
                    return;
                
				inv.act = act / unitfact[act_unit - 1];
				erase_screen();
                //Enter Date
                display_medium_message(DATE_TIME_IN_1,2,0,NORMAL);
                
                //display_text(30,48,"ENTER for NOW",0,SMALL,NORMAL);
                display_small_message(DATE_TIME_IN_9,16,0,NORMAL);
                
                dret = date_in(8,26,&tm_date);
                if(home_set())
                    return;

                if(!dret)
                    inv.date = nowtime;
                else
                {    
                    erase_screen();
                    //Enter Time
                    display_medium_message(DATE_TIME_IN_2,10,0,NORMAL);
                    //display_text(30,56,"hhmm",0,SMALL,NORMAL);
                    display_small_message(DATE_TIME_IN_7,56,0,NORMAL);
                    
                    time_in(8,24,&tm_date);
                    if(home_set())
                        return;
                    tm_date.tm_isdst = -1;
                    inv.date = mk_time(&tm_date);
                }
				date_flag = TRUE;

			}
			// Locked: CI
			//inv.syst = current.system;
			// Removed:
			//inv.syst = CI;

			/* if Tc, Enter mopertc */
			if(nuc_index == tc_index)
			{
                erase_screen();
                //display_text(0,2,"ENTER Mo/Tc",0,MEDIUM,NORMAL);
                //display_text(0,12,"0 FOR NONE",0,MEDIUM,NORMAL);
                display_medium_message(INV_7,2,0,NORMAL);
                display_medium_message(INV_2,12,0,NORMAL);
                motc = inpflt(8,42,MEDIUM,0,4,0.,1.0);					
                if(home_set())
                    return;
                inv.mopertc = motc;
			}
			
			display_inventory(&inv,&nucdata,FALSE,date_flag);
			//display_text(16,52,"OK?  Y OR N",0,MEDIUM,NORMAL);
            display_medium_message(OK_MSG,52,0,NORMAL);
            yn = yesorno();
            if(home_set())
                return;
            if(yn)
			{
				memcpy(&inventory[index],&inv,sizeof(inv));
				return ;
			}
			erase_screen();
		} /* end of forever */



	}

	static void display_inventory(INVENTORY *inv,NUCDATA *nuc, bool update_flag,
					  bool date_flag)
	{
		char index;
		char str[20];
        char str1[12];
		short nc;
		float motc;
		float act_inv;
        time_t nowtime;
        short i;
        short tc_index;
        short ypos;

		erase_screen();
        ypos = 0;
		display_text(6,ypos,&nuc -> name[0],0,SMALL,NORMAL);
		if(inv -> type != -1)
		{
			index = inv -> type;
			display_text(64,ypos,(char *)study_type[index],0,SMALL,NORMAL);
		}

		if(inv -> id != -1)
		{
            (void)sprintf(str,"%0d",inv -> id);
			display_text(96,ypos,str,0,SMALL,NORMAL);
		}

        ypos += 8;
		//display_text(0,8,"Lot:",0,SMALL,NORMAL);
        display_small_message(INV_8,8,0,NORMAL);
		display_text(30,ypos,&inv -> lot[0],0,SMALL,NORMAL);

        ypos += 8;
        display_text(0,ypos,"Volume  :",0,SMALL,NORMAL);
        display_small_message(INV_19,16,0,NORMAL);
        nc = sprintf(str,"%5.1f",inv -> vol);
		display_text(6 *(15 - nc),ypos,str,0,SMALL,NORMAL);
		display_text(90,ypos,"ml",0,SMALL,NORMAL);

        ypos += 8;
        //display_text(0,24,"Activity:",0,SMALL,NORMAL);
        display_small_message(INV_10,ypos,0,NORMAL);

        if(update_flag)
			act_inv = get_current_act(inv,nuc,&nowtime);
		else
			act_inv = inv -> act;

        // Removed:
		//if(inv -> syst == CI && act_inv > 10.01 ||
		//   inv -> syst == BQ && act_inv > 1.0e+12)
		if(act_inv > 10.01)
		{
			//display_text(54,24,"OVER",0,SMALL,NORMAL);
            display_small_message(INV_11,ypos,0,NORMAL);
		}
		else
		{
			// Removed:
            //format_activity(act_inv,inv -> syst,str);
			format_activity(act_inv, CI,str);
            //display_act_small(str,9,3);
            display_act_small(str,9,ypos / 8);
		}

		/* display current Mo/Tc if it exists */
        //get Tc99m index
        ypos += 8;
        tc_index = NuclideData_getIndexFromName("Tc99m");
		if(inv -> nucnum == tc_index && inv -> mopertc != 0)
		{
			display_text(0,ypos,"Mo/Tc:",0,SMALL,NORMAL);
			motc = inv -> mopertc;
			if(update_flag)
				motc = update_motc(inv);
			if(motc > 0)
			{
                (void)sprintf(str,"%05.3f",motc);
				display_text(42,ypos,str,0,SMALL,NORMAL);
			}
		}

        ypos += 8;
		if(date_flag)
		{
			for(i = 0; i < 18; i++)
				str[i] = ' ';

            dateout_language(&str1[0],&inv -> date,4);
            strncpy(str,str1,strlen(str1));
            timeout(&str[12],&inv -> date);
			display_text(0,ypos,str,0,SMALL,NORMAL);

		}


	}



	
static short get_study(bool none_flag)
	{
		char str[22];
		short i,j;
		short num;
		short min_num;

		/* display studies */
		erase_screen();
		//display_text(12,0,"SELECT STUDY",0,SMALL,NORMAL);
        display_small_message(INV_12,0,0,NORMAL);
		if(none_flag)
		{
			//display_text(6,16,"0. NONE",0,SMALL,NORMAL);
            display_small_message(INV_13,16,0,NORMAL);
			min_num = 0;
		}
		else
			min_num = 1;
		for(j = 1; j <= 4; j++)
		{
			for(i = 0; i < 20; i++)
				str[i] = ' ';
            str[20] = '\0';
			str[0] = j + '0';
			str[1] = '.';
			strncpy(&str[3],study_type[j - 1],5);
			str[10] = j + '4';
			str[11] = '.';
			strncpy(&str[13],study_type[j + 3],5);
            display_text(6, 8 * (j + 2),str,0,SMALL,NORMAL);
		}

		num = choose(min_num,8);

		return(num);
		
	}	

 
float get_timepoint_act(INVENTORY *inv, NUCDATA *nuc, time_t *timepoint){
	float timepoint_activity;

	timepoint_activity = nucdecay(inv->act,
								  inv->date,
								  *timepoint,
								  nuc->halflife,
								  nuc->hlunit);
	return timepoint_activity;
}

float get_current_act(INVENTORY *inv, NUCDATA *nuc, time_t *nowtime)
	{
		float curr_act;

		 read_clock(nowtime);
         clock_time = *nowtime;
         low_clock_time = clock_time;
         reset_minute_counter_with_seconds();
	
		 curr_act = nucdecay(inv -> act,
						 inv -> date,
						 *nowtime,nuc -> halflife,
						 nuc -> hlunit);
	
		 return(curr_act);
	}

static void enter_activity(float *activ, short *cal_unit)
	{

		short unit;
		float act;
		float act1;

		 for(;;)  /* forever */
		 {
			 set_nokey();
             //display_text(8,2,"ENTER ACTIVITY",0,MEDIUM,NORMAL);
             display_medium_message(INV_14,2,0,NORMAL);
			 val_unit_msg();
             act = inpfl_u(8,27,5,&unit,4);
             // Locked: CI
             //if(current.system == BQ)
             //    unit += 4;
			 act1 = act/unitfact[unit];

			 // Locked: CI
			 //if (current.system == CI && act1 > 10.01 ||
			 //	 current.system == BQ && act1 > 1.0e+12 )
			 if (act1 > 10.01)
			 {
				 beep();
				 erase_screen();
				 //display_text(8,32,"OUT OF RANGE",0,MEDIUM,NORMAL);
                 display_medium_message(DOSE_3,32,0,NORMAL);
				 contmsg();
				 erase_screen();
				 continue;
			 }
	
             //if(current.system == CI)
             *cal_unit = unit + 1;
             //else
            //     *cal_unit = unit + 5;
			 *activ = act;
	
			 erase_screen();
			 return;
		 }
	}


    static void make_kit(void)
	{
		short inv_index;
		short nuc_index;
		short kit_index;
		short i;
		short num;
		short study;
		INVENTORY inv;
		INVENTORY kit_inv;
		short id;
		float draw_act;
		float req_vol;
		float conc;
		float now_act;
		float drawn_vol;
		float rem_vol;
		float meas_act;
		float rem_act;
		float curr_motc;
        time_t act_dt;
        time_t nowtime;
		char str[20];
		short act_unit;
		float kit_vol;
        NUCDATA  nucdata;
        short nc;
        //float act;
        //char kun;
        short tc_index;
        short ch_num = current.main_chamber;
        bool ynret;

		/*
		find empty slot for kit 
		if none
		error message, return
		*/
		kit_index = -1;	/* initialize to no room*/
		for (i = 0; i < MAX_INVENTORY; i++)
		{
			if (inventory[i].nucnum == -1)
			{
				kit_index = i;		/* is empty, use this one */
				break;
			}
		}

		if (kit_index == -1)		/* no slot found */
		{
			/* error message -- no room */
			//display_text(8,12,"NO ROOM TO",0,MEDIUM,NORMAL);
			//display_text(8,22,"ADD KIT",0,MEDIUM,NORMAL);
            display_medium_message(INV_15,12,0,NORMAL);
            display_medium_message(INV_16,22,0,NORMAL);
			contmsg();
            if(home_set())
                return;
			erase_screen();
			return;
		}

        //get Tc99m index
        tc_index = NuclideData_getIndexFromName("Tc99m");
        
		for(;;)
		{
			/* get # of bare Tc items on list */
			num = 0;
			for(i = 0; i < MAX_INVENTORY; i++)
			{
				if(inventory[i].nucnum == tc_index && 
				   inventory[i].type == -1)
				{
					++num;
					if(num == 1)
						inv_index = i;
				}
			}
			if(num == 0)
			{
				beep();
				erase_screen();
				//display_text(0,12,"NO TC FOR KITS",0,MEDIUM,NORMAL);
				//display_text(8,22,"IN INVENTORY",0,MEDIUM,NORMAL);
                display_medium_message(INV_17,12,0,NORMAL);
                display_medium_message(INV_18,22,0,NORMAL);
				contmsg();
				return;
			}

			if(num > 1)
			{
				inv_index = inventory_list(TRUE,FALSE);
				if(inv_index == -1 || home_set())
					return;
			}

			/* copy to local inventory structure */
			memcpy(&inv,&inventory[inv_index],sizeof(inv));

			// Locked: CI
            //if(current.system != inv.syst)
            //{
            //    erase_screen();
            //    if(inv.syst == CI)
            //    {
/*                  display_text(0,8,"Ci Must Be Selected",0,SMALL,NORMAL);
 -                  display_text(0,16,"On Main Screen",0,SMALL,NORMAL);
 -                  display_text(0,24,"To Withdraw From",0,SMALL,NORMAL);
 -                  display_text(0,32,"Item Entered in Ci",0,SMALL,NORMAL);*/
            //        display_small_message(INV_32,8,0,NORMAL);
            //        display_small_message(INV_33,16,0,NORMAL);
            //        display_small_message(INV_34,24,0,NORMAL);
            //        display_small_message(INV_35,32,0,NORMAL);
            //    }
            //    else
            //    {
/*                  display_text(0,8,"Bq Must Be Selected",0,SMALL,NORMAL);
 -                  display_text(0,16,"On Main Screen",0,SMALL,NORMAL);
 -                  display_text(0,24,"To Withdraw From",0,SMALL,NORMAL);
 -                  display_text(0,32,"Item Entered in Bq",0,SMALL,NORMAL);*/
            //        display_small_message(INV_36,8,0,NORMAL);
            //        display_small_message(INV_33,16,0,NORMAL);
            //        display_small_message(INV_34,24,0,NORMAL);
            //        display_small_message(INV_37,32,0,NORMAL);
            //    }
            //    contmsg();
            //    return;
            //}
            

            //nuclide is Tc99m
            nuc_index = tc_index;
			NuclideData_getNuclide(nuc_index,&nucdata);
	
			display_inventory(&inv,&nucdata,TRUE,FALSE);
			//display_text(0,52,"OK? Y OR N",0,MEDIUM,NORMAL);
            display_medium_message(OK_MSG,52,0,NORMAL);
            
			
            ynret = yesorno();
            if(home_set())
                return;
            if(!ynret)
			{
				if(num == 1)
					return;
				continue;
			}

			/* user selects study */
			study = get_study(FALSE);
            if(home_set())
                return;
            
			kit_inv.type = study - 1;

            //nucnum is Tc
            kit_inv.nucnum = tc_index;

			/* enter optional ID */
			erase_screen();
			//display_text(0,2,"ENTER ID",0,MEDIUM,NORMAL);
			//display_text(0,12,"0 FOR NONE",0,MEDIUM,NORMAL);
            display_medium_message(INV_1,2,0,NORMAL);
            display_medium_message(INV_2,12,0,NORMAL);
            id = inpint(8,52,MEDIUM,0,2,0,99,'\0');
            if(home_set())
                return;
            
			if(id == 0)
				kit_inv.id =  -1 ;
			else
				kit_inv.id = id;
			
			/* enter optional lot number */
			erase_screen();
			//display_text(0,2,"ENTER LOT",0,MEDIUM,NORMAL);
            //display_text(0,12,"NUMBER",0,MEDIUM,NORMAL);
            display_medium_message(INV_3,2,0,NORMAL);
            display_medium_message(INV_4,12,0,NORMAL);
            (void)inpalnum(8,42,&kit_inv.lot[0],15,FALSE);
            if(home_set())
                return;

			/* required activity */
			for(;;)
			{
				erase_screen();
				enter_activity(&draw_act,&act_unit);
                if(home_set())
                    return;
				draw_act /=  unitfact[act_unit - 1];
				
				read_clock(&act_dt);
                clock_time = act_dt;
                low_clock_time = clock_time;
                reset_minute_counter_with_seconds();
				now_act = nucdecay(inv.act,inv.date,
								   act_dt,nucdata.halflife,
								   nucdata.hlunit);
				
				if(draw_act < now_act)
					break;

				/*draw_act >= now_act*/
				beep();
				erase_screen();
				//display_text(0,12,"ACTIVITY GREATER",0,MEDIUM,NORMAL);
				//display_text(0,22,"THAN THAT IN",0,MEDIUM,NORMAL);
				//display_text(0,32,"INVENTORY",0,MEDIUM,NORMAL);
                display_medium_message(INV_19,12,0,NORMAL);
                display_medium_message(INV_20,22,0,NORMAL);
                display_medium_message(INV_6,32,0,NORMAL);
                contmsg();
                if(home_set())
                    return;

			}

			conc = now_act / inv.vol;
			req_vol = draw_act / conc;

			erase_screen();
			display_text(6,0,&nucdata.name[0],0,SMALL,NORMAL);

			if(inv.id != -1)
			{
                (void)sprintf(str,"%02d",inv.id);
				display_text(96,0,str,0,SMALL,NORMAL);
			}

			//display_text(0,16,"Activity:",0,SMALL,NORMAL);
            display_small_message(INV_10,16,0,NORMAL);
            // Removed:
			//if(inv.syst == CI && draw_act > 10.01 ||
			//   inv.syst == BQ && draw_act > 1.0e+12)
			if(draw_act > 10.01)
                //display_text(54,16,"OVER",0,SMALL,NORMAL);
                display_small_message(INV_11,16,0,NORMAL);
			else
			{
				// Removed:
				//format_activity(draw_act,inv.syst,str);
				format_activity(draw_act, CI, str);
                display_act_small(str,9,2);
			}


			//DIsplay_text(0,32,"Withdraw:",0,SMALL,NORMAL);
            display_small_message(INV_21,32,0,NORMAL);
            nc = sprintf(str,"%5.1f",req_vol);
			display_text(6 * (15 - nc),32,str,0,SMALL,NORMAL);
			display_text(90,32,"ml",0,SMALL,NORMAL);
			contmsg();
            if(home_set())
                return;

			/* measure activity */
			erase_screen();
			//display_text(8,12,"MEASURE",0,MEDIUM,NORMAL);
			//display_text(8,22,"ACTIVITY",0,MEDIUM,NORMAL);
            display_medium_message(LAB_20,12,0,NORMAL);
            display_medium_message(INV_22,22,0,NORMAL);
			contmsg();
            if(home_set())
                return;
			measure_activity(&nucdata.name[0]);
            if(home_set())
                return;
	    // Changed:
            //act = measurement[ch_num].act;
            //kun = measurement[ch_num].kun;
			//meas_act =  act / unitfact[kun - 1];
	    meas_act = measurement[ch_num].act0;


			/* kit activity is meas_act */
			kit_inv.act = meas_act;

			/* calculate withdrawn volume from measured activity */
			now_act = get_current_act(&inv,&nucdata,&nowtime);
			conc = now_act / inv.vol;
			drawn_vol = meas_act / conc;

			/* get remaining volume & activity & save */
			rem_vol = inv.vol - drawn_vol;
			rem_act = now_act - meas_act;
			inv.vol = rem_vol;
			inv.act = rem_act;

            /* save date/time read in get_current_act */
            inv.date = nowtime;
            
			/* update Mo/Tc if it has been entered */
			if(inv.mopertc == 0)
				kit_inv.mopertc = 0;
			else
			{
				curr_motc = update_motc(&inv);
				inv.mopertc = curr_motc;
				/* this is also Mo/Tc for kit */
				kit_inv.mopertc = curr_motc;
			}


			/* save inv to inventory */
			memcpy(&inventory[inv_index],&inv,sizeof(inv));
			

			/* user enters kit volume */
			erase_screen();
			//display_text(0,12,"ENTER KIT",0,MEDIUM,NORMAL);
			//display_text(0,22,"VOLUME IN ml",0,MEDIUM,NORMAL);
            display_medium_message(INV_23,12,0,NORMAL);
            display_medium_message(LAB_15,22,0,NORMAL);
            kit_vol = inpflt(8,42,MEDIUM,0,4,0.1,9999.);
            if(home_set())
                return;
			
			kit_inv.vol = kit_vol;
            kit_inv.date = nowtime;
            // Locked: CI
			//kit_inv.syst = current.system;
            // Removed:
			//kit_inv.syst = CI;


			/* save kit to inventory */
			memcpy(&inventory[kit_index],&kit_inv,sizeof(kit_inv));

			return;

		}
	}

static void	delete_from_inventory(void)
	{

		short inv_index;
		short nuc_index;
		short i;
		short num;
        NUCDATA nucdata;
        bool yn;
		
        num = display_menu(INVENTORY_DELETE_MENU);
        if(home_set())
            return;
		if(num == 1)
		{
			erase_screen();
            //display_text(0,22,"PRINT INVENTORY?",0,MEDIUM,NORMAL);
            display_medium_message(INV_24,22,0,NORMAL);
            //display_text(0,32,"Y OR N",0,MEDIUM,NORMAL);
            display_medium_message(YES_OR_NO,32,0,NORMAL);
            yn = yesorno();
            if(home_set())
                return;
            
			if(yn)
			{
				erase_screen();
				print_inventory();
			}
			erase_screen();
/*            display_text(8,12,"ARE YOU SURE",0,MEDIUM,NORMAL);
 -             display_text(8,22,"YOU WANT TO",0,MEDIUM,NORMAL);
 -             display_text(8,32,"DELETE WHOLE",0,MEDIUM,NORMAL);
 -             display_text(8,42,"INVENTORY ?",0,MEDIUM,NORMAL);*/
            display_medium_message(INV_25,12,0,NORMAL);
            display_medium_message(INV_26,22,0,NORMAL);
            display_medium_message(INV_27,32,0,NORMAL);
            display_medium_message(INV_28,42,0,NORMAL);
            
			if(yesorno())
			{
			   for(i = 0; i < MAX_INVENTORY; i++)
				   inventory[i].nucnum = -1;
			}
			return;
		}
		
		for(;;)
		{
			inv_index = inventory_list(FALSE,TRUE);
	
			if(inv_index == -1)
				return;
	
			nuc_index = inventory[inv_index].nucnum;
			NuclideData_getNuclide(nuc_index,&nucdata);
	
			display_inventory(&inventory[inv_index],&nucdata,1,0);
            //display_text(0,52,"Delete? Y or N",0,MEDIUM,NORMAL);
            display_medium_message(ADD_NUCLIDE_8,52,0,NORMAL);
            
			
			if(yesorno())
			{
				/* delete by setting nucnum to NONE */
				inventory[inv_index].nucnum = -1;
			}
            if(home_set())
                return;
		}
	}

static short inventory_list(bool tc_flag, bool del_flag)
	{

		short i,i2;
		short i1;
		short k;
		bool start_flag;
		short maxinv;
		short inv_list[30];
		short scr_num;
		char ch;
		bool item_flag;
        short tc_index = NuclideData_getIndexFromName("Tc99m");

		maxinv = 0;
		for(i = 0; i < 30; i++)
			inv_list[i] = -1;

		for(i = 0; i < 30; i++)
		{
			item_flag = (inventory[i].nucnum != -1);
			
			if(item_flag && tc_flag)
			{
				item_flag = ((inventory[i].nucnum == tc_index &&
								inventory[i].type == -1));
			}

			if(item_flag)
			{

				inv_list[maxinv] = i;
				++maxinv;

			}
		}

		/* case of no inventory */
		if(maxinv == 0)
		{
			erase_screen();
            //display_text(0,24,"Inventory is Empty",0,SMALL,0);
            display_small_message(INV_29,24,0,NORMAL);
			contmsg();
			return(-1);
		}


		i1 = 0;
		i2 = 5;
		if(i2 > maxinv)
			i2 = maxinv;
		scr_num = 0;
		for(;;)
		{
			erase_screen();
			if(del_flag)
                //display_text(42,0,"DELETE",0,SMALL,NORMAL);
                display_small_message(INV_30,0,0,NORMAL);
			else
                //display_text(42,0,"SELECT",0,SMALL,NORMAL);
                display_small_message(INV_31,0,0,NORMAL);

			if(i2 < maxinv)
                //display_text(0,52,"DOWN: Next",0,SMALL,NORMAL);
                display_small_message(GEOM_5,52,0,NORMAL);
			if(scr_num > 0)
                //display_text(72,52,"UP: Prev",0,SMALL,NORMAL);
                display_small_message(GEOM_6,52,0,NORMAL);
			for(i = i1; i < i2; i++)
				display_1_inv(inv_list,i,i - i1 + 1);
			do
			{
				ch = keyin();
                if(home_set())
                    return (-1);
				start_flag = FALSE;
				switch (ch)
				{
				case MENUBUT:
					return (-1);
				case ONE:
				case TWO:
				case THREE:
				case FOUR:
				case FIVE:
					k = ch -1 + 5 * scr_num;
					if(inv_list[k] == -1)
						beep();
					else
						return(inv_list[k]);
					break;


				case UP:
					if (scr_num == 0)
					{
						beep();
						break;
					}
					i1 -= 5;
					if(i1 < 0)
						i1 = 0;
					i2 = i1 + 5;
					if(i2 > maxinv)
						i2 = maxinv;
					--scr_num;
					start_flag = TRUE;
					break;

				case DOWN:
					if(i2 == maxinv)
					{
						beep();
						break;
					}
					i1 += 5;
					i2 = i1 + 5;
					if(i2 > maxinv)
						i2 = maxinv;
					++scr_num;
					start_flag = TRUE;
					break;

				default:
					beep();
					break;

				}
			}while(!start_flag);
		}

	}


static void display_1_inv(short *inv_list, short i,short line)
	{
		short nc;
		short nuc_index;
		short k;
		char buffer[10];
		char nucname[6];
		INVENTORY inv;
        char strng[30];


		strcpy(strng,"  .                 ");
        nc = sprintf(buffer,"%d",line);
		strncpy(&strng[2 - nc],buffer,nc);
		k = inv_list[i];
		memcpy(&inv,&inventory[k],sizeof(inv));
		nuc_index = inv.nucnum;
		if(nuc_index != -1)
		{
			NuclideData_getName(nuc_index,nucname);
			strncpy(&strng[4],nucname,5);
			if(inv.type != -1)
				strncpy(&strng[11],&study_type[inv.type][0],5);
			if(inv.id != -1)
                sprintf(&strng[18],"%02d",inv.id);
		}
        display_text(0,8 * line,strng,0,SMALL,NORMAL);
	}







static void	withdraw_from_inventory(void)
	{
		short inv_index;
		short nuc_index;
		INVENTORY inv;
		short act_unit;
		float draw_act;
		float drawn_vol;
		float req_vol;
		float now_act;
		float rem_vol;
		float conc;
		char str[20];
        char dstr[20];
		short i;
		short ndec;
		float act_disp;
		float meas_act;
		float rem_act;
		float curr_motc;
        time_t  dt;             //date,time of use
        time_t  nowtime;
        short kun;
        short nc;
		NUCDATA nucdata;
        struct tm tmdt;
        //float act;
        short ch_num = current.main_chamber;
		bool ynret;
        bool dret;
        
		for(;;)
		{
			inv_index = inventory_list(FALSE,FALSE);
	
			if(inv_index == -1 || home_set())
				return;
	
			/* get local copy of inventory item */
			memcpy(&inv, &inventory[inv_index],sizeof(inv));

			nuc_index = inv.nucnum;
			NuclideData_getNuclide(nuc_index,&nucdata);
	
			//display_inventory(&inv,&nucdata,1,0);
            display_inventory(&inv,&nucdata,TRUE,TRUE);
			//display_text(0,52,"OK? Y OR N",0,MEDIUM,NORMAL);
            display_medium_message(OK_MSG,52,0,NORMAL);
            ynret = yesorno();
            if(home_set())
                return;
			if(!ynret)
				continue;

			// Locked: CI
			//if(current.system != inv.syst)
			//{
			//	erase_screen();
			//	if(inv.syst == CI)
			//	{
/*					display_text(0,8,"Ci Must Be Selected",0,SMALL,NORMAL);
 - 					display_text(0,16,"On Main Screen",0,SMALL,NORMAL);
 - 					display_text(0,24,"To Withdraw From",0,SMALL,NORMAL);
 - 					display_text(0,32,"Item Entered in Ci",0,SMALL,NORMAL);*/
            //        display_small_message(INV_32,8,0,NORMAL);
            //        display_small_message(INV_33,16,0,NORMAL);
            //        display_small_message(INV_34,24,0,NORMAL);
            //        display_small_message(INV_35,32,0,NORMAL);
			//	}
			//	else
			//	{
/*					display_text(0,8,"Bq Must Be Selected",0,SMALL,NORMAL);
 - 					display_text(0,16,"On Main Screen",0,SMALL,NORMAL);
 - 					display_text(0,24,"To Withdraw From",0,SMALL,NORMAL);
 - 					display_text(0,32,"Item Entered in Bq",0,SMALL,NORMAL);*/
            //        display_small_message(INV_36,8,0,NORMAL);
            //        display_small_message(INV_33,16,0,NORMAL);
            //        display_small_message(INV_34,24,0,NORMAL);
            //        display_small_message(INV_37,32,0,NORMAL);
            //    }
			//	contmsg();
			//	return;
			//}

			/* activity & time */
			for(;;)
			{
				for(;;)
				{
					erase_screen();
					enter_activity(&draw_act,&act_unit);
                    if(home_set())
                        return;
                    
					draw_act /=  unitfact[act_unit - 1];
	
					/* 	date for activity -- default is today */
                    read_clock(&nowtime);
                    clock_time = nowtime;
                    low_clock_time = clock_time;
                    reset_minute_counter_with_seconds();
					erase_screen();
                    //display_text(8,12,"Date of Use",0,MEDIUM,NORMAL);
                    display_medium_message(INV_38,2,0,NORMAL);
                    display_text(6,16,"ENTER for TODAY",0,SMALL,NORMAL);
                    //display_text(30,56,"MMDDYYYY",0,SMALL,NORMAL);
                    //display_small_message(DATE_TIME_IN_6,56,0,NORMAL);
                    
                    dret = date_in(8,30,&tmdt);
                    if(home_set())
                        return;
                    if(!dret)
                        memcpy(&tmdt,gmtime(&nowtime),sizeof(tmdt));
                    
                    erase_screen();
                    //display_text(8,12,"Time of Use",0,MEDIUM,NORMAL);
                    display_medium_message(INV_39,12,0,NORMAL);
                    //display_text(30,56,"hhmm",0,SMALL,NORMAL);
                    display_small_message(DATE_TIME_IN_7,56,0,NORMAL);
                   
                    dret = time_in(8,30,&tmdt);
                    if(home_set())
                        return;
                    if(!dret)
                        continue;   //top of forever
                    
                    tmdt.tm_isdst = -1;
                    dt = mk_time(&tmdt);
					
					now_act = nucdecay(inv.act,inv.date,
									   dt,nucdata.halflife,
									   nucdata.hlunit);
					
					if(draw_act < now_act)
						break;
	
					/*draw_act >= now_act*/
					beep();
					erase_screen();
					//display_text(0,12,"ACTIVITY GREATER",0,MEDIUM,NORMAL);
					//display_text(0,22,"THAN THAT IN",0,MEDIUM,NORMAL);
					//display_text(0,32,"INVENTORY",0,MEDIUM,NORMAL);
                    display_medium_message(INV_19,12,0,NORMAL);
                    display_medium_message(INV_20,22,0,NORMAL);
                    display_medium_message(INV_6,32,0,NORMAL);
					contmsg();
                    if(home_set())
                        return;
                    
	
				}

				conc = now_act / inv.vol;
				req_vol = draw_act / conc;
	
				erase_screen();
                display_text(6,0,&nucdata.name[0],0,SMALL,NORMAL);
				if(inv.type != -1)
                    display_text(48,0,(char *)&study_type[inv.type][0],0,SMALL,NORMAL);
	
				if(inv.id != -1)
				{
                    sprintf(str,"%02d",inv.id);
                    display_text(96,0,str,0,SMALL,NORMAL);
				}
	
                //display_text(0,16,"Activity:",0,SMALL,NORMAL);
                display_small_message(INV_10,16,0,NORMAL);
                // Removed:
				//ndec = getdec(draw_act,inv.syst,&kun);
                ndec = getdec(draw_act, CI, &kun);
				act_disp = draw_act * unitfact[kun - 1];
				// Removed:
				//if(inv.syst == CI && draw_act > 10.01 ||
				//   inv.syst == BQ && draw_act > 1.0e+12)
				if(draw_act > 10.01)
				{
                    //display_text(54,16,"OVER",0,SMALL,NORMAL);
                    display_small_message(INV_11,16,0,NORMAL);
				}
				else
				{
					act2str(act_disp,str,ndec,FALSE,kun);
                    display_act_small(str,9,2);
				}

	
                for(i = 0; i < 20; i++)
                    str[i] = ' ';
                dateout_language(dstr,&dt,4);
                strncpy(str,dstr,strlen(dstr));
                timeout(&str[12],&dt);
                display_text(0,24,str,0,SMALL,NORMAL);
	
	
	
	
                //display_text(0,32,"Withdraw:",0,SMALL,NORMAL);
                display_small_message(INV_21,32,0,NORMAL);
                
	            nc = sprintf(str,"%5.1f",req_vol);
                display_text(6 * (15 - nc),32,str,0,SMALL,NORMAL);
                display_text(90,32,"ml",0,SMALL,NORMAL);

               //display_text(8,52,"OK? Y OR N",0,MEDIUM,NORMAL);
                display_medium_message(OK_MSG,52,0,NORMAL);
               ynret = yesorno();
                if(home_set())
                    return;
                if(ynret)
					break;
			}

			/* measure activity */
			erase_screen();
            //display_text(8,12,"MEASURE",0,MEDIUM,NORMAL);
            //display_text(8,22,"ACTIVITY",0,MEDIUM,NORMAL);
            display_medium_message(LAB_20,12,0,NORMAL);
            display_medium_message(INV_22,22,0,NORMAL);
            
			contmsg();
            if(home_set())
                return;
            
			measure_activity(&nucdata.name[0]);
            if(home_set())
                return;
	    // Changed:
            //act = measurement[ch_num].act;
            //kun = measurement[ch_num].kun;
			//meas_act =  act / unitfact[kun - 1];
	    meas_act = measurement[ch_num].act0;

			/* calculate withdrawn volume from measured activity */
			now_act = get_current_act(&inv,&nucdata,&nowtime);
			conc = now_act / inv.vol;
			drawn_vol = meas_act / conc;

			/* get remaining volume & activity & save */
			rem_vol = inv.vol - drawn_vol;
			rem_act = now_act - meas_act;
			inv.vol = rem_vol;
			inv.act = rem_act;
			/* save date/time read in get_current_act */
            inv.date = nowtime;

			/* if Tc, update Mo/Tc */
			if(inv.nucnum == 0 && inv.mopertc != 0)
			{
				curr_motc = update_motc(&inv);
				inv.mopertc = curr_motc;
			}

			/* save inv to inventory */
			memcpy(&inventory[inv_index],&inv,sizeof(inv));
			return;
		}
	}

void Amulet_printInventory(void){
	print_inventorysec();
}

void get_study_type(short istudy, char *study);
static void	print_inventory(void)
	{
		short i;
		short nuc_index;
		short index;
		char str[12];
		//float act_disp;
		short nc;
		//short ndec;
		float curr_act;
		NUCDATA nuc;
        time_t nowtime;
        //short kun;
        char prtype;
        short tc_index;
        char strng[90];
        short line;
        float conc;
        bool over_flag;
        char lot[15];
        char nucname[6];
		char message[50];
		short len;

        //EE_READ(print[0],(uchar *)&prtype);
        prtype = current.printer;
        if (!start_printer(prtype,1,FALSE,PAPER))
            return;
			
		
        prhead_language(prtype);
		
        feed(1,prtype);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(TRUE);
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[6],"INVENTORY",9);
        //get_message(INV_40,&strng[6]);
		get_amulet_message(L_PR_INVENTORY,message);    // "INVENTORY"
		strncpy(&strng[6],message,strlen(message));
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(FALSE);
		feed(1,prtype);

        line = 5;

		for(i = 0; i < MAX_INVENTORY; i++)
		{
			if(inventory[i].nucnum == -1)
				continue;


            if(prtype != ROLL_PRINTER)
            {    
                if(line >= 52)
                {
                    formfeed(prtype);
                    if(prtype == SLIP_PRINTER)
                    {
                        change_paper();
                        if(home_set())
                            return;
                    }    
                    prhead_language(prtype);
                    feed(1,prtype);
                    line = 3;
                }
            }


            lininit(strng,TRUE,prtype);
			nuc_index = inventory[i].nucnum;
			NuclideData_getNuclide(nuc_index,&nuc);
			stringtofixed(nucname, nuc.name, 6);
			strncpy(&strng[2], nucname, 6);

			if(inventory[i].type != -1)
			{
				index = inventory[i].type;
				//strncpy(&strng[9],&study_type[index][0],5);
				get_study_type(index,message);
				len = strlen(message);
				strncpy(&strng[9],message,len);
			}
			else
				len = 5;
			len += 2;

			if(inventory[i].id != -1)
			{
                sprintf(str,"%02d",inventory[i].id);
				//strncpy(&strng[16],str,2);
				strncpy(&strng[9 + len],str,2);
			}
            pr_write(strng);
            ++line;

            lininit(strng,TRUE,prtype);
            dateout_language(str,&inventory[i].date,4);
            strncpy(&strng[2],str,11);
            timeout(str,&inventory[i].date);
            strncpy(&strng[14],str,5);
            pr_write(strng);
            ++line;

            lininit(strng,TRUE,prtype);
			//strncpy(&strng[2],"Lot #:",6);
            //get_message(INV_41,&strng[2]);
			get_amulet_message(L_PR_LOT,message);    // "Lot #:"
			strncpy(&strng[2],message,strlen(message));
            stringtofixed(lot, inventory[i].lot, 15);
			strncpy(&strng[9], lot, 15);
            pr_write(strng);
            ++line;

            lininit(strng,TRUE,prtype);
			//  strncpy(&strng[2],"Volume:",7);			
            //get_message(DOSE_6,&strng[2]);
			get_amulet_message(L_VOLUME,message);    // "Volume:"
			//strcat(message,":");
			strncpy(&strng[2],message,strlen(message));
            nc = sprintf(str,"%5.1f",inventory[i].vol);
			strncpy(&strng[10],str,nc);
			strncpy(&strng[10 + nc],"ml",2);
            pr_write(strng);
            ++line;

            lininit(strng,TRUE,prtype);
			//strncpy(&strng[2],"Activity:",9);
            //get_message(DOSE_5,&strng[2]);
			get_amulet_message(L_ACTIVITY,message);    // "Activity"
			strcat(message,":");
			strncpy(&strng[2],message,strlen(message));
            
			// get current activity 
			curr_act = get_current_act(&inventory[i],&nuc,&nowtime);
            over_flag = FALSE;
            if(curr_act > 10.)
            {    
	    		//strncpy(&strng[11],"OVER",4);
                //get_message(INV_42,&strng[11]);
				get_amulet_message(L_PR_OVER,message);    // "OVER"
				strncpy(&strng[12],message,strlen(message));
                over_flag = TRUE;
            }    
			else
			{
				format_activity_system(curr_act, str);
				if(prtype == USB_EPS_PRINTER) replace(str, '$', 'u');
				else replace(str, '$', MU_PRT);
				//strncpy(&strng[11],str,9);
				strncpy(&strng[12],str,9);
			}
            pr_write(strng);
            ++line;

            //if not over range, calculate and display concentration
            if(!over_flag)
            {    
                lininit(strng,TRUE,prtype);
                //strncpy(&strng[2],"Conc:    ",9);
				get_amulet_message(L_CONC,message);    // "Conc"
				strcat(message,":    ");
				strncpy(&strng[2],message,strlen(message));
                conc = curr_act / inventory[i].vol;

                format_activity_system(conc, str);
                if(prtype == USB_EPS_PRINTER) replace(str, '$', 'u');
				else replace(str, '$', MU_PRT);
                strncpy(&strng[11],str,9);
                strncpy(&strng[20],"/ml",3);
                pr_write(strng);
                ++line;
            }

            
			/* print current Mo/Tc if it exists */
            //get Tc99m index
            tc_index = NuclideData_getIndexFromName("Tc99m");
			if(inventory[i].nucnum == tc_index && 
			   inventory[i].mopertc != 0)
			{
                lininit(strng,TRUE,prtype);
				strncpy(&strng[2],"Mo/Tc:",6);
				curr_act = update_motc(&inventory[i]);
				if(curr_act > 0)
				{
                    nc = sprintf(str,"%6.3f",curr_act);
					strncpy(&strng[10],str,nc);
				}
                pr_write(strng);
                line += 6;
			}
            else
                line += 5;
			feed(1,prtype);
		}
		formfeed(prtype);
	}

    
    /* initialize inventory */
    void initialize_inventory(void)
    {
		short i;
        
        for(i = 0; i < MAX_INVENTORY; i++)
			inventory[i].nucnum = -1;

    }




    /* returns current motc ratio; -1 is error return */
    float update_motc(INVENTORY *inv)
    {
        float curr_motc;
        float curr_tc;
        time_t nowtime;
        float halflife;
        uchar hlunit;
        NUCDATA nuc;

        read_clock(&nowtime);
        clock_time = nowtime;
        low_clock_time = clock_time;
        reset_minute_counter_with_seconds();

        /* can't calculate if Tc too low 
          decay Tc to now */
        NuclideData_getNuclide(inv -> nucnum,&nuc);  /* Tc data */
        curr_tc = nucdecay(inv -> act, 
                           inv -> date,nowtime,
                           nuc.halflife,nuc.hlunit);
        // Locked: CI
        //if(current.system == CI)
        //{
        //    if(curr_tc < 1.0e-9)
        //        return(-1.);
        //}
        //else /* Bq */
        //    if(curr_tc < 40.)
        //        return(-1.);

        if(curr_tc < 1.0e-9) return(-1.);

        /* get current ratio 
          and decay  back from now */
        halflife = MoTcHl;
        hlunit = MoTcHlUnit;
        curr_motc = nucdecay(inv -> mopertc,
                             nowtime,inv -> date,
                            halflife,hlunit);
        return(curr_motc);
    }
    

    float update_motc_timepoint(INVENTORY *inv, time_t *timepoint)
    {
        float curr_motc;
        float curr_tc;
        float halflife;
        uchar hlunit;
        NUCDATA nuc;

        /* can't calculate if Tc too low
          decay Tc to now */
        NuclideData_getNuclide(inv -> nucnum,&nuc);  /* Tc data */
        curr_tc = nucdecay(inv -> act,
                           inv -> date, *timepoint,
                           nuc.halflife,nuc.hlunit);

        // Locked: CI
        //if(current.system == CI)
        //{
        //    if(curr_tc < 1.0e-9)
        //        return(-1.);
        //}
        //else /* Bq */
        //    if(curr_tc < 40.)
        //        return(-1.);
        if(curr_tc < 1.0e-9) return(-1.);

        /* get current ratio
          and decay  back from now */
        halflife = MoTcHl;
        hlunit = MoTcHlUnit;
        curr_motc = nucdecay(inv -> mopertc,
                             *timepoint,inv -> date,
                            halflife,hlunit);
        return(curr_motc);
    }

void write_Inventory(void){
	write_binary_to_Options_folder("inventry.bin", (char *) &inventory, MAX_INVENTORY * sizeof(INVENTORY));
}

static void	print_inventorysec(void)
	{
		short i;
		short nuc_index;
		short index;
		char str[12];
		//float act_disp;
		short nc;
		//short ndec;
		float curr_act;
		NUCDATA nuc;
        time_t nowtime;
        //short kun;
        char prtype;
        short tc_index;
        char strng[90];
        short line;
        float conc;
        bool over_flag;
        char lot[15];
        char nucname[6];
		char message[50];
		short len;

        //EE_READ(print[0],(uchar *)&prtype);
        prtype = current.printer;
        if (!start_printer(prtype,1,FALSE,PAPER))
            return;


        prhead_languagesec(prtype);

        feed(1,prtype);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(TRUE);
        lininit(strng,TRUE,prtype);
		//strncpy(&strng[6],"INVENTORY",9);
        //get_message(INV_40,&strng[6]);
		get_amulet_message(L_PR_INVENTORY,message);    // "INVENTORY"
		strncpy(&strng[6],message,strlen(message));
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(FALSE);
		feed(1,prtype);

        line = 5;

		for(i = 0; i < MAX_INVENTORY; i++)
		{
			if(inventory[i].nucnum == -1)
				continue;


            if(prtype != ROLL_PRINTER)
            {
                if(line >= 52)
                {
                    formfeed(prtype);
                    if(prtype == SLIP_PRINTER)
                    {
                        change_paper();
                        if(home_set())
                            return;
                    }
                    prhead_languagesec(prtype);
                    feed(1,prtype);
                    line = 3;
                }
            }


            lininit(strng,TRUE,prtype);
			nuc_index = inventory[i].nucnum;
			NuclideData_getNuclide(nuc_index,&nuc);
			stringtofixed(nucname, nuc.name, 6);
			strncpy(&strng[2], nucname, 6);

			if(inventory[i].type != -1)
			{
				index = inventory[i].type;
				//strncpy(&strng[9],&study_type[index][0],5);
				get_study_type(index,message);
				len = strlen(message);
				strncpy(&strng[9],message,len);
			}
			else
				len = 5;
			len += 2;

			if(inventory[i].id != -1)
			{
                sprintf(str,"%02d",inventory[i].id);
				//strncpy(&strng[16],str,2);
				strncpy(&strng[9 + len],str,2);
			}
            pr_write(strng);
            ++line;

            lininit(strng,TRUE,prtype);
            dateout_language(str,&inventory[i].date,4);
            strncpy(&strng[2],str,11);
            timeout(str,&inventory[i].date);
            strncpy(&strng[14],str,5);
            pr_write(strng);
            ++line;

            lininit(strng,TRUE,prtype);
			//strncpy(&strng[2],"Lot #:",6);
            //get_message(INV_41,&strng[2]);
			get_amulet_message(L_PR_LOT,message);    // "Lot #:"
			strncpy(&strng[2],message,strlen(message));
            stringtofixed(lot, inventory[i].lot, 15);
			strncpy(&strng[9], lot, 15);
            pr_write(strng);
            ++line;

            lininit(strng,TRUE,prtype);
			//  strncpy(&strng[2],"Volume:",7);
            //get_message(DOSE_6,&strng[2]);
			get_amulet_message(L_VOLUME,message);    // "Volume:"
			//strcat(message,":");
			strncpy(&strng[2],message,strlen(message));
            nc = sprintf(str,"%5.1f",inventory[i].vol);
			strncpy(&strng[10],str,nc);
			strncpy(&strng[10 + nc],"ml",2);
            pr_write(strng);
            ++line;

            lininit(strng,TRUE,prtype);
			//strncpy(&strng[2],"Activity:",9);
            //get_message(DOSE_5,&strng[2]);
			get_amulet_message(L_ACTIVITY,message);    // "Activity"
			strcat(message,":");
			strncpy(&strng[2],message,strlen(message));

			// get current activity
			curr_act = get_current_act(&inventory[i],&nuc,&nowtime);
            over_flag = FALSE;
            if(curr_act > 10.)
            {
	    		//strncpy(&strng[11],"OVER",4);
                //get_message(INV_42,&strng[11]);
				get_amulet_message(L_PR_OVER,message);    // "OVER"
				strncpy(&strng[12],message,strlen(message));
                over_flag = TRUE;
            }
			else
			{
				format_activity_system(curr_act, str);
				if(prtype == USB_EPS_PRINTER) replace(str, '$', 'u');
				else replace(str, '$', MU_PRT);
				//strncpy(&strng[11],str,9);
				strncpy(&strng[12],str,9);
			}
            pr_write(strng);
            ++line;

            //if not over range, calculate and display concentration
            if(!over_flag)
            {
                lininit(strng,TRUE,prtype);
                //strncpy(&strng[2],"Conc:    ",9);
				get_amulet_message(L_CONC,message);    // "Conc"
				strcat(message,":    ");
				strncpy(&strng[2],message,strlen(message));
                conc = curr_act / inventory[i].vol;

                format_activity_system(conc, str);
                if(prtype == USB_EPS_PRINTER) replace(str, '$', 'u');
				else replace(str, '$', MU_PRT);
                strncpy(&strng[11],str,9);
                strncpy(&strng[20],"/ml",3);
                pr_write(strng);
                ++line;
            }


			/* print current Mo/Tc if it exists */
            //get Tc99m index
            tc_index = NuclideData_getIndexFromName("Tc99m");
			if(inventory[i].nucnum == tc_index &&
			   inventory[i].mopertc != 0)
			{
                lininit(strng,TRUE,prtype);
				strncpy(&strng[2],"Mo/Tc:",6);
				curr_act = update_motc(&inventory[i]);
				if(curr_act > 0)
				{
                    nc = sprintf(str,"%6.3f",curr_act);
					strncpy(&strng[10],str,nc);
				}
                pr_write(strng);
                line += 6;
			}
            else
                line += 5;
			feed(1,prtype);
		}
		formfeed(prtype);
	}
