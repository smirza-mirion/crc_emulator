/*********************************************************************
  MODULE:	MO ASSAY for CRC-25R,W

  FILE:		MolyAssay_25.c

  DATE:     03/27/08
            09/11/08  -- moved moinit to right before init_mo_empty and init_mo_assay

  CALLED BY:
		tests menu

    *************************************************************************/
#include "crc.h"
#include "i2c.h"
#include "screen.h"
#include "keyboard.h"
#include "printer.h"
#include "uart.h"
#include "daily.h"
#include "chambfac.h"
#include "message.h"
#include "nuc.h"
#include <string.h>

    extern CHAMBER chamber[];
    extern MEASUREMENT measurement[];
    extern CURRENT current;
    extern BKGDATA bkgdata[];
    
    
    char mo_actstr[12];
    char tc_actstr[12];
    static char concstr[12];
    float moptc;
	static float vol;
    short dosehour;
    static short method;
    float mobkg;
    float moact;
    static bool bad_flag;
    time_t tcstamp;

    void moinit(void);
    static void moly(void);
    static void badmoly(void);
    static bool mo_empty(void);
    static bool moassay_mo(void);
    static bool moassay_tc(void);
    static short print_mo_assay(short mode);
    static void pr_tc_activ(char printer);
    static void prexp(char printer);
    static bool mo_bkg(short ch_num, bool *hiflag);    
    static void display_mo_bkg(short ch_num,bool hiloflag);
    void init_mo_empty(short nuc_index, short ch_num);
    void init_mo_assay(short nuc_index, short ch_num);
    static  void print_nuclide_name(char printer);
    static void print_staffid(char printer);
    static  void print_nuclide_name_staffid(char printer);
    void reverse_feed(short num, char printer);
	void moly_assay(void)
	{

        char mo[4];
        short nchoose;
        short num_mo;
        short i;
        short ch_num = current.main_chamber;

        EE_READ(mo_choice,(uchar *)mo);
		
        num_mo = 0;
        for (i = 0; i < 3; i++)
        {
            if(mo[i] == 1)
                ++num_mo;
        }
        
        if (num_mo == 0)
			return;
		
        // do initialization 
		//moinit();
        erase_screen();


		if (num_mo == 1)
        {
            if(mo[0] == 1)      //Capmac for Mallinkrodt
                method = 1;
            if(mo[1] == 1)      //Capmac for Bristol Myers
                method = 2;
            if(mo[2] == 1)      //Canister
                method = 0;
        }
		
        if (num_mo > 1)
        {
/*	        display_text(24,0,"SELECT",0,SMALL,NORMAL);
 - 			display_text(0,16,"1. CAPMAC for",0,SMALL,NORMAL);
 -             display_text(0,24,"   Mallinckrodt Gen",0,SMALL,NORMAL);
 -             display_text(0,32,"2. CAPMAC for",0,SMALL,NORMAL);
 -             display_text(0,40,"   Bristol Myers Gen",0,SMALL,NORMAL);
 - 			display_text(0,48,"3. Capintec Canister",0,SMALL,NORMAL);*/
            display_small_message(MOLY_1,0,0,NORMAL);
            display_small_message(MOLY_2,16,0,NORMAL);
            display_small_message(MOLY_3,24,0,NORMAL);
            display_small_message(MOLY_4,32,0,NORMAL);
            display_small_message(MOLY_5,40,0,NORMAL);
            display_small_message(MOLY_6,48,0,NORMAL);
			
			nchoose = choose(0,3);
            if(home_set())
                return;
			erase_screen();

			switch (nchoose)
			{
			case 0:
				return;

			case 1:
				method = 1;
				break;

			case 2:
				method = 2;
				break;
            case 3:
                method = 0;
                break;
			}

		}

        moly();

        //reset to Tc99m
        getiso(ch_num,0);

	}


	void moinit(void)
	{
        short ch_num = current.main_chamber;

	    //erase_screen();
        // Removed:
	    //measurement[ch_num].syst0 = -1;
		measurement[ch_num].kun0 = 0;
        id_init();
		measurement[ch_num].idecmin = 0;
		measurement[ch_num].idec = 0;
		measurement[ch_num].act0 = 0.;
		strcpy(&measurement[ch_num].actstr0[0],"      ");
        setchamber(ch_num,0);
		measurement[ch_num].nmeas = 0;
		measurement[ch_num].changed = FALSE;
		measurement[ch_num].gain = 0;
		measurement[ch_num].dogain = 2;
        
    }

static void moly(void)
	{

		if(!mo_empty() || home_set())
            return;

        erase_screen();
		if(!moassay_mo() || home_set())
            return;
        erase_screen();
		moassay_tc();
	}



static void badmoly(void)
	 {

        bad_flag = TRUE;
        erase_lines(22,42,0);
        display_text(40,22,"ERROR",0,BIG,NORMAL);
        //erase_text_line(38,0,BIG);
		display_text(30,38,"TOO HIGH",0,BIG,NORMAL);
		//display_text(6,56,"Any Key to Terminate",0,SMALL,NORMAL);
        display_small_message(MOLY_7,56,0,NORMAL);
		(void)keyin();
		erase_screen();
	 }


