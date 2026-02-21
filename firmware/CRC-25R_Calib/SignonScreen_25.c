/*********************************************************************
  MODULE:	SIGNON FOR crc-25

  FILE:		SignonScreen._25c

  DATE:		07/25/07

  ANALYSIS:	displays signon message (first screen)
  CALLED BY:
		main

  *************************************************************************/

#include "crc.h"
#include "screen.h"
#include "keyboard.h"
#include "message.h"
#include "uart.h"
#include "amulet.h"
#include <string.h>

extern const char logo;
extern const char *rev_num;
extern const char *crcname;
extern CURRENT current;
    void signon(void)
	{
        char strng[20];
        short xpos,ypos;

        erase_screen();

        set_nokey();


        //display logo
        write_special((char *)&logo,0,0,LOGO,0);

        display_text(18,2,(char *)crcname,0,BIG,NORMAL);

		//display rev number
        strcpy(strng,rev_num);
        if(current.demo_mode)
            strcat(strng,"  DEMO");
        xpos = 24;

        ypos = 18;
        display_text(xpos,ypos,"Rev",0,SMALL,NORMAL);
        xpos += 24;
		display_text(xpos,ypos,strng,0,SMALL,NORMAL);

        ypos += 10;
        display_text(18,ypos,"COPYRIGHT 2007",0,SMALL,NORMAL);
        ypos += 8;
		display_text(6,ypos,"ALL RIGHTS RESERVED",0,SMALL,NORMAL);
        ypos += 8;
		display_text(0,ypos,"CAPINTEC, INC. NJ USA",0,SMALL,NORMAL);

		//display_text(0,56,"  ENTER to Continue  ",1,SMALL,NORMAL);
        display_small_message(SIGNON_1,56,0,NORMAL);

		for(;;)
		{
            /* if(keyin() == OK)
            {
                set_nokey();
				return;
            }
            beep(); */

			service_amulet();
			if (g_ucRelease) {
				set_nokey();
				return;
			}
		}

	}





