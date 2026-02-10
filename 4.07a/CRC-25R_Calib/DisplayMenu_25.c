/*********************************************************************
  MODULE:		DISPLAY MENU for CRC-25 family

  FILE:			DisplayMenu_25.c

  DATE:			05/09/07
                08/27/08  -- added Delete Source to Factory Detector menu  

  ANALYSIS:     display a menu

    *************************************************************************/

#include "crc.h"
#include "screen.h"
#include "menu.h"
#include "keyboard.h"
#include "chambfac.h"
#include <stdio.h>
#include <string.h>


static const MENU menus[] =
	{
            MAIN_MENU,  //for PET
            "",
            "Calculations",1,        
            "Diagnostics",3,
            "Setup",4,
            "Tests",5,
            "",0,
            "",0,
            1,-1,               //Menu for Factory

            MAIN_MENU_LOCK, //locked to BQ -- for Pet
            "",
            "Decay Calc",2,
            "Diagnostics",3,
            "Setup",4,
            "Tests",5,
            "",0,
            "",0,
            1,-1,               //Menu for Factory
            
            MAIN_MENU_E,     //Enhanced
            "",
            "Inventory",5,
            "Calculations",1,        
            "Diagnostics",3,
            "Setup",4,
            "",0,
            "",0,
            1,-1,               //Menu for Factory
        
            MAIN_MENU_E_LOCK,     //Enhanced & locked
            "",
            "Inventory",5,
            "Decay Calc",2,
            "Diagnostics",3,
            "Setup",4,
            "",0,
            "",0,
            1,-1,               //Menu for Factory

            UTIL_CALC_MENU,
            "",
            "C)<>Bq Conv",1,        //Ci Bq Conversion
            "Decay Calc",2,
            "",0,
            "",0,
            "",0,
            "",0,
            1,-1,
            
            CAL_SETUP_BQ,
            "SETUP",
			"Time",1,
			"Printing",3,
			"Other",4,
			"Screen",5,
			"",0,
			"",0,
			1,-1,

			CAL_SETUP,
            "SETUP",
			"Time",1,
			"C)/Bq",2,			
			"Printing",3,
			"Other",4,
			"Screen",5,
			"",0,
			1,-1,

			WELL_SETUP,
            "",
			"Nuclides",1,
			"Test Source",6,
			"Trig. Level",2,
			"Efficiencies",3,
			"User Keys",4,
			"Reset HV",5,
			1,-1,

			BETA_SETUP,
            "SETUP",
            "Test Source",6,
			"Limits",7,
			"Efficiency",3,
			"User Keys",4,
			"Reset HV",5,
			"",0,
			1,-1,
			
            DETECTOR_FACTORY_SETUP,
            "DETECTOR",
            "S / N",1,
            "Calibration",2,
            "Delete Source",3,
            "",0,
            "",0,
            "",0,
            1,-1,

            
			OTHER_MENU,
            "",
			"User Keys",1,
			"Sources",2,
			"Moly Setup",3,
			"Nuclides",4,
			"",0,
            "",0,
			1,-1,

            OTHER_MENU_PET,
            "",
			"User Keys",1,
			"Sources",2,
			"Nuclides",4,
            "Remote",6,
			"",0,
            "",0,
			1,-1,


			OTHER_MENU_E,
            "",
			"User Keys",1,
			"Sources",2,
			"Moly Setup",3,
			"Nuclides",4,
			"Linearity",5,
            "Remote",6,
			1,-1,
			
			OTHER_MENU_PET_E,
            "",
			"User Keys",1,
			"Sources",2,
			"Nuclides",4,
			"Linearity",5,
            "Remote",6,
			"",0,
			1,-1,
            
            TS_MENU,
            "",
			"Co57",0,
			"Co60",1,
			"Ba133",2,
			"Cs137",3,
			"Ra226",4,
			"Constancy",6,
			1,-1,

            TS_MENU_PET,
            "",
			"Co57",0,
			"Co60",1,
			"Ba133",2,
			"Cs137",3,
			"Na22",4,
			"Constancy",6,
			1,-1,

			
            NUCLIDE_MENU_R,
            "",
			"Add Nuclide",1,
			"Del Nuclide",2,
			"New Cal Num",3,
			"",0,
            "",0,
			"",0,
			1,-1,
			
            NUCLIDE_MENU_W,
            "",
            "Add Nuclide",1,
            "Del Nuclide",2,
            "New Cal Num",3,
            "Corr Factors",4,
            "",0,
            "",0,
            1,-1,
			
            NUCLIDE_MENU_PET,
            "",
			"Add Nuclide",1,
			"Del Nuclide",2,
			"Cal Numbers",3,
			"",0,
			"",0,
			"",0,
			1,-1,


			MO_MENU,
            "MO ASSAY",
			"Moly Method",1,
			"Mo/Tc Limit",2,
			"",0,
			"",0,
			"",0,
			"",0,
			1,-1,


			FACTORY,
            "",
			"INIT EE",1,
			"SERIAL #",2,
			"CHAMBER",3,
			"",0,
            "",0,
            "",0,
			1,-1,

            FACTORY_WB,
            "",
            "INIT EE",1,
            "SERIAL #",2,
            "CHAMBER",3,
            "COUNTER",4,
            "",0,
            "",0,
            1,-1,
            
			CAL_FACTORY,
            "",
            "SERIAL #",3,
			"RESPONSE",1,
            "VOLTAGE",4,
            "",0,
            "",0,
            "",0,
			1,-1,

            
            
            TESTS_MENU,
            "TESTS",
			"Daily",1,
			"Chamber Volts",2,
			"Accuracy",3,
			"Moly Assay",5,
			"",0,
			"",0,
			1,-1,

            TESTS_MENU_E,
            "TESTS",
			"Daily",1,
			"Chamber Volts",2,
			"Accuracy",3,
			"Enhanced",4,
            "Moly Assay",5,
			"",0,
			1,-1,

            TESTS_MENU_PET,
            "TESTS",
            "Daily",1,
            "Chamber Volts",2,
            "Accuracy",3,
            "",0,
            "",0,
            "",0,
            1,-1,

            TESTS_MENU_PET_E,
            "TESTS",
            "Daily",1,
            "Chamber Volts",2,
            "Accuracy",3,
            "Enhanced",4,
            "",0,
            "",0,
            1,-1,
            ENHANCED_TESTS_MENU,
            "TESTS",
			"Geometry",1,
			"Linearity",2,
			"QC",3,
			"",0,
			"",0,
			"",0,
			1,-1,
                           
            ENHANCED_TESTS_PET_MENU,
            "TESTS",
			"Geometry",1,
			"Linearity",2,
			"",0,
			"",0,
			"",0,
			"",0,
			1,-1,

            BETA_LIMITS_MENU,
            "LIMITS",
            "Background",1,
            "% Impurity",2,
            "Precision",3,
			"",0,
			"",0,
			"",0,
			1,-1,

		    WELL_NUCLIDES_MENU,
            "",
			"Wipe",1,
			"Unrestricted",2,
			"Sealed Source",3,
			"",0,
			"",0,
			"",0,
			1,-1,

            WELL_MEASURE_MENU,
            "",
            "Wipe",1,
            "Unrestricted",2,
            "Sealed Source",3,
            "General",4,
            "",0,
            "",0,
            1,-1,
            
            WELL_TRIG_MENU,
            "",
            "Background",0,
            "Wipe",1,
            "Unrestricted",2,
            "Sealed Source",3,
            "",0,
            "",0,
            1,-1,

            BETA_CF_MENU,
            "",
            "Activity Data",1,
            "Impurity Data",2,
			"",0,
			"",0,
			"",0,
			"",0,
			1,-1,

            BETA_DISTANCE_MENU,
            "",
            "10 cm",0,
            "20 cm",1,
            "",0,
            "",0,
            "",0,
            "",0,
            1,-1,
    
            BETA_MAIN_MENU,
            "",
            "Auto Cal",1,
            "Setup",2,
			"",0,
            "",0,
			"",0,
			"",0,
			1,-1,

            WELL_MAIN_MENU,
            "",
            "Measure",5,
            "Auto Cal",1,
            "Setup",2,
            "Lab Tests",4,
            "MDA Test",3,
            "",0,
            1,-1,

    	    CF_MENU,
            "",
			"Input Eff",0,
			"Measure Eff",1,
			"",0,
			"",0,
			"",0,
			"",0,
			1,-1,

            CAL_FACTOR_MENU,
            "",
            "Input",0,
            "Measure",1,
            "",0,
            "",0,
            "",0,
            "",0,
            1,-1,

            

            CONTAINER_MENU,
            "CONTAINER",
            "Reference",0,
            "Syringe",1,
            "Vial",2,
			"",0,
			"",0,
			"",0,
			0,0,

            PRINTER_MENU,
            "PRINTER IS",
            "RS-232",1,
            "USB",2,
            "NONE",3,
            "",0,
            "",0,
            "",0,
            0,0,

            PRINTER_OKI_MENU,
            "MEASUREMENT ON",
            "TICKET",0,
            "ONE LINE",1,
            "",0,
            "",0,
            "",0,
            "",0,
            0,0,

            CI_BQ_MENU,
            "",
            "Set To Ci",1,
            "Set To Bq",2,
            "Ci or Bq",3,
            "",0,
            "",0,
            "",0,
            0,0,

            SCREEN_SAVER_MENU,
            "Screen Off",
            "After 5  min",1,
            "After 10 min",2,
            "After 15 min",3,
            "Never",0,
            "",0,
            "",0,
            0,0,

            SCREEN_CONTROL_MENU,
            "",
            "Screen Saver",1,
            "Contrast",2,
            "",0,
            "",0,
            "",0,
            "",0,
            1,-1,

            LAB_TESTS_MENU,
            "",
            "Schilling",1,
            "Plasma",3,
            "RBC",4,
            "",0,
            "",0,
            "",0,
            1,-1,

            WELL_TEST_SOURCE_MENU,
            "Select Source",
            "Cs137",3,
            "Ba133",2,
            "",0,
            "",0,
            "",0,
            "",0,
            1,-1,

            LIN_TEST_MENU,
            "Select Test",
            "Standard",0,
            "Lineator",1,
            "Calicheck",2,
            "",0,
            "",0,
            "",0,
            1,-1,

            INVENTORY_MENU,
            "INVENTORY",
            "Add",1,
            "Make Kit",2,
            "Withdraw",3,
            "Print",4,
            "Delete",5,
            "",0,
            1,-1,

            INVENTORY_DELETE_MENU,
            "Delete",
            "All",1,
            "Single Item",2,
            "",0,
            "",0,
            "",0,
            "",0,
            0,0,
            
            GEOMETRY_TEST_MENU,
            "SELECT",
            "Syringe",1,
            "Vial",2,
            "",0,
            "",0,
            "",0,
            "",0,
            1,-1,

            QC_MENU,
            "QC TESTS",
            "1 Strip",1,
            "2 Strips",2,
            "HMPAO",3,
            "MAG3",4,
            "",0,
            "",0,
            1,-1,

            OTHER_MENU_BT,
            "",
            "User Keys",1,
            "Source Data",2,
            "Test Sources",3,
            "Nuclides",4,
            "",0,
            "",0,
            1,-1,

            NUCLIDE_MENU_BT,
            "",
            "Add Nuclide",1,
            "Del Nuclide",2,
            "",0,
            "",0,
            "",0,
            "",0,
            1,-1,

            SOURCES_MENU_BT,
            "",
            "Add Source",1,
            "ADCL",2,
            "MULTI",3,
            "",0,
            "",0,
            "",0,
            1,-1,
            
            ADD_SOURCE_MENU_BT,
            "",
            "Add Source",1,
            "Delete Sorce",2,
            "",0,
            "",0,
            "",0,
            "",0,
            1,-1,

            
            MULTI_SOURCE_MENU_BT,
            "",
            "Print Data",1,
            "New Data",2,
            "Done",3,
            "",0,
            "",0,
            "",0,
            1,-1,

            MULTI_SOURCE_METHOD_BT,
            "Multi Data From",
            "Measurement",1,
            "Input",2,
            "",0,
            "",0,
            "",0,
            "",0,
            1,-1,

            TS_MENU_BT,
            "",
            "Co57",0,
            "Co60",1,
            "Ba133",2,
            "Cs137",3,
            "Constancy",6,
            "",0,
            1,-1,

            PRINTER_MENU_RS232,
            "SELECT",
            "SLIP - TICKET",0,
            "ROLL",1,
            "OKI",2,
            "LX-300",3,
            "",0,
            "",0,
            0,0,

            
            PRINTER_MENU_USB,
            "SELECT",
            "HP",-1,
            "EPSON",-2,
            "",0,
            "",0,
            "",0,
            "",0,
            0,0,
            
    };


