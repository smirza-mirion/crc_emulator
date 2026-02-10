/*********************************************************************
  MODULE:   CALIBRATOR TESTS for CRC-25

  FILE:		TestsMenu_25.c

  DATE:		01/03/07

  ANALYSIS:	presents menu of calibrator tests and branches


  *************************************************************************/
#include "crc.h"
#include "menu.h"
#include "screen.h"
#include "keyboard.h"
#include "chambfac.h"
#include "daily.h"
#include "remote.h"


    extern CURRENT current;
    extern CHAMBER chamber[];
    
static void enhanced_tests(void);
void geometry_test(void);
void qc_menu(void);
void linearity_test(void);
void moly_assay(void);


	void tests(void)
	{

		short iret;
        short mnum;
        short ch_num = current.main_chamber;       

        short ch_type;

        ch_type = chamber_type(ch_num);
        if (ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB) //PET
            mnum = TESTS_MENU_PET_E;
        else
            mnum = TESTS_MENU_E;

        for(;;)
        {
            iret = display_menu(mnum);
            if(home_set())
                return;
        
            chamber[ch_num].control = CONTROL_MAIN;
			
            switch (iret)
			{
			case -1:     //MENU
				return;
			case 1:
				daily();
				break;
			case 2:
                measure_bias(FALSE);
                if(!chamber_hv_adc(ch_num)){
                	delay_bias(ch_type);
                }
                if(home_set())
                    break;
				break;
			case 3:
				accuracy_test(FALSE);
				break;
            case 4:
                enhanced_tests();
                break;
            case 5:
                moly_assay();
                break;
			}
            if(home_set())
                return;
 
		}

	}



    static void enhanced_tests(void)
    {
        short iret;
        short ch_num = current.main_chamber;
        short ch_type;

        ch_type = chamber_type(ch_num);
        if (ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB) //PET
            iret = display_menu(ENHANCED_TESTS_PET_MENU);
        else
            iret = display_menu(ENHANCED_TESTS_MENU);

        if(home_set())
            return;
        
        for(;;)
        {
            
            switch (iret)
            {
            case -1:     //MENU
                return;
            case 1:
                geometry_test();
                break;
            case 2:
                linearity_test();
                break;
            case 3:  //not PET  
                qc_menu();
                break;

            }
            return;
        }

    }


