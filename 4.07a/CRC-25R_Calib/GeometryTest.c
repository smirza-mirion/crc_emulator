/*********************************************************************
  MODULE:	GEOMETRY TEST

  FILE:		GeometryTest.c

  DATE:	   08/22/07

    CALLED BY:
		enhanced_tests
  *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "keyboard.h"
#include "i2c.h"
#include "menu.h"
#include "printer.h"
#include "uart.h"
#include "chambfac.h"
#include "message.h"
#include "nuc.h"
#include <string.h>
#include <math.h>

	extern CURRENT current;
    extern CHAMBER chamber[];
    extern MEASUREMENT measurement[];

enum
{
    GEO_SYR = 1,
    GEO_VIAL,
};

typedef struct geomeas GEOMEAS;
	struct geomeas
	{
		float meas_act;
		float totvol;
		char act_str[10];
		float var;
	};

GEOMEAS geomeas[10];

static float enter_volume(char *vol_str,float min_vol);
//static short test_list(void);
//static void display_1_test(short i,short line);
static  void pr_geom(short num_meas, short ref_num,short sorv);
static  void pr_geomsec(short num_meas, short ref_num,short sorv);
static  void display_geom(short num_meas,short sorv);

bool new_cal_exist(short nuc_index);


#define MAX_VOL 99.

    void geometry_test(void)
	{


		short i;
		bool ynret;
		short nmeas;
		float vol0;	  		// initial volume
		float volin;
        float totvol;
		short sorv;
		bool  volok;
		float corr_act;
	    short ref_num;
		float ref_act;
		short num_meas;
		float min_vol;
        time_t time0;
        time_t time1;
        float act1;
        char prtype;
        char nucname[8];
        short nuc_index;
        //char kun;
        bool nucok;
        bool ok;
        short ch_num = current.main_chamber;
        short ch_type;
        float fOrigResponse;

        ch_type = chamber_type(ch_num);


		/* initialize */
		for(i = 0; i < 10; i++)
		{
			geomeas[i].totvol = -1;
			geomeas[i].var = -1;
		}
		// Removed: System is read at the beginning
        //EE_READ(syst, &current.system);

        do
        {
            if(ch_type == R_CHAMB)
            {
                //nuclide is Tc99m
                strcpy(nucname,"Tc99m");
                nuc_index = NuclideData_getIndexFromName(nucname);
                nucok = TRUE;
            }

            else
            {
                //user selects nuclide
                erase_screen();
                nucok = TRUE;       //assume OK
                nuc_index = get_nuclide(FALSE,-1);
                if(home_set())
                    return;
                NuclideData_getName(nuc_index,nucname);
            }

            fOrigResponse = NuclideData_getResponse(nuc_index, ch_type);
            ok = new_cal_exist(nuc_index);
            if(!ok && (fOrigResponse == 0.0))
            {
                nucok = FALSE;
                no_calnum_msg();
                if(home_set())
                    return;
            }
        }while(!nucok);

        sorv = display_menu(GEOMETRY_TEST_MENU);
        if(sorv == -1 || home_set())
            return;

        vol0 = enter_volume("INITIAL",0.1);
        if(home_set())
            return;


		for(nmeas = 0; nmeas < 10; nmeas++)
		{

            if(nmeas == 0)
            {
                geomeas[nmeas].totvol = vol0;
            }

            else
            {
                erase_screen();
                do
                {
                    volok = TRUE;
                    min_vol = 0.1;
                    volin = enter_volume("ADDED", min_vol);
                    if(home_set())
                        return;

                    totvol =  geomeas[nmeas - 1].totvol + volin;

                    if(totvol <=  MAX_VOL)
                        geomeas[nmeas].totvol = totvol;
                    else
                    {
                        beep();
                        erase_screen();
                        //display_text(0,22,"VOLUME",0,MEDIUM,NORMAL);
                        //display_text(0,34,"TOO LARGE",0,MEDIUM,NORMAL);
                        display_medium_message(GEOM_1,22,0,NORMAL);
                        display_medium_message(GEOM_19,34,0,NORMAL);
                        contmsg();
                        if(home_set())
                            return;
                        volok = FALSE;
                    }


                } while(!volok);

			}
			erase_screen();

            measure_activity(nucname);
            if(home_set())
                return;

	    // Changed:
            //kun = measurement[ch_num].kun;
			//act1 = measurement[ch_num].act / unitfact[kun - 1];
			act1 = measurement[ch_num].act0;
			if(nmeas == 0)
            {
				read_clock(&time0);
				corr_act = act1;
            }
			else
			{
				/* decay back to time 0 */
				read_clock(&time1);
				corr_act = nucdecay(act1,time1,time0, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
			}
			geomeas[nmeas].meas_act = corr_act;

			// Locked: CI
            //format_activity(act1, current.system, &geomeas[nmeas].act_str[0]);
			format_activity(act1, CI, &geomeas[nmeas].act_str[0]);

            num_meas = nmeas + 1;

			if(nmeas > 0 && nmeas < 9)
			{
				erase_screen();
				display_text(0,32,"Another Sample?",0,MEDIUM,NORMAL);
                display_medium_message(GEOM_3,32,0,NORMAL);
				//display_text(16,52,"Y OR N ",0,MEDIUM,NORMAL);
                display_medium_message(YES_OR_NO,52,0,NORMAL);
                ynret = yesorno();
                if(home_set())
                    return;
				if(!ynret)
					break;
			}

		} /* end nmeas loop */


		/* reference  is initial volume, activity */
		ref_act = geomeas[0].meas_act;

		/* calculate variations */
		for(i = 0; i < num_meas; i++)
		{
			geomeas[i].var = 100 *
			  (geomeas[i].meas_act - ref_act) / ref_act;

		}

        display_geom(num_meas,sorv);
        if(home_set())
            return;

        prtype = current.printer;
        do
		{
			if (!start_printer(prtype,1,FALSE,PAPER))
                break;

            pr_geom(num_meas, ref_num,sorv);

            ynret = print_again();
            if(home_set())
                return;
		}while(ynret);


	}

