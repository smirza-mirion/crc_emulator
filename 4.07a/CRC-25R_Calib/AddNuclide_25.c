/*********************************************************************
  MODULE:	 Add Nuclides -- for CRC-25

  FILE:		AddNuclide_25c

  DATE:		01/08/07
            

  CALLED BY:
        nuclide_menu

    *************************************************************************/
#include "crc.h"
#include "menu.h"
#include "screen.h"
#include "keyboard.h"
#include "i2c.h"
#include "message.h"
#include "chambfac.h"
#include "nuc.h"
#include <string.h>

    extern CURRENT current;
    float NuclideCal_getResponse(char calstrng[], short ch_type);
static void  display_user_data(NUCDATA *nucdata);
static bool add_nuclide_data(float *tau, short *taunit,char *nucname);

	void add_nuclide(void)
	{
		short index;	/* index into nuclide data structure */
		short i;		/* loop counter */
		char ch;			/* character in name */
		short hlunit;
		short taunit;
		float tau;
		char nucname[8];
		NUCDATA  nucdata;
        bool yn;
        float response;
        short j;


        short ch_type;

        ch_type = chamber_type(current.main_chamber);
        for (;;)
        {

    		/*
    			find empty slot for nuclide -- index
    		if none
    			error message, return
    		*/
    		index = -1;	// initialize to no room  -- 10 slots
    		for (i = 0; i < 10; i++)
    		{
                //EE_READ(usernucl[i].hlunit,(uchar *)&hlunit);
    			if (hlunit == -1)
    			{
    				index = i;		// is empty, use this one 
    				break;
    			}
    		}
    
    		if (index == -1)		// no slot found 
    		{
    			// error message -- no room 
    			//display_text(8,12,"NO ROOM TO ADD",0,MEDIUM,NORMAL);
                display_medium_message(NO_ROOM,12,0,NORMAL);
    			contmsg();
    			erase_screen();
    			return;
    		}
    
    
    
    
    		// nuclide can be added, user inputs name, half-life 
    
    		if(!add_nuclide_data(&tau,&taunit,nucname))
    			return;
    
            
            strcpy(&nucdata.name[0],nucname);
    		nucdata.halflife = tau;
    		nucdata.hlunit = (char)taunit;

            
            
    		// generate code from name
            j = 0;
    		for (i = 0; i < 2; i++)
    		{
    			ch = nucdata.name[i];
    			switch(ch)
    			{
    			case ' ':
    				//nucdata.code[i] = '1';
    				break;
    			case 'A':
    			case 'B':
    			case 'C':
    				nucdata.code[j++] = '2';
    				break;
    			case 'D':
    			case 'E':
    			case 'F':
    				nucdata.code[j++] = '3';
    				break;
    			case 'G':
    			case 'H':
    			case 'I':
    				nucdata.code[j++] = '4';
    				break;
    			case 'J':
    			case 'K':
    			case 'L':
    				nucdata.code[j++] = '5';
    				break;
    			case 'M':
    			case 'N':
    			case 'O':
    				nucdata.code[j++] = '6';
    				break;
    			case 'P':
    			case 'R':
    			case 'S':
    				nucdata.code[j++] = '7';
    				break;
    			case 'T':
    			case 'U':
    			case 'V':
    				nucdata.code[j++] = '8';
    				break;
    			case 'W':
    			case 'X':
    			case 'Y':
    				nucdata.code[j++] = '9';
    				break;
    			case 'Q':
    			case 'Z':
    				nucdata.code[j++] = '0';
    				break;
    			}
    		}
    
    		for (i = 2; i < 5; i++)
    			nucdata.code[j++] = nucdata.name[i];

            //change 2nd letter to lower case
            if(nucdata.name[1] != ' ')
                nucdata.name[1] += 0x20;
            
    		display_user_data(&nucdata);
    		//display_text(16,52,"OK?  Y OR N",0,MEDIUM,NORMAL);
            display_medium_message(OK_MSG,52,0,NORMAL);
            yn = yesorno();
            if(home_set())
                return;
    		if(yn)
    		{
                response = get_calnum(FALSE,ch_type);
                if(home_set())
                    return;
                if(response == -1)
                    response = 0.;
                nucdata.response[ch_type] = response;
                //save values in usernucl[index]
                //EE_WRITE(usernucl[index],(uchar *)&nucdata);
    			return ;
    		}
    		erase_screen();
        }



	}


    float get_calnum(bool zero_flag,short ch_type)                
    {
    
        bool iret;
        char calstrng[10];
        float response;
    
        erase_screen();
        //display_text(40,2,"ENTER",0,MEDIUM,NORMAL);
        //display_text(8,12,"CALIBRATION #",0,MEDIUM,NORMAL);
        display_medium_message(CAL_NUM_1,2,0,NORMAL);
        display_medium_message(CAL_NUM_2,12,0,NORMAL);
            
    
        do
        {
            do
            {
                iret = numstr_in(24,52,calstrng,MEDIUM,0,8,8,
                                 TRUE,TRUE);
                if(!iret)
                {
                    if(home_set())
                        return -1;
                    if(zero_flag)
                        return -1.;
                    else
                        beep();
                }    
                            
            }while(!iret);
    

            response = NuclideCal_getResponse(calstrng,ch_type);
            if(response == -1)
            {
                erase_lines(52,12,0);
                beep();
            }
        }while(response == -1);
    
        return(response);
    
    }
    
        extern const char *timeunit[];
    static void  display_user_data(NUCDATA *nucdata)
    {
        short ndec;
        char formatstr[7];
        char buf[10];
        char strng[16];
        

        erase_screen();
        display_text(40,0,&nucdata -> name[0],0,MEDIUM,NORMAL);
        ndec = 3;
        if (nucdata -> halflife > 50.)
            ndec = 2;
        if (nucdata -> halflife > 500.)
            ndec = 1;
        if (nucdata -> halflife > 5000.)
            ndec = 0;
        strcpy(formatstr,"%#6. f");
        formatstr[4] = ndec + 0x30;
        sprintf(buf,formatstr,nucdata -> halflife);
        strcpy(strng,"HL :");
        strcat(strng,buf);
        strcat(strng,timeunit[nucdata -> hlunit - 1]);
        display_text(16,32,strng,0,MEDIUM,NORMAL);
             
    }




