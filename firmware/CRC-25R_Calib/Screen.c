/*********************************************************************
  MODULE:   SCREEN HANDLING

  FILE:     Screen.c

  DATE:      09/14/06


  *************************************************************************/

#include "coldfire.h"
#include "crc.h"
#include "screen.h"
#include "cs.h"
#include "pit.h"
#include "i2c.h"
#include "keyboard.h"
#include "uart.h"
#include "printer.h"
#include "remote.h"
#include <stdio.h>



//define screen setup values
#define SYSP1  0x30  //0x32
#define SYSP2  0x87     //8 dots/char, dual frame ac drive
#define SYSP3  0x07
#define SYSP4  0x20     //16 chars / row  x2
#define SYSP5  0xa4     //0x7c
#define SYSP6  0x3f
#define SYSP7  0x10     //16 bytes horiz 
#define SYSP8   0x00
#define PLANP1  0x00
#define PLANP2  0x00
#define PLANP3  0x3f
#define PLANP4  0x00
#define PLANP5  0x16
#define PLANP6  0x3f
#define PLANP7  0x00

#define PLANP8  0x00
#define PLANP9  0x00
#define PLANP0  0x00
#define OVLAYP  0x0c
#define PLANES  0x54
#define CSRP1   0x05
#define CSRP2   0x07

#define GROFFSET   0x16
#define HORIZ_BYTES 16
#define GRAPHICS_ADDRESS_OFFSET 0x1600
#define ADDRG 0x1600
#define SCREEN_WIDTH 128

//screen commands in array screen_commands[]
enum commands
{
    MREAD,
    MWRITE,
    WRITE_CURSOR,
    SYSSET,
    OVLAY,
    HRZSCR,
    DISOFF,
    DISON,
    CSRFRM,
    CSRDRT,
    ERASE_SCREEN,
    PLNSET,
    READCG,
    GRAYSCALE,
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    //array of screen commands
static const uchar screen_commands[] = 
{
    0x43,    //Memory Read
    0x42,    //Memory Write
    0x46,    //Write cursor
    0x40,    //System Set
    0x5b,    //overlay 
    0x5a,    //horizontal scroll
    0x58,    //display off    
    0x59,    //display on
    0x5d,    //cursor form
    0x4c,    //cursor direction right
    0x52,    //erase
    0x44,    //plane set
    0x55,    //read internal character generator
    0x60,   //set gray scale
};


enum onoff
{
    ON,
    OFF,
};


static short grplane;

static uchar get_bit(short xpos, bool flag);




	void initialize_screen(void)
	{
		uchar con;   //screen contrast

        write_screen_command(SYSSET);
		write_screen_data(SYSP1,FALSE);
		write_screen_data(SYSP2,FALSE);
		write_screen_data(SYSP3,FALSE);
		write_screen_data(SYSP4,FALSE);
		write_screen_data(SYSP5,FALSE);
		write_screen_data(SYSP6,FALSE);
		write_screen_data(SYSP7,FALSE);
		write_screen_data(SYSP8,FALSE);
		
		
		//plane addresses
		grplane = 0;
		screen_set();
		
		//overlay

        write_screen_command(OVLAY);
        
		write_screen_data(OVLAYP,FALSE);
		
		//horizontal scroll, shift 0
		write_screen_command(HRZSCR);
		write_screen_data(0,FALSE);

        //gray scale off (1bit per pixel)
        write_screen_command(GRAYSCALE);
        write_screen_data(0,FALSE);
        
		
		//display off, set cursor, enable planes
		write_screen_command(DISOFF);
		write_screen_data(PLANES,FALSE);
		//erase screen
		erase_screen();
		
		//set cursor form
		write_screen_command(CSRFRM);
		write_screen_data(CSRP1,FALSE);
		write_screen_data(CSRP2,FALSE);
		
		//release screen display inhibit
		write_screen_command(DISON);
		
		//set cursor direction right
		write_screen_command(CSRDRT);
		
		//erase again
		erase_screen();

        //initialize screen contrast
        EE_READ(contrast,&con);
        write_dac6(con);

        //initialize screen saver
        init_screen_saver();

		
						  
	}

