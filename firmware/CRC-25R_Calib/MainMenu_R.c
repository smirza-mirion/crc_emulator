/*********************************************************************
  MODULE:	 MAIN MENU  for CRC-25 R,W

  FILE:		MainMenu_R.c

  DATE:		11/28/06

  ANALYSIS:	displays utilities menu and gets user choice

  CALLED BY:
		activity

    *************************************************************************/
#include "crc.h"
#include "i2c.h"
#include "menu.h"
#include "screen.h"
#include "keyboard.h"
#include "chambfac.h"
#include "remote.h"

extern CURRENT current;

void inventory_menu(void);
void print_diagnostic_data(void);
static void util_calc_menu(void);

	short main_menu(void)
	{
        short iret;
        short menu_num;

        for(;;)  /* forever */
		{

            //if (current.lockedbq) //locked to BQ
            //    menu_num = MAIN_MENU_E_LOCK;
            //else
               menu_num = MAIN_MENU_E;


            iret = display_menu(menu_num);

            if(home_set())
                return 0;
            
            switch (iret)

            {
            case -1:    //MENU pressed for Factory
                erase_screen();
                facmenu();
                break;
            case 1:
                util_calc_menu();
                break;
            case 2:
                //util_decay();
                break;
            case 3:
                remote_busy(TRUE);
                busy_sent();
                set_pc_freeze_flag(TRUE);
                print_diagnostic_data();
                remote_busy(FALSE);
                busy_sent();
                set_pc_freeze_flag(FALSE);
                break;
            case 4:
                setup();
                break;
            case 5:
                inventory_menu();
                break;
            }
            if(home_set())
                return 0;
		} /* forever */
	}




    static void util_calc_menu(void)
    {
    
        short iret;

        erase_screen();

        for(;;)
        {    
            iret = display_menu(UTIL_CALC_MENU);

            if(home_set())
                return;
            
            switch (iret)
            {
            case -1:        //MENU
                return;
            case 1:
                util_convert();
                break;
            case 2:
                //util_decay();
                break;
            }
            if(home_set())
                return;
        }
    }



    extern REMOTE remote[];
    extern const short max_chambers;
    void busy_sent(void)
    {

        short i;

        if(current.demo_mode)
            return;
        
        for(i = 0; i < max_chambers; i++)
        {
            if(remote[i].exists && remote[i].mode != -1) //skip if still in Heartbeat mode
            {    
                while(remote[i].valid_flag);   //wait for remote to be sent message
            }
        }    


    }    
