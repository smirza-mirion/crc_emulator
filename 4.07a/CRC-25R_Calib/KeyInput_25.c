/*********************************************************************
  MODULE:	KEYBOAD INPUT for CRC-25 family

  FILE:		KeyInput_25.c

  DATE:		09/18/06

  ANALYSIS:  getkey: finds out if key available & returns key if available
			 translate key: translates hardware key to hardware
				independent value
			 keyin: waits for key to be pressed; returns key pressed
             set_nokey:  sets key to NOKEY
             setkey:     sets key to given character

    *************************************************************************/
#include "crc.h"
#include "coldfire.h"
#include "keydef.h"
#include "keyboard.h"
#include "cs.h"
#include "pit.h"
#include "qspi.h"
#include "uart.h"
#include "sl811s.h"
#include "printer.h"
#include "amulet.h"
#include "lowlevelsnoop.h"
#include "i2c.h"
#include <string.h>

static char translate_key(void);
static void do_click(void);
static bool getkey0(void);
void service_remote_request(void);
void VolumeMirrorUpdate(void);
void UsbUpdate_ReceiveFilePacket(void);
void FileTransfer_UploadFile(void);
static void service_clock(void);

//volatile bool watchdogswitch = TRUE;

static char key;
static bool home_ret;
extern CURRENT current;
extern ushort g_Init_USB_PCL;
extern time_t clock_time;

static char today_clock;

    // if key pressed, translates key & returns it
    // else, returns NOKEY
    char getkey(void)
	{
		char ch;

		if (!getkey0())
			return NOKEY;

		/* key available */
        ch = translate_key();

        /*if(ch==USER1){
        	//set_touch_power(TRUE);
        	PushUart2Tx(0x13);
        	PushUart2Tx(0xFF);
        	PushUart2Tx(0xFF);
        	PushUart2Tx(0xFF);
        	PushUart2Tx(0x13);
        	PushUart2Tx(0);
        	cf.uarts[2].ucr = UART_UCR_TX_ENABLED;
        	key = NOKEY;
			return NOKEY;
        }

        if(ch==USER2){
        	set_touch_power(FALSE);
        	key = NOKEY;
			return NOKEY;
        }

        if(ch==USER3){
        	set_touch_reset(TRUE);
        	key = NOKEY;
			return NOKEY;
        }

        if(ch==USER4){
        	set_touch_reset(FALSE);
        	key = NOKEY;
			return NOKEY;
        }

        if(ch==ISO1){
        	set_touch_program(TRUE);
        	key = NOKEY;
			return NOKEY;
        }

        if(ch==ISO2){
        	set_touch_program(FALSE);
        	key = NOKEY;
			return NOKEY;
        } */

		key = NOKEY;
        if(ch == HOMEKEY)
            home_ret = TRUE;
		return (ch);
	}

	//translation of key into hardware independent value
    static char translate_key(void)
	{
        char ch;

        ch = 0;
        switch(key)
		{
		case OK_KEY:
            ch = OK;
            break;
		case CLR_KEY:
            ch = CLR;
            break;
		case UP_KEY:
            ch = UP;
            break;
		case DOWN_KEY:
            ch = DOWN;
            break;
		case ISO1_KEY:
            ch = ISO1;
            break;
		case ISO2_KEY:
            ch = ISO2;
            break;
		case ISO3_KEY:
            ch = ISO3;
            break;
		case ISO4_KEY:
            ch = ISO4;
            break;
		case ISO5_KEY:
            ch = ISO5;
            break;
		case ISO6_KEY:
            ch = ISO6;
            break;
		case ISO7_KEY:
            ch = ISO7;
            break;
		case ISO8_KEY:
            ch = ISO8;
            break;
        case SOURCES_KEY:
            ch = SOURCESBUT;
            break;

		case USER1_KEY:
            ch = USER1;
            break;
		case USER2_KEY:
            ch = USER2;
            break;
		case USER3_KEY:
            ch = USER3;
            break;
		case USER4_KEY:
            ch = USER4;
            break;
        case USER5_KEY:
            ch = USER5;
            break;

		case CALBUT_KEY:
            ch = CALBUT;
            break;
		case NUCBUT_KEY:
            ch = NUCBUT;
            break;

		case TIME_KEY:
            ch = TIMEBUT;
            break;
		case MENU_KEY:
            ch = MENUBUT;
            break;
        case TEST_KEY:
            ch = TESTBUT;
            break;
        case SYSBUT_KEY:
            ch = SYSBUT;
            break;
		case BKGBUT_KEY:
            ch = BKGBUT;
            break;

		case ZERO_KEY:
            ch = ZERO;
            break;
		case ONE_KEY:
            ch = ONE;
            break;
		case TWO_KEY:
            ch = TWO;
            break;
		case THREE_KEY:
            ch = THREE;
            break;
		case FOUR_KEY:
            ch = FOUR;
            break;
		case FIVE_KEY:
            ch = FIVE;
            break;
		case SIX_KEY:
            ch = SIX;
            break;
		case SEVEN_KEY:
            ch = SEVEN;
            break;
		case EIGHT_KEY:
            ch = EIGHT;
            break;
		case NINE_KEY:
            ch = NINE;
            break;

        case DETECTOR_KEY:
            ch = DET_KEY;
            break;
		case DISPLAY_KEY:
            ch = DISP_KEY;
            break;
		case SYR_VIAL_KEY:
            ch = SVR_KEY;
            break;
        case START_STOP_KEY:
            ch = COUNT_KEY;
            break;

        case HOME_KEY:
            ch = HOMEKEY;
		}
        return ch;
	}



	//waits for key press
    //returns translated key
    char keyin(void)
	{
		char ch;

		key = NOKEY;

		//wait for key press
		do
		{

		} while(!getkey0());

		ch = translate_key();

        if(ch == HOMEKEY)
            home_ret = TRUE;

		key = NOKEY;
		return (ch);
	}

    //sets key to NOKEY
    void set_nokey(void)
    {
        key = NOKEY;
    }

    //set key to given character
    void setkey(char ch)
    {
        key = ch;
    }


    //returns FALSE is no key pressed
    //returns TRUE if  key was pressed

    //getkey0 is called for all key entry -- use for servicing chambers
    static bool getkey0(void)
    {
        //service the core watchdog
        service_watchdog();

        service_amulet();

        //service chambers & remotes
        if(get_adc_enabled())
        {
            get_measurements();
            service_remote_request();
        }

        service_pc();

        service_clock();

        if (key == NOKEY)
            return FALSE;
        else
            return TRUE;

    }