static bool mo_empty(void)
     {
         short nuc_index;
         char ch;
         short ch_num = current.main_chamber;
         bool hiloflag;

         bad_flag = FALSE;
          

         mobkg = 0.;
         if(method == 0)
         {
             nuc_index = NuclideData_getIndexFromName("MoCAN");
             display_text(20,32,"CANISTER",0,BIG,NORMAL);
         }
         else
         {
             nuc_index = NuclideData_getIndexFromName("MoCAP");
             display_text(30,32,"CAPMAC",0,BIG,NORMAL);
         }

         strcpy(&chamber[ch_num].nucdata.name[0],"Mo99  ");
         disp_nuclide();
         
         display_text(96,2,"BKG",0,MEDIUM,NORMAL);
         display_text(40,16,"EMPTY",0,BIG,NORMAL);

         //display_text(0,48,"Press N(6) to Skip",0,SMALL,NORMAL);
         //display_text(0,56,"Other Key to Proceed",0,SMALL,NORMAL);
         display_small_message(MOLY_8,48,0,NORMAL);
         display_small_message(MOLY_9,56,0,NORMAL);
         

         ch = keyin();
         if(ch == SIX || home_set())
             return(TRUE);

         measurement[ch_num].mode = MOBKGMODE;

         moinit();

         init_mo_empty(nuc_index,ch_num);

         plwait(TRUE);
         set_nokey();
         measurement[ch_num].display_flag = FALSE;

         for(;;)
         {
            if(measurement[ch_num].display_flag)
            {

                hiloflag = TRUE;
                if(bkgdata[ch_num].count <= 20)
                    hiloflag = FALSE;
                display_mo_bkg(ch_num,hiloflag);
                measurement[ch_num].display_flag = FALSE;
            }

            do
            {
                ch = getkey();      //triggers servicing chambers
                if(home_set())
                    return FALSE;

                if(bkgdata[ch_num].status != TEST_WAIT)
                {

                    if(ch == OK)
                        bkgdata[ch_num].done = TRUE;
                    else
                    {
                        if(ch != NOKEY)
                            beep();
                    }    
                }
                else
                {
                    if(ch != NOKEY)
                        beep();
                }    
                if(!chamber[ch_num].connected_flag)
                    return 2;

            }while(!measurement[ch_num].valid_flag);    
                   

            if(measurement[ch_num].gain > 0)
            {
                 badmoly();
                 return(FALSE);
            }    

            if(bkgdata[ch_num].done)
                break;
         }    


         mobkg = measurement[ch_num].meas;
         return(TRUE);
     }

static bool moassay_mo(void)
     {
         short nuc_index;
         short ch_num = current.main_chamber;
         bool hiflag;
         char ch;


         display_text(20,16,"ELUATE IN",0,BIG,NORMAL);
         if(method == 0)
         {
             nuc_index = NuclideData_getIndexFromName("MoCAN");
             display_text(20,32,"CANISTER",0,BIG,NORMAL);
         }
         else
         {
             nuc_index = NuclideData_getIndexFromName("MoCAP");
             display_text(30,32,"CAPMAC",0,BIG,NORMAL);
         }
         strcpy(&chamber[ch_num].nucdata.name[0],"Mo99  ");
         disp_nuclide();
         display_text(80,0,"ASSAY",0,MEDIUM,NORMAL);
         contmsg();
         if(home_set())
             return FALSE;

         moinit();

         init_mo_assay(nuc_index,ch_num);
         plwait(TRUE);
         set_nokey();
         hiflag = FALSE;
         
         measurement[ch_num].display_flag = FALSE;

         for(;;)
         {
            if(measurement[ch_num].display_flag)
            {

                display_text(20,22,measurement[ch_num].actstr,0,BIG,NORMAL);
                strcpy(mo_actstr,&measurement[ch_num].actstr[0]);
                
                if(bkgdata[ch_num].status != TEST_WAIT)
                {
                    if(!mo_bkg(ch_num,&hiflag))
                    {
                        erase_screen();
                        return (FALSE);
                    }    

                }    
                
                measurement[ch_num].display_flag = FALSE;
            }

            do
            {
                ch = getkey();      //triggers servicing chambers
                if(home_set())
                    return FALSE;

                if(bkgdata[ch_num].status != TEST_WAIT)
                {

                    if(ch == OK)
                        bkgdata[ch_num].done = TRUE;
                    else
                    {
                        if(ch != NOKEY)
                            beep();
                    }    
                }
                else
                {
                    if(ch != NOKEY)
                        beep();
                }    
                if(!chamber[ch_num].connected_flag)
                    return 2;

            }while(!measurement[ch_num].valid_flag);    
                   

            if(bkgdata[ch_num].done)
                break;
         }

        //get last value to mo_actstr
         strcpy(mo_actstr,&measurement[ch_num].actstr[0]);
         
         // Locked: CI
         //if (current.system == CI)
         //   moact = measurement[ch_num].act / 1.0e-6;  /*uCi*/
         //else
         //   moact = measurement[ch_num].act / 1.0e+6;  /*MBq*/
	 // Changed:
         //moact = measurement[ch_num].act / 1.0e-6;  /*uCi*/
	 moact = measurement[ch_num].act0;  /*uCi*/
         return TRUE;
         
         


         
     }

