/*********************************************************************
  MODULE:       SETUP REMOTES  for CRC-25 R,W

  FILE:         SetupRemotes_R.c

  DATE:         12/08/06

  ANALYSIS:     user enter/edits nuclides for R and PET remotes

*************************************************************************/

#include "crc.h"
#include "screen.h"
#include "menu.h"
#include "keyboard.h"
#include "i2c.h"
#include "chambfac.h"
#include "message.h"
#include "nuc.h"
#include <string.h>

static void wr_nuc(char *nucname,short i);
static short on_list(char *nucname,short ch_type);
static bool delete_from_list(char *nucname, short list_index, short ch_type);

static KEYDEF rem_nucs[8];

    void select_remote_nuclides(void)
    {

        short i;
        char ch;
        char ch1;
        short nuc_index;
        char name[8];
        char nucname[8];
        short num;  //number on list
        short list_index; //index of nuclide on the list
        bool del;

        EE_READ(remote_nucs,(uchar *)rem_nucs);

        erase_screen();
        
        num = 0;
        for(i = 0; i < 8; i++)
        {    
            strcpy(name,&rem_nucs[i].keyiso[0].iso_name[0]);
            if(strlen(name) != 0)
            {
                ++num;
                wr_nuc(name,i);
            }    
        }

        //display_text(8,14,"CHOOSE NUCLIDE",0,MEDIUM,NORMAL);
        display_medium_message(CHOOSE_NUCLIDE_1,14,0,NORMAL);
        for(;;)
        {
            
            nuc_index = -1;
            ch = keyin();
            if(home_set())
                return;
            
            switch(ch)
            {
            case MENUBUT:
                //display_text(8,52,"Save Nuclides?",0,MEDIUM,NORMAL);
                display_medium_message(REMOTES_1,52,0,NORMAL);
                if(yesorno())
                    EE_WRITE(remote_nucs,(uchar *)rem_nucs);
                return;

            case OK:
                EE_WRITE(remote_nucs,(uchar *)rem_nucs);
                return;

            case ISO1:
            case ISO2:
            case ISO3:
            case ISO4:
            case ISO5:
            case ISO6:
            case ISO7:
            case ISO8:    
                nuc_index = NuclideData_getNucIndexFromButtonKey(ch - ISO1,0);
                break;

            case USER1:
            case USER2:
            case USER3:
            case USER4:
            case USER5:
                ch1 = ch - USER1;
                nuc_index = user_key_index(ch1);
                break;

            case NUCBUT:
                read_screen(0);
                nuc_index = get_nuclide_index(TRUE);
                if(home_set())
                    return;
                write_screen(0);
                break;

            default:
                break;
            }
            if(nuc_index < 0)
                beep();
            else
            {
                NuclideData_getName(nuc_index,nucname);
                list_index = on_list(nucname,0);
                //if not on list, error message if list full, else add to list
                if(list_index == -1)  
                {
                    if(num == 8)
                    {
                         beep();
                         read_screen(0);
                         erase_lines(14,50,0);
                         //display_text(8,32,"LIST FULL",0,MEDIUM,0);
                         display_medium_message(REMOTES_2,32,0,NORMAL);
                         contmsg();
                        if(home_set())
                            return;
                         
                         write_screen(0);
                     }
                     else
                     {
                         //add to list
                         strcpy(&rem_nucs[num].keyiso[0].iso_name[0],nucname);
                         ++num;
                        for(i = 0; i < 8; i++)
                        {    
                            strcpy(name,&rem_nucs[i].keyiso[0].iso_name[0]);
                            if(strlen(name) != 0)
                                wr_nuc(name,i);
                        }
                         
                     }    
                }
                else    //on list, delete it
                {    
                    del = delete_from_list(nucname,list_index,0);
                    if(home_set())
                        return;
                    if(del)
                        --num;
                }    
            }
        }
    }

    static short on_list(char *nucname,short ch_type)
    {
        short i;
        char name[8];
        short num;
        short len;

        num = -1;   //not on list
        for(i = 0; i < 8; i++)
        {    
            if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(name,&rem_nucs[i].keyiso[R_CHAMB].iso_name[0]);
            else strcpy(name,&rem_nucs[i].keyiso[ch_type].iso_name[0]);
            len = strlen(name);
            if(len != 0)
            {    
                if(strncmp(nucname,name,len) == 0)    
                {
                    num = i;
                    break;
                }
            }
        }
        return num;
    }

    
    
    static bool delete_from_list(char *nucname, short list_index, short ch_type)
    {
        short i;
        bool  ynret;
        char name[8];

        read_screen(0);
        erase_lines(24,40,0);
        //display_text(6,40,"Already On List",0,SMALL,NORMAL);
        //display_text(6,48,"Delete? Y or N",0,SMALL,NORMAL);
        display_small_message(REMOTES_3,40,0,NORMAL);
        display_small_message(REMOTES_4,48,0,NORMAL);
        ynret = yesorno();
        if(!ynret)
        {    
            write_screen(0);
            return FALSE;
        }

        erase_lines(40,24,0);
        for(i = list_index; i < 7; i++){
        	if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(&rem_nucs[i].keyiso[R_CHAMB].iso_name[0], &rem_nucs[i + 1].keyiso[R_CHAMB].iso_name[0]);
        	else strcpy(&rem_nucs[i].keyiso[ch_type].iso_name[0], &rem_nucs[i + 1].keyiso[ch_type].iso_name[0]);
        }

        if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(&rem_nucs[7].keyiso[R_CHAMB].iso_name[0],"");
        else strcpy(&rem_nucs[7].keyiso[ch_type].iso_name[0],"");

        for(i = 0; i < 8; i++)
        {    
            if(ch_type == C_CHAMB || ch_type == K_CHAMB) strcpy(name,&rem_nucs[i].keyiso[R_CHAMB].iso_name[0]);
            else strcpy(name,&rem_nucs[i].keyiso[ch_type].iso_name[0]);
            if(strlen(name) != 0)
                wr_nuc(name,i);
        }

        return TRUE;

     }



    static void wr_nuc(char *nucname,short i)
    {
        short j,line;

        switch(i)
        {
        case 0:
        case 1:
        case 2:
            j = 7 * i;
            line = 3; 
            break;
        case 3:
        case 4:
        case 5:
            j = 7 * (i - 3);
            line = 4; 
            break;
        case 6:
        case 7:
        case 8:
            j = 7 * (i - 6);
            line = 5; 
            break;
        case 9:
            j = 0;
            line = 6; 
            break;
        }
        display_text(6 * j,8 * line + 2,nucname,0,SMALL,NORMAL);
    }
