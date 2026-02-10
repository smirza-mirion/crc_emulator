/**
 * \file
 * \details This file handles calls from the Amulet Splash Screen
 */

#define PHASE_INITIAL_PRE_INIT	0
#define PHASE_INITIAL_WAIT		1
#define PHASE_INITIAL_SENDERROR 2
#define PHASE_INITIAL_SENDWARNING 3
#define PHASE_INITIAL_WAIT_AMULET_INDEX 4
#define PHASE_INITIAL_PRE_INIT2	5

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "amulet.h"
#include "message.h"
#include "keyboard.h"
#include "ff.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
char SetAmuletWord(unsigned char ucIndex, short unsigned int uiValue);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
void ClearPageStack(void);
void DB_shutdown(void);

void send_to_amulet_string(uchar ucIndex, char message0[]);

extern int m_iPhase;
extern uchar EepromRoutines_brightnessCurrent;
extern CURRENT  current;
extern char Amulet_amuletImage[];
extern char Amulet_amuletImage2[];
extern ushort amuletBuild;
extern ushort amuletIndexPageNum;
extern AUTOLINEARITYTEST AmuletAutoLinearityTest_test;
extern int AmuletAutoLinearityTest_mode;
extern unsigned char m_ucClear;

extern char pr_crcname[];
extern char pr_crcname_large[];
static unsigned long int after_one_sec;
static unsigned long int after_three_sec;

extern const char *rev_num;

char AmuletInitialRecoveryMsg[100];
/**
 * \details Handles the Amulet Screen Index.htm. Index.htm is displayed only at Power On as the Splash screen.
 * \param Amulet_Byte_ID Description
 * \param 100 Activate screen brightness
 * \param 101 Display Chamber Main Screen Button
 * \param Amulet_Word_ID Description
 * \param 100 Screen Brightness
 * \param Amulet_String_ID Description
 * \param 99 Title
 * \param 100 Revision
 * \param 101 Copyright
 * \param 102 All Rights Reserved
 * \param 103 Continue
 * \param 104 Login
 * \param 105,106 Upgrade System
 * \returns None
 */
