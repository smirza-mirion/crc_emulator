/***********************************************************
  MODULE:  Display Text

  FILE:  DisplayText.c

  DATE;   07/08/04

  **********************************************************************/

#include "crc.h"
#include "screen.h"
#include <stdio.h>
#include <string.h>

static void write_char(short xpos0,short ypos0,short fontsize, 
					   short plane, short rflag);
static void write_byte(uchar val, short x, short y, short plane,
				uchar save,short rflag);
static void char_scan_byte(ushort addr,
					char ch_byte, char keep_byte);

extern FONT fonts[];
static uchar ch_array[140];
    void display_text(short xpos0, short ypos0, char *string,short plane,
                    short fontsize, short rflag)
    {
            
        short xpos;
        char ch;
    
        xpos = xpos0;
        while(*string != '\0')
        {
            ch = *string;
            get_ch_array(*string++,fontsize,ch_array);
            write_char(xpos,ypos0,fontsize,plane,rflag);
            if(rflag == REV)
                rev_outline(fonts[fontsize].x,fonts[fontsize].y,xpos,ypos0,plane,NORMAL); 
            xpos += fonts[fontsize].delx;
            
            //for Very Big font, adjust spacing for decimal point
            if (ch == '.' && fontsize == VBIG)
               xpos -= 8;
        }
    
    }
    
    
    void rev_outline(short n, short m, short xpos, short ypos, short plane, short rflag)
    {
        draw_horiz_line(xpos - 1,xpos + n, ypos - 1, plane ,rflag);
        draw_vert_line(xpos - 1,ypos - 1, ypos + m, plane, rflag);
        draw_horiz_line(xpos - 1,xpos + n,ypos + m,plane, rflag);
        draw_vert_line(xpos + n,ypos - 1,ypos + m,plane,rflag);
    
    }
    
    
    //un-reverse 1 character
    void unrev(short xpos, short ypos, char *chrev, short plane, short fontsize)
    {
        display_text(xpos, ypos, chrev,plane,fontsize,NORMAL);
        rev_outline(fonts[fontsize].x,fonts[fontsize].y,xpos,ypos,plane,REV);
    
    }
    
    
    static void write_char(short xpos0,short ypos0,
                    short fontsize, short plane,short rflag)
    {
        short j,k;
        short yp;
        short xp[8];
    
        for(j = 0; j < fonts[fontsize].nbytes; j++)
        {
            xp[j] = xpos0 + 8 * j;
        }
    
        k = -1;
        for(yp = ypos0; yp < ypos0 + fonts[fontsize].y; yp++ )
        {
            for(j = 0; j < fonts[fontsize].nbytes; j++)
            {
                ++k;
                write_byte(ch_array[k],xp[j],yp,plane,
                        fonts[fontsize].keep[j],rflag);
            }
        }
    }
    
    static void write_byte(uchar val0, short x, short y, short plane,
                    uchar save,short rflag)
    {
        ushort addr;
        short n,n1;
        uchar ebyte1,ebyte2,byte1,byte2;
        uchar sbyte1,sbyte2;
        uchar val;
    
        val = val0;
        if (rflag == REV)
            val = ~val;
    
        addr = get_screen_address(x,y,plane);
    
        n = x % 8;
        n1 = 8 - n;
        byte1 = val;
        sbyte1 = 0;
        sbyte2 = save;
        if(n != 0)
        {
            byte1 = val >> n;
            sbyte1 = 0xff << n1;
            if(save != 0)
                sbyte2 = save >> n;
        }
        ebyte1 = sbyte1 | sbyte2;
        char_scan_byte(addr,byte1,ebyte1);
    
        if(n != 0)
        {
            byte2 = val << n1;
            sbyte1 = 0xff >> n;
            sbyte2 = 0;
            if(save != 0)
                sbyte2 = save << n1;
            ebyte2 = sbyte1 | sbyte2;
            ++addr;
            char_scan_byte(addr,byte2,ebyte2);
        }
    
    }
    
        
    static void char_scan_byte(ushort addr,char ch_byte, 
                               char keep_byte)
    {
        uchar wr_byte;
        uchar current_byte;
    
        // move cursor to position 
          set_cursor(addr);
    
        // read current byte at that address (moves cursor) 
        current_byte = read_screen_byte(TRUE);
    
        /* part of byte to keep is current_byte & keep_byte
                then OR in ch_byte which is data to write */
        wr_byte = (current_byte & keep_byte) |  ch_byte;
    
    
        // move cursor to position 
        set_cursor(addr);
    
        // write new byte to screen 
        write_screen_data(wr_byte,TRUE);
    
    }
    
    //erase lines of text
    void erase_text_line(short ypos, short fontsize, short plane)
    {
        erase_lines(ypos, fonts[fontsize].y,plane);
    }
    
    //erase text of given length
    void erase_text(short xpos, short ypos,short plane,
                short length,short fontsize)
    {
        short xp1;
        short yp1;
    
        xp1 = xpos + length * fonts[fontsize].delx;
        yp1 = ypos + fonts[fontsize].dely;
    
        erase_box(xpos, ypos,xp1, yp1,plane);
    
    }
    
    //write special character
    void write_special(char *spchar,short xpos, short ypos, short fontsize, short plane)
    {
        short num;

        num = fonts[fontsize].nbytes * fonts[fontsize].dely;
        memcpy(ch_array,spchar,num);
        write_char(xpos,ypos,fontsize,plane,NORMAL);

    }    