	void erase_screen(void)
	{
        short i;
        short nbytes;
        
        set_cursor(0);
        
        //send WRITE command
        write_screen_command(MWRITE);
        
        //erase all memory
        nbytes = 32767;
        
        //write 00 to screen at each byte
        //cursor advances automatically
        for(i = 0; i < nbytes; i++)
            write_screen_data(0,FALSE);
        
	}
	
	void screen_set(void)
	{
		uchar data;
		
		write_screen_command(PLNSET);
		write_screen_data(PLANP1,FALSE);
		write_screen_data(PLANP2,FALSE);
		write_screen_data(PLANP3,FALSE);
		write_screen_data(PLANP4,FALSE);
		data = PLANP5;
		if (grplane == 1)
			data += GROFFSET;
		write_screen_data(data,FALSE);
		write_screen_data(PLANP6,FALSE);
		write_screen_data(PLANP7,FALSE);
		write_screen_data(PLANP8,FALSE);
		write_screen_data(PLANP9,FALSE);
		data = PLANP0;
		if (grplane == 1)
			data += GROFFSET;
		write_screen_data(data,FALSE);
		
	
	}
	
	
	
	void write_screen_command(uchar command)
	{
		uchar sc_command;
		
		//get command from array of commands
		sc_command = screen_commands[command];
	
		//write command to lcd
        write_lcd_cmd(sc_command);
			
	}
	

    void write_screen_data(uchar data, bool cmd_flag)
	{
		if(cmd_flag)
			write_screen_command(MWRITE);
		//write data to lcd
        write_lcd_data(data);
	
	}
	
	uchar read_screen_byte(bool cmd_flag)
	{

        
        if(cmd_flag)
            write_screen_command(MREAD);
		
		//read in the data and return it
        return (read_lcd_data());
	}
	
	
	
	void set_cursor(ushort address)
	{
		uchar wbyte;
		
		write_screen_command(WRITE_CURSOR);
		
		//write low byte of address
		wbyte = (uchar)(address & 0xff);
		write_screen_data(wbyte,FALSE);
		//write high byte of address
		wbyte = (uchar)(address >> 8);
		write_screen_data(wbyte,FALSE);
	}
	
	//draw or erase 1 pixel
	void draw_point(short xpos, short ypos, short plane, bool draw_flag)
	{
		uchar wr_byte;
		uchar current_byte;
		ushort addr;
		
		//move cursor to point
		addr = get_screen_address(xpos,ypos,plane);
		set_cursor(addr);
		
		//read current byte at that address (moves cursor)
		current_byte = read_screen_byte(TRUE);
		//form new byte using on bit for draw, off bit for erase
		if (draw_flag == DRAW)
		{
			wr_byte = get_bit(xpos,ON);
			wr_byte |= current_byte;
	
		}
		else //erase the point
		{
			wr_byte = get_bit(xpos,OFF);
			wr_byte &= current_byte;
		}
		
		//move cursor to point
		set_cursor(addr);
		//write new byte to screen
		write_screen_data(wr_byte,TRUE);
		
	}
	
	ushort get_screen_address(short xpos, short ypos, short plane)
	{
		ushort addr;
		
		addr = (ypos * HORIZ_BYTES)+ (xpos / 8);
		
		if (plane == 1)
		{
			addr += ADDRG;
			if (grplane == 1)
				addr += GRAPHICS_ADDRESS_OFFSET;
		}
	
		
		return (addr);
		
	}
	
	//draw or erase horizontal line
	void draw_horiz_line(short xpos0, short xpos1, short ypos, short plane,
		bool draw_flag)
	{
		short xpos;
		
		for(xpos = xpos0; xpos <= xpos1; xpos++)
			draw_point(xpos,ypos,plane,draw_flag);
	}
	
	//draw or erase vertical line
	
