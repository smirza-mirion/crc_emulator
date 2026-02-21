/*********************************************************************
  MODULE:	FONTS FOR PRINTERS

  FILE:		PrinterFonts.c

  DATE:		01/24/07


  *************************************************************************/
#include "crc.h"
#include "printer.h"
#include "uart.h"

extern CURRENT current;
static void lx_bold(bool on);

/* ==========================================================================

   FONTS FOR EPSON ROLL AND SLIP PRINTERS

   ===========================================================================*/ 
    //5 x 7
    void font35(void)
	{
		char strng[4];

		strng[0] = ESC;
		strng[1] = '!';
		strng[2] = 0;
		if(current.printer < 0)
			usb_write_num(strng,3);
		else
        uart_write_num(strng,3,U_PR);
	}

    //5x7 double width
    void font17(void)	
	{
		char strng[4];

		strng[0] = ESC;
		strng[1] = '!';
		strng[2] = 32;
		if(current.printer < 0)
			usb_write_num(strng,3);
		else
        uart_write_num(strng,3,U_PR);
	}

    //7x7
	void font42(void)  
	{
		char strng[4];

		strng[0] = ESC;
		strng[1] = '!';
		strng[2] = 1;
		if(current.printer < 0)
			usb_write_num(strng,3);
		else
        uart_write_num(strng,3,U_PR);
	}

    //7x7 double width
    void font21(void)  
	{
		char strng[4];

		strng[0] = ESC;
		strng[1] = '!';
		strng[2] = 33;
		if(current.printer < 0)
			usb_write_num(strng,3);
		else
        uart_write_num(strng,3,U_PR);
	}


    //reset printer
    void pr_reset(void)
	{
		char strng[4];

		strng[0] = ESC;
		strng[1] = '@';
        strng[2] = '\0';
		if(current.printer < 0)
			usb_write_num(strng,3);
		else
        uart_write(strng,U_PR);

	}

    /* ==========================================================================

   FONTS FOR OKIDATA PRINTER

   ===========================================================================*/ 

    //font 6 enhanced
    void font6e(bool on)
    {
        char strng[12];
        
        if (on)
        {
             strng[0] = ESC;
             strng[1] = 0x3a;    //12 cp
             strng[2] = ESC;
             strng[3] = 'W';
             strng[4] = 1;      //double width
             strng[5] = ESC;
             strng[6] = 'E';    //emphasized
             strng[7] = ESC;
             strng[8] = 'G';       //enhanced
             uart_write_num(strng,9,U_PR);             

        }
        else
        {
             //emphsized & enhanced off
             strng[0] = ESC;
             strng[1] = 'F';
             strng[2] = ESC;
             strng[3] = 'H';
             strng[4] = ESC;
             strng[5] = 'W';
             strng[6] = 0;      //double width off
             uart_write_num(strng,7,U_PR);             

         }
    }

    //font 8 enhanced
    void font85e(bool on)
    {
        char strng[10];
        
        if (on)
        {
             strng[0] = 0x12;    //10 cpi
             strng[1] = 0x0f;    //compressed = 17.1 cpi
             strng[2] = ESC;
             strng[3] = 'W';
             strng[4] = 1;      //double width
             strng[5] = ESC;
             strng[6] = 'G';       //enhanced
             uart_write_num(strng,7,U_PR);             
         }
        else
        {
             //emphsized & enhanced off
             strng[0] = ESC;
             strng[1] = 'F';
             strng[2] = ESC;
             strng[3] = 'H';
             strng[4] = 0x12;    // end compressed 
             strng[5] = ESC;
             strng[6] = 'W';
             strng[7] = 0;      //double width off
             uart_write_num(strng,8,U_PR);             

        }
    }

    //near letter quality
    void fontnlq(bool on)
    {
        char strng[8];
        if (on)
        {
            // nlq courier - 12 cpi
            strng[0] = ESC;
            strng[1] = 0x3a;    //12 cpi
            strng[2] = ESC;
            strng[3] = 'I';
            strng[4] = 3;
            uart_write_num(strng,5,U_PR);             
        }
        else
        {
             // 12 cpi util -- note: manual says 10 cpi 
             strng[0] = ESC;
             strng[1] = 'I';
             strng[2] = 0;
             uart_write_num(strng,3,U_PR);             
        }
    }

    //17.1 cpi
    void font171(void)
    {
        char strng[4];

        strng[0] = 0x12;    //10 cpi
        strng[1] = 0x0f;    //compressed = 17.1 cpi
        strng[2] = '\0';
        uart_write(strng,U_PR);
    }

    extern CURRENT current;
    //print bold & or remove it
    void bold(bool on)
    {
        char strng[6];

        if(current.printer == LX_PRINTER)
        {
            lx_bold(on);
            return;
        }    

        if(current.printer == USB_PRINTER)
        {
            pcl_bold(on);
            return;
        }


        if(on)
        {    
            strng[0] = ESC;
            strng[1] = 'E';       //emphasized
            strng[2] = ESC;
            strng[3] = 'G';       //enhanced
        }
        else
        {
             strng[0] = ESC;
             strng[1] = 'F';
             strng[2] = ESC;
             strng[3] = 'H';

        }    
        strng[4] = '\0';
        uart_write(strng,U_PR);
    }

    //double width
    void eps_dbl(bool on)
    {

        char strng[4];

        strng[0] = ESC;
        strng[1] = 'W';
        strng[2] = '0';

        if(on)
            strng[2] ='1';  
        uart_write_num(strng,3,U_PR);


    }    


    static void lx_bold(bool on)  
    {
        char strng[4];

        strng[0] = ESC;
        strng[1] = '!';
        strng[2] = 1;

        if(on)
            strng[2] = 9;   //sets bit 3
        uart_write_num(strng,3,U_PR);
    }
