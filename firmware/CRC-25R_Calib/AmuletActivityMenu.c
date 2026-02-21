#define PHASE_ACTIVITY_PRE_INIT 0
#define PHASE_ACTIVITY_WAIT 1
#define PHASE_ACTIVITY_Ci 2
#define PHASE_ACTIVITY_mCi 3
#define PHASE_ACTIVITY_uCi 4
#define PHASE_ACTIVITY_7 5
#define PHASE_ACTIVITY_8 6
#define PHASE_ACTIVITY_9 7
#define PHASE_ACTIVITY_4 8
#define PHASE_ACTIVITY_5 9
#define PHASE_ACTIVITY_6 10
#define PHASE_ACTIVITY_1 11
#define PHASE_ACTIVITY_2 12
#define PHASE_ACTIVITY_3 13
#define PHASE_ACTIVITY_0 14
#define PHASE_ACTIVITY_PERIOD 15
#define PHASE_ACTIVITY_BACKSPACE 16
#define PHASE_ACTIVITY_ENTER 17
#define PHASE_ACTIVITY_DISPLAY 18
#define PHASE_ACTIVITY_GBq 19
#define PHASE_ACTIVITY_MBq 20
#define PHASE_ACTIVITY_YEAR 21
#define PHASE_ACTIVITY_DAY	22
#define PHASE_ACTIVITY_HOUR	23
#define PHASE_ACTIVITY_MINUTE	24
#define PHASE_ACTIVITY_SECOND	25
#define PHASE_ACTIVITY_KBq	26

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "nuc.h"

extern int m_iPhase;
extern CURRENT  current;
extern char m_cPeriodPos;
extern char m_acOutput[100];
extern volatile char m_acTitle[52];
extern unsigned char m_ucUnit;
extern unsigned char m_ucActivityConfig;
extern unsigned char m_ucSetActivity;
extern unsigned char m_ucLenLimit;
extern float m_fAddInventory_Activity;
extern unsigned char m_ucAddInventory_MeasuredNuclide;
extern const float unitfact[];
extern time_t m_dtmAddInventory_Date;
extern float m_fWithdrawInventory_WithdrawActivity;
extern float m_fKitInventory_KitActivity;
extern float m_fInfo_InputUnitlessActivity;
extern unsigned char m_ucInfo_InputUnit;
extern float m_fInfo_FromActivity;
extern unsigned char m_ucInfo_FromUnit;
extern float m_fInfo_ToActivity;
extern unsigned char m_ucInfo_ToUnit;
extern float m_fSetupSources_Co57Activity;
extern float m_fSetupSources_Co60Activity;
extern float m_fSetupSources_Ba133Activity;
extern float m_fSetupSources_Cs137Activity;
extern float m_fSetupSources_Na22Activity;
extern float m_fSetupSources_Ge68Activity;
extern time_t m_dtmInfo_ToTime;
extern SETUP_NUCLIDE m_tSetupNuclide[10];
extern float AmuletDoseTableMenu_activity;
extern float AmuletDoseTableMenu_dose;
extern float AmuletWellMeasureEfficiency_calibrationActivity;
extern float AmuletWellSetupTestSourceMenu_act;
extern NUCDATA AmuletWellAddEditUserNuclides_temp;
extern PROBEBIOASSAYSETTING AmuletWellSetupBioAssayMenu_bioAssaySetting;
extern PROBEBIOASSAYEFFICIENCY AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff;
extern PROBETHYROIDUPTAKETEST AmuletAddEditThyroidUptakeTest_test;
extern WELLRBCSURVIVALTEST AmuletAddEditRBCSurvivalTest_test;

void send_to_amulet_string(uchar ucIndex, char message0[]);
bool test_sig_digits(char *str, uchar limit);
void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);

