/*******************************************************************************
  MODULE:   EPSON ESCP language subs for USB connected to Epson printer
  
  FILE:     eps_subs.c
  
  DATE:     12/18/06
  *******************************************************************************/

#include "crc.h"
#include "printer.h"
#include "screen.h"
#include "keyboard.h"
#include "remote.h"
#include <string.h>

    //reset printer
    void eps_reset(bool p_flag)
    {
        char strng[40];
        short i;

        if(p_flag)
        {    
            //exit packet mode
            for(i = 0; i < 3; i++)
                strng[i] = 0;
            strng[3] = ESC;
            strng[4] = 1;
            strncpy(&strng[5],"@EJL 1284.4",11);
            strng[16] = '\n';
            strncpy(&strng[17],"@EJL     ",9);
            strng[26] = '\n';
            usb_write_num(strng,27);
        }

                      
        //initialize
        strng[0] = ESC;
        strng[1] = '@';
        strng[2] = '\0';
        usb_write(strng);


        //set unit to 1/180 inch
        strcpy(strng," (U   ");
        strng[0] = ESC;
        strng[3] = 1;
        strng[4] = 0;
        strng[5] = 20;
        usb_write_num(strng,6);

        //set to monochrome
        strcpy(strng," (K    ");
        strng[0] = ESC;
        strng[3] = 1;
        strng[4] = 0;
        strng[5] = 0;
        strng[6] = 1;
        usb_write_num(strng,7);


    }




    //print bold & or remove it
    void eps_bold(float horiz, char *str)
    {
        char strng[8];
        char str1[90];
        short len;

        //set starting horizontal position
        pr_write("\r");
        eps_horiz(horiz);

        //copy with no LF and with CR added
        strcpy(str1,str);
        len = strlen(str1);
        if(str1[len - 1] == '\n' || str1[len - 1] == '\r')
            str1[len - 1] = '\0';
        strcat(str1,"\r");
        usb_write(str1);
        
        //return to starting horizontal position
        eps_horiz(horiz);
        
        //move to bolding position
        strcpy(strng,"    ");
        strng[0] = ESC;
        strng[1] = 0x5c;
        strng[2] = 1;
        strng[3] = 0;
        usb_write_num(strng,4);

        strcpy(strng," (v   "); 
        strng[0] = ESC;
        strng[3] = 2;
        strng[4] = 0;
        strng[5] = 1;
        strng[6] = 0;
        usb_write_num(strng,7);

        //write original line
        usb_write(str);
            

    }


    //set characters per inch
