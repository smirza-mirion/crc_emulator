/*********************************************************************
  MODULE:	Linearity Test for CRC-25

  FILE:		LinearityTest_25.c

  DATE:	    02/07/07

  CALLED BY:
		enhanced tests
  *************************************************************************/
#include "crc.h"
#include "i2c.h"
#include "screen.h"
#include "printer.h"
#include "uart.h"
#include "chambfac.h"
#include "keyboard.h"
#include "message.h"
#include "nuc.h"
#include <string.h>

extern CURRENT current;

//static void calicheck_test(LINDEF *lindef);
//static void lineator_test(LINDEF *lindef);
//static void standard_linearity_test(LINDEF *lindef);

	void linearity_test(void)
	{
/*		LINDEF def;

		// read in linearity definition 
        EE_READ(lindef,(uchar *)&def);


		// make sure test is defined 
		if(def.type == -1)
		{
			erase_screen();
			beep();
            //display_text(0,12,"Must Define",0,MEDIUM,NORMAL);
            //display_text(0,24,"Linearity Test",0,MEDIUM,NORMAL);
            display_medium_message(LIN_19,12,0,NORMAL);
            display_medium_message(LIN_20,24,0,NORMAL);
			contmsg();
			erase_screen();
            return;
		}

		// branch of type of test 
		switch(def.type)
		{
		case STANDARD:
			standard_linearity_test(&def);
			break;
		case LINEATOR:
			lineator_test(&def);
			break;
		case CALICHECK:
			calicheck_test(&def);
			break;
		} */

	}

extern const char *lin_tube_name[];

typedef struct linmeas LINMEAS;
struct linmeas
{
    float meas_act;
    float factor;
    float pc_ratio;
    char act_str[10];
    time_t meas_time;
};


static  void pr_lin(LINMEAS *linmeas, LINDEF *lindef,char prtype);
static void lineator_test(LINDEF *lindef);
static  void display_lin(LINMEAS *linmeas, LINDEF *lindef);

/*    static void lineator_test(LINDEF *lindef)
    {
        short i;
        bool ynret;
        short nmeas;
        short num_meas;
        bool meas_ok;
        char str[8];
        float factors[8];
        LINMEAS linmeas[8];
        time_t time0;
        time_t time1;
        float corr_act;
        char prtype;
        char nucname[8];
        short nuc_index;
        //float act;
        float act1;
        //char kun;
        short ch_num = current.main_chamber;

        // Removed: System is read at the beginning
        //EE_READ(syst, &current.system);
        // read in calibration factors 
        for(i = 0; i < 8; i++)
            EE_READ(lindef.lin_cal_factors[i],(uchar *)&factors[i]);

        num_meas = lindef -> num;

        nuc_index = lindef -> nuc_index;
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
                display_text(0,16,(char *)&lin_tube_name[nmeas][0],0,SMALL,NORMAL);
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
                if(nmeas == 0)
                {
                    read_clock(&time0);
                    corr_act = act1;
                }
                else
                {
                    // decay back to time 0 
                    read_clock(&time1);
                    corr_act = nucdecay(act1, time1, time0, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
                }

                linmeas[nmeas].meas_act = corr_act;
                linmeas[nmeas].factor = linmeas[0].meas_act / linmeas[nmeas].meas_act ;
                linmeas[nmeas].pc_ratio = 100. * linmeas[nmeas].factor / factors[nmeas];

                // Locked: CI
                //format_activity(linmeas[nmeas].meas_act, current.system, &linmeas[nmeas].act_str[0]);
                format_activity(linmeas[nmeas].meas_act, CI, &linmeas[nmeas].act_str[0]);

                erase_screen();
                //display_text(0,0,"Tube # ",0,SMALL,NORMAL);
                display_small_message(LIN_15,0,0,NORMAL);
                sprintf(str,"%d",nmeas + 1);
                display_text(42,0,str,0,SMALL,NORMAL);
                display_text(0,8,(char *)&lin_tube_name[nmeas][0],0,SMALL,NORMAL);
                //display_text(0,16,"Present Factor:",0,SMALL,NORMAL);
                display_small_message(LIN_21,16,0,NORMAL);
                sprintf(str,"%6.2f",linmeas[nmeas].factor);
                display_text(54,24,str,0,SMALL,NORMAL);
                //display_text(0,32,"% Ratio: ",0,SMALL,NORMAL);
                display_small_message(LIN_22,32,0,NORMAL);
                sprintf(str,"%6.2f",linmeas[nmeas].pc_ratio);
                display_text(54,32,str,0,SMALL,NORMAL);
                //display_text(0,48,"OK? Y OR N",0,SMALL,NORMAL);
                display_small_message(SM_OK_MSG,48,0,NORMAL);

                meas_ok = yesorno();
                if(home_set())
                    return;


            } while(!meas_ok);



        } // end nmeas loop 


        // display summary 
        display_lin(linmeas,lindef);



        erase_screen();
        //EE_READ(print[0],(uchar *)&prtype);
        prtype = current.printer;
        do
        {
            if(!start_printer(prtype,1,FALSE,PAPER))
                break;

            pr_lin(linmeas,lindef,prtype);

            ynret = print_again();
            if(home_set())
                return;

        }while(ynret);
    }*/

