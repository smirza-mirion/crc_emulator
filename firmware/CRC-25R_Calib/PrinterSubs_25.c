/**
 * \file
 * \details This file contains functions, which calls either the UART layer or the USB layer and implements printer calls.
 */
/*********************************************************************
  MODULE: Subroutines for the Printers

  FILE:		PrinterSubs_25.c

  DATE: 	01/25/07

      *************************************************************************/

#include "crc.h"
#include "uart.h"
#include "printer.h"
#include "i2c.h"
#include "screen.h"
#include "keyboard.h"
#include "pit.h"
#include "sl811h.h"
#include "message.h"
#include "mca.h"
#include <string.h>
#include <stdlib.h>

#define FIRST_TIME  -2
#define NOT_READ -100
static void release_paper(void);
static char paper_status(bool rel_flag);
static bool oki_printer_on(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void GetExtendedTimeInfoLanguage(time_t *dtmDateTime, char *acMsg);
void trim(char *acByte);

void usb_bold(bool on);

//static int linecnt;
int linecnt;
static unsigned char gbuffer[320][76];
static short gheight, gleftpixel;

extern ushort g_Init_USB_PCL;
extern const FONT fonts[];
extern bool printed_margin;

void convert_printing_usb(char *strng);
void convert_printing_serial(char *strng0, char *strng);
static void convert_printing_serial1(char *strng);

//Checks OKI printer to make sure it is on
    /*bool check_oki(short type)
	{

		bool status;
		short prev;

		prev = FIRST_TIME;


		for(;;)
		{
			status = oki_printer_on();
            if(status)
                return TRUE;

            if(prev == NO_PRINTER)
                return FALSE;

            prev = NO_PRINTER;
            
			beep();
            erase_lines(42,22,0);
            erase_lines(42,22,1);
			//display_text(8,42,strng,0,MEDIUM,NORMAL);
            if(type == TICKET)
                display_medium_message(PRINTER_2,42,0,NORMAL);  //INSERT TICKET
            else
                display_medium_message(PRINTER_3,42,0,NORMAL);  //CHECK PRINTER
            contmsg();
            erase_lines(42,22,0);
		}

	}*/

/**
 * \details Check oki printer and display error message if there is a problem
 * \param type 0 = Ticket printer, 1 = Line printer
 * \returns True = Printer OK, False = Printer Error
 */
bool check_oki(short type){
	char titlestring[50], messagestring[100];
	bool status;

	status = oki_printer_on();
	if(status) return TRUE;

	get_amulet_message(L_PRINTER_PROBLEM, titlestring);    // "Printer Problem"
	if(type == TICKET)
		get_amulet_message(L_INSERT_TICKET, messagestring);    // "INSERT TICKET"
	else
		get_amulet_message(L_CHECK_PRINTER, messagestring);    // "CHECK PRINTER"
	Amulet_DisplayError(titlestring, messagestring,TRUE);
	return FALSE;
}

    //check USB Printer connection and paper status
/*    bool check_usb_printer(void)
    {
        bool usb_ret;
        short prev;

        prev = FIRST_TIME;


        for(;;)
        {
            usb_ret = IsUSBPrinterConnected();
            if(usb_ret)
                return TRUE;

            if(prev == NO_PRINTER)
                return FALSE;

            prev = NO_PRINTER;
            
            beep();
            erase_lines(42,22,0);
            erase_lines(42,22,1);
            display_medium_message(PRINTER_3,42,0,NORMAL);  //CHECK PRINTER            
            contmsg();
            erase_lines(42,22,0);
        }
    
    } */

/**
 * \details Check usb printer and display error message if there is a problem
 * \returns True = Printer OK, False = Printer Error
 */
bool check_usb_printer(void){
	bool usb_ret;
	char titlestring[50], messagestring[100];

	usb_ret = IsUSBPrinterConnected();
	if(usb_ret) return TRUE;

	get_amulet_message(L_PRINTER_PROBLEM, titlestring);    // "Printer Problem"
	get_amulet_message(L_CHECK_PRINTER, messagestring);    // "CHECK PRINTER"
	Amulet_DisplayError(titlestring, messagestring,TRUE);
	return FALSE;
}
    
    //performs form feed according to printer
static bool oki_printer_on(void);
/**
 * \details Form feed the printer
 * \param printer Printer ID
 * \returns None
 */
    void formfeed(char printer)
    {

        switch(printer)
        {
        case SLIP_PRINTER:
            uart_write("\f",U_PR);
            release_paper();
            break;
        case ROLL_PRINTER:
        case USB_EPS_LABEL_PRINTER:	
            feed(6,printer);
            break;
        case OKI_PRINTER:
            uart_write("\r\f\r\r",U_PR);
            break;
        case USB_PRINTER:
        case USB_EPS_PRINTER:    
            usb_write("\r\f");
            break;
        case LX_PRINTER:    
            uart_write("\r\f\r\r",U_PR);
            break;
        case NONE_PRINTER:
            break;
        }
    }



	
    //initialize line according to printer
/**
 * \details Initialize line according to printer
 * \param strng Pointer to byte array to be initialized
 * \param flag Flag to be set after the carriage return
 * \param printer Printer type
 * \returns None
 */
    void lininit(char *strng, bool flag, char printer)
	{
		short i,imax;

        if(printer == NONE_PRINTER)
           return;

        switch(printer)
        {
        case ROLL_PRINTER:
        case USB_EPS_LABEL_PRINTER:	
        case SLIP_PRINTER:
            imax = 37;
            break;
        case OKI_PRINTER:
        case LX_PRINTER:    
            imax = 78;
            if(flag)
                pr_write("            ");
            break    ;
        case USB_PRINTER:    
            imax = 78;
            if(flag)
                pcl_horizontal("720");
            break;
        case USB_EPS_PRINTER:
            imax = 70;
            if(flag)
                eps_horiz(0.5);
            break;
        }       


		for(i = 0; i < imax; i++)
			strng[i] = ' ';
		strng[imax] = '\r';
		strng[imax + 1] = '\n';
        strng[imax + 2] = '\0';
        strng[imax + 3] = printer;
        strng[imax + 4] = flag;
	}

/**
 * \details Initialize line according to printer for output from the database
 * \param strng Pointer to byte array to be initialized
 * \param flag Flag to be set after the carriage return
 * \param printer Printer type
 * \returns None
 */
    void lininit_db(char *strng, bool flag, char printer)
	{
		short i,imax;

        if(printer == NONE_PRINTER)
           return;

        switch(printer)
        {
        case ROLL_PRINTER:
        case USB_EPS_LABEL_PRINTER:
        case SLIP_PRINTER:
            imax = 34;
            break;
        case OKI_PRINTER:
        case LX_PRINTER:
            imax = 78;
            if(!printed_margin){
            	if(flag) pr_write("            ");
            	printed_margin = TRUE;
            }
            break;
        case USB_PRINTER:
            imax = 78;
            if(!printed_margin){
            	if(flag) pcl_horizontal("720");
            	printed_margin = TRUE;
            }
            break;
        case USB_EPS_PRINTER:
            imax = 70;
            if(!printed_margin){
            	if(flag) eps_horiz(0.5);
            	printed_margin = TRUE;
            }
            break;
        }


		for(i = 0; i < imax; i++)
			strng[i] = ' ';
		strng[imax] = '\r';
		strng[imax + 1] = '\n';
        strng[imax + 2] = '\0';
        strng[imax + 3] = printer;
        strng[imax + 4] = flag;
        strng[imax + 5] = FALSE; //Large Flag
        strng[imax + 6] = FALSE; //Bold Flag
	}

/**
 * \details Update the line with the large and bold attributes
 * \param strng Pointer to byte array to be updated
 * \param large Large Attribute
 * \param bold Bold Attribute
 * \returns None
 */
    void linupdate_db(char *strng, bool large, bool bold)
    {
    	int length;

    	length = strlen(strng);
    	strng[length + 3] = large;
    	strng[length + 4] = bold;
    }

void lininit3(char *strng, bool flag, char printer){
	short i,imax;

	if(printer == NONE_PRINTER) return;

	switch(printer){
		case ROLL_PRINTER:
		case USB_EPS_LABEL_PRINTER:
		case SLIP_PRINTER:
			imax = 34;
			break;
		case OKI_PRINTER:
		case LX_PRINTER:
			imax = 78;
			if(flag) pr_write("            ");
			break;
        case USB_PRINTER:
        	imax = 78;
        	if(flag) pcl_horizontal("720");
        	break;
        case USB_EPS_PRINTER:
        	imax = 70;
        	if(flag) eps_horiz(0.5);
        	break;
	}

	for(i=0; i<imax; i++){
		strng[i] = ' ';
		strng[100+i] = 0;
	}

	strng[imax] = '\r';
	strng[imax + 100] = 0;
	strng[imax + 1] = '\n';
	strng[imax + 101] = 0;
	strng[imax + 2] = '\0';
	strng[imax + 102] = 0;
	strng[imax + 3] = printer;
	strng[imax + 103] = 0;
	strng[imax + 4] = flag;
	strng[imax + 104] = 0;
}

    extern CURRENT current;

//initialize printer
/**
 * \details Initialize Printer
 * \returns None
 */
void printer_init(void){
	char str[6];

	switch(current.printer){
		case OKI_PRINTER:
			break;

		case ROLL_PRINTER:
		case USB_EPS_LABEL_PRINTER:	
		case SLIP_PRINTER:
			pr_reset();
			break;

		case LX_PRINTER:
			pr_reset();
			//set Draft mode
			strcpy(str," x0");
			str[0] = ESC;
			uart_write(str,U_PR);
			break;

		case USB_PRINTER:
			g_Init_USB_PCL = 0;
			pcl_reset();
			break;

		case USB_EPS_PRINTER:
			g_Init_USB_PCL = 0;
			eps_reset(TRUE);
			break;
	}
}

	
    //feed n lines, according to printer
/**
 * \details Feed n lines
 * \param num Number of line feed
 * \param printer Printer number
 * \returns None
 */
    void feed(short num,char printer)
	{
		short i;
        char strng[22];
        short n;

        if(printer == NONE_PRINTER)
           return;
           
        n = num;
        if(n > 9)
            n = 9;

		switch(printer)
		{
        case SLIP_PRINTER:
		case ROLL_PRINTER:
			strng[0] = ESC;
			strng[1] = 'd';
			strng[2] = n;
            strng[3] = '\0';
            uart_write(strng,U_PR);
			break;
		case USB_EPS_LABEL_PRINTER:	
			strng[0] = ESC;
			strng[1] = 'd';
			strng[2] = n;
            strng[3] = '\0';
            usb_write(strng);
			break;
		case OKI_PRINTER:
        case LX_PRINTER:    
			for(i = 0; i < n; i++)
				strng[i] = '\n';

            if(printer == LX_PRINTER){
            	strng[n] = '\0';
            }else{
            	strng[n] = '\r';
            	strng[n+1] = '\0';
            }
            uart_write(strng,U_PR);
			break;
        case USB_PRINTER:    
        case USB_EPS_PRINTER:    
            for(i = 0; i < n; i++)
                strng[i] = '\n';
            strng[n] = '\0';
            usb_write(strng);
            break;

        }


		linecnt += n;
	}

/**
 * \details Feed n lines for output from database
 * \param num Number of line feed
 * \param printer Printer number
 * \returns None
 */
    void feed_db(short num,char printer)
	{
		short i;
        char strng[22];
        short n;

        if(printer == NONE_PRINTER)
           return;

        n = num;
        if(n > 9)
            n = 9;

		switch(printer)
		{
        case SLIP_PRINTER:
		case ROLL_PRINTER:
			strng[0] = ESC;
			strng[1] = 'd';
			strng[2] = n;
            strng[3] = '\0';
            uart_write(strng,U_PR);
			break;
		case USB_EPS_LABEL_PRINTER:
			strng[0] = ESC;
			strng[1] = 'd';
			strng[2] = n;
            strng[3] = '\0';
            usb_write(strng);
			break;
		case OKI_PRINTER:
        case LX_PRINTER:
			for(i = 0; i < n; i++)
				strng[i] = '\n';
            strng[n] = '\0';
            uart_write(strng,U_PR);
			break;
        case USB_PRINTER:
        case USB_EPS_PRINTER:
            for(i = 0; i < n; i++)
                strng[i] = '\n';
            strng[n] = '\0';
            usb_write(strng);
            break;

        }

		if(printer == LX_PRINTER){
			pr_write("            ");
		}

		linecnt += n;
	}

/**
 * \details Reverse feed n lines
 * \param num Number of reverse line feed
 * \param printer Printer number
 * \returns None
 */
	void reverse_feed(short num, char printer)
	{
		char str[10];
		short i;
		
		if(printer == OKI_PRINTER || printer == LX_PRINTER)
		{
			str[0] = ESC;
			str[1] = ']';
			str[2] = '\0';
			for(i = 0; i < num; i++)
				uart_write(str,U_PR);
		}
		else
			return;
	}	

    void feed2(short num,char printer)
	{
		short i;
        char strng[22];
        short n;

        if(printer == NONE_PRINTER)
           return;

        n = num;
        if(n > 9)
            n = 9;

        linecnt += n;

		switch(printer)
		{
        case SLIP_PRINTER:
		case ROLL_PRINTER:
			strng[0] = ESC;
			strng[1] = 'd';
			strng[2] = n;
            strng[3] = '\0';
            uart_write(strng,U_PR);
			break;
		case OKI_PRINTER:
        case LX_PRINTER:
			for(i = 0; i < n; i++)
				strng[i] = '\n';
            strng[n] = '\0';
            uart_write(strng,U_PR);
			break;
        case USB_PRINTER:
        case USB_EPS_PRINTER:
            for(i = 0; i < n; i++)
                strng[i] = '\n';
            strng[n] = '\0';
            usb_write(strng);
            break;

        }
	}

/**
 * \details Send paper release command to SLIP printer
 * \returns None
 */
	static void release_paper(void)
	{
		char strng[4];

		strng[0] = ESC;
		strng[1] = 'q';
        strng[2] = '\0';
        uart_write(strng,U_PR);

	}

    
    /* for SLIP printer:
	 checks for printer connected and paper in
	   displays appropriate messages
	*/
/*	bool insert_paper(short type, bool rel_flag)
	{

		char status;
		char prev;
		char strng[16];

		prev = FIRST_TIME;

		if(rel_flag)
			release_paper();

		for(;;)
		{
			status = paper_status(rel_flag);
			switch(status)
			{
			case PAPER_IN:
            case PAPER_IN_ALL:    
				return TRUE;

			case NO_PRINTER:
				if(prev == NO_PRINTER)
					return FALSE;
				strcpy(strng,"CONNECT PRINTER");
				prev = NO_PRINTER;
				break;

			default:
				if(prev == NO_PRINTER || prev == FIRST_TIME)
				{
					strcpy(&strng[0],"INSERT TICKET  ");
					if (type == PAPER)
						strcpy(&strng[7],"PAPER ");
					prev = status;
					break;
				}
				return FALSE;
			}
			beep();
            read_screen(0);
            read_screen(1);
            erase_screen();
			display_text(8,42,strng,0,MEDIUM,NORMAL);
			display_text(6,56,"Any Key to Continue",0,SMALL,NORMAL);
			(void)keyin();
            erase_screen();
            write_screen(0);
            write_screen(1);
            if(home_set())
                return FALSE;
		}
	} */

/**
 * \details Display release paper message
 * \param type 0 = Ticket, 1 = Roll
 * \param rel_flag True = Send release command, False = Do not send release command
 * \returns True = Paper is present, False = No paper in printer
 */
bool insert_paper(short type, bool rel_flag){
	char status;
	char strng[100], titlestring[50];

	if(rel_flag) release_paper();

	get_amulet_message(L_PRINTER_PROBLEM, titlestring);    // "Printer Problem"
	status = paper_status(rel_flag);
	switch(status){
		case PAPER_IN:
		case PAPER_IN_ALL:
			return TRUE;

		case NO_PRINTER:
			//strcpy(strng,"CONNECT PRINTER");
			get_amulet_message(L_CONNECT_PRINTER, strng);    // "CONNECT PRINTER"
			break;

		default:
			//strcpy(&strng[0],"INSERT TICKET  ");
			//if (type == PAPER) strcpy(&strng[7],"PAPER ");
			if(type == PAPER) get_amulet_message(L_INSERT_PAPER, strng);    // "INSERT PAPER"
			else get_amulet_message(L_INSERT_TICKET, strng);    // "INSERT TICKET"
			break;
	}

	Amulet_DisplayError(titlestring, strng, TRUE);
	return FALSE;
}

    /* For SLIP printer:
	 sends command to printer and reads back status
	   rel_flag (release) set at beginning of printing only
	   checks for printer connected when release flag set
	   via timeout  
	*/
/**
 * \details For SLIP printer, sends command to printer and reads back status
 * \param rel_flag True = Send release command, False = Do not send release command
 * \returns Paper status
 */
    static char paper_status(bool rel_flag)
	{
		char status;
		short num;
		char strng[2];
        
        strng[0] = ESC;
		strng[1] = 'v';
        uart_write_num(strng,2,U_PR);

		num = 0;
        status = NO_PRINTER;
		/* wait for return unless timed out */
		for(;;)
		{
			++num;
            delay_msec(5);
            if(uart_char_waiting(U_PR))
            {   
                status = uart_getchar(FALSE,U_PR);
                if((status & NOT_CONNECTED) == 0)
                    break;
            }        
            if ((rel_flag) &&( num > 500))
                break;

        }
		return status;
	}

/**
 * \details Initialize Printer
 * \param printer Printer type
 * \param mode 1 = Check printer, Other = Skip check
 * \param flag True = Indent next line, False = No indent
 * \param type 0 = Ticket, 1 = Paper
 * \returns True = Printer OK, False = Printer Error
 */
	bool start_printer(char printer, short mode, bool flag, short type)
	{

		char strng[4];

		// read screen
		read_screen(0);
        read_screen(1);

		switch(printer)
		{
		case SLIP_PRINTER:
			pr_reset();

			if(mode == 1)
			{
				if (!insert_paper(type,TRUE))
				{
					// restore screen
                    write_screen(0);
                    write_screen(1);
					return FALSE;
				}

				if (type == TICKET)
				{
					/* reverse  */
					strng[0] = ESC;
					strng[1] = 'K';
					strng[2] = 32;
                    uart_write_num(strng,3,U_PR);
				}
			}
			break;

		case OKI_PRINTER:
			if(mode == 1)
			{
				if(!check_oki(type))
				{
					//restore screen
					write_screen(0);
                    write_screen(1);
					return FALSE;
				}
			}
			fontnlq(TRUE);
			if(flag)
                uart_write("\r\n    ",U_PR);
			break;

        case LX_PRINTER:
            if(mode == 1)
            {
                uart_write("\r",U_PR);
                if(!check_oki(type))
                {
                    //restore screen
                    write_screen(0);
                    write_screen(1);
                    return FALSE;
                }
            }
            if(flag)
                uart_write("\r\n    ",U_PR);
            break;

		case ROLL_PRINTER:
		//case USB_EPS_LABEL_PRINTER:	
			pr_reset();
			break;

		case NONE_PRINTER:
			return FALSE;

        case USB_PRINTER:
        case USB_EPS_PRINTER:    
		case USB_EPS_LABEL_PRINTER:	
            if(mode == 1)
            {
                if(!check_usb_printer())
                {
                    //restore screen
                    write_screen(0);
                    write_screen(1);
                    return FALSE;
                }
            }

            if(printer == USB_PRINTER)
            {    
                pcl_symbol("10U");
                pcl_cpi("10",TRUE);
            }
            break;
		}


		return TRUE;
	}

//extern const char *pr_crcname; //for printing header
extern char pr_crcname[];
extern char pr_crcname_large[];
//extern const char *oki_crcname;    //for OKI header & USB printers

	//print heading
	extern CURRENT current;
    extern const char *rev_num;
    extern time_t clock_time;
    extern time_t low_clock_time;
/**
 * \details Print report header
 * \param printer Printer type
 * \returns None
 */
    void prhead(char printer)
    {
        char sn[10];
        time_t nowtime;
        char strng[90];
        char dstr[12];
        short kp;
		char message[50];
		int len;

        switch(printer)
        {
        case NONE_PRINTER:        
            return;
        
        case OKI_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            // 12 cpi, HSD 
            strng[0] = ESC;   //10 cpi util 
            strng[1] = 'I';
            strng[2] = '0';
            strng[3] = ESC;    //12 cpi
            strng[4] = 0x3a;
            strng[5] = '\r';
            strng[6] = '\0';
            uart_write(strng,U_PR);

            bold(TRUE);
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-TOUCH    RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);
            
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&nowtime,4);
            strncpy(&strng[58],dstr,11);
            timeout(dstr,&nowtime);
            strncpy(&strng[71],dstr,5);
            //uart_write(strng,U_PR);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			
			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);

            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
            lininit(strng,TRUE,printer);
            ReadSN(sn);
            strncpy(strng,pr_crcname,strlen(pr_crcname));
            //strncpy(&strng[13],"REV       SN: ",14);
			get_amulet_message(L_REV_SN,message);    // "REV       SN: "
			strncpy(&strng[13],message,strlen(message));
            strncpy(&strng[17],rev_num,strlen(rev_num));
            strncpy(&strng[27],sn,10);
            pr_write(strng);
            prdate(printer);
            break;

        case LX_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();
    
            // 12 cpi
            eps_cpi(12);
            bold(TRUE);
            
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);
            
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&nowtime,4);
            strncpy(&strng[58],dstr,11);
            timeout(dstr,&nowtime);
            strncpy(&strng[71],dstr,5);
            pr_write(strng);
            bold(FALSE);
    
            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);
            
            break;

        case USB_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();
            
            //12 cpi
            pcl_cpi("12",FALSE);
    
            pcl_bold(TRUE);

            kp = 52;
    
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeout(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,5);
			pr_write(strng);
    
            pcl_bold(FALSE);
    
            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			
			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            //return to 10cpi
            pcl_cpi("10",TRUE);
            break;
        
        case USB_EPS_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            eps_vert(TRUE,0.75);
            
            kp = 52;
    
            lininit(strng,FALSE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);
            
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeout(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,5);
            eps_bold(0.5,strng);
    
            // serial number
            lininit(strng,TRUE,printer);
            strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[19],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            break;
        }
    }