	void draw_vert_line(short xpos, short ypos0, short ypos1, short plane,
		bool draw_flag)
	{
		uchar wr_byte;
		uchar current_byte;
		ushort addr;
		short nlines;
		short i;
		uchar byte1;
		
		nlines = ypos1 - ypos0 + 1;
		
		//get on or off bit
		if (draw_flag == DRAW)
			byte1 = get_bit(xpos,ON);
		else
			byte1 = get_bit(xpos,OFF);
			
		//get starting address
		addr = get_screen_address(xpos,ypos0,plane);
		
		for (i = 0; i < nlines; i++)
		{
			set_cursor(addr);
			
			//read current byte at that address (moves cursor)
			current_byte = read_screen_byte(TRUE);
			
			//form new byte
			if(draw_flag == DRAW)
				wr_byte = byte1 | current_byte;
			else  //erase the pixel
				wr_byte = byte1 & current_byte;
				
			//move cursor to point
			set_cursor(addr);
			//write new byte to screen
			write_screen_data(wr_byte,TRUE);
			
			//increment addr by 1 scan line
			addr += HORIZ_BYTES;
				
		}   
	}
	
		
	
	void erase_box(short xpos0, short ypos0,
			short xpos1, short ypos1, short plane)
	{
        short nlines;
        short yy;
        
        //if erasing all across screen, call fast routine    
        if((xpos0 == 0) && (xpos1 == SCREEN_WIDTH - 1))
        {
            nlines = ypos1 - ypos0 + 1;
            erase_lines(ypos0,nlines,plane);
        }
        else
        {    
            for(yy = ypos0; yy <= ypos1; yy++)
            {    
                draw_horiz_line(xpos0,xpos1,yy,plane,ERASE_FLAG);
            }    
        }
        
	}
	
	
	//erase across screen from ypos for nlines scan lines
	void erase_lines(short ypos, short nlines, short plane)
	{
		ushort addr;
		short nbytes;
		int i;
		
		//move cursor to start
		addr = get_screen_address(0,ypos,plane);
		set_cursor(addr);
		
		//send WRITE command
		write_screen_command(MWRITE);
		
		//# of bytes to erase
		nbytes = HORIZ_BYTES * nlines;
	
		
		//write 00 to screen at each byte
		//cursor advances automatically
		for(i = 0; i < nbytes; i++)
			write_screen_data(0,FALSE);
		
		
	}
	
    static void empty_box(short xpos0, short ypos0, short xpos1, short ypos1,
                          short plane);
    static void filled_box(short xpos0, short ypos0, short xpos1, short ypos1,
                          short plane);
    static void solid_box(short xpos0, short ypos0, short xpos1, short ypos1,
                          short plane);
	
	
	//draw box -- empty or filled or solid
	void draw_box(short xpos0, short ypos0, short xpos1, short ypos1,
		short plane, short fill_flag)
	{
        switch(fill_flag)
        {
        case EMPTY_BOX:
            empty_box(xpos0,ypos0,xpos1,ypos1,plane);
            break;
        case FILLED_BOX:  //every other dot
            filled_box(xpos0,ypos0,xpos1,ypos1,plane);
            break;
        case SOLID_BOX:
            solid_box(xpos0,ypos0,xpos1,ypos1,plane);
            break;
        }    
            
	}

    //outline of box
    static void empty_box(short xpos0, short ypos0, short xpos1, short ypos1,
                          short plane)
    {    
            draw_horiz_line(xpos0,xpos1,ypos0,plane,DRAW);
            draw_horiz_line(xpos0,xpos1,ypos1,plane,DRAW);
            draw_vert_line(xpos0,ypos0,ypos1,plane,DRAW);
            draw_vert_line(xpos1,ypos0,ypos1,plane,DRAW);
    }
	
    //display every other pixel
    static void filled_box(short xpos0, short ypos0, short xpos1, short ypos1,
                          short plane)
    {    
        short yy,xpos,x0;

        for(yy = ypos0; yy <= ypos1; yy++)
        {
            if((yy % 2) == 0)
                x0 = xpos0;
            else
                x0 = xpos0 + 1;
            for(xpos = x0; xpos <= xpos1; xpos += 2)        
                draw_point(xpos,yy,plane,DRAW);
        }    
    }

