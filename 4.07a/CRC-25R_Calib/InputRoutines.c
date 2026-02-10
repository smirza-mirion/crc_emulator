
/*********************************************************************
  MODULE:	Input Routines
  
  FILE:		InputRoutines.c

  DATE:		11/02/06

  ANALYSIS:	routines for inputting:
	inpflt:		input floating point number	, with range checking
				returns floating point value
	inpint:		input integer , with range checking
				returns integer value
    inpfl_u:    input combination of floating point number and unit        
	numstr_in:	input numeric string
				returns FALSE if empty string, else returns TRUE
	yesorno:	Y or N entered to question
				returns TRUE (Y), or FALSE (N)
	contmsg:	continue message
    key_pc_continue:  continue message with key press or PC command
	ackmsg:		acknowledge message
	
	These routines use _ for prompt
  *************************************************************************/
    		
#include "crc.h"
#include "screen.h"
#include "keyboard.h"
#include "message.h"
#include "uart.h"
#include <stdlib.h>  //for atof
#include <string.h>
#include <stdio.h>

 static void increment_xpos(short *xpos, short fontsize);
 static void decrement_xpos(short *xpos, short fontsize);
 static void prompt(short xpos, short ypos, short fontsize, bool on_flag);
 static void err_format(char *str,float val);


extern FONT fonts[];
extern volatile bool AmuletPCQC_Abort;
static erase_input_area(short xpos0, short ypos0, short fontsize, short plane,
                        short length);

//input floating point number 
float inpflt(short xpos0, short ypos0,short fontsize, short plane,
	short length,float minval, float maxval)
	{

		bool end_flag;
		short num;
		char ch;
		char ch1;
		char strng[20];
		float val;
        bool ok;
		short xpos;
		short ypos;
		bool dp_flag;
        char err_str[20];
        char fstr[6];

        // blank screen area for input 
        erase_input_area(xpos0,ypos0,fontsize,plane,length);

		num = 0;
		dp_flag = FALSE;
		xpos = xpos0;
		ypos = ypos0;
		prompt(xpos,ypos,fontsize,TRUE);


		for(;;) // forever loop 
		{
			ch = keyin();

            if(home_set())
                return 0.;
            
			ch1 = -1;
			switch(ch)
			{
			case CLR:
				if (num == 0)
					beep();
				else
				{
					end_flag = num == length;
					--num;
					if (!end_flag)
					{
					    prompt(xpos,ypos,fontsize,FALSE);
						decrement_xpos(&xpos,fontsize);
						prompt(xpos,ypos,fontsize,TRUE);
					}	
                    display_text(xpos, ypos," ",0,fontsize,NORMAL);
					// if erasing decimal pt, reset dp_flag 
					if(strng[num] == '.')
						dp_flag = FALSE;
					strng[num] = '\0';

                 }
				break;

			case ENTER:
				ok = TRUE;
				switch(num)
				{
				case 0:
					// must have at least 1 character 
					ok = FALSE;
					break;
				case 1:
					// can't have only  .
					if( strng[0] == '.')
						ok = FALSE;
					break;
				default:
					break;

				}
				if(!ok)
				{
					beep();
					break;
				}
				prompt(xpos,ypos,fontsize,FALSE);
				val =  (float)atof(strng);
				if(val >= minval && val <= maxval)
					return(val);

				// outside of limits 
				// display error message with min & max
                read_screen(plane);
                beep();
                erase_screen();
                //ERROR
                display_medium_message(INPUT_1,2,0,REV);
                //Entry:
                display_medium_message(INPUT_2,16,0,NORMAL);
                err_format(fstr,val);
                sprintf(err_str,fstr,val);
                display_text(72,16,err_str,0,MEDIUM,NORMAL);
                //Minimum: 
                display_medium_message(INPUT_3,28,0,NORMAL);
                err_format(fstr,minval);
                sprintf(err_str,fstr,minval);
                display_text(72,28,err_str,0,MEDIUM,NORMAL);
                //Maximum:
                display_medium_message(INPUT_4,40,0,NORMAL);
                err_format(fstr,maxval);
                sprintf(err_str,fstr,maxval);
                display_text(72,40,err_str,0,MEDIUM,NORMAL);
                contmsg();
                if(home_set())
                    return 0.;
                write_screen(plane);

				// user re-enters 
                erase_input_area(xpos0,ypos0,fontsize,plane,length);
				num = 0;
				xpos = xpos0;
				prompt(xpos,ypos,fontsize,TRUE);
				dp_flag = FALSE;
				break;


			case DP:
				//  only 1 decimal point
				if(dp_flag || num == length)
					beep(); 
				else
				{
					ch1 = '.';
					dp_flag = TRUE;
				}
				break;

			case ONE:
			case TWO:
			case THREE:
			case FOUR:
			case FIVE:
			case SIX:
			case SEVEN:
			case EIGHT:
			case NINE:
			case ZERO:
				if (num == length)
					beep();	   
				else
				{
					// got number 
					ch1 = ch + '0';
				}
				break;
			default:
				beep();
				break;
			}
			
			if(ch1 != -1)
			{
				strng[num++] = ch1;
				strng[num] = '\0';
				end_flag = num == length;
				display_text(xpos,ypos,&strng[num - 1],0,fontsize,NORMAL);
				if (!end_flag)
				{
					prompt(xpos,ypos,fontsize,FALSE);			
     	            increment_xpos(&xpos,fontsize);
     	            prompt(xpos,ypos,fontsize,TRUE);
     	        }    
				

			}
		
		}  // end forever loop 

	}

    //formatting for error message for inpflt
    static void err_format(char *str,float val)
    {
        strcpy(str,"%5.0f");
        if(val < 1000.)
            strcpy(str,"%5.1f");
        if(val < 100.)
            strcpy(str,"%5.2f");
        if(val < 10.)
            strcpy(str,"%5.3f");
        if(val < 1.)
            strcpy(str,"%5.4f");

    }   