void Amulet_printLinearityLin(LINMEAS *linmeas, LINDEF *lindef){
	char prtype;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		pr_lin(linmeas, lindef, prtype);
	}
}

static  void pr_lin(LINMEAS *linmeas, LINDEF *lindef, char prtype)
    {
        short i;
        char head[4][30];
        short nc;
        short num_meas;
        char strng[90], amstring1[90], amstring2[90];
        char buffer[10];
        char tubename[14];
        char nucname[8];
        short ch_num = current.main_chamber;
        char buffer2[90];

        //num_meas = lindef -> num;
        num_meas = lindef->num_Lin[ch_num];

        prhead_languagesec(prtype);

        feed(1,prtype);
        if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
            bold(TRUE);

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[2],"LINEATOR LINEARITY TEST",23);
        //get_message(LIN_50,&strng[2]);
        get_amulet_strin(L_LINEATOR_LINEARITY_TEST, &strng[2]);    // "LINEATOR LINEARITY TEST"
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
            bold(FALSE);

        lininit(strng,TRUE,prtype);
        //Nuclide:
        //get_message(DIAG_51,&strng[2]);
        //strng[9] = ':';

        //NuclideData_getName(lindef -> nuc_index,nucname);
        NuclideData_getName(lindef->nuc_index_Lin[ch_num], nucname);
        //strncpy(&strng[11],nucname,6);
        get_amulet_message(L_NUCLIDE_COLON2, amstring1);    // "NUCLIDE:"
        sprintf(amstring2, "%s %s", amstring1, nucname);
        strin(&strng[2], amstring2);
        pr_write(strng);

        lininit(strng, TRUE, prtype);
        get_amulet_message(L_CHAMBER_SN, amstring1);    // "Chamber S/N: %s"
        sprintf(buffer2, amstring1, &(lindef->chamb_num_Lin[ch_num][0]));
        strncpy(&strng[2], buffer2, strlen(buffer2));
        pr_write(strng);

        lininit(strng, TRUE, prtype);
        get_amulet_message(L_LINEATOR_SN, amstring1);    // "Lineator S/N: %s"
        sprintf(buffer2, amstring1, &(lindef->serial_num_Lin[ch_num][0]));
        strncpy(&strng[2], buffer2, strlen(buffer2));
        pr_write(strng);

        feed(1,prtype);

        //strncpy(&head[0][0],"Tube:           ",16);
        //strncpy(&head[0][0],"                ",16);
        //get_message(LIN_46,&head[0][0]);
        get_amulet_message(L_TUBE2, &head[0][0]);    // "Tube:"

        //strncpy(&head[1][0],"Activity:       ",16);
        //strncpy(&head[1][0],"                ",16);
        //get_message(LIN_47,&head[1][0]);
        get_amulet_message(L_ACTIVITY_COLON, &head[1][0]);    // "Activity:"

        //strncpy(&head[2][0],"Present Factor: ",16);
        //strncpy(&head[2][0],"                ",16);
        //get_message(LIN_51,&head[2][0]);
        get_amulet_message(L_PRESENT_FACTOR, &head[2][0]);    // "Present Factor:"

        //strncpy(&head[3][0],"Ratio:          ",16);
        //strncpy(&head[3][0],"                ",16);
        //get_message(LIN_52,&head[3][0]);
        get_amulet_message(L_RATIO2, &head[3][0]);    // "Ratio:"

        for(i = 0; i < num_meas; i++)
        {
            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[0][0],6);
            strcpy(tubename,&lin_tube_name[i][0]);
            //strncpy(&strng[7],tubename,strlen(tubename));
            sprintf(amstring1, "%s %s", &head[0][0], tubename);
            strin(&strng[1], amstring1);
            pr_write(strng);

            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[1][0],10);
            strin(&strng[1],&head[1][0]);
            strncpy(&strng[21],&linmeas[i].act_str[0],9);
            pr_mu(&strng[27],prtype);
            pr_write(strng);

            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[2][0],16);
            strin(&strng[1],&head[2][0]);
            nc = sprintf(buffer,"%6.2f",linmeas[i].factor);
            strncpy(&strng[22],buffer,nc);
            pr_write(strng);

            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[3][0],8);
            strin(&strng[1], &head[3][0]);
            nc = sprintf(buffer,"%6.2f",linmeas[i].pc_ratio);
            strncpy(&strng[22],buffer,nc);
            strng[22 + nc] = '%';
            pr_write(strng);

            feed(1,prtype);
        }
        formfeed(prtype);
    }