/**
 * \details Print report header with either english or french
 * \param printer Printer type
 * \returns None
 */
void prhead_language(char printer)
    {
        char sn[10];
        time_t nowtime;
        char strng[90];
        char dstr[12];
        short kp;
		char message[50];
		int len;

        switch(printer)
        {
        case NONE_PRINTER:
            return;

        case OKI_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            // 12 cpi, HSD
            strng[0] = ESC;   //10 cpi util
            strng[1] = 'I';
            strng[2] = '0';
            strng[3] = ESC;    //12 cpi
            strng[4] = 0x3a;
            strng[5] = '\r';
            strng[6] = '\0';
            uart_write(strng,U_PR);

            bold(TRUE);
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[58],dstr,11);
            timeout(dstr,&nowtime);
            strncpy(&strng[71],dstr,5);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            //uart_write(strng,U_PR);
			pr_write(strng);

            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
            lininit(strng,TRUE,printer);
            ReadSN(sn);
            strncpy(strng,pr_crcname,strlen(pr_crcname));
            //strncpy(&strng[13],"REV       SN: ",14);
			get_amulet_message(L_REV_SN,message);    // "REV       SN: "
			strncpy(&strng[13],message,strlen(message));
            strncpy(&strng[17],rev_num,strlen(rev_num));
            strncpy(&strng[27],sn,10);
            pr_write(strng);
            prdate_language(printer);
            break;

        case LX_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            // 12 cpi
            eps_cpi(12);
            bold(TRUE);

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[58],dstr,11);
            timeout(dstr,&nowtime);
            strncpy(&strng[71],dstr,5);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            //strncpy(&strng[19],dstr,6);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            //uart_write(strng,U_PR);
			pr_write(strng);

            break;

        case USB_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            //12 cpi
            pcl_cpi("12",FALSE);

            pcl_bold(TRUE);

            kp = 52;

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeout(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,5);
			pr_write(strng);

            pcl_bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
            ReadSN(dstr);
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
			pr_write(strng);

            //return to 10cpi
            pcl_cpi("10",TRUE);
            break;

        case USB_EPS_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            eps_vert(TRUE,0.75);

            kp = 52;

            lininit(strng,FALSE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeout(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,5);
            eps_bold(0.5,strng);

            // serial number
            lininit(strng,TRUE,printer);
            strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[19],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            break;
        }
    }

void prhead_language_db(char printer){
	int len;
	short kp;
	time_t nowtime;
	char sn[10], dstr[12], message[50], strng[90];

	read_clock(&nowtime);
	clock_time = nowtime;
	low_clock_time = clock_time;
	reset_minute_counter_with_seconds();
	ReadSN(sn);

	switch(printer){
		case NONE_PRINTER:
			return;

        case OKI_PRINTER:
            // 12 cpi, HSD
            strng[0] = ESC;   //10 cpi util
            strng[1] = 'I';
            strng[2] = '0';
            strng[3] = ESC;    //12 cpi
            strng[4] = 0x3a;
            strng[5] = '\r';
            strng[6] = '\0';
            uart_write(strng,U_PR);

            bold(TRUE);
            lininit(strng, TRUE, printer);

            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));

            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[68],dstr,8);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);

            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);

			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            strncpy(&strng[4 + len],sn,10);
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            //uart_write(strng,U_PR);
			pr_write(strng);
            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
            lininit(strng,TRUE,printer);
            strncpy(strng,pr_crcname,strlen(pr_crcname));
            //strncpy(&strng[13],"REV       SN: ",14);
			get_amulet_message(L_REV_SN,message);    // "REV       SN: "
			strncpy(&strng[13],message,strlen(message));
            strncpy(&strng[17],rev_num,strlen(rev_num));
            strncpy(&strng[27],sn,10);
            pr_write(strng);

            lininit(strng,TRUE,printer);
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[0], dstr, 11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[13], dstr, 8);
            pr_write(strng);
            break;

        case LX_PRINTER:
            // 12 cpi
            eps_cpi(12);
            bold(TRUE);

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[68],dstr,8);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            //strncpy(&strng[19],dstr,6);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
            strncpy(&strng[4 + len],sn,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            //uart_write(strng,U_PR);
			pr_write(strng);
            break;

        case USB_PRINTER:
            //12 cpi
            pcl_cpi("12",FALSE);

            pcl_bold(TRUE);
            kp = 49;
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,8);
			pr_write(strng);
            pcl_bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[4 + len],sn,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
			pr_write(strng);

            //return to 10cpi
            pcl_cpi("10",TRUE);
            break;

        case USB_EPS_PRINTER:
            eps_vert(TRUE,0.75);
            kp = 49;

            lininit(strng,FALSE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,8);
            eps_bold(0.5,strng);

            // serial number
            lininit(strng,TRUE,printer);
            strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            strncpy(&strng[38],rev_num,strlen(rev_num));
            strncpy(&strng[19],sn,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;
        }
    }
/**
 * \details Print report header with test time
 * \param printer Printer type
 * \param testtime Test time
 * \returns None
 */
void prhead_testtime(char printer, time_t testtime){
	char sn[10];
	char strng[90];
	char dstr[12];
	short kp;
	char message[50];
	int len;

	switch(printer){
		case NONE_PRINTER:
			return;

        case OKI_PRINTER:
            // 12 cpi, HSD
            strng[0] = ESC;   //10 cpi util
            strng[1] = 'I';
            strng[2] = '0';
            strng[3] = ESC;    //12 cpi
            strng[4] = 0x3a;
            strng[5] = '\r';
            strng[6] = '\0';
            uart_write(strng,U_PR);

            bold(TRUE);
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&testtime,4);
            strncpy(&strng[58],dstr,11);
            timeout(dstr,&testtime);
            strncpy(&strng[71],dstr,5);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
            lininit(strng,TRUE,printer);
            ReadSN(sn);
            strncpy(strng,pr_crcname,strlen(pr_crcname));
            //strncpy(&strng[13],"REV       SN: ",14);
			get_amulet_message(L_REV_SN,message);    // "REV       SN: "
			strncpy(&strng[13],message,strlen(message));
            strncpy(&strng[17],rev_num,strlen(rev_num));
            strncpy(&strng[27],sn,10);
            pr_write(strng);
            prdate_measure(printer, &testtime);
            break;

        case LX_PRINTER:
            // 12 cpi
            eps_cpi(12);
            bold(TRUE);

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&testtime,4);
            strncpy(&strng[58],dstr,11);
            timeout(dstr,&testtime);
            strncpy(&strng[71],dstr,5);
            pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;

        case USB_PRINTER:
            //12 cpi
            pcl_cpi("12",FALSE);

            pcl_bold(TRUE);

            kp = 52;

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&testtime,4);
            strncpy(&strng[kp],dstr,11);
            timeout(dstr,&testtime);
            strncpy(&strng[kp + 13],dstr,5);
			pr_write(strng);

            pcl_bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            //return to 10cpi
            pcl_cpi("10",TRUE);
            break;

        case USB_EPS_PRINTER:
            eps_vert(TRUE,0.75);
            kp = 52;
            lininit(strng,FALSE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&testtime,4);
            strncpy(&strng[kp],dstr,11);
            timeout(dstr,&testtime);
            strncpy(&strng[kp + 13],dstr,5);
            eps_bold(0.5,strng);

            // serial number
            lininit(strng,TRUE,printer);
            strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[19],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;
        }
    }
/**
 * \details Print report header with test time in either English or French
 * \param printer Printer type
 * \param testtime Test time
 * \returns None
 */
