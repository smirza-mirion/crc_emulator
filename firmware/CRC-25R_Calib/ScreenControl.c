/*********************************************************************
  MODULE:   SCREEN CONTROL

  FILE:		ScreenControl.c

  DATE:		12/14/06
            

  ANALYSIS:	 screen control functions
          Screen Saver Setup
          Contrast

  CALLED BY:
        other menu

    *************************************************************************/
#include "crc.h"
#include "menu.h"
#include "screen.h"
#include "i2c.h"
#include "keyboard.h"
#include "message.h"
#include <string.h>


static void screen_saver_setup(void);
static void screen_contrast(void);
    
    short screen_control_menu(void)
    {
    
        short iret;
    
        for (;;)
        {
    
            iret = display_menu(SCREEN_CONTROL_MENU);
    
            if(home_set())
                return 0;
            
            switch (iret)
            {
    
            case -1:  //MENU 
                return -1;
            case 1:
                screen_saver_setup();
                break;
            case 2:
                screen_contrast();
                break;
    
            }
            if(home_set())
                return 0;
        }
    }
    
    

    //static char *saver_str[] = {"Never","After 5 min","After 10 min","After 15 min"};
    static void screen_saver_setup(void)
    {
        short max;
        short index;
        bool yn;
        

        EE_READ(maxscr,(uchar *)&max);
        index = max / 5;
        for(;;)
        {
            erase_screen();
            //display_text(0,0,"Screen is Off",0,MEDIUM,NORMAL);
            display_medium_message(SAVER_1,0,0,NORMAL);
            
            //display_text(8,22,saver_str[index],0,MEDIUM,NORMAL);
            display_medium_message(SAVER_2 + index,22,0,NORMAL);

            //OK? Y OR N
            display_medium_message(OK_MSG,52,0,NORMAL);
            yn = yesorno();
            if(home_set())
                return;
            if(yn)
            {
                max = 5 * index;
                EE_WRITE(maxscr,(uchar *)&max);

                //re-initialize screen saver
                init_screen_saver();
                
                erase_screen();
                return;
            }

        

            index = display_menu(SCREEN_SAVER_MENU);
            if(home_set())
                return;


        }

    }   


    static void screen_contrast(void)
    {

        char con;  //contrast, max is 0x3f = 63
        char con0;
        char ch;
        bool flag;  //write data & change contrast flag

        EE_READ(contrast,(uchar *)&con);
        con0 = con;

        erase_screen();
        //display_text(8,2,"Use Arrows To",0,MEDIUM,NORMAL);
        //display_text(8,14,"Adjust Screen",0,MEDIUM,NORMAL);
        display_medium_message(CONTRAST_1,2,0,NORMAL);
        display_medium_message(CONTRAST_2,14,0,NORMAL);
        //display_text(0,44,"Press Enter When Done",0,SMALL,NORMAL);
        display_small_message(CONTRAST_3,44,0,NORMAL);


        for(;;)
        {
            ch = keyin();
            if(home_set())
            {    
                write_dac6(con0);   // return to original contrast             
                return;
            }    

            flag = FALSE;

            switch(ch)
            {
            case UP:
                if(con == 63)
                    beep();
                else
                {
                    con += 4;
                    if(con > 63)
                        con = 63;
                    flag = TRUE;
                }
                break;

            case DOWN:
                if(con == 0)
                    beep();
                else
                {
                    con -= 4;
                    if(con < 0)
                        con = 0;
                    flag = TRUE;
                }
                break;
                
            case OK:
                EE_WRITE(contrast,(uchar *)&con);
                return;

            default:
                beep();
                break;
                
            }

            if(flag)
                write_dac6(con);

        }    

    }

    void write_dac6(uchar val)
    {
        uchar data;
        
        //must shift left twice
        data = val << 2;
        
        setup_i2c();

        i2c_write(DAC6_SLAVE_ADDRESS,&data,1,TRUE);
    }
    
