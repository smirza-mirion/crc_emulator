/*********************************************************************
  MODULE:	Moly Assay SETUP

  FILE:		MolySetup.c

  DATE:		04/13/04
            

  ANALYSIS:	 setup Moly Assay

  CALLED BY:
        Other Menu

    *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "menu.h"
#include "i2c.h"
#include "keyboard.h"
#include "message.h"
#include <string.h>
static void  mo_lim_unit(short xpos, short ypos);
static void display_mo(uchar *mo, short num);
static void moly_limit(void);
static void currentmo(uchar *mo);
static void moly_method(void);

    void moly_setup_menu(void)
    {
        short iret;

        for(;;)
        {    
            iret = display_menu(MO_MENU);

            if(home_set())
                return;
    
            switch (iret)
            {
            case -1:
                return;
            case 1:
                erase_screen();
                moly_method();
                break;
            case 2:
                erase_screen();
                moly_limit();
                break;
            }
            if(home_set())
                return;
        }    
    }

    static void moly_method(void)
    {

        uchar mo[4];
        bool yn;

        EE_READ(mo_choice,mo);

        for (;;)
        {
            currentmo(mo);
            //OK? Y OR N
            display_medium_message(OK_MSG,52,0,NORMAL);
            yn = yesorno();
            if(home_set())
                return;
            if(yn)
            {
                EE_WRITE(mo_choice,mo);
                return;
            }
        
        
            erase_screen();
            mo[0] = mo[1] = mo[2] = 0;
            //Will You Use
            //CAPMAC For
            display_medium_message(MOLY_SETUP_1,12,0,NORMAL);
            display_medium_message(MOLY_SETUP_2,22,0,NORMAL);            
            display_text(8,32,"Mallindkrodt",0,MEDIUM,NORMAL);
            //Generator ?
            display_medium_message(MOLY_SETUP_3,42,0,NORMAL);            
            yn = yesorno();
            if(home_set())
                return;
            if (yn)
                mo[0] = 1;
            erase_screen();
            //Will You Use
            //CAPMAC For
            display_medium_message(MOLY_SETUP_1,12,0,NORMAL);
            display_medium_message(MOLY_SETUP_2,22,0,NORMAL);            
            display_text(8,32,"Bristol Myers",0,MEDIUM,NORMAL);
            //Generator ?
            display_medium_message(MOLY_SETUP_3,42,0,NORMAL);                        
            yn = yesorno();
            if(home_set())
                return;
            if (yn)
                mo[1] = 1;
            erase_screen();
            //Will You Use
            display_medium_message(MOLY_SETUP_1,12,0,NORMAL);
            display_text(8,22,"CAPINTEC",0,MEDIUM,NORMAL);
            display_text(8,32,"CANISTER ?",0,MEDIUM,NORMAL);
            yn = yesorno();
            if(home_set())
                return;
            if (yn)
                mo[2] = 1;
            erase_screen();

        }

    }

    static void moly_limit(void)
	{

		float limit;
        char buf[10];
        bool yn;

        EE_READ(mo_tc_lim,(uchar *)&limit);

		for(;;)
		{
			//display_text(8,12,"MO/TC LIMIT:",0,MEDIUM,NORMAL);
            display_medium_message(MOLY_SETUP_4,12,0,NORMAL);
            sprintf(buf,"%05.3f",limit);
			display_text(8,26,buf,0,MEDIUM,NORMAL);
			mo_lim_unit(64,26);

			//display_text(16,52,"OK?  Y or N",0,MEDIUM,NORMAL);
            display_medium_message(OK_MSG,52,0,NORMAL);
            yn = yesorno();
            if(home_set())
                return;
			if(yn)
			{
                EE_WRITE(mo_tc_lim,(uchar *)&limit);
				return;
			}

            erase_screen();
            //display_text(8,8,"INPUT",0,MEDIUM,NORMAL);
            //display_text(8,20,"MO/TC LIMIT",0,MEDIUM,NORMAL);
            //display_text(8,32,"in",0,MEDIUM,NORMAL);
            display_medium_message(MOLY_SETUP_5,8,0,NORMAL);
            display_medium_message(MOLY_SETUP_6,20,0,NORMAL);
            display_medium_message(MOLY_SETUP_7,32,0,NORMAL);            
            mo_lim_unit(32,32);
            limit = inpflt(16,44,MEDIUM,0,4,0.001,10.00);
            if(home_set())
                return;

			erase_screen();
		}

	}

  static void  mo_lim_unit(short xpos, short ypos)
	{

		char ustr[8];

		// Locked: CI
		//if (current.system == CI)
		//	strcpy(ustr,"$Ci/mCi");		/* uCi/mCi */
		//else
		//	strcpy(ustr,"MBq/GBq");		/* MBq/GBq */
		strcpy(ustr,"$Ci/mCi");		/* uCi/mCi */

        display_text(xpos, ypos, ustr,0,MEDIUM,NORMAL);
	}




    static void display_mo(uchar *mo, short num)
    {
        char str[2];
        short ypos;

        if (mo[num] == 1)
            strcpy(str,"Y");
        else
            strcpy(str,"N");

        switch(num)
        {
        case 0:
            ypos = 12;
            break;
        case 1:
            ypos = 32;
            break;
        case 2:
            ypos = 42;
            break;
        }    
                
        display_text(112, ypos,str,0,MEDIUM,NORMAL);

    }


    static void currentmo(uchar *mo)
    {

        //display_text(0,2,"CAPMAC For",0,MEDIUM,NORMAL);
        display_medium_message(MOLY_SETUP_8,2,0,NORMAL);
        display_text(0,12,"Mallinckrodt",0,MEDIUM,NORMAL);
        display_mo(mo,0);
        //display_text(0,22,"CAPMAC For",0,MEDIUM,NORMAL);
        display_medium_message(MOLY_SETUP_8,22,0,NORMAL);
        display_text(0,32,"Bristol Myers",0,MEDIUM,NORMAL);
        display_mo(mo,1);
        display_text(0,42,"CANISTER",0,MEDIUM,NORMAL);
        display_mo(mo,2);
        
        

    }    
    