void prhead_testtime_language(char printer, time_t testtime){
	char sn[10];
	char strng[90];
	char dstr[12];
	short kp;
	char message[50];
	int len;

	switch(printer){
		case NONE_PRINTER:
			return;

        case OKI_PRINTER:
            // 12 cpi, HSD
            strng[0] = ESC;   //10 cpi util
            strng[1] = 'I';
            strng[2] = '0';
            strng[3] = ESC;    //12 cpi
            strng[4] = 0x3a;
            strng[5] = '\r';
            strng[6] = '\0';
            uart_write(strng,U_PR);

            bold(TRUE);
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-TOUCH    RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&testtime,4);
            strncpy(&strng[58],dstr,11);
            timeout(dstr,&testtime);
            strncpy(&strng[71],dstr,5);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			
			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
            lininit(strng,TRUE,printer);
            ReadSN(sn);
            strncpy(strng,pr_crcname,strlen(pr_crcname));
            //strncpy(&strng[13],"REV       SN: ",14);
			get_amulet_message(L_REV_SN,message);    // "REV       SN: "
			strncpy(&strng[13],message,strlen(message));
            strncpy(&strng[17],rev_num,strlen(rev_num));
            strncpy(&strng[27],sn,10);
            pr_write(strng);
            prdate_measure_language(printer, &testtime);
            break;

        case LX_PRINTER:
            // 12 cpi
            eps_cpi(12);
            bold(TRUE);

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&testtime,4);
            strncpy(&strng[58],dstr,11);
            timeout(dstr,&testtime);
            strncpy(&strng[71],dstr,5);
            pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			
			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;

        case USB_PRINTER:
            //12 cpi
            pcl_cpi("12",FALSE);

            pcl_bold(TRUE);

            kp = 52;

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&testtime,4);
            strncpy(&strng[kp],dstr,11);
            timeout(dstr,&testtime);
            strncpy(&strng[kp + 13],dstr,5);
            pr_write(strng);

            pcl_bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			
			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            //return to 10cpi
            pcl_cpi("10",TRUE);
            break;

        case USB_EPS_PRINTER:
            eps_vert(TRUE,0.75);
            kp = 52;
            lininit(strng,FALSE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&testtime,4);
            strncpy(&strng[kp],dstr,11);
            timeout(dstr,&testtime);
            strncpy(&strng[kp + 13],dstr,5);
            eps_bold(0.5,strng);

            // serial number
            lininit(strng,TRUE,printer);
            strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[19],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;
        }
    }

#include "coldfire.h"
/**
 * \details Test if OKI printer is on
 * \returns True = Printer on, False = Printer off
 */
    static bool oki_printer_on(void)
    {
        uchar pr_byte;

        //read CTS
        pr_byte = cf.gpio.porttcp;

        //on if bit 0 is 0
        if((pr_byte & 0x01) == 0)
            return TRUE;

        return FALSE;

    }    

/**
 * \details Print current date and time
 * \param printer Printer Type
 * \returns None
 */
    void prdate(short printer)
    {
        
        time_t nowtime;
        char strng[40];
        char str[12];

        if(printer == NONE_PRINTER)
            return;
        
        read_clock(&nowtime);
        clock_time = nowtime;
        low_clock_time = clock_time;
        reset_minute_counter_with_seconds();
        lininit(strng,FALSE,printer);
        dateout(str,&nowtime,4);
        strncpy(&strng[0],str,11);
        timeout(str,&nowtime);
        strncpy(&strng[12],str,5);
        pr_write(strng);

    }
/**
 * \details Print current date and time in either English or French
 * \param printer Printer Type
 * \returns None
 */
    void prdate_language(short printer)
    {

        time_t nowtime;
        char strng[40];
        char str[12];

        if(printer == NONE_PRINTER)
            return;

        read_clock(&nowtime);
        clock_time = nowtime;
        low_clock_time = clock_time;
        reset_minute_counter_with_seconds();
        lininit(strng,FALSE,printer);
        dateout_language(str,&nowtime,4);
        strncpy(&strng[0],str,11);
        timeout(str,&nowtime);
        strncpy(&strng[12],str,5);
        pr_write(strng);

    }
/**
 * \details Print measurement time
 * \param printer Printer type
 * \param measuredon Pointer to time_t with the measurement time
 * \returns None
 */
    void prdate_measure(short printer, time_t *measuredon)
    {

        //time_t nowtime;
        char strng[40];
        char str[12];

        if(printer == NONE_PRINTER)
            return;

        //read_clock(&nowtime);
        //clock_time = nowtime;
        lininit(strng,FALSE,printer);
        dateout(str,measuredon,4);
        strncpy(&strng[0],str,11);
        timeout(str,measuredon);
        strncpy(&strng[12],str,5);
        pr_write(strng);

    }
/**
 * \details Print measurement time in either English or French
 * \param printer Printer type
 * \param measuredon Pointer to time_t with the measurement time
 * \returns None
 */
    void prdate_measure_language(short printer, time_t *measuredon)
    {

        //time_t nowtime;
        char strng[40];
        char str[12];

        if(printer == NONE_PRINTER)
            return;

        //read_clock(&nowtime);
        //clock_time = nowtime;
        lininit(strng,FALSE,printer);
        dateout_language(str,measuredon,4);
        strncpy(&strng[0],str,11);
        timeout(str,measuredon);
        strncpy(&strng[12],str,5);
        pr_write(strng);

    }
/**
 * \details Print text
 * \param strng Pointer to Null terminated text string
 * \returns None
 */
    void pr_write(char *strng)
    {
		char strng1[200];
		
		if(current.printer == NONE_PRINTER)
            return;
        
        if(current.printer < 0)  //USB printers
		{
			if(current.printer == USB_EPS_LABEL_PRINTER)
				convert_printing_serial1(strng);
			else
				convert_printing_usb(strng);
            usb_write(strng);
		}	
        else
		{	
			convert_printing_serial(strng,strng1);
			uart_write(strng1,U_PR);
		}	
    }    
/**
 * \details Set line count variable
 * \param line Line count
 * \returns None
 */
    void pr_set_linecnt(int line){
    	linecnt = line;
    }

/**
 * \details Print text with header
 * \param strng Pointer to Null terminated text string
 * \param header Pointer to Null terminated header string
 * \returns None
 */
    void pr_write2(char *strng, char *header){
    	char *strPtr;
    	char printer;
    	char margin;
		char strng1[200];

        if(current.printer == NONE_PRINTER)
            return;

        if(current.printer < 0)  //USB printers
		{
        	if(current.printer == USB_EPS_LABEL_PRINTER)
        		convert_printing_serial1(strng);
        	else
        		convert_printing_usb(strng);
            usb_write(strng);
		}	
        else
		{
			convert_printing_serial(strng,strng1);
            uart_write(strng1,U_PR);
		}	

        linecnt++;

        if(current.printer != ROLL_PRINTER){
        	if(linecnt > 53){
        		formfeed(current.printer);
        		if(strcmp(header, "DIAGNOSTICS") == 0){
        			prhead(current.printer);
        			feed(1, current.printer);
        		}else{
        			rawheader(current.printer, header, clock_time);
        		}
        		linecnt = 5;
        	}
        }
        strPtr = strng;
        while(*strPtr != '\r') *strPtr++ = ' ';
        strPtr += 3;
        printer = *strPtr++;
        margin = *strPtr;

        if(margin){
        	switch(printer){
				case OKI_PRINTER:
				case LX_PRINTER:
					pr_write("            ");
					break    ;
				case USB_PRINTER:
					pcl_horizontal("720");
					break;
				case USB_EPS_PRINTER:
					eps_horiz(0.5);
					break;
        	}
        }
    }
/**
 * \details Print text with header in either English or French
 * \param strng Pointer to Null terminated text string
 * \param header Pointer to Null terminated header string
 * \returns None
 */
    void pr_write2_language(char *strng, char *header){
    	char *strPtr;
    	char printer;
    	char margin;
		char strng1[200];

        if(current.printer == NONE_PRINTER)
            return;


        if(current.printer < 0)  //USB printers
		{
        	if(current.printer == USB_EPS_LABEL_PRINTER)
        		convert_printing_serial1(strng);
        	else
        		convert_printing_usb(strng);
            usb_write(strng);
		}
        else
		{
			convert_printing_serial(strng,strng1);
            uart_write(strng1,U_PR);
		}


        linecnt++;

        if(current.printer != ROLL_PRINTER){
        	if(linecnt > 53){
        		formfeed(current.printer);
        		if(strcmp(header, "DIAGNOSTICS") == 0){
        			prhead_languagesec(current.printer);
        			feed(1, current.printer);
        		}else{
        			rawheader_language(current.printer, header, clock_time);
        		}
        		linecnt = 5;
        	}
        }
        strPtr = strng;
        while(*strPtr != '\r') *strPtr++ = ' ';
        strPtr += 3;
        printer = *strPtr++;
        margin = *strPtr;

        if(margin){
        	switch(printer){
				case OKI_PRINTER:
				case LX_PRINTER:
					pr_write("            ");
					break    ;
				case USB_PRINTER:
					pcl_horizontal("720");
					break;
				case USB_EPS_PRINTER:
					eps_horiz(0.5);
					break;
        	}
        }
    }

    void pr_write2_language_db(char *strng, char *header){
    	char *strPtr;
    	char printer;
    	char margin;
		char strng1[200];
		bool large;
		bool boldflag;

        if(current.printer == NONE_PRINTER)
            return;

        if(current.printer != ROLL_PRINTER){
        	if(linecnt > 53){
        		formfeed(current.printer);
        		rawheader_language(current.printer, header, clock_time);

        		strPtr = strng;
        		while(*strPtr != '\r') strPtr++;
        		strPtr += 3;
        		printer = *strPtr++;
        		margin = *strPtr++;
        		large = *strPtr++;
        		boldflag = *strPtr;

				if(margin){
					switch(printer){
						case OKI_PRINTER:
						case LX_PRINTER:
							pr_write("            ");
							break;
						case USB_PRINTER:
							pcl_horizontal("720");
							break;
						case USB_EPS_PRINTER:
							eps_horiz(0.5);
							break;
					}
				}

				if(printer == USB_PRINTER){
					if(large){
						pcl_cpi("10", TRUE);
					}else{
						pcl_cpi("12", TRUE);
					}
				}

				if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER){
					if(boldflag) bold(TRUE);
					else bold(FALSE);
				}

        		linecnt = 5;
        	}
        }

        if(current.printer < 0)  //USB printers
		{
        	if(current.printer == USB_EPS_LABEL_PRINTER)
        		convert_printing_serial1(strng);
        	else
        		convert_printing_usb(strng);
            usb_write(strng);
		}	
        else
		{
			convert_printing_serial(strng,strng1);
            uart_write(strng1,U_PR);
		}	
        linecnt++;

        strPtr = strng;
        while(*strPtr != '\r') *strPtr++ = ' ';
        strPtr += 3;
        printer = *strPtr++;
        margin = *strPtr;

        if(margin){
        	switch(printer){
				case OKI_PRINTER:
				case LX_PRINTER:
					pr_write("            ");
					break;
				case USB_PRINTER:
					pcl_horizontal("720");
					break;
				case USB_EPS_PRINTER:
					eps_horiz(0.5);
					break;
        	}
        }
    }

void print_page_num(void);
void pr_write3(char *strng, char *header){
	int index;
	char *strPtr, *strPtrControl, *outputPtr;
	char printer;
	char margin;
	char output[200];
	char controlstring[15];

	if(current.printer == NONE_PRINTER) return;

	if(current.printer < 0){  //USB printers
		for(index=0; index<200; index++) output[index] = 0;

		// Sent Control Characters
		strPtr = strng;
		strPtrControl = strng + 100;
		outputPtr = output;
		while(*strPtr != 0){
			if(*strPtrControl != 0){
				if(((*strPtrControl) & 1) == 1){ //Bold
					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "(s3B");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);
				}

				if(((*strPtrControl) & 2) == 2){ //Italics
					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "(s1S");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);
				}

				if(((*strPtrControl) & 4) == 4){ //Underline
					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "&d0D");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);
				}

				if(((*strPtrControl) & 8) == 8){ //Normal
					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "(s0B");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);

					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "(s0S");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);

					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "&d@");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);
				}
			}

			*outputPtr = *strPtr;

			strPtr++;
			strPtrControl++;
			outputPtr++;
		}

		convert_printing_usb(output);

		usb_write(output);
	}else
	{	
		convert_printing_serial(strng,output);
		uart_write(output,U_PR);
	}	

	linecnt++;

	if(current.printer != ROLL_PRINTER){
		if(linecnt > 64) //53)
		{
			if(strcmp(header, "DIAGNOSTICS") == 0){
				//don't do anything
			}else{
				//formfeed(current.printer);
				print_page_num();
				rawheader(current.printer, header, clock_time);
				//set to saved values
				if(current.printer == USB_PRINTER)
				{	
					pcl_cpi("",FALSE);
					pcl_height("",FALSE);
					pcl_line_spacing("",FALSE);
				}
			}
			linecnt = 5;
		}
	}
	strPtr = strng;
	strPtrControl = strPtr + 100;
	while(*strPtr != '\r'){
		*strPtr++ = ' ';
		*strPtrControl++ = 0;
	}
	*strPtrControl++ = 0;
	*strPtrControl++ = 0;
	*strPtrControl++ = 0;
	strPtr += 3;
	printer = *strPtr++;
	margin = *strPtr;

	if(margin){
		switch(printer){
			case OKI_PRINTER:
			case LX_PRINTER:
				pr_write("            ");
				break    ;
			case USB_PRINTER:
				pcl_horizontal("720");
				break;
			case USB_EPS_PRINTER:
				eps_horiz(0.5);
				break;
		}
	}
}

void pr_write3_language(char *strng, char *header){
	int index;
	char *strPtr, *strPtrControl, *outputPtr;
	char printer;
	char margin;
	char output[200];
	char controlstring[15];

	if(current.printer == NONE_PRINTER) return;

	if(current.printer < 0){  //USB printers
		for(index=0; index<200; index++) output[index] = 0;

		// Sent Control Characters
		strPtr = strng;
		strPtrControl = strng + 100;
		outputPtr = output;
		while(*strPtr != 0){
			if(*strPtrControl != 0){
				if(((*strPtrControl) & 1) == 1){ //Bold
					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "(s3B");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);
				}

				if(((*strPtrControl) & 2) == 2){ //Italics
					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "(s1S");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);
				}

				if(((*strPtrControl) & 4) == 4){ //Underline
					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "&d0D");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);
				}

				if(((*strPtrControl) & 8) == 8){ //Normal
					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "(s0B");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);

					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "(s0S");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);

					controlstring[0] = 0x1b;
					controlstring[1] = 0;
					strcat(controlstring, "&d@");
					strcat(output, controlstring);
					outputPtr += strlen(controlstring);
				}
			}

			*outputPtr = *strPtr;

			strPtr++;
			strPtrControl++;
			outputPtr++;
		}

		convert_printing_usb(output);

		usb_write(output);
	}else
			{	
		convert_printing_serial(strng,output);
		uart_write(output,U_PR);
	}	

	linecnt++;

	if(current.printer != ROLL_PRINTER){
		if(linecnt > 64) //53)
		{
			if(strcmp(header, "DIAGNOSTICS") == 0){
				//don't do anything
			}else{
				//formfeed(current.printer);
				print_page_num();
				rawheader_languagesec(current.printer, header, clock_time);
				//set to saved values
				if(current.printer == USB_PRINTER)
				{
					pcl_cpi("",FALSE);
					pcl_height("",FALSE);
					pcl_line_spacing("",FALSE);
				}
			}
			linecnt = 5;
		}
	}
	strPtr = strng;
	strPtrControl = strPtr + 100;
	while(*strPtr != '\r'){
		*strPtr++ = ' ';
		*strPtrControl++ = 0;
	}
	*strPtrControl++ = 0;
	*strPtrControl++ = 0;
	*strPtrControl++ = 0;
	strPtr += 3;
	printer = *strPtr++;
	margin = *strPtr;

	if(margin){
		switch(printer){
			case OKI_PRINTER:
			case LX_PRINTER:
				pr_write("            ");
				break    ;
			case USB_PRINTER:
				pcl_horizontal("720");
				break;
			case USB_EPS_PRINTER:
				eps_horiz(0.5);
				break;
		}
	}
}
/**
 * \details Switch Bold printing on HP printers
 * \param on True = Bold, False = Normal
 * \returns None
 */
    void usb_bold(bool on)
    {

        if(current.printer == USB_PRINTER)
            pcl_bold(on);
   }    
