/**
 * \file
 * \details This file contains the measurement_screen. This function is a forever loop and is responsible for all foreground tasks.
 * \details The main responsibility is calculating the Activity measurement from the ionization chamber and sending this data to the screen.
 * \details Other foreground processes are:
 * \details 1) Communications with the Amulet Screen
 * \details 2) Communications with the PC
 * \details 3) Communications with the Printer
 */
/*********************************************************************
  MODULE:	Main Measurement Screen for CRC-25R

  FILE:		ActMeasScreen_R.c

  DATE:	    03/25/08

  ANALYSIS: 	Calibrator activity measurement and branching
  DESIGN:		waits for pc character or key press
				if keypress -- service it
				if no keypress -- service chamber and display
					new activity if chamber ready
				ticket printed if ENTER pressed

  CALLED BY:
		main routine

  *************************************************************************/
#include "crc.h"
#include "screen.h"
#include "i2c.h"
#include "keyboard.h"
#include "uart.h"
#include "qspi.h"
#include "pit.h"
#include "printer.h"
#include "chambfac.h"
#include "sl811.h"
#include "daily.h"
#include "sl811h.h"
#include "remote.h"
#include "amulet.h"
#include "message.h"
#include <string.h>

	extern CHAMBER chamber[];
    extern REMOTE remote[];
    extern MEASUREMENT measurement[];
	extern CURRENT current;
	extern volatile bool DisplayActivity_SendActivity;
	extern volatile bool DisplayActivity_SendActivity2;
	extern volatile bool AmuletPCQC_Abort;
    bool meas_screen;

    void verify_chamber_checksum(void);
    void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
    void send_to_amulet_string(uchar ucIndex, char message0[]);

char ActMeasScreen_date_time[32];

static void display_time(void);
static void display_time2(void);

/**
 * \details This function is executed at the end of the Entry point function (main). It contains the forever loop, which process the foreground processes
 * \returns The program never exits this function
 */