void AmuletInitial_menu(void){
	ushort brightness, brightness2;
	unsigned char ucValue2;
	char message[125];
	char prefix[20];
	int imageversion;
	bool upgrade;
	double double_value;
	FILINFO fileInfo;
	FIL fileObject;
	UINT bytesWritten;
	char longFileName[100];
	//char strng[100], titlestring[50];
	//static bool toggle;

	switch(m_iPhase){
		case PHASE_INITIAL_PRE_INIT:
			//toggle = TRUE;
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
			SetAmuletByte(100, 0xFF);

			//strcpy(message, "Rev ");
			get_amulet_message(L_REVI,message);    // "Rev "
			strcat(message, rev_num);
			send_to_amulet_string(100, message);

			get_amulet_message(L_COPYRIGHT,message);    // "COPYRIGHT"
			strcat(message," 2025");
			send_to_amulet_string(101,message);
			
			send_amulet_message(L_RIGHTS,102);    // "ALL RIGHTS RESERVED"
			send_amulet_message(L_CONTINUE_BTN,103);    // "Continue"
			send_amulet_message(L_LOGIN,104);    // "Login"
			send_amulet_message(L_UPDATE_SYSTEM,105);    // "Update System"

			after_three_sec = g_csec_tstamp + 300;
			m_iPhase = PHASE_INITIAL_WAIT_AMULET_INDEX;
			break;
			
		case PHASE_INITIAL_WAIT_AMULET_INDEX:
			if((amuletIndexPageNum != 0xFFFF) || (g_csec_tstamp > after_three_sec)){
				m_iPhase = PHASE_INITIAL_PRE_INIT2;
			}
			break;

		case PHASE_INITIAL_PRE_INIT2:
			upgrade = FALSE;
			if(amuletIndexPageNum == 0x20){
				if(Amulet_amuletImage[0]!=0){
					if(strlen(Amulet_amuletImage)>4){
						strcpy(prefix, Amulet_amuletImage);
						prefix[strlen(prefix) - 4] = 0;
						imageversion = atoi(prefix);
						if(imageversion != amuletBuild){
							upgrade = TRUE;
						}
					}
				}
			}else if(amuletIndexPageNum == 0x21){
				if(Amulet_amuletImage2[0]!=0){
					if(strlen(Amulet_amuletImage2)>5){
						strcpy(prefix, Amulet_amuletImage2);
						if(prefix[strlen(prefix) - 5] == 'g'){
							prefix[strlen(prefix) - 5] = 0;
							imageversion = atoi(prefix);
							if(imageversion != amuletBuild){
								upgrade = TRUE;
							}
						}
					}
				}
			}

			if(upgrade){
				f_chdrive(0);
				if(f_chdir("/images")== FR_OK){
					fileInfo.lfname = longFileName;
					fileInfo.lfsize = 100;
					if(f_stat("progname.txt", &fileInfo) == FR_OK) f_unlink("progname.txt");
					f_open(&fileObject, "progname.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
					f_write(&fileObject, "flash.mem", 9, &bytesWritten);
					f_sync(&fileObject);
					f_close(&fileObject);
				}
			}

			if(current.branding == 0){
				SetAmuletString(99, "CRC-55t");
				strcpy(pr_crcname, "CRC-55t");
				//strcpy(pr_crcname_large, "CRC-55t      RADIOISOTOPE DOSE CALIBRATOR");
				get_amulet_message(L_55T_FULL_TITLE,pr_crcname_large);    // "CRC-55t      RADIOISOTOPE DOSE CALIBRATOR"
			}else if(current.branding == 1){
				SetAmuletString(99, "CAPRAC-t");
				strcpy(pr_crcname, "CAPRAC-t");
				//strcpy(pr_crcname_large, "CAPRAC-t        RADIOISOTOPE WELL COUNTER");
				get_amulet_message(L_CAPRAC_FULL_TITLE,pr_crcname_large);    // "CAPRAC-t        RADIOISOTOPE WELL COUNTER"
			}else if(current.branding == 2){
				SetAmuletString(99, "CAPTUS-700t");
				strcpy(pr_crcname, "CAPTUS-700t");
				//strcpy(pr_crcname_large, "CAPTUS-700t          RADIOISOTOPE COUNTER");
				get_amulet_message(L_700T_FULL_TITLE,pr_crcname_large);    // "CAPTUS-700t          RADIOISOTOPE COUNTER"
			}else if(current.branding == 3){
				SetAmuletString(99, "CRC-77t");
				strcpy(pr_crcname, "CRC-77t");
				//strcpy(pr_crcname_large, "CRC-77t      RADIOISOTOPE DOSE CALIBRATOR");
				get_amulet_message(L_77T_FULL_TITLE,pr_crcname_large);    // "CRC-77t      RADIOISOTOPE DOSE CALIBRATOR"
			}else{
				SetAmuletString(99, "");
				strcpy(pr_crcname, "");
				strcpy(pr_crcname_large, "");
			}

			if(AmuletInitialRecoveryMsg[0] == 0){
				if((current.security_mode == 1) && (current.user_id == -1)){
					ClearPageStack();
					PushPageStack(AmuletHTMLIndex[INDEX_HTM]);
					SetAmuletByte(104, 0xFF);
					m_iPhase = PHASE_INITIAL_WAIT;
				}else{
					ClearPageStack();
					if(upgrade){
						SetAmuletByte(103, 0xFF);
						m_iPhase = PHASE_INITIAL_WAIT;
					}else{
						if(current.num_chambers>0){
							if(AmuletAutoLinearityTest_test.AutoLinearityTestID == -4){
								AmuletAutoLinearityTest_mode = 0;
								m_ucClear = 105;
								PushPageStack(AmuletHTMLIndex[MAINSCREEN_HTM]);
								SetAmuletHTML(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
								PushPageStack(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
								return;
							}else{
								SetAmuletByte(101, 0xFF);
								after_one_sec = g_csec_tstamp + 100;
								m_iPhase = PHASE_INITIAL_WAIT;
							}
						}else if(Mca_installedDetector == DET_WELL){
							SetAmuletByte(102, 0xFF);
							m_iPhase = PHASE_INITIAL_WAIT;
						}else if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)){
							SetAmuletByte(102, 0xFF);
							m_iPhase = PHASE_INITIAL_WAIT;
						}else{
							after_one_sec = g_csec_tstamp + 100;
							m_iPhase = PHASE_INITIAL_SENDERROR;
						}
					}
				}
			}else{
				after_one_sec = g_csec_tstamp + 100;
				m_iPhase = PHASE_INITIAL_SENDWARNING;
			}
			break;

		case PHASE_INITIAL_WAIT:
			/*if(g_csec_tstamp > after_one_sec){
				if(toggle){
					toggle = FALSE;
					get_amulet_message(L_PRINTER_PROBLEM, titlestring);    // "Printer Problem"
					get_amulet_message(L_CONNECT_PRINTER, strng);    // "CONNECT PRINTER"
					Amulet_DisplayError(titlestring, strng, TRUE);
				}
			}*/
			break;

		case PHASE_INITIAL_SENDERROR:
			if(g_csec_tstamp > after_one_sec){
				//Amulet_DisplayError("", "No detectors found\nNo Chamber attached\nNo Well attached",FALSE);
				get_amulet_message(L_NONE_FOUND,message);    // "No detectors found\nNo Chamber attached\nNo Well attached"
				Amulet_DisplayError("",message,FALSE);
				m_iPhase = PHASE_INITIAL_WAIT;
			}
			break;

		case PHASE_INITIAL_SENDWARNING:
			if(g_csec_tstamp > after_one_sec){
				Amulet_DisplayError("", AmuletInitialRecoveryMsg,FALSE);
				DB_shutdown();
				m_iPhase = PHASE_INITIAL_WAIT;
			}
			break;
	}
}


void send_to_amulet_string(uchar ucIndex, char message0[])
{
	char message[200];
	uchar raw;
	uchar byt1,byt2;
	short i,j;

	j = -1;
	for(i = 0; i < 100; i++)
	{
		++j;
		raw = message0[i];
		if(raw == '\0')
		{
			message[j] = '\0';
			break;
		}

		if(raw < 0x80)
		{
			message[j] = raw;
			continue;
		}	

		//characters > 0x7f;
		byt1 = 0xc0 + ((raw & 0xc0) >> 6);
		byt2 = 0x80 + (raw & 0x3f);
		message[j] = byt1;
		++j;
		message[j] = byt2;
	}


	SetAmuletString(ucIndex, message);
	
}
	
void send_amulet_message(short msg_num, uchar ucIndex){
	char message[100];

	get_amulet_message(msg_num, message);
	send_to_amulet_string(ucIndex, message);
}

void undo_string_coding(char str[], char message0[])
{

	uchar byt1,byt2;
	uchar raw;
	short i,j;

	i = j = -1;
	for(;;)
	{
		++i;
		++j;
		raw = message0[i];
		if(raw < 0x80)
		{
			str[j] = raw;
			if(raw == '\0')
				break;
		}
		else
		{	
			byt2 = message0[++i];
			byt2 = byt2 & 0x3f;
			byt1 = ((raw & 0x03) << 6);
			str[j] = byt1 | byt2;
		}
	}	

}	