/*static  void display_lin(LINMEAS *linmeas, LINDEF *lindef)
    {

        short i,j,k;
        char buffer[10];
        short line;

        for(j = 0; j < 2; j++)
        {
            erase_screen();
            // display_text(24,0,"LINEATOR TEST",0,SMALL,NORMAL);
            //display_text(0,8,"#    FACTOR   %RATIO",0,SMALL,NORMAL);
            display_small_message(LIN_23,0,0,NORMAL);
            display_small_message(LIN_24,8,0,NORMAL);

            for(k = 0; k < 4; k++)
            {
                i = 4 * j + k;
                line = k + 2;
                buffer[0] = i + '1';
                buffer[1] = '\0';
                display_text(0, 8 * line,buffer,0,SMALL,NORMAL);

                sprintf(buffer,"%6.2f",linmeas[i].factor);
                display_text(30,8 * line,buffer,0,SMALL,NORMAL);

                sprintf(buffer,"%6.2f",linmeas[i].pc_ratio);
                display_text(84,8 * line,buffer,0,SMALL,NORMAL);
            }
            contmsg();

        }
    }*/


typedef struct calmeas CALMEAS;
struct calmeas
{
    float meas_act;
    float result;
    float var;
    char res_str[10];
    time_t meas_time;
};

//extern const char *cal_tube_name[];
static  void pr_cal(CALMEAS *calmeas, LINDEF *lindef, char *mean_str,char prtype);
static  void display_calicheck(CALMEAS *calmeas, LINDEF *lindef, char *mean_str);

/*    static void calicheck_test(LINDEF *lindef)
    {
        short i;
        bool ynret;
        short nmeas;
        short num_meas;
        bool meas_ok;
        char str[8];
        float factors[12];
        float mean;
        CALMEAS calmeas[12];
        time_t time0;
        time_t time1;
        float corr_act;
        char mean_str[10];
        float act_disp;
        short kun;
        char prtype;
        char nucname[8];
        short nuc_index;
        //float act;
        float act1;
        short ndec;
        short ch_num = current.main_chamber;

        // Removed: System is read at the beginning
        //EE_READ(syst, &current.system);

        // read in calibration factors
        for(i = 0; i < 12; i++)
            EE_READ(lindef.lin_cal_factors[i],(uchar *)&factors[i]);

        num_meas = lindef -> num;

        nuc_index = lindef -> nuc_index;
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
                if(nmeas == 0)
                {
                    read_clock(&time0);
                    corr_act = act1;
                }
                else
                {
                    // decay back to time 0
                    read_clock(&time1);
                    corr_act = nucdecay(act1, time1, time0, NuclideData_getHalflife(nuc_index), NuclideData_getHalflifeUnit(nuc_index));
                }

                calmeas[nmeas].meas_act = corr_act;
                calmeas[nmeas].result = calmeas[nmeas].meas_act * factors[nmeas];

                // Locked: CI
                //ndec = getdec(calmeas[nmeas].result,current.system,&kun);
                ndec = getdec(calmeas[nmeas].result, CI, &kun);
                act_disp = calmeas[nmeas].result * unitfact[kun - 1];
                act2str(act_disp,calmeas[nmeas].res_str,ndec,FALSE,kun);

                erase_screen();
                //display_text(0,0,"Tube # ",0,SMALL,NORMAL);
                display_small_message(LIN_15,0,0,NORMAL);
                sprintf(str,"%d",nmeas + 1);
                display_text(42,0,str,0,SMALL,NORMAL);
                //display_text(0,8,(char *)&cal_tube_name[nmeas][0],0,SMALL,NORMAL);
                display_small_message(LIN_57 + nmeas,8,0,NORMAL);
                //display_text(0,24,"Result",0,SMALL,NORMAL);
                display_small_message(LIN_25,24,0,NORMAL);
                display_act_small((char *)&calmeas[nmeas].res_str[0],7,3);
                //display_text(0,40,"OK? Y OR N",0,SMALL,NORMAL);
                display_small_message(SM_OK_MSG,40,0,NORMAL);

                meas_ok = yesorno();
                if(home_set())
                    return;


            } while(!meas_ok);



        } // end nmeas loop 


        // calculate mean & variation 
        mean = 0.;
        for(i = 0; i < num_meas; i++)
            mean += calmeas[i].result;
        mean /= (float)num_meas;
        // Locked: CI
        //ndec = getdec(mean,current.system,&kun);
        ndec = getdec(mean, CI, &kun);
        act_disp = mean * unitfact[kun - 1];
        act2str(act_disp,mean_str,ndec,FALSE,kun);

        for(i = 0; i < num_meas; i++)
            calmeas[i].var = 100. * (calmeas[i].result - mean) / mean;


        // display summary 
        display_calicheck(calmeas,lindef,mean_str);


        erase_screen();
        //EE_READ(print[0],(uchar *)&prtype);
        prtype = current.printer;
        do
        {
            if (!start_printer(prtype,1,FALSE,PAPER))
                break;

            pr_cal(calmeas,lindef,mean_str,prtype);

            ynret = print_again();
            if(home_set())
                return;
        }while(ynret);

    }*/