/**
 * \details Display printer error screen
 * \returns None
 */
    void pr_error(void)
    {
        beep();
        read_screen(0);
        read_screen(1);
        erase_screen();
        //display_text(8,40,"CHECK PRINTER",0,MEDIUM,NORMAL);
        display_medium_message(PRINTER_3,40,0,NORMAL);
        contmsg();
        write_screen(0);
        write_screen(1);

    }

void pr_patline(char printer, char *str1){
	char strng[90];

	feed(2,printer);
	lininit(strng,TRUE,printer);
	strncpy(&strng[2],str1,5);
	strncpy(&strng[8],"__________________________",26);
	pr_write(strng);
}
    
void lab_head(char printer, char *title, short t_len, time_t testtime){
	char strng[90];

	prhead_testtime(printer, testtime);
	feed(1,printer);
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(TRUE);
	lininit(strng,TRUE,printer);
	strncpy(&strng[5],title,t_len);
	if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
	else pr_write(strng);
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	feed(1,printer);
	pr_patline(printer,"Name:");
	pr_patline(printer,"ID  :");
	pr_patline(printer,"Dr. :");
	feed(2,printer);
}

void rawheader(char printer, char *title, time_t testtime){
	char strng[90];

	if(printer == USB_PRINTER)
	{
		pcl_height("12",FALSE);
		pcl_line_spacing("6",FALSE);
	}

	prhead_testtime(printer, testtime);
	feed(1,printer);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(TRUE);

	if(strlen(title) != 0)
	{	
		lininit(strng, TRUE, printer);
		strncpy(&strng[5], title, strlen(title));
		if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);
	}	
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	if(printer == USB_PRINTER)
		pcl_cpi("12",TRUE);
	feed(1,printer);
}

void rawheader_language(char printer, char *title, time_t testtime){
	char strng[90];

	if(printer == USB_PRINTER)
	{
		pcl_height("12",FALSE);
		pcl_line_spacing("6",FALSE);
	}

	prhead_testtime_language(printer, testtime);
	feed(1,printer);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(TRUE);

	if(strlen(title) != 0)
	{
		lininit(strng, TRUE, printer);
		strncpy(&strng[5], title, strlen(title));
		if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);
	}
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	if(printer == USB_PRINTER)
		pcl_cpi("12",TRUE);
	feed(1,printer);
}

void rawheader2(char printer, char *title1, char *title2, time_t testtime){
	char strng[90];

	if(printer == USB_PRINTER)
	{
		pcl_height("12",FALSE);
		pcl_line_spacing("6",FALSE);
	}

	prhead_testtimesec(printer, testtime);
	feed(1,printer);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(TRUE);

	lininit(strng, TRUE, printer);
	strncpy(&strng[5], title1, strlen(title1));
	if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
	else pr_write(strng);

	lininit(strng, TRUE, printer);
	strncpy(&strng[5], title2, strlen(title2));
	if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
	else pr_write(strng);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);

	if(printer == USB_PRINTER)
		pcl_cpi("12",TRUE);

	feed(1,printer);
}

void well_head(char printer, char *title, short t_len, time_t testtime){
	char strng[90];

	prhead_testtime(printer, testtime);
	feed(1,printer);
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(TRUE);
	lininit(strng,TRUE,printer);
	strncpy(&strng[5],title,t_len);
	if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
	else pr_write(strng);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	feed(1,printer);
	pr_patline(printer,"Name:");
	pr_patline(printer,"ID  :");
	feed(2,printer);
}

// routines for USB EPSON LABEL PRINTER (USB_EPS_LABEL_PRINTER);
/**
 * \details Send start string to the EPSON Label Printer
 * \returns None
 */
void start_label(void)
{
	char strng[8];

	strng[0] = 0x1c;
	strng[1] = '(';
	strng[2] = 'L';
	strng[3] = 2;
	strng[4] = 0;
	strng[5] = 67;
	strng[6] = 50;		//current label
	strng[7] = '\0';

	usb_write_num(strng,7);
	
}
/**
 * \details Send label peel string to the EPSON Label Printer
 * \returns None
 */
void label_peel(void)
{
	char strng[8];

	strng[0] = 0x1c;
	strng[1] = '(';
	strng[2] = 'L';
	strng[3] = 2;
	strng[4] = 0;
	strng[5] = 65;
	strng[6] = 49;
	strng[7] = '\0';

	usb_write_num(strng,7);
	
}	
void well_head2(char printer, char *title1, short t_len1, char *title2, short t_len2, time_t testtime){
	char strng[90];

	prhead_testtime(printer, testtime);
	feed(1,printer);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(TRUE);

	lininit(strng,TRUE,printer);
	strncpy(&strng[5], title1, t_len1);
	if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
	else pr_write(strng);

	lininit(strng,TRUE,printer);
	strncpy(&strng[5],title2,t_len2);
	if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
	else pr_write(strng);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);

	feed(1,printer);
	pr_patline(printer,"Name:");
	pr_patline(printer,"ID  :");
	feed(2,printer);
}

// justify: 0= left justify, 1 = right justify
void insertconst(char *line, int columnstart, int justify, int lengthlimit, char *datastring){
	int columns, maxlimit;
	char *linePtr, datastring2[90];

	linePtr = line;
	columns = 0;
	while(*linePtr++ != '\r') columns++;

	// Only print if columnstart is within the width of the printer
	if(columnstart < columns){
		if(strlen(datastring) > 0){
			if(lengthlimit == 0){
				// Since no widths were given, calculate max limits
				if(justify) lengthlimit = columnstart + 1; // Right justify
				else lengthlimit = columns - columnstart; // Left justify
			}else{
				// Adjust lengthlimit when it is larger than columns
				if(justify) maxlimit = columnstart + 1;
				else maxlimit = columns - columnstart;
				if(lengthlimit > maxlimit) lengthlimit = maxlimit;
			}

			strcpy(datastring2, datastring);
			// Truncate string, if longer than limit
			if(strlen(datastring2) > lengthlimit) datastring2[lengthlimit] = 0;

			// Insert string into line
			if(justify) strncpy(&(line[columnstart - strlen(datastring2) + 1]), datastring2, strlen(datastring2)); // Right justify
			else strncpy(&(line[columnstart]), datastring2, strlen(datastring2)); // Left justify
		}
	}
}
/**
 * \details Insert a string into a print line
 * \param line Pointer to Null terminated print line
 * \param columnstart Zero based column number for inserting the string
 * \param justify 0 = Left justify, 1 = Right justify
 * \param lengthlimit Max width of the string to insert
 * \param datastring Pointer to Null terminated insert string
 * \returns None
 */
void insertconst3(char *line, int columnstart, int justify, int lengthlimit, char *datastring){
	int columns, maxlimit, parseindex, index;
	char *linePtr, *linePtrFront, *dataPtr, *controlPtr, datastring2[90];
	char controlstring[100], datastring3[100];

	linePtr = line;
	columns = 0;
	while(*linePtr++ != '\r') columns++;

	// Only print if columnstart is within the width of the printer
	if(columnstart < columns){
		// Parse out @ commands
		for(index=0; index<100; index++){
			controlstring[index] = 0;
			datastring3[index] = 0;
		}
		parseindex = 0;
		if(strlen(datastring) > 0){
			dataPtr = datastring;
			while(*dataPtr != 0){
				if(*dataPtr == '@'){
					dataPtr++;
					if(*dataPtr == 0){
						break;
					}else if((*dataPtr > '0') && (*dataPtr < '9')){
						controlstring[parseindex] = *dataPtr - '0';
					}
					dataPtr++;
				}else{
					datastring3[parseindex] = *dataPtr;
					dataPtr++;
					parseindex++;
				}
			}
		}

		if(strlen(datastring3) > 0){
			if(lengthlimit == 0){
				// Since no widths were given, calculate max limits
				if(justify) lengthlimit = columnstart + 1; // Right justify
				else lengthlimit = columns - columnstart; // Left justify
			}else{
				// Adjust lengthlimit when it is larger than columns
				if(justify) maxlimit = columnstart + 1;
				else maxlimit = columns - columnstart;
				if(lengthlimit > maxlimit) lengthlimit = maxlimit;
			}

			strcpy(datastring2, datastring3);
			// Truncate string, if longer than limit
			if(strlen(datastring2) > lengthlimit) datastring2[lengthlimit] = 0;

			// Insert string into line
			dataPtr = datastring2;
			controlPtr = controlstring;
			linePtr = line;
			if(justify) linePtr += (columnstart - strlen(datastring2) + 1);
			else linePtr += columnstart;
			linePtrFront = linePtr;
			while(*dataPtr != 0){
				if(linePtr == linePtrFront){
					if(*controlPtr != 0){
						*(linePtr + 100) = *controlPtr;
					}
				}else{
					*(linePtr + 100) = *controlPtr;
				}
				*linePtr = *dataPtr;
				linePtr++;
				dataPtr++;
				controlPtr++;
			}
			*(linePtr + 100) = *controlPtr;
		}
	}
}

void insertstring(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, char *datastring){
	char input[90];

	sprintf(input, formatstring, datastring);
	insertconst(line, columnstart, justify, lengthlimit, input);
}
/**
 * \details Insert a format string with a string placeholder into a print line
 * \param line Pointer to Null terminated print line
 * \param columnstart Zero based column number for inserting the string
 * \param justify 0 = Left justify, 1 = Right justify
 * \param lengthlimit Max width of the string to insert
 * \param formatstring Pointer to Null terminated format string
 * \param datastring Pointer to Null terminated string
 * \returns None
 */
void insertstring3(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, char *datastring){
	char input[100];

	sprintf(input, formatstring, datastring);
	insertconst3(line, columnstart, justify, lengthlimit, input);
}

void insertfloat(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, float datafloat){
	char input[90];

	sprintf(input, formatstring, datafloat);
	insertconst(line, columnstart, justify, lengthlimit, input);
}
/**
 * \details Insert a format string with a float placeholder into a print line
 * \param line Pointer to Null terminated print line
 * \param columnstart Zero based column number for inserting the string
 * \param justify 0 = Left justify, 1 = Right justify
 * \param lengthlimit Max width of the string to insert
 * \param formatstring Pointer to Null terminated format string
 * \param datafloat Float data number
 * \returns None
 */
void insertfloat3(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, float datafloat){
	char input[100];

	sprintf(input, formatstring, datafloat);
	insertconst3(line, columnstart, justify, lengthlimit, input);
}

void insertint(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, int dataint){
	char input[90];

	sprintf(input, formatstring, dataint);
	insertconst(line, columnstart, justify, lengthlimit, input);
}
/**
 * \details Insert a format string with a int placeholder into a print line
 * \param line Pointer to Null terminated print line
 * \param columnstart Zero based column number for inserting the string
 * \param justify 0 = Left justify, 1 = Right justify
 * \param lengthlimit Max width of the string to insert
 * \param formatstring Pointer to Null terminated format string
 * \param dataint Int data number
 * \returns None
 */
void insertint3(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, int dataint){
	char input[100];

	sprintf(input, formatstring, dataint);
	insertconst3(line, columnstart, justify, lengthlimit, input);
}

void inserttime_t(char *line, int columnstart, int justify, int lengthlimit, time_t datatime, bool dateonly){
	char input[90];

	if(dateonly) dateout(input, &datatime, 4);
	else GetExtendedTimeInfo(&datatime, input);
	trim(input);
	insertconst(line, columnstart, justify, lengthlimit, input);
}

void inserttime_t_language(char *line, int columnstart, int justify, int lengthlimit, time_t datatime, bool dateonly){
	char input[90];

	if(dateonly) dateout_language(input, &datatime, 4);
	else GetExtendedTimeInfoLanguage(&datatime, input);
	trim(input);
	insertconst(line, columnstart, justify, lengthlimit, input);
}
/**
 * \details Insert a date string into a print line in either English or French
 * \param line Pointer to Null terminated print line
 * \param columnstart Zero based column number for inserting the string
 * \param justify 0 = Left justify, 1 = Right justify
 * \param lengthlimit Max width of the string to insert
 * \param datatime Datetime value
 * \param dateonly True = Insert date string only, False = Insert date and time string
 * \returns None
 */
void inserttime_t3_language(char *line, int columnstart, int justify, int lengthlimit, time_t datatime, bool dateonly){
	char input[100];

	if(dateonly) dateout_language(input, &datatime, 4);
	else GetExtendedTimeInfoLanguage(&datatime, input);
	trim(input);
	insertconst3(line, columnstart, justify, lengthlimit, input);
}

void g_clear(short height, short leftpixel){
	short index, jndex;

	if(height>320) gheight = 320;
	else gheight = height;

	gleftpixel = leftpixel;
	for(jndex=0; jndex<320; jndex++){
		for(index=0; index<76; index++) gbuffer[jndex][index] = 0;
	}
}

void g_plot(short x, short y){
	short row, col;
	short shift;
	uchar mask;

	row = gheight - y - 1;
	col = x >> 3;
	shift = x % 8;
	mask = 0x80;
	mask = mask >> shift;
	gbuffer[row][col] = gbuffer[row][col] | mask;
}

void g_swap(short *a, short *b){
	short c;
	c = *a;
	*a = *b;
	*b = c;
}

void g_line(short x0, short y0, short x1, short y1, short dotted){
	bool steep;
	short doton;
	short deltax, deltay, error, ystep, x, y;

	doton = 0;

	if((x0 != x1) || (y0 != y1)){
		if(x0 == x1){
			if(y0 > y1) g_swap(&y0, &y1);
			for(y=y0; y<=y1; y++){
				if(!doton) g_plot(x0, y);

				if(dotted){
					if(!doton) doton = dotted;
					else doton--;
				}
			}
		}else if(y0 == y1){
			if(x0 > x1) g_swap(&x0, &x1);
			for(x=x0; x<=x1; x++){
				if(!doton) g_plot(x, y0);

				if(dotted){
					if(!doton) doton = dotted;
					else doton--;
				}
			}
		}else{
			if(abs(y1 - y0) > abs(x1 - x0)) steep = TRUE;
			else steep = FALSE;

			if(steep){
				g_swap(&x0, &y0);
				g_swap(&x1, &y1);
			}

			if(x0 > x1){
				g_swap(&x0, &x1);
				g_swap(&y0, &y1);
			}

			deltax = x1 - x0;
			deltay = abs(y1 - y0);
			error = deltax / 2;

			if(y0 < y1) ystep = 1;
			else ystep = -1;

			y = y0;
			for(x=x0; x<=x1; x++){
				if(!doton){
					if(steep) g_plot(y, x);
					else g_plot(x, y);
				}

				if(dotted){
					if(!doton) doton = dotted;
					else doton--;
				}

				error -= deltay;
				if(error<0){
					y += ystep;
					error += deltax;
				}
			}
		}
	}else{
		g_plot(x0, y0);
	}
}

