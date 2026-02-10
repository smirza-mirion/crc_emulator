/*********************************************************************
  MODULE:		PRINT CHAMBER MEASUREMENT

  FILE:		PrintMeasurement.c

  DATE:		01/24/07
            03/27/08 -- fixed OKI ticket printing of date

  ANALYSIS:  print ticket for Epson Slip printer or Epson Roll printer
			 or OKI printer or USB printer
             Or Print 1 line on Oki Printer

  PASSED PARAMETERS:  mode -- 1st or 2nd printing

  CALLED BY:
		activity


  *************************************************************************/
#include "crc.h"
#include "printer.h"
#include "uart.h"
#include "i2c.h"
#include "keyboard.h"
#include "chambfac.h"
#include "message.h"
#include <string.h>
    extern CURRENT current;
	extern CHAMBER chamber[];
    extern MEASUREMENT measurement[];
    extern time_t clock_time;
    
    extern char const *cal_type_str[];
static void print_activity(time_t nowtime);
static	void print_future(time_t nowtime);
static	void print_nuclide_name(void);
static void print_staffid(void);
void reverse_feed(short num, char printer);

void start_label(void);
void label_peel(void);
void trim(char *acByte);
static char printer;
    
    bool prticket(short mode)
	{

        time_t nowtime;
        
        
        nowtime = clock_time;
        
        printer = current.printer;

		if(!start_printer(printer,mode,1,TICKET))
			return FALSE;

		if (printer == USB_EPS_LABEL_PRINTER)
			start_label();

		// nuclide 
		print_nuclide_name();

        //staff identification
        print_staffid();

        //feed(1,printer);

		// activity 
		print_activity(nowtime);

		// future activity 
		print_future(nowtime);

		feed(1,printer);

		if (mode == 1)
		{
            switch(printer)
            {        
            case OKI_PRINTER:
				fontnlq(1);
                feed(3,printer);
                break;
            case SLIP_PRINTER:
            case LX_PRINTER:    
                feed(3,printer);
                break;
			case USB_EPS_LABEL_PRINTER:
				if(current.feed_label) label_peel();
				return FALSE;
            default:    
                formfeed(printer);
                return FALSE;
            }    
			return TRUE;
		}
		else
		{
            formfeed(printer);
			if(printer == SLIP_PRINTER)
                uart_write("\n",U_PR);
			return TRUE;
		}

	}

    extern const char *unit_str[];
    static void print_activity(time_t nowtime)
	{
    	char timestr[20];
        char strng[90];
        short kp;
        short ch_num = current.main_chamber;
        char str[14];
        char acMsg[15];

        set_act_line(printer);
        
		if (measurement[ch_num].wasover)
        {    
            str[0] = 0;
            strng[0] = 0;
            //get_message(INV_42,&str[5]);
            get_amulet_message(L_OVER, acMsg);    // " OVER "
            trim(acMsg);
            strcpy(str, acMsg);
            pr_write(str);
        }    
		else
        {
            kp = 0;
            strncpy(strng,"    ",4);
            //if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER)
            if(printer == OKI_PRINTER || printer == LX_PRINTER)
                kp = 2;
            if(printer < 0)
                kp = 4;

            //strncpy(&strng[kp],&measurement[ch_num].actstr[0],6);
            strncpy(&strng[kp],&measurement[ch_num].actstr[0],strlen(measurement[ch_num].actstr) - 3);
            //strng[kp + 6] = '\0';
            strng[kp + strlen(measurement[ch_num].actstr) - 3] = '\0';
            if(printer == USB_EPS_PRINTER)
                eps_bold(0.25,strng);
            else
                pr_write(strng);
        }
        
        switch(printer)
        {        
		case ROLL_PRINTER:
		case USB_EPS_LABEL_PRINTER:	
		case SLIP_PRINTER:
			font35();
            break;

        case OKI_PRINTER:
            font85e(FALSE);
            fontnlq(TRUE);
            break;

        default:
            break;
        }    
               

		if (!measurement[ch_num].wasover)
		{
			strcpy(&strng[0],unit_str[measurement[ch_num].kun - 1]);
            pr_mu(&strng[0],printer);
		}
        strcat(strng," ");
        pr_write(strng);
        switch(printer)
        {        
        case OKI_PRINTER:
            //font85e(FALSE);
            fontnlq(TRUE);
            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
        	if(strlen(measurement[ch_num].actstr) > 9){
        		strcpy(strng, "\r\n");
        		pr_write(strng);
        	}
        	break;

		case USB_EPS_LABEL_PRINTER:	
        case USB_EPS_PRINTER:    
            break;

        case LX_PRINTER:
            bold(FALSE);
            eps_cpi(10);
            break;

        case USB_PRINTER:
            pcl_cpi("10",TRUE);
            pcl_bold(FALSE);
            break;
        }    
        
        if(((printer == ROLL_PRINTER) || (printer == SLIP_PRINTER)) && (strlen(measurement[ch_num].actstr) > 9)){
        	timeoutsec(timestr, &nowtime);
        	sprintf(strng, "           %s", timestr);
        }else timeoutsec(strng,&nowtime);
        strcat(strng," ");
        pr_write(strng);
        
		//date
        dateout_language(strng,&nowtime,2);
        if(((printer == ROLL_PRINTER) || (printer == SLIP_PRINTER)) && (strlen(measurement[ch_num].actstr) > 9)) strcat(strng,"\r\n\r\n");
        else strcat(strng,"\r\n");
        pr_write(strng);
        
	}



    static void print_future(time_t nowtime)
	{
		
        char strng[90];
        short kp;
        char dstr[20];
        short ch_num = current.main_chamber;

		if(measurement[ch_num].future.dosetime == NO_TIME)
            return;

        set_act_line(printer);
        
        kp = 0;
        strncpy(strng,"    ",4);
        //if(printer == OKI_PRINTER || printer < 0 || printer == LX_PRINTER)
        if(printer == OKI_PRINTER || printer == LX_PRINTER)
            kp = 2;
        if(printer < 0)
            kp = 4;
        
        //strncpy(&strng[kp],&measurement[ch_num].future.actstr[0],6);
        strncpy(&strng[kp],&measurement[ch_num].future.actstr[0],strlen(measurement[ch_num].future.actstr) - 3);
        //strng[kp + 6] = '\0';
        strng[kp + strlen(measurement[ch_num].future.actstr) - 3] = '\0';
        if(printer == USB_EPS_PRINTER)
            eps_bold(0.25,strng);
        else
            pr_write(strng);

        switch(printer)
        {        
        case OKI_PRINTER:
            font85e(FALSE);
            fontnlq(TRUE);
            break;
        case LX_PRINTER:    
        case USB_PRINTER:
        case USB_EPS_PRINTER:    
            break;
        case ROLL_PRINTER:
		case USB_EPS_LABEL_PRINTER:	
        case SLIP_PRINTER:
            font35();
            break;
        }    
        //strcpy(strng,&measurement[ch_num].future.actstr[6]);
        strcpy(strng,&measurement[ch_num].future.actstr[strlen(measurement[ch_num].future.actstr) - 3]);
        pr_mu(&strng[0],printer);
        strcat(strng," ");
        pr_write(strng);
        switch(printer)
        {        
        case OKI_PRINTER:
            //font85e(FALSE);
            //fontnlq(TRUE);
            break;
        case ROLL_PRINTER:
		case USB_EPS_LABEL_PRINTER:	
        case SLIP_PRINTER:
        case USB_EPS_PRINTER:    
            break;
        case LX_PRINTER:
            bold(FALSE);
            eps_cpi(10);
            break;
        case USB_PRINTER:
            pcl_bold(FALSE);
            break;
        }    

        timeoutsec(&strng[0],&measurement[ch_num].future.dosetime);

        if(!same_day(&nowtime,&measurement[ch_num].future.dosetime))
        {
            strcat(strng," ");
            dateout_language(dstr,&measurement[ch_num].future.dosetime,2);
            strcat(strng,dstr);
        }
		
        strcat(strng,"\r\n");
        pr_write(strng);
	}


    static	void print_nuclide_name(void)
	{
		short i;
		char ch;
        short ch_num = current.main_chamber;
        char str[8];
        short t_index;
        char strng[90];

        lininit(strng,FALSE,printer);
        if(chamber[ch_num].calkey)    
		{
            switch(printer)
            {
            case OKI_PRINTER:
				fontnlq(1);
                break;
            case ROLL_PRINTER:
			case USB_EPS_LABEL_PRINTER:	
            case SLIP_PRINTER:    
				font17();
                break;
            case LX_PRINTER:
                eps_cpi(12);
                break;
            case USB_PRINTER:
                pcl_horizontal("360");
                pcl_cpi("12",TRUE);
                break;
            case USB_EPS_PRINTER:
                eps_vert(TRUE,1.0);
                eps_horiz(0.5);
                break;
            }
            
            strncpy(&strng[1],"Cal#:",5);
            
			for(i = 0; i < 7; i++)
			{
				ch = chamber[ch_num].calstrng[i];
                str[i] = ch;
				if(ch == MULT)
                    str[i] = 'x';
				if(ch == DIVSIGN)
                {
                    //if(printer == USB_EPS_PRINTER)
                    if(printer == USB_EPS_PRINTER || printer == USB_PRINTER)
                        str[i] = '/';
                    else
                        str[i] = DIV_PRT;
                }    
                if(ch == '\0')
                    str[i] = ' ';
			}
            strncpy(&strng[7],str,7);
            //pr_write(strng);

            strcpy(&strng[15],"\r\n");
            switch(printer)
            {
            case OKI_PRINTER:
				font6e(TRUE);
                pr_write(strng);
                break;
            case ROLL_PRINTER:
			case USB_EPS_LABEL_PRINTER:	
            case SLIP_PRINTER:
				font17();
                pr_write(strng);
                break;
            case LX_PRINTER:
                bold(TRUE);
                eps_cpi(12);
                eps_dbl(TRUE);  //double to 6cpi
                pr_write(strng);
                break;
            case USB_PRINTER:
                pcl_cpi("6",TRUE);
                pcl_bold(TRUE);
                pcl_horizontal("360");
                pr_write(strng);
                break;
            case USB_EPS_PRINTER:
                eps_vert(TRUE,1.0);
                eps_bold(0.5,strng);
                break;
            }
		}

		else
		{
            strncpy(&strng[1],&chamber[ch_num].nucdata.name[0],6);
            
            t_index = chamber[ch_num].type;
            strncpy(&strng[10],cal_type_str[t_index],strlen(cal_type_str[t_index]));
            strcpy(&strng[15],"\r\n");

            switch(printer)
            {        
            case OKI_PRINTER:
				reverse_feed(2,printer);
				font6e(TRUE);
                pr_write(strng);
                break;
            case ROLL_PRINTER:
			case USB_EPS_LABEL_PRINTER:	
            case SLIP_PRINTER:
				font17();
                pr_write(strng);
                break;
            case LX_PRINTER:
                bold(TRUE);
                eps_cpi(12);
                eps_dbl(TRUE);  //double to 6cpi
                pr_write(strng);
                break;
            case USB_PRINTER:
                pcl_cpi("6",TRUE);
                pcl_bold(TRUE);
                pcl_horizontal("360");
                pr_write(strng);
                break;
            case USB_EPS_PRINTER:
                eps_vert(TRUE,1.0);
                eps_bold(0.5,strng);
                break;
            }

            
            
		}

	}

    // staffid
    static void print_staffid(void)
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
		case USB_EPS_LABEL_PRINTER:	
        case SLIP_PRINTER:
            font35();
            break;
        case LX_PRINTER:
            bold(FALSE);
            eps_dbl(FALSE);
            eps_cpi(10);
            break;
        case USB_EPS_PRINTER:
            eps_horiz(0.5);
            break;
        }
        
        lininit(strng,pflag,printer);
        //chamber number if more than 1 chamber
        if(current.num_chambers > 1)
        {    
            strncpy(&strng[kp + 3],"CH:",3);
            strng[kp + 7] = current.main_chamber + '1';
        }    
        
        //strncpy(&strng[20],"BY:",3);
        //get_message(DAILY_13,&strng[kp + 18]);
        
        if (id_set())
        {
        	get_message(DAILY_13,&strng[kp + 18]);
            get_id_str(str);
            strncpy(&strng[kp + 22],str,2);
        }
        strcpy(&strng[25],"\r\n");
        pr_write(strng);


    }   

    void set_act_line(char printer)
    {

        switch(printer)
        {        
        case OKI_PRINTER:
            fontnlq(FALSE);
            font85e(TRUE);
            break;
        case ROLL_PRINTER:
		case USB_EPS_LABEL_PRINTER:	
        case SLIP_PRINTER:
            font21();
            break;
        case LX_PRINTER:
            bold(TRUE);
            eps_cpi(10);
            break;
        case USB_PRINTER:
            pcl_cpi("10",TRUE);
            pcl_bold(TRUE);
            pcl_horizontal("180");  //1/4 inch over
            break;
        case USB_EPS_PRINTER:
            break;
        }    

    }    
    
    //print activity data in 1 line on OKI or LX printer
    void prline(void)
	{
        time_t nowtime;
		short i;
        short ch_num = current.main_chamber;
        char strng[90];
        char str[20];
        char ch;
        short iby;
        short i0;
        short k0;
        char acMsg[20];

		if (!check_oki(PAPER))
			return;

        //printer = OKI_PRINTER;
        printer = current.printer;
        
        uart_write("\r",U_PR);
        lininit(strng,TRUE,printer);

        nowtime = clock_time;
		
        i0 = 0;
        //chamber number if more than 1 chamber
        if(current.num_chambers > 1)
        {    
            strncpy(strng,"CH:",3);
            strng[3] = ch_num + '1';
            i0 = 5;
        }    
        // nuclide  or cal#
        if(chamber[ch_num].calkey)    
		{
            strncpy(&strng[i0],"CAL#:",5);
			for(i = 0; i < 7; i++)
			{
				ch = chamber[ch_num].calstrng[i];
                if(ch == '\0')
                    ch = ' ';
				if(ch == MULT)
					ch = 'x';
				if(ch == DIVSIGN)
					ch = DIV_PRT;
                strng[i + 5 + i0] = ch;
                k0 = i0 + 13;
			}
		}
		else
        {    
            strncpy(&strng[i0],&chamber[ch_num].nucdata.name[0],6);
            k0 = i0 + 7;
        }    

		// activity 
		if (measurement[ch_num].wasover){
            //strncpy(&strng[k0],"OVER",4);
			get_amulet_message(L_OVER, acMsg);    // " OVER "
			trim(acMsg);
			strncpy(&strng[k0], acMsg, 4);
		}else{
            //strncpy(&strng[k0],&measurement[ch_num].actstr[0],9);
			strncpy(&strng[k0 + 11 - strlen(measurement[ch_num].actstr)], &measurement[ch_num].actstr[0], strlen(measurement[ch_num].actstr));
            if (strng[k0 + 8] == MU)
                strng[k0 + 8] = MU_PRT;

            //if(strlen(measurement[ch_num].actstr) == 10) k0+=1;
            //else if(strlen(measurement[ch_num].actstr) == 11) k0+=2;
		}
		k0+=2;
        timeoutsec(str,&nowtime);
        strncpy(&strng[k0 + 10],str,strlen(str));
		daytostr_language(str,&nowtime);
        strncpy(&strng[k0 + 19],str,strlen(str));

        iby = k0 + 33;

		// future activity 
		if(measurement[ch_num].future.dosetime != NO_TIME)
		{
			strncpy(&strng[k0 + 33],&measurement[ch_num].future.actstr[0],9);
			if (strng[k0 + 39] == MU)
				strng[k0 + 39] = MU_PRT;
            timeoutsec(str,&measurement[ch_num].future.dosetime);
            strncpy(&strng[k0 + 43],str,strlen(str));
            iby += 19;
			if(!same_day(&nowtime,&measurement[ch_num].future.dosetime))
			{
				daytostr_language(str,&measurement[ch_num].future.dosetime);
                strncpy(&strng[k0 + 52],str,strlen(str));
                iby += 14;
			}
		}
        // staffid 
        if (id_set())
        {
            strncpy(&strng[iby],"BY:",3);
            get_id_str(str);
            strncpy(&strng[iby + 4],str,2);
        }

        uart_write(strng,U_PR);
	}

    void pr_mu(char *str, char printer)
    {
        if (*str == MU)
        {
            if(printer == USB_EPS_PRINTER)
                *str = 'u';
            else
                *str = MU_PRT;
        }    
    }