const float motc_factor[] =
{
    1.0,1.11057,1.23337,1.36974,1.52119,1.68939,1.87618,2.08363,
    2.31401,2.56987,2.85402,3.16959,3.52005,3.90926
};

static bool moassay_tc(void)
	{
		float limit;
		float ratio;
		short index;
		bool ynret;
		bool tc_done;
		short  ihour;
		short pr_ret;
        char fmtstr[10];
        char buffer[10];
        char ch;
        NUCDATA nucdata;
        char strng[30];
        float conc;
        float tcact, tcact0;
        short ndec;
        short ch_num = current.main_chamber;

		id_init();

        measurement[ch_num].mode = MEASMODE;


		switch(method)
        {
        case 0:
            index = NuclideData_getIndexFromName("Tc99m");
            break;
        case 1:     //Malinckrodt
            index = NuclideData_getIndexFromName("TcCAPM");
            break;
        case 2:     //Bristol Myers
            index = NuclideData_getIndexFromName("TcCAPB");
            break;
        }
		
        NuclideData_getNuclide(index,&nucdata);
        chamber[ch_num].nucdata = nucdata;
		strcpy(&chamber[ch_num].nucdata.name[0],"Tc99m ");
        measurement[ch_num].response = nucdata.response[chamber_type(ch_num)];
        
		disp_nuclide();

		nucinit(ch_num,TRUE);
        respcor(ch_num,&measurement[ch_num].response);
        
		display_text(80,2,"ASSAY",0,MEDIUM,NORMAL);
		//display_text(18,56,"ENTER to Accept",0,SMALL,NORMAL);
        display_small_message(AUTO_ZERO_8,56,0,NORMAL);
		tc_done = FALSE;

		do
		{
			ch = getkey();
            if(home_set())
                return FALSE;

			switch (ch)
			{
			case NOKEY:
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
                dokey(1);
				break;

			case OK:
				tc_done = TRUE;
				break;

            case CLR:
                keybuf_bsp();
				break;

			default:
				beep();
				break;
			}

			if(!tc_done)
			{
                if(measurement[ch_num].over_flag)
                    overflow(ch_num);
                else
                {    
                    if(measurement[ch_num].display_flag)
                    {    
                        activity_to_screen();
                        measurement[ch_num].display_flag = FALSE;
                    }
                }
			}
		}while(!tc_done);

        set_idnum();
		erase_screen();
		strcpy(tc_actstr,&measurement[ch_num].actstr[0]);

		tcact0 = tcact = measurement[ch_num].act0;
		// Locked: CI
		//if (current.system == CI)
		//	tcact /= 1.0e-3;    /*mCi*/
		//else
		//	tcact /= 1.0e+9;    /*GBq*/
		tcact /= 1.0e-3;    /*mCi*/

		if (measurement[ch_num].wasover)
		{
			disp_nuclide();
			display_text(30,23,"OVER RANGE",0,BIG,NORMAL);
			contmsg();
			erase_screen();
			return TRUE;
		}

		// Locked: CI
		//if ((current.system == CI && tcact < 1.0) ||
		//	(current.system == BQ && tcact < 0.037))
		if(tcact < 1.0)
		{
			disp_nuclide();
			display_text(30,23,"ACTIVITY",0,BIG,NORMAL);
			display_text(30,38,"TOO LOW",0,BIG,NORMAL);
			contmsg();
			erase_screen();
			return TRUE;
		}

		moptc = moact / tcact;
		/* read motc limit from eeprom */
        EE_READ(mo_tc_lim,(uchar *)&limit);

		display_text(0,0,"Mo99/Tc99m:",0,MEDIUM,NORMAL);
        if(moptc < 0)
            moptc = 0.;
        
        strcpy(fmtstr,"%5. f");
		ndec = 3;
		if (moptc >= 1.)
			ndec = 0;
        fmtstr[3] = ndec + '0';
        sprintf(strng,fmtstr,moptc);
        display_text(0,11,strng,0,MEDIUM,NORMAL);
        // Locked: CI
		//if (current.system == CI)
        //    strcpy(buffer,"$Ci/mCi");   //microCi/mCi
		//else
        //    strcpy(buffer,"MBq/GBq");
        strcpy(buffer,"$Ci/mCi");   //microCi/mCi

        display_text(50,11,buffer,0,MEDIUM,NORMAL);
		dosehour = 0;
		for(ihour = 12; ihour >= 0; ihour--)
		{
			ratio = limit / motc_factor[ihour];
			if (moptc < ratio)
			{
				dosehour = ihour;
				break;
			}
		}

		if (dosehour < 12)
		{
			if (dosehour >= 6 && dosehour < 9)
                //display_text(10,22,"CAUTION",0,MEDIUM,NORMAL);
                display_medium_message(MOLY_10,22,0,NORMAL);
			if (dosehour > 2 && dosehour < 6)
                //display_text(10,22,"MO HIGH",0,MEDIUM,NORMAL);
                display_medium_message(MOLY_11,22,0,NORMAL);
			if (dosehour > 2 && dosehour < 12)
			{
				//splay_text(12,40,"DO NOT USE AFTER",0,SMALL,NORMAL);
                display_small_message(MOLY_12,40,0,NORMAL);
                sprintf(strng,"%d",dosehour);
				display_text(18,48,strng,0,SMALL,NORMAL);
				//display_text(36,48,"HOURS",0,SMALL,NORMAL);
                display_small_message(MOLY_13,48,0,NORMAL);
			}

			if (dosehour <= 2)
			{
				//display_text(0,22,"MO TOO HIGH",0,MEDIUM,NORMAL);
				//display_text(10,34,"DO NOT USE",0,MEDIUM,NORMAL);
                display_medium_message(MOLY_14,22,0,NORMAL);
                display_medium_message(MOLY_15,34,0,NORMAL);
			}
            ackmsg();   //acknowledge message
            erase_lines(40,24,0);
		}

        erase_lines(34,30,0);
		//display_text(0,34,"VOLUME?  ml",0,MEDIUM,NORMAL);
        display_medium_message(MOLY_16,34,0,NORMAL);

        vol = inpflt(8,52,MEDIUM,0, 4,1.0,100.);
        if(home_set())
            return FALSE;


		erase_screen();
		disp_nuclide();
        display_text(10,19,&tc_actstr[1],0,BIG,NORMAL);		
        erase_text_line(34,0,BIG);
		conc = tcact0 / vol;
		// Locked: CI
        //format_activity(conc,current.system,concstr);
		format_activity(conc, CI, concstr);
        display_text(10,34,&concstr[1],0,BIG,NORMAL);
        display_text(90,34,"/ml",0,BIG,NORMAL);

		contmsg();
        if(home_set())
            return FALSE;

		do
		{
			pr_ret = print_mo_assay(1);
			if(pr_ret == -1) /* start printer failed or no printer */
				return TRUE;
			if(pr_ret == 1)   /* SLIP or OKI or LX */
				print_mo_assay(2);
			erase_screen();
			ynret = print_again();
			erase_screen();
		}while(ynret);

		return TRUE;
	}

