#define PHASE_KEYBOARD_PRE_INIT 0
#define PHASE_KEYBOARD_WAIT 1
#define PHASE_KEYBOARD_1 2
#define PHASE_KEYBOARD_2 3
#define PHASE_KEYBOARD_3 4
#define PHASE_KEYBOARD_4 5
#define PHASE_KEYBOARD_5 6
#define PHASE_KEYBOARD_6 7
#define PHASE_KEYBOARD_7 8
#define PHASE_KEYBOARD_8 9
#define PHASE_KEYBOARD_9 10
#define PHASE_KEYBOARD_0 11
#define PHASE_KEYBOARD_BACKSPACE 12
#define PHASE_KEYBOARD_QUP 13
#define PHASE_KEYBOARD_QLO 14
#define PHASE_KEYBOARD_WUP 15
#define PHASE_KEYBOARD_WLO 16
#define PHASE_KEYBOARD_EUP 17
#define PHASE_KEYBOARD_ELO 18
#define PHASE_KEYBOARD_RUP 19
#define PHASE_KEYBOARD_RLO 20
#define PHASE_KEYBOARD_TUP 21
#define PHASE_KEYBOARD_TLO 22
#define PHASE_KEYBOARD_YUP 23
#define PHASE_KEYBOARD_YLO 24
#define PHASE_KEYBOARD_UUP 25
#define PHASE_KEYBOARD_ULO 26
#define PHASE_KEYBOARD_IUP 27
#define PHASE_KEYBOARD_ILO 28
#define PHASE_KEYBOARD_OUP 29
#define PHASE_KEYBOARD_OLO 30
#define PHASE_KEYBOARD_PUP 31
#define PHASE_KEYBOARD_PLO 32
#define PHASE_KEYBOARD_BACKSLASH 33
#define PHASE_KEYBOARD_AUP 34
#define PHASE_KEYBOARD_ALO 35
#define PHASE_KEYBOARD_SUP 36
#define PHASE_KEYBOARD_SLO 37
#define PHASE_KEYBOARD_DUP 38
#define PHASE_KEYBOARD_DLO 39
#define PHASE_KEYBOARD_FUP 40
#define PHASE_KEYBOARD_FLO 41
#define PHASE_KEYBOARD_GUP 42
#define PHASE_KEYBOARD_GLO 43
#define PHASE_KEYBOARD_HUP 44
#define PHASE_KEYBOARD_HLO 45
#define PHASE_KEYBOARD_JUP 46
#define PHASE_KEYBOARD_JLO 47
#define PHASE_KEYBOARD_KUP 48
#define PHASE_KEYBOARD_KLO 49
#define PHASE_KEYBOARD_LUP 50
#define PHASE_KEYBOARD_LLO 51
#define PHASE_KEYBOARD_ZUP 52
#define PHASE_KEYBOARD_ZLO 53
#define PHASE_KEYBOARD_XUP 54
#define PHASE_KEYBOARD_XLO 55
#define PHASE_KEYBOARD_CUP 56
#define PHASE_KEYBOARD_CLO 57
#define PHASE_KEYBOARD_VUP 58
#define PHASE_KEYBOARD_VLO 59
#define PHASE_KEYBOARD_BUP 60
#define PHASE_KEYBOARD_BLO 61
#define PHASE_KEYBOARD_NUP 62
#define PHASE_KEYBOARD_NLO 63
#define PHASE_KEYBOARD_MUP 64
#define PHASE_KEYBOARD_MLO 65
#define PHASE_KEYBOARD_MINUS 66
#define PHASE_KEYBOARD_PLUS 67
#define PHASE_KEYBOARD_DIV 68
#define PHASE_KEYBOARD_MULT 69
#define PHASE_KEYBOARD_SPACE 70
#define PHASE_KEYBOARD_PERIOD 71
#define PHASE_KEYBOARD_COMMA 72
#define PHASE_KEYBOARD_COLON 73
#define PHASE_KEYBOARD_ENTER 74
#define PHASE_KEYBOARD_APPEND 75
#define PHASE_KEYBOARD_PERCENT	76
#define PHASE_KEYBOARD_NTILDEUP	77
#define PHASE_KEYBOARD_NTILDELO	78
#define PHASE_KEYBOARD_UUMUP	79
#define PHASE_KEYBOARD_UUMLO	80
#define PHASE_KEYBOARD_AACCENTUP	81
#define PHASE_KEYBOARD_AACCENTLO	82
#define PHASE_KEYBOARD_EACCENTUP	83
#define PHASE_KEYBOARD_EACCENTLO	84
#define PHASE_KEYBOARD_IACCENTUP	85
#define PHASE_KEYBOARD_IACCENTLO	86
#define PHASE_KEYBOARD_OACCENTUP	87
#define PHASE_KEYBOARD_OACCENTLO	88
#define PHASE_KEYBOARD_UACCENTUP	89
#define PHASE_KEYBOARD_UACCENTLO	90

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "ff.h"
#include "amulet.h"
#include "nuc.h"
#include "keyboard.h"
#include "mca.h"
#include "database.h"