void g_char(short x0, short y0, char chr, short fontsize){
	uchar ch_array[14];
	uchar ch_array2[14];
	uchar clearmask[2];
	uchar negmask;
	short bitsshifted, index;
	short xoffset, yoffset;

	get_ch_array(chr, fontsize, ch_array);

	clearmask[0] = clearmask[1] = 0xFF;
	clearmask[0] = clearmask[0] >> fonts[fontsize].x;

	bitsshifted = x0%8;

	if(bitsshifted > 0){
		negmask = 0xFF;
		negmask = negmask >> bitsshifted;
		negmask = ~negmask;
		clearmask[0] = clearmask[0] >> bitsshifted;
		clearmask[0] = clearmask[0] | negmask;
		if(bitsshifted > (8-fonts[fontsize].x)){
			clearmask[1] = clearmask[1] >> (bitsshifted + fonts[fontsize].x - 8);
			for(index=0; index<fonts[fontsize].y; index++){
				ch_array2[index] = ch_array[index] << (8 - bitsshifted);
				ch_array[index] = ch_array[index] >> bitsshifted;
			}
		}else{
			for(index=0; index<fonts[fontsize].y; index++){
				ch_array[index] = ch_array[index] >> bitsshifted;
			}
		}
	}

	xoffset = x0/8;
	yoffset = (gheight - 1) - y0;
	for(index=0; index<fonts[fontsize].y; index++){
		gbuffer[yoffset+index][xoffset] = gbuffer[yoffset+index][xoffset] & clearmask[0];
		gbuffer[yoffset+index][xoffset] = gbuffer[yoffset+index][xoffset] | ch_array[index];
		if(clearmask[1]!=0xFF){
			gbuffer[yoffset+index][xoffset+1] = gbuffer[yoffset+index][xoffset+1] & clearmask[1];
			gbuffer[yoffset+index][xoffset+1] = gbuffer[yoffset+index][xoffset+1] | ch_array2[index];
		}
	}
}

void g_string(short x0, short y0, char *printstring, short fontsize){
	while(*printstring != 0){
		g_char(x0, y0, *printstring++, fontsize);
		x0 += fonts[fontsize].delx;
	}
}

void g_setresolution_hp(char *res){
	//char res[4];
	char output[20];

	output[0] = 27;
	output[1] = 0;
	//ltoa(resolution, res, 10);
	strcat(output, "*t");
	strcat(output, res);
	strcat(output, "R");
	usb_write(output);
}

void g_setheight_hp(char *ht){
	//char ht[8];
	char output[20];

	output[0] = 27;
	output[1] = 0;
	//ltoa(height, ht, 10);
	strcat(output, "*r");
	strcat(output, ht);
	strcat(output, "T");
	usb_write(output);
}

void g_setwidth_hp(char *wd){
	//char wd[8];
	char output[20];

	output[0] = 27;
	output[1] = 0;
	//ltoa(width, wd, 10);
	strcat(output, "*r");
	strcat(output, wd);
	strcat(output, "S");
	usb_write(output);
}

void g_setcursorx_hp(char *xchr){
	//char xchr[8];
	char output[20];

	output[0] = 27;
	output[1] = 0;
	//ltoa(x, xchr, 10);
	strcat(output, "*p");
	strcat(output, xchr);
	strcat(output, "X");
	usb_write(output);
 }

void g_setcursory_hp(char *ychr){
	//char ychr[8];
	char output[20];

	output[0] = 27;
	output[1] = 0;
	//ltoa(y, ychr, 10);
	strcat(output, "*p");
	strcat(output, ychr);
	strcat(output, "Y");
	usb_write(output);
}

void g_setcompress_hp(char *modechr){
	//char modechr[3];
	char output[20];

	output[0] = 27;
	output[1] = 0;
	//ltoa(mode, modechr, 10);
	strcat(output, "&*b");
	strcat(output, modechr);
	strcat(output, "M");
	usb_write(output);
}

void g_start_hp(char *stx){
	//char stx[4];
	char output[20];

	output[0] = 27;
	output[1] = 0;
	//ltoa(start_x, stx, 10);
	strcat(output, "*r");
	strcat(output, stx);
	strcat(output, "A");
	usb_write(output);
}

void g_start_epsonusbraster(uchar resolution){
	uchar output[30];

	output[0] = 27;
	output[1] = '(';
	output[2] = 'G';
	output[3] = 1;
	output[4] = 0;
	output[5] = 1;
	usb_write_num((char *) output, 6);

	output[0] = 27;
	output[1] = '(';
	output[2] = 'U';
	output[3] = 1;
	output[4] = 0;
	output[5] = 20;
	usb_write_num((char *) output, 6);

	output[0] = 27;
	output[1] = '(';
	output[2] = 'e';
	output[3] = 2;
	output[4] = 0;
	output[5] = 0;
	output[6] = 0x10;
	usb_write_num((char *) output, 7);

	output[0] = 27;
	output[1] = '(';
	output[2] = 'D';
	output[3] = 4;
	output[4] = 0;
	output[5] = 0xa0;
	output[6] = 5;
	output[7] = 8;
	output[8] = 4;
	usb_write_num((char *) output, 9);

	//output[0] = 27;
	//output[1] = '+';
	//output[2] = resolution/10;
	//usb_write_num((char *) output, 3);
}

void g_data_hp(uchar *data, short bytecount, char *datachr){
	//char datachr[4];
	char output[107];
	short index;
	short length;

	output[0] = 27;
	output[1] = 0;
	//ltoa(bytecount, datachr);
	strcat(output, "*b");
	strcat(output, datachr);
	strcat(output, "W");
	length = strlen((const char *) output);

	for(index=length; index<length+bytecount; index++){
		output[index] = *data;
		data++;
	}
	usb_write_num(output, length+bytecount);
}

void g_stop_hp(void){
	char output[5];

	output[0] = 27;
	output[1] = 0;
	strcat(output, "*rC");
	usb_write(output);
}

void g_stop_epsonusbraster(void){
	char output[10];

	output[0] = 12;
	usb_write_num(output, 1);

	output[0] = 27;
	output[1] = '@';
	usb_write_num(output, 2);
}

void g_send_hp(void){
	short index;
	char output[3];

	g_setresolution_hp("100");
	g_setwidth_hp("608");
	g_setheight_hp("320");
	g_setcursorx_hp("100");
	//g_setcursory_hp("0");
	g_setcompress_hp("0");
	g_start_hp("1");
	for(index=0;index<gheight;index++){
		g_data_hp(&(gbuffer[index][0]), 76, "76");
	}
	g_stop_hp();

	output[0] = 0x0D;
	output[1] = 0x0A;
	output[2] = 0;

	usb_write(output);
}

void g_reset_epson(void){
	uchar output[2];

	output[0] = 27;
	output[1] = '@';
	uart_write_num((char *) output, 2, U_PR);
}

void g_linespacing_okidata(uchar dotsperline){
	uchar output[3];

	output[0] = 27;
	output[1] = 'A';
	output[2] = dotsperline;
	uart_write_num((char *) output, 3, U_PR);

	output[0] = 27;
	output[1] = '2';
	uart_write_num((char *) output, 2, U_PR);
}

void g_linespacing_epson(uchar dotsperline){
	uchar output[3];

	output[0] = 27;
	output[1] = '3';
	output[2] = dotsperline;
	uart_write_num((char *) output, 3, U_PR);
}

void g_linespacing_epsonusb(uchar dotsperline){
	uchar output[3];

	output[0] = 27;
	output[1] = '3';
	output[2] = dotsperline;
	//uart_write_num((char *) output, 3, U_PR);
	usb_write_num((char *) output, 3);
}

void g_linespacing_roll(uchar dotsperline){
	uchar output[3];
	output[0] = 27;
	output[1] = 0x33;
	output[2] = dotsperline;
	uart_write_num((char *) output, 3, U_PR);
}

void g_defaultlinespacing_roll(void){
	uchar output[2];

	output[0] = 27;
	output[1] = 0x32;
	uart_write_num((char *) output, 2, U_PR);
}

void g_data_okidata(ushort bytecount, char *datachr){
	uchar output[1000];
	short index;
	ushort bytestotal;

	bytestotal = bytecount + gleftpixel;

	output[0] = 27;
	output[1] = 'L';
	output[2] = (uchar) (bytestotal & 0xFF);
	output[3] = (uchar) ((bytestotal >> 8) & 0xFF);

	for(index=4; index<gleftpixel + 4; index++) output[index] = 0;

	for(index=gleftpixel + 4; index<gleftpixel + bytecount + 4; index++){
		output[index] = (uchar) (*datachr);
		datachr++;
	}

	output[gleftpixel + bytecount + 4]= 13;
	output[gleftpixel + bytecount + 5]= 10;
	uart_write_num((char *) output, gleftpixel + bytecount + 6, U_PR);
}

void g_data_epson(uchar mode, ushort bytecount, char *datachr){
	uchar output[1000];
	short index;
	ushort bytestotal;

	bytestotal = bytecount + gleftpixel;

	output[0] = 27;
	output[1] = '*';
	output[2] = mode;
	output[3] = (uchar) (bytestotal & 0xFF);
	output[4] = (uchar) ((bytestotal >> 8) & 0xFF);

	for(index=5; index<gleftpixel + 5; index++) output[index] = 0;

	for(index=gleftpixel + 5; index<gleftpixel + bytecount + 5; index++){
		output[index] = (uchar) (*datachr);
		datachr++;
	}

	output[gleftpixel + bytecount + 5]= 13;
	output[gleftpixel + bytecount + 6]= 10;
	uart_write_num((char *) output, gleftpixel + bytecount + 7, U_PR);
}

void g_data_epsonusb(uchar mode, ushort bytecount, char *datachr){
	uchar output[1000];
	short index;
	ushort bytestotal;

	bytestotal = bytecount + gleftpixel;

	bytestotal /= 6;

	output[0] = 27;
	output[1] = '*';
	output[2] = mode;
	output[3] = (uchar) (bytestotal & 0xFF);
	output[4] = (uchar) ((bytestotal >> 8) & 0xFF);

	for(index=5; index<gleftpixel + 5; index++) output[index] = 0;

	for(index=gleftpixel + 5; index<gleftpixel + bytecount + 5; index++){
		output[index] = (uchar) (*datachr);
		datachr++;
	}

	output[gleftpixel + bytecount + 5]= 13;
	output[gleftpixel + bytecount + 6]= 10;
	usb_write_num((char *) output, gleftpixel + bytecount + 7);
}

void g_data_epsonusbraster(char *datachr, ushort bytecount){
	uchar output[700];
	short index, jndex;
	ushort bytesent;
	uchar mask[8];

	mask[7] = 1;
	mask[6] = 2;
	mask[5] = 4;
	mask[4] = 8;
	mask[3] = 16;
	mask[2] = 32;
	mask[1] = 64;
	mask[0] = 128;

	bytesent = 8 * bytecount;

	//0.5" horizontal
    eps_horiz(0.5);

	output[0] = 27;
	output[1] = 'i';
	output[2] = 0;
	output[3] = 0;
	output[4] = 2;
	output[5] = (uchar) ((bytesent) & 0xFF);
	output[6] = (uchar) ((bytesent >> 8) & 0xFF);
	output[7] = 1;
	output[8] = 0;

	for(index=0; index<bytecount; index++){

		/*for(jndex=0; jndex<8; jndex++){
			if(mask[jndex] & (*datachr)){
				if(jndex%2){
					output[(4 * index) + (jndex/2)+ 9] |= 0x0F;
				}else{
					output[(4 * index) + (jndex/2)+ 9] |= 0xF0;
				}
			}else{
				if(jndex%2){
					output[(4 * index) + (jndex/2)+ 9] &= 0xF0;
				}else{
					output[(4 * index) + (jndex/2)+ 9] &= 0x0F;
				}
			}
		}*/

		for(jndex=0; jndex<8; jndex++){
			if(mask[jndex] & (*datachr)){
				output[(8*index) + jndex + 9] = 0xFF;
			}else{
				output[(8*index) + jndex + 9] = 0x00;
			}
		}
		datachr++;
	}

	output[bytesent + 9] = '\r';
	usb_write_num((char *) output, bytesent + 10);

	output[0] = 27;
	output[1] = '(';
	output[2] = 'v';
	output[3] = 2;
	output[4] = 0;
	output[5] = 1;
	output[6] = 0;
	usb_write_num((char *) output, 7);
}

void g_data_roll(uchar mode, ushort bytecount, char *datachr){
	uchar output[1000];
	short index;
	ushort bytestotal;

	bytestotal = bytecount;

	output[0] = 27;
	output[1] = 0x2A;
	output[2] = mode;
	output[3] = (uchar) (bytestotal & 0xFF);
	output[4] = (uchar) ((bytestotal >> 8) & 0xFF);

	//for(index=5; index<gleftpixel + 5; index++) output[index] = 0;

	for(index=5; index<bytecount + 5; index++){
		output[index] = (uchar) (*datachr);
		datachr++;
	}

	output[bytecount + 5]= 13;
	output[bytecount + 6]= 10;
	uart_write_num((char *) output, bytecount + 7, U_PR);
}

void g_send_roll(void){
	short rownum, jndex, index, colindex;
	uchar col[1000];

	//g_linespacing_roll(8);
	g_linespacing_roll(14);
	rownum = 40;

	for(jndex=0; jndex<rownum; jndex++){
		colindex = 0;
		for(index=gheight - 1; index>=0; index--){
			col[colindex++] = gbuffer[index][jndex];
		}
		g_data_roll(0, colindex, (char *) col);
	}
}

void g_send_epson(void){
	short index, jndex, kndex, colindex;
	short rownum;
	short bytenum;
	uchar col[1000];
	uchar mask, bytevalue;

	g_linespacing_epson(24);

	rownum = gheight;
	rownum /= 8;
	if((gheight%8)>0){
		rownum++;
	}
	bytenum = 608;
	bytenum /= 8;

	for(jndex=0; jndex<rownum; jndex++){
		colindex = 0;
		for(index=0; index<bytenum; index++){
			for(kndex=0; kndex<8; kndex++){
				mask = 0x80;
				mask = mask >> kndex;
				bytevalue = 0;
				if(((gbuffer[8*jndex + 0][index]) & mask) == mask) bytevalue += 128;
				if(((gbuffer[8*jndex + 1][index]) & mask) == mask) bytevalue += 64;
				if(((gbuffer[8*jndex + 2][index]) & mask) == mask) bytevalue += 32;
				if(((gbuffer[8*jndex + 3][index]) & mask) == mask) bytevalue += 16;
				if(((gbuffer[8*jndex + 4][index]) & mask) == mask) bytevalue += 8;
				if(((gbuffer[8*jndex + 5][index]) & mask) == mask) bytevalue += 4;
				if(((gbuffer[8*jndex + 6][index]) & mask) == mask) bytevalue += 2;
				if(((gbuffer[8*jndex + 7][index]) & mask) == mask) bytevalue += 1;
				col[colindex] = bytevalue;
				colindex++;
			}
		}
		g_data_epson(1, colindex, (char *) col);
	}
}