/* input numerical string
	disp_flag  don't display if password
	er_length		erase length -- different from
				length only if date or time
	start_key can be NO_START_KEY if re-entered from
				date or time input
   md_flag  for multipy / divide keys for cal number input
 */

#define MULT_KEY UP
#define DIV_KEY DOWN



    bool numstr_in(short xpos0, short ypos0,char *strng,
                   short fontsize, short plane,
                   short length,short er_length,
                   bool disp_flag, bool md_flag)
    {
    
        bool end_flag;
        short num;
        char ch;
        bool valid;
        short xpos;
        short ypos;
        bool mul_div_flag;
        char disp_str[2];
        char ch1;
        
    
        mul_div_flag = FALSE;
        disp_str[1] = '\0';
        num = 0;
        xpos = xpos0;
        ypos = ypos0;
    
        // blank screen area for input
        erase_input_area(xpos0,ypos0,fontsize,plane,length);
        prompt(xpos,ypos,fontsize,TRUE);
        
        for(;;)	
        {
            
            ch = keyin();
    
            if(home_set())
                return FALSE;
            
            valid = FALSE;
            switch(ch)
            {
            case CLR:
                if(num == 0)
                    beep();
    
                else
                {
                    end_flag = num == length;
                    if(!end_flag)
                    {
                         prompt(xpos,ypos,fontsize,FALSE);
                         decrement_xpos(&xpos,fontsize);
                         prompt(xpos,ypos,fontsize,TRUE);
                     }    
                     display_text(xpos,ypos," ",0,fontsize,NORMAL);
                       
                    --num;
                    //if erasing DIV or MULT, reset mul_div_flag
                    if ((strng[num] == MULT) || (strng[num] == DIVSIGN))
                        mul_div_flag = FALSE;
                    strng[num] = '\0';
                }
                break;
    
            case ENTER:
                end_flag = num == length;
                prompt(xpos,ypos,fontsize,FALSE);
                return (num > 0);
    
            case MULT_KEY:
            case DIV_KEY:
                if (!md_flag)
                {
                    beep();
                    break;
                }
    
                   
                //can't be 1st character, can only have 1 or them
                if (num == 0 || num == length || mul_div_flag)
                {
                    beep();
                    break;
                }
                if (ch == MULT_KEY)
                {
                    disp_str[0] = 'X';
                    strng[num++] = MULT;
                }
                if (ch == DIV_KEY)
                {
                    disp_str[0] = MED_DIV;
                    strng[num++] = DIVSIGN;
                }
                
                mul_div_flag = TRUE;
                valid = TRUE;
                break;
            case ONE:
            case TWO:
            case THREE:
            case FOUR:
            case FIVE:
            case SIX:
            case SEVEN:
            case EIGHT:
            case NINE:
            case ZERO:
                if(num == length)
                    beep();
                
                else
                {
                    // got number 
                    valid = TRUE;
                    ch1 = ch + '0';  
                    strng[num++] = ch1;
                    disp_str[0] = ch1;
                }
                break;
    
            default:
                beep();
                break;
    
            }
            if (valid)
            {
                strng[num] = '\0';
                end_flag = num == length;
                if(disp_flag)
                    display_text(xpos,ypos,disp_str,0,fontsize,NORMAL);
                else
                   display_text(xpos,ypos,"*",0,fontsize,NORMAL);
                       
                if(!end_flag)
                {
                     prompt(xpos,ypos,fontsize,FALSE);
                     increment_xpos(&xpos,fontsize);
                     prompt(xpos,ypos,fontsize,TRUE);
                }     
            }
        }  // end forever loop 
    
    }
    
    bool yesorno(void)
    {
        char ch;
        for(;;)
        {
    
            ch = keyin();

            if(home_set())
                return FALSE;
            
            switch (ch)
            {
            case NINE:   /*yes*/
                return(TRUE);
            case SIX:  /*no*/
                return(FALSE);
            default:
                beep();
                break;
            }
        }
     }
    
    
    void contmsg(void)
    {
        //Any Key to Continue
        erase_text_line(56,SMALL,0);
        display_small_message(CONTINUE,56,0,NORMAL);
        (void)keyin();
        erase_text_line(56,SMALL,0);
    }

  //continue via any key press or command from PC