extern int m_iPhase;
extern CURRENT  current;
extern char m_acOutput[100];
extern volatile char m_acTitle[52];
extern unsigned char keyboardCase;
extern bool keyboardWildcard;
extern unsigned char m_ucSetKeyBoard;
extern unsigned char m_ucLenLimit;
extern char m_acAddInventory_Lot[17];
extern char m_acKitInventory_KitLot[17];
extern char m_acSetupSources_Co57SN[11];
extern char m_acSetupSources_Co60SN[11];
extern char m_acSetupSources_Ba133SN[11];
extern char m_acSetupSources_Cs137SN[11];
extern char m_acSetupSources_Na22SN[11];
extern char m_acSetupSources_Ge68SN[11];
extern SETUP_NUCLIDE m_tSetupNuclide[10];
extern char AmuletWellSetupTestSourceMenu_SN[11];
extern char AmuletSetupLinearityLineatorMenu_lineatorSerial[11];
extern char AmuletSetupLinearityCalicheckMenu_calicheckSerial[11];
extern WELLWIPELOCATION AmuletWellSetupAddEditLocation_wellWipeLocation;
extern char AmuletInactivate_comment[26];
extern TESTIDENT *AmuletTestIdentMenu_info;
extern NUCDATA AmuletWellAddEditUserNuclides_temp;
extern char AmuletLoginMenu_username[26];
extern char AmuletLoginMenu_password[26];
extern USER AmuletAddEditUser_user;
extern char AmuletAddEditUser_passwordEcho[26];
extern PROBEBIOASSAYTEST AmuletWellBioAssayAnalysis_test;
extern PROBETHYROIDUPTAKEPROTOCOL AmuletAddEditThyroidUptakeProtocol_protocol;
extern char AmuletWellThyroidUptake_lastName[14];
extern char AmuletWellThyroidUptake_patientID[11];
extern PROBETHYROIDUPTAKETEST AmuletAddEditThyroidUptakeTest_test;
extern PROBETHYROIDUPTAKEPATMEAS AmuletWellThyroidUptakeMeasurePatient;
extern char AmuletWellRBCSurvival_lastName[18];
extern char AmuletWellRBCSurvival_patientID[11];
extern WELLRBCSURVIVALTEST AmuletAddEditRBCSurvivalTest_test;
extern WELLRBCSURVIVALMEAS AmuletWellRBCSurvivalMeasureMenu_measurement;
extern char AmuletGenericYesNo_comment[26];
extern HALFLIFECALC HalflifeCalc_test;
extern char AmuletSetupKey_setupKey[50];

void send_to_amulet_string(uchar ucIndex, char message0[]);
void SetAmuletBackHTML(void);
void trim(char *acByte);
void DB_shutdown(void);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);