void Amulet_printLinearityCali(CALMEAS *calmeas, LINDEF *lindef){
	char prtype;
	short num_meas, i;
	//short  kun;
	//short ndec;
	float mean;
	//float act_disp;
	char mean_str[10];
	short ch_num = current.main_chamber;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		//num_meas = lindef->num;
		num_meas = lindef->num_Cali[ch_num];
		mean = 0.;
        for(i = 0; i < num_meas; i++) mean += calmeas[i].result;
        mean /= (float)num_meas;
        // Locked: CI
        //ndec = getdec(mean, current.system, &kun);
        //ndec = getdec(mean, CI, &kun);
        //act_disp = mean * unitfact[kun - 1];
        //act2str(act_disp, mean_str, ndec, FALSE, kun);
        format_activity_system(mean, mean_str);

		pr_cal(calmeas, lindef, mean_str, prtype);
		}
}

static  void pr_cal(CALMEAS *calmeas, LINDEF *lindef, char *mean_str,char prtype)
    {
        short i;
        char head[3][30];
        short nc;
        short num_meas;
        char strng[90], amstring1[90], amstring2[90], amstring3[90];
        char buffer[10];
        char tubename[22];
        char nucname[8];
        char buffer2[90];
        short ch_num = current.main_chamber;

        //num_meas = lindef -> num;
        num_meas = lindef->num_Cali[ch_num];

        prhead_languagesec(prtype);

        feed(1,prtype);
        if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
            bold(TRUE);

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[2],"CALICHECK LINEARITY TEST",24);
        //get_message(LIN_53,&strng[2]);
        get_amulet_strin(L_CALICHECK_LINEARITY_TEST, &strng[2]);    // "CALICHECK LINEARITY TEST"
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
            bold(FALSE);

        lininit(strng,TRUE,prtype);
        //Nuclide:
        //get_message(DIAG_51,&strng[2]);
        //strng[9] = ':';
        //NuclideData_getName(lindef -> nuc_index,nucname);
        NuclideData_getName(lindef->nuc_index_Cali[ch_num], nucname);
        //strncpy(&strng[11],nucname,6);
        get_amulet_message(L_NUCLIDE_COLON2, amstring1);    // "NUCLIDE:"
        sprintf(amstring2, "%s %s", amstring1, nucname);
        strin(&strng[2], amstring2);
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        get_amulet_message(L_CHAMBER_SN, amstring1);    // "Chamber S/N: %s"
        sprintf(buffer2, amstring1, &(lindef->chamb_num_Cali[ch_num][0]));
        strncpy(&strng[2], buffer2, strlen(buffer2));
        pr_write(strng);

        lininit(strng,TRUE,prtype);
        get_amulet_message(L_CALICHECK_SN, amstring1);    // "Calicheck S/N: %s"
        sprintf(buffer2, amstring1, &(lindef->serial_num_Cali[ch_num][0]));
        strncpy(&strng[2], buffer2, strlen(buffer2));
        pr_write(strng);

        feed(1,prtype);

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[1],"Mean Result: ",13);
        //get_message(LIN_54,&strng[1]);
        //strncpy(&strng[20],&mean_str[0],9);
        //pr_mu(&strng[26],prtype);
        get_amulet_message(L_MEAN_RESULT, amstring1);    // "Mean Result: %s"
        strcpy(amstring2, &mean_str[0]);
        pr_mu(&amstring2[6], prtype);
        sprintf(amstring3, amstring1, amstring2);
        strin(&strng[1], amstring3);
        pr_write(strng);
        feed(1,prtype);

        //strncpy(&head[0][0],"Tube:                ",21);
        //strncpy(&head[0][0],"                     ",21);
        //get_message(LIN_46,&head[0][0]);
        get_amulet_message(L_TUBE2, &head[0][0]);    // "Tube:"

        //strncpy(&head[1][0],"Result:              ",21);
        //strncpy(&head[1][0],"                     ",21);
        //get_message(LIN_55,&head[1][0]);
        get_amulet_message(L_RESULT2, &head[1][0]);    // "Result:"

        //strncpy(&head[2][0],"Variation from Mean: ",21);
        //strncpy(&head[2][0],"                     ",21);
        //get_message(LIN_56,&head[2][0]);
        get_amulet_message(L_VARIATION_FROM_MEAN, &head[2][0]);    // "Variation from Mean:"


        for(i = 0; i < num_meas; i++)
        {
            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[0][0],6);
            //strcpy(tubename,&cal_tube_name[i][0]);
            //strcpy(tubename,"                    ");
            //get_message(LIN_69 + i,tubename);
            //strncpy(&strng[7],tubename,strlen(tubename));
            get_amulet_message(L_TUBE2, amstring1);    // "Tube:"
            strcat(amstring1, " ");
            switch(i){
            	case 0:
            		get_amulet_message(L_CAL__1, amstring2);    // "Black"
            		break;

            	case 1:
            		get_amulet_message(L_CAL__2, amstring2);    // "Black + Red"
            		break;

            	case 2:
            		get_amulet_message(L_CAL__3, amstring2);    // "Black + Orange"
            		break;

            	case 3:
            		get_amulet_message(L_CAL__4, amstring2);    // "Black + Yellow"
            		break;

            	case 4:
            		get_amulet_message(L_CAL__5, amstring2);    // "Black + Green"
            		break;

            	case 5:
            		get_amulet_message(L_CAL__6, amstring2);    // "Black + Blue"
            		break;

            	case 6:
            		get_amulet_message(L_CAL__7, amstring2);    // "Black + Purple"
            		break;

            	case 7:
            		get_amulet_message(L_CAL__8, amstring2);    // "Black/Purple/Red"
            		break;

            	case 8:
            		get_amulet_message(L_CAL__9, amstring2);    // "Black/Purple/Orange"
            		break;

            	case 9:
            		get_amulet_message(L_CAL__10, amstring2);    // "Black/Purple/Yellow"
            		break;

            	case 10:
            		get_amulet_message(L_CAL__11, amstring2);    // "Black/Purple/Green"
            		break;

            	case 11:
            		get_amulet_message(L_CAL__12, amstring2);    // "Black/Purple/Blue"
            		break;
            }
            strcat(amstring1, amstring2);
            strin(&strng[1], amstring1);
            pr_write(strng);

            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[1][0],10);
            strin(&strng[1], &head[1][0]);
            strncpy(&strng[21],&calmeas[i].res_str[0],9);
            pr_mu(&strng[27],prtype);
            pr_write(strng);

            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[2][0],20);
            strin(&strng[1], &head[2][0]);
            nc = sprintf(buffer,"%6.2f",calmeas[i].var);
            strncpy(&strng[22],buffer,nc);
            strng[22 + nc] = '%';
            pr_write(strng);

            feed(1,prtype);
        }

        formfeed(prtype);
    }

