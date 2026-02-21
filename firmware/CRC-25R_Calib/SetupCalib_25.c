/*********************************************************************
  MODULE:	CALIBRATOR SETUP  for CRC-25

  FILE:		SetupCalib_25.c

  DATE:		02/07/07
            

  ANALYSIS:	 setup menu for calibrator

  DESIGN: 	presents menus for setup and branches according to
            selection

  CALLED BY:
        utilities

    *************************************************************************/
#include "crc.h"
#include "menu.h"
#include "screen.h"
#include "i2c.h"
#include "keyboard.h"
#include "printer.h"
#include "message.h"
#include "chambfac.h"
#include "remote.h"
#include <string.h>


static void	nuclide_menu(void);
static void choose_printer(void);
static void other_menu(void);
//static void set_cibq(void);
static void moly_setup_menu(void);
short define_linearity_test(void);
void select_remote_nuclides(void);


    extern CURRENT current;
    
    short setup(void)
    {
    
        short iret;
    
        for (;;)
        {
    
            //if(current.lockedbq)
            //    iret = display_menu(CAL_SETUP_BQ);
            //else
            //    iret = display_menu(CAL_SETUP);
    
            if(home_set())
                return 0;
            
            switch (iret)
            {
    
            case -1:  //MENU button
                return MENU_RET;
            case 1:
                set_date_time();
                break;
            case 2:
                //set_cibq();
                break;
            case 4:
                erase_screen();
                if (password())
                {    
                    other_menu();
                    break;
                }       
                else
                {    
                    if(home_set())
                       return 0;
                    erase_screen();
                }    
                break;
    
            case 3:
                choose_printer();
                break;

            case 5:
                screen_control_menu();
                break;
                
    
            }
            if(home_set())
                return 0;
        }
    }
    
    
    bool password(void)
    {
        char sn[6];
        char pw[4];
        char strng[4];
    
        //ENTER PASSWORD
        display_medium_message(PASSWORD,32,0,NORMAL);
        if(!numstr_in(0,0,strng,MEDIUM,0,3,3,FALSE,FALSE))
        {
            if(home_set())
                return FALSE;
            beep();
            return FALSE;
        }
        EE_READ(snum,(uchar *)sn);
        strncpy(pw,&sn[3],3);
        if (strng[0] == pw[0] && strng[1] == pw[1] && strng[2] == pw[2])
            return TRUE;
    
        beep();
        return FALSE;
    }
    
    static void other_menu(void)
    {
        short iret;
        short menu_num;

        short ch_type;

        ch_type = chamber_type(current.main_chamber);
        switch(ch_type)
        {
        case R_CHAMB:
            menu_num = OTHER_MENU_E;
            break;
        case P_CHAMB:
        case C_CHAMB:
        case K_CHAMB:
            menu_num = OTHER_MENU_PET_E;
            break;
        }
    
        for(;;)
        {    
            iret = display_menu(menu_num);
    
            if(home_set())
                return;
        
            switch (iret)
            {
            case -1:  //MENU key
                erase_screen();
                return;
            case 1:
                setup_user_keys();
                break;
            case 2:
                test_source_menu();
                break;
            case 3:
                moly_setup_menu();
                break;
            case 4:
                nuclide_menu();
                break;
            case 5:
                define_linearity_test();
                break;
            case 6:
                remote_busy(TRUE);
                select_remote_nuclides();
                remote_busy(FALSE);
                break;
            }
            if(home_set())
                return;
        }    
    }
    
    static void	nuclide_menu(void)
    {
    
        short iret;
        short menu_num;
    
        short ch_type;

        ch_type = chamber_type(current.main_chamber);
        erase_screen();

        switch(ch_type)
        {
        case P_CHAMB:
        case C_CHAMB:
        case K_CHAMB:
            menu_num = NUCLIDE_MENU_PET;
            break;
        case R_CHAMB:
            if(current.detector == DETECTOR_NONE)
                menu_num = NUCLIDE_MENU_R;
            else
                menu_num = NUCLIDE_MENU_W;
            break;

        }

        for(;;)
        {    
            iret = display_menu(menu_num);

            if(home_set())
                return;
            
            switch (iret)
            {
            case -1:        //MENU
                return;
            case 1:
                erase_screen();
                add_nuclide();
                break;
            case 2:
                erase_screen();
                delete_nuclide();
                break;
            case 3:
                erase_screen();
                change_cal_num();
                break;
            case 4:
                erase_screen();
                container_correction_factors();
                break;
            }
            if(home_set())
                return;
        }
    }
    