void AmuletKeyboard_menu(void) {
	FILINFO fileInfo;
	char longFileName[100];
	unsigned char keyChar[2];

	switch(m_iPhase) {
		case PHASE_KEYBOARD_PRE_INIT:
			if(current.language == 1){
				SetAmuletByte(99, 0xFF);

				keyChar[0] = 180;
				keyChar[1] = 0;
				send_to_amulet_string(200, (char *) keyChar);

				keyChar[0] = 209;
				send_to_amulet_string(201, (char *) keyChar);

				keyChar[0] = 241;
				send_to_amulet_string(202, (char *) keyChar);

				keyChar[0] = 220;
				send_to_amulet_string(203, (char *) keyChar);

				keyChar[0] = 252;
				send_to_amulet_string(204, (char *) keyChar);

				keyChar[0] = 193;
				send_to_amulet_string(205, (char *) keyChar);

				keyChar[0] = 201;
				send_to_amulet_string(206, (char *) keyChar);

				keyChar[0] = 205;
				send_to_amulet_string(207, (char *) keyChar);

				keyChar[0] = 211;
				send_to_amulet_string(208, (char *) keyChar);

				keyChar[0] = 218;
				send_to_amulet_string(209, (char *) keyChar);

				keyChar[0] = 225;
				send_to_amulet_string(210, (char *) keyChar);

				keyChar[0] = 233;
				send_to_amulet_string(211, (char *) keyChar);

				keyChar[0] = 237;
				send_to_amulet_string(212, (char *) keyChar);

				keyChar[0] = 243;
				send_to_amulet_string(213, (char *) keyChar);

				keyChar[0] = 250;
				send_to_amulet_string(214, (char *) keyChar);
			}

			m_acOutput[0] = 0;
			send_to_amulet_string(102, m_acOutput);
			SetAmuletByte(100, 0xFF);

			send_amulet_message(L_BACKSPACE, 105);    // "Backspace"
			send_amulet_message(L_SHIFT, 106);    // "Shift"
			send_amulet_message(L_WILDCARD, 107);    // "<-- Wildcard"
			send_amulet_message(L_ACCEPT_BUTTON, 108);    // "Accept"
			send_amulet_message(L_CANCEL_BUTTON, 109);    // "Cancel"

			send_to_amulet_string(100, (char *)m_acTitle);
			SetAmuletByte(101, 0xFF);

			if(current.language == 1) SetAmuletByte(106, 0xFF);

			switch(keyboardCase){
				case 0:
					if(current.language == 1) SetAmuletByte(107, 0xFF);
					break;

				case 1:
					SetAmuletByte(102, 0xFF);
					break;

				case 2:
					SetAmuletByte(103, 0xFF);
					break;

			}
			keyboardCase = 0;

			if(keyboardWildcard) SetAmuletByte(105, 0xFF);
			else SetAmuletByte(105, 0x00);

			m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_WAIT:
			break;

		case PHASE_KEYBOARD_1:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "1");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_2:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "2");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_3:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "3");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_4:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "4");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_5:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "5");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_6:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "6");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_7:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "7");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_8:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "8");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_9:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "9");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_0:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "0");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_BACKSPACE:
			beep_amulet();
			if (strlen(m_acOutput) > 0) {
				m_acOutput[strlen(m_acOutput) - 1] = 0;
				send_to_amulet_string(102, m_acOutput);
				SetAmuletByte(100, 0xFF);
			}
			m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_QUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "Q");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_QLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "q");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_WUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "W");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_WLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "w");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_EUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "E");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_ELO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "e");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_RUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "R");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_RLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "r");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_TUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "T");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_TLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "t");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_YUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "Y");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_YLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "y");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_UUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "U");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_ULO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "u");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_IUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "I");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_ILO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "i");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_OUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "O");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_OLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "o");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_PUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "P");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_PLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "p");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_BACKSLASH:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "\\");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_AUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "A");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_ALO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "a");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_SUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "S");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_SLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "s");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_DUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "D");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_DLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "d");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_FUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "F");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_FLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "f");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_GUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "G");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_GLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "g");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_HUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "H");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_HLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "h");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_JUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "J");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_JLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "j");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_KUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "K");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_KLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "k");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_LUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "L");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_LLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "l");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_ZUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "Z");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_ZLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "z");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_XUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "X");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_XLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "x");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_CUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "C");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_CLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "c");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_VUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "V");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_VLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "v");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_BUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "B");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_BLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "b");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_NUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "N");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_NLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "n");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_MUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "M");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_MLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "m");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_MINUS:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "-");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_PLUS:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "+");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_DIV:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "/");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_MULT:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "*");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_SPACE:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, " ");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_PERIOD:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, ".");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_COMMA:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, ",");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_COLON:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, ":");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_PERCENT:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "%");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_NTILDEUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "Ñ");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_NTILDELO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "ñ");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_UUMUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "Ü");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_UUMLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "ü");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_AACCENTUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "Á");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_AACCENTLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "á");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_EACCENTUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "É");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_EACCENTLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "é");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_IACCENTUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "Í");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_IACCENTLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "í");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_OACCENTUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "Ó");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_OACCENTLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "ó");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_UACCENTUP:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "Ú");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_UACCENTLO:
			beep_amulet();
			if (strlen(m_acOutput) < m_ucLenLimit) {
				strcat(m_acOutput, "ú");
				m_iPhase = PHASE_KEYBOARD_APPEND;
			}
			else m_iPhase = PHASE_KEYBOARD_WAIT;
			break;

		case PHASE_KEYBOARD_ENTER:
			beep_amulet();
			switch(m_ucSetKeyBoard) {
				case 1:
					strcpy(m_acAddInventory_Lot, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 2:
					strcpy(m_acKitInventory_KitLot, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 3:
					strcpy(m_acSetupSources_Co57SN, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 4:
					strcpy(m_acSetupSources_Co60SN, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;
				case 5:
					strcpy(m_acSetupSources_Ba133SN, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 6:
					strcpy(m_acSetupSources_Cs137SN, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 7:
					strcpy(m_acSetupSources_Na22SN, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
					strcpy(m_tSetupNuclide[m_ucSetKeyBoard - 8].acName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27:
					strcpy(m_tSetupNuclide[m_ucSetKeyBoard - 18].acElement, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 28:
					strcpy(AmuletWellSetupTestSourceMenu_SN, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 29:
					strcpy(AmuletSetupLinearityLineatorMenu_lineatorSerial, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 30:
					strcpy(AmuletSetupLinearityCalicheckMenu_calicheckSerial, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 31:
					strcpy(AmuletWellSetupAddEditLocation_wellWipeLocation.Name, m_acOutput);
					trim(AmuletWellSetupAddEditLocation_wellWipeLocation.Name);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 32:
					strcpy(AmuletInactivate_comment, m_acOutput);
					trim(AmuletInactivate_comment);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 33:
					strcpy(AmuletTestIdentMenu_info->TestID, m_acOutput);
					trim(AmuletTestIdentMenu_info->TestID);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 34:
					strcpy(AmuletTestIdentMenu_info->PatientID, m_acOutput);
					trim(AmuletTestIdentMenu_info->PatientID);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 35:
					strcpy(AmuletTestIdentMenu_info->FirstName, m_acOutput);
					trim(AmuletTestIdentMenu_info->FirstName);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 36:
					strcpy(AmuletTestIdentMenu_info->LastName, m_acOutput);
					trim(AmuletTestIdentMenu_info->LastName);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 37:
					if((strcmp(m_acOutput, "M") == 0) || (strcmp(m_acOutput, "m") == 0)){
						strcpy(AmuletTestIdentMenu_info->Sex, "M");
						SetAmuletBackHTML();
						m_ucSetKeyBoard = 0;
					}else if((strcmp(m_acOutput, "F") == 0) || (strcmp(m_acOutput, "f") == 0)){
						strcpy(AmuletTestIdentMenu_info->Sex, "F");
						SetAmuletBackHTML();
						m_ucSetKeyBoard = 0;
					}else if(strcmp(m_acOutput, "") == 0){
						AmuletTestIdentMenu_info->Sex[0] = 0;
						SetAmuletBackHTML();
						m_ucSetKeyBoard = 0;
					}else{
						m_acOutput[0] = 0;
						send_to_amulet_string(102, m_acOutput);
						SetAmuletByte(100, 0xFF);
						SetAmuletByte(104, 0xFF);
						m_iPhase = PHASE_KEYBOARD_WAIT;
					}
					break;

				case 38:
					strcpy(AmuletTestIdentMenu_info->Physician, m_acOutput);
					trim(AmuletTestIdentMenu_info->Physician);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 39:
					strcpy(AmuletTestIdentMenu_info->TechID, m_acOutput);
					trim(AmuletTestIdentMenu_info->TechID);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 40:
					if(strcmp(m_acOutput, "erase ALL data") == 0){
						SetAmuletString(102, "Starting");
						SetAmuletByte(100, 0xFF);
						DB_shutdown();
						SetAmuletString(102, "Shutdown DB");
						SetAmuletByte(100, 0xFF);
						service_watchdog();
						f_chdrive(0);
						f_chdir("/data");
						service_watchdog();
						fileInfo.lfname = longFileName;
						fileInfo.lfsize = 100;
						if(f_stat("capintec.db", &fileInfo) == FR_OK) f_unlink("capintec.db");
						SetAmuletString(102, "Erased DB");
						SetAmuletByte(100, 0xFF);
						if(f_stat("capintec.db-journal", &fileInfo) == FR_OK) f_unlink("capintec.db-journal");
						SetAmuletString(102, "Erased Journal");
						SetAmuletByte(100, 0xFF);
						service_watchdog();
						initDB(TRUE);
						SetAmuletString(102, "Created DB");
						SetAmuletByte(100, 0xFF);
						service_watchdog();
						Mca_saveCurrentDetector();
						SetAmuletString(102, "Wrote Data 1");
						SetAmuletByte(100, 0xFF);
						service_watchdog();
						DB_WriteBranding(current.branding);
						SetAmuletString(102, "Wrote Data 2");
						SetAmuletByte(100, 0xFF);
						service_watchdog();
						DB_shutdown();
						SetAmuletString(102, "Please restart");
						SetAmuletByte(100, 0xFF);
						Amulet_DisplayNotification("Erase All Database Data", "Database has been cleared.\nPlease restart.", FALSE);
						m_ucSetKeyBoard = 0;
					}else{
						Amulet_DisplayError("Pass Phrase Error", "Incorrect Pass Phrase", TRUE);
					}
					return;

				case 41:
					strcpy(AmuletWellAddEditUserNuclides_temp.name, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 42:
					strcpy(AmuletWellAddEditUserNuclides_temp.fullname, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 43:
					strcpy(AmuletLoginMenu_username, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 44:
					strcpy(AmuletLoginMenu_password, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 45:
					strcpy(AmuletAddEditUser_user.UserName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 46:
					strcpy(AmuletAddEditUser_user.Password, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 47:
					strcpy(AmuletAddEditUser_passwordEcho, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 48:
					strcpy(AmuletAddEditUser_user.FirstName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 49:
					strcpy(AmuletAddEditUser_user.LastName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 50:
					strcpy(AmuletWellBioAssayAnalysis_test.Comment, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 51:
					strcpy(AmuletAddEditThyroidUptakeProtocol_protocol.ProtocolName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 52:
					strcpy(AmuletWellThyroidUptake_lastName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 53:
					strcpy(AmuletWellThyroidUptake_patientID, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 54:
					strcpy(AmuletAddEditThyroidUptakeTest_test.FirstName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 55:
					strcpy(AmuletAddEditThyroidUptakeTest_test.LastName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 56:
					strcpy(AmuletAddEditThyroidUptakeTest_test.PatientID, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 57:
					if((strcmp(m_acOutput, "M") == 0) || (strcmp(m_acOutput, "m") == 0)){
						strcpy(AmuletAddEditThyroidUptakeTest_test.Sex, "M");
						SetAmuletBackHTML();
						m_ucSetKeyBoard = 0;
					}else if((strcmp(m_acOutput, "F") == 0) || (strcmp(m_acOutput, "f") == 0)){
						strcpy(AmuletAddEditThyroidUptakeTest_test.Sex, "F");
						SetAmuletBackHTML();
						m_ucSetKeyBoard = 0;
					}else if(strcmp(m_acOutput, "") == 0){
						AmuletAddEditThyroidUptakeTest_test.Sex[0] = 0;
						SetAmuletBackHTML();
						m_ucSetKeyBoard = 0;
					}else{
						m_acOutput[0] = 0;
						send_to_amulet_string(102, m_acOutput);
						SetAmuletByte(100, 0xFF);
						SetAmuletByte(104, 0xFF);
						m_iPhase = PHASE_KEYBOARD_WAIT;
					}
					break;

				case 58:
					strcpy(AmuletAddEditThyroidUptakeTest_test.Physician, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 59:
					strcpy(AmuletAddEditThyroidUptakeTest_test.TechID, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 60:
					strcpy(AmuletAddEditThyroidUptakeTest_test.LotNum, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 61:
					strcpy(AmuletWellThyroidUptakeMeasurePatient.comment, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 62:
					strcpy(AmuletWellRBCSurvival_lastName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 63:
					strcpy(AmuletWellRBCSurvival_patientID, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 64:
					strcpy(AmuletAddEditRBCSurvivalTest_test.FirstName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 65:
					strcpy(AmuletAddEditRBCSurvivalTest_test.LastName, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 66:
					strcpy(AmuletAddEditRBCSurvivalTest_test.PatientID, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 67:
					if((strcmp(m_acOutput, "M") == 0) || (strcmp(m_acOutput, "m") == 0)){
						strcpy(AmuletAddEditRBCSurvivalTest_test.Sex, "M");
						SetAmuletBackHTML();
						m_ucSetKeyBoard = 0;
					}else if((strcmp(m_acOutput, "F") == 0) || (strcmp(m_acOutput, "f") == 0)){
						strcpy(AmuletAddEditRBCSurvivalTest_test.Sex, "F");
						SetAmuletBackHTML();
						m_ucSetKeyBoard = 0;
					}else if(strcmp(m_acOutput, "") == 0){
						AmuletAddEditRBCSurvivalTest_test.Sex[0] = 0;
						SetAmuletBackHTML();
						m_ucSetKeyBoard = 0;
					}else{
						m_acOutput[0] = 0;
						send_to_amulet_string(102, m_acOutput);
						SetAmuletByte(100, 0xFF);
						SetAmuletByte(104, 0xFF);
						m_iPhase = PHASE_KEYBOARD_WAIT;
					}
					break;

				case 68:
					strcpy(AmuletAddEditRBCSurvivalTest_test.Physician, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 69:
					strcpy(AmuletAddEditRBCSurvivalTest_test.TechID, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 70:
					strcpy(AmuletAddEditRBCSurvivalTest_test.LotNum, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 71:
					strcpy(AmuletWellRBCSurvivalMeasureMenu_measurement.comment, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 72:
					strcpy(AmuletGenericYesNo_comment, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 73:
					strcpy(HalflifeCalc_test.LotNum, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 74:
					strcpy(AmuletSetupKey_setupKey, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				case 75:
					strcpy(m_acSetupSources_Ge68SN, m_acOutput);
					SetAmuletBackHTML();
					m_ucSetKeyBoard = 0;
					break;

				default:
					m_iPhase = PHASE_KEYBOARD_WAIT;
					break;
			}
			break;

		case PHASE_KEYBOARD_APPEND:
			send_to_amulet_string(102, m_acOutput);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_KEYBOARD_WAIT;
			break;
	}
}