/*static  void display_calicheck(CALMEAS *calmeas, LINDEF *lindef, char *mean_str)
    {
        short i,j,k;
        short nc;
        char buffer[10];
        short line;
        short num_meas;
        short nscreens;

        num_meas = lindef -> num;
        nscreens = num_meas / 4;
        if((num_meas % 4) != 0)
            ++nscreens;

        for(j = 0; j < nscreens; j++)
        {
            erase_screen();
            //display_text(24,0,"CALICHECK TEST",0,SMALL,NORMAL);
            //display_text(6,8,"MEAN: ",0,SMALL,NORMAL);
            display_small_message(LIN_26,0,0,NORMAL);
            display_small_message(LIN_27,8,0,NORMAL);
            display_act_small(mean_str,7,1);

            //display_text(0,16,"#    RESULT   % VAR ",0,SMALL,NORMAL);
            display_small_message(LIN_28,16,0,NORMAL);

            for(k = 0; k < 4; k++)
            {
                i = 4 * j + k;
                if(i < num_meas)
                {
                    line = k + 3;
                    sprintf(buffer,"%d",i + 1);
                    display_text(0, 8 * line, buffer,0,SMALL,NORMAL);

                    display_act_small(&calmeas[i].res_str[0],3,line);
                    nc = sprintf(buffer,"%5.2f",calmeas[i].var);
                    display_text(6 * (20 - nc), 8 * line,buffer,0,SMALL,NORMAL);
                }


            }
            contmsg();

	    }
    }*/

static  bool display_standard(float *predicted_act, short num_test,
                              bool start_flag);
// Removed: Units Conversion is not necessary
//float convact(float act0, short system);
static  void pr_standard(float *predicted_act, short num_test,char prtype, char *nucname);
void predicted_standard_activity(float *predicted_act, short num_test, float *slope, NUCDATA *nucdata);


    extern STANDLIN standlin[][12];

