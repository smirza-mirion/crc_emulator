/**
 * \file
 * \details This file handles calls from the Amulet Main Screen
 */
#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "Amulet.h"
#include "message.h"
#include "nuc.h"
#include "mca.h"
#include "printer.h"

extern int m_iPhase;
extern CURRENT  current;
extern bool AmuletGenericYesNo_AccuracyTestPending;
extern bool CalibratorMain_WelcomeScreen;
extern volatile bool DisplayActivity_SendActivity;
extern short max_chambers;
extern REMOTE remote[];
extern volatile short DisplayActivity_addedDigits;
extern const char *rev_num;
extern char Amulet_amuletImage[20];
extern char Amulet_amuletImage2[20];
extern ushort amuletBuild;
extern unsigned char m_ucHotKeyNuclideID[UPPER_LIMIT_CHAMB][8];
extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern time_t clock_time;
extern AUTOLINEARITYTEST AmuletAutoLinearityTest_test;
extern volatile int m_iMenu;
extern volatile int m_iCurPhase;
extern volatile int m_iNextPhase;

void AmuletGenericYesNo_saveInDatabase(bool autoconstancy);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void trim_and_shrink(char *acByte);
void GetExtendedTimeInfoLanguageSec(time_t *dtmDateTime, char *acMsg);
void GetExtendedNuclideString(unsigned char ucNuclideID, char *acMsg);

/**
 * \details Handle the Amulet Screen MainScreen.htm. MainScreen.htm is the home screen of the calibrator and displays the activity in the chamber.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label	(TOGGLE)
 * \param 21 Show Chamber Button(TOGGLE)
 * \param 22 Show Well Button(TOGGLE)
 * \param 23 Show Beta Button(TOGGLE)
 * \param 100 Language (STATE) English = 0, French = 1
 * \param 101 Show Screen (TOGGLE)
 * \param 102 Show Print Button (TOGGLE)
 * \param 103 Show Activty Unit (TOGGLE)
 * \param 104 Show Activity (TOGGLE)
 * \param 105 Show AutoLinearity Pause Status (TOGGLE)
 * \param 106 No Moly and Inventory (STATE) Show Moly and Invetory Buttons = 0, No Moly and Inventory Buttons = 1
 * \param Amulet_String_ID Description
 * \param 0_1 Current Nuclide
 * \param 2 Current Time
 * \param 3 Cal Num
 * \param 4 Remote Indicator
 * \param 6 Future Time
 * \param 7 Future Activity
 * \param 9 Current Chamber
 * \param 10 Current Activity
 * \param 11 Current Unit
 * \param 12 Over Range Error
 * \param 13 Low Battery Warning
 * \param 100 Title
 * \param 101 HotKey 1
 * \param 102 HotKey 2
 * \param 103 HotKey 3
 * \param 104 HotKey 4
 * \param 105 HotKey 5
 * \param 106 HotKey 6
 * \param 107 HotKey 7
 * \param 108 HotKey 8
 * \param 109 AutoLinearity Paused!!!
 * \param 110_111 Please Select Nuclide:
 * \param 112_113 Please Enter Password:
 * \param 114_115 Please Enter Cal #:
 * \param 116_117 Please Enter Dose Time:
 * \returns None
 */
