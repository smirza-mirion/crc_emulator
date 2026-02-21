/*********************************************************************
  MODULE:	DEFINE LINEARITY TEST for CRC-25

  FILE:		DefineLinearity_25.c

  DATE:	    03/26/08  -- # of Calicheck tubes & # of standard tests changed to 5 - 12

  User selects and defines linearity test

  *************************************************************************/
#include "crc.h"
#include "linearity.h"
#include "menu.h"
#include "i2c.h"
#include "screen.h"
#include "keyboard.h"
#include "printer.h"
#include "uart.h"
#include "chambfac.h"
#include "message.h"
#include "nuc.h"
#include <string.h>

typedef struct calmeas CALMEAS;
struct calmeas
{
    float meas_act;
    char act_str[10];
};

static bool define_standard_linearity_test(LINDEF *lindef);
static void linearity_calibration(LINDEF *lin_def);
static void tubes_tests(LINDEF *lindef);
void display_act_small(char *actstr, short col, short line);
//static void pr_lincalib(CALMEAS *calmeas, LINDEF *lin_def,char prtype);
void get_calicheck_tube(short itube, char *tube);

void pr_lincalib(short linearity_type, CALMEAS *calmeas, LINDEF *lin_def, bool finished, char prtype);
/*
 - //Calicheck tube names
 -     const char *cal_tube_name[] = 
 -     {
 -       "Black Only",
 -       "Black + Red",
 -       "Black + Orange",
 -       "Black + Yellow",
 -       "Black + Green",
 -       "Black + Blue",
 -       "Black + Purple",
 -       "Black/Purple/Red",
 -       "Black/Purple/Orange",
 -       "Black/Purple/Yellow",
 -       "Black/Purple/Green",
 -       "Black/Purple/Blue"
 -     };
 - */
    // Lineator tube names
    const char *lin_tube_name[] = 
    {
      "1",
      "1 + 2",
      "1 + 3",
      "1 + 2, 3",
      "1 + 4",
      "1 + 2, 4",
      "1 + 3, 4",
      "1 + 2, 3, 4"
    };

    extern CURRENT current;
    
	short define_linearity_test(void)
	{
/*		LINDEF lin_def;
		short lintype;
		bool iret;
        bool yn;
        short nuc_index;
        bool nucok;
        char nucname[8];
        bool ok;
        //NUCDATA nucdata;
        float fOrigResponse;
        short ch_num = current.main_chamber;
        short ch_type;

        ch_type = chamber_type(ch_num);
        
		
        EE_READ(lindef,(uchar *)&lin_def);
        erase_screen();

		if(lin_def.type != -1)
		{
//			display_text(0,0,"LINEARITY TEST",0,MEDIUM,NORMAL);
// - 			display_text(0,12,"ALREADY DEFINED",0,MEDIUM,NORMAL);
// - 			display_text(0,24,"NEW DEFINITION?",0,MEDIUM,NORMAL);
            display_medium_message(LIN_1,0,0,NORMAL);
            display_medium_message(LIN_2,12,0,NORMAL);
            display_medium_message(LIN_3,24,0,NORMAL);
			//display_text(0,42,"Y OR N",0,MEDIUM,NORMAL);
            display_medium_message(YES_OR_NO,42,0,NORMAL);

            yn = yesorno();
            if(home_set())
                return -1;
			if(!yn)
				return 0;
		}


		lintype = display_menu(LIN_TEST_MENU);
        if(lintype == -1)
            return 0;
        if(home_set())
            return -1;
        
        lin_def.type = lintype;
		lin_def.num = 8;	// default

        do
        {    
            if(ch_type == R_CHAMB)
            {    
                //nuclide is Tc99m
                nuc_index = NuclideData_getIndexFromName("Tc99m");
                nucok = TRUE;
            }    
    
            else
            {    
                //user selects nuclide
                erase_screen();
                nucok = TRUE;   //assume OK
                nuc_index = get_nuclide(FALSE,-1);
                if(home_set())
                    return 0;
                NuclideData_getName(nuc_index,nucname);
            }
            fOrigResponse = NuclideData_getResponse(nuc_index, ch_type);
            ok = new_cal_exist(nuc_index);
            if(!ok && (fOrigResponse == 0.0))
            {
                nucok = FALSE;
                no_calnum_msg();
                if(home_set())
                    return 0;
            }   
        }while(!nucok);
        lin_def.nuc_index = nuc_index;
        
		do
		{
			if(lintype == STANDARD || lintype == CALICHECK)
				tubes_tests(&lin_def);
            if(home_set())
                return 0;

			if(lintype == STANDARD)
			{
				iret = define_standard_linearity_test(&lin_def);
                if(home_set())
                    return 0;
				if(iret)
                    EE_WRITE(lindef,(uchar *)&lin_def);
			}

			else
			{
				linearity_calibration(&lin_def);
                iret = TRUE;
				// lindef written to eeprom in linearity_calibration
                if(home_set())
                    return 0;
			}
		
		} while(!iret);

		return 0; */
		return 0;
	}