/*    static void standard_linearity_test(LINDEF *lindef)
    {
        bool ynret;
        short iret;
        short num_meas;
        short meas_ok;
        char str[8];
        float act_disp;
        short num_test;
        float predicted_act[12];
        bool new_flag;
        float slope;
        short elapsed_hours;
        short elapsed_mins;
        char pr_actstr[10];
        time_t nowtime;
        long diffmin;
        char prtype;
        char nucname[8];
        short nuc_index;
        NUCDATA nucdata;
        short ch_num = current.main_chamber;
        //float act;
        float act1;
        short kun;
        short i;
        short ndec;

        // Removed: System is read at the beginning
        //EE_READ(syst, &current.system);

        num_meas = lindef -> num;

        nuc_index = lindef -> nuc_index;

        NuclideData_getNuclide(nuc_index,&nucdata);
        strcpy(nucname,&nucdata.name[0]);

        // get number of test
        num_test = 1;
        for(i = 0; i < 12; i++)
        {
            if(standlin[ch_num][i].meas_flag != 1)
                break;
            ++num_test;
        }

        if(num_test > num_meas)
            new_flag = TRUE;
        else
            new_flag = FALSE;

        if(!new_flag)
        {
            if(num_test > 1)
            {
                predicted_standard_activity(predicted_act,num_test - 1,&slope,&nucdata);
                iret = display_standard(predicted_act,num_test - 1, TRUE);
                if(home_set())
                    return;
                if(iret)  //want to do new test
                    new_flag = TRUE;
                erase_screen();

            }
        }

        if(new_flag)
        {
            if(num_test <= num_meas)
            {
//                display_text(0,12,"TEST NOT",0,MEDIUM,NORMAL);
// -                 display_text(0,22,"COMPLETE",0,MEDIUM,NORMAL);
// -                 display_text(0,32,"START NEW TEST?",0,MEDIUM,NORMAL);
                display_medium_message(LIN_29,12,0,NORMAL);
                display_medium_message(LIN_30,22,0,NORMAL);
                display_medium_message(LIN_31,32,0,NORMAL);
               // display_text(2,42,"Y OR N",0,MEDIUM,NORMAL);
                display_medium_message(YES_OR_NO,42,0,NORMAL);
                if(!yesorno() || home_set())
                {
                    erase_screen();
                    return;
                }
            }
            erase_screen();
            num_test = 1;
            // initialize standlin to no tests done
            for(i = 0; i < num_meas; i++)
                standlin[ch_num][i].meas_flag = 0;

            if(num_meas < 12)
            {
                for(i = num_meas; i < 12; i++)
                    standlin[ch_num][i].meas_flag = -1;
            }

        }



        do
        {
            erase_screen();
            //display_text(0,8,"Test # ",0,SMALL,NORMAL);
            display_small_message(LIN_32,8,0,NORMAL);
            sprintf(str,"%d",num_test);
            display_text(42,8,str,0,SMALL,NORMAL);
            if(num_test == 1)
            {
                //display_text(0,16,"First Measurement",0,SMALL,NORMAL);
                display_small_message(LIN_33,16,0,NORMAL);
            }
            else
            {
                //display_text(0,16,"Measure At: ",0,SMALL,NORMAL);
                display_small_message(LIN_34,16,0,NORMAL);
                sprintf(str,"%d",lindef -> hours[num_test - 1]);
                display_text(72,16,str,0,SMALL,NORMAL);
                //display_text(6 * (13 + nc),16,"Hours",0,SMALL,NORMAL);
                display_small_message(LIN_35,16,0,NORMAL);

                //get elapsed hours and minutes
                read_clock(&nowtime);
                diffmin = (long)difftime(nowtime,standlin[ch_num][0].date) / 60;
                elapsed_hours = diffmin / 60;
                elapsed_mins = diffmin % 60;


                //display_text(0,24,"Elapsed: ",0,SMALL,NORMAL);
                display_small_message(LIN_36,24,0,NORMAL);
                sprintf(str,"%d",elapsed_hours);
                display_text(54,24,str,0,SMALL,NORMAL);
                //display_text(72,24,"Hr",0,SMALL,NORMAL);
                display_small_message(LIN_37,24,0,NORMAL);
                sprintf(str,"%d",elapsed_mins);
                display_text(90,24,str,0,SMALL,NORMAL);
                //display_text(108,24,"Min",0,SMALL,NORMAL);
                display_small_message(LIN_38,24,0,NORMAL);
            }

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

            // save measurement in standlin
            standlin[ch_num][num_test - 1].meas = act1;
            // Locked: CI
            //standlin[ch_num][num_test - 1].syst = current.system;
            // Removed:
            //standlin[ch_num][num_test - 1].syst = CI;
            strcpy(&standlin[ch_num][num_test - 1].actstr[0],
                    &measurement[ch_num].actstr[0]);

            read_clock(&nowtime);
            standlin[ch_num][num_test - 1].date = nowtime;

            // get predicted activity
            predicted_standard_activity(predicted_act,num_test,&slope,&nucdata);
            // Locked: CI
            //ndec = getdec(predicted_act[num_test - 1],current.system,&kun);
            ndec = getdec(predicted_act[num_test - 1], CI, &kun);
            act_disp = predicted_act[num_test - 1] * unitfact[kun - 1];
            act2str(act_disp,pr_actstr,ndec,FALSE,kun);

            erase_screen();
            //display_text(0,16,"Measured",0,SMALL,NORMAL);
            display_small_message(LIN_39,16,0,NORMAL);
            display_act_small(&standlin[ch_num][num_test - 1].actstr[0],9,2);

            if(num_test != 1)
            {
                //display_text(0,24,"Predicted",0,SMALL,NORMAL);
                display_small_message(LIN_40,24,0,NORMAL);
                display_act_small(pr_actstr,9,3);
            }

            //display_text(0,40,"OK? Y OR N",0,SMALL,NORMAL);
            display_small_message(SM_OK_MSG,40,0,NORMAL);
            meas_ok = yesorno();
            if(home_set())
                return;


        } while(!meas_ok);

        // accepted, mark as done
        standlin[ch_num][num_test - 1].meas_flag = 1;


        // display summary 
        if(num_test != 1)
            display_standard(predicted_act, num_test, FALSE);


        erase_screen();
        //EE_READ(print[0],(uchar *)&prtype);
        prtype = current.printer;
        do
        {
            if (!start_printer(prtype,1,FALSE,PAPER))
                break;

            pr_standard(predicted_act, num_test,prtype,nucname);

            ynret = print_again();
            if(home_set())
                return;
        }while(ynret);
    }*/