/*Selection of Printer:  RS-232, USB, NONE (default)
  RS232: SLIP-TICKET, ROLL, OKI
 If OKI, selection of Ticket or 1 line printing
 USB: HP, EPSON
 */
 
   static const short printer_type[] = 
   {
       PRINTER_TYPE_USB_EPSON,
       PRINTER_TYPE_USB_HP,
       PRINTER_TYPE_SLIP,
       PRINTER_TYPE_ROLL,
       PRINTER_TYPE_OKI,
       PRINTER_TYPE_LX,
       PRINTER_TYPE_NONE,
   };

   static const short oki_print[] = 
   {
        OKI_TICKET,
        OKI_LINE,
   };

   enum interface
   {
       I_RS232 = 1,
       I_USB,
       I_NONE, 
   };    
          
    static void choose_printer(void)
    {

        char printer;
        char prtype;
        short i_type;
        bool yn;

        EE_READ(print[0],(uchar *)&printer);
        EE_READ(print[1],(uchar *)&prtype);
        
        for(;;)
        {
            erase_screen();
            //PRINTER IS
            display_medium_message(PRINTER_1,0,0,NORMAL);
            
            display_medium_message(printer_type[printer + 2],22,0,NORMAL);
            if(printer == OKI_PRINTER || printer == LX_PRINTER)
                display_medium_message(oki_print[prtype],34,0,NORMAL);

            //OK? Y OR N
            display_medium_message(OK_MSG,52,0,NORMAL);
            yn = yesorno();
            if(home_set())
                return;
            if(yn)
            {
                EE_WRITE(print[0],(uchar *)&printer);
                EE_WRITE(print[1],(uchar *)&prtype);
                //save in current structure
                current.printer = printer;
                //initialize new selection
                printer_init();
                erase_screen();
                return;
            }

        

            i_type = display_menu(PRINTER_MENU);
            if(home_set())
                return;

            if(i_type == I_NONE)
            {    
                printer = NONE_PRINTER;
                continue;
            }    

            if(i_type == I_RS232)
                printer = display_menu(PRINTER_MENU_RS232);
            else
                printer = display_menu(PRINTER_MENU_USB);
            if(home_set())
                return;
            if(printer != OKI_PRINTER && printer != LX_PRINTER)
                prtype = PR_TICKET;

            else
            {

                prtype = display_menu(PRINTER_OKI_MENU);
                if(home_set())
                    return;
            }

        }
    }
    
//Set Ci Bq -- selects: Selectable to Ci or Bq, set to Ci or Set to Bq

    /*static const short ci_bq_msg[] =
        {
            CI_BQ_2,
            CI_BQ_3,
            CI_BQ_4,
        };*/
    
    /*static void set_cibq(void)
    {
        short ichoose;
        bool ok;
        bool syslocked;

        EE_READ(syslock,(uchar *)&syslocked);
        EE_READ(syst, &current.system);

        erase_screen();

        //Ci / Bq
        display_medium_message(CI_BQ_1,12,0,NORMAL);
        if (syslocked)
        {
            if (current.system == CI)
                //FIXED TO Ci
                display_medium_message(CI_BQ_2,32,0,NORMAL);
            else
                //FIXED TO Bq
                display_medium_message(CI_BQ_3,32,0,NORMAL);
            
        }
        else
            //SELECTABLE
            display_medium_message(CI_BQ_4,32,0,NORMAL);


        //OK? Y OR N
        display_medium_message(OK_MSG,52,0,NORMAL);
        if (yesorno())
            return;
        if(home_set())
            return;


        do
        {
            ichoose = display_menu(CI_BQ_MENU);
            if(home_set())
                return;
            erase_screen();
            //Ci / Bq
            display_medium_message(CI_BQ_1,12,0,NORMAL);
            display_medium_message(ci_bq_msg[ichoose - 1],32,0,NORMAL);
            //OK? Y OR N
            display_medium_message(OK_MSG,52,0,NORMAL);
            ok = yesorno();
            if(home_set())
                return;
        }while(!ok);

        erase_screen();
        if(ichoose == 3)    //selectable
            syslocked = FALSE;
        else
        {
            syslocked = TRUE;
            current.system = ichoose - 1;
        }

        EE_WRITE(syst,(uchar *)&current.system);
        EE_WRITE(syslock,(uchar *)&syslocked);

    
    
    }*/


static void  mo_lim_unit(short xpos, short ypos);
static void display_mo(uchar *mo, short num);
static void moly_limit(void);
static void currentmo(uchar *mo);
static void moly_method(void);

    static void moly_setup_menu(void)
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
            display_text(8,32,"Mallinckrodt",0,MEDIUM,NORMAL);
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
            if(limit < 1.)
                sprintf(buf,"%05.3f",limit);
            else
                sprintf(buf,"%4.2f",limit);
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
            limit = inpflt(16,44,MEDIUM,0,4,0.001,9.99);
            if(home_set())
                return;

            erase_screen();
        }

    }

static void  mo_lim_unit(short xpos, short ypos){
	char ustr[8];

	// Locked: CI
	//if (current.system == CI)
	//	strcpy(ustr,"$Ci/mCi");     /* uCi/mCi */
	//else
	//	strcpy(ustr,"MBq/GBq");     /* MBq/GBq */
	strcpy(ustr,"$Ci/mCi");     /* uCi/mCi */

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
    