/*static void tubes_tests(LINDEF *lindef)
	{
		short num;

		erase_screen();
		//display_text(0,10,"ENTER NUMBER",0,MEDIUM,NORMAL);
		//display_text(0,22,"OF ",0,MEDIUM,NORMAL);
        display_medium_message(LIN_4,10,0,NORMAL);
        display_medium_message(LIN_5,22,0,NORMAL);
		if(lindef -> type == STANDARD)
            //display_text(24,22,"TESTS",0,MEDIUM,NORMAL);
            display_medium_message(LIN_6,22,0,NORMAL);
		else
            //display_text(24,22,"TUBES",0,MEDIUM,NORMAL);
            display_medium_message(LIN_7,22,0,NORMAL);
		display_text(0,34,"5 - 12",0,MEDIUM,NORMAL);

        num = inpint(8,46,MEDIUM,0,2,5,12,'\0');

		
		lindef -> num = num;
	}*/

/*static bool define_standard_linearity_test(LINDEF *lindef)
	{
   		short itest;
		short num_tests;
		char str[8];
		short hours[12];
		short ival;
		short min_hours;
		short nc;
        char strng[30];
        short ch_num = current.main_chamber;
        

		num_tests = lindef -> num;

		hours[0] = 0;
			
		erase_screen();
//		display_text(0,0,"STANDARD",0,MEDIUM,NORMAL);
// - 		display_text(0,11,"LINEARITY TEST",0,MEDIUM,NORMAL);
// - 		display_text(0,22,"TIME FOR",0,MEDIUM,NORMAL);
        display_medium_message(LIN_8,0,0,NORMAL);
        display_medium_message(LIN_9,11,0,NORMAL);
        display_medium_message(LIN_10,22,0,NORMAL);
		min_hours = 0;

		for(itest = 1; itest < num_tests; itest++)
		{
			//display_text(0,33,"MEASUREMENT ",0,MEDIUM,NORMAL);
            display_medium_message(LIN_11,33,0,NORMAL);
            nc = sprintf(str,"%d",itest + 1);
			display_text(8 * (14 - nc),33,str,0,MEDIUM,NORMAL);
			//display_text(0,44,"IN HOURS",0,MEDIUM,NORMAL);
            display_medium_message(LIN_12,44,0,NORMAL);

            ival = inpint(8,55,MEDIUM,0,3,min_hours,255,'\0');
            if(home_set())
                return FALSE;
			hours[itest] = ival;
			min_hours = ival + 1;
            erase_text_line(55,MEDIUM,0);
		}


		erase_screen();
		//display_text(0,0,"HOURS FOR TESTS",0,SMALL,NORMAL);
        display_small_message(LIN_13,0,0,NORMAL);
   	    strcpy(strng,"1:  0  2:     3:    ");
        nc = sprintf(str,"%d",hours[1]);
		strncpy(&strng[13 - nc],str,nc);
        nc = sprintf(str,"%d",hours[2]);
		strncpy(&strng[20 - nc],str,nc);
		display_text(0,8,strng,0,SMALL,NORMAL);
		strcpy(strng,"4:     5:     6:    ");
        nc = sprintf(str,"%d",hours[3]);
		strncpy(&strng[6 - nc],str,nc);
        nc = sprintf(str,"%d",hours[4]);
		strncpy(&strng[13 - nc],str,nc);
//        nc = sprintf(str,"%d",hours[5]);
// - 		strncpy(&strng[20 - nc],str,nc);
// - 		display_text(0,16,strng,0,SMALL,NORMAL);
// - 		strcpy(strng,"7:     8:     9:    ");
// -         nc = sprintf(str,"%d",hours[6]);
// - 		strncpy(&strng[6 - nc],str,nc);
// -         nc = sprintf(str,"%d",hours[7]);
// - 		strncpy(&strng[13 - nc],str,nc);

        if(num_tests >= 6)
        {    
            nc = sprintf(str,"%d",hours[5]);
            strncpy(&strng[20 - nc],str,nc);
        }
        display_text(0,16,strng,0,SMALL,NORMAL);
            
        if(num_tests >= 7)
        {
            display_text(0,16,strng,0,SMALL,NORMAL);
            strcpy(strng,"7:     8:     9:    ");
            nc = sprintf(str,"%d",hours[6]);
            strncpy(&strng[6 - nc],str,nc);
            if(num_tests >= 8)
            {    
                nc = sprintf(str,"%d",hours[7]);
                strncpy(&strng[13 - nc],str,nc);
            }    
            if(num_tests >= 9)
            {
                nc = sprintf(str,"%d",hours[8]);
                strncpy(&strng[20 - nc],str,nc);
            }
            display_text(0,24,strng,0,SMALL,NORMAL);
            
            if(num_tests >= 10)
            {
                strcpy(strng,"10:    11:    12:   ");
                nc = sprintf(str,"%d",hours[9]);
                strncpy(&strng[6 - nc],str,nc);
                if(num_tests >= 11)
                {
                    nc = sprintf(str,"%d",hours[10]);
                    strncpy(&strng[13 - nc],str,nc);
                }
                if(num_tests == 12)
                {
                    nc = sprintf(str,"%d",hours[11]);
                    strncpy(&strng[20 - nc],str,nc);
                }
                display_text(0,32,strng,0,SMALL,NORMAL);
            }
        }    
        

		//display_text(0,48,"OK? Y OR N",0,SMALL,NORMAL);
        display_small_message(SM_OK_MSG,48,0,NORMAL);
		if(!yesorno() || home_set())
			return FALSE;
		
		// initialize standlin to no tests done 
		for(itest = 0; itest < num_tests; itest++)
		{
			standlin[ch_num][itest].meas_flag = 0;
			lindef -> hours[itest] = hours[itest];
		}

		if(num_tests < 12)
		{
			for(itest = num_tests; itest < 12; itest++)
				standlin[ch_num][itest].meas_flag = -1;
		}

		return TRUE;

    return TRUE;
	}*/