void AmuletMainScreen_menu(void){
	short i;
	char acName[8];
	short ch_num = current.main_chamber;
	short ch_type;
	int imageversion;
	char prefix[20];
	static ulong delay;
	char nucshrink[25];
	char message[51];
	short index;
	char branding[25];

	if(current.num_chambers>0) ch_type = chamber_type(ch_num);

	switch(m_iPhase){
		case 0:
			if(AmuletGenericYesNo_AccuracyTestPending){
				AmuletGenericYesNo_saveInDatabase(TRUE);
				AmuletGenericYesNo_AccuracyTestPending = FALSE;
			}
			delay = g_csec_tstamp + 10;
			m_iPhase = 1;
			break;

		case 1:
			if(g_csec_tstamp > delay){
				if(chamber_77t()) SetAmuletByte(106, 1);
				else SetAmuletByte(106, 0);

				SetAmuletByte(100, current.language);
				send_amulet_message(L_AUTOLINEARITY_PAUSED, 109);    // "AutoLinearity Paused!!!"
				send_amulet_message(L_PLEASE_SELECT_NUCLIDE, 110);    // "Please Select Nuclide"
				send_amulet_message(L_ENTER_PASSWORD, 112);    // "Please Enter Password:"
				send_amulet_message(L_PLEASE_ENTER_CAL_NUM, 114);    // "Please Enter Cal #:"
				send_amulet_message(L_PLEASE_ENTER_DOSE_TIME, 116);    // "Please Enter Dose Time:"
				CalibratorMain_WelcomeScreen = FALSE;
				DisplayActivity_SendActivity = TRUE;
				for(index=0; index<max_chambers; index++){
					if(remote[index].exists) remote[index].keys_active = TRUE;
					else remote[index].keys_active = FALSE;
				}
				DisplayActivity_addedDigits = 0;
				if(current.branding == 0) sprintf(branding, "CRC-55t, %s", rev_num);
				else if(current.branding == 1) sprintf(branding, "CAPRAC-t, %s", rev_num);
				else if(current.branding == 2) sprintf(branding, "CAPTUS-700t, %s", rev_num);
				else if(current.branding == 3) sprintf(branding, "CRC-77t, %s", rev_num);
				else sprintf(branding, "%s", rev_num);

				send_to_amulet_string(100, branding);
				if(Amulet_amuletImage[0]!=0){
					if(strlen(Amulet_amuletImage)>4){
						strcpy(prefix, Amulet_amuletImage);
						prefix[strlen(prefix) - 4] = 0;
						imageversion = atoi(prefix);
						if(imageversion == amuletBuild) Amulet_amuletImage[0] = 0;
					}
				}
				if(Amulet_amuletImage2[0]!=0){
					if(strlen(Amulet_amuletImage2)>5){
						strcpy(prefix, Amulet_amuletImage2);
						prefix[strlen(prefix) - 5] = 0;
						imageversion = atoi(prefix);
						if(imageversion == amuletBuild) Amulet_amuletImage2[0] = 0;
					}
				}

				if(current.num_chambers>1) SetAmuletByte(21, 0xFF);
				sprintf(message, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(message, ", R");
						break;
					case P_CHAMB:
						strcat(message, ", PET");
						break;
					case B_CHAMB:
						strcat(message, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(message, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(message, ", HR");
						break;
					case K_CHAMB:
						strcat(message, ", 1K");
						break;
				}
				if(remote[current.main_chamber].exists) send_to_amulet_string(4, "*");
				send_to_amulet_string(9, message);
				SetAmuletByte(20, 0xFF);

				if(current.num_chambers>0){
					for(i=0; i<8; i++){
						acName[7] = 0;
						if(m_ucHotKeyNuclideID[ch_type][i] != 255){
							NuclideData_getName(m_ucHotKeyNuclideID[ch_type][i], acName);
							trim_and_shrink(acName);
							if(NuclideData_getEffectiveResponse(m_ucHotKeyNuclideID[ch_type][i], ch_type) == 0.0){
								strcpy(message, "(");
								strcat(message, acName);
								strcat(message, ")");
							}else{
								strcpy(message, acName);
							}
							send_to_amulet_string(101+i, message);
						}
					}
				}

				if(Mca_installedDetector == DET_WELL || Mca_installedDetector == DET_PROBE700 || Mca_installedDetector == DET_WELL700 || Mca_installedDetector == DET_DRILLEDPROBE700) SetAmuletByte(22, 0xFF);
				else if(Mca_installedDetector == DET_BETA) SetAmuletByte(23, 0xFF);

				if(measurement[ch_num].future.dosetime != NO_TIME){
					GetExtendedTimeInfoLanguageSec(&(measurement[ch_num].future.dosetime), message);
					send_to_amulet_string(6, message);
				}

				SetAmuletByte(101, 0xFF);

				if(current.num_chambers>0){
					strcpy(nucshrink, chamber[ch_num].nucdata.name);
					trim_and_shrink(nucshrink);
					if(DisplayActivity_SendActivity){
						if(nucshrink[0]!=0){
							GetExtendedNuclideString(chamber[ch_num].nuc_index, message);
							send_to_amulet_string(0, message);
						}
						else send_to_amulet_string(0, "");
						//else send_to_amulet_string(0, "*****");
					}
				}
				GetExtendedTimeInfoLanguageSec(&clock_time, message);
				send_to_amulet_string(2, message);
				if(current.num_chambers>0) disp_cal(measurement[ch_num].resp0);

				if(current.printer != NONE_PRINTER)
					SetAmuletByte(102, 0xFF);

				if(!current.lock) SetAmuletByte(103, 0xFF);

				activity_to_screen();

				if(AmuletAutoLinearityTest_test.AutoLinearityTestID < -1) SetAmuletByte(105, 0xFF);

				m_iMenu = 0;
				m_iPhase = PHASE_GENERIC_NOP;
				m_iCurPhase = PHASE_GENERIC_CUR_NOP;
				m_iNextPhase = PHASE_GENERIC_CUR_NOP;
			}
			break;
	}
}
