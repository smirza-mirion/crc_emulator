/*******************************************************************************
  MODULE:   HP PCL language subs for USB connected to HP printer
  
  FILE:     pcl_subs.c
  
  DATE:     09/14/06
  *******************************************************************************/

#include "crc.h"
#include "printer.h"
#include "keyboard.h"
#include "remote.h"
#include <string.h>



    void pcl_reset(void)
    {
    
        char strng[4];
    
        strng[0] = ESC;
        strng[1] = 'E';
        strng[2] = '\0';
        usb_write(strng);

        pcl_symbol("10U");
             
        pcl_cpi("10",TRUE);
        
    
    }    


    //print bold & or remove it
    void pcl_bold(bool on)
    {
        char strng[6];

        strcpy(strng," (s B");
        strng[0] = ESC;

        if(on)
            strng[3] = '3';
        else
            strng[3] = '0';

        usb_write(strng);
    }


	static char cpi_save[8];
    //set characters per inch
    void pcl_cpi(char *cpi_str,bool save)
    {
        char strng[10];
		char c_str[8];

		if(save)
			strcpy(cpi_save,cpi_str);

		if(strlen(cpi_str) == 0)
			strcpy(c_str,cpi_save);
		else
			strcpy(c_str,cpi_str);
        strcpy(strng," (s");
        strng[0] = ESC;
        //strcat(strng,cpi_str);
        strcat(strng,c_str);
        strcat(strng,"H");
        usb_write(strng);

    }    

    //expanded font
    void pcl_expanded(void)
    {
        char strng[10];
        
        strcpy(strng," (s24S");
        strng[0] = ESC;
        usb_write(strng);
        


    }

	static char height_save[8];
    //set height in points (1/72")
    void pcl_height(char *ht_str,bool save)
    {
        char strng[10];
        
		char h_str[8];

		if(save)
			strcpy(height_save,ht_str);

		if(strlen(ht_str) == 0)
			strcpy(h_str,height_save);
		else
			strcpy(h_str,ht_str);
        strcpy(strng," (s");
        strng[0] = ESC;
        //strcat(strng,ht_str);
        strcat(strng,h_str);
        strcat(strng,"V");
        usb_write(strng);

    }    

    
    //vertical cursor position:  with sign is relative, no sign is absolute from top of page
    //unit is dots (1/300")
    void pcl_vertcial(char *vt_str)
    {

        char strng[10];
        
        strcpy(strng," *p");
        strng[0] = ESC;
        strcat(strng,vt_str);
        strcat(strng,"Y");
        usb_write(strng);

    }    

    
    //horizontal cursor position:  with sign is relative, no sign is absolute from left edge of page
    //unit is decipoints (1/720")
    void pcl_horizontal(char *hz_str)
    {

        char strng[10];
        
        strcpy(strng," &a");
        strng[0] = ESC;
        strcat(strng,hz_str);
        strcat(strng,"H");
        usb_write(strng);

    }    
    //select symbol set    
    void pcl_symbol(char *sym_str)
    {

        char strng[10];
        
        strcpy(strng," (");
        strng[0] = ESC;
        strcat(strng,sym_str);
        usb_write(strng);

    }    

	//line spacing
	static char spacing_save[8];
	void pcl_line_spacing(char *ls_str,bool save)
	{
		char strng[10];
		char l_str[8];

		if(save)
			strcpy(spacing_save,ls_str);

		if(strlen(ls_str) == 0)
			strcpy(l_str,spacing_save);
		else
			strcpy(l_str,ls_str);
		
        strcpy(strng," &l");
        strng[0] = ESC;
        //strcat(strng,ls_str);
        strcat(strng,l_str);
        strcat(strng,"D");
        usb_write(strng);
    }

#include "screen.h"    
    void screen_print_pcl(void)
    {

        short offset;
        short y;
        short i,j,k;
        char str[50];
        uchar byte1,byte2;
        char scr;

        //send busy
        remote_busy(TRUE);
        busy_sent();
        set_pc_freeze_flag(TRUE);

        offset = 6;
        
        //setup for pcl graphics printing
        //move starting x to the right 1 inch
        pcl_horizontal("720");
        //75 dpm
        strcpy(str," *t75R");
        str[0] = ESC;
        usb_write(str);
        //logical presentation
        strcpy(str," *r0F");
        str[0] = ESC;
        usb_write(str);
        //start at current x position
        strcpy(str," *r1A");
        str[0] = ESC;
        usb_write(str);

        //read the screen into scrmem
        read_screen(0);
        read_screen(1);

            
        //line on top
        strcpy(str," *b34W");
        str[0] = ESC;
        str[offset] = 0x01;
        for(i = 0; i < 32; i++)
            str[offset + i + 1] = 0xff;
        str[offset + 33] = 0x80;
        usb_write_num(str,40);

        //8 side lines
        for(j = 0; j < 8; j++)
        {    
            strcpy(str," *b34W");
            str[0] = ESC;
            str[offset] = 0x01;
            for(i = 0; i < 32; i++)
                str[offset + i + 1] = 0;
            str[offset + 33] = 0x80;
            usb_write_num(str,40);
        }
        
        
        //print screen with double width and double length
        for(y = 0; y < 64; y++ )
        {

            service_pc();
            
            strcpy(str," *b34W");
            str[0] = ESC;

            //dot on left
            str[offset] = 0x01;
            
            for(i = 0; i < 16; i++)
            {
                j = 16 * y + i;
                //str[i + offset + 1] = get_scrmem(-1,j);
                byte1 = byte2 = 0;
                scr = get_scrmem(-1,j);
                if((scr & 0x80) != 0)
                        byte1 |= 0xc0;
                if((scr & 0x40) != 0)
                        byte1 |= 0x30;
                if((scr & 0x20) != 0)
                        byte1 |= 0x0c;
                if((scr & 0x10) != 0)
                        byte1 |= 0x03;
                if((scr & 0x08) != 0)
                        byte2 |= 0xc0;
                if((scr & 0x04) != 0)
                        byte2 |= 0x30;
                if((scr & 0x02) != 0)
                        byte2 |= 0x0c;
                if((scr & 0x01) != 0)
                        byte2 |= 0x03;
                k = 2 * i + offset +1;
                str[k] = byte1;
                str[k + 1] = byte2;
                
            }
            //dot on right
            str[offset + 33] = 0x80;
            
            // print the graphics line 
            usb_write_num(str,40);
/*            if(bytes_remaining() != 0)
 -                 return;*/
            //print again for double length
            usb_write_num(str,40);
/*            if(bytes_remaining() != 0)
 -                 return;*/
        }
        //8 side lines
        for(j = 0; j < 8; j++)
        {    
            strcpy(str," *b34W");
            str[0] = ESC;
            str[offset] = 0x01;
            for(i = 0; i < 32; i++)
                str[offset + i + 1] = 0;
            str[offset + 33] = 0x80;
            usb_write_num(str,40);
        }
        
        //line on bottom
        strcpy(str," *b34W");
        str[0] = ESC;
        str[offset] = 0x01;
        for(i = 0; i < 32; i++)
            str[offset + i + 1] = 0xff;
        str[offset + 33] = 0x80;
        usb_write_num(str,40);
        

        //end graphics mode
        strcpy(str," *rc");
        str[0] = ESC;
        usb_write(str);
       
        pcl_symbol("10U");
        pcl_cpi("10",TRUE);

        //line spacing
        str[0] = '\n';
        str[1] = '\n';
        str[2] = '\0';
        usb_write(str);

        //release busy
        remote_busy(FALSE);
        busy_sent();
        set_pc_freeze_flag(FALSE);
        

    }