void g_send_epsonusb(void){
	short index, jndex, kndex, colindex;
	short rownum;
	short bytenum;
	uchar col[1000];
	uchar mask, bytevalue;

	//g_linespacing_epsonusb(24);

	rownum = gheight;
	rownum /= 8;
	if((gheight%8)>0){
		rownum++;
	}
	//bytenum = 608;
	bytenum = 576;
	bytenum /= 8;

	for(jndex=0; jndex<rownum; jndex++){
		colindex = 0;
		for(index=0; index<bytenum; index++){
			for(kndex=0; kndex<8; kndex++){
				mask = 0x80;
				mask = mask >> kndex;
				bytevalue = 0;
				if(((gbuffer[8*jndex + 0][index]) & mask) == mask) bytevalue += 128;
				if(((gbuffer[8*jndex + 1][index]) & mask) == mask) bytevalue += 64;
				if(((gbuffer[8*jndex + 2][index]) & mask) == mask) bytevalue += 32;
				if(((gbuffer[8*jndex + 3][index]) & mask) == mask) bytevalue += 16;
				if(((gbuffer[8*jndex + 4][index]) & mask) == mask) bytevalue += 8;
				if(((gbuffer[8*jndex + 5][index]) & mask) == mask) bytevalue += 4;
				if(((gbuffer[8*jndex + 6][index]) & mask) == mask) bytevalue += 2;
				if(((gbuffer[8*jndex + 7][index]) & mask) == mask) bytevalue += 1;
				col[colindex] = bytevalue;
				colindex++;
			}
		}
		g_data_epsonusb(64, colindex, (char *) col);
	}
}

void g_send_epsonusb_raster(void){
	short index ;

	g_start_epsonusbraster(40);

	for(index=0; index<gheight; index++){
		service_watchdog();
	    g_data_epsonusbraster((char *) &(gbuffer[index][0]), 76);
	    service_watchdog();
	    g_data_epsonusbraster((char *) &(gbuffer[index][0]), 76);
	}

	//g_stop_epsonusbraster();
	eps_reset(FALSE);

	eps_vert(FALSE,1.);
    eps_vert(FALSE,0.5);
}

void g_send_okidata(void){
	short index, jndex, kndex, colindex;
	short rownum;
	short bytenum;
	uchar col[1000];
	uchar mask, bytevalue;

	g_linespacing_okidata(8);

	rownum = gheight;
	rownum /= 8;
	if((gheight%8)>0){
		rownum++;
	}
	bytenum = 608;
	bytenum /= 8;

	for(jndex=0; jndex<rownum; jndex++){
		colindex = 0;
		for(index=0; index<bytenum; index++){
			for(kndex=0; kndex<8; kndex++){
				mask = 0x80;
				mask = mask >> kndex;
				bytevalue = 0;
				if(((gbuffer[8*jndex + 0][index]) & mask) == mask) bytevalue += 128;
				if(((gbuffer[8*jndex + 1][index]) & mask) == mask) bytevalue += 64;
				if(((gbuffer[8*jndex + 2][index]) & mask) == mask) bytevalue += 32;
				if(((gbuffer[8*jndex + 3][index]) & mask) == mask) bytevalue += 16;
				if(((gbuffer[8*jndex + 4][index]) & mask) == mask) bytevalue += 8;
				if(((gbuffer[8*jndex + 5][index]) & mask) == mask) bytevalue += 4;
				if(((gbuffer[8*jndex + 6][index]) & mask) == mask) bytevalue += 2;
				if(((gbuffer[8*jndex + 7][index]) & mask) == mask) bytevalue += 1;
				col[colindex] = bytevalue;
				colindex++;
			}
		}
		g_data_okidata(colindex, (char *) col);
	}
}

void graphics_test(void){
	char formfeed[2];

	g_clear(320, 0);

	g_line(0, 0, 607, 0, 0);
	g_line(0, 0, 0, 319, 0);
	g_line(0, 319, 607, 319, 0);
	g_line(607, 0, 607, 319, 0);
	g_line(304, 160, 304, 319, 0);
	g_line(304, 160, 380, 319, 0);
	g_line(304, 160, 456, 319, 0);
	g_line(304, 160, 532, 319, 0);
	g_line(304, 160, 607, 319, 0);
	g_line(304, 160, 607, 280, 0);
	g_line(304, 160, 607, 240, 0);
	g_line(304, 160, 607, 200, 0);
	g_line(304, 160, 607, 160, 0);
	g_line(304, 160, 607, 120, 0);
	g_line(304, 160, 607, 80, 0);
	g_line(304, 160, 607, 40, 0);
	g_line(304, 160, 607, 0, 0);
	g_line(304, 160, 532, 0, 0);
	g_line(304, 160, 456, 0, 0);
	g_line(304, 160, 380, 0, 0);
	g_line(304, 160, 304, 0, 0);
	g_line(304, 160, 228, 0, 0);
	g_line(304, 160, 152, 0, 0);
	g_line(304, 160, 76, 0, 0);
	g_line(304, 160, 0, 0, 0);
	g_line(304, 160, 0, 40, 0);
	g_line(304, 160, 0, 80, 0);
	g_line(304, 160, 0, 120, 0);
	g_line(304, 160, 0, 160, 0);
	g_line(304, 160, 0, 200, 0);
	g_line(304, 160, 0, 240, 0);
	g_line(304, 160, 0, 280, 0);
	g_line(304, 160, 0, 319, 0);
	g_line(304, 160, 76, 319, 0);
	g_line(304, 160, 152, 319, 0);
	g_line(304, 160, 228, 319, 0);

	g_string(20, 20, "Hello World", SMALL);
	g_string(50, 50, "Hello World", MEDIUM);
	g_string(80, 80, "Hello World", BIG);

	g_setcursory_hp("0");
	g_send_hp();

	//g_send_okidata();
	//g_send_epson();
	//g_send_roll();

	formfeed[0] = 12;
	formfeed[1] = 0;
	usb_write(formfeed);
	//uart_write_num(formfeed, 2, U_PR);
}

void g_resetlinespacing(char prtype){
	switch(prtype){
		case ROLL_PRINTER:
			//g_linespacing_roll(10);
			g_defaultlinespacing_roll();
			break;

		case OKI_PRINTER:
			g_linespacing_okidata(10);
			break;

		case LX_PRINTER:
			g_linespacing_epson(30);
			break;

		//case USB_EPS_PRINTER:
			//g_linespacing_epsonusb(30);
			//break;
	}
}

void g_print(char prtype, ulong *data, float startROIeV, float endROIeV, AUTOCAL *autocal){
	short xorigin, yorigin, index, yint0, yint1, height, leftpixel, jndex, kndex;
	long max_count_int;
	float max, factor, y0, y1, fheight, max_count;
	char max_count_string[20];
	//float energy, rawdata[256];
	float energy, fNumOfChannels;
	float *rawdata; // rawdata[4096]
	unsigned long int *display; // display[512]
	bool markROI;
	AUTOCAL *autoCal;
	unsigned long average;
	//float test[256];

	if(autocal == NULL) autoCal = Mca_getCurrentAutoCal();
	else autoCal = autocal;

	rawdata = (float *) malloc(4096 * sizeof(float));
	display = (unsigned long int *) malloc(512 * sizeof(unsigned long int));

	if((prtype == OKI_PRINTER) || (prtype == LX_PRINTER) || (prtype == ROLL_PRINTER) || (prtype == USB_PRINTER) || (prtype == USB_EPS_PRINTER)){
		if((startROIeV <= 0.0) || (endROIeV <= 0.0)) markROI = FALSE;
		else markROI = TRUE;

		Mca_loadUncorrectedSpectrum(data, autoCal, autoCal->num_of_channels);
		if(autoCal->num_of_channels < 1024){
			fNumOfChannels = autoCal->num_of_channels - 1;
			for(index=0; index<(autoCal->num_of_channels); index++){
				energy = index;
				energy *= 2000.0;
				energy /= fNumOfChannels;
				rawdata[index] = Mca_getCorrectedSpectra(energy);
			}
		}else{
			for(index=0; index<(autoCal->num_of_channels); index++) rawdata[index] = data[index];
		}

		if((prtype == OKI_PRINTER) || (prtype == LX_PRINTER) || (prtype == USB_PRINTER) || (prtype == USB_EPS_PRINTER)){
			switch(autoCal->num_of_channels){
				case 256:
					for(jndex=0; jndex<511; jndex++){
						if((jndex%2)== 0){
							display[jndex] = rawdata[jndex/2];
						}else{
							average = rawdata[(jndex-1)/2];
							average += rawdata[(jndex+1)/2];
							average >>= 1;
							display[jndex] = average;
						}
						display[511] = display[510];
					}
					break;

				case 512:
					for(jndex=0; jndex<512; jndex++) display[jndex] = rawdata[jndex];
					break;

				case 1024:
					for(jndex=0; jndex<512; jndex++){
						average = rawdata[2*jndex];
						average += rawdata[2*jndex + 1];
						average >>= 1;
						display[jndex] = average;
					}
					break;

				case 2048:
					for(jndex=0; jndex<512; jndex++){
						average = rawdata[4*jndex];
						for(kndex=1; kndex<4; kndex++) average+= rawdata[4*jndex + kndex];
						average >>= 2;
						display[jndex] = average;
					}
					break;

				case 4096:
					for(jndex=0; jndex<512; jndex++){
						average = rawdata[8*jndex];
						for(kndex=1; kndex<8; kndex++) average += rawdata[8*jndex + kndex];
						average >>= 3;
						display[jndex] = average;
					}
					break;
			}

			max = 0;
			for(index=0; index<512; index++){
				if(display[index] > max) max = display[index];
			}

			max_count = 500;
			if(max > max_count){
				do{
					max_count *= 2;
				}while(max > max_count);
			}

			switch(prtype){
				case OKI_PRINTER:
					height = 190;
					leftpixel = 110;
					break;

				case LX_PRINTER:
					height = 190;
					leftpixel = 120;
				break;

				case USB_EPS_PRINTER:
					height = 320;
					leftpixel = 0;
					break;

				case USB_PRINTER:
					height = 320;
					leftpixel = 0;
					break;
			}

			xorigin = 96;
			yorigin = 30;

			fheight = height - yorigin - 1;
			factor = fheight / max_count;

			g_clear(height, leftpixel);
			//g_line(0, 0, 607, 0, 0);
			//g_line(0, 0, 0, height - 1, 0);
			g_line(xorigin - 4, yorigin, xorigin + 511, yorigin, 0);
			g_line(xorigin, yorigin - 4, xorigin, height - 1, 0);

			for(index=1; index<512; index++){
				y0 = display[index - 1];
				y0 *= factor;
				yint0 = y0;
				y1 = display[index];
				y1 *= factor;
				yint1 = y1;
				g_line(index - 1 + xorigin, yint0 + yorigin, index + xorigin, yint1 + yorigin, 0);

				if(markROI){
					energy = index;
					energy *= 2000.0;
					energy /= 511.0;

					if((energy >= startROIeV) && (energy <= endROIeV)){
						g_line(index + xorigin, yorigin, index + xorigin, yint1 + yorigin, 0);
					}
				}
			}

			/*for(index=1; index<256; index++){
				y0 = rawdata[index - 1];
				y0 *= factor;
				yint0 = y0;
				y1 = rawdata[index];
				y1 *= factor;
				yint1 = y1;
				g_line(2 * (index-1) + xorigin, yint0 + yorigin, (2 * index) + xorigin, yint1 + yorigin, 0);

				if(markROI){
					energy = index;
					energy *= 2000.0;
					energy /= 255.0;

					if((energy >= startROIeV) && (energy <= endROIeV)){
						g_line((2 * index) + xorigin, yorigin, (2 * index) + xorigin, yint1 + yorigin, 0);
					}
				}
			}*/



			g_line(64 + xorigin, yorigin - 4, 64 + xorigin, yorigin, 0);
			g_line(64 + xorigin, yorigin, 64 + xorigin, height - 1, 2);   // 1/8

			g_line(127 + xorigin, yorigin - 4, 127 + xorigin, yorigin, 0);
			g_line(127 + xorigin, yorigin, 127 + xorigin, height - 1, 2); // 1/4

			g_line(191 + xorigin, yorigin - 4, 191 + xorigin, yorigin, 0);
			g_line(191 + xorigin, yorigin, 191 + xorigin, height - 1, 2); // 3/8

			g_line(255 + xorigin, yorigin - 4, 255 + xorigin, yorigin, 0);
			g_line(255 + xorigin, yorigin, 255 + xorigin, height - 1, 2); // 1/2

			g_line(319 + xorigin, yorigin - 4, 319 + xorigin, yorigin, 0);
			g_line(319 + xorigin, yorigin, 319 + xorigin, height - 1, 2); // 5/8

			g_line(382 + xorigin, yorigin - 4, 382 + xorigin, yorigin, 0);
			g_line(382 + xorigin, yorigin, 382 + xorigin, height - 1, 2); // 3/4

			g_line(446 + xorigin, yorigin - 4, 446 + xorigin, yorigin, 0);
			g_line(446 + xorigin, yorigin, 446 + xorigin, height - 1, 2); // 7/8

			g_line(510 + xorigin, yorigin - 4, 510 + xorigin, height - 1, 0); // 1

			g_line(xorigin - 4, ((height - yorigin - 1) / 4) + yorigin, xorigin, ((height - yorigin - 1) / 4) + yorigin, 0);
			g_line(xorigin, ((height - yorigin - 1) / 4) + yorigin, 510 + xorigin, ((height - yorigin - 1) / 4) + yorigin, 3);

			g_line(xorigin - 4, ((height - yorigin - 1)/2) + yorigin, xorigin, ((height - yorigin - 1)/2) + yorigin, 0);
			g_line(xorigin, ((height - yorigin - 1)/2) + yorigin, 510 + xorigin, ((height - yorigin - 1)/2) + yorigin, 3);

			g_line(xorigin - 4, ((height - yorigin - 1) * 3 / 4) + yorigin, xorigin, ((height - yorigin - 1) * 3 / 4) + yorigin, 0);
			g_line(xorigin, ((height - yorigin - 1) * 3 / 4) + yorigin, 510 + xorigin, ((height - yorigin - 1) * 3 / 4) + yorigin, 3);

			g_line(xorigin - 4, height - 1, 510 + xorigin, height - 1, 0);

			g_string(xorigin + 2, yorigin - 5, "0", SMALL);
			g_string(xorigin + 119, yorigin - 5, "500", SMALL);
			g_string(xorigin + 243, yorigin - 5, "1000", SMALL);
			g_string(xorigin + 243, yorigin - 14, "keV", BIG);
			g_string(xorigin + 370, yorigin - 5, "1500", SMALL);
			g_string(xorigin + 484, yorigin - 5, "2000", SMALL);

			g_string(xorigin - 70, ((height - yorigin - 1)/2) + yorigin + 9, "Counts", BIG);

			max_count_int = max_count;
			sprintf(max_count_string, "%ld", max_count_int);
			g_string(xorigin - 6 - (6 * (strlen(max_count_string))), height - 1, max_count_string, SMALL);

		}else if(prtype == ROLL_PRINTER){
			switch(autoCal->num_of_channels){
				case 256:
					for(jndex=0; jndex<256; jndex++) display[jndex] = rawdata[jndex];
					break;

				case 512:
					for(jndex=0; jndex<256; jndex++){
						average = rawdata[2*jndex];
						average += rawdata[2*jndex + 1];
						average >>= 1;
						display[jndex] = average;
					}
					break;

				case 1024:
					for(jndex=0; jndex<256; jndex++){
						average = rawdata[4*jndex];
						for(kndex=1; kndex<4; kndex++) average += rawdata[4*jndex + kndex];
						average >>= 2;
						display[jndex] = average;
					}
					break;

				case 2048:
					for(jndex=0; jndex<256; jndex++){
						average = rawdata[8*jndex];
						for(kndex=1; kndex<8; kndex++) average += rawdata[8*jndex + kndex];
						average >>= 3;
						display[jndex] = average;
					}
					break;

				case 4096:
					for(jndex=0; jndex<256; jndex++){
						average = rawdata[16*jndex];
						for(kndex=1; kndex<16; kndex++) average += rawdata[16*jndex + kndex];
						average >>= 4;
						display[jndex] = average;
					}
					break;
			}

			max = 0;
			for(index=0; index<256; index++){
				if(display[index] > max) max = display[index];
			}

			max_count = 500;
			if(max > max_count){
				do{
					max_count *= 2;
				}while(max > max_count);
			}

			height = 196;
			leftpixel = 0;
			xorigin = 56;
			yorigin = 22;

			fheight = height - yorigin - 1;
			factor = fheight / max_count;

			g_clear(height, leftpixel);
			//g_line(0, 0, 607, 0, 0);
			//g_line(0, 0, 0, height - 1, 0);
			g_line(xorigin-4, yorigin, xorigin + 255, yorigin, 0);
			g_line(xorigin, yorigin-4, xorigin, height - 1, 0);

			for(index=1; index<256; index++){
				y0 = display[index - 1];
				y0 *= factor;
				yint0 = y0;
				y1 = display[index];
				y1 *= factor;
				yint1 = y1;
				g_line(index - 1 + xorigin, yint0 + yorigin, index + xorigin, yint1 + yorigin, 0);

				if(markROI){
					energy = index;
					energy *= 2000.0;
					energy /= 255.0;

					if((energy >= startROIeV) && (energy <= endROIeV)){
						g_line(index + xorigin, yorigin , index + xorigin, yint1 + yorigin, 0);
					}
				}
			}

			g_line(32 + xorigin, yorigin - 4, 32 + xorigin, yorigin, 0);
			g_line(32 + xorigin, yorigin, 32 + xorigin, height - 1, 3);   // 1/8

			g_line(64 + xorigin, yorigin - 4, 64 + xorigin, yorigin, 0);
			g_line(64 + xorigin, yorigin, 64 + xorigin, height - 1, 3); // 1/4

			g_line(96 + xorigin, yorigin - 4, 96 + xorigin, yorigin, 0);
			g_line(96 + xorigin, yorigin, 96 + xorigin, height - 1, 3); // 3/8

			g_line(128 + xorigin, yorigin - 4, 128 + xorigin, yorigin, 0);
			g_line(128 + xorigin, yorigin, 128 + xorigin, height - 1, 3); // 1/2

			g_line(159 + xorigin, yorigin - 4, 159 + xorigin, yorigin, 0);
			g_line(159 + xorigin, yorigin, 159 + xorigin, height - 1, 3); // 5/8

			g_line(191 + xorigin, yorigin - 4, 191 + xorigin, yorigin, 0);
			g_line(191 + xorigin, yorigin, 191 + xorigin, height - 1, 3); // 3/4

			g_line(223 + xorigin, yorigin - 4, 223 + xorigin, yorigin, 0);
			g_line(223 + xorigin, yorigin, 223 + xorigin, height - 1, 3); // 7/8

			g_line(255 + xorigin, yorigin - 4, 255 + xorigin, height - 1, 0); // 1

			g_line(xorigin - 4, ((height - yorigin - 1) / 4) + yorigin, xorigin, ((height - yorigin - 1) / 4) + yorigin, 0);
			g_line(xorigin, ((height - yorigin - 1) / 4) + yorigin, 255 + xorigin, ((height - yorigin - 1) / 4) + yorigin, 3);

			g_line(xorigin - 4, ((height - yorigin - 1)/2) + yorigin, xorigin, ((height - yorigin - 1)/2) + yorigin, 0);
			g_line(xorigin, ((height - yorigin - 1)/2) + yorigin, 255 + xorigin, ((height - yorigin - 1)/2) + yorigin, 3);

			g_line(xorigin - 4, ((height - yorigin - 1) * 3 / 4) + yorigin, xorigin, ((height - yorigin - 1) * 3 / 4) + yorigin, 0);
			g_line(xorigin, ((height - yorigin - 1) * 3 / 4) + yorigin, 255 + xorigin, ((height - yorigin - 1) * 3 / 4) + yorigin, 3);

			g_line(xorigin - 4, height - 1, 255 + xorigin, height - 1, 0);

			g_string(xorigin, yorigin - 5, "0", SMALL);
			g_string(xorigin + 56, yorigin - 5, "500", SMALL);
			g_string(xorigin + 116, yorigin - 5, "1000", SMALL);
			g_string(xorigin + 120, yorigin - 14, "keV", SMALL);
			g_string(xorigin + 179, yorigin - 5, "1500", SMALL);
			g_string(xorigin + 232, yorigin - 5, "2000", SMALL);

			g_string(xorigin - 40, ((height - yorigin - 1)/2) + yorigin + 3, "Counts", SMALL);

			max_count_int = max_count;
			sprintf(max_count_string, "%ld", max_count_int);
			g_string(xorigin - 6 - (6 * (strlen(max_count_string))), height - 1, max_count_string, SMALL);
		}

		switch(prtype){
			case ROLL_PRINTER:
				g_send_roll();
				//g_linespacing_roll(10);
				g_defaultlinespacing_roll();
				break;

			case OKI_PRINTER:
				g_send_okidata();
				g_linespacing_okidata(10);
				break;

			case LX_PRINTER:
				g_send_epson();
				g_linespacing_epson(30);
				break;

			case USB_EPS_PRINTER:
				g_send_epsonusb_raster();
				break;

			case USB_PRINTER:
				g_send_hp();
				break;
		}
	}

	free(display);
	free(rawdata);
}