void Amulet_printGeometry(short num_meas, short ref_num, short sorv){
	char printer;

	printer = current.printer;
	if(start_printer(printer,1,FALSE,PAPER)){
		pr_geomsec(num_meas, ref_num, sorv);
	}
}


	static float enter_volume(char *vol_str,float min_vol)
		{
			float vol;

			 erase_screen();
			 //display_text(0,10,"Enter",0,MEDIUM,NORMAL);
             display_medium_message(EFF_12,10,0,NORMAL);
			 display_text(0,22,(char *)vol_str,0,MEDIUM,NORMAL);
			 //display_text(0,34,"Volume In ml",0,MEDIUM,NORMAL);
             display_medium_message(LAB_15,34,0,NORMAL);

             vol = inpflt(8,52,MEDIUM,0,4,min_vol,99.);

			 return(vol);
		 }

/*    static  short test_list(void)
 - 	{
 -
 - 		short i,i2;
 - 		short i1;
 - 		short k;
 - 		bool start_flag;
 - 		short maxtest;
 - 		short scr_num;
 - 		char ch;
 -
 - 		maxtest = 0;
 -
 - 		for(i = 0; i < 10; i++)
 - 		{
 - 			if(geomeas[i].totvol != -1)
 - 				++maxtest;
 - 		}
 -
 -
 - 		i1 = 0;
 - 		i2 = 5;
 - 		if(i2 > maxtest)
 - 			i2 = maxtest;
 - 		scr_num = 0;
 - 		for(;;)
 - 		{
 - 			erase_screen();
 - 			//display_text(24,0,"SELECT BASE",0,SMALL,NORMAL);
 -             display_small_message(GEOM_4,0,0,NORMAL);
 -
 - 			if(i2 < maxtest)
 -                 //display_text(0,56,"DOWN: Next",0,SMALL,NORMAL);
 -                 display_small_message(GEOM_5,56,0,NORMAL);
 - 			if(scr_num > 0)
 -                 //display_text(72,56,"UP: Prev",0,SMALL,NORMAL);
 -                 display_small_message(GEOM_6,56,0,NORMAL);
 - 			for(i = i1; i < i2; i++)
 - 				display_1_test(i,i - i1 + 1);
 - 			do
 - 			{
 - 				ch = keyin();
 -                 if(home_set())
 -                     return (-1);
 - 				start_flag = FALSE;
 - 				switch (ch)
 - 				{
 - 				case ONE:
 - 				case TWO:
 - 				case THREE:
 - 				case FOUR:
 - 				case FIVE:
 - 					k = ch -1 + 5 * scr_num;
 -                     if(k >= maxtest)
 -                     {
 -                         beep();
 -                         break;
 -                     }
 - 					return(k);
 -
 -
 - 				case UP:
 - 					if (scr_num == 0)
 - 					{
 - 						beep();
 - 						break;
 - 					}
 - 					i1 -= 5;
 - 					if(i1 < 0)
 - 						i1 = 0;
 - 					i2 = i1 + 5;
 - 					if(i2 > maxtest)
 - 						i2 = maxtest;
 - 					--scr_num;
 - 					start_flag = TRUE;
 - 					break;
 -
 - 				case DOWN:
 - 					if(i2 == maxtest)
 - 					{
 - 						beep();
 - 						break;
 - 					}
 - 					i1 += 5;
 - 					i2 = i1 + 5;
 - 					if(i2 > maxtest)
 - 						i2 = maxtest;
 - 					++scr_num;
 - 					start_flag = 1;
 - 					break;
 -
 - 				default:
 - 					beep();
 - 					break;
 -
 - 				}
 - 			}while(!start_flag);
 - 		}
 -
 - 	}
 -
 -
 -     static void display_1_test(short i, short line)
 - 	{
 - 		short nc;
 - 		char buffer[10];
 -         char strng[30];
 -
 -
 - 		strcpy(strng,"  .                 ");
 -         nc = sprintf(buffer,"%d",line);
 - 		strncpy(&strng[2 - nc],buffer,nc);
 -         nc = sprintf(buffer,"%5.1f",geomeas[i].totvol);
 - 		strncpy(&strng[9 - nc],buffer,nc);
 - 		strncpy(&strng[9],"ml",2);
 - 		strncpy(&strng[12],&geomeas[i].act_str[0],6);
 - 		display_text(0,8 * line,strng,0,SMALL,0);
 - 		if(geomeas[i].act_str[6] == MU)
 -             write_special((char *)&smu,108,8 * line,SMALL,0);
 - 		else
 -         {
 -             buffer[0] = geomeas[i].act_str[6];
 -             buffer[1] = '\0';
 -             display_text(108,8 * line,buffer,0,SMALL,NORMAL);
 -         }
 - 		display_text(114,8 * line,&geomeas[i].act_str[7],0,SMALL,NORMAL);
 -
 - 	}*/

    static	void pr_geom(short num_meas, short ref_num,
					   short sorv)
	{

		short i;
		short nc;
		char buffer[10];
        char prtype;
        char strng[90];
        char message[90], message2[90], message3[90];

        prtype = current.printer;


        prhead_language(prtype);
		feed(1,prtype);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(TRUE);

        lininit(strng,TRUE,prtype);
		//strncpy(&strng[4],"GEOMETRY TEST",13);
        //get_message(GEOM_13,&strng[4]);
        get_amulet_message(L_GEOMETRY_TEST, message);    // "GEOMETRY TEST"
        strncpy(&strng[4], message, strlen(message));
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(FALSE);

        lininit(strng,TRUE,prtype);
		//strncpy(&strng[4],"Using:",6);
        //get_message(GEOM_14,&strng[4]);
        get_amulet_message(L_USING, message);    // "Using:"
		if(sorv == GEO_SYR){
			//strncpy(&strng[11],"Syringe",7);
            //get_message(GEOM_15,&strng[11]);
			get_amulet_message(L_SYRINGE, message2);    // "Syringe"
		}else{
			//strncpy(&strng[11],"Vial",4);
            //get_message(GEOM_16,&strng[11]);
			get_amulet_message(L_VIAL, message2);    // "Vial"
		}
		sprintf(message3, "%s %s", message, message2);
		strncpy(&strng[4], message3, strlen(message3));
        pr_write(strng);
		feed(1,prtype);


        lininit(strng,TRUE,prtype);
		//strncpy(&strng[0],"#   Volume    Assay    Variation",32);
        //get_message(GEOM_17,&strng[0]);
        get_amulet_message(L_VOLUME_ASSAY_VARIATION, message);    // "#   Volume    Assay    Variation"
        strncpy(&strng[0], message, strlen(message));
        pr_write(strng);

		feed(1,prtype);
		for(i = 0; i < num_meas; i++)
		{

            lininit(strng,TRUE,prtype);
			/* sample number */
            nc = sprintf(buffer,"%d",i + 1);
			strncpy(&strng[0],buffer,nc);

			/* volume */
            nc = sprintf(buffer,"%5.1f",geomeas[i].totvol);
			strncpy(&strng[9 - nc],buffer,nc);
			strncpy(&strng[9],"ml",2);

			/* activity*/
			strncpy(&strng[12],&geomeas[i].act_str[0],9);
			if (strng[18] == MU)
            {
                if(prtype == USB_EPS_PRINTER)
                    strng[18] = 'u';
                else
                    strng[18] = MU_PRT;
            }

			/* variation */
			if(i == ref_num){
				//strncpy(&strng[26],"BASE",4);
                //get_message(GEOM_18,&strng[26]);
				get_amulet_message(L_BASE, message);    // "BASE"
				strncpy(&strng[26], message, strlen(message));
			}
			else
			{
                nc = sprintf(buffer,"%4.1f",geomeas[i].var);
				strncpy(&strng[29 - nc],buffer,nc);
				strng[30] = '%';
			}
            pr_write(strng);

		}

		formfeed(prtype);

	}

    static	void pr_geomsec(short num_meas, short ref_num,
					   short sorv)
	{

		short i;
		short nc;
		char buffer[10];
        char prtype;
        char strng[90];
        char message[90], message2[90], message3[90];

        prtype = current.printer;


        prhead_languagesec(prtype);
		feed(1,prtype);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(TRUE);

        lininit(strng,TRUE,prtype);
		//strncpy(&strng[4],"GEOMETRY TEST",13);
        //get_message(GEOM_13,&strng[4]);
        get_amulet_message(L_GEOMETRY_TEST, message);    // "GEOMETRY TEST"
        strncpy(&strng[4], message, strlen(message));
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(FALSE);

        lininit(strng,TRUE,prtype);
		//strncpy(&strng[4],"Using:",6);
        //get_message(GEOM_14,&strng[4]);
        get_amulet_message(L_USING, message);    // "Using:"
		if(sorv == GEO_SYR){
			//strncpy(&strng[11],"Syringe",7);
            //get_message(GEOM_15,&strng[11]);
			get_amulet_message(L_SYRINGE, message2);    // "Syringe"
		}else{
			//strncpy(&strng[11],"Vial",4);
            //get_message(GEOM_16,&strng[11]);
			get_amulet_message(L_VIAL, message2);    // "Vial"
		}
		sprintf(message3, "%s %s", message, message2);
		strncpy(&strng[4], message3, strlen(message3));
        pr_write(strng);
		feed(1,prtype);


        lininit(strng,TRUE,prtype);
		//strncpy(&strng[0],"#   Volume    Assay    Variation",32);
        //get_message(GEOM_17,&strng[0]);
        get_amulet_message(L_VOLUME_ASSAY_VARIATION, message);    // "#   Volume    Assay    Variation"
        strncpy(&strng[0], message, strlen(message));
        pr_write(strng);

		feed(1,prtype);
		for(i = 0; i < num_meas; i++)
		{

            lininit(strng,TRUE,prtype);
			/* sample number */
            nc = sprintf(buffer,"%d",i + 1);
			strncpy(&strng[0],buffer,nc);

			/* volume */
            nc = sprintf(buffer,"%5.1f",geomeas[i].totvol);
			strncpy(&strng[9 - nc],buffer,nc);
			strncpy(&strng[9],"ml",2);

			/* activity*/
			strncpy(&strng[12],&geomeas[i].act_str[0],9);
			if (strng[18] == MU)
            {
                if(prtype == USB_EPS_PRINTER)
                    strng[18] = 'u';
                else
                    strng[18] = MU_PRT;
            }

			/* variation */
			if(i == ref_num){
				//strncpy(&strng[26],"BASE",4);
                //get_message(GEOM_18,&strng[26]);
				get_amulet_message(L_BASE, message);    // "BASE"
				strncpy(&strng[26], message, strlen(message));
			}
			else
			{
                nc = sprintf(buffer,"%4.1f",geomeas[i].var);
				strncpy(&strng[29 - nc],buffer,nc);
				strng[30] = '%';
			}
            pr_write(strng);

		}

		formfeed(prtype);

	}

    static  void display_geom(short num_meas,short sorv)
    {

        short i,j,k;
        char buffer[30];
        short nscreens;
        short line;
        char fmt_str[10];

        nscreens = num_meas / 4;
        if((num_meas % 4) != 0)
            ++nscreens;

        for(j = 0; j < nscreens; j++)
        {
            erase_screen();
            //display_text(24,0,"GEOMETRY TEST",0,SMALL,NORMAL);
            display_small_message(GEOM_7,0,0,NORMAL);

            strcpy(buffer,"Using: ");
            display_small_message(GEOM_8,8,0,NORMAL);
            if(sorv == GEO_SYR)
                //strcat(buffer,"Syringe");
                display_small_message(GEOM_9,8,0,NORMAL);
            else
                //strcat(buffer,"Vial");
                display_small_message(GEOM_10,8,0,NORMAL);

            //display_text(0,8,buffer,0,SMALL,NORMAL);

            //display_text(0,16,"Volume  Assay    %Var",0,SMALL,NORMAL);
            display_small_message(GEOM_11,16,0,NORMAL);

            for(k = 0; k < 4; k++)
            {
                i = 4 * j + k;
                if(i < num_meas)
                {
                    line = k + 3;

                    //volume
                    strcpy(fmt_str,"%5.1f");
                    if(geomeas[i].totvol < 100.)
                        strcpy(fmt_str,"%4.1f");
                    sprintf(buffer,fmt_str,geomeas[i].totvol);
                    strcat(buffer,"ml");
                    display_text(0, 8 * line, buffer,0,SMALL,NORMAL);

                    // activity
                    display_act_small(&geomeas[i].act_str[0],6,line);

                    // variation
                    if(i == 0)
                        //strcpy(buffer," BASE");
                        display_small_message(GEOM_12, 8 * line,0,NORMAL);
                    else
                    {
                        //sprintf(buffer,"%4.1f",geomeas[i].var);
                        strcpy(fmt_str,"%4.1f");
                        if(fabs(geomeas[i].var) > 100.)
                           strcpy(fmt_str,"%5.0f");
                        sprintf(buffer,fmt_str,geomeas[i].var);
                        display_text(96, 8 * line,buffer,0,SMALL,NORMAL);
                    }
                }

            }
            contmsg();
            if(home_set())
                return;
        }

        erase_screen();
    }


    void measure_activity(char *nucname)
    {
        bool meas_done;
        bool okflag;
        char ch;
        short nuc_index;
        short ch_num = current.main_chamber;
        NUCDATA  nucdata;
        float fUserResponse;
        short ch_type;

        ch_type = chamber_type(ch_num);
        erase_screen();


        nuc_index = NuclideData_getIndexFromName(nucname);
        fUserResponse = NuclideData_getUserResponse(nuc_index, ch_type);
        NuclideData_getNuclide(nuc_index,&nucdata);

        chamber[ch_num].nucdata = nucdata;

        if(fUserResponse != 0.0) measurement[ch_num].response = fUserResponse;
        else measurement[ch_num].response = nucdata.response[ch_type];

        respcor(ch_num,&measurement[ch_num].response);
        nucinit(ch_num,TRUE);
        disp_nuclide();

        okflag = FALSE;


        //display_text(0,56,"ENTER to Accept",0,SMALL,0);
        display_small_message(AUTO_ZERO_8,56,0,NORMAL);
        set_nokey();


         meas_done = FALSE;
         do
         {
             ch = getkey();
             if(home_set())
                 return;

             switch (ch)
             {
             case NOKEY:
                 break;

             case DOWN:
                downbut();
                 break;

             case OK:
                 if (!okflag)
                 {
                     beep();
                     break;
                 }
                 meas_done = TRUE;
                 break;

            default:
                beep();
                break;

            }

            if(!meas_done)
            {

                if (measurement[ch_num].display_flag)
                {
                    activity_to_screen();
                    measurement[ch_num].display_flag = FALSE;

                    okflag = TRUE;
                }
             }
         }while(!meas_done);

 }