/*static void linearity_calibration(LINDEF *lin_def)
	{
		short i;
		bool ynret;
        short nmeas;
		short num_meas;
		bool meas_ok;
		char str[8];
		float factors[12];
		CALMEAS calmeas[12];
        char prtype;
        //NUCDATA nucdata;
        char nucname[8];
        short nuc_index;
        //float act;
        float act1;
        //char kun;
        short ch_num = current.main_chamber;

        // Removed: System is read at the beginning
        //EE_READ(syst, &current.system);
		num_meas = lin_def -> num;
        
        nuc_index = lin_def -> nuc_index;
        NuclideData_getName(nuc_index, nucname);
        
		for(nmeas = 0; nmeas < num_meas; nmeas++)
		{
			do
			{
				erase_screen();
				//display_text(0,8,"Measure Tube # ",0,SMALL,NORMAL);
                display_small_message(LIN_14,8,0,NORMAL);
                sprintf(str,"%d",nmeas + 1);
				display_text(90,8,str,0,SMALL,NORMAL);
				if(lin_def -> type == LINEATOR)
                    display_text(0,16,(char *)&lin_tube_name[nmeas][0],0,SMALL,NORMAL);
				else
                    //display_text(0,16,(char *)&cal_tube_name[nmeas][0],0,SMALL,NORMAL);
                    display_small_message(LIN_57 + nmeas,16,0,NORMAL);
				contmsg();
                if(home_set())
                    return;

				erase_screen();
                measure_activity(nucname);
                if(home_set())
                    return;

		// Changed:
                //act = measurement[ch_num].act;
                //kun = measurement[ch_num].kun;
				//act1 = act / unitfact[kun - 1];
				act1 = measurement[ch_num].act0;

				calmeas[nmeas].meas_act = act1;
				strcpy(&calmeas[nmeas].act_str[0],&measurement[ch_num].actstr[0]);
				factors[nmeas] = calmeas[0].meas_act / calmeas[nmeas].meas_act;

				erase_screen();
				//display_text(0,0,"Tube # ",0,SMALL,NORMAL);
                display_small_message(LIN_15,0,0,NORMAL);
                sprintf(str,"%d",nmeas + 1);
				display_text(42,0,str,0,SMALL,NORMAL);
				if(lin_def -> type == LINEATOR)
                    display_text(0,8,(char *)&lin_tube_name[nmeas][0],0,SMALL,NORMAL);
				else
                    //display_text(0,8,(char *)&cal_tube_name[nmeas][0],0,SMALL,NORMAL);
                    display_small_message(LIN_57 + nmeas,8,0,NORMAL);

				//display_text(0,24,"Activity: ",0,SMALL,NORMAL);
                display_small_message(LIN_16,24,0,NORMAL);
                display_act_small( &calmeas[nmeas].act_str[0],10,3);
				
				if(lin_def -> type == LINEATOR)
                    //display_text(0,32,"Initial Factor",0,SMALL,NORMAL);
                    display_small_message(LIN_17,32,0,NORMAL);
				else
                    //display_text(0,32,"Calibration Factor",0,SMALL,NORMAL);
                    display_small_message(LIN_18,32,0,NORMAL);
                sprintf(str,"%5.2f",factors[nmeas]);
				display_text(12,40,str,0,SMALL,NORMAL);
				//display_text(0,48,"OK? Y OR N",0,SMALL,NORMAL);
                display_small_message(SM_OK_MSG,48,0,NORMAL);

				meas_ok = yesorno();
                if(home_set())
                    return;

			} while(!meas_ok);

			

		} // end nmeas loop 

		// save factors to eeprom 
		for(i = 0; i < num_meas; i++)
            lin_def -> lin_cal_factors[i] = factors[i];
        // save test definition
        EE_WRITE(lindef,(uchar *)lin_def);


		erase_screen();
        //EE_READ(print[0],(uchar *)&prtype);
        prtype = current.printer;
        do
		{
			if (!start_printer(prtype,1,FALSE,PAPER))
                break;
			
            pr_lincalib(calmeas,lin_def,prtype);
			
			ynret = print_again();
			}while(ynret);
	}*/