/**
 * \details Change a print line to embedded extended characters for a HP printer
 * \param strng Pointer to Null terminated print line
 * \returns None
 */
void convert_printing_usb(char *strng)
{
	int len;
	int i;
	uchar ch;
	char cstr[10];

	//select Roman 8 extended
	cstr[0] = ESC;
	cstr[1] = '(';
	cstr[2] = '8';
	cstr[3] = 'U';
	cstr[4] = '\0';
	usb_write(cstr);

	len = strlen(strng);

	for(i = 0; i < len; i++)
	{
		ch = strng[i];
		if(ch > 0x7f)
		{	
			switch (ch)
			{
			case 0xfc:	//ü
				strng[i] = 0xcf; 
				break;
			case 0xdc:	//Ü
				strng[i] = 0xdb;	
				break;
			case 0xf1:	//ñ
				strng[i] = 0xb7;	
				break;
			case 0xd1:	//Ñ
				strng[i] = 0xb6;	
				break;
			case 0xe9:	//é
				strng[i] = 0xc5;
				break;
			case 0xc9:	//É
				strng[i] = 0xdc;	
				break;
			case 0xe1:	//á
				strng[i] = 0xc4;	
				break;
			case 0xc1:	//Á  
				strng[i] = 0xe0;
				break;
			case 0xed:	//í
				strng[i] = 0xd5;	
				break;
			case 0xcd:	//Í  
				strng[i] = 0xe5;
				break;
			case 0xf3:	//ó
				strng[i] = 0xc6;
				break;
			case 0xd3:	//Ó  
				strng[i] = 0xe7;
				break;
			case 0xfa:  //ú
				strng[i] = 0xc7;
				break;
			case 0xda: 	//Ú  
				strng[i] = 0xed;
				break;
			case 0xe6:	//µ
				strng[i] = 0xf3;
				break;
			}	
					
		}
	}

}

static void convert_printing_oki(char *strng0,char *strng);
/**
 * \details Change a print line to embedded extended characters for a LX printer or OKI printer
 * \param strng0 Pointer to Null terminated print line
 * \param strng Pointer to char array, which receives the converted print line
 * \returns None
 */
void convert_printing_serial(char *strng0, char *strng)
{
	if(current.printer != OKI_PRINTER)
	{
		convert_printing_serial1(strng0);
		strcpy(strng,strng0);
		return;
	}

	convert_printing_oki(strng0,strng);
	
}	
/**
 * \details Change a print line to embedded extended characters for a LX printer
 * \param strng Pointer to Null terminated print line, modifications are made to the print line
 * \returns None
 */
static void convert_printing_serial1(char *strng)
{
	int len;
	int i;
	uchar ch;


	len = strlen(strng);

	for(i = 0; i < len; i++)
	{
		ch = strng[i];
		if(ch > 0x7f)
		{	
			switch (ch)
			{
			case 0xfc:	//ü
				strng[i] = 0x81;
				break;
			case 0xdc:	//Ü
				strng[i] = 0x9a;
				break;
			case 0xf1:	//ñ
				strng[i] = 0xa4;
				break;
			case 0xd1:	//Ñ
				strng[i] = 0xa5;
				break;
			case 0xe9:	//é
				strng[i] = 0x82;
				break;
			case 0xc9:	//É
				strng[i] = 0x90;
				break;
			case 0xe1:	//á
				strng[i] = 0xa0;
				break;
			case 0xc1:	//Á  
				strng[i] = 'A';
				break;
			case 0xed:	//í
				strng[i] = 0xa1;
				break;
			case 0xcd:	//Í  
				strng[i] = 'I';;
				break;
			case 0xf3:	//ó
				strng[i] = 0xa2;
				break;
			case 0xd3:	//Ó  
				strng[i] = 'O';
				break;
			case 0xfa:  //ú
				strng[i] = 0xa3;
				break;
			case 0xda: 	//Ú  
				strng[i] = 'U';
				break;
			}	
					
		}
	}

}	
/**
 * \details Change a print line to embedded extended characters for a OKI printer
 * \param strng0  Pointer to Null terminated print line
 * \param strng Pointer to Null terminated print line, modifications are made to the print line
 * \returns None
 */
static void convert_printing_oki(char *strng0,char *strng)
{
	int len;
	int i,j;
	uchar ch;


	len = strlen(strng0);

	j = -1;
	for(i = 0; i < len; i++)
	{
		ch = strng0[i];
		if(ch < 0x80)
			strng[++j] = ch;
		else
		{	
			switch (ch)
			{
			case 0xfc:	//ü
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0x81;
				break;
			case 0xdc:	//Ü
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0x9a;
				break;
			case 0xf1:	//ñ
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0xa4;
				break;
			case 0xd1:	//Ñ
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0xa5;
				break;
			case 0xe9:	//é
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0x82;
				break;
			case 0xc9:	//É
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0x90;
				break;
			case 0xe1:	//á
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0xa0;
				break;
			case 0xc1:	//Á  
				strng[++j] = 'A';
				break;
			case 0xed:	//í
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0xa1;
				break;
			case 0xcd:	//Í  
				strng[++j] = 'I';;
				break;
			case 0xf3:	//ó
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0xa2;
				break;
			case 0xd3:	//Ó  
				strng[++j] = 'O';
				break;
			case 0xfa:  //ú
				strng[++j] = ESC;
				strng[++j] = '^';
				strng[++j] = 0xa3;
				break;
			case 0xda: 	//Ú  
				strng[++j] = 'U';
				break;
			default:
				strng[++j] = ch; //need for µ
				break;
			}	
					
		}
	}
	strng[++j] = '\0';

}	

void prheadsec(char printer)
    {
        char sn[10];
        time_t nowtime;
        char strng[90];
        char dstr[12];
        short kp;
		char message[50];
		int len;

        switch(printer)
        {
        case NONE_PRINTER:
            return;

        case OKI_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            // 12 cpi, HSD
            strng[0] = ESC;   //10 cpi util
            strng[1] = 'I';
            strng[2] = '0';
            strng[3] = ESC;    //12 cpi
            strng[4] = 0x3a;
            strng[5] = '\r';
            strng[6] = '\0';
            uart_write(strng,U_PR);

            bold(TRUE);
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-TOUCH    RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&nowtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[68],dstr,8);
            //uart_write(strng,U_PR);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));

			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);

            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
            lininit(strng,TRUE,printer);
            ReadSN(sn);
            strncpy(strng,pr_crcname,strlen(pr_crcname));
            //strncpy(&strng[13],"REV       SN: ",14);
			get_amulet_message(L_REV_SN,message);    // "REV       SN: "
			strncpy(&strng[13],message,strlen(message));
            strncpy(&strng[17],rev_num,strlen(rev_num));
            strncpy(&strng[27],sn,10);
            pr_write(strng);
            prdatesec(printer);
            break;

        case LX_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            // 12 cpi
            eps_cpi(12);
            bold(TRUE);

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&nowtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[68],dstr,8);
            pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));

            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);

            break;

        case USB_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            //12 cpi
            pcl_cpi("12",FALSE);

            pcl_bold(TRUE);

            kp = 49;

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,8);
			pr_write(strng);

            pcl_bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));

			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            //return to 10cpi
            pcl_cpi("10",TRUE);
            break;

        case USB_EPS_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            eps_vert(TRUE,0.75);

            kp = 49;

            lininit(strng,FALSE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,8);
            eps_bold(0.5,strng);

            // serial number
            lininit(strng,TRUE,printer);
            strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[19],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            break;
        }
    }

/**
 * \details Print report header with either english or french
 * \param printer Printer type
 * \returns None
 */
void prhead_languagesec(char printer)
    {
        char sn[10];
        time_t nowtime;
        char strng[90];
        char dstr[12];
        short kp;
		char message[50];
		int len;

        switch(printer)
        {
        case NONE_PRINTER:
            return;

        case OKI_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            // 12 cpi, HSD
            strng[0] = ESC;   //10 cpi util
            strng[1] = 'I';
            strng[2] = '0';
            strng[3] = ESC;    //12 cpi
            strng[4] = 0x3a;
            strng[5] = '\r';
            strng[6] = '\0';
            uart_write(strng,U_PR);

            bold(TRUE);
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[68],dstr,8);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            //uart_write(strng,U_PR);
			pr_write(strng);

            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
        	read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            lininit(strng,TRUE,printer);
            ReadSN(sn);
            strncpy(strng,pr_crcname,strlen(pr_crcname));
            //strncpy(&strng[13],"REV       SN: ",14);
			get_amulet_message(L_REV_SN,message);    // "REV       SN: "
			strncpy(&strng[13],message,strlen(message));
            strncpy(&strng[17],rev_num,strlen(rev_num));
            strncpy(&strng[27],sn,10);
            pr_write(strng);

            lininit(strng,TRUE,printer);
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[0],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[13],dstr,8);
			pr_write(strng);
            break;

        case LX_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            // 12 cpi
            eps_cpi(12);
            bold(TRUE);

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[68],dstr,8);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            //strncpy(&strng[19],dstr,6);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            //uart_write(strng,U_PR);
			pr_write(strng);

            break;

        case USB_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            //12 cpi
            pcl_cpi("12",FALSE);

            pcl_bold(TRUE);

            kp = 49;

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,8);
			pr_write(strng);

            pcl_bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));
            ReadSN(dstr);
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
			pr_write(strng);

            //return to 10cpi
            pcl_cpi("10",TRUE);
            break;

        case USB_EPS_PRINTER:
            read_clock(&nowtime);
            clock_time = nowtime;
            low_clock_time = clock_time;
            reset_minute_counter_with_seconds();

            eps_vert(TRUE,0.75);

            kp = 49;

            lininit(strng,FALSE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&nowtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&nowtime);
            strncpy(&strng[kp + 13],dstr,8);
            eps_bold(0.5,strng);

            // serial number
            lininit(strng,TRUE,printer);
            strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[19],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            break;
        }
    }

