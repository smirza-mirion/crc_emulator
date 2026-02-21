/**
 * \file
 * \details This file handles calls from the Amulet Setup Screen
 */
#define PHASE_SETUP_PRE_INIT 0
#define PHASE_SETUP_WAIT 1
#define PHASE_SETUP_CI_OR_BQ 2
#define PHASE_SETUP_CI_ONLY 3
#define PHASE_SETUP_BQ_ONLY 4
#define PHASE_SETUP_NONE 5
#define PHASE_SETUP_USBHP 6
#define PHASE_SETUP_USBEPSON 7
#define PHASE_SETUP_COMSLIP 8
#define PHASE_SETUP_COMROLL 9
#define PHASE_SETUP_COMOKITICKET 10
#define PHASE_SETUP_COMOKILINE 11
#define PHASE_SETUP_COMLX300TICKET 12
#define PHASE_SETUP_COMLX300LINE 13
#define PHASE_SETUP_TEST_VOLUME	14
#define PHASE_SETUP_USBEPSONLABEL 15
#define PHASE_SETUP_FEEDLABEL 16
#define PHASE_SETUP_DISPLAY_BYPASS 17

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "printer.h"
#include "amulet.h"
#include "cs.h"
#include "mca.h"
#include "message.h"
#include "chambfac.h"
#include "database.h"
//#include "fat.h"

extern short tone_duration;
extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern unsigned char m_ucVolumeCurrent;
extern uchar EepromRoutines_brightnessCurrent;
extern uchar EepromRoutines_sleepTimeoutCurrent;
extern uchar EepromRoutines_sleepBrightnessCurrent;
extern unsigned char m_ucCheckboxState;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void ee_write_printer(unsigned char printer, unsigned char prtype);
unsigned char BasePageStack(void);
//static char dummy[1024];

void send_to_amulet_string(uchar ucIndex, char message0[]);

char m_acSetup_Passwd[4];

/**
 * \details Handles the Amulet Screen Setup.htm. Setup.htm shows calibrator settings.
 * \param Amulet_Byte_ID Description
 * \param 92 Language (STATE) English = 0, French = 1
 * \param 93 Display Language Label and TextBox (STATE)
 * \param 100 Internal number for Activity group used for initHref
 * \param 101 Internal number for Printers group used for initHref
 * \param 102 ForceUpdate of Activity and Printer groups, reappear all buttons and forceUpdate of Volume
 * \param 103 Initial Volume Level
 * \param 59 Change Brightness Level
 * \param 105 Initial Brightness Level
 * \param 106 Initial Sleep Timeout Level
 * \param 107 Initial Sleep Brightness Level
 * \param 110 Refresh Sleep Timeout String Field
 * \param 111 Refresh Sleep Brightness String Field
 * \param 112 Refresh Sleep Brightness String Field
 * \param 113 Show Chamber Advanced Button
 * \param 114 Show Well Advanced Button
 * \param 115 Show Staff Button
 * \param Amulet_Word_ID Description
 * \param 100 Brightness Level
 * \param Amulet_String_ID Description
 * \param 110 Sleep Timeout
 * \param 111 Sleep Brightness
 * \param 112 Brightness Value
 * \param 113 Date Format
 * \param 114 Driver: Legacy or CDC
 * \param 115 Brightness Label
 * \param 116 Sleep Timeout Label
 * \param 117 Sleep Brightness Label
 * \param 118 Volume Label
 * \param 119 Title
 * \param 120 Advanced Chamber
 * \param 121 Date Format
 * \param 122 Test
 * \param 123 Activity Unit
 * \param 124 Printer
 * \param 125_126 Please Enter Password
 * \param 127 Language Label
 * \param 128 Language
 * \param 129 USB PC Driver
 * \param 130 Screen Calib
 * \param 131 Advanced Detector
 * \param 132 Staff
 * \returns None
 */