    //solidly filled box
    static void solid_box(short xpos0, short ypos0, short xpos1, short ypos1,
                          short plane)
    {    
        short yy;

        for(yy = ypos0; yy <= ypos1; yy++)
            draw_horiz_line(xpos0,xpos1,yy,plane,DRAW);
    }



    static SCRMEM scrmem[2];    //1 structure for each plane
    void read_screen(short plane)
    {
        ushort addr;
        int i;
        
        addr = get_screen_address(0,0,plane);
        set_cursor(addr);

        write_screen_command(MREAD);

        for(i = 0; i < 1024; i++)
            scrmem[plane].byte[i] = read_screen_byte(FALSE);
        
    }

    void write_screen(short plane)
    {
        ushort addr;
        int i;

        addr = get_screen_address(0,0,plane);
        set_cursor(addr);

        write_screen_command(MWRITE);

        for(i = 0; i < 1024; i++)
            write_screen_data(scrmem[plane].byte[i],FALSE);
            
    }    
    
	static const uchar pon[] = 
	{
		0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01
	};
	static const uchar poff[] = 
	{
		0x7f,0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe
	};
	
	//get the bit mask corresponding to pixel for ON or OFF
	static uchar get_bit(short xpos, bool flag)
	{
		short nbit;
		uchar mask;
		
		nbit = xpos % 8;
		if (flag == ON)
			mask = pon[nbit];
		else
			mask = poff[nbit];
			
		return(mask);
	}
	
	
	static uchar dump_str[150];
    static void scr_space(uchar printer);
    static void set_scr_line(uchar printer);
    static void reset_space(uchar printer);
    static void screen_print_side(uchar printer);
    
void set_pc_freeze_flag(bool flag);
    void screen_print(short ystart,short yend)
    {

        short x,offset;
        short y,y0;
        uchar mask;
        uchar printer;
        uchar nbit;
        ushort addr;

        //send busy
        remote_busy(TRUE);
        busy_sent();
        set_pc_freeze_flag(TRUE);

        EE_READ(print[0],&printer);

        offset = 5;
        if (printer == OKI_PRINTER)
            offset = 4;

        //read the screen into scrmem
        read_screen(0);
        read_screen(1);

        scr_space(printer);

        set_scr_line(printer);

            
        //line on top
        for(x = 0; x < 130; x++)
            dump_str[x + offset] = 0x1;
        if(printer == OKI_PRINTER)
            dump_str[134] = '\r';
        dump_str[135] = '\n';
        
        // print the graphics line 
        uart_write_num((char *)dump_str,136,U_PR);

        screen_print_side(printer);
        
        for(y0 = ystart; y0 <= yend; y0 += 8)
        {
            service_pc();
            
            //line on left
            dump_str[offset] = 0xff;
            
            for(x = 0; x < 128; x++)
            {
                mask = 0x80;
                dump_str[x + offset + 1] = 0;
                nbit = x % 8;
                for(y = y0; y < y0 + 8; y++)
                {
                    addr = get_screen_address(x,y,0);
                    if ((scrmem[0].byte[addr] & pon[nbit]) != 0)
                        dump_str[x + offset + 1] |= mask;
                    if ((scrmem[1].byte[addr] & pon[nbit]) != 0)
                        dump_str[x + offset + 1] |= mask;
                    mask >>= 1;
                }
            }
            //line on right
            dump_str[129 + offset] = 0xff;
            
            if(printer == OKI_PRINTER)
                dump_str[134] = '\r';
            dump_str[135] = '\n';
            // print the graphics line 
            uart_write_num((char *)dump_str,136,U_PR);
        }
        screen_print_side(printer);
        
        //line on bottom
        for(x = 0; x < 130; x++)
            dump_str[x + offset] = 0x80;
        if(printer == OKI_PRINTER)
            dump_str[134] = '\r';
        dump_str[135] = '\n';
        // print the graphics line 
        uart_write_num((char *)dump_str,136,U_PR);

        reset_space(printer);

        //release busy
        remote_busy(FALSE);
        busy_sent();
        set_pc_freeze_flag(FALSE);
        

    }