/**
 * \details Print report header with test time
 * \param printer Printer type
 * \param testtime Test time
 * \returns None
 */
void prhead_testtimesec(char printer, time_t testtime){
	char sn[10];
	char strng[90];
	char dstr[12];
	short kp;
	char message[50];
	int len;

	switch(printer){
		case NONE_PRINTER:
			return;

        case OKI_PRINTER:
            // 12 cpi, HSD
            strng[0] = ESC;   //10 cpi util
            strng[1] = 'I';
            strng[2] = '0';
            strng[3] = ESC;    //12 cpi
            strng[4] = 0x3a;
            strng[5] = '\r';
            strng[6] = '\0';
            uart_write(strng,U_PR);

            bold(TRUE);
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&testtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&testtime);
            strncpy(&strng[68],dstr,8);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));

            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
            lininit(strng,TRUE,printer);
            ReadSN(sn);
            strncpy(strng,pr_crcname,strlen(pr_crcname));
            //strncpy(&strng[13],"REV       SN: ",14);
			get_amulet_message(L_REV_SN,message);    // "REV       SN: "
			strncpy(&strng[13],message,strlen(message));
            strncpy(&strng[17],rev_num,strlen(rev_num));
            strncpy(&strng[27],sn,10);
            pr_write(strng);
            prdate_measuresec(printer, &testtime);
            break;

        case LX_PRINTER:
            // 12 cpi
            eps_cpi(12);
            bold(TRUE);

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&testtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&testtime);
            strncpy(&strng[68],dstr,8);
            pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));

            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;

        case USB_PRINTER:
            //12 cpi
            pcl_cpi("12",FALSE);

            pcl_bold(TRUE);

            kp = 49;

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&testtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&testtime);
            strncpy(&strng[kp + 13],dstr,8);
			pr_write(strng);

            pcl_bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));

            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            //return to 10cpi
            pcl_cpi("10",TRUE);
            break;

        case USB_EPS_PRINTER:
            eps_vert(TRUE,0.75);
            kp = 49;
            lininit(strng,FALSE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout(dstr,&testtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&testtime);
            strncpy(&strng[kp + 13],dstr,8);
            eps_bold(0.5,strng);

            // serial number
            lininit(strng,TRUE,printer);
            strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[19],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;
        }
    }

/**
 * \details Print report header with test time in either English or French
 * \param printer Printer type
 * \param testtime Test time
 * \returns None
 */
void prhead_testtime_languagesec(char printer, time_t testtime){
	char sn[10];
	char strng[90];
	char dstr[12];
	short kp;
	char message[50];
	int len;

	switch(printer){
		case NONE_PRINTER:
			return;

        case OKI_PRINTER:
            // 12 cpi, HSD
            strng[0] = ESC;   //10 cpi util
            strng[1] = 'I';
            strng[2] = '0';
            strng[3] = ESC;    //12 cpi
            strng[4] = 0x3a;
            strng[5] = '\r';
            strng[6] = '\0';
            uart_write(strng,U_PR);

            bold(TRUE);
            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-TOUCH    RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&testtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&testtime);
            strncpy(&strng[68],dstr,8);
			pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));

			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            //strncpy(&strng[19],dstr,6);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;

        case ROLL_PRINTER:
        case SLIP_PRINTER:
            lininit(strng,TRUE,printer);
            ReadSN(sn);
            strncpy(strng,pr_crcname,strlen(pr_crcname));
            //strncpy(&strng[13],"REV       SN: ",14);
			get_amulet_message(L_REV_SN,message);    // "REV       SN: "
			strncpy(&strng[13],message,strlen(message));
            strncpy(&strng[17],rev_num,strlen(rev_num));
            strncpy(&strng[27],sn,10);
            pr_write(strng);
            prdate_measure_languagesec(printer, &testtime);
            break;

        case LX_PRINTER:
            // 12 cpi
            eps_cpi(12);
            bold(TRUE);

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&testtime,4);
            strncpy(&strng[55],dstr,11);
            timeoutsec(dstr,&testtime);
            strncpy(&strng[68],dstr,8);
            pr_write(strng);
            bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));

			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[58],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;

        case USB_PRINTER:
            //12 cpi
            pcl_cpi("12",FALSE);

            pcl_bold(TRUE);

            kp = 49;

            lininit(strng,TRUE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);
            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&testtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&testtime);
            strncpy(&strng[kp + 13],dstr,8);
            pr_write(strng);

            pcl_bold(FALSE);

            // serial number
            lininit(strng,TRUE,printer);
            //strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
			get_amulet_message(L_SN_MAIN_UNIT,message);    // "S/N Main Unit:"
			len = strlen(message);
			strncpy(&strng[3],message,len);
			get_amulet_message(L_PR_REV,message);    // "Rev:"
			strncpy(&strng[35],message,strlen(message));
            strncpy(&strng[40],rev_num,strlen(rev_num));

			//strncpy(&strng[8],message,strlen(message));
            //strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[4 + len],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);

            //return to 10cpi
            pcl_cpi("10",TRUE);
            break;

        case USB_EPS_PRINTER:
            eps_vert(TRUE,0.75);
            kp = 49;
            lininit(strng,FALSE,printer);
            //strncpy(&strng[0],"CRC-55t      RADIOISOTOPE DOSE CALIBRATOR",41);
            //strncpy(&strng[7],oki_crcname,4);

            strncpy(strng, pr_crcname_large, strlen(pr_crcname_large));
            dateout_language(dstr,&testtime,4);
            strncpy(&strng[kp],dstr,11);
            timeoutsec(dstr,&testtime);
            strncpy(&strng[kp + 13],dstr,8);
            eps_bold(0.5,strng);

            // serial number
            lininit(strng,TRUE,printer);
            strncpy(&strng[3],"S/N  Main Unit:               Rev:",34);
            strncpy(&strng[38],rev_num,strlen(rev_num));
            ReadSN(dstr);
            strncpy(&strng[19],dstr,10);
            strncpy(&strng[kp],"Capintec, Inc. USA",18);
            pr_write(strng);
            break;
        }
    }

/**
 * \details Print current date and time
 * \param printer Printer Type
 * \returns None
 */
    void prdatesec(short printer)
    {

        time_t nowtime;
        char strng[40];
        char str[12];

        if(printer == NONE_PRINTER)
            return;

        read_clock(&nowtime);
        clock_time = nowtime;
        low_clock_time = clock_time;
        reset_minute_counter_with_seconds();
        lininit(strng,FALSE,printer);
        dateout(str,&nowtime,4);
        strncpy(&strng[0],str,11);
        timeoutsec(str,&nowtime);
        strncpy(&strng[12],str,8);
        pr_write(strng);

    }

/**
 * \details Print current date and time in either English or French
 * \param printer Printer Type
 * \returns None
 */
    void prdate_languagesec(short printer)
    {

        time_t nowtime;
        char strng[40];
        char str[12];

        if(printer == NONE_PRINTER)
            return;

        read_clock(&nowtime);
        clock_time = nowtime;
        low_clock_time = clock_time;
        reset_minute_counter_with_seconds();
        lininit(strng,FALSE,printer);
        dateout_language(str,&nowtime,4);
        strncpy(&strng[0],str,11);
        timeoutsec(str,&nowtime);
        strncpy(&strng[12],str,8);
        pr_write(strng);

    }

/**
 * \details Print measurement time
 * \param printer Printer type
 * \param measuredon Pointer to time_t with the measurement time
 * \returns None
 */
    void prdate_measuresec(short printer, time_t *measuredon)
    {

        //time_t nowtime;
        char strng[40];
        char str[12];

        if(printer == NONE_PRINTER)
            return;

        //read_clock(&nowtime);
        //clock_time = nowtime;
        lininit(strng,FALSE,printer);
        dateout(str,measuredon,4);
        strncpy(&strng[0],str,11);
        timeoutsec(str,measuredon);
        strncpy(&strng[12],str,8);
        pr_write(strng);

    }

/**
 * \details Print measurement time in either English or French
 * \param printer Printer type
 * \param measuredon Pointer to time_t with the measurement time
 * \returns None
 */
    void prdate_measure_languagesec(short printer, time_t *measuredon)
    {

        //time_t nowtime;
        char strng[40];
        char str[12];

        if(printer == NONE_PRINTER)
            return;

        //read_clock(&nowtime);
        //clock_time = nowtime;
        lininit(strng,FALSE,printer);
        dateout_language(str,measuredon,4);
        strncpy(&strng[0],str,11);
        timeoutsec(str,measuredon);
        strncpy(&strng[12],str,8);
        pr_write(strng);

    }

/**
 * \details Print text with header
 * \param strng Pointer to Null terminated text string
 * \param header Pointer to Null terminated header string
 * \returns None
 */
    void pr_write2sec(char *strng, char *header){
    	char *strPtr;
    	char printer;
    	char margin;
		char strng1[200];

        if(current.printer == NONE_PRINTER)
            return;

        if(current.printer < 0)  //USB printers
		{
        	if(current.printer == USB_EPS_LABEL_PRINTER)
        		convert_printing_serial1(strng);
        	else
        		convert_printing_usb(strng);
            usb_write(strng);
		}
        else
		{
			convert_printing_serial(strng,strng1);
            uart_write(strng1,U_PR);
		}

        linecnt++;

        if(current.printer != ROLL_PRINTER){
        	if(linecnt > 53){
        		formfeed(current.printer);
        		if(strcmp(header, "DIAGNOSTICS") == 0){
        			prheadsec(current.printer);
        			feed(1, current.printer);
        		}else{
        			rawheadersec(current.printer, header, clock_time);
        		}
        		linecnt = 5;
        	}
        }
        strPtr = strng;
        while(*strPtr != '\r') *strPtr++ = ' ';
        strPtr += 3;
        printer = *strPtr++;
        margin = *strPtr;

        if(margin){
        	switch(printer){
				case OKI_PRINTER:
				case LX_PRINTER:
					pr_write("            ");
					break    ;
				case USB_PRINTER:
					pcl_horizontal("720");
					break;
				case USB_EPS_PRINTER:
					eps_horiz(0.5);
					break;
        	}
        }
    }

/**
 * \details Print text with header in either English or French
 * \param strng Pointer to Null terminated text string
 * \param header Pointer to Null terminated header string
 * \returns None
 */
    void pr_write2_languagesec(char *strng, char *header){
    	char *strPtr;
    	char printer;
    	char margin;
		char strng1[200];

        if(current.printer == NONE_PRINTER)
            return;


        if(current.printer < 0)  //USB printers
		{
        	if(current.printer == USB_EPS_LABEL_PRINTER)
        		convert_printing_serial1(strng);
        	else
        		convert_printing_usb(strng);
            usb_write(strng);
		}
        else
		{
			convert_printing_serial(strng,strng1);
            uart_write(strng1,U_PR);
		}


        linecnt++;

        if(current.printer != ROLL_PRINTER){
        	if(linecnt > 53){
        		formfeed(current.printer);
        		if(strcmp(header, "DIAGNOSTICS") == 0){
        			prhead_languagesec(current.printer);
        			feed(1, current.printer);
        		}else{
        			rawheader_languagesec(current.printer, header, clock_time);
        		}
        		linecnt = 5;
        	}
        }
        strPtr = strng;
        while(*strPtr != '\r') *strPtr++ = ' ';
        strPtr += 3;
        printer = *strPtr++;
        margin = *strPtr;

        if(margin){
        	switch(printer){
				case OKI_PRINTER:
				case LX_PRINTER:
					pr_write("            ");
					break    ;
				case USB_PRINTER:
					pcl_horizontal("720");
					break;
				case USB_EPS_PRINTER:
					eps_horiz(0.5);
					break;
        	}
        }
    }

    void pr_write2_language_dbsec(char *strng, char *header){
    	char *strPtr;
    	char printer;
    	char margin;
		char strng1[200];
		bool large;
		bool boldflag;

        if(current.printer == NONE_PRINTER)
            return;

        if(current.printer != ROLL_PRINTER){
        	if(linecnt > 53){
        		formfeed(current.printer);
        		rawheader_languagesec(current.printer, header, clock_time);

        		strPtr = strng;
        		while(*strPtr != '\r') strPtr++;
        		strPtr += 3;
        		printer = *strPtr++;
        		margin = *strPtr++;
        		large = *strPtr++;
        		boldflag = *strPtr;

				if(margin){
					switch(printer){
						case OKI_PRINTER:
						case LX_PRINTER:
							pr_write("            ");
							break;
						case USB_PRINTER:
							pcl_horizontal("720");
							break;
						case USB_EPS_PRINTER:
							eps_horiz(0.5);
							break;
					}
				}

				if(printer == USB_PRINTER){
					if(large){
						pcl_cpi("10", TRUE);
					}else{
						pcl_cpi("12", TRUE);
					}
				}

				if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER){
					if(boldflag) bold(TRUE);
					else bold(FALSE);
				}

        		linecnt = 5;
        	}
        }

        if(current.printer < 0)  //USB printers
		{
        	if(current.printer == USB_EPS_LABEL_PRINTER)
        		convert_printing_serial1(strng);
        	else
        		convert_printing_usb(strng);
            usb_write(strng);
		}
        else
		{
			convert_printing_serial(strng,strng1);
            uart_write(strng1,U_PR);
		}
        linecnt++;

        strPtr = strng;
        while(*strPtr != '\r') *strPtr++ = ' ';
        strPtr += 3;
        printer = *strPtr++;
        margin = *strPtr;

        if(margin){
        	switch(printer){
				case OKI_PRINTER:
				case LX_PRINTER:
					pr_write("            ");
					break;
				case USB_PRINTER:
					pcl_horizontal("720");
					break;
				case USB_EPS_PRINTER:
					eps_horiz(0.5);
					break;
        	}
        }
    }

void rawheadersec(char printer, char *title, time_t testtime){
	char strng[90];

	if(printer == USB_PRINTER)
	{
		pcl_height("12",FALSE);
		pcl_line_spacing("6",FALSE);
	}

	prhead_testtimesec(printer, testtime);
	feed(1,printer);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(TRUE);

	if(strlen(title) != 0)
	{
		lininit(strng, TRUE, printer);
		strncpy(&strng[5], title, strlen(title));
		if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);
	}
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	if(printer == USB_PRINTER)
		pcl_cpi("12",TRUE);
	feed(1,printer);
}

void rawheader_languagesec(char printer, char *title, time_t testtime){
	char strng[90];

	if(printer == USB_PRINTER)
	{
		pcl_height("12",FALSE);
		pcl_line_spacing("6",FALSE);
	}

	prhead_testtime_languagesec(printer, testtime);
	feed(1,printer);

	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(TRUE);

	if(strlen(title) != 0)
	{
		lininit(strng, TRUE, printer);
		strncpy(&strng[5], title, strlen(title));
		if(printer == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);
	}
	if(printer == OKI_PRINTER || printer == USB_PRINTER || printer == LX_PRINTER) bold(FALSE);
	if(printer == USB_PRINTER)
		pcl_cpi("12",TRUE);
	feed(1,printer);
}

void inserttime_tsec(char *line, int columnstart, int justify, int lengthlimit, time_t datatime, bool dateonly){
	char input[90];

	if(dateonly) dateout(input, &datatime, 4);
	else GetExtendedTimeInfoSec(&datatime, input);
	trim(input);
	insertconst(line, columnstart, justify, lengthlimit, input);
}