void Amulet_printLinearityStd(float *predicted_act, short num_test, char *nucname){
	char prtype;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		pr_standard(predicted_act, num_test, prtype, nucname);
	}
}

    static  void pr_standard(float *predicted_act, short num_test,char prtype, char *nucname)
    {

        short i;
        char head[4][16];
        short nc;
        char buffer[10];
        char pr_actstr[10];
        //float act_disp;
        float meas0;
        float var;
        char strng[90];
        char amstring1[90], amstring2[90];
        //short kun;
        //short ndec;
        char dtstr[20];
        short ch_num = current.main_chamber;

        prhead_languagesec(prtype);

        feed(1,prtype);
        if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
            bold(TRUE);

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[2],"STANDARD LINEARITY TEST",23);
        //get_message(LIN_81,&strng[2]);
        get_amulet_strin(L_STANDARD_LINEARITY_TEST, &strng[2]);    // "STANDARD LINEARITY TEST"
        if(prtype == USB_EPS_PRINTER)
            eps_bold(0.5,strng);
        else
            pr_write(strng);
        if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
            bold(FALSE);

        lininit(strng,TRUE,prtype);
        //Nuclide:
        //get_message(DIAG_51,&strng[2]);
        //strng[9] = ':';
        //get_amulet_strin(L_NUCLIDE_COLON2, &strng[2]);    // "NUCLIDE:"
        //strncpy(&strng[11],nucname,6);
        get_amulet_message(L_NUCLIDE_COLON2, amstring1);    // "NUCLIDE:"
        sprintf(amstring2, "%s %s", amstring1, nucname);
        strin(&strng[2], amstring2);
        pr_write(strng);

        feed(1,prtype);

        //strncpy(&head[0][0],"Test:           ",16);
        //strncpy(&head[0][0],"                ",16);
        //get_message(LIN_82,&head[0][0]);
        get_amulet_message(L_TEST2, &head[0][0]);    // "Test:"

        //strncpy(&head[1][0],"Predicted:      ",16);
        //strncpy(&head[1][0],"                ",16);
        //get_message(LIN_83,&head[1][0]);
        get_amulet_message(L_PREDICTED2, &head[1][0]);    // "Predicted:"

        //strncpy(&head[2][0],"Actual:         ",16);
        //strncpy(&head[2][0],"                ",16);
        //get_message(LIN_84,&head[2][0]);
        get_amulet_message(L_ACTUAL, &head[2][0]);    // "Actual:"

        //strncpy(&head[3][0],"Variation:      ",16);
        //strncpy(&head[3][0],"                ",16);
        //get_message(LIN_85,&head[3][0]);
        get_amulet_message(L_VARIATION, &head[3][0]);    // "Variation:"

        for(i = 0; i < num_test; i++)
        {

            lininit(strng,TRUE,prtype);
            strncpy(&strng[1],&head[0][0],strlen(&head[0][0]));
            nc = sprintf(buffer,"%d",i + 1);
            strncpy(&strng[strlen(&head[0][0]) + 2],buffer,nc);
            dateout_language(dtstr,&standlin[ch_num][i].date,4);
            strncpy(&strng[13],dtstr,strlen(dtstr));
            timeoutsec(dtstr,&standlin[ch_num][i].date);
            strncpy(&strng[25],dtstr,strlen(dtstr));
            pr_write(strng);

            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[1][0],11);
            strin(&strng[1], &head[1][0]);
            // Locked: CI
            //ndec = getdec(predicted_act[i],current.system, &kun);
            //ndec = getdec(predicted_act[i], CI, &kun);
            //act_disp = predicted_act[i] * unitfact[kun - 1];
            //act2str(act_disp,pr_actstr,ndec,FALSE,kun);
            format_activity_system(predicted_act[i], pr_actstr);
            strncpy(&strng[21],&pr_actstr[0],9);
            pr_mu(&strng[27],prtype);
            pr_write(strng);

            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[2][0],16);
            strin(&strng[1],&head[2][0]);
            strncpy(&strng[21],&standlin[ch_num][i].actstr[0],9);
            pr_mu(&strng[27],prtype);
            pr_write(strng);

            // Removed: Units Conversion is not needed
            //meas0 = convact(standlin[ch_num][i].meas,standlin[ch_num][i].syst);
            meas0 = standlin[ch_num][i].meas;
            var = 100. * (meas0 - predicted_act[i]) / predicted_act[i];
            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],&head[3][0],11);
            strin(&strng[1],&head[3][0]);
            nc = sprintf(buffer,"%6.2f",var);
            strncpy(&strng[28 - nc],buffer,nc);
            strng[28] = '%';
            pr_write(strng);

            feed(1,prtype);
        }

        formfeed(prtype);

    }