void AmuletActivity_menu(void) {
	unsigned char ucBlank;
	short sTemp;
	float fActivity;
	char titlestring[51], formatstring[51];

	switch(m_iPhase) {
		case PHASE_ACTIVITY_PRE_INIT:
			m_cPeriodPos = -1;
			m_acOutput[0] = 0;

			send_to_amulet_string(100, (char *)m_acTitle);
			send_to_amulet_string(102, m_acOutput);
			send_amulet_message(L_BACKSPACE, 105);    // "Backspace"
			send_amulet_message(L_ACCEPT_BUTTON, 106);    // "Accept"
			send_amulet_message(L_CANCEL_BUTTON, 107);    // "Cancel"

			if(m_ucActivityConfig == 1){
				// Position CI
				//SetAmuletWord(109, m_uiOffsetX + 34);
				SetAmuletWord(109, 280);
				//SetAmuletWord(110, m_uiOffsetY + 95);
				SetAmuletWord(110, 139);

				// Position mCI
				//SetAmuletWord(111, m_uiOffsetX + 134);
				SetAmuletWord(111, 380);
				//SetAmuletWord(112, m_uiOffsetY + 95);
				SetAmuletWord(112, 139);

				// Position uCi
				//SetAmuletWord(113, m_uiOffsetX + 234);
				SetAmuletWord(113, 480);
				//SetAmuletWord(114, m_uiOffsetY + 95);
				SetAmuletWord(114, 139);

				//m_ucUnit = 1;
				//SetAmuletByte(99, 2);
				//SetAmuletString(104, "mCi");
				m_ucUnit = 0;
				SetAmuletByte(99, 3);
				send_to_amulet_string(104, "uCi");
			}

			if(m_ucActivityConfig == 2){
				//// Position GBq
				////SetAmuletWord(141, m_uiOffsetX + 54);
				//SetAmuletWord(141, 300);
				////SetAmuletWord(142, m_uiOffsetY + 95);
				//SetAmuletWord(142, 139);

				//// Position MBq
				////SetAmuletWord(143, m_uiOffsetX + 174);
				//SetAmuletWord(143, 420);
				////SetAmuletWord(144, m_uiOffsetY + 95);
				//SetAmuletWord(144, 139);
				// Position GBq
				SetAmuletWord(141, 280);
				SetAmuletWord(142, 139);

				// Position MBq
				SetAmuletWord(143, 380);
				SetAmuletWord(144, 139);

				// Position KBq
				SetAmuletWord(155, 480);
				SetAmuletWord(156, 139);

				m_ucUnit = 4;
				SetAmuletByte(99, 5);
				send_to_amulet_string(104, "MBq");
			}

			if(m_ucActivityConfig == 3){
				// Position CI
				//SetAmuletWord(109, m_uiOffsetX + 4);
				SetAmuletWord(109, 250);
				//SetAmuletWord(110, m_uiOffsetY + 85);
				SetAmuletWord(110, 129);

				// Position GBq
				//SetAmuletWord(141, m_uiOffsetX + 54);
				SetAmuletWord(141, 300);
				//SetAmuletWord(142, m_uiOffsetY + 105);
				SetAmuletWord(142, 149);

				// Position mCI
				//SetAmuletWord(111, m_uiOffsetX + 114);
				SetAmuletWord(111, 360);
				//SetAmuletWord(112, m_uiOffsetY + 85);
				SetAmuletWord(112, 129);

				// Position MBq
				//SetAmuletWord(143, m_uiOffsetX + 174);
				SetAmuletWord(143, 420);
				//SetAmuletWord(144, m_uiOffsetY + 105);
				SetAmuletWord(144, 149);

				// Position uCi
				//SetAmuletWord(113, m_uiOffsetX + 234);
				SetAmuletWord(113, 480);
				//SetAmuletWord(114, m_uiOffsetY + 85);
				SetAmuletWord(114, 129);

				// Position KBq
				SetAmuletWord(155, 540);
				SetAmuletWord(156, 149);

				//m_ucUnit = 1;
				//SetAmuletByte(99, 2);
				//SetAmuletString(104, "mCi");
				m_ucUnit = 0;
				SetAmuletByte(99, 3);
				send_to_amulet_string(104, "uCi");
			}

			if(m_ucActivityConfig == 4){
				// Position Year
				//SetAmuletWord(153, m_uiOffsetX + 4);
				SetAmuletWord(153, 250);
				//SetAmuletWord(154, m_uiOffsetY + 85);
				SetAmuletWord(154, 129);

				// Position Day
				//SetAmuletWord(151, m_uiOffsetX + 57);
				SetAmuletWord(151, 303);
				//SetAmuletWord(152, m_uiOffsetY + 105);
				SetAmuletWord(152, 149);

				// Position Hour
				//SetAmuletWord(149, m_uiOffsetX + 124);
				SetAmuletWord(149, 370);
				//SetAmuletWord(150, m_uiOffsetY + 85);
				SetAmuletWord(150, 129);

				// Position Minute
				//SetAmuletWord(147, m_uiOffsetX + 184);
				SetAmuletWord(147, 430);
				//SetAmuletWord(148, m_uiOffsetY + 105);
				SetAmuletWord(148, 149);

				// Position Second
				//SetAmuletWord(145, m_uiOffsetX + 244);
				SetAmuletWord(145, 490);
				//SetAmuletWord(146, m_uiOffsetY + 85);
				SetAmuletWord(146, 129);

				if(current.language == 0) SetAmuletByte(99, 7);
				else if(current.language == 1) SetAmuletByte(99, 13);
				m_ucUnit = 12;
				send_amulet_message(L_DAY, 104);    // " Day"
			}

			if(m_ucActivityConfig == 5){
				if(current.system == BQ){
					//// Position GBq
					////SetAmuletWord(141, m_uiOffsetX + 54);
					//SetAmuletWord(141, 300);
					////SetAmuletWord(142, m_uiOffsetY + 95);
					//SetAmuletWord(142, 139);

					//// Position MBq
					////SetAmuletWord(143, m_uiOffsetX + 174);
					//SetAmuletWord(143, 420);
					////SetAmuletWord(144, m_uiOffsetY + 95);
					//SetAmuletWord(144, 139);

					// Position GBq
					SetAmuletWord(141, 280);
					SetAmuletWord(142, 139);

					// Position MBq
					SetAmuletWord(143, 380);
					SetAmuletWord(144, 139);

					// Position KBq
					SetAmuletWord(155, 480);
					SetAmuletWord(156, 139);

					m_ucUnit = 4;
					SetAmuletByte(99, 5);
					send_to_amulet_string(104, "MBq");
				}else{
					// Position CI
					//SetAmuletWord(109, m_uiOffsetX + 34);
					SetAmuletWord(109, 280);
					//SetAmuletWord(110, m_uiOffsetY + 95);
					SetAmuletWord(110, 139);

					// Position mCI
					//SetAmuletWord(111, m_uiOffsetX + 134);
					SetAmuletWord(111, 380);
					//SetAmuletWord(112, m_uiOffsetY + 95);
					SetAmuletWord(112, 139);

					// Position uCi
					//SetAmuletWord(113, m_uiOffsetX + 234);
					SetAmuletWord(113, 480);
					//SetAmuletWord(114, m_uiOffsetY + 95);
					SetAmuletWord(114, 139);

					//m_ucUnit = 1;
					//SetAmuletByte(99, 2);
					//SetAmuletString(104, "mCi");
					m_ucUnit = 0;
					SetAmuletByte(99, 3);
					send_to_amulet_string(104, "uCi");
				}
			}

			SetAmuletByte(100, 0xFF);
			if(m_ucActivityConfig == 1) SetAmuletByte(102, 0xFF);
			if(m_ucActivityConfig == 2) SetAmuletByte(103, 0xFF);
			if(m_ucActivityConfig == 3){
				SetAmuletByte(102, 0xFF);
				if((m_ucSetActivity == 4) || (m_ucSetActivity == 5) || (m_ucSetActivity == 6) || (m_ucSetActivity == 24) || (m_ucSetActivity == 25) || (m_ucSetActivity == 27) || (m_ucSetActivity == 28) || (m_ucSetActivity == 29) || (m_ucSetActivity == 30) || (m_ucSetActivity == 31) || (m_ucSetActivity == 32) || (m_ucSetActivity == 33) || (m_ucSetActivity == 34)) SetAmuletByte(105, 0xFF);
				else SetAmuletByte(103, 0xFF);
			}
			if(m_ucActivityConfig == 4){
				if(current.language == 0) SetAmuletByte(104, 0xFF);
				else if(current.language == 1) SetAmuletByte(106, 0xFF);
			}
			if(m_ucActivityConfig == 5){
				if(current.system == BQ){
					if((m_ucSetActivity == 4) || (m_ucSetActivity == 5) || (m_ucSetActivity == 6) || (m_ucSetActivity == 24) || (m_ucSetActivity == 25) || (m_ucSetActivity == 27) || (m_ucSetActivity == 28) || (m_ucSetActivity == 29) || (m_ucSetActivity == 30) || (m_ucSetActivity == 31) || (m_ucSetActivity == 32) || (m_ucSetActivity == 33) || (m_ucSetActivity == 34)) SetAmuletByte(105, 0xFF);
					else SetAmuletByte(103, 0xFF);
				}else SetAmuletByte(102, 0xFF);
			}

			m_iPhase = PHASE_ACTIVITY_WAIT;
			break;

		case PHASE_ACTIVITY_WAIT:
			break;

		case PHASE_ACTIVITY_Ci:
			beep_amulet();
			m_ucUnit = 2;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_mCi:
			beep_amulet();
			m_ucUnit = 1;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_uCi:
			beep_amulet();
			m_ucUnit = 0;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_7:
			beep_amulet();
			if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "7");
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_8:
			beep_amulet();
			if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "8");
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_9:
			beep_amulet();
			if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "9");
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_4:
			beep_amulet();
			if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "4");
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_5:
			beep_amulet();
			if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "5");
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_6:
			beep_amulet();
			if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "6");
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_1:
			beep_amulet();
			if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "1");
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_2:
			beep_amulet();
			if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "2");
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_3:
			beep_amulet();
			if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "3");
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_0:
			beep_amulet();
			if(m_acOutput[0]!= 0){
				if(test_sig_digits(m_acOutput, m_ucLenLimit)) strcat(m_acOutput, "0");
			}
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_PERIOD:
			beep_amulet();
			if(m_cPeriodPos == -1) {
				if(test_sig_digits(m_acOutput, m_ucLenLimit)) {
					m_cPeriodPos = strlen(m_acOutput);
					strcat(m_acOutput, ".");
				}
			}
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_BACKSPACE:
			beep_amulet();
			if(strlen(m_acOutput) > 0) {
					if(m_acOutput[strlen(m_acOutput) - 1] == '.') {
						m_cPeriodPos = -1;
					}
					m_acOutput[strlen(m_acOutput) - 1] = 0;
			}
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_ENTER:
			if((strlen(m_acOutput) == 0) || ((strlen(m_acOutput) ==1) && (m_acOutput[0]=='.'))) ucBlank = TRUE;
			else ucBlank = FALSE;

			switch(m_ucSetActivity) {
				case 1:
					if(ucBlank == TRUE) {
						m_fAddInventory_Activity = -999;
					}
					else {
						m_fAddInventory_Activity = atof(m_acOutput);
						m_fAddInventory_Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) m_fAddInventory_Activity /= BQFACTOR;
					}

					if(m_ucAddInventory_MeasuredNuclide != 0xFF){
						m_dtmAddInventory_Date = 0;
						m_ucAddInventory_MeasuredNuclide = 0xFF;
					}

					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 2:
					if(ucBlank == TRUE) {
						m_fWithdrawInventory_WithdrawActivity = -999;

					}else {
						m_fWithdrawInventory_WithdrawActivity = atof(m_acOutput);
						m_fWithdrawInventory_WithdrawActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) m_fWithdrawInventory_WithdrawActivity /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 3:
					if(ucBlank == TRUE) {
						m_fKitInventory_KitActivity = -999;

					}else {
						m_fKitInventory_KitActivity = atof(m_acOutput);
						m_fKitInventory_KitActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) m_fKitInventory_KitActivity /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 4:
					if(ucBlank == TRUE) {
						m_fInfo_InputUnitlessActivity = -999;
						m_ucInfo_InputUnit = 0xFF;
					}else{
						m_fInfo_InputUnitlessActivity = atof(m_acOutput);
						m_fInfo_InputUnitlessActivity /= unitfact[m_ucUnit];
						m_ucInfo_InputUnit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 5:
					if(ucBlank == TRUE){
						m_fInfo_FromActivity = -999;
						m_ucInfo_FromUnit = 0xFF;
					}else{
						m_fInfo_FromActivity = atof(m_acOutput);
						m_fInfo_FromActivity /= unitfact[m_ucUnit];
						m_ucInfo_FromUnit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 6:
					if(ucBlank == TRUE){
						m_fInfo_ToActivity = -999;
						m_ucInfo_ToUnit = 0xFF;
					}else{
						m_fInfo_ToActivity = atof(m_acOutput);
						m_fInfo_ToActivity /= unitfact[m_ucUnit];
						m_ucInfo_ToUnit = m_ucUnit;
						m_dtmInfo_ToTime = 0;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 7:
					if(ucBlank == TRUE){
						m_fSetupSources_Co57Activity = -999;
						//m_ucSetupSources_Co57Unit = 0xFF;
					}else{
						m_fSetupSources_Co57Activity = atof(m_acOutput);
						m_fSetupSources_Co57Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) m_fSetupSources_Co57Activity /= BQFACTOR;
						//m_ucSetupSources_Co57Unit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 8:
					if(ucBlank == TRUE){
						m_fSetupSources_Co60Activity = -999;
						//m_ucSetupSources_Co60Unit = 0xFF;
					}else{
						m_fSetupSources_Co60Activity = atof(m_acOutput);
						m_fSetupSources_Co60Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) m_fSetupSources_Co60Activity /= BQFACTOR;
						//m_ucSetupSources_Co60Unit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 9:
					if(ucBlank == TRUE){
						m_fSetupSources_Ba133Activity = -999;
						//m_ucSetupSources_Ba133Unit = 0xFF;
					}else{
						m_fSetupSources_Ba133Activity = atof(m_acOutput);
						m_fSetupSources_Ba133Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) m_fSetupSources_Ba133Activity /= BQFACTOR;
						//m_ucSetupSources_Ba133Unit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 10:
					if(ucBlank == TRUE){
						m_fSetupSources_Cs137Activity = -999;
						//m_ucSetupSources_Cs137Unit = 0xFF;
					}else{
						m_fSetupSources_Cs137Activity = atof(m_acOutput);
						m_fSetupSources_Cs137Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) m_fSetupSources_Cs137Activity /= BQFACTOR;
						//m_ucSetupSources_Cs137Unit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 11:
					if(ucBlank == TRUE){
						m_fSetupSources_Na22Activity = -999;
						//m_ucSetupSources_Ra266Unit = 0xFF;
					}else{
						m_fSetupSources_Na22Activity = atof(m_acOutput);
						m_fSetupSources_Na22Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) m_fSetupSources_Na22Activity /= BQFACTOR;
						//m_ucSetupSources_Ra266Unit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
					if(ucBlank == TRUE){
						m_tSetupNuclide[m_ucSetActivity - 12].fHalflife = -1;
						m_tSetupNuclide[m_ucSetActivity - 12].sHalflifeUnit = -1;
					}else{
						m_tSetupNuclide[m_ucSetActivity - 12].fHalflife = atof(m_acOutput);
						sTemp = m_ucUnit;
						sTemp -= 15;
						sTemp *= -1;
						m_tSetupNuclide[m_ucSetActivity - 12].sHalflifeUnit = sTemp;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 22:
					if(ucBlank!=TRUE){
						AmuletDoseTableMenu_activity = atof(m_acOutput);
						AmuletDoseTableMenu_activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletDoseTableMenu_activity /= BQFACTOR;
						//AmuletDoseTableMenu_activityUnit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 23:
					if(ucBlank!=TRUE){
						AmuletDoseTableMenu_dose = atof(m_acOutput);
						AmuletDoseTableMenu_dose /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletDoseTableMenu_dose /= BQFACTOR;
						//AmuletDoseTableMenu_doseUnit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 24:
					if(ucBlank!=TRUE){
						fActivity = atof(m_acOutput);
						fActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) fActivity /= BQFACTOR;

						if(fActivity > 9.999e-003){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MAX_IS_369_9_MBQ_ENGLISH, formatstring);    // "Maximum is 369.9 MBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MAX_IS_9999_UCI_ENGLISH, formatstring);    // "Maximum is 9999 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}
						if(fActivity < 1.0e-007){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MIN_IS_3_701_KBQ_ENGLISH, formatstring);    // "Minimum is 3.701 kBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MIN_IS_1_UCI_ENGLISH, formatstring);    // "Minimum is .1 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}

						AmuletWellMeasureEfficiency_calibrationActivity = atof(m_acOutput);
						AmuletWellMeasureEfficiency_calibrationActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletWellMeasureEfficiency_calibrationActivity /= BQFACTOR;
						//AmuletWellMeasureEfficiency_calibrationActivityUnit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 25:
					if(ucBlank!=TRUE){
						fActivity = atof(m_acOutput);
						fActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) fActivity /= BQFACTOR;

						if(fActivity > 9.999e-003){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MAX_IS_369_9_MBQ_ENGLISH, formatstring);    // "Maximum is 369.9 MBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MAX_IS_9999_UCI_ENGLISH, formatstring);    // "Maximum is 9999 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}
						if(fActivity < 1.0e-007){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MIN_IS_3_701_KBQ_ENGLISH, formatstring);    // "Minimum is 3.701 kBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MIN_IS_1_UCI_ENGLISH, formatstring);    // "Minimum is .1 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}

						AmuletWellSetupTestSourceMenu_act = atof(m_acOutput);
						AmuletWellSetupTestSourceMenu_act /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletWellSetupTestSourceMenu_act /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 26:
					if(ucBlank == TRUE){
						AmuletWellAddEditUserNuclides_temp.halflife = -1.0;
						AmuletWellAddEditUserNuclides_temp.hlunit = -1;
					}else{
						AmuletWellAddEditUserNuclides_temp.halflife = atof(m_acOutput);
						sTemp = m_ucUnit;
						sTemp -= 15;
						sTemp *= -1;
						AmuletWellAddEditUserNuclides_temp.hlunit = sTemp;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 27:
					if(!ucBlank){
						AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Threshold = atof(m_acOutput);
						AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Threshold /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Threshold /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 28:
					if(!ucBlank){
						AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Threshold = atof(m_acOutput);
						AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Threshold /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Threshold /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 29:
					if(!ucBlank){
						AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Threshold = atof(m_acOutput);
						AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Threshold /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Threshold /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 30:
					if(!ucBlank){
						fActivity = atof(m_acOutput);
						fActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) fActivity /= BQFACTOR;

						if(fActivity > 9.999e-003){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MAX_IS_369_9_MBQ_ENGLISH, formatstring);    // "Maximum is 369.9 MBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MAX_IS_9999_UCI_ENGLISH, formatstring);    // "Maximum is 9999 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}
						if(fActivity < 1.0e-007){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MIN_IS_3_701_KBQ_ENGLISH, formatstring);    // "Minimum is 3.701 kBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MIN_IS_1_UCI_ENGLISH, formatstring);    // "Minimum is .1 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity = atof(m_acOutput);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Activity /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 31:
					if(!ucBlank){
						fActivity = atof(m_acOutput);
						fActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) fActivity /= BQFACTOR;

						if(fActivity > 9.999e-003){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MAX_IS_369_9_MBQ_ENGLISH, formatstring);    // "Maximum is 369.9 MBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MAX_IS_9999_UCI_ENGLISH, formatstring);    // "Maximum is 9999 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}
						if(fActivity < 1.0e-007){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MIN_IS_3_701_KBQ_ENGLISH, formatstring);    // "Minimum is 3.701 kBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MIN_IS_1_UCI_ENGLISH, formatstring);    // "Minimum is .1 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity = atof(m_acOutput);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Activity /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 32:
					if(!ucBlank){
						fActivity = atof(m_acOutput);
						fActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) fActivity /= BQFACTOR;

						if(fActivity > 9.999e-003){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MAX_IS_369_9_MBQ_ENGLISH, formatstring);    // "Maximum is 369.9 MBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MAX_IS_9999_UCI_ENGLISH, formatstring);    // "Maximum is 9999 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}
						if(fActivity < 1.0e-007){
							get_amulet_message(L_INPUT_ERROR_ENGLISH, titlestring);    // "Input Error"
							if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5){
								get_amulet_message(L_MIN_IS_3_701_KBQ_ENGLISH, formatstring);    // "Minimum is 3.701 kBq"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}else{
								get_amulet_message(L_MIN_IS_1_UCI_ENGLISH, formatstring);    // "Minimum is .1 uCi"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
							}
							return;
						}

						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity = atof(m_acOutput);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Activity /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 33:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeTest_test.CalibrationActivity = atof(m_acOutput);
						AmuletAddEditThyroidUptakeTest_test.CalibrationActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletAddEditThyroidUptakeTest_test.CalibrationActivity /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 34:
					if(!ucBlank){
						AmuletAddEditRBCSurvivalTest_test.CalibrationActivity = atof(m_acOutput);
						AmuletAddEditRBCSurvivalTest_test.CalibrationActivity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) AmuletAddEditRBCSurvivalTest_test.CalibrationActivity /= BQFACTOR;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				case 35:
					if(ucBlank == TRUE){
						m_fSetupSources_Ge68Activity = -999;
						//m_ucSetupSources_Ra266Unit = 0xFF;
					}else{
						m_fSetupSources_Ge68Activity = atof(m_acOutput);
						m_fSetupSources_Ge68Activity /= unitfact[m_ucUnit];
						if(m_ucUnit==3 || m_ucUnit==4 || m_ucUnit==5) m_fSetupSources_Ge68Activity /= BQFACTOR;
						//m_ucSetupSources_Ra266Unit = m_ucUnit;
					}
					SetAmuletBackHTML();
					m_ucSetActivity = 0;
					break;

				default:
					m_iPhase = PHASE_ACTIVITY_WAIT;
					break;
			}
			break;

		case PHASE_ACTIVITY_DISPLAY:
			switch(m_ucUnit){
				case 0:
					send_to_amulet_string(104, "uCi");
					break;

				case 1:
					send_to_amulet_string(104, "mCi");
					break;

				case 2:
					send_to_amulet_string(104, "Ci");
					break;

				case 3:
					send_to_amulet_string(104, "kBq");
					break;

				case 4:
					send_to_amulet_string(104, "MBq");
					break;

				case 5:
					send_to_amulet_string(104, "GBq");
					break;

				case 11:
					send_amulet_message(L_YR, 104);    // " Yr"
					break;

				case 12:
					send_amulet_message(L_DAY, 104);    // " Day"
					break;

				case 13:
					send_amulet_message(L_HR4, 104);    // " Hr"
					break;

				case 14:
					send_amulet_message(L_MIN, 104);    // " Min"
					break;

				case 15:
					send_amulet_message(L_SEC, 104);    // " Sec"
					break;
			}
			send_to_amulet_string(102, m_acOutput);
			SetAmuletByte(101, 0xFF);

			m_iPhase = PHASE_ACTIVITY_WAIT;
			break;

		case PHASE_ACTIVITY_GBq:
			beep_amulet();
			m_ucUnit = 5;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_MBq:
			beep_amulet();
			m_ucUnit = 4;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_YEAR:
			beep_amulet();
			m_ucUnit = 11;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_DAY:
			beep_amulet();
			m_ucUnit = 12;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_HOUR:
			beep_amulet();
			m_ucUnit = 13;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_MINUTE:
			beep_amulet();
			m_ucUnit = 14;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_SECOND:
			beep_amulet();
			m_ucUnit = 15;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;

		case PHASE_ACTIVITY_KBq:
			beep_amulet();
			m_ucUnit = 3;
			m_iPhase = PHASE_ACTIVITY_DISPLAY;
			break;
	}
}