void Amulet_printMoly(time_t tc_stamp, char *tcactstr, float tcact, float volume, char *moactstr, float MOPTC, short DOSEHOUR){
	short pr_ret;
	float tcact0, conc;

	tcstamp = tc_stamp;
	strncpy(tc_actstr, tcactstr, 12);
	strncpy(mo_actstr, moactstr, 12);
	vol = volume;
	tcact0 = tcact;
	conc = tcact0 / vol;
	// Locked: CI
	//format_activity(conc, current.system, concstr);
	//format_activity(conc, CI, concstr);
	format_activity_system(conc, concstr);
	moptc = MOPTC;
	dosehour = DOSEHOUR;

	pr_ret = print_mo_assay(1);
	if(pr_ret == -1) return;
	if(pr_ret == 1) print_mo_assay(2);
}

static short print_mo_assay(short mode)
	{

		char printer;
        char buffer[10];
        char fmtstr[8];
        char strng[40];
        short ndec;
        short nc;
        short kp;

        printer = current.printer;
		if(!start_printer(printer,mode,TRUE,TICKET))
			return -1;


/*        //nuclide name
 -         print_nuclide_name(printer);
 - 
 -         //staff id
 -         print_staffid(printer);*/

		//print nuclide name and staff id
		print_nuclide_name_staffid(printer);
        
        
		//Tc activity & date 
		pr_tc_activ(printer);

		// Tc conc
		strcpy(strng,"                   ");
		strncpy(&strng[5],&concstr[1],5);
        strncpy(&strng[10],&concstr[6],3);
        pr_mu(&strng[10],printer);
		strncpy(&strng[13],"/ml",3);
        pr_write(strng);

		// volume 
		//strcpy(strng," Vol:      ml\r\n");
        strcpy(strng,"           ml\r\n");
        get_message(MOLY_18,&strng[1]);
		ndec = 2;
		if (vol > 99.)
			ndec = 1;
        strcpy(fmtstr,"%6. f");
        fmtstr[3] = (char)ndec + '0';
        nc = sprintf(buffer,fmtstr,vol);
        strncpy(&strng[11 - nc],buffer,nc);
        pr_write(strng);

		// Mo activity
        switch(printer)
        {        
        case OKI_PRINTER:
            font171();
            kp = 7;
            break;
        case USB_PRINTER:
        case USB_EPS_PRINTER:
            kp = 5;
            break;
        case LX_PRINTER:
            eps_cpi(15);
            kp = 7;
            break;
        case SLIP_PRINTER:
            kp = 1;
            break;
        case ROLL_PRINTER:
            kp = 0;
            break;
            
		}
        
        strcpy(strng,"            ");
        strcpy(&strng[kp],"Mo:");
        strng[kp + 3] = '\0';
        pr_write(strng);
        
        pr_mu(&mo_actstr[6],printer);
		if(printer == OKI_PRINTER)
			fontnlq(TRUE);
        if(printer == LX_PRINTER)
            eps_cpi(12); 
        pr_write(mo_actstr);

		// mo/tc 
		if(printer == OKI_PRINTER)
		{
			font171();
            uart_write("  ",U_PR);
		}
        if(printer == LX_PRINTER)
            eps_cpi(15);         

        pr_write("  Mo/Tc: ");
		if(printer == OKI_PRINTER)
			fontnlq(TRUE);
        if(printer == LX_PRINTER)
            eps_cpi(12); 
		strcpy(strng,"            \r\n");

		if (current.system == CI)
		{
			strng[5] = MU_PRT;
            if(printer == USB_EPS_PRINTER)
                strng[5] = 'u';
			strncpy(&strng[6],"Ci/mCi",6);
		}
		else
			strncpy(&strng[5],"MBq/GBq",7);

		ndec = 3;
		if (moptc > 1.0)
			ndec = 0;
        strcpy(fmtstr,"%5. f");
        fmtstr[3] = (char)ndec + '0';
        nc = sprintf(buffer,fmtstr,moptc);
		strncpy(&strng[5 - nc],buffer,nc);
        pr_write(strng);

		prexp(printer);

		if (mode == 1)
		{
            switch(printer)
            {        
            case ROLL_PRINTER:
                feed(2,printer);
                return 0;
            case USB_PRINTER:
            case USB_EPS_PRINTER:    
                formfeed(printer);
                return 0;
            case SLIP_PRINTER:
            case OKI_PRINTER:
            case LX_PRINTER:    
                feed(2,printer);
                return 1;
            }    
		}
		else
			formfeed(printer);

        return 1;

	 }