/*static  bool display_standard(float *predicted_act, short num_test,
                              bool start_flag)
    {

        short i,j,k;
        short nc;
        char buffer[10];
        short line;
        short nscreens;
        float meas0;
        float var;
        short ch_num = current.main_chamber;


        nscreens = num_test / 4;
        if((num_test % 4) != 0)
            ++nscreens;


        for(j = 0; j < nscreens; j++)
        {
            erase_screen();
            //display_text(6,0,"STANDARD LINEARITY",0,SMALL,NORMAL);
            //display_text(0,8,"#   MEASURED    %VAR",0,SMALL,NORMAL);
            display_small_message(LIN_41,0,0,NORMAL);
            display_small_message(LIN_42,8,0,NORMAL);

            for(k = 0; k < 4; k++)
            {
                i = 4 * j + k;
                if(i < num_test)
                {
                    line = k + 3;
                    sprintf(buffer,"%d",i + 1);
                    display_text(0, 8 * line,buffer,0,SMALL,NORMAL);
                    display_act_small(&standlin[ch_num][i].actstr[0],3,line);
                    //Removed: Units Conversion is not needed
                    //meas0 = convact(standlin[ch_num][i].meas,standlin[ch_num][i].syst);
                    meas0 = standlin[ch_num][i].meas;
                    var = 100. * (meas0 - predicted_act[i]) / predicted_act[i];
                    nc = sprintf(buffer,"%6.2f",var);
                    display_text(6 * (20 - nc),8 * line,buffer,0,SMALL,NORMAL);
                }

            }
            if((j == nscreens - 1) && start_flag)
            {
                //display_text(6,56,"New Test? Y or N",0,SMALL,NORMAL);
                display_small_message(LIN_43,56,0,NORMAL);
                return(yesorno());
            }
            else
                contmsg();

        }
        return(TRUE);
    }*/



void predicted_standard_activity(float *predicted_act, short num_test,
                                        float *slope, NUCDATA *nucdata)
    {
        short i;
        float meas0;
        /* for least squares fit */
        float x[12];
        float xi;
        float sx;
        float sy;
        short ch_num = current.main_chamber;

        if(num_test == 1)
        {
        	//Removed: Units Conversion is not needed
            //meas0 = convact(standlin[ch_num][0].meas,standlin[ch_num][0].syst);
        	meas0 = standlin[ch_num][0].meas;
            predicted_act[0] = meas0;
            return;
        }

        if(num_test > 1 && num_test < 4)
        {
        	//Removed: Units Conversion is not needed
            //meas0 = convact(standlin[ch_num][0].meas,standlin[ch_num][0].syst);
        	meas0 = standlin[ch_num][0].meas;

            /* calculate predicted activity from act0 */
            for(i = 0; i < num_test; i++)
            {
                predicted_act[i] =
                    nucdecay(meas0,standlin[ch_num][0].date,
                             standlin[ch_num][i].date,
                             nucdata -> halflife,nucdata -> hlunit);
            }
        }

        if(num_test >= 4)
        {
            /* do least squares fit */
            sx = 0.;
            sy = 0.;
            for(i = 0; i < num_test; i++)
            {
                xi = nucdecay(1.,standlin[ch_num][0].date,
                             standlin[ch_num][i].date,
                             nucdata -> halflife,nucdata -> hlunit);
                x[i] = xi;
                sx += xi;
                //Removed: Units Conversion is not needed
                //meas0 = convact(standlin[ch_num][i].meas,standlin[ch_num][i].syst);
                meas0 = standlin[ch_num][i].meas;
                sy += meas0;
            }
            *slope = sy / sx;
            for(i = 0; i < num_test; i++)
                predicted_act[i] = *slope * x[i];
        }

    }

// Removed: Conversion function to flip units is not needed
//float convact(float act0, short system)
//    {
//       if (system == current.system)
//            return(act0);
//
//        if (current.system == CI)
//            return(act0 / BQFACTOR);
//
//        else
//            return(act0 * BQFACTOR);
//    }