/*    void eps_cpi(char *cpi_str)
 -     {
 -         char strng[10];
 -         
 -         strcpy(strng," X   ");
 -         strng[0] = ESC;
 - 
 -         if(strcmp(cpi_str,"10") == 0)
 -             strng[2] = 36;
 -         else    //12cpi
 -             strng[2] = 30;
 -         strng[3] = 0;
 -         strng[4] = 0;
 -        usb_write_num(strng,5);
 - 
 -     }*/
    void eps_cpi(short n)
    {

        char strng[4];
        strcpy(strng,"  ");
        strng[0] = ESC;
        switch(n)
        {        
        case 10:
            strng[1] = 'P';
            break;
        case 12:
            strng[1] = 'M';
            break;
        case 15:
            strng[1] = 'g';
            break;
        default:
            break;
        }    
        pr_write(strng);

    }

    void eps_compressed(bool on)
    {
        char strng[4];
        if(on)
            strng[0] = 0x0f;
        else
            strng[0] = 0x12;
        strng[1] = '\0';
        usb_write(strng);
    }    

    //change font
    void eps_font(short font)
    {
        char strng[10];
        
        strcpy(strng," k ");
        strng[0] = ESC;
        strng[2] = (char)font;
        usb_write_num(strng,3);
    }    
            

    //vertical position
    void eps_vert(bool abs_flag,float fin)
    {    
        char num;
        char strng[8];

        num = (char)(fin * 180);

        strcpy(strng," (v   ");   //relative
        if(abs_flag)   //absolute
            strng[2] = 'V';
        strng[0] = ESC;
        strng[3] = 2;
        strng[4] = 0;
        strng[5] = num;
        strng[6] = 0;
        usb_write_num(strng,7);
    }    


    //horizontal position
    void eps_horiz(float fin)
    {    

        char num_l,num_h;
        char strng[6];
        float fn;

        fn = fin * 180;
        if(fn <= 256)
        {    
            num_l = (char)fn;
            num_h = 0;
        }    
        else
        {    
            num_h = (char)(fn / 256);
            num_l = (char)((int)fn % 256);
        }
        strcpy(strng,"    ");
        strng[0] = ESC;
        strng[1] = 0x5c;
        strng[2] = num_l;
        strng[3] = num_h;
        usb_write_num(strng,4);
    }    


    //graph example
    static void graph_space(void);
    static void start_graph_line(char *strng);
    static void setup_graphics(void);
    static void side_lines(char *strng, short nbytes);

    static const uchar mask[8] ={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
    
    void screen_print_eps(void)
    {
        char strng[280];
        short i;
        short j;
        short k;
        short nbytes = 268;
        short y;
        uchar smem;

        //send busy
        remote_busy(TRUE);
        busy_sent();
        set_pc_freeze_flag(TRUE);
        

        //read the screen into scrmem
        read_screen(0);
        read_screen(1);
        
        setup_graphics();


        //line on top     
        start_graph_line(strng);
        for(i = 0; i < 258; i ++)
            strng[9 + i] = 0xff;
        strng[nbytes - 1] = '\r';
        usb_write_num(strng,nbytes);
        graph_space();

        //8 lines on sides
        for(j = 0; j < 8; j++)
            side_lines(strng,nbytes);

        //each screen line
        for(y = 0; y < 64; y++ )
        {

            service_pc();
            
            start_graph_line(strng);
            //dot on left and right, assume blank in middle
            strng[9] = 0xff;
            for(i = 0; i < 256; i ++)
                strng[10 + i] = 0x0;
            strng[nbytes - 2] = 0xff;
            strng[nbytes - 1] = '\r';

            //get each byte -- 1 graphics byte for each pixel bit  -- do each one twice
            for(i = 0; i < 16; i++)
            {
                smem = get_scrmem(-1,16 * y + i);
                for(j = 0; j < 8; j++)
                {
                    if((smem & mask[j]) != 0)
                    {
                        k = 16 * i + 2 * j;
                        strng[10 + k] = 0xff;
                        ++k;
                        strng[10 + k] = 0xff;

                    }   

                }    
                    
            }   
            usb_write_num(strng,nbytes);
/*            if(bytes_remaining() != 0)
 -                 return;*/
            graph_space();
            //print 3 more times for correct proportions
            for(k = 0; k < 3; k++)
            {    
                eps_horiz(0.5);
                usb_write_num(strng,nbytes);
/*                if(bytes_remaining() != 0)
 -                     return;*/
                graph_space();
            }    

        }

        //8 lines on sides
        for(j = 0; j < 8; j++)
            side_lines(strng,nbytes);


        //line on bottom     
        start_graph_line(strng);
        for(i = 0; i < 258; i ++)
            strng[9 + i] = 0xff;
        strng[nbytes - 1] = '\r';
        usb_write_num(strng,nbytes);
        graph_space();

        //reset for text
        eps_reset(FALSE);

        //move beyond end of graph
        eps_vert(FALSE,1.);
        eps_vert(FALSE,0.5);

        //release busy
        remote_busy(FALSE);
        busy_sent();
        set_pc_freeze_flag(FALSE);
        


    }    
        
            
    static void graph_space(void)
    {

        char strng[10];
        
        strcpy(strng," (v  ");
        strng[0] = ESC;
        strng[3] = 2;
        strng[4] = 0;
        strng[5] = 1;
        strng[6] = 0;
        usb_write_num(strng,7);
        

    }   


    static void start_graph_line(char *strng)
    {

        //0.5" horizontal
        eps_horiz(0.5);

        strcpy(strng," i");
        strng[0] = ESC;
        strng[2] = 0;
        strng[3] = 0;
        strng[4] = 2;
        strng[5] = 0x2;  //256 + 2 = 258
        strng[6] = 1;
        strng[7] = 1;
        strng[8] = 0;

    }   

    

    static void setup_graphics(void)
    {

        char strng[10];

        //graphics mode
        strcpy(strng," (G   ");
        strng[0] = ESC;
        strng[3] = 1;
        strng[4] = 0;
        strng[5] = 1;
        usb_write_num(strng,6);

        //set unit to 1/180 inch
        strcpy(strng," (U   ");
        strng[0] = ESC;
        strng[3] = 1;
        strng[4] = 0;
        strng[5] = 20;
        usb_write_num(strng,6);
        
        //select dot size: variable1
        strcpy(strng," (e   ");
        strng[0] = ESC;
        strng[3] = 2;
        strng[4] = 0;
        strng[5] = 0;
        strng[6] = 0x10;
        usb_write_num(strng,7);

        
        //set resolution of Raster mode (180 x 360 dpi)
        strcpy(strng," (D  ");
        strng[0] = ESC;
        strng[3] = 4;
        strng[4] = 0;
        strng[5] = 0xa0;
        strng[6] = 5;
        strng[7] = 8;
        strng[8] = 4;
        usb_write_num(strng,9);



    }   

    static void side_lines(char *strng, short nbytes)
    {

        short i;
        
        start_graph_line(strng);
        strng[9] = 0xff;
        for(i = 0; i < 256; i ++)
            strng[10 + i] = 0x0;
        strng[nbytes - 2] = 0xff;
        strng[nbytes - 1] = '\r';
        usb_write_num(strng,nbytes);
        graph_space();

    }   