static void	get_hl(short *taunit,float *tau);

static bool add_nuclide_data(float *tau, short *taunit,char *nucname)
	{
		NUCDATA tempnuc;
		bool match;
		short i;
		char name[8];
        short num;
        short maxnuc;

        maxnuc = NuclideData_getNumOfNucs();

		do
		{

            //enter nuclide name
            strcpy(nucname,"      ");
            //display_text(8,12,"TYPE IN",0,MEDIUM,NORMAL);
            //display_text(8,22,"NUCLIDE NAME",0,MEDIUM,NORMAL);
            display_medium_message(ADD_NUCLIDE_1,12,0,NORMAL);
            display_medium_message(ADD_NUCLIDE_2,22,0,NORMAL);
    
            num = inpalnum(8,42,nucname,6,TRUE);
            
			if (num == 0 || nucname[0] == '\0' || nucname[0] == ' ')
			{
				erase_screen();
				return FALSE;
		   }

		   // does it already exist? 
			match = FALSE;
			for(i = 0; i < maxnuc; i++)
			{
                NuclideData_getName(i,name);
				if (name[1] != ' ' && name[1] > 'Z')
					name[1] -= 0x20;		//change lower case to upper case 
				if (strncmp(name,nucname,6) == 0)
				{
					match = TRUE;
					break;
				}
			}

			// now search user nuclides 
			if (!match)
			{
				for(i = 0; i < 10; i++)
				{
                    //EE_READ(usernucl[i],(uchar *)&tempnuc);
					if (tempnuc.hlunit == -1)   // deleted 
						continue;
                if (tempnuc.name[1] != ' ' && tempnuc.name[1] > 'Z')
                    tempnuc.name[1] -= 0x20;        //change lower case to upper case 

					if (strncmp(&tempnuc.name[0],nucname,6) == 0)
					{
						match = TRUE;
						break;
					}
				}
			}

			if (match)
			{
				beep();
				erase_screen();
				//display_text(20,20,"NUCLIDE",0,MEDIUM,NORMAL);
				//display_text(0,35,"ALREADY EXISTS",0,MEDIUM,NORMAL);
                display_medium_message(ADD_NUCLIDE_3,20,0,NORMAL);
                display_medium_message(ADD_NUCLIDE_4,35,0,NORMAL);                
				contmsg();
                if(home_set())
                    return FALSE;
				strcpy(nucname,"      ");
				erase_screen();
			}
		}while(match);

		get_hl(taunit,tau);
        if(home_set())
            return FALSE;
		erase_screen();
        return TRUE;
	}

	


