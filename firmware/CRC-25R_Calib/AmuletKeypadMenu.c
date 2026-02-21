#define PHASE_KEYPAD_PRE_INIT 0
#define PHASE_KEYPAD_WAIT 1
#define PHASE_KEYPAD_7 2
#define PHASE_KEYPAD_8 3
#define PHASE_KEYPAD_9 4
#define PHASE_KEYPAD_4 5
#define PHASE_KEYPAD_5 6
#define PHASE_KEYPAD_6 7
#define PHASE_KEYPAD_1 8
#define PHASE_KEYPAD_2 9
#define PHASE_KEYPAD_3 10
#define PHASE_KEYPAD_0 11
#define PHASE_KEYPAD_DIV 12
#define PHASE_KEYPAD_MULT 13
#define PHASE_KEYPAD_SUB 14
#define PHASE_KEYPAD_ADD 15
#define PHASE_KEYPAD_BACK 16
#define PHASE_KEYPAD_ENTER 17
#define PHASE_KEYPAD_DISPLAY 18
#define PHASE_KEYPAD_PERIOD 19
#define PHASE_KEYPAD_PLUSMINUS 20

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "chambfac.h"
#include "screen.h"
#include "nuc.h"
#include "mca.h"
#include "database.h"

extern CURRENT  current;
extern MEASUREMENT measurement[];
extern CHAMBER chamber[];
extern char m_cDivPos;
extern char m_cMultPos;
extern char m_cPeriodPos;
extern char m_acOutput[100];
extern volatile char m_acTitle[52];
extern int m_iPhase;
extern unsigned char m_ucClear;
extern unsigned char m_ucKeyPadConfig;
extern bool m_flgShowPlusMinus;
extern float m_fMaxValue;
extern float m_fMinValue;
extern unsigned short int m_uiSetKeyPad;
extern volatile bool DisplayActivity_SendActivity;
extern unsigned char m_ucAddInventory_ID;
extern float m_fAddInventory_Volume;
extern float m_fAddInventory_MoPerTc;
extern char m_cKitInventory_KitID;
extern float m_fKitInventory_KitVolume;
extern unsigned char m_ucSetTime;
extern unsigned char m_ucTimeMode;
extern unsigned char m_ucSecMode;
extern float m_fSetupMoly_MoTcLimit;
extern SETUP_NUCLIDE m_tSetupNuclide[10];
extern char AmuletSetupCalNumMenu_UserRCal[10];
extern char AmuletSetupCalNumMenu_UserPCal[10];
extern char AmuletSetupLinearityStandardMenu_testTotal;
extern short AmuletSetupLinearityStandardMenu_testHour[12];
extern char AmuletSetupLinearityCalicheckMenu_testTotal;
extern float AmuletDoseTableMenu_volume;
extern short AmuletDoseTableMenu_interval;
extern unsigned char m_ucSetWellMeasure;
extern bool AmuletWellMeasurementMenu_WipeBackgroundThreshold;
extern int AmuletWellSetupBioAssayMeasureEfficiencyMenu_countTime;
extern short AmuletWellMeasureMenu_countTime[8];
extern SETUP_EFFICIENCY AmuletWellEditEfficiencies_temp;
extern float moly_volume;
extern char AmuletSetupCalibSerialMenu_serial[11];
extern char AmuletSetupChamber_serial[7];
extern float AmuletSetupChamber_respCorr;
extern float AmuletSetupChamber_nomVolts;
extern WELLWIPETYPE AmuletWellSetupTypes_wellWipeType;
extern float AmuletWellEditFullEfficiency_fullEfficiency;
extern char AmuletWellFactoryMenu_serialNum[7];
extern short AmuletWellFactoryMenu_highVoltage;
extern short AmuletWellFactoryMenu_threshold;
extern WELLSCHILLINGTEST AmuletWellSchillingMenu_test;
extern WELLPLASMATEST AmuletWellPlasmaMenu_test;
extern WELLRBCTEST AmuletWellRBCMenu_test;
extern WELLMDATEST AmuletWellMDATestMenu_mda;
extern WELLCHITEST AmuletWellChiTestMenu_test;
extern char generalNuclideID;
extern float startEV[10];
extern float endEV[10];
extern short AmuletWellStabilityTestMenu_countTime;
extern long AmuletWellStabilityTestMenu_reps;
extern WELLWIPELOCATION AmuletWellSetupAddEditLocation_wellWipeLocation;
extern char AmuletWellFactoryDetectorsMenu_probeSerialNum[7];
extern char AmuletWellFactoryDetectorsMenu_wellSerialNum[7];
extern short AmuletWellFactoryDetectorsMenu_probeHighVoltage;
extern short AmuletWellFactoryDetectorsMenu_wellHighVoltage;
extern short AmuletWellFactoryDetectorsMenu_probeThreshold;
extern short AmuletWellFactoryDetectorsMenu_wellThreshold;
extern NUCDATA AmuletWellAddEditUserNuclides_temp;
extern PROBEBIOASSAYSETTING AmuletWellSetupBioAssayMenu_bioAssaySetting;
extern int AmuletWellBioAssay_probeDistance;
extern PROBEBIOASSAYEFFICIENCY AmuletWellSetupBioAssayEnterEfficiencyMenu_eff;
extern PROBEBIOASSAYEFFICIENCY AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff;
extern float AmuletWellSetupBioAssayEnterROI_I131Lower_keV;
extern float AmuletWellSetupBioAssayEnterROI_I131Upper_keV;
extern float AmuletWellSetupBioAssayEnterROI_I125Lower_keV;
extern float AmuletWellSetupBioAssayEnterROI_I125Upper_keV;
extern float AmuletWellSetupBioAssayEnterROI_I123Lower_keV;
extern float AmuletWellSetupBioAssayEnterROI_I123Upper_keV;
extern PROBETHYROIDUPTAKEPROTOCOL AmuletAddEditThyroidUptakeProtocol_protocol;
extern PROBETHYROIDUPTAKETEST AmuletAddEditThyroidUptakeTest_test;
extern PROBETHYROIDUPTAKENORMAL AmuletWellThyroidUptakeEnterNormal_value[4];
extern WELLRBCSURVIVALTEST AmuletAddEditRBCSurvivalTest_test;
extern WELLRBCSURVIVALMEAS AmuletWellRBCSurvivalMeasureMenu_measurement;
extern float AmuletWellRBCSurvivalEnterNormal_min;
extern float AmuletWellRBCSurvivalEnterNormal_max;
extern time_t clock_time;
extern PROBETHYROIDUPTAKEPATMEAS AmuletWellThyroidUptakeMeasurePatient;
extern PROBETHYROIDUPTAKEDOSETOTALMEAS AmuletWellThyroidUptakeMeasureDose_total;
extern int AmuletAutoLinearityTest_intervalMinutes;
extern int AmuletAutoLinearityTest_totalMinutes;
extern int AmuletHalflifeCalc_intervalSeconds;
extern int AmuletHalflifeCalc_totalSeconds;
extern char AmuletSetupPassword_currentPassword[10];
extern char AmuletSetupPassword_newPassword[10];
extern char AmuletSetupPassword_confirmPassword[10];