static void pr_tc_activ(char printer)
	{

        time_t nowtime;
        char strng[90];
        short kp;
        
        //nowtime = clock_time;
        nowtime = tcstamp;

        set_act_line(printer);
        
		kp = 0;
		strncpy(strng,"    ",4);
		if(printer == OKI_PRINTER || printer == LX_PRINTER)
			kp = 2;
		if(printer < 0)
			kp = 4;

/*        kp = 0;
-        if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER)
-        {
-            //kp = 4;
-            //strncpy(strng,"    ",4);
 -             kp = 2;
 -             strncpy(strng,"  ",2);
-        }
-*/

        strncpy(&strng[kp],tc_actstr,6);
        strng[kp + 6] = '\0';
        pr_write(strng);

        switch(printer)
        {        
        case ROLL_PRINTER:
        case SLIP_PRINTER:
            font35();
            break;
        default:
            break;
        }    
        


        //unit
        strcpy(&strng[0],&tc_actstr[6]);
        strcat(strng," ");
        pr_write(strng);

        
        switch(printer)
        {        
        case OKI_PRINTER:
            font85e(FALSE);
            fontnlq(TRUE);
            break;
        case ROLL_PRINTER:
        case SLIP_PRINTER:
            break;
        case LX_PRINTER:    
            bold(FALSE);
            eps_cpi(12);
            break;
        case USB_PRINTER:
            pcl_cpi("10",TRUE);
            pcl_bold(FALSE);
            break;
        }    
        
        timeoutsec(strng,&nowtime);
        strcat(strng," ");
        pr_write(strng);

		//date
        dateout_language(strng,&nowtime,2);
        strcat(strng,"\r\n");
        pr_write(strng);
	}