void measurement_screen(void){
	short ukey;
	char ch;
	char prtype;
	struct tm tm_dose;
	short ch_num;
	//short ch_type;
	bool start_flag;
	short nuc_index;
	bool dose_flag;
	bool here_flag = FALSE;
	char message[100];
	char titlestring[52], formatstring[104];

	start_flag = TRUE;
	erase_screen();
	meas_screen = TRUE;
	dose_flag = FALSE;

	ActMeasScreen_date_time[0] = 0;
	AmuletPCQC_Abort = FALSE;
	for(;;){  /* forever */
		if(home_set() && !here_flag) verify_chamber_checksum();

		clear_home();
		here_flag = FALSE;
		ch_num = current.main_chamber;

		if(remote[ch_num].mode == REMOTE_IN_PROGRESS_MODE) remote[ch_num].mode = REMOTE_ACTIVITY_MODE;

		if(current.num_chambers == 0){
			erase_screen();
			display_text(0,10,"NO CHAMBER",0,MEDIUM,NORMAL);
			while(1){
				display_time2();
				ch = getkey();
			}
			//ch = keyin();
			//switch(ch){
			//	case MENUBUT:
			//		main_menu();
			//		break;
			//}
                //continue;
		}

		ch_num = current.main_chamber;
		//ch_type = chamber_type(ch_num);
		if(start_flag){
			meas_screen = TRUE;
			erase_screen();
			set_display_time();
			set_nokey();
			if(chamber[ch_num].calkey){
				//set to 1st built-in button, Tc99m
				getiso(ch_num,0);
				chamber[ch_num].calkey = FALSE;
			}else{
				nuc_index = chamber[ch_num].nuc_index;
				set_nuclide_data(nuc_index, ch_num);
			}
		}

		//turn off start_flag
		start_flag = FALSE;

		// look for keyboard input
		ch = getkey();

		if(home_set()){
			here_flag = TRUE;
			if(input_time_set()){
                    start_flag = TRUE;
                    dose_flag = FALSE;
                    if(DisplayActivity_SendActivity) remote[ch_num].keys_active = TRUE;
                    else remote[ch_num].keys_active = FALSE;
			}else beep();
		}

		if(get_pc_ret() == PC_START_FLAG){
			start_flag = TRUE;
			//reset pc ret
			clear_pc_ret();
		}

		if(start_flag)  //start flag can be set via PC
			continue;   //top of forever

		switch(ch){
			case NOKEY:
				if(display_time_set()){
					display_time();
					clear_display_time();
				}
				break;  /* go to display */

			case MENUBUT:
				dose_flag = FALSE;
				meas_screen = FALSE;
				remote[ch_num].keys_active = FALSE;
				main_menu();
				start_flag = TRUE;
				remote[ch_num].keys_active = TRUE;
				break;

			case TESTBUT:
				dose_flag = FALSE;
				meas_screen = FALSE;
				remote[ch_num].keys_active = FALSE;
				tests();
				start_flag = TRUE;
                remote[ch_num].keys_active = TRUE;
                break;

			case BKGBUT:
				dose_flag = FALSE;
				meas_screen = FALSE;
				chamber[ch_num].control = CONTROL_MAIN;
				measure_bkg(0);
				start_flag = TRUE;
				break;

			/* system button pressed*/
			case SYSBUT:
				//sysbut();
				break;

            case ISO1:
            case ISO2:
            case ISO3:
            case ISO4:
            case ISO5:
            case ISO6:
            case ISO7:
            case ISO8:
                dose_flag = FALSE;
                getiso(ch_num, ch - ISO1);
                break;

            case USER1:
            case USER2:
            case USER3:
            case USER4:
            case USER5:
                dose_flag = FALSE;
                ukey = ch - USER1;
                getuser(ch_num, ukey);
                break;

            case SOURCESBUT:
                dose_flag = FALSE;
                sources_key();
                break;

            case UP:
                upbut();
                break;

			case DOWN:
                downbut();
				break;

			case CALBUT:
                dose_flag = FALSE;
                remote[ch_num].keys_active = FALSE;
				calnum_but();
                remote[ch_num].keys_active = TRUE;
				break;

			case NUCBUT:
                remote[ch_num].keys_active = FALSE;
                meas_screen = FALSE;
				nucbut();
                remote[ch_num].keys_active = TRUE;
				break;

			case TIMEBUT:
                if (chamber[ch_num].calkey){  /*cal # input*/
                	beep();
                    break;
                }
                if(dose_flag){
                    remote[ch_num].keys_active = FALSE;
                    dose_table();
                    remote[ch_num].keys_active = TRUE;
                    dose_flag = FALSE;
                    start_flag = TRUE;
                }else{
                    prompt_date_time( );
                    dose_flag = TRUE;
                    remote[ch_num].keys_active = FALSE;
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
				dokey(ch);
				break;

            case CLR:
				keybuf_bsp();
				break;

            case DET_KEY:
            	if(current.num_chambers == 1) beep();
            	else{
            		if(select_chamber()) ch_num = current.main_chamber;
            	}
            	break;

			case OK:
				//printing
				if(!input_time_set()){
					remote[ch_num].keys_active = FALSE;
					set_idnum();
					EE_READ(print[1],(uchar *)&prtype);

					if(prtype == PR_TICKET){
						if(prticket(1)) prticket(2);
                     }else prline();

					id_init();
					remote[ch_num].keys_active = TRUE;
					break;
				}

                //time set
				if(decode_time_date(TRUE,&tm_dose)){
                    measurement[ch_num].future.dosetime = mk_time(&tm_dose);
                    erase_lines(Y_FUT_TIME,64 - Y_FUT_TIME,0);
				}else{ //invalid date/time
                    beep();
                    measurement[ch_num].future.dosetime = NO_TIME;
                    erase_lines(Y_FUT_TIME,64 - Y_FUT_TIME,0);
                    disp_cal(measurement[ch_num].response);
				}
				id_init();
				break;
		}  /* end of main switch */

		if(!start_flag){
			//if chamber not connected, display error message
			if(!chamber[ch_num].connected_flag){
				//sprintf(message, "Chamber #%d has lost connection.\nPlease turn off calibrator and reattach chamber.", ch_num+1);
				//Amulet_DisplayError("Chamber Error", message, FALSE);
				get_amulet_message(L_CHAMBER_ERROR, titlestring);    // "Chamber Error"
				get_amulet_message(L_CHAMBER_LOST_CONNECTION, formatstring);    // "Chamber #%d has lost connection.\nPlease turn off calibrator and reattach chamber."
				sprintf(message, formatstring, ch_num+1);
				Amulet_DisplayError(titlestring, message, FALSE);
				errchamb(ch_num);
				main_menu();
				start_flag = TRUE;
				continue;
			}

			//if chamber overflow -- display overflow
			if(measurement[ch_num].over_flag){
				overflow(ch_num);
				continue;
			}

			//if display ready , display activity
			if(measurement[ch_num].display_flag){
				activity_to_screen();
				display_time();
				measurement[ch_num].display_flag = FALSE;
			}
		}
	} /* end of forever */
}

extern time_t prev_clock_time;
extern time_t clock_time;

static void display_time(void){
	char t_str[9];
	char ascstr[26];
	char date_time[35];

	//display time only if it has changed
	if(clock_time != prev_clock_time || display_time_set()){
		if(DisplayActivity_SendActivity){
			dateout_language(ascstr, &clock_time, 4);
			timeoutsec(t_str, &clock_time);
			display_text(X_TIME, Y_TIME, t_str, 1, MEDIUM,NORMAL);
			strcpy(date_time, ascstr);
			strcat(date_time, " ");
			strcat(date_time, t_str);
			send_to_amulet_string(2, date_time);
			prev_clock_time = clock_time;
			if(lowBattery1) send_amulet_message(L_LOW_BATTERY, 13);    // "Low Battery"
			else send_to_amulet_string(13, "");
			lowBattery1 = FALSE;
		}

		if(DisplayActivity_SendActivity2){
			dateout_language(ascstr, &clock_time, 4);
			timeoutsec(t_str, &clock_time);
			display_text(X_TIME, Y_TIME, t_str, 1, MEDIUM,NORMAL);
			strcpy(date_time, ascstr);
			strcat(date_time, " ");
			strcat(date_time, t_str);
			send_to_amulet_string(2, date_time);
			prev_clock_time = clock_time;
			if(lowBattery2) send_amulet_message(L_LOW_BATTERY, 13);    // "Low Battery"
			else send_to_amulet_string(13, "");
			lowBattery2 = FALSE;
		}
	}
}

static void display_time2(void){
	char t_str[9];
	char ascstr[26];
	char date_time[35];

	//display time only if it has changed
	if(clock_time != prev_clock_time){
		dateout_language(ascstr, &clock_time, 4);
		timeoutsec(t_str, &clock_time);
		strcpy(date_time, ascstr);
		strcat(date_time, " ");
		strcat(date_time, t_str);

		if(strcmp(ActMeasScreen_date_time, date_time) != 0){
			if(DisplayActivity_SendActivity2){
				send_to_amulet_string(2, date_time);
				if(lowBattery3) send_amulet_message(L_LOW_BATTERY, 13);    // "Low Battery"
				else send_to_amulet_string(13, "");
				lowBattery3 = FALSE;
				strcpy(ActMeasScreen_date_time, date_time);
			}
		}
		prev_clock_time = clock_time;
	}
}

bool get_meas_screen(void){
	return meas_screen ;
}

void set_meas_screen(bool flag){
	meas_screen = flag;
}