void SetAmuletBackHTML(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
bool testdate(char *datestr, bool autoslash);
bool testnumber(char *numstr1);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void trim_and_shrink(char *acByte);
void GetExtendedNuclideString(unsigned char ucNuclideID, char *acMsg);
unsigned char PopPageStack(void);
void PushPageStack(unsigned char ucPage);
unsigned char CheckPassword(char *Passwd);
float NuclideCal_getResponse(char calstrng[], short ch_type);
int MCASetThreshold(short value);
int MCASetHV(short value, bool state);
bool test_date(short mon, short day, short year);
void wildcard(char *input, char *output);

void AmuletKeypad_menu(void) {
	short i, index, stringLength, slashCount, beginIndex;
	short ch_num, ch_type;
	float response;
	unsigned char ucBlank;
	char nucshrink[25];
	float tempfloat, hv, factor, fcode;
	char message[51];
	char formatstring[51], titlestring[51];
	char tempOutput[101];
	char cCountTimeIndex;
	struct tm datetime;
	char timestr[7], secstr[3], minstr[3], hourstr[3], daystr[3], monstr[3], yearstr[5];
	char wildcard_string[100];

	switch(m_iPhase) {
		case PHASE_KEYPAD_PRE_INIT:
			m_cPeriodPos = m_cDivPos = m_cMultPos = -1;// m_cAddPos = m_cSubPos = -1;
			m_acOutput[0] = 0;

			send_to_amulet_string(100, (char *)m_acTitle);
			send_amulet_message(L_BACKSPACE, 104);    // "Backspace"
			send_amulet_message(L_ACCEPT_BUTTON, 105);    // "Accept"
			send_amulet_message(L_CANCEL_BUTTON, 106);    // "Cancel"

			switch (m_ucKeyPadConfig) {
				case 1:
					SetAmuletWord(101, 364);
					SetAmuletWord(102, 420);
					SetAmuletWord(103, 440);
					SetAmuletWord(104, 420);
					break;

				case 2:
					SetAmuletWord(109, 364);
					SetAmuletWord(110, 420);
					break;

				case 3:
					SetAmuletWord(101, 364);
					SetAmuletWord(102, 420);
					break;

				default:
					break;
			}

			SetAmuletByte(100, 0xFF);

			switch(m_ucKeyPadConfig) {
				case 1:
					SetAmuletByte(101, 0xFF);
					SetAmuletByte(102, 0xFF);
					break;

				case 2:
					SetAmuletByte(105, 0xFF);
					break;

				case 3:
					SetAmuletByte(101, 0xFF);
					break;

				default:
					break;
			}

			if(m_flgShowPlusMinus) SetAmuletByte(107, 0xFF);
			m_iPhase = PHASE_KEYPAD_WAIT;
			break;

		case PHASE_KEYPAD_WAIT:
			break;

		case PHASE_KEYPAD_7:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "7");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "7");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_8:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "8");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "8");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_9:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "9");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "9");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_4:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "4");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "4");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_5:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "5");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "5");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_6:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "6");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "6");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_1:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "1");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "1");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_2:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "2");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "2");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_3:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "3");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "3");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_0:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, TRUE)) strcat(m_acOutput, "0");
			}else{
				if(testnumber(m_acOutput)) strcat(m_acOutput, "0");
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_DIV:
			beep_amulet();
			if(m_ucKeyPadConfig == 3){
				if(testdate(m_acOutput, FALSE)) strcat(m_acOutput, "/");
			}else{
				if(strlen(m_acOutput) != 0) {
					if((m_cDivPos == -1) && (m_cMultPos == -1)) {
						if(testnumber(m_acOutput)) {
							m_cDivPos = strlen(m_acOutput);
							strcat(m_acOutput, "/");
						}
					}
				}
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_MULT:
			beep_amulet();
			if(strlen(m_acOutput) != 0) {
				if((m_cDivPos == -1) && (m_cMultPos == -1)) {
					if(testnumber(m_acOutput)) {
						m_cMultPos = strlen(m_acOutput);
						strcat(m_acOutput, "*");
					}
				}
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_SUB:
			beep_amulet();
			if(testnumber(m_acOutput)) strcat(m_acOutput, "-");
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_ADD:
			beep_amulet();
			if(testnumber(m_acOutput)) strcat(m_acOutput, "+");
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_BACK:
			beep_amulet();
			if (strlen(m_acOutput) > 0) {
				if(m_acOutput[strlen(m_acOutput) - 1] == '/') {
					m_cDivPos = -1;
				}
				if(m_acOutput[strlen(m_acOutput) - 1] == '*') {
					m_cMultPos = -1;
				}
				if(m_acOutput[strlen(m_acOutput) - 1] == '.') {
					m_cPeriodPos = -1;
				}
				m_acOutput[strlen(m_acOutput) - 1] = 0;
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_ENTER:
			beep_amulet();
			if((strlen(m_acOutput)==0) || ((strlen(m_acOutput)==1) && (m_acOutput[0]=='.'))) ucBlank = 1;
			else ucBlank = 0;

			if(m_fMaxValue != m_fMinValue){
				if(!ucBlank){
					tempfloat = atof(m_acOutput);
					if(tempfloat > m_fMaxValue){
						get_amulet_message(L_MAXIMUM_IS, formatstring);    // "Maximum is %.4f"
						sprintf(message, formatstring, m_fMaxValue);
						get_amulet_message(L_INPUT_ERROR, titlestring);    // "Input Error"
						Amulet_DisplayError(titlestring, message, TRUE);
						return;
					}else if(tempfloat < m_fMinValue){
						get_amulet_message(L_MINIMUM_IS, formatstring);    // "Minimum is %.4f"
						sprintf(message, formatstring, m_fMinValue);
						get_amulet_message(L_INPUT_ERROR, titlestring);    // "Input Error"
						Amulet_DisplayError(titlestring, message,TRUE);
						return;
					}
				}
			}

			switch(m_uiSetKeyPad) {
				case 1:
					if(ucBlank == 0) {
						for (i=0; i<strlen(m_acOutput); i++) {
							if (m_acOutput[i] == 47) {
								m_acOutput[i] = -127;
							}
						}

						if(current.num_chambers>0){
							ch_num = current.main_chamber;
							ch_type = chamber_type(ch_num);
						}else{
							ch_num = 0;
							ch_type = R_CHAMB;
						}

						response = get_response(m_acOutput, ch_type);

						if (response != -1) {
							erase_screen();
							chamber[ch_num].calkey = TRUE;
							measurement[ch_num].response = response;
							measurement[ch_num].resp0 = response;
							strncpy(&chamber[ch_num].calstrng[0],m_acOutput,10);

							for (i=0; i<strlen(m_acOutput); i++) {
								if (m_acOutput[i] == -127) {
									m_acOutput[i] = 47;
								}
							}
							send_to_amulet_string(3, m_acOutput);
							erase_text_line(56,SMALL,0);
							nucset(ch_num);
							lowresp(ch_num);
							strcpy(&chamber[ch_num].nucdata.name[0],"      ");  /* for PC */
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
						}else{
							PopPageStack();
							get_amulet_message(L_CAL_NUMBER_ERROR, titlestring);    // "Cal Number Error"
							get_amulet_message(L_INVALID_CAL_NUMBER, formatstring);    // "Invalid Cal Number"
							DISPLAY_ERROR(titlestring, formatstring);
						}
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 2:
					if (ucBlank==0) {
						i = atoi(m_acOutput);
					}
					else {
						i = 0;
					}
					m_ucAddInventory_ID = i;
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 3:
					if(ucBlank==1) m_fAddInventory_Volume = -999;
					else m_fAddInventory_Volume = atof(m_acOutput);
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 4:
					if(ucBlank==1) m_fAddInventory_MoPerTc = 0;
					else m_fAddInventory_MoPerTc = atof(m_acOutput);
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 5:
					if(ucBlank==1) i = 0;
					else i = atoi(m_acOutput);
					m_cKitInventory_KitID = i;
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 6:
					if(ucBlank==1) m_fKitInventory_KitVolume = -999;
					else m_fKitInventory_KitVolume = atof(m_acOutput);
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 7:
					if(CheckPassword(m_acOutput)){
						PopPageStack();
						SetAmuletHTML(AmuletHTMLIndex[OTHER_HTM]);
						PushPageStack(AmuletHTMLIndex[OTHER_HTM]);
					}else{
						SetAmuletBackHTML();
					}
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 8:
					if(CheckPassword(m_acOutput)){
						PopPageStack();
						m_ucSetTime = 1;
						get_amulet_message(L_PLEASE_ENTER_CURRENT_TIME, formatstring);    // "Please Enter Current Time:"
						strcpy((char *)m_acTitle, formatstring);
						m_ucTimeMode = 0;
						m_ucSecMode = 0;
						switch(current.time_format){
							case 0:
								SetAmuletHTML(AmuletHTMLIndex[SETTIME2_HTM]);
								PushPageStack(AmuletHTMLIndex[SETTIME2_HTM]);
								break;

							case 1:
								SetAmuletHTML(AmuletHTMLIndex[SETTIME2EUR_HTM]);
								PushPageStack(AmuletHTMLIndex[SETTIME2EUR_HTM]);
								break;

							case 2:
								SetAmuletHTML(AmuletHTMLIndex[SETTIME2JAP_HTM]);
								PushPageStack(AmuletHTMLIndex[SETTIME2JAP_HTM]);
								break;

							default:
								SetAmuletHTML(AmuletHTMLIndex[SETTIME2_HTM]);
								PushPageStack(AmuletHTMLIndex[SETTIME2_HTM]);
								break;
						}
					}else{
						SetAmuletBackHTML();
					}
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 9:
					if(ucBlank!=1){
						m_fSetupMoly_MoTcLimit = atof(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
					if(strlen(m_acOutput)==0){
						if(chamber_C()) strcpy(m_tSetupNuclide[m_uiSetKeyPad - 10].acCCalNum, m_acOutput);
						else if(chamber_K()) strcpy(m_tSetupNuclide[m_uiSetKeyPad - 10].acKCalNum, m_acOutput);
						else strcpy(m_tSetupNuclide[m_uiSetKeyPad - 10].acRCalNum, m_acOutput);
					}else{
						//response = get_response(m_acOutput, R_CHAMB);
						if(chamber_C()) response = NuclideCal_getResponse(m_acOutput, C_CHAMB);
						else if(chamber_K()) response = NuclideCal_getResponse(m_acOutput, K_CHAMB);
						else response = NuclideCal_getResponse(m_acOutput, R_CHAMB);

						if(response == -1){
							PopPageStack();
							get_amulet_message(L_CAL_NUMBER_ERROR, titlestring);    // "Cal Number Error"
							get_amulet_message(L_INVALID_CAL_NUMBER, formatstring);    // "Invalid Cal Number"
							DISPLAY_ERROR(titlestring, formatstring);
						}else{
							if(chamber_C()) strcpy(m_tSetupNuclide[m_uiSetKeyPad - 10].acCCalNum, m_acOutput);
							else if(chamber_K()) strcpy(m_tSetupNuclide[m_uiSetKeyPad - 10].acKCalNum, m_acOutput);
							else strcpy(m_tSetupNuclide[m_uiSetKeyPad - 10].acRCalNum, m_acOutput);
						}
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 20:
				case 21:
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27:
				case 28:
				case 29:
					if(strlen(m_acOutput)==0){
						strcpy(m_tSetupNuclide[m_uiSetKeyPad - 20].acPCalNum, m_acOutput);
					}else{
						//response = get_response(m_acOutput, P_CHAMB);
						response = NuclideCal_getResponse(m_acOutput, P_CHAMB);
						if(response == -1){
							PopPageStack();
							get_amulet_message(L_CAL_NUMBER_ERROR, titlestring);    // "Cal Number Error"
							get_amulet_message(L_INVALID_CAL_NUMBER, formatstring);    // "Invalid Cal Number"
							DISPLAY_ERROR(titlestring, formatstring);
						}else{
							strcpy(m_tSetupNuclide[m_uiSetKeyPad - 20].acPCalNum, m_acOutput);
						}
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 30:
					if(strlen(m_acOutput) == 0){
						strcpy(AmuletSetupCalNumMenu_UserRCal, "EMPTY");
					}else{
						strcpy(AmuletSetupCalNumMenu_UserRCal, m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 31:
					if(strlen(m_acOutput) == 0){
						strcpy(AmuletSetupCalNumMenu_UserPCal, "EMPTY");
					}else{
						strcpy(AmuletSetupCalNumMenu_UserPCal, m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 32:
					if(ucBlank) AmuletSetupLinearityStandardMenu_testTotal = -1;
					else AmuletSetupLinearityStandardMenu_testTotal = atoi(m_acOutput);
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 33:
				case 34:
				case 35:
				case 36:
				case 37:
				case 38:
				case 39:
				case 40:
				case 41:
				case 42:
				case 43:
				case 44:
					if(ucBlank) AmuletSetupLinearityStandardMenu_testHour[m_uiSetKeyPad-33] = -1;
					else AmuletSetupLinearityStandardMenu_testHour[m_uiSetKeyPad-33] = atoi(m_acOutput);
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 45:
					if(ucBlank) AmuletSetupLinearityCalicheckMenu_testTotal = -1;
					else AmuletSetupLinearityCalicheckMenu_testTotal = atoi(m_acOutput);
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 46:
					if(ucBlank) AmuletDoseTableMenu_volume = 0;
					else AmuletDoseTableMenu_volume = atof(m_acOutput);
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 47:
					if(ucBlank) AmuletDoseTableMenu_interval = 0;
					else AmuletDoseTableMenu_interval = atoi(m_acOutput);
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 48:
					if(CheckPassword(m_acOutput)){
						PopPageStack();
						m_ucClear = 6;
						SetAmuletHTML(AmuletHTMLIndex[SETUPSOURCES_HTM]);
						PushPageStack(AmuletHTMLIndex[SETUPSOURCES_HTM]);
					}else{
						SetAmuletBackHTML();
					}
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 49:
					if(CheckPassword(m_acOutput)){
						PopPageStack();
						SetAmuletHTML(AmuletHTMLIndex[SETUPLINEARITY_HTM]);
						PushPageStack(AmuletHTMLIndex[SETUPLINEARITY_HTM]);
					}else{
						SetAmuletBackHTML();
					}
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 50:
					if(CheckPassword(m_acOutput)){
						PopPageStack();
						m_ucClear = 7;
						SetAmuletHTML(AmuletHTMLIndex[SETUPMOLY_HTM]);
						PushPageStack(AmuletHTMLIndex[SETUPMOLY_HTM]);
					}else{
						SetAmuletBackHTML();
					}
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 51:
					if(!ucBlank){
						Mca_setGain(atoi(m_acOutput), Mca_getGain2());
						Mca_getCurrentAutoCal()->gain1 = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 52:
					if(!ucBlank){
						Mca_setGain(Mca_getGain1(), atoi(m_acOutput));
						Mca_getCurrentAutoCal()->gain2 = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 53:
					if(!ucBlank){
						//Mca_setZeroOpAmpOffset(-1 * atoi(m_acOutput));
						Mca_setZeroOpAmpOffset(atoi(m_acOutput));
						Mca_getCurrentAutoCal()->zeroopampoffset = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 54:
					if(CheckPassword(m_acOutput)){
						PopPageStack();
						SetAmuletHTML(AmuletHTMLIndex[WELLADVANCEDSETUP_HTM]);
						PushPageStack(AmuletHTMLIndex[WELLADVANCEDSETUP_HTM]);
					}else{
						SetAmuletBackHTML();
					}
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 55:
					if(!ucBlank){
						MCASetThreshold(atoi(m_acOutput));
						Mca_getCurrentAutoCal()->threshold = atoi(m_acOutput);
						Mca_getDetectorMirrorPtr()->threshold = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;


				case 56:
					if(!ucBlank){
						cCountTimeIndex = -1;
						switch(m_ucSetWellMeasure){
							case 1:
								cCountTimeIndex = WELL_TIME_GENERAL;
								break;

							case 2:
								if(!AmuletWellMeasurementMenu_WipeBackgroundThreshold) cCountTimeIndex = WELL_TIME_BACKGROUND;
								break;

							case 3:
								cCountTimeIndex = WELL_TIME_EFFICIENCY;
								break;

							case 7:
							case 8:
								cCountTimeIndex = WELL_TIME_SCHILLING;
								break;

							case 9:
							case 10:
							case 11:
								cCountTimeIndex = WELL_TIME_PLASMA;
								break;

							case 12:
							case 13:
							case 14:
							case 15:
								cCountTimeIndex = WELL_TIME_RBC;
								break;

							case 16:
								cCountTimeIndex = WELL_TIME_SYSTEM;
								break;

							case 17:
								cCountTimeIndex = WELL_TIME_MDA;

							case 20:
							case 21:
							case 22:
								AmuletWellSetupBioAssayMeasureEfficiencyMenu_countTime = atoi(m_acOutput);
								break;
						}
						if(cCountTimeIndex >= 0){
							AmuletWellMeasureMenu_countTime[cCountTimeIndex] = atoi(m_acOutput);
							DB_updateWellTime(AmuletWellMeasureMenu_countTime[cCountTimeIndex], cCountTimeIndex);
						}

						Mca_setAcquireTime(atoi(m_acOutput));
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 57:
					if(!ucBlank){
						hv = atof(m_acOutput);
						if((Mca_installedDetector == DET_WELL) || (Mca_installedDetector == DET_BETA)){
							factor = 1246 - 595;
							factor /= 4095.0;
							hv -= 595.0;
							fcode = hv/factor;
						}else{
							factor = hv;
							factor /= .263158;
							fcode = factor;
						}
						MCASetHV(fcode, Mca_getHVState());
						Mca_getCurrentAutoCal()->hv = fcode;
						Mca_getDetectorMirrorPtr()->hv = fcode;
						//MCASetHV(atoi(m_acOutput), Mca_getHVState());
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 58:
					if(!ucBlank){
						AmuletWellEditEfficiencies_temp.userEnergy1 = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 59:
					if(!ucBlank){
						AmuletWellEditEfficiencies_temp.userEnergy2 = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 60:
					if(!ucBlank){
						AmuletWellEditEfficiencies_temp.userEnergy3 = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 61:
					if(!ucBlank){
						if(Mca_installedDetector == DET_WELL){
							AmuletWellEditEfficiencies_temp.userWellEff = atof(m_acOutput);
							AmuletWellEditEfficiencies_temp.wellStamp = (time_t) 0;
						}else{
							AmuletWellEditEfficiencies_temp.userWell700Eff = atof(m_acOutput);
							AmuletWellEditEfficiencies_temp.well700Stamp = (time_t) 0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 62:
					if(!ucBlank){
						AmuletWellEditEfficiencies_temp.userProbe700Eff = atof(m_acOutput);
						AmuletWellEditEfficiencies_temp.probe700Stamp = (time_t) 0;
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 63:
					if(!ucBlank){
						tempfloat = atof(m_acOutput);
						if(tempfloat >= .1){
							moly_volume = tempfloat;
							beep_amulet();
							SetAmuletBackHTML();
							m_uiSetKeyPad = 0;
						}else{
							m_iPhase = PHASE_KEYPAD_WAIT;
						}
					}else{
						m_iPhase = PHASE_KEYPAD_WAIT;
					}
					break;

				case 64:
					if(strcmp(m_acOutput, "513")==0){
						PopPageStack();
						m_ucClear = 36;
						SetAmuletHTML(AmuletHTMLIndex[FACTORY_HTM]);
						PushPageStack(AmuletHTMLIndex[FACTORY_HTM]);
					}else{
						SetAmuletBackHTML();
					}
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 65:
					beep_amulet();
					if(!ucBlank){
						if(strlen(m_acOutput) != 10){
							get_amulet_message(L_SERIAL_NUMBER_ERROR, titlestring);    // "Serial Number Error"
							Amulet_DisplayError(titlestring, "Serial number must have ten digits",TRUE);
							return;
						}else{
							strcpy(AmuletSetupCalibSerialMenu_serial, m_acOutput);
						}
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 66:
					if(!ucBlank){
						strcpy(AmuletSetupChamber_serial, m_acOutput);
					}
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 67:
					if(!ucBlank){
						AmuletSetupChamber_respCorr = atof(m_acOutput);
						AmuletSetupChamber_respCorr /= 100.0;
					}
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 68:
					if(!ucBlank){
						AmuletSetupChamber_nomVolts = atof(m_acOutput);
					}
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 69:
				case 70:
				case 71:
				case 100:
					if(!ucBlank){
						AmuletWellSetupTypes_wellWipeType.Threshold = atof(m_acOutput);
						if(current.system == BQ){
							AmuletWellSetupTypes_wellWipeType.Threshold = Mca_convertBqToDpm(AmuletWellSetupTypes_wellWipeType.Threshold);
						}
					}
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 72:
					if(!ucBlank){
						AmuletWellSetupTypes_wellWipeType.Threshold = atof(m_acOutput);
						if(current.system == CI){
							AmuletWellSetupTypes_wellWipeType.Threshold /= 1e+9;
							AmuletWellSetupTypes_wellWipeType.Threshold = Mca_convertCiToDpm(AmuletWellSetupTypes_wellWipeType.Threshold);
						}else if(current.system == BQ){
							AmuletWellSetupTypes_wellWipeType.Threshold = Mca_convertBqToDpm(AmuletWellSetupTypes_wellWipeType.Threshold);
						}
					}
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 73:
					if(!ucBlank){
						AmuletWellEditFullEfficiency_fullEfficiency = atof(m_acOutput);
					}
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 74:
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 75:
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 76:
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 77:
					beep_amulet();
					if(!ucBlank){
						if(strlen(m_acOutput) != 6){
							get_amulet_message(L_SERIAL_NUMBER_ERROR, titlestring);    // "Serial Number Error"
							get_amulet_message(L_SERIAL_NUMBER_MUST_HAVE_SIX_DIGITS, formatstring);    // "Serial number must have six digits"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}else{
							strcpy(AmuletWellFactoryMenu_serialNum, m_acOutput);
						}
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 78:
					beep_amulet();
					if(!ucBlank){
						hv = atof(m_acOutput);
						factor = 1246 - 595;
						factor /= 4095.0;
						hv -= 595.0;
						fcode = hv/factor;
						AmuletWellFactoryMenu_highVoltage = fcode;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 79:
					beep_amulet();
					if(!ucBlank){
						AmuletWellFactoryMenu_threshold = atoi(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 80:
					beep_amulet();
					if(!ucBlank){
						AmuletWellSchillingMenu_test.UrineVolume = atof(m_acOutput);
					}else{
						AmuletWellSchillingMenu_test.UrineVolume = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 81:
					beep_amulet();
					if(!ucBlank){
						AmuletWellSchillingMenu_test.AliquotVolume = atof(m_acOutput);
					}else{
						AmuletWellSchillingMenu_test.AliquotVolume = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 82:
					beep_amulet();
					if(!ucBlank){
						AmuletWellSchillingMenu_test.DilutionFactor = atof(m_acOutput);
					}else{
						AmuletWellSchillingMenu_test.DilutionFactor = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 83:
					beep_amulet();
					if(!ucBlank){
						AmuletWellPlasmaMenu_test.DilutionFactor = atof(m_acOutput);
					}else{
						AmuletWellPlasmaMenu_test.DilutionFactor = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 84:
					beep_amulet();
					if(!ucBlank){
						AmuletWellPlasmaMenu_test.SampleVolume = atof(m_acOutput);
					}else{
						AmuletWellPlasmaMenu_test.SampleVolume = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 85:
					beep_amulet();
					if(!ucBlank){
						AmuletWellPlasmaMenu_test.Hematocrit = atof(m_acOutput);
					}else{
						AmuletWellPlasmaMenu_test.Hematocrit = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 86:
					beep_amulet();
					if(!ucBlank){
						AmuletWellPlasmaMenu_test.PatientWeight = atof(m_acOutput);
					}else{
						AmuletWellPlasmaMenu_test.PatientWeight = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 87:
					beep_amulet();
					if(!ucBlank){
						AmuletWellRBCMenu_test.DoseHematocrit = atof(m_acOutput);
					}else{
						AmuletWellRBCMenu_test.DoseHematocrit = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 88:
					beep_amulet();
					if(!ucBlank){
						AmuletWellRBCMenu_test.PatientHematocrit = atof(m_acOutput);
					}else{
						AmuletWellRBCMenu_test.PatientHematocrit = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 89:
					beep_amulet();
					if(!ucBlank){
						AmuletWellRBCMenu_test.PatientWeight = atof(m_acOutput);
					}else{
						AmuletWellRBCMenu_test.PatientWeight = -1;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 90:
					beep_amulet();
					if(!ucBlank){
						AmuletWellMDATestMenu_mda.PrecisionFactor = atof(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 91:
					beep_amulet();
					if(!ucBlank){
						AmuletWellMDATestMenu_mda.CorrectionFactor = atof(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 92:
					beep_amulet();
					if(!ucBlank){
						AmuletWellChiTestMenu_test.SampleTime = atoi(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 93:
					beep_amulet();
					if(!ucBlank){
						AmuletWellChiTestMenu_test.NumberOfSamples = atoi(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 94:
					beep_amulet();
					generalNuclideID = -2;
					if(ucBlank){
						startEV[0] = -1.0;
					}else{
						startEV[0] = atof(m_acOutput);
						if(endEV[0] >= 0.0){
							if(startEV[0] > endEV[0]){
								tempfloat = endEV[0];
								endEV[0] = startEV[0];
								startEV[0] = tempfloat;
							}
						}
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 95:
					beep_amulet();
					generalNuclideID = -2;
					if(ucBlank){
						endEV[0] = -1.0;
					}else{
						endEV[0] = atof(m_acOutput);
						if(startEV[0] >= 0.0){
							if(endEV[0] < startEV[0]){
								tempfloat = startEV[0];
								startEV[0] = endEV[0];
								endEV[0] = tempfloat;
							}
						}
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 96:
					if(CheckPassword(m_acOutput)){
						PopPageStack();
						m_ucClear = 0;
						SetAmuletHTML(AmuletHTMLIndex[WELLSETUPWIPES_HTM]);
						PushPageStack(AmuletHTMLIndex[WELLSETUPWIPES_HTM]);
					}else{
						SetAmuletBackHTML();
					}
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 97:
					if(CheckPassword(m_acOutput)){
						PopPageStack();
						m_ucClear = 22;
						SetAmuletHTML(AmuletHTMLIndex[WELLSETUPTESTSOURCE_HTM]);
						PushPageStack(AmuletHTMLIndex[WELLSETUPTESTSOURCE_HTM]);
					}else{
						SetAmuletBackHTML();
					}
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 98:
					beep_amulet();
					if(!ucBlank){
						AmuletWellStabilityTestMenu_countTime = atoi(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 99:
					beep_amulet();
					if(!ucBlank){
						AmuletWellStabilityTestMenu_reps = atol(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 101:
					beep_amulet();
					if(!ucBlank){
						AmuletWellSetupTypes_wellWipeType.CountTime = atol(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 102:
				case 103:
				case 105:
					if(!ucBlank){
						AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold = atof(m_acOutput);
						if(current.system == BQ){
							AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold = Mca_convertBqToDpm(AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold);
						}
					}
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 104:
					if(!ucBlank){
						AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold = atof(m_acOutput);
						if(current.system == CI){
							AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold /= 1e+9;
							AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold = Mca_convertCiToDpm(AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold);
						}else if(current.system == BQ){
							AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold = Mca_convertBqToDpm(AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold);
						}
					}
					SetAmuletBackHTML();
					beep_amulet();
					m_uiSetKeyPad = 0;
					break;

				case 106:
					beep_amulet();
					if(!ucBlank){
						AmuletWellSetupAddEditLocation_wellWipeLocation.CountTime = atol(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 107:
					beep_amulet();
					if(!ucBlank){
						if(strlen(m_acOutput) != 6){
							get_amulet_message(L_SERIAL_NUMBER_ERROR, titlestring);    // "Serial Number Error"
							get_amulet_message(L_SERIAL_NUMBER_MUST_HAVE_SIX_DIGITS, formatstring);    // "Serial number must have six digits"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}else{
							strcpy(AmuletWellFactoryDetectorsMenu_probeSerialNum, m_acOutput);
						}
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 108:
					beep_amulet();
					if(!ucBlank){
						if(strlen(m_acOutput) != 6){
							get_amulet_message(L_SERIAL_NUMBER_ERROR, titlestring);    // "Serial Number Error"
							get_amulet_message(L_SERIAL_NUMBER_MUST_HAVE_SIX_DIGITS, formatstring);    // "Serial number must have six digits"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}else{
							strcpy(AmuletWellFactoryDetectorsMenu_wellSerialNum, m_acOutput);
						}
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 109:
					beep_amulet();
					if(!ucBlank){
						hv = atof(m_acOutput);
						factor = hv;
						factor /= .263158;
						fcode = factor;
						AmuletWellFactoryDetectorsMenu_probeHighVoltage = fcode;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 110:
					beep_amulet();
					if(!ucBlank){
						hv = atof(m_acOutput);
						factor = hv;
						factor /= .263158;
						fcode = factor;
						AmuletWellFactoryDetectorsMenu_wellHighVoltage = fcode;
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 111:
					beep_amulet();
					if(!ucBlank){
						AmuletWellFactoryDetectorsMenu_probeThreshold = atoi(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 112:
					beep_amulet();
					if(!ucBlank){
						AmuletWellFactoryDetectorsMenu_wellThreshold = atoi(m_acOutput);
					}
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 113:
					if(!ucBlank){
						AmuletWellAddEditUserNuclides_temp.energy1 = atof(m_acOutput);
					}else{
						AmuletWellAddEditUserNuclides_temp.energy1 = 0.0;
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 114:
					if(!ucBlank){
						AmuletWellAddEditUserNuclides_temp.energy2 = atof(m_acOutput);
					}else{
						AmuletWellAddEditUserNuclides_temp.energy2 = 0.0;
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 115:
					if(!ucBlank){
						AmuletWellAddEditUserNuclides_temp.energy3 = atof(m_acOutput);
					}else{
						AmuletWellAddEditUserNuclides_temp.energy3 = 0.0;
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 116:
					if(Mca_installedDetector == DET_WELL){
						if(!ucBlank){
							AmuletWellAddEditUserNuclides_temp.wellefficiency = atof(m_acOutput);
							AmuletWellAddEditUserNuclides_temp.wellmeasuredon = (time_t) 0;
						}else{
							AmuletWellAddEditUserNuclides_temp.wellefficiency = 0.0;
							AmuletWellAddEditUserNuclides_temp.wellmeasuredon = (time_t) 0;
						}
					}else{
						if(!ucBlank){
							AmuletWellAddEditUserNuclides_temp.well700efficiency = atof(m_acOutput);
							AmuletWellAddEditUserNuclides_temp.well700measuredon = (time_t) 0;
						}else{
							AmuletWellAddEditUserNuclides_temp.well700efficiency = 0.0;
							AmuletWellAddEditUserNuclides_temp.well700measuredon = (time_t) 0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 117:
					if(!ucBlank){
						AmuletWellAddEditUserNuclides_temp.probe700efficiency = atof(m_acOutput);
						AmuletWellAddEditUserNuclides_temp.probe700measuredon = (time_t) 0;
					}else{
						AmuletWellAddEditUserNuclides_temp.probe700efficiency = 0.0;
						AmuletWellAddEditUserNuclides_temp.probe700measuredon = (time_t) 0;
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 118:
					if(!ucBlank){
						AmuletWellSetupBioAssayMenu_bioAssaySetting.CountTime = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 119:
					if(!ucBlank){
						AmuletWellSetupBioAssayMenu_bioAssaySetting.ProbeDistance = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 120:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I131Efficiency = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 121:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I125Efficiency = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 122:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I123Efficiency = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 123:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I131I125Contamination = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 124:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I123I125Contamination = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 125:
					if(!ucBlank){
						AmuletWellBioAssay_probeDistance = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 126:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterROI_I131Lower_keV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 127:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterROI_I131Upper_keV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 128:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterROI_I125Lower_keV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 129:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterROI_I125Upper_keV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 130:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterROI_I123Lower_keV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 131:
					if(!ucBlank){
						AmuletWellSetupBioAssayEnterROI_I123Upper_keV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 132:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeProtocol_protocol.DefaultCountTime = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 133:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeProtocol_protocol.DefaultDistance = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 134:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeProtocol_protocol.I123LowerEV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 135:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeProtocol_protocol.I123UpperEV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 136:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeProtocol_protocol.I131LowerEV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 137:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeProtocol_protocol.I131UpperEV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 138:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mLowerEV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 139:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeProtocol_protocol.Tc99mUpperEV = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 140:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeTest_test.CountingTime = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 141:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeTest_test.ProbeDistance = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 142:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeTest_test.DoseMultiplier = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 143:
					if(!ucBlank){
						AmuletAddEditThyroidUptakeTest_test.DoseMultiplier = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 144:
				case 145:
				case 146:
				case 147:
					if(!ucBlank){
						AmuletWellThyroidUptakeEnterNormal_value[m_uiSetKeyPad - 144].hour = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 148:
				case 149:
				case 150:
				case 151:
					if(!ucBlank){
						AmuletWellThyroidUptakeEnterNormal_value[m_uiSetKeyPad - 148].min = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 152:
				case 153:
				case 154:
				case 155:
					if(!ucBlank){
						AmuletWellThyroidUptakeEnterNormal_value[m_uiSetKeyPad - 152].max = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 159:
					if(!ucBlank){
						AmuletAddEditRBCSurvivalTest_test.CountingTime = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 160:
					if(!ucBlank){
						AmuletWellRBCSurvivalMeasureMenu_measurement.SampleHematocrit = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 161:
					if(!ucBlank){
						AmuletWellRBCSurvivalEnterNormal_min = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 162:
					if(!ucBlank){
						AmuletWellRBCSurvivalEnterNormal_max = atof(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 201:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasurePatient.Leg1ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM = AmuletWellThyroidUptakeMeasurePatient.Leg1ROICounts;
						if(AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM == -1) AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM;
						else{
							AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM;
							AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM += AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM;
							AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 202:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasurePatient.Leg2ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM = AmuletWellThyroidUptakeMeasurePatient.Leg2ROICounts;
						if(AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM == -1) AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM;
						else{
							AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM;
							AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM += AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM;
							AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 203:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts;
						if(AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM == -1) AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM;
						else{
							AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM;
							AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM += AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM;
							AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 204:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM = AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts;
						if(AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM == -1) AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM;
						else{
							AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM;
							AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM += AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM;
							AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 205:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 206:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 207:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 208:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 209:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 210:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 211:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 212:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 213:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 214:
					if(!ucBlank){
						spec_meas.averagecpm = 0;
						spec_meas.dead_time = 0;
						spec_meas.live_time = 0;
						spec_meas.num_of_channels = 512;
						for(index=0; index<4096; index++) spec_meas.spectrum[index] = 0;
						spec_meas.real_time  = 0;
						spec_meas.stamp = clock_time;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2_Spectrum), &spec_meas);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICounts = atoi(m_acOutput);
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICounts;
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM;
						else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM /= 2.0;
						}
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 215:
					if(!ucBlank){
						AmuletAutoLinearityTest_intervalMinutes = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 216:
					if(!ucBlank){
						AmuletAutoLinearityTest_totalMinutes = atoi(m_acOutput) * 60;
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 217:
					if(!ucBlank){
						stringLength = strlen(m_acOutput);

						if(stringLength){
							i = stringLength - 1;
							while(i>=0){
								if(m_acOutput[i] == '/'){
									m_acOutput[i] = 0;
									i--;
								}else{
									break;
								}
							}
						}

						stringLength = strlen(m_acOutput);

						slashCount = 0;
						for(i=0; i<stringLength; i++){
							if(m_acOutput[i] == '/') slashCount++;
						}

						if(slashCount > 3){
							get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
							get_amulet_message(L_INVALID_DATE, formatstring);    // "Invalid Date"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}

						if((stringLength < 3) && (slashCount == 0)){
							get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
							get_amulet_message(L_INVALID_TIME, formatstring);    // "Invalid Time"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}

						timestr[0] = 0;
						secstr[0] = 0;
						minstr[0] = 0;
						hourstr[0] = 0;
						daystr[0] = 0;
						monstr[0] = 0;
						yearstr[0] = 0;

						if((slashCount == 0) && (stringLength > 6)){
							get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
							get_amulet_message(L_INVALID_TIME, formatstring);    // "Invalid Time"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}

						beginIndex = 0;
						slashCount = 0;
						strcat(m_acOutput, "/");
						stringLength = strlen(m_acOutput);
						for(i=0; i<=stringLength; i++){
							if((m_acOutput[i] == '/')){
								m_acOutput[i] = 0;
								if(slashCount == 0) strcpy(timestr, &m_acOutput[beginIndex]);
								else if(slashCount == 1) strcpy(daystr, &m_acOutput[beginIndex]);
								else if(slashCount == 2) strcpy(monstr, &m_acOutput[beginIndex]);
								else if(slashCount == 3) strcpy(yearstr, &m_acOutput[beginIndex]);
								else break;
								beginIndex = i + 1;
								slashCount++;
							}
						}

						gmtime_r(&clock_time, &datetime);

						if(strlen(timestr) == 0){
							sprintf(hourstr, "%d", datetime.tm_hour);
							sprintf(minstr, "%d", datetime.tm_min);
							sprintf(secstr, "%d", datetime.tm_sec);
						}else if(strlen(timestr) == 3){
							hourstr[0] = timestr[0];
							hourstr[1] = 0;
							minstr[0] = timestr[1];
							minstr[1] = timestr[2];
							minstr[2] = 0;
							secstr[0] = '0';
							secstr[1] = 0;
						}else if(strlen(timestr) == 4){
							hourstr[0] = timestr[0];
							hourstr[1] = timestr[1];
							hourstr[2] = 0;
							minstr[0] = timestr[2];
							minstr[1] = timestr[3];
							minstr[2] = 0;
							secstr[0] = '0';
							secstr[1] = 0;
						}else if(strlen(timestr) == 5){
							hourstr[0] = timestr[0];
							hourstr[1] = 0;
							minstr[0] = timestr[1];
							minstr[1] = timestr[2];
							minstr[2] = 0;
							secstr[0] = timestr[3];
							secstr[1] = timestr[4];
							secstr[2] = 0;
						}else if(strlen(timestr) == 6){
							hourstr[0] = timestr[0];
							hourstr[1] = timestr[1];
							hourstr[2] = 0;
							minstr[0] = timestr[2];
							minstr[1] = timestr[3];
							minstr[2] = 0;
							secstr[0] = timestr[4];
							secstr[1] = timestr[5];
							secstr[2] = 0;
						}else{
							get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
							get_amulet_message(L_INVALID_TIME, formatstring);    // "Invalid Time"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}

						if(atoi(hourstr) > 23){
							get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
							get_amulet_message(L_MAX_HOUR_IS_23, formatstring);    // "Max Hour is 23"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}

						if(atoi(minstr) > 59){
							get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
							get_amulet_message(L_MAX_MINUTE_IS_59, formatstring);    // "Max Minute is 59"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}

						if(atoi(secstr) > 59){
							get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
							get_amulet_message(L_MAX_SECOND_IS_59, formatstring);    // "Max Second is 59"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}

						if(strlen(daystr)){
							if(atoi(daystr) == 0){
								get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
								get_amulet_message(L_MIN_DAY_IS_1, formatstring);    // "Min Day is 1"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
								return;
							}else if(atoi(daystr) > 31){
								get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
								get_amulet_message(L_MAX_DAY_IS_31, formatstring);    // "Max Day is 31"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
								return;
							}
						}else{
							sprintf(daystr, "%d", datetime.tm_mday);
						}

						if(strlen(monstr)){
							if(atoi(monstr) == 0){
								get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
								get_amulet_message(L_MIN_MONTH_IS_1, formatstring);    // "Min Month is 1"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
								return;
							}else if(atoi(monstr) > 12){
								get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
								get_amulet_message(L_MAX_MONTH_IS_12, formatstring);    // "Max Month is 12"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
								return;
							}
						}else{
							sprintf(monstr, "%d", datetime.tm_mon + 1);
						}

						if(strlen(yearstr)){
							if(atoi(yearstr) > 30){
								get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
								get_amulet_message(L_MAX_YEAR_IS_2030, formatstring);    // "Max Year is 2030"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
								return;
							}else if(strlen(yearstr) == 1){
								get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
								get_amulet_message(L_INVALID_YEAR, formatstring);    // "Invalid Year"
								Amulet_DisplayError(titlestring, formatstring, TRUE);
								return;
							}
							yearstr[2] = yearstr[0];
							yearstr[3] = yearstr[1];
							yearstr[0] = '2';
							yearstr[1] = '0';
							yearstr[4] = 0;
						}else{
							sprintf(yearstr, "%d", datetime.tm_year + 1900);
						}

						if(!test_date(atoi(monstr), atoi(daystr), atoi(yearstr))){
							get_amulet_message(L_DOSE_TIME_ERROR, titlestring);    // "Dose Time Error"
							get_amulet_message(L_INVALID_DATE, formatstring);    // "Invalid Date"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}

						datetime.tm_hour = atoi(hourstr);
						datetime.tm_isdst = 0;
						datetime.tm_mday = atoi(daystr);
						datetime.tm_min = atoi(minstr);
						datetime.tm_mon = atoi(monstr) - 1;
						datetime.tm_sec = atoi(secstr);
						datetime.tm_wday = 0;
						datetime.tm_yday = 0;
						datetime.tm_year = atoi(yearstr) - 1900;
						measurement[current.main_chamber].future.dosetime = mk_time(&datetime);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 218:
					if(!ucBlank){
						AmuletHalflifeCalc_intervalSeconds = atoi(m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 219:
					if(!ucBlank){
						AmuletHalflifeCalc_totalSeconds = atoi(m_acOutput);
						AmuletHalflifeCalc_totalSeconds *= 60;
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 220:
					if(!ucBlank){
						strcpy(AmuletSetupPassword_currentPassword, m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 221:
					if(!ucBlank){
						strcpy(AmuletSetupPassword_newPassword, m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				case 222:
					if(!ucBlank){
						strcpy(AmuletSetupPassword_confirmPassword, m_acOutput);
					}
					beep_amulet();
					SetAmuletBackHTML();
					m_uiSetKeyPad = 0;
					break;

				default:
					m_iPhase = PHASE_KEYPAD_WAIT;
					break;
			}
			break;

		case PHASE_KEYPAD_DISPLAY:
			if(m_ucKeyPadConfig == 4){
				wildcard(m_acOutput, wildcard_string);
				send_to_amulet_string(102, wildcard_string);
			}else{
				send_to_amulet_string(102, m_acOutput);
			}
			SetAmuletByte(106, 0xFF);
			m_iPhase = PHASE_KEYPAD_WAIT;
			break;

		case PHASE_KEYPAD_PERIOD:
			beep_amulet();
			if(m_cPeriodPos == -1) {
				//if(numcount(m_acOutput) < m_ucLenLimit) {
					m_cPeriodPos = strlen(m_acOutput);
					strcat(m_acOutput, ".");
				//}
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;

		case PHASE_KEYPAD_PLUSMINUS:
			beep_amulet();
			if(m_acOutput[0] == '-'){
				strcpy(tempOutput, &(m_acOutput[1]));
				strcpy(m_acOutput, tempOutput);
			}else{
				strcpy(tempOutput, "-");
				strcat(tempOutput, m_acOutput);
				strcpy(m_acOutput, tempOutput);
			}
			m_iPhase = PHASE_KEYPAD_DISPLAY;
			break;
	}
}