extern const short month_days[];
static void prexp(char printer)
	{
		
        time_t exptime;
        struct tm tm_exp;
        short dhour;
        short mval[13];
        char strng[40];
        char str[6];
		char message[40];

        memcpy(mval,month_days,sizeof(mval));
		
        //start with current time;
        exptime = tcstamp;
        memcpy(&tm_exp,gmtime(&exptime),sizeof(tm_exp));
        tm_exp.tm_wday = 0;  //will be calculated by mk_time
        tm_exp.tm_yday = 0;
        tm_exp.tm_isdst = -1;
		if(printer == OKI_PRINTER)
			fontnlq(TRUE);

		if (dosehour <= 2)
        {    
			//"     MO TOO HIGH - DO NOT USE"
            //strcpy(strng,"                             ");
            //get_message(MOLY_19,&strng[5]);
			lininit(strng,FALSE,printer);
			get_amulet_message(L_MO_TOO_HIGH_DO_NOT_USE,message);    // "MO TOO HIGH - DO NOT USE"
			//strncpy(&strng[2],message,strlen(message));
			strncpy(&strng[5],message,strlen(message));
            pr_write(strng);
        }    
		else
		{
			//"      Expires "
            strcpy(strng,"              ");
            //get_message(MOLY_20,&strng[6]);
			get_amulet_message(L_EXPIRES,message);    // "Expires"
			strncpy(&strng[2],message,strlen(message));
            pr_write(strng);
            
			tm_exp.tm_hour += dosehour;
			dhour = tm_exp.tm_hour;
			if (tm_exp.tm_hour > 23)
				dhour -= 24;
			strcpy(strng,"  :   ");
            (void)sprintf(str,"%02d",dhour);
            strncpy(&strng[0],str,2);
            (void)sprintf(str,"%02d",tm_exp.tm_min);
            strncpy(&strng[3],str,2);
            pr_write(strng);
			if (tm_exp.tm_hour > 23)
			{
                tm_exp.tm_hour = dhour;
                mval[2] = 28;
				if (tm_exp.tm_year % 4 == 0)
					mval[2] = 29;
				++tm_exp.tm_mday;
				if (tm_exp.tm_mday > mval[tm_exp.tm_mon + 1])
				{
					++tm_exp.tm_mon;
					if (tm_exp.tm_mon == 13)
					{
						tm_exp.tm_mon = 1;
						++tm_exp.tm_year;
					}
				}
                exptime = mk_time(&tm_exp);
				dateout_language(strng,&exptime,2);
                pr_write(strng);
				if(printer == OKI_PRINTER)
					fontnlq(FALSE);
			}
		}
        feed(1,printer);
	}



    void low_mo_act(short ch_num)
    {
        float actfact;
        float response;
        char actstr[12];
        float lowbkg = bkgdata[ch_num].lowbkg;
        float activity;

        if(bkgdata[ch_num].count < 10)
        {
            ++bkgdata[ch_num].count;
            return;
        }

        if(bkgdata[ch_num].count >= 10 && bkgdata[ch_num].count < 50)
        {
            bkgdata[ch_num].sum += measurement[ch_num].volts;
            measurement[ch_num].actbuf[bkgdata[ch_num].count - 10] =
                measurement[ch_num].volts;
            ++bkgdata[ch_num].count;
            measurement[ch_num].meas = bkgdata[ch_num].sum /
                (float)(bkgdata[ch_num].count - 10);
            response = measurement[ch_num].response;
            actfact = get_gainfact(0);
            // Locked: CI
            //if(current.system == BQ)
            //    actfact *= BQFACTOR;
            // Changed:
            //measurement[ch_num].act = (measurement[ch_num].meas - lowbkg) *
            //    actfact / response;
            measurement[ch_num].act0 = (measurement[ch_num].meas - lowbkg) *
            	actfact / response;

            activity = measurement[ch_num].act0;
            if(current.system == BQ) activity *= BQFACTOR;
            // Locked: CI
            //format_activity(measurement[ch_num].act,current.system,actstr);
            // Changed:
            //format_activity(measurement[ch_num].act, CI, actstr);
            //format_activity(measurement[ch_num].act0, CI, actstr);
            format_activity(activity, current.system, actstr);
            strcpy(&measurement[ch_num].actstr[0],actstr);
            measurement[ch_num].display_flag = TRUE;
            return;
        }    

        if(bkgdata[ch_num].count == 50)
        {    
            measurement[ch_num].nmeas = 40;
            measurement[ch_num].longavg = TRUE;
            set_nokey();
            plwait(FALSE);
            //display_text(18,56,"ENTER to Accept",0,SMALL,NORMAL);
            display_small_message(AUTO_ZERO_8,56,0,NORMAL);
            //assume good
            bkgdata[ch_num].status = TEST_GOOD;
            bkgdata[ch_num].count = 51;
        }    

        ismeas(ch_num);
        
        response = measurement[ch_num].response;
        actfact = get_gainfact(0);
        // Locked: CI
        //if(current.system == BQ)
        //    actfact *= BQFACTOR;
        // Changed:
        //measurement[ch_num].act = (measurement[ch_num].meas - lowbkg) *
        //    actfact / response;
        measurement[ch_num].act0 = (measurement[ch_num].meas - lowbkg) *
        	actfact / response;

        activity = measurement[ch_num].act0;
        if(current.system == BQ) activity *= BQFACTOR;
        // Locked: CI
        //format_activity(measurement[ch_num].act,current.system,actstr);
        // Changed:
        //format_activity(measurement[ch_num].act, CI, actstr);
        //format_activity(measurement[ch_num].act0, CI, actstr);
        format_activity(activity, current.system, actstr);
        strcpy(&measurement[ch_num].actstr[0],actstr);
        measurement[ch_num].display_flag = TRUE;
        measurement[ch_num].display_flag_2 = TRUE;
    }
    