static short find_menu(short menu_num,MENU *menu);
	short display_menu(short menu_num)
	{
		short len;
		MENU menu;
		short i;
		short i0;
		short num_sel;
		short pos;
		char pr_str[16];
        char ch;

		if(find_menu(menu_num,&menu) == -1)
			return -1;


		num_sel = 0;
		for(i = 0; i < 6; i++)
		{
            if(menu.selection[i].prompt[0] != '\0')
				++num_sel;
		}
		switch(num_sel)
		{
		case 6:
			i0 = 0;
			break;
		case 5:
		case 4:
			i0 = 1;
			break;
		case 3:
		case 2:
			i0 = 2;
			break;
		}

		erase_screen();
        //display heading if not zero length
		len = strlen(&menu.heading[0]);
        if (len != 0)
        {
            short xpos = 8 * ((16 - len) / 2);
                
            display_text(xpos,0,&menu.heading[0],0,MEDIUM,NORMAL);
        }

                

        //display each prompt preceded by number
        for(i = 0; i < 6; i++)
		{
            short ypos;

            if (menu.selection[i].prompt[0] == '\0')
				break;

			pr_str[0] = '1' + (uchar)i;
            strcpy(&pr_str[1],". ");
            strcpy(&pr_str[3],&menu.selection[i].prompt[0]);
            ypos = 10 * (i + i0) + 2;
            display_text(0,ypos,pr_str,0,MEDIUM,NORMAL);


		}



		for(;;)
		{
			ch = keyin();
			switch (ch)
			{

            case HOMEKEY:
                return 0;
                
            case MENUBUT:
			       if(menu.key_select.active == 0)
					    beep();
				else
					return(menu.key_select.ret);
				break;

			case ONE:
			case TWO:
			case THREE:
			case FOUR:
			case FIVE:
			case SIX:
				pos = ch - ONE;
                if (menu.selection[pos].prompt[0] != '\0')
                    return(menu.selection[pos].ret);
				beep();
				break;

			default:
				beep();
				break;
			}

		}

	}


		/* find the menu */
static short find_menu(short menu_num,MENU *menu)
	{
		short index;
		short i;

		index = -1;
		for(i = 0; i < NUM_MENUS; i++)
		{
			memcpy(menu,&menus[i],sizeof(*menu));
			if(menu -> id == menu_num)
			{
				index = i;
				break;
			}
		}

		if(index == -1)
		{
#if SYSCALLS
            printf("menu %d not found\r\n",menu_num);
#endif      
		}
		return (index);
	}
	

	