#include "remote.h"
    extern CHAMBER chamber[];
    extern CURRENT current;
    
    void key_pc_continue(void)
    {

        char key;
        char key0;
        short ret;
        short ch_num = current.main_chamber;

		ret = -1;
		
        set_nokey();
        key = NOKEY;
        clear_pc_ret();

        if(chamber[ch_num].control == CONTROL_MAIN)
            //Any Key to Continue
            display_small_message(CONTINUE,56,0,NORMAL);

        for(;;)
        {

            key0 = getkey(); //looks for PC communications

            if(AmuletPCQC_Abort) return;
            if(home_set()) return;
            
            if(chamber[ch_num].control == CONTROL_MAIN)
                key = key0;
            else
                ret = get_pc_ret();
            
            if(key != NOKEY || ret == PC_CONTINUE)
                break;
        }    

        if(chamber[ch_num].control == CONTROL_MAIN)
            erase_text_line(56,SMALL,0);
    }    
    
    
    void ackmsg(void)
    {
        display_text(0,56,"Acknowledge by ID:",0,SMALL,NORMAL);
        enter_idnum(18,7);
        erase_text_line(56,SMALL,0);
    }


	/* user enters digit, not displayed
	   only accepted if between min and max
	   used in choosing from list */
	short choose(short min, short max)
	{
		char ch;
		short num;
		
		for(;;)
		{
			
			ch = keyin();

            if(home_set())
                return 0;
            
 			switch(ch)
			{
 			case ONE:
			case TWO:
			case THREE:
			case FOUR:
			case FIVE:
			case SIX:
			case SEVEN:
			case EIGHT:
			case NINE:
			case ZERO:
				num = (short)ch;

				if (num < min || num > max)
					beep();
				else
					return (num);
				break;
			default:
				beep();
				break;
			}
		}
   }





 static void increment_xpos(short *xpos, short fontsize)
 {
	 *xpos += fonts[fontsize].delx;
 }

 static void decrement_xpos(short *xpos, short fontsize)
 {
	 *xpos -= fonts[fontsize].delx;
 }

