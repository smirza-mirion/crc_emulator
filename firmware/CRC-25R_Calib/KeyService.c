/*********************************************************************
  MODULE:	KEY SERVICE

  FILE:		KeyService.c

  DATE:		08/28/05

  *************************************************************************/
#include "crc.h"
#include "keyboard.h"
#include "screen.h"
#include "message.h"
#include <string.h>
#include <stdlib.h>


    static short maxpos;    //max key strokes allowed  
    static char keybuf[20];
    static short   keypos;
    static short idnum;
    static bool input_time;

    void dokey(short num)
	{
        char buf[4];

		if(keypos == maxpos)
		{
			beep();
			return;
		}


		//set keybuf[keypos] to entered character
        if(num == 10)
            keybuf[keypos] = '/';
        else
            keybuf[keypos] = (char)num + '0';

		//if inputting time, display the entered character
        if(input_time)
		{
            buf[0] = keybuf[keypos];
            buf[1] = '\0';
            display_text(8 * keypos,Y_FUT_PRMT,buf,0,MEDIUM,NORMAL);            
		}
		
        //increment key position
        ++keypos;
	}




void prompt_date_time(void)
{

    short i;
    
    erase_lines(Y_FUT_TIME,64 - Y_FUT_TIME,0);

    //display_text(12,Y_FUT_TIME,"hhmm/DD/MM/YY",0,SMALL,NORMAL);
    display_small_message(DATE_TIME_IN_8,Y_FUT_TIME,0,NORMAL);
    //set variables for inputting time
    keypos = 0;
    input_time = TRUE;
    maxpos = 14;
    
    for(i = 0; i < 15; i++)
        keybuf[i] = '\0';


}



    void keybuf_bsp(void)
    {
        char buf[2];
        

        if(keypos == 0)
        {
            beep();
            return;
        }
        
        --keypos;
        keybuf[keypos] = '\0';
        
        if(input_time)
        {
            buf[0] = ' ';
            buf[1] = '\0';
            display_text(8 * keypos,Y_FUT_PRMT,buf,0,MEDIUM,NORMAL);            
        }
    }
    

    
    //copy part of keybuf & null terminate
    void get_keybuf(char *str,short pos, short num)
    {
        strncpy(str,&keybuf[pos],num);
        str[num] = '\0';
    }


	//routines for ID number

    //initialize for id number entry
    void id_init(void)
	{
		short i;

		for(i = 0; i < 4; i++)
			keybuf[i] = '\0';
		keypos = 0;
		input_time = FALSE;
		maxpos = 2;
        idnum = 0;	    
    }
    
    //set idnum
    void set_idnum(void)
    {
        idnum = atoi(keybuf);
    }

    //format idnum with leading 0
    void get_id_str(char *str)
    {
        sprintf(str,"%02d",idnum);
    }

    bool id_set(void)
    {
        //if (idnum == 0)
        //    return FALSE;
        //else
        //    return TRUE;
        return FALSE;
    }

    //enter idnum
    void enter_idnum(short xpos, short ypos)
    {
        idnum = inpint(6 * xpos, 8 * ypos, SMALL, 0, 2, 0, 99,'\0');
    }
    

    //input_time_set is for routines to know if input_time has been set
    bool input_time_set(void)
    {
        return(input_time);
    }