void pollroutines(void){
	service_watchdog();
}


//routine to scan the keyboard when timed interupt occurs
enum  scan_states
{
     NO_KEY_PRESSED,
     KEY_PRESSED,
     KEY_STAYED_PRESSED,
     KEY_DEPRESSED,
};


#define PRESS_BOUNCE_DEL    2
#define DEPRESS_BOUNCE_DEL  2


static uchar kbd_row = 1;
static uchar kbd_col;
static uchar last_kbd_read = 0;
static char scan_state = NO_KEY_PRESSED;
static char bounce;

static char key_code = 0xff;   //no pressed key

    void scan_keyboard(void)
    {
        char i;

        reset_timed_interrupt();

        switch (scan_state)
        {
        case NO_KEY_PRESSED:
            //write keyboard row to the keyboard
            write_keyboard(kbd_row);
            //read column value
            kbd_col = read_keyboard();
            if(kbd_col != 0) //pressed key(s) found for the row
            {
                for(i = 1; i < 0x80; i<<=1)
                {
                    last_kbd_read = kbd_col & i;
                    if(last_kbd_read != 0)
                    {
                        scan_state = KEY_PRESSED;
                        bounce = PRESS_BOUNCE_DEL;
                        break;
                    }
                }
            }
            else    //no pressed key for the row, do next row
            {
                kbd_row <<= 1;
                if(kbd_row == 0x20)  //only 5 rows
                   kbd_row = 1;
            }
            break;

        case KEY_PRESSED:
            //read the column again
            kbd_col = read_keyboard();

            //test against last value
            kbd_col &= last_kbd_read;
            if(kbd_col == 0)
            {
                //not the same
                scan_state = KEY_DEPRESSED;
                bounce = DEPRESS_BOUNCE_DEL;
            }
            else    //same
            {
                --bounce;
                if(bounce == 0)
                {
                    scan_state = KEY_STAYED_PRESSED;
                    switch (kbd_row)
                    {
                    case 0x01:
                        key_code = 0x0;
                        break;
                    case 0x02:
                        key_code = 0x10;
                        break;
                    case 0x04:
                        key_code = 0x20;
                        break;
                    case 0x08:
                        key_code = 0x30;
                        break;
                    case 0x10:
                        key_code = 0x40;
                        break;
                    }

                    switch(last_kbd_read)
                    {
                    case 0x01:
                        break;
                    case 0x02:
                        key_code |= 0x1;
                        break;
                    case 0x04:
                        key_code |= 0x2;
                        break;
                    case 0x08:
                        key_code |= 0x3;
                        break;
                    case 0x10:
                        key_code |= 0x4;
                        break;
                    case 0x20:
                        key_code |= 0x5;
                        break;
                    case 0x40:
                        key_code |= 0x6;
                        break;
                    case 0x80:
                        key_code |= 0x7;
                        break;
                    }
                }
            }
            break;

        case KEY_STAYED_PRESSED:
            //read column again
            kbd_col = read_keyboard();

            //compare with previous
            if((last_kbd_read & kbd_col) == 0)
            {
                scan_state = KEY_DEPRESSED;
                bounce = DEPRESS_BOUNCE_DEL;
            }
            break;

        case KEY_DEPRESSED:
            //read column again
            kbd_col = read_keyboard();

            kbd_col &= last_kbd_read;
            if(kbd_col != 0)
            {
                scan_state = KEY_PRESSED;
                bounce = PRESS_BOUNCE_DEL;
            }
            else
            {
                --bounce;
                if(bounce == 0) //got the key
                {
                    scan_state = NO_KEY_PRESSED;
                    if(key_code != NOKEY)
                    {
                        //assign key
                        key = key_code;
                        //service the screen saver
                        key_press_screen_save();
                        //reset key_code to No Key
                        key_code = NOKEY;
                        //generate click
                        do_click();
                    }

                }
            }
            break;
        }

    }