static const char *unit_str[] =
	{
		"MIN ",
		"HOUR",
		"DAY ",
		"YEAR",
		"$Ci ",     //uCi
		"mCi ",		//mCi
		"MBq ",		//MBq
		"$Ci ",     //uCi
		"mCi ",		//mCi
		" Ci ",		//Ci
		"MBq ",		//MBq
		"GBq ",		//GBq
};

	
	float inpfl_u(short xpos0,short ypos0,short length,short *unit,
				  short type)
	{

		short i,num;
		short n_unit,min_unit,max_unit;
		char ch1;
		short x_unit;
		short xpos;
		bool u_flag,d_flag;
		float val;
		bool end_flag;
		char ch;
		char strng[20];
        char str1[2];

        str1[1] = '\0';

		xpos = xpos0;
		num = 0;
		d_flag = FALSE;
		for (i = 0; i <= length; i++)
			strng[i] = ' ';

		switch(type)
		{
		case 0: //time unit
			min_unit = 0;
			max_unit = 3;
			n_unit = 0;
			break;
		case 1: //uCi,mCi,MBq
			min_unit = 4;
			max_unit = 6;
			n_unit = 4;

			// Locked: CI
			//if (current.system == BQ)
			//	n_unit = 6;
			break;
		case 2: //uCi,mCi,Ci,MBq,GBq
			min_unit = 7;
			max_unit = 11;
			n_unit = 7;
			// Locked: CI
			//if (current.system == BQ)
			//	n_unit = 10;
			break;
        case 3: //input activity without changing system
        case 4:
        	// Locked: CI
            //switch(current.system)
        	switch(CI)
            {
            case CI:
                min_unit = 7;
                max_unit = 9;
                if(type == 3)
                    n_unit = 7;     //start with uCi
                else  //case 4
                    n_unit = 8;     //start with mCi
                break;
            case BQ:
                min_unit = 10;
                max_unit = 11;
                n_unit = 10;
                break;
            }
            break;
		}
		prompt(xpos,ypos0,MEDIUM,TRUE);

		val = -1.;
		x_unit = xpos0 + 64;
		display_text(x_unit,ypos0,(char*)&unit_str[n_unit][0],0,MEDIUM,NORMAL);
		u_flag = FALSE;

		for(;;)   /* forever loop */
		{
			ch = keyin();

            if(home_set())
                return 0.;
            
			ch1 = -1;
			switch(ch)
			{
			case DOWN:
				if (u_flag)  /* DOWN */
				{
					if (n_unit == min_unit)
						n_unit = max_unit;
					else
						--n_unit;
					display_text(x_unit,ypos0,(char *)unit_str[n_unit],
								  0,MEDIUM,NORMAL);
				}
				/* RIGHT arrow to move to unit */
				else
				{
					val = atof(strng);
					if (val <= 0.)
						beep();
					else
					{
						end_flag = num == length;
						prompt(xpos,ypos0,MEDIUM,FALSE);
						display_text(x_unit,ypos0,(char*)unit_str[n_unit],
									 0,MEDIUM,NORMAL);
						prompt(x_unit,ypos0,MEDIUM,TRUE);			 
						u_flag = TRUE;
					}
				}
				break;

			case CLR:
				if (num == 0)
					beep();
				else
				{
					end_flag = num == length;
					if (u_flag)
					{
					    prompt(x_unit,ypos0,MEDIUM,FALSE);
						u_flag = FALSE;
						if (end_flag)
						    xpos = xpos0 + 8 * (num - 1);
						else
                            xpos = xpos0 + 8 * num;    
						prompt(xpos,ypos0,MEDIUM,TRUE);
					}

                    --num;
                    if (!end_flag)
					{
					    prompt(xpos,ypos0,MEDIUM,FALSE);
         	            decrement_xpos(&xpos,MEDIUM);
         	            prompt(xpos,ypos0,MEDIUM,TRUE);
         	        }    
                    if (strng[num] == '.')
					    d_flag = FALSE;
					display_text(xpos,ypos0," ",0,MEDIUM,NORMAL);    
					strng[num] = '\0';
				}
				break;

			case OK:
				val = atof(strng);
				if (val <= 0.)
					beep();
				else
				{
					*unit = n_unit - min_unit;
					return val;
				}
				break;

			case UP:
				if (u_flag)  /* UP*/
				{
					if (n_unit == max_unit)
						n_unit = min_unit;
					else
						++n_unit;
					display_text(x_unit,ypos0,(char *)unit_str[n_unit],
								  0,MEDIUM,NORMAL);
				}

				/* DP */
				else
				{
					if(d_flag || num == length)
						beep();
					else
					{
						d_flag = TRUE;
						ch1 = '.';
					}
				}
				break;

			case ZERO:
			case ONE:
			case TWO:
			case THREE:
			case FOUR:
			case FIVE:
			case SIX:
			case SEVEN:
			case EIGHT:
			case NINE:
				if (num == length)
					beep();
				else
				{
					if (u_flag)
						beep();
					else
						ch1 = ch + '0'; 
				}
				break;

			default:
				beep();
				break;
			}

			if (ch1 != -1)
			{
				/* got number or dp */
				strng[num] = ch1;
				++num;
				end_flag = num == length;
                str1[0] = ch1;
				display_text(xpos,ypos0,str1,0,MEDIUM,NORMAL);
				if (!end_flag)
				{
                    prompt(xpos,ypos0,MEDIUM,FALSE);   
					increment_xpos(&xpos,MEDIUM);
					prompt(xpos,ypos0,MEDIUM,TRUE);
				}	
			}
		} /* end of forever loop */

	}

	/* alpha-numeric string input 
	   returns number of characters*/
	static char alpha[6];
    static const char *alstring[] = 
    {
        "0QZ ",
        "1---",
        "2ABC",
        "3DEF",
        "4GHI",
        "5JKL",
        "6MNO",
        "7PRS",
        "8TUV",
        "9WXY",
    };


    //enter alpha numeric string
    //if alflag set, used for nuclide name entry
    short inpalnum(short xpos0,short ypos0,char *strng,short length0,bool alflag)  
	{

		short numch;
		short xpos;
		short ypos;
		char  ch;
		char ch1;
		short pos;
		short i;
		char prev_ch;
		short key;
		char bl[2];
		bool clr_flag;
		short length = length0 - 1;
		bool start_flag;
        char str[2];
        bool new_key;

		pos = -1;
		prev_ch = 0x7f;  //none
		clr_flag = FALSE;
		
		strcpy(alpha,"   ");

		numch = -1;
		xpos = xpos0;
		ypos = ypos0;
		strcpy(bl," ");		
		
		for (i = 0; i < length0; i++)
			strng[i] = ' ';
        strng[length0] = '\0';

        str[1] = '\0';

        prompt(xpos,ypos,MEDIUM,TRUE);		
		start_flag = TRUE;    		
		for(;;)   /* forever loop */
		{
			ch = keyin();

            if(home_set())
                return 0;


			switch(ch)
			{
			case CLR:
				if (numch == -1)
					beep();
				else
				{
                    if(clr_flag) // more than 1 clear in a row
                    {
                         if(numch == 0)
                         {
                             beep();
                             break;
                         }
                        --numch;
                        prompt(xpos,ypos0,MEDIUM,FALSE);
                        decrement_xpos(&xpos,MEDIUM);
                        prompt(xpos,ypos0,MEDIUM,TRUE);
                    }
                    strng[numch] = '\0';

					display_text(xpos,ypos,bl,0,MEDIUM,NORMAL);   
					pos = -1;
                    if(alflag && numch < 2)
                        pos = 0;
					clr_flag = TRUE;
                    
				}
				break;

             case ENTER:
                 prompt(xpos,ypos,MEDIUM,FALSE);
                 return (numch + 1);


              case UP:  //repeat key
                 //can't use if no input yet or all input in
                 //or if after 1st 2 characters with alflag set
                  if (numch == -1 || numch == length || (alflag && numch >= 2))  
						beep();
                  else
                  {
	                   pos = -1;
                       if(alflag && numch < 2)
                           pos = 0;
				        ++numch;   //next character
                        prompt(xpos,ypos0,MEDIUM,FALSE);
				        increment_xpos(&xpos,MEDIUM); //move to next position
				        prompt(xpos,ypos0,MEDIUM,TRUE);
                        clr_flag = FALSE;
                        prev_ch = '^';
                    } 		        
					break;

                case DOWN:  //space key
                    //space can't be first character or last character                    
                    if((numch == -1 ||
                       ((numch == length) && !clr_flag)) )
                    {
                        beep();
                        break;
                    }    
                    if(alflag && numch != 0) //nuclide name: space can only be 2nd character
                    {
                        beep();
                        break;
                    }
                    if (!clr_flag)
                    {
                        ++numch;   //next character
                        if (!start_flag)
                        {
                            prompt(xpos,ypos,MEDIUM,FALSE);
                            increment_xpos(&xpos,MEDIUM); //move to next position
                            prompt(xpos,ypos,MEDIUM,TRUE);
                         }  
                    }    
                    clr_flag = FALSE;
                    prev_ch = ' ';
                    strng[numch] = ' ';
                    str[0] = ' ';
                    display_text(xpos,ypos,str,0,MEDIUM,NORMAL);     
                    break;
				case ZERO:
				case ONE:
				case TWO:
				case THREE:
				case FOUR:
				case FIVE:
				case SIX:
				case SEVEN:
				case EIGHT:
				case NINE:

                    if(alflag) 
                    {
                        if((numch == 4 && !clr_flag) ||
                           (ch == ONE && numch < 1))
                        {    
                            beep();
                            break;
                        }   
                    }
                        
                    new_key = TRUE;
                    if(ch == prev_ch) 
                    {    
                        new_key = FALSE;
                        if(alflag && numch >= 2)
                            new_key = TRUE;
                    }    
                    if(new_key)
				    {
				        if((numch == length) && !clr_flag)
				        {
				            beep();
                            break;    				        
				        }
                        pos = -1;
				        if (!clr_flag)
                        {
                            if(prev_ch != '^')
                            {    
                                ++numch;   //next character
                                if (!start_flag)
                                {
                                    prompt(xpos,ypos,MEDIUM,FALSE);
                                    increment_xpos(&xpos,MEDIUM); //move to next position
                                    prompt(xpos,ypos,MEDIUM,TRUE);
                                }
                            }
				        }    
						prev_ch = ch;	
                        key = (short)ch;		    
						//read in string for key
						memcpy(alpha,alstring[key],4);
						alpha[4] = '\0';
                        if(alflag && numch < 2)  
                            pos = 0;
				    }
                    clr_flag = FALSE;  //reset clear flag after key pressed
                    start_flag = FALSE;
                    if(alflag && numch >= 2)
                        pos = 0;
                    else
                    {    
                        ++pos;
                        if (pos == 4)
                        {    
                            pos = 0;
                            if(alflag)
                                pos = 1;
                        }
                    }

                    if(alflag && pos > 0 && key == 1)//for nuclide, 1 key only for number
                    {
                        beep();
                        break;
                    }
                    
					ch1 = alpha[pos];

					strng[numch] = ch1;
                    str[0] = ch1;
					display_text(xpos,ypos,str,0,MEDIUM,NORMAL);     
					break;

                case ISO1:  //Tc99m for m
                    if(alflag && numch >= 3)
                    {
                        if(numch > 4 ||
                           (numch == 4 && strng[4] == 'm'))  //only 1 m allowed
                            beep();
                        else
                        {    
                            if(!clr_flag)
                            {    
                                ++numch;   //next character
                                prompt(xpos,ypos0,MEDIUM,FALSE);
                                increment_xpos(&xpos,MEDIUM); //move to next position
                                prompt(xpos,ypos0,MEDIUM,TRUE);
                            }    
                            strng[numch] = 'm';
                            str[0] = 'm';
                            display_text(xpos,ypos,str,0,MEDIUM,NORMAL);
                        }    
                    }
                    else
                        beep();
                    clr_flag = FALSE;  //reset clear flag after key pressed
                    break;
                default:
					beep();
					break;
				}



			} /* end of forever loop */



		}