    static void screen_print_side(uchar printer)
    {
        short x,offset;

        offset = 5;
        if (printer == OKI_PRINTER)
            offset = 4;
        

        //line on left
        dump_str[offset] = 0xff;
        
        for(x = 0; x < 128; x++)
        {
            dump_str[x + offset + 1] = 0;
        }
        //line on right
        dump_str[129 + offset] = 0xff;
        
        if(printer == OKI_PRINTER)
            dump_str[134] = '\r';
        dump_str[135] = '\n';
        // print the graphics line 
        uart_write_num((char *)dump_str,136,U_PR);

        
    }    
    
    static void scr_space(uchar printer)
    {

        dump_str[0] = ESC;

        switch(printer)
        {
        case OKI_PRINTER:
            /*begin unidirectional printing */
            dump_str[1] = 'U';
            dump_str[2] = '1';
            /* set height to 8/72" & then activate */
            dump_str[3] = ESC;
            dump_str[4] = 'A';
            dump_str[5] = 8;
            dump_str[6] = ESC;
            dump_str[7] = '2';
            dump_str[8] = '\n';
            uart_write_num((char *)dump_str,9,U_PR);
            break;
        case ROLL_PRINTER:
            /* begin unidirectional printing  */
            dump_str[1] = 'U';
            dump_str[2] = 1;
            dump_str[3] = '\n';
            uart_write_num((char *)dump_str,4,U_PR);
            /* change line spacing */
            dump_str[1] = '3';
            dump_str[2] = 16;  
            dump_str[3] = '\n';
            uart_write_num((char *)dump_str,4,U_PR);
           break;
        case SLIP_PRINTER:
            /* change line spacing */
            dump_str[1] = '3';
            dump_str[2] = 8;
            dump_str[3] = '\n';
            uart_write_num((char *)dump_str,4,U_PR);
            break;
        }
    }

    static void set_scr_line(uchar printer)
    {

        /*set up for each line, 130 dots */
        dump_str[0] = ESC;

        if(printer == OKI_PRINTER)
        {
            dump_str[1] = 'K';
            dump_str[2] = 130;
            dump_str[3] = 0;

        }

        else
        {
            dump_str[1] = '*';
            dump_str[2] = 0;
            dump_str[3] = 130; //128;
            dump_str[4] = 0;
        }
    }

    static void reset_space(uchar printer)
    {

        dump_str[0] = ESC;
        switch(printer)
        {
        case OKI_PRINTER:
            /* 1/6" = 12/72" & activate*/
            dump_str[1] = 'A';
            dump_str[2] = 12;
            dump_str[3] = ESC;
            dump_str[4] = '2';
            /*begin bidirectional printing */
            dump_str[5] = ESC;
            dump_str[6] = 'U';
            dump_str[7] = '0';
            dump_str[8] = '\n';
            uart_write_num((char *)dump_str,9,U_PR);
            break;
        case ROLL_PRINTER:
            /* reset line spacing */
            dump_str[1] = '2';
            /* turn off unidirectional printing */
            dump_str[2] = ESC;
            dump_str[3] = 'U';
            dump_str[4] = 0;
            dump_str[5] = '\n';
            uart_write_num((char *)dump_str,6,U_PR);
            break;
        case SLIP_PRINTER:
            /* reset line spacing */
            dump_str[1] = '2';
            dump_str[2] = '\n';
            uart_write_num((char *)dump_str,3,U_PR);
            break;
        }
    }

//routines for grplane

    void set_plane(short plane)
    {
        grplane = plane;
    }

    //reverse grplane
    void reverse_plane(void)
    {
        grplane = !grplane;
    }


    
//getting data from scrmem
    
    uchar get_scrmem(short plane, short addr)
    {
        uchar byt,byt0,byt1;

        byt0 = scrmem[0].byte[addr];
        byt1 = scrmem[1].byte[addr];

        switch(plane)
        {
        case 0:
            byt = byt0;
            break;
        case 1:
            byt = byt1;
            break;
        case -1:
            byt = byt0 | byt1;
            break;
        }

        return byt;

        
    }    

    