static bool mo_bkg(short ch_num, bool *hiflag){
	// Locked: CI
	//if((current.system == CI && measurement[ch_num].act < -100.0e-6) ||
	//	(current.system == BQ && measurement[ch_num].act < -3.7e+6))
        // Changed:
	//if(measurement[ch_num].act < -100.0e-6)
        if(measurement[ch_num].act0 < -100.0e-6)
	{
		//display_text(30,40,"ERROR",0,MEDIUM,REV);
		display_medium_message(ADD_NUCLIDE_6,40,0,REV);
		contmsg();
		return FALSE;
	}
	plwait(FALSE)
	;
	// Locked: CI
	//if ((current.system == CI && measurement[ch_num].act > 1.0e-3) ||
	//	(current.system == BQ && measurement[ch_num].act > 37.0e+6))
	// Changed:
	//if (measurement[ch_num].act > 1.0e-3)
        if (measurement[ch_num].act0 > 1.0e-3)
	{
		//display_text(32,38,"MO HIGH",0,MEDIUM,NORMAL);
		display_medium_message(MOLY_11,41,0,NORMAL);
		if (!*hiflag)
		{
			//display_text(18,56,"Terminate? (Y/N)",0,SMALL,NORMAL);
			display_small_message(MOLY_17,56,0,NORMAL);
			if(yesorno()|| home_set())
				return FALSE;
			*hiflag = TRUE;
			erase_lines(56,8,0);
			//display_text(18,56,"ENTER to Accept",0,SMALL,NORMAL);
			display_small_message(AUTO_ZERO_8,56,0,NORMAL);
		}
	}
	return TRUE;
}

    void low_mo_bkg(short ch_num)
    {
        float actfact;
        float response;
        char actstr[12];
        float lowbkg = bkgdata[ch_num].lowbkg;
        float activity;

        if(bad_flag)
            return;
        
        if(bkgdata[ch_num].count < 10)
        {
            ++bkgdata[ch_num].count;
            return;
        }

        if(bkgdata[ch_num].count >= 10 && bkgdata[ch_num].count < 50)
        {
            bkgdata[ch_num].sum += measurement[ch_num].volts;
            measurement[ch_num].actbuf[bkgdata[ch_num].count - 10] =
                measurement[ch_num].volts;
            ++bkgdata[ch_num].count;
            measurement[ch_num].meas = bkgdata[ch_num].sum /
                (float)(bkgdata[ch_num].count - 10);
            response = measurement[ch_num].response;
            actfact = get_gainfact(0);
            // Locked: CI
            //if(current.system == BQ)
            //    actfact *= BQFACTOR;
            // Changed:
            //measurement[ch_num].act = (measurement[ch_num].meas - lowbkg) *
            //    actfact / response;
            measurement[ch_num].act0 = (measurement[ch_num].meas - lowbkg) *
            	actfact / response;

            activity = measurement[ch_num].act0;
            if(current.system == BQ) activity *= BQFACTOR;
            // Locked: CI
            //format_activity(measurement[ch_num].act,current.system,actstr);
            // Changed:
            //format_activity(measurement[ch_num].act, CI, actstr);
            //format_activity(measurement[ch_num].act0, CI, actstr);
            format_activity(activity, current.system, actstr);
            strcpy(&measurement[ch_num].actstr[0],actstr);
            measurement[ch_num].display_flag = TRUE;
            return;
        }    

        if(bkgdata[ch_num].count == 50)
        {    
            measurement[ch_num].nmeas = 40;
            measurement[ch_num].longavg = TRUE;
            set_nokey();
            plwait(FALSE);
            //display_text(18,56,"ENTER to Accept",0,SMALL,NORMAL);
            display_small_message(AUTO_ZERO_8,56,0,NORMAL);
            //assume good
            bkgdata[ch_num].status = TEST_GOOD;
            bkgdata[ch_num].count = 51;
        }    

        ismeas(ch_num);
        
        response = measurement[ch_num].response;
        actfact = get_gainfact(0); 
        // Locked: CI
        //if(current.system == BQ)
        //    actfact *= BQFACTOR;
        // Changed:
        //measurement[ch_num].act = (measurement[ch_num].meas - lowbkg) *
        //    actfact / response;
        measurement[ch_num].act0 = (measurement[ch_num].meas - lowbkg) *
        	actfact / response;
        activity = measurement[ch_num].act0;
        if(current.system == BQ) activity *= BQFACTOR;
        // Locked: CI
        //format_activity(measurement[ch_num].act,current.system,actstr);
        // Changed:
        //format_activity(measurement[ch_num].act, CI, actstr);
        //format_activity(measurement[ch_num].act0, CI, actstr);
        format_activity(activity, current.system, actstr);
        strcpy(&measurement[ch_num].actstr[0],actstr);
        measurement[ch_num].display_flag = TRUE;
        measurement[ch_num].display_flag_2 = TRUE;
        
        if((measurement[ch_num].meas + chamber[ch_num].bkg) < 0.1)
            bkgdata[ch_num].status = TEST_GOOD;
        else
            bkgdata[ch_num].status = BKG_HIGH;
    }
    
    static void display_mo_bkg(short ch_num,bool hiloflag)
    {


        if(hiloflag)
        {    
            if(bkgdata[ch_num].status == TEST_GOOD)
                display_text(40,38," OK ",0,BIG,NORMAL);
            if(bkgdata[ch_num].status == BKG_HIGH)
                display_text(40,38,"HIGH",0,BIG,NORMAL);
        }
        
        display_text(20,22,measurement[ch_num].actstr,0,BIG,NORMAL);
        
    }

    void init_mo_empty(short nuc_index, short ch_num)
    {

         NUCDATA nucdata;
         float bkg;

        bkgdata[ch_num].done = FALSE;
        bkgdata[ch_num].status = TEST_WAIT;

        //blank activity string for case of requesting background too early
        strcpy(&measurement[ch_num].actstr[0],"         ");

        NuclideData_getNuclide(nuc_index,&nucdata);
        chamber[ch_num].nucdata = nucdata;
        measurement[ch_num].response = nucdata.response[chamber_type(ch_num)];
        measurement[ch_num].mode = MOBKGMODE;

         erase_lines(16,48,0);

         respcor(ch_num,&measurement[ch_num].response);
         bkg = chamber[ch_num].bkg;
         bkgdata[ch_num].lowbkg = -bkg;
         bkgdata[ch_num].sum = 0.;
         bkgdata[ch_num].count = 0;


    }    

    void init_mo_assay(short nuc_index, short ch_num)
    {    
         NUCDATA nucdata;

         bkgdata[ch_num].done = FALSE;
         bkgdata[ch_num].status = TEST_WAIT;

         NuclideData_getNuclide(nuc_index,&nucdata);
         chamber[ch_num].nucdata = nucdata;
         measurement[ch_num].response = nucdata.response[chamber_type(ch_num)];
         measurement[ch_num].mode = MOASSMODE;
         erase_lines(16,30,0);
         respcor(ch_num,&measurement[ch_num].response);
         measurement[ch_num].idec = measurement[ch_num].idecmin;
         measurement[ch_num].act0 = 0.;
         // Removed:
         //measurement[ch_num].syst0 = -1;
         measurement[ch_num].kun0 = 0;
         strcpy(&measurement[ch_num].actstr0[0],"xxxxxx");
         bkgdata[ch_num].sum = 0.;
         bkgdata[ch_num].count = 0;
         bkgdata[ch_num].lowbkg = mobkg;
         
    }

    static  void print_nuclide_name(char printer)
    {
        short ch_num = current.main_chamber;
        char strng[90];

        lininit(strng,FALSE,printer);

        switch(printer)
        {        
        case OKI_PRINTER:
            font6e(TRUE);
            break;
        case ROLL_PRINTER:
        case SLIP_PRINTER:
            font17();
            break;
        case LX_PRINTER:
            bold(TRUE);
            eps_cpi(12);
            eps_dbl(TRUE);  //double to 6cpi
            break;
        case USB_EPS_PRINTER:    
            eps_cpi(12);
            break;
        case USB_PRINTER:
            pcl_cpi("6",TRUE);
            pcl_bold(TRUE);
            pcl_horizontal("360");
            break;
        }

        strncpy(&strng[1],&chamber[ch_num].nucdata.name[0],6);
        
        strcpy(&strng[20],"\r\n");
        pr_write(strng);
            
            

    }
    
    static void print_staffid(char printer)
    {

        char strng[90];
        char str[4];
        bool pflag = FALSE;
        short kp = 0;
        
        switch(printer)
        {        
         case OKI_PRINTER:
            font6e(FALSE); 
            fontnlq(TRUE);
            kp = 4;
            break;
        case USB_PRINTER:
            pcl_bold(FALSE);
            pcl_cpi("10",TRUE);
            pcl_horizontal("360");
            break;
        case ROLL_PRINTER:
        case SLIP_PRINTER:
            font35();
            break;
        case LX_PRINTER:    
            eps_dbl(FALSE);
            bold(FALSE);
            eps_cpi(10);
            break;
        }
        
        lininit(strng,pflag,printer);
        
        //strncpy(&strng[20],"BY:",3);
        //get_message(DAILY_13,&strng[kp + 20]);
        
        if (id_set())
        {
        	get_message(DAILY_13,&strng[kp + 20]);
            get_id_str(str);
            strncpy(&strng[kp + 24],str,2);
        }    
        pr_write(strng);


    }
    static  void print_nuclide_name_staffid(char printer)
    {
        short ch_num = current.main_chamber;
        char strng[90];
        char str[4];
		


        switch(printer)
        {
        case OKI_PRINTER:
			reverse_feed(1,printer);
            font6e(TRUE);
            break;
        case ROLL_PRINTER:
        case SLIP_PRINTER:
            font17();
            break;
        case LX_PRINTER:
            bold(TRUE);
            eps_cpi(12);
            eps_dbl(TRUE);  //double to 6cpi
            break;
        case USB_EPS_PRINTER:
            eps_cpi(12);
            break;
        case USB_PRINTER:
            pcl_cpi("6", TRUE);
            pcl_bold(TRUE);
            pcl_horizontal("360");
            break;
        }

		strng[0] = ' ';
        strncpy(&strng[1],&chamber[ch_num].nucdata.name[0],6);
		strng[7] = '\0';
		pr_write(strng);

     switch(printer)
        {
         case OKI_PRINTER:
            font6e(FALSE);
            fontnlq(TRUE);
            break;
        case USB_PRINTER:
            pcl_bold(FALSE);
            pcl_cpi("10",TRUE);
            //pcl_horizontal("360");
            break;
        case ROLL_PRINTER:
        case SLIP_PRINTER:
            font35();
            break;
        case LX_PRINTER:
            eps_dbl(FALSE);
            bold(FALSE);
            eps_cpi(10);
            break;
        }

		
     	 strncpy(strng,"              ",13);

	    //BY:
        get_message(DAILY_13,&strng[4]);

        if (id_set())
        {
            get_id_str(str);
            strncpy(&strng[8],str,2);
        }
		else
			strncpy(&strng[8],"  ",2);
		strncpy(&strng[10],"\r\n",2);
		strng[12] = '\0';
		
		

        pr_write(strng);



    }