static void prompt(short xpos, short ypos, short fontsize, bool on_flag)
{
     short y1;
     
     y1 = ypos + fonts[fontsize].y;
     
     if(on_flag)
        display_text(xpos,y1,"_",0,fontsize,NORMAL);		
     else
        display_text(xpos,y1," ",0,fontsize,NORMAL);    
}



    short  time_date_input(short xpos0, short ypos0,char *strng,
                  short fontsize, short plane)
    {
    
        bool end_flag;
        short num;
        char ch;
        bool valid;
        short xpos;
        short ypos;
        char disp_str[2];
        char ch1;
        short length = 15;
        
    
        disp_str[1] = '\0';
        num = 0;
        xpos = xpos0;
        ypos = ypos0;
    
        // blank screen area for input 
        erase_input_area(xpos0,ypos0,fontsize,plane,length);
        prompt(xpos,ypos,fontsize,TRUE);
    
        for(;;) 
        {
            
            ch = keyin();
    
            if(home_set())
                return FALSE;
            
            valid = FALSE;
            switch(ch)
            {
            case CLR:
                if(num == 0)
                    beep();
    
                else
                {
                    end_flag = num == length;
                    if(!end_flag)
                    {
                         prompt(xpos,ypos,fontsize,FALSE);
                         decrement_xpos(&xpos,fontsize);
                         prompt(xpos,ypos,fontsize,TRUE);
                     }    
                     display_text(xpos,ypos," ",0,fontsize,NORMAL);
                       
                    --num;
                    strng[num] = '\0';
                }
                break;
    
            case ENTER:
                end_flag = num == length;
                prompt(xpos,ypos,fontsize,FALSE);
                return (num);
    
            case UP:
            case DOWN:
                //can't be 1st character
                if (num == 0 || num == length)
                {
                    beep();
                    break;
                }
                disp_str[0] = '/';
                strng[num++] = '/';
                
                valid = TRUE;
                break;
            case ONE:
            case TWO:
            case THREE:
            case FOUR:
            case FIVE:
            case SIX:
            case SEVEN:
            case EIGHT:
            case NINE:
            case ZERO:
                if(num == length)
                    beep();
                
                else
                {
                    // got number 
                    valid = TRUE;
                    ch1 = ch + '0';  
                    strng[num++] = ch1;
                    disp_str[0] = ch1;
                }
                break;
    
            default:
                beep();
                break;
    
            }
            if (valid)
            {
                strng[num] = '\0';
                end_flag = num == length;
                display_text(xpos,ypos,disp_str,0,fontsize,NORMAL);
                       
                if(!end_flag)
                {
                     prompt(xpos,ypos,fontsize,FALSE);
                     increment_xpos(&xpos,fontsize);
                     prompt(xpos,ypos,fontsize,TRUE);
                }     
            }
        }  // end forever loop 
    
    }