void Amulet_printCalib(short linearity_type, CALMEAS *calmeas, LINDEF *lin_def, bool finished){
	char prtype;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		pr_lincalib(linearity_type, calmeas, lin_def, finished, prtype);
	}
}

void pr_lincalib(short linearity_type, CALMEAS *calmeas, LINDEF *lin_def, bool finished, char prtype)
	{
		short i;
		//char head[3][20];
		char head[3][26];
		short nc;
		char buffer[10];
		short num_meas;
        char strng[90];
        char tubename[25];
        char nucname[8];
        char ch_num = current.main_chamber;
        char buffer2[90];
		char msg[52];
		short len;

		//num_meas = lin_def -> num;
        switch(linearity_type){
			case LINEATOR:
				num_meas = lin_def->num_Lin[ch_num];
				break;

			case CALICHECK:
				num_meas = lin_def->num_Cali[ch_num];
				break;
        }

        prhead_languagesec(prtype);

		feed(1,prtype);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(TRUE);
        lininit(strng,TRUE,prtype);
		//if(lin_def -> type == CALICHECK)
        if(linearity_type == CALICHECK)
		{	
			//strncpy(&strng[2],"CALICHECK TEST CALIBRATION",26);
            //get_message(LIN_44,&strng[2]);
			get_amulet_message(L_CALICHECK_TEST_CALIBRATION,msg);    // "CALICHECK TEST CALIBRATION"
			strncpy(&strng[2],msg,strlen(msg));
		}	
		
		else
		{	
			//get_message(LIN_45,&strng[2]);
			get_amulet_message(L_LINEATOR_TEST_CALIBRATION,msg);    // "LINEATOR TEST CALIBRATION "
			strncpy(&strng[2],msg,strlen(msg));
		}	
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
			bold(FALSE);
        
        lininit(strng,TRUE,prtype);
        //Nuclide:
        //get_message(DIAG_51,&strng[2]);
		get_amulet_message(L_PR_NUCLIDE,msg);    // "NUCLIDE"
		nc = strlen(msg);
		strncpy(&strng[2],msg,nc);
		
		
        //strng[9] = ':';
        strng[11] = ':';
        //NuclideData_getName(lin_def -> nuc_index,nucname);
        switch(linearity_type){
			case LINEATOR:
				NuclideData_getName(lin_def->nuc_index_Lin[ch_num], nucname);
				break;

			case CALICHECK:
				NuclideData_getName(lin_def->nuc_index_Cali[ch_num],nucname);
				break;
        }
        //strncpy(&strng[11],nucname,6);
        strncpy(&strng[13],nucname,6);
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        switch(linearity_type){
			case LINEATOR:
				//sprintf(buffer2, "Chamber   S/N: %s", &(lin_def->chamb_num_Lin[ch_num][0]));
				get_amulet_message(L_PR_CHAMBER_SN,msg);    // "Chamber   S/N:"
				sprintf(buffer2, "%s %s", msg,&(lin_def->chamb_num_Lin[ch_num][0]));
				break;

			case CALICHECK:
				//sprintf(buffer2, "Chamber   S/N: %s", &(lin_def->chamb_num_Cali[ch_num][0]));
				get_amulet_message(L_PR_CHAMBER_SN,msg);    // "Chamber   S/N:"
				sprintf(buffer2, "%s %s", msg,&(lin_def->chamb_num_Cali[ch_num][0]));
				break;
        }
        strncpy(&strng[2], buffer2, strlen(buffer2));
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        switch(linearity_type){
			case LINEATOR:
				sprintf(buffer2, "Lineator  S/N: %s", &(lin_def->serial_num_Lin[ch_num][0]));
				break;

			case CALICHECK:
				sprintf(buffer2, "Calicheck S/N: %s", &(lin_def->serial_num_Cali[ch_num][0]));
				break;
        }
        strncpy(&strng[2], buffer2, strlen(buffer2));
        pr_write(strng);

		feed(1,prtype);

        //strncpy(&head[0][0],"Tube:               ",20);
        strncpy(&head[0][0],"                    ",20);
        //get_message(LIN_46,&head[0][0]);
		get_amulet_message(L_TUBE,msg);    // "Tube"
		strcat(msg,":");
		nc = strlen(msg);
		strncpy(&head[0][0],msg,nc);

        if(finished){
        	//strncpy(&head[1][0],"Activity:           ",20);
        	strncpy(&head[1][0],"                    ",20);
        	//get_message(LIN_47,&head[1][0]);
			get_amulet_message(L_ACTIVITY,msg);    // "Activity"
			strcat(msg,":");
			nc = strlen(msg);
			strncpy(&head[1][0],msg,nc);
        }

        strncpy(&head[2][0],"                     ",24);
		//if(lin_def -> type == CALICHECK)
        if(linearity_type == CALICHECK)
		{	
			//strncpy(&head[2][0],"Calibration Factor: ",20);
            //get_message(LIN_48,&head[2][0]);
			get_amulet_message(L_CALIBRATION_FACTOR,msg);    // "Calibration Factor: "
			strncpy(&head[2][0],msg,strlen(msg));
			len = strlen(msg);

		}	
		else
		{	
			//strncpy(&head[2][0],"Initial Factor:     ",20);
            //get_message(LIN_49,&head[2][0]);
			get_amulet_message(L_INITIAL_FACTOR,msg);    // "Initial Factor:     "
			strncpy(&head[2][0],msg,strlen(msg));
		}	

		for(i = 0; i < num_meas; i++)
		{
            lininit(strng,TRUE,prtype);
			strncpy(&strng[1],&head[0][0],6);
			//if(lin_def -> type == CALICHECK)
			if(linearity_type == CALICHECK)
            {    
                //strcpy(tubename,&cal_tube_name[i][0]);
                strcpy(tubename,"                   ");
                //get_message(LIN_69 + i,tubename);
				//get_amulet_message(L_PR_TUBE_1_CALICHECK + i,tubename);    // "Black"
				get_calicheck_tube(i,tubename);
				
            }    
			else
                strcpy(tubename,&lin_tube_name[i][0]);

            strncpy(&strng[7],tubename,strlen(tubename));
            pr_write(strng);

            if(finished){
            	lininit(strng,TRUE,prtype);
            	strncpy(&strng[1],&head[1][0],10);
            	//strncpy(&strng[20],&calmeas[i].act_str[0],9);
            	strncpy(&strng[20],&calmeas[i].act_str[0], strlen(&calmeas[i].act_str[0]));
            	//pr_mu(&strng[26],prtype);
            	pr_write(strng);
            }

            lininit(strng,TRUE,prtype);
            strncpy(&strng[1],&head[2][0],strlen(&head[2][0]));
            //nc = sprintf(buffer,"%5.2f",lin_def -> lin_cal_factors[i]);
            switch(linearity_type){
				case LINEATOR:
					nc = sprintf(buffer,"%5.2f",lin_def->factors_Lin[ch_num][i]);
					break;

				case CALICHECK:
					nc = sprintf(buffer,"%5.2f",lin_def->factors_Cali[ch_num][i]);
					break;
            }
			//strncpy(&strng[21],buffer,nc);
			strncpy(&strng[24],buffer,nc);
            pr_write(strng);
			feed(1,prtype);
		}
		formfeed(prtype);
	}

    extern char smu;
    void display_act_small(char *actstr, short col, short line)
    {

        char str[10];
        short xpos;
        short ypos;

        strncpy(str,actstr,6);
        str[6] = '\0';
        xpos = 6 * col;
        ypos = 8 * line;
        
        display_text(xpos,ypos,str,0,SMALL,NORMAL);
        
        
        xpos = 6 * (col + 6);
        if(actstr[6] == MU)
            write_special(&smu,xpos,ypos,SMALL,0);
        else
        {
            str[0] = actstr[6];
            str[1] = '\0';
            display_text(xpos,ypos,str,0,SMALL,NORMAL);
        }
        xpos = 6 * (col + 7);
        display_text(xpos,ypos,&actstr[7],0,SMALL,NORMAL);


    }