static void	get_hl(short *taunit,float *tau)
	{

		erase_screen();
		
        //display_text(0,0,"HALF-LIFE:",0,MEDIUM,NORMAL);
        display_medium_message(ADD_NUCLIDE_5,0,0,NORMAL);
        val_unit_msg();
		*tau = inpfl_u(8,27,5,taunit,0);
		++*taunit;
	}




/*
  delete_nuclide:
  user chooses nuclide to delete from list of user input
			nuclides
			data is displayed and user confirms deletetion
			hlunit set to 0xff to indicate empty
			if assigned to user key, user key set to none
			if new Cal #, free space

  */   

    void delete_nuclide(void)
	{
		short i;
        short imax;
		short none = -1;
		short index;
		short nuc_index;
		short ukey;
		NUCDATA nucdata;
        

		short ch_type;

		ch_type = chamber_type(current.main_chamber);
		do
		{
			erase_screen();
			specify_nuclide_msg();

			nuc_index = get_nuclide_index(FALSE);
			erase_screen();
			switch (nuc_index)
			{
			case -1:
				no_nuclide_msg();
				break;

			case -2:
				specify_further_msg();
				break;

			case -3:
				return;

			default:
				break;
			}
            if(home_set())
                return;
		}while(nuc_index < 0);


		NuclideData_getNuclide(nuc_index, &nucdata);

		if (nuc_index < USERNUC)
		{
			/* error -- not a user added nuclide */
			beep();
			//display_text(30,4,"ERROR",0,MEDIUM,REV);
			//display_text(0,24,"CANNOT DELETE",0,MEDIUM,NORMAL);
            display_medium_message(ADD_NUCLIDE_6,4,0,REV);
            display_medium_message(ADD_NUCLIDE_7,24,MEDIUM,NORMAL);
			display_text(30,40,&nucdata.name[0],0,MEDIUM,NORMAL);
			contmsg();
			return;
		}

        
        display_user_data(&nucdata);
		//display_text(8,52,"DELETE? Y OR N",0,MEDIUM,NORMAL);
        display_medium_message(ADD_NUCLIDE_8,52,0,NORMAL);
		if(yesorno())
		{
			// delete by setting hlunit to -1
			index = nuc_index - USERNUC;
            //EE_WRITE(usernucl[index].hlunit,(uchar *)&none);

			
            /* if a user key is assigned to this nuclide, make
				it NONE */
            imax = 5;
            if(ch_type == P_CHAMB || ch_type == C_CHAMB || ch_type == K_CHAMB)
                imax = 9;
			for (i = 0; i < imax; i++)
			{
                
                EE_READ(user_keys[0].keys[i],(uchar *)&ukey);
                if(ukey == nuc_index)
                    EE_WRITE(user_keys[0].keys[i],(uchar *)&none);
			}

            //delete counter user keys if has counter

            if (current.detector != DETECTOR_NONE)
            {
                for (i = 0; i < 5; i++)
                {
                    EE_READ(user_keys[1].keys[i],(uchar *)&ukey);
                    if(ukey == nuc_index)
                        EE_WRITE(user_keys[1].keys[i],(uchar *)&none); 
                }
            }
			/* if new cal #, free space */
			for (i = 0; i < MAX_NEW_CAL_OBSOLETE; i++)
			{
                //EE_READ(usercal[i].nuc_index,(uchar *)&index);
				if (index == nuc_index)
				{
                    //EE_WRITE(usercal[i].nuc_index,(uchar *)&none);
					break;
				}
			}
		}
	}