//input integer -- with starting character
short inpint(short xpos0, short ypos0,short fontsize, short plane,
    short length,short minval, short maxval,char ch0)
    {

        bool end_flag;
        short num;
        char ch;
        char strng[20];
        short val;
        short xpos;
        short ypos;
        char err_str[20];
        bool start_flag;
                
        // blank screen area for input 
        erase_input_area(xpos0,ypos0,fontsize,plane,length);

        num = 0;
        xpos = xpos0;
        ypos = ypos0;
        prompt(xpos,ypos,fontsize,TRUE);
        start_flag = TRUE;
        if(ch0 == '\0')
            start_flag = FALSE;

        for(;;) // forever loop 
        {
            if(start_flag)
            {
                ch = ch0;
                start_flag = FALSE;
            }
            else
                ch = keyin();

            if(home_set())
                return 0;

            switch(ch)
            {
            case CLR:
                if (num == 0)
                    beep();
                     
                else
                {
                    end_flag = (num == length);
                    --num;
                    if (!end_flag)
                    {
                        prompt(xpos,ypos,fontsize,FALSE);
                        decrement_xpos(&xpos,fontsize);
                        prompt(xpos,ypos,fontsize,TRUE);
                    }   
                    display_text(xpos,ypos," ",0,fontsize,NORMAL);
                    strng[num] = '\0';
                }
                break;

            case ENTER:
                if(num == 0)
                {
                    beep();
                    break;
                }

                prompt(xpos,ypos,fontsize,FALSE);
                val =  atoi(strng);
                if(val >= minval && val <= maxval)
                    return(val);

                // outside of limits 
                // display error message with min & max
                read_screen(plane);
                beep();
                erase_screen();
                //ERROR
                display_medium_message(INPUT_1,2,0,REV);
                //Entry:
                display_medium_message(INPUT_2,16,0,NORMAL);
                sprintf(err_str,"%d",val);
                display_text(72,16,err_str,0,MEDIUM,NORMAL);
                //Minimum:
                display_medium_message(INPUT_3,28,0,NORMAL);
                sprintf(err_str,"%d",minval);
                display_text(72,28,err_str,0,MEDIUM,NORMAL);
                //Maximum:
                display_medium_message(INPUT_4,40,0,NORMAL);
                sprintf(err_str,"%d",maxval);
                display_text(72,40,err_str,0,MEDIUM,NORMAL);
                contmsg();
                if(home_set())
                    return 0;
                write_screen(plane);
                
                // user re-enters 
                erase_text(xpos0,ypos0,plane,length,fontsize);
                num = 0;
                xpos = xpos0;
                prompt(xpos,ypos,fontsize,TRUE);
                break;


            case ONE:
            case TWO:
            case THREE:
            case FOUR:
            case FIVE:
            case SIX:
            case SEVEN:
            case EIGHT:
            case NINE:
            case ZERO:
                if (num == length)
                    beep();    
                else
                {
                    // got number 
                    strng[num++] = ch + '0';  
                    strng[num] = '\0';
                    end_flag = (num == length);
                    display_text(xpos,ypos,&strng[num - 1],0,fontsize,NORMAL);
                    if (!end_flag)
                    {
                        prompt(xpos,ypos,fontsize,FALSE);                   
                        increment_xpos(&xpos,fontsize);
                    }   
                    prompt(xpos,ypos,fontsize,TRUE);
                }
                break;
            default:
                beep();
                break;
            }

        }  // end forever loop 

    }


    static erase_input_area(short xpos0, short ypos0, short fontsize, short plane,
                            short length)
    {
        short xpos1;
        short ypos1;
                
        // blank screen area for input 
        xpos1 = xpos0 + length * fonts[fontsize].delx;
        ypos1 = ypos0 + fonts[fontsize].y;
        erase_box(xpos0,ypos0,xpos1,ypos1,plane);


    }    

