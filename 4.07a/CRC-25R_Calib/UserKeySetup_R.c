/*********************************************************************
  MODULE:	USER KEY ASSIGNMENT for CRC-25R

  FILE:		UserKeySetup_R.c

  DATE:		08/23/06

  ANALYSIS:  user assigns nuclide to User keys 


  DESIGN:	 accesses/stores in ukeys[]  or cntr_ukeys[]
  CALLED BY:
		setup routines

  *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "i2c.h"
#include "keyboard.h"
#include "message.h"
#include "chambfac.h"
#include "nuc.h"
#include <string.h>
    
	void setup_user_keys(void)
	{

		short nuc_index;
		bool ok;
		char uname[6];
		char nucname[8];
		short num;
        char ch;
        char ustr[10];
        bool yn;

       
        strcpy(ustr,"U1 TO U5");

        
		for(;;)
		{
			erase_screen();


			//SELECT
			//USER KEY
            display_medium_message(USER_KEY_3,12,0,NORMAL);
            display_medium_message(USER_KEY_4,24,0,NORMAL);            
			display_text(8,36,ustr,0,MEDIUM,NORMAL);
			//Any Other Key To
			//Continue Setup
            display_small_message(USER_KEY_5,48,0,NORMAL);
            display_small_message(USER_KEY_6,56,0,NORMAL);            
			
			ch = keyin();
            if(home_set())
                return;

			switch (ch)
			{
            
            case USER1:
            case USER2:
            case USER3:
            case USER4:
            case USER5:
                num = (short)ch - USER1;
                break;

			default:
				return;
			}

            erase_screen();

                    
            EE_READ(user_keys[0].keys[num],(uchar *)&nuc_index);


			if(nuc_index == -1)
				strcpy(nucname,"NONE  ");
			else
				NuclideData_getName(nuc_index,nucname);


			for(;;)
			{
                strcpy(uname,"U :");
                uname[1] = (char)num + '1';
				display_text(8,12,uname,0,MEDIUM,NORMAL);
				display_text(48,12,nucname,0,MEDIUM,NORMAL);
				//OK? Y OR N
                display_medium_message(OK_MSG,52,0,NORMAL);
				yn = yesorno();
                if(home_set())
                    return;
				if(yn)
				{
                    EE_WRITE(user_keys[0].keys[num],(uchar *)&nuc_index);
					break;
				}

				do
				{
                    erase_screen();
					ok = TRUE;
					specify_nuclide_msg();
					nuc_index = get_nuclide_index(TRUE);
                    if(home_set())
                        return;

                    
                    erase_screen();
					switch (nuc_index)
					{
					case -1:
						no_nuclide_msg();
						ok = FALSE;
						break;

					case -2:
						specify_further_msg();
						ok = FALSE;
						break;
                    case -3:
                        nuc_index = -1;
                        break;

                    case -4:  
                        no_calnum_msg();
                        if(home_set())
                            return;
                        ok = FALSE;
                        nuc_index = -1;
                        break;

					default:
						break;
					}
				}while(!ok);

				if(nuc_index == -1)
					strcpy(nucname,"NONE  ");
				else
					NuclideData_getName(nuc_index,nucname);

                erase_lines(54,10,0);

			}
		}

	}