#define CLICK_TONE  1    //10ms
#define BEEP_TONE   15 //150 ms
    extern short tone_duration;
    static void do_click(void)
    {
        //tone_duration = CLICK_TONE;  //decremented in interrupt routine
        //start_tone();
    }

    void beep(void)
    {
        //tone_duration = BEEP_TONE;
        //start_tone();
    }

void beep_amulet(void) {
	VolumeMirrorUpdate();
	tone_duration = BEEP_TONE;
	start_tone();
}

void click_amulet(void){
	//tone_duration = CLICK_TONE;
	//start_tone();
}


    //routines to set and return value of home_ret
    //home_ret set to TRUE when HOME key pressed

//clears home by setting home_ret to FALSE;
    void clear_home(void)
    {
        home_ret = FALSE;
    }

    //returns value of home_ret
    bool home_set(void)
    {
        return(home_ret);
    }


    //service watchdog timer
    void service_watchdog(void)
    {
    	//if(watchdogswitch){
		cf.wtm.wsr = 0x5555;
		cf.wtm.wsr = 0xaaaa;
    	//}
    }

void service_pc(void){
	char serch;
	uchar usb_in[64];

	//service any data from PC via RS232
	if(current.pccomm == PC_COMM_RS232){
		// if character from PC, handle it
		if(is_pc_char_waiting()){
#ifndef TERMINAL
			serch = (char)(uart_getchar(FALSE,U_PC));
			if (serch == PC_START_CHAR) from_pc(TRUE);
			else from_pc(FALSE);
#else // #ifndef TERMINAL
			serch = (char)(uart_getchar(FALSE,U_PC));
			(void)uart_char_waiting(U_PC);
			from_terminal(serch);
#endif // #ifndef TERMINAL
		}
	}

	//service any data from PC via USB
	if(current.pccomm == PC_COMM_USB){
		//if data from USB, handle it
		if(current.usb_device_protocol == 0){
			if(is_usb_char_waiting()){
				USB_Get_Data(usb_in);
				serch = usb_in[0];
				if(serch == PC_START_CHAR) from_pc(TRUE);
				else from_pc(FALSE);
			}
		}else if(current.usb_device_protocol == 1){
			from_pc_cdc();
		}
	}

	if(g_Init_USB_PCL == 1){
		if(current.printer == USB_PRINTER || current.printer == USB_EPS_PRINTER){
			if (current.printer == USB_PRINTER) pcl_reset();
			if (current.printer == USB_EPS_PRINTER) eps_reset(TRUE);
		}
		g_Init_USB_PCL = 0;
	}


	if(current.pccomm == PC_COMM_USB_DEBUG_CHAMBER) SendLowLevelChamber();

	//if(current.pccomm == PC_COMM_USB_DEBUG_WELL) SendLowLevelWell();

	if(current.pccomm == PC_COMM_USB_UPDATE) UsbUpdate_ReceiveFilePacket();

	if(current.pccomm == PC_COMM_USB_UPLOAD_FILE) FileTransfer_UploadFile();
}

void set_today_clock(void){
	struct tm *set_tm_time;

	set_tm_time = gmtime(&clock_time);
	today_clock = (char)set_tm_time -> tm_mday;
	//printf("new clock_time, today_clock = %d %d\r\n",clock_time,today_clock);
}

static void service_clock(void){
	struct tm *serv_tm_time;
	char service_day;

	serv_tm_time = gmtime(&clock_time);
	service_day = (char)serv_tm_time -> tm_mday;

	if(service_day != today_clock){
#ifndef SIMULATE_DECAY_SOURCE
		//printf("old clock_time,service day, today: %d %d  %d\r\n",clock_time,service_day,today_clock);
		read_clock(&clock_time);
		low_clock_time = clock_time;
		//reset_minute_counter();
		reset_minute_counter_with_seconds();
		set_today_clock();
#endif
	}
}