void AmuletSetup_menu(void){
	char printer;
	char prtype;
	unsigned char ucPrinterInternal;
	ushort brightness, brightness2;
	unsigned char ucValue2;
	char message[25];
	char bypass_string[20];
	double double_value;
	//F_FILE *fptr;

	switch(m_iPhase){
		case PHASE_SETUP_PRE_INIT:
			if(m_ucClear == 5) {
				m_acSetup_Passwd[0] = 0;
				m_ucClear = 0;
			}

			EE_READ(syst, &current.system);
			EE_READ(syslock, (uchar *) &current.lock);

			SetAmuletByte(92, current.language);
			if(Mca_installedDetector == DET_EMPTY) SetAmuletByte(93, 0xFF);

			if(current.num_chambers > 0){
				if(current.lock){
					if(current.system==CI){
						SetAmuletByte(100, 2);
					}else if(current.system==BQ){
						SetAmuletByte(100, 3);
					}
				}else{
					SetAmuletByte(100, 1);
				}
				SetAmuletByte(99, 0xFF);
			}else{
				if(current.system==CI){
					SetAmuletByte(100, 2);
				}else if(current.system==BQ){
					SetAmuletByte(100, 3);
				}
				SetAmuletByte(98, 0xFF);
			}

			EE_READ(print[0], (uchar *) &printer);
			EE_READ(print[1], (uchar *) &prtype);

			if(chamber_77t()){
				if((printer != -1) && (printer != 4)){
					printer = 4;
					prtype = 0;
					ee_write_printer(printer, prtype);
				}
				SetAmuletByte(117, 1);
			}else SetAmuletByte(117, 0);

			ucPrinterInternal = 0;
			switch(printer){
				case -1:
					//usb/HP
					ucPrinterInternal = 5;
					break;

				case -2:
					//usb/Epson
					ucPrinterInternal = 6;
					break;

				case -3:
					//usb/EpsonLabel
					ucPrinterInternal = 13;
					break;

				case 0:
					//232/slip
					ucPrinterInternal = 7;
					break;

				case 1:
					//232/roll
					ucPrinterInternal = 8;
					break;

				case 2:
					if(prtype==0){
						//232/oki-ticket
						ucPrinterInternal = 9;
					}else if(prtype==1){
						//232/oki-line
						ucPrinterInternal = 10;
					}
					break;

				case 3:
					if(prtype==0){
						//232/lx300-ticket
						ucPrinterInternal = 11;
					}else if(prtype==1){
						//232/lx300-line
						ucPrinterInternal = 12;
					}
					break;

				case 4:
					//none
					ucPrinterInternal = 4;
					break;
			}

			SetAmuletByte(101, ucPrinterInternal);
			if(m_ucVolumeCurrent == 255) SetAmuletByte(103, 254);
			else SetAmuletByte(103, m_ucVolumeCurrent);

			SetAmuletByte(106, EepromRoutines_sleepTimeoutCurrent);
			if(EepromRoutines_sleepTimeoutCurrent == 0)
			{	
				//strcpy(message, "OFF");
				get_amulet_message(L_TIMEOUT_OFF,message);    // "OFF"
			}	
			else sprintf(message, "%u", EepromRoutines_sleepTimeoutCurrent);
			send_to_amulet_string(110, message);
			SetAmuletByte(110, 0xFF);

			SetAmuletByte(107, EepromRoutines_sleepBrightnessCurrent);
			sprintf(message, "%u", EepromRoutines_sleepBrightnessCurrent);
			send_to_amulet_string(111, message);
			SetAmuletByte(111, 0xFF);

			SetAmuletByte(105, EepromRoutines_brightnessCurrent);
			sprintf(message, "%u", EepromRoutines_brightnessCurrent);
			send_to_amulet_string(112, message);
			SetAmuletByte(112, 0xFF);

			send_amulet_message(L_BRIGHTNESS,115);    // "Brightness:"

			send_amulet_message(L_SLEEP_TIMEOUT,116);    // "Sleep Timeout:"

			send_amulet_message(L_SLEEP_BRIGHTNESS,117);    // "Sleep Brightness:"

			send_amulet_message(L_VOLUME,118);    // "Volume:"

			send_amulet_message(L_SETUP_TITLE,119);    // "Setup"
			
			send_amulet_message(L_ADVANCED_CHAMBER,120);    // "Advanced Chamber"

			send_amulet_message(L_ENTER_PASSWORD,125);    // "Please Enter Password:"
			
			send_amulet_message(L_TEST,122);    // "Test"

			send_amulet_message(L_ACTIVITY_UNIT,123);    // "Activity Unit:"

			send_amulet_message(L_SETUP_DATE_FORMAT,121);    // "Date Format:"

			send_amulet_message(L_SETUP_PRINTER,124);    // "Printer:"

			send_amulet_message(L_ADVANCED_DETECTOR,131);    // "Advanced Detector"

			send_amulet_message(L_STAFF,132);    // "Staff"

			if(current.language == ENGLISH){
				switch(current.time_format){
					case 0:
						strcpy(message, "mm/dd/yyyy");
						break;

					case 1:
						strcpy(message, "dd/mm/yyyy");
						break;

					case 2:
						strcpy(message, "yyyy/mm/dd");
						break;

					default:
						strcpy(message, "mm/dd/yyyy");
						break;
				}
			}else if(current.language == FRENCH){
				switch(current.time_format){
					case 0:
						strcpy(message, "mm/dd/aaaa");
						break;

					case 1:
						strcpy(message, "dd/mm/aaaa");
						break;

					case 2:
						strcpy(message, "aaaa/mm/dd");
						break;

					default:
						strcpy(message, "mm/dd/aaaa");
						break;
				}
			}else{
				switch(current.time_format){
					case 0:
						strcpy(message, "mm/dd/yyyy");
						break;

					case 1:
						strcpy(message, "dd/mm/yyyy");
						break;

					case 2:
						strcpy(message, "yyyy/mm/dd");
						break;

					default:
						strcpy(message, "mm/dd/yyyy");
						break;
				}
			}
			send_to_amulet_string(113, message);

			if(current.usb_device_protocol == 0)				//strcpy(message, "Legacy");
				get_amulet_message(L_LEGACY,message);    // "Legacy"
			else if(current.usb_device_protocol == 1) //strcpy(message, "CDC");
				get_amulet_message(L_CDC,message);    // "CDC"
			//SetAmuletString(114, message);
			send_to_amulet_string(114,message);

			send_amulet_message(L_LANGUAGE, 127);    // "Language:"

			if(current.language == 0) get_amulet_message(L_ENGLISH, message);    // "English"
			else if(current.language == 1) get_amulet_message(L_FRENCH, message);    // "French"
			send_to_amulet_string(128, message);

			send_amulet_message(L_USB_PC_DRIVER,129);    // "USB PC Driver:"

			send_amulet_message(L_SCREEN_CALIB,130);    // "Screen Calib"
		
			if(current.feed_label) SetAmuletByte(118, 2);
			else SetAmuletByte(118, 1);

			SetAmuletByte(102, 0xFF);

			brightness = EepromRoutines_brightnessCurrent;
			if(brightness<10) brightness = 10;
			brightness *= 10;
			if(brightness>999) brightness = 999;
			if(INVERSION) brightness = 1000 - brightness;
			if (brightness<10) brightness = 10;
			SetAmuletWord(100, brightness);
			brightness2 = 900 - brightness;
			double_value = brightness2;
			double_value *= 235.0;
			double_value /= 890.0;
			double_value += 20.0;
			brightness2 = double_value;
			if(brightness2 > 255) brightness2 = 255;
			if(brightness2 < 20) brightness2 = 20;
			ucValue2 = brightness2;
			SetAmuletByte(55, ucValue2);
			SetAmuletByte(59, 0xFF);

			if(Mca_installedDetector == DET_EMPTY){
				if(current.security_mode != 0){
					SetAmuletByte(115, 0xFF);
				}
			}else{
				SetAmuletByte(115, 0xFF);
			}

			if(BasePageStack() == AmuletHTMLIndex[MAINSCREEN_HTM]){
				SetAmuletByte(113, 0xFF);
				if(!chamber_77t()) SetAmuletByte(116, 0xFF);
			}else if(BasePageStack() == AmuletHTMLIndex[WELLMAINSCREEN_HTM]){
				SetAmuletByte(114, 0xFF);
				if(current.printer == USB_EPS_LABEL_PRINTER) SetAmuletByte(116, 0xFF);
			}

			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_WAIT:
			break;

		case PHASE_SETUP_CI_OR_BQ:
			current.lock = FALSE;
			EE_WRITE(syst, &current.system);
			EE_WRITE(syslock, (uchar *) &current.lock);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_CI_ONLY:
			current.system = CI;
			current.lock = TRUE;
			EE_WRITE(syst, &current.system);
			EE_WRITE(syslock, (uchar *) &current.lock);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_BQ_ONLY:
			current.system = BQ;
			current.lock = TRUE;
			EE_WRITE(syst, &current.system);
			EE_WRITE(syslock, (uchar *) &current.lock);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_NONE:
			ee_write_printer(4, 0);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_USBHP:
			ee_write_printer(0xFF, 0);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_USBEPSON:
			ee_write_printer(0xFE, 0);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_COMSLIP:
			ee_write_printer(0x00, 0);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_COMROLL:
			ee_write_printer(0x01, 0);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_COMOKITICKET:
			ee_write_printer(0x02, 0);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_COMOKILINE:
			ee_write_printer(0x02, 1);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_COMLX300TICKET:
			ee_write_printer(0x03, 0);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_COMLX300LINE:
			ee_write_printer(0x03, 1);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_TEST_VOLUME:
			if(tone_duration == 0){
				tone_duration = 32767;
				start_tone();
			}else{
				tone_duration = 1;
			}
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_USBEPSONLABEL:
			ee_write_printer(0xFD, 0);
			beep_amulet();
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_FEEDLABEL:
			if(m_ucCheckboxState == 1) current.feed_label = 0;
			else if(m_ucCheckboxState == 2) current.feed_label = 1;
			DB_WriteFeedLabel(current.feed_label);
			m_iPhase = PHASE_SETUP_WAIT;
			break;

		case PHASE_SETUP_DISPLAY_BYPASS:
			if(current.bypass > 0){
				sprintf(bypass_string, "%d", current.bypass);
				send_to_amulet_string(119, bypass_string);
				SetAmuletByte(119, 0xFF);
			}
			m_iPhase = PHASE_SETUP_WAIT;
			break;
	}
}

void ee_write_printer(unsigned char printer, unsigned char prtype){
	EE_WRITE(print[0], (uchar *) &printer);
	EE_WRITE(print[1], (uchar *) &prtype);
	current.printer = printer;
	printer_init();
}
