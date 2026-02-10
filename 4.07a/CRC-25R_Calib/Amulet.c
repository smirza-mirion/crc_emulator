/**
 * \file
 * \details This file contains functions, which talk to the Amulet screen.
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "crc.h"
#include "i2c.h"
#include "screen.h"
#include "keyboard.h"
#include "daily.h"
#include "pit.h"
#include "qspi.h"
#include "amulet.h"
#include "coldfire.h"
#include "nuc.h"
#include "chambfac.h"
#include "mca.h"
#include "err.h"
#include "printer.h"
#include "counter.h"
#include "ff.h"
#include "wipes.h"
#include "database.h"
#include "message.h"

#define FILE_BUFFER_SIZE 1000
#define XMODEM_DATA_SIZE 128
#define SOH 0x01
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define ETB 0x17
#define UART_BUFFER_SIZE 1024

extern volatile bool DisplayActivity_SendActivity;
extern volatile bool DisplayActivity_SendActivity2;
extern volatile short DisplayActivity_addedDigits;
extern time_t clock_time;
extern CURRENT  current;
extern CHAMBER chamber[];
extern MEASUREMENT measurement[];
extern ACCDATA acc_data[];
extern volatile unsigned char g_ucRelease;
extern bool meas_screen;
extern const char *unit_str[];
/*typedef struct linmeas LINMEAS;
struct linmeas{
    float meas_act;
    float factor;
    float pc_ratio;
    char act_str[10];
    time_t meas_time;
};
typedef struct calmeas CALMEAS;
struct calmeas{
    float meas_act;
    float result;
    float var;
    char res_str[10];
    time_t meas_time;
};
typedef struct geomeas GEOMEAS;
struct geomeas{
	float meas_act;
	float totvol;
	char act_str[10];
	float var;
};
//extern GEOMEAS geomeas[10];*/
char Geometry_acContainer[8];
int Geometry_nmeas;
extern INVENTORY inventory[MAX_INVENTORY];
#define ITEMS_PER_PAGE 10

extern volatile unsigned char g_flgAmuletMode;
extern unsigned char m_ucHotKeyNuclideID[UPPER_LIMIT_CHAMB][8];
extern char AmuletErrorMsgMenu_title[52];
extern char AmuletErrorMsgMenu_message[100];
extern char AmuletErrorMsgLargeMenu_title[52];
extern char AmuletErrorMsgLargeMenu_message[4096];
extern char AmuletNotificationMsgMenu_title[52];
extern char AmuletNotificationMsgMenu_message[100];
extern bool AmuletNotificationMsgMenu_showOK;
extern bool AmuletErrorMsgMenu_showOK;
extern bool AmuletWarningMsgMenu_showOK;
extern char AmuletWarningMsgMenu_title[52];
extern char AmuletWarningMsgMenu_message[100];
extern uchar EepromRoutines_brightnessCurrent;
extern uchar EepromRoutines_sleepTimeoutCurrent;
extern uchar EepromRoutines_sleepBrightnessCurrent;
extern WELLMDATEST AmuletWellMDATestMenu_mda;
extern WELLCHITEST AmuletWellChiTestMenu_test;
extern short max_chambers;
extern REMOTE remote[];
extern short AmuletSetupLinearityStandardMenu_chamberType;
extern bool AmuletWellManual_showClearEu152LinCorr;
extern WELLWIPELOCATION AmuletWellSetupAddEditLocation_wellWipeLocation;
extern bool AmuletWellMeasurementMenu_WipeBackgroundThreshold;
extern uchar AmuletGenericItems_config;
extern uchar AmuletInactivate_config;
extern uchar AmuletTestIdentMenu_config;
extern bool AmuletInfoMenu_DoseTable;
extern bool AmuletWellMeasurementMenu_unlockDetector;
extern bool AmuletWellAddEditUserNuclides_addNew;
extern short AmuletWellMeasureEfficiency_mode;
extern bool AmuletLoginMenu_blockGuest;
extern int AmuletLoginMenu_minRole;
extern bool AmuletSecurity_securityMode;
extern bool AmuletSecurity_activeOnlyStaff;
extern bool AmuletWellSetupThyroidUptakeProtocol_setupMode;
extern bool AmuletWellThyroidUptake_runMode;
extern int AmuletWellMeasurementMenu_generalDetectorTest;
extern bool AmuletWellRBCSurvival_runMode;
extern bool AmuletWellThyroidUptakeEnterNormal_enter;
extern int AmuletAutoLinearityTest_mode;
extern uchar AmuletGenericYesNo_config;
extern bool AmuletWellRBCSurvivalEnterNormal_enter;
extern long long int AmuletGenericYesNo_LatchedDailyTestID;
extern long long int AmuletDailyMenu_ChamberDailyTestID;
extern int AmuletChamberSearch_mode;
extern int AmuletHalflifeCalc_printSummary;

int MCASetHV(short value, bool state);
void CallHTML(unsigned char ucValue);
void trim(char *acByte);
void trim_and_shrink(char *acByte);
void acc_meas_string(char *output, short iCurrentSource, short ch_num, short iOffset, bool *error);
unsigned char IsNumber(char *ptr);
void LinearElapsedTime(short index, LINDEF *def, time_t start_time, char *elapsed, char *time_due);
float get_current_act(INVENTORY *inv, NUCDATA *nuc, time_t *nowtime);
void SetAmuletBackHTML(void);
void SetAmuletHomeHTML(void);
void PushPageStack(unsigned char ucPage);
unsigned char PopPageStack(void);
void ClearPageStack(void);
unsigned char CurrentPageStack(void);
void GetExtendedNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void GetExtendedNuclideString(unsigned char ucNuclideID, char *acMsg);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void GetExtendedTimeInfoLanguageSec(time_t *dtmDateTime, char *acMsg);
unsigned char CheckPassword(char *Passwd);
void BrightnessMirrorUpdate(void);
void SleepTimeoutUpdate(void);
void SleepBrightnessUpdate(void);
char SendUart2Tx(char *cOutput);
void EnableTx2(void);
char PushUart2Tx(char cInput);
char PopUart2Rx(char *cOutput);
void ClearUart2Rx(void);
void ClearUart2Tx(void);
void SendWakeUp(void);
short unsigned int QueryPendingUart2Rx(void);
short unsigned int QueryPendingUart2Tx(void);
void set_touch_program(bool on);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
bool EepromRoutines_sleepNow(void);
void SleepRefreshTimeout(void);
void Amulet_DisplayWarning(char *title, char *notificationstring, bool ShowOK);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);
void GetShortNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void AmuletError_setNewPage(uint value);
void AmuletWarning_setNewPage(uint value);
short format_activity_kunit_kbq(float act, short syst, char *actstr);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void get_study_type(short istudy, char *study);
void setup_short_watchdog(void);

unsigned int m_uiYear;
unsigned char m_ucMonth;
unsigned char m_ucDay;
unsigned char m_ucHour;
unsigned char m_ucMinute;
unsigned char m_ucSecond;
ushort amuletBuild = 0;
ushort amuletIndexPageNum = 0xFFFF;
char Amulet_amuletImage[20];
char Amulet_amuletImage2[20];
char m_cKeypress[2];
char m_cKeystring[100];
unsigned char m_ucBargraphValue = 0;
unsigned char m_ucSetNuclide = 0;
unsigned char m_ucCheckboxState = 0;

unsigned long int m_ulLastMainScreenStamp = 0;

volatile int m_iMenu = MENU_NONE;
volatile int m_iPhase = PHASE_GENERIC_NOP;
volatile int m_iCurPhase = PHASE_GENERIC_CUR_NOP;
volatile int m_iNextPhase = PHASE_GENERIC_CUR_NOP;

unsigned char m_ucClear=0;
static ushort m_usBackClear=0;

// Inventory Vars
short int m_iSelectedMapItem;
short int m_iMap[MAX_INVENTORY];

// Add Inventory Module Vars
unsigned char m_ucAddInventory_Nuclide;
unsigned char m_ucAddInventory_ID;
char m_acAddInventory_Lot[17];
float m_fAddInventory_Volume;
float m_fAddInventory_Activity;
time_t m_dtmAddInventory_Date;
unsigned char m_ucAddInventory_MeasuredNuclide;
short m_iAddInventory_Study;
float m_fAddInventory_MoPerTc;

// KeyPad
char m_cDivPos;
char m_cMultPos;
char m_cPeriodPos;
unsigned short int m_uiSetKeyPad = 0;
unsigned char m_ucKeyPadConfig = 0;
unsigned char m_ucLenLimit = 0;
unsigned char m_ucLenFractionLimit = 0;
float m_fMaxValue = 0.0;
float m_fMinValue = 0.0;
volatile char m_acTitle[52];
char m_acOutput[100];
bool m_flgShowPlusMinus;

// KeyBoard
unsigned char m_ucSetKeyBoard = 0;
unsigned char keyboardCase = 0;
bool keyboardWildcard = FALSE;

// Activity
unsigned char m_ucSetActivity = 0;
unsigned char m_ucUnit; // 0=uCi, 1=mCi, 2=Ci, 3=KBq, 4=MBq, 5=GBq
unsigned char m_ucActivityConfig = 0; // 1=CI Only, 2=BQ Only, 3=CI & BQ

// Time
unsigned char m_ucSetTime = 0;
unsigned char m_ucTimeMode = 0; // 0 = Date And Time, 1 = Date Only
unsigned char m_ucSecMode = 0;	// 0 = No Seconds, 1 = Seconds

// Measure
unsigned char m_ucSetMeasure = 0;

// Nuclide
uchar m_ucNuclideConfig = 0;
uchar m_ucNuclideClearButton = 0;

bool AmuletAccuracy_daily;

// Well Measure
extern unsigned char m_ucSetWellMeasure;

#define MESSAGE_BUFFER_SIZE 20
#define CMD_SENT_SIZE 200

volatile static short unsigned int uiMasterHead;
volatile static short unsigned int uiMasterTail;
volatile static char acMasterMessage[MESSAGE_BUFFER_SIZE][UART_BUFFER_SIZE];

volatile static short unsigned int uiSlaveHead;
volatile static short unsigned int uiSlaveTail;
volatile static char acSlaveMessage[MESSAGE_BUFFER_SIZE][UART_BUFFER_SIZE];

CMDSENT CmdSent[CMD_SENT_SIZE];
static short unsigned int uiCmdQueueHead;
static short unsigned int uiCmdQueueTail;
static short int iCmdQueue[CMD_SENT_SIZE + 1];

#define PAGE_STACK_SIZE 50
static unsigned char m_ucPageStack[PAGE_STACK_SIZE];
static unsigned char m_ucPageStackIndex = 0;

float moly_volume;

volatile unsigned long int Amulet_lastFactoryStamp = 0;
volatile unsigned long int Amulet_lastBypassStamp = 0;
volatile short Amulet_consecFactoryCount = 1;
volatile short Amulet_consecBypassCount = 1;
volatile short linearity_type = -1;

unsigned char AmuletHTMLIndex[AMULET_HTML_ARRAY_SIZE];

/**
 * \details Sets m_iCurPhase and m_iNextPhase to PHASE_GENERIC_CUR_NOP
 * \returns None
 */
void ClearNextPhase(void){
	m_iCurPhase = PHASE_GENERIC_CUR_NOP;
	m_iNextPhase = PHASE_GENERIC_CUR_NOP;
}

void undo_string_coding(char str[], char message0[]);
/**
 * \details This is continuously executed in the foreground to process the circular input queue for messages from the Amulet Touchscreen
 * \returns None
 */
void service_amulet(void){
	char acMessage[UART_BUFFER_SIZE];
	char acCommand[UART_BUFFER_SIZE];
	unsigned char ucCommandByte;
	unsigned char ucVariable;
	unsigned char ucValue, ucValue2;
	unsigned short int uiValue, uiValue2;
	CMDSENT CmdSent;
	char flgByte;
	unsigned char ucByte;
	unsigned char *pucByte;
	char flgWord;
	short unsigned int uiWord;
	short unsigned int *puiWord;
	char flgString;
	char acString[UART_BUFFER_SIZE];
	char *pcString;
	struct tm tmt;
	char acYear[5], acMonth[3], acDay[3], acHour[3], acMinute[3];
	short nuc_index;
    char date_time[35];
    short ch_num;
    char message[51];
    char nucshrink[25];
    char prtype;
    short index;
    short ch_type;
	char acTempMessage[61];
	int random_number, flg_passed;
	double double_value, sqrt_value;
	char key[20], fraction[20];
	
	if (QueryPendingMasterMessage() != 0) {
		PopMasterMessage(acCommand);

		ucCommandByte = (unsigned char) acCommand[0];

		switch (ucCommandByte) {
			// Get Byte
			case 0xD0:
				ucVariable = ConvertByte(&(acCommand[1]));
				acCommand[0] = 0xE0;
				switch (ucVariable) {
					case 0x00:
						ctoh(m_ucBargraphValue, acYear);
						strcat(acCommand, acYear);
						break;
				}
				SendUart2Tx(acCommand);
				break;

			// Get String
			case 0xD2:
				ucVariable = ConvertByte(&(acCommand[1]));
				acCommand[0] = 0xE2;
				switch (ucVariable) {
					case 0x00:
						memcpy (&tmt, gmtime(&clock_time), sizeof(tmt));
						sprintf(acYear, "%04d", tmt.tm_year + 1900);
						strcat(acCommand, acYear);
						break;

					case 0x01:
						memcpy (&tmt, gmtime(&clock_time), sizeof(tmt));
						sprintf(acMonth, "%02d", tmt.tm_mon + 1);
						strcat(acCommand, acMonth);
						break;

					case 0x02:
						memcpy (&tmt, gmtime(&clock_time), sizeof(tmt));
						sprintf(acDay, "%02d", tmt.tm_mday);
						strcat(acCommand, acDay);
						break;

					case 0x03:
						memcpy (&tmt, gmtime(&clock_time), sizeof(tmt));
						sprintf(acHour, "%02d", tmt.tm_hour);
						strcat(acCommand, acHour);
						break;

					case 0x04:
						memcpy (&tmt, gmtime(&clock_time), sizeof(tmt));
						sprintf(acMinute, "%02d", tmt.tm_min);
						strcat(acCommand, acMinute);
						break;
				}
				SendUart2Tx(acCommand);
				break;

			// Set Byte
			case 0xD5:
				SleepRefreshTimeout();
				ucVariable = ConvertByte(&(acCommand[1]));
				ucValue = ConvertByte(&(acCommand[3]));
				acCommand[0] = 0xE5;
				SendUart2Tx(acCommand);

				switch (ucVariable) {
					// Variable 0 - Switch On Features
					case 0x00:
						switch (ucValue) {
							// Emit Beep
							case 0x00:
								beep_amulet();
								break;

							// Release Openning Screen
							case 0x01:
								g_ucRelease = 0xff;
								break;

							// Emit Click
							case 0x02:
								click_amulet();
								break;

							// Sent Nuclide, Cal Num, Date/Time
							case 0x03:
								if(current.num_chambers>0){
									ch_num = current.main_chamber;
									strcpy(nucshrink, chamber[ch_num].nucdata.name);
									trim_and_shrink(nucshrink);
									if(DisplayActivity_SendActivity){
										if(nucshrink[0]!=0){
											GetExtendedNuclideString(chamber[ch_num].nuc_index, message);
											send_to_amulet_string(0, message);
										}
										else send_to_amulet_string(0,"");
										//else send_to_amulet_string(0, "*****");
									}
								}
								GetExtendedTimeInfoLanguageSec(&clock_time, date_time);
								send_to_amulet_string(2, date_time);
								if(current.num_chambers>0) disp_cal(measurement[ch_num].resp0);
								break;

							case 0x04:
								SetAmuletBackHTML();
								break;

							case 0x05:
								MCASetHV(Mca_getHVValue(), TRUE);
								beep_amulet();
								break;

							case 0x06:
								MCASetHV(Mca_getHVValue(), FALSE);
								beep_amulet();
								break;

							case 0x07:
								Mca_setTimeType(COUNT_LIVE);
								beep_amulet();
								break;

							case 0x08:
								Mca_setTimeType(COUNT_REAL);
								beep_amulet();
								break;

							case 0x09:
								AmuletGenericYesNo_LatchedDailyTestID = 0;
								AmuletAccuracy_daily = FALSE;
								break;

							case 0x0A:
								AmuletGenericYesNo_LatchedDailyTestID = AmuletDailyMenu_ChamberDailyTestID;
								AmuletAccuracy_daily = TRUE;
								break;

							case 0x0B:
								beep_amulet();
								EE_READ(print[1],(uchar *)&prtype);

								if(prtype == PR_TICKET){
									if(prticket(1)) prticket(2);
								}else prline();

								SetAmuletByte(102, 0xFF);
								break;

							case 0x0C:
								if(g_csec_tstamp - Amulet_lastFactoryStamp < 50) Amulet_consecFactoryCount++;
								else Amulet_consecFactoryCount = 1;
								Amulet_lastFactoryStamp = g_csec_tstamp;

								if(Amulet_consecFactoryCount >= 3){
								    strcpy((char *)m_acTitle, "Please enter factory password");
									//m_uiOffsetX = 245;
									//m_uiOffsetY = 98;
									m_fMinValue = 0.0;
									m_fMaxValue = 0.0;
									m_ucLenLimit = 3;
									m_ucLenFractionLimit = 0;
									m_ucKeyPadConfig = 4;
									m_uiSetKeyPad = 64;
									m_flgShowPlusMinus = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
									PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
									beep_amulet();
									Amulet_consecFactoryCount = 0;
								}
								break;

							case 0x0D:
								beep_amulet();
								if(current.system == CI) current.system = BQ;
								else current.system = CI;

								EE_WRITE(syst, &current.system);

								for(index=0; index<max_chambers; index++){
									if(!chamber[index].exists) continue;

									ch_type = chamber_type(index);
									if(measurement[index].dogain == 0) measurement[index].idecmin = get_decmin(ch_type, current.system);
									measurement[index].act0 = 0;
									measurement[index].kun0 = 0;
									measurement[index].idec = measurement[index].idecmin;
								}
								SetAmuletByte(103, 0xFF);
								break;

							case 0x0E:
								beep_amulet();
								//if(DisplayActivity_addedDigits >= MAX_ADDED_SIGNIFICANT) DisplayActivity_addedDigits = 0;
								//else DisplayActivity_addedDigits++;
								DisplayActivity_addedDigits++;
								SetAmuletByte(104, 0xFF);
								break;

							case 0x0F:
								m_flgShowPlusMinus = TRUE;
								break;

							case 0x10:
								m_flgShowPlusMinus = FALSE;
								break;

							case 0x11:
								if(chamber_C()) AmuletSetupLinearityStandardMenu_chamberType = C_CHAMB;
								else if(chamber_K()) AmuletSetupLinearityStandardMenu_chamberType = K_CHAMB;
								else AmuletSetupLinearityStandardMenu_chamberType = R_CHAMB;
								break;

							case 0x12:
								AmuletSetupLinearityStandardMenu_chamberType = P_CHAMB;
								break;

							case 0x13:
								if(CurrentPageStack() != AmuletHTMLIndex[LINEARITY_HTM]){
									m_ucClear = 30;
									linearity_type = 0;
									SetAmuletHTML(AmuletHTMLIndex[LINEARITY_HTM]);
									PushPageStack(AmuletHTMLIndex[LINEARITY_HTM]);
									beep_amulet();
								}
								break;

							case 0x14:
								if(CurrentPageStack() != AmuletHTMLIndex[LINEARITY_HTM]){
									m_ucClear = 30;
									linearity_type = 1;
									SetAmuletHTML(AmuletHTMLIndex[LINEARITY_HTM]);
									PushPageStack(AmuletHTMLIndex[LINEARITY_HTM]);
									beep_amulet();
								}
								break;

							case 0x15:
								if(CurrentPageStack() != AmuletHTMLIndex[LINEARITY_HTM]){
									m_ucClear = 30;
									linearity_type = 2;
									SetAmuletHTML(AmuletHTMLIndex[LINEARITY_HTM]);
									PushPageStack(AmuletHTMLIndex[LINEARITY_HTM]);
									beep_amulet();
								}
								break;

							case 0x16:
								SetAmuletHomeHTML();
								beep_amulet();
								break;

							case 0x17:
								if((g_csec_tstamp - Amulet_lastFactoryStamp) < 50) Amulet_consecFactoryCount++;
								else Amulet_consecFactoryCount = 1;
								Amulet_lastFactoryStamp = g_csec_tstamp;
								if(Amulet_consecFactoryCount >= 3){
									m_ucClear = 49;
									SetAmuletHTML(AmuletHTMLIndex[LOWLEVEL_HTM]);
									PushPageStack(AmuletHTMLIndex[LOWLEVEL_HTM]);
									beep_amulet();
									Amulet_consecFactoryCount = 0;
								}
								break;

							case 0x18:
								AmuletWellManual_showClearEu152LinCorr = TRUE;
								break;

							case 0x19:
								AmuletWellManual_showClearEu152LinCorr = FALSE;
								break;

							case 0x20:
								m_ucTimeMode = 0;
								break;

							case 0x21:
								m_ucTimeMode = 1;
								break;

							case 0x22:
								// Show Dose Table Button
								AmuletInfoMenu_DoseTable = TRUE;
								break;

							case 0x23:
								// Hide Dose Table Button
								AmuletInfoMenu_DoseTable = FALSE;;
								break;

							case 0x24:
								AmuletWellMeasurementMenu_unlockDetector = FALSE;
								break;

							case 0x25:
								AmuletWellMeasurementMenu_unlockDetector = TRUE;
								break;

							case 0x26:
								AmuletWellAddEditUserNuclides_addNew = TRUE;
								break;

							case 0x27:
								AmuletWellAddEditUserNuclides_addNew = FALSE;
								break;

							case 0x028:
								AmuletWellMeasureEfficiency_mode = MEASURE_EFF_EDIT_WELL;
								break;

							case 0x029:
								AmuletWellMeasureEfficiency_mode = MEASURE_EFF_EDIT_PROBE;
								break;

							case 0x02A:
								AmuletWellMeasureEfficiency_mode = MEASURE_EFF_ADD_WELL;
								break;

							case 0x02B:
								AmuletWellMeasureEfficiency_mode = MEASURE_EFF_ADD_PROBE;
								break;

							case 0x02C:
								AmuletWellMDATestMenu_mda.DetectorTest = Mca_installedDetector;
								break;

							case 0x02D:
								AmuletWellMDATestMenu_mda.DetectorTest = DET_DRILLEDPROBE700_AS_WELL;
								break;

							case 0x02E:
								AmuletWellMDATestMenu_mda.DetectorTest = DET_DRILLEDPROBE700_AS_PROBE;
								break;

							case 0x02F:
								AmuletWellChiTestMenu_test.DetectorTest = Mca_installedDetector;
								break;

							case 0x030:
								AmuletWellChiTestMenu_test.DetectorTest = DET_DRILLEDPROBE700_AS_WELL;
								break;

							case 0x031:
								AmuletWellChiTestMenu_test.DetectorTest = DET_DRILLEDPROBE700_AS_PROBE;
								break;

							case 0x32:
								AmuletLoginMenu_blockGuest = FALSE;
								break;

							case 0x33:
								AmuletLoginMenu_blockGuest = TRUE;
								break;

							case 0x34:
							case 0x35:
							case 0x36:
							case 0x37:
								AmuletLoginMenu_minRole = ucValue - 0x34;
								break;

							case 0x38:
								AmuletSecurity_securityMode = TRUE;
								break;

							case 0x39:
								AmuletSecurity_securityMode = FALSE;
								break;

							case 0x3A:
								AmuletSecurity_activeOnlyStaff = FALSE;
								break;

							case 0x3B:
								AmuletSecurity_activeOnlyStaff = TRUE;
								break;

							case 0x3C:
								AmuletWellSetupThyroidUptakeProtocol_setupMode = TRUE;
								break;

							case 0x3D:
								AmuletWellSetupThyroidUptakeProtocol_setupMode = FALSE;
								break;

							case 0x3E:
								AmuletWellMeasurementMenu_generalDetectorTest = Mca_installedDetector;
								break;

							case 0x3F:
								AmuletWellMeasurementMenu_generalDetectorTest = DET_DRILLEDPROBE700_AS_WELL;
								break;

							case 0x40:
								AmuletWellMeasurementMenu_generalDetectorTest = DET_DRILLEDPROBE700_AS_PROBE;
								break;

							case 0x41:
								keyboardWildcard = FALSE;
								break;

							case 0x42:
								keyboardWildcard = TRUE;
								break;

							case 0x43:
								AmuletWellThyroidUptakeEnterNormal_enter = TRUE;
								break;

							case 0x44:
								AmuletWellThyroidUptakeEnterNormal_enter = FALSE;
								break;

							case 0x45:
								AmuletWellRBCSurvivalEnterNormal_enter = TRUE;
								break;

							case 0x46:
								AmuletWellRBCSurvivalEnterNormal_enter = FALSE;
								break;

							case 0x47:
								if(g_csec_tstamp - Amulet_lastBypassStamp < 50) Amulet_consecBypassCount++;
								else Amulet_consecBypassCount = 1;
								Amulet_lastBypassStamp = g_csec_tstamp;

								if(Amulet_consecBypassCount >= 6){
									beep_amulet();
									srand((unsigned int) clock_time);
									do{
										random_number = rand();
										random_number = random_number % 65535;
										double_value = random_number;
										sqrt_value = sqrt(double_value);
										sprintf(key, "%.3f", sqrt_value);
										strcpy(fraction, &(key[strlen(key) - 3]));
										if(strcmp(fraction, "000") != 0) flg_passed = 1;
										else flg_passed = 0;
									} while(!flg_passed);
									current.bypass = random_number;
									Amulet_consecBypassCount = 1;
									Amulet_lastBypassStamp = 0;
									m_iCurPhase = 1;
									m_iNextPhase = 17;
								}
								break;

							case 0xFF:
								disable_all_interrupts();
								setup_short_watchdog();
								for(;;);
								break;
						}
						break;

					case 0x01:
						m_ucLenLimit = ucValue;
						break;

					case 0x02:
						m_ucLenFractionLimit = ucValue;
						break;

					case 0x03:
						m_iMenu = ucValue;
						if (ucValue == 0) {
							meas_screen = TRUE;
							erase_screen();
							set_display_time();
							set_nokey();
							if(current.num_chambers>0){
								ch_num = current.main_chamber;
								if(!chamber[ch_num].calkey){
									nuc_index = chamber[ch_num].nuc_index;
									set_nuclide_data(nuc_index, ch_num);
								}
							}
							m_iPhase = PHASE_GENERIC_NOP;
							m_iCurPhase = PHASE_GENERIC_CUR_NOP;
							m_iNextPhase = PHASE_GENERIC_CUR_NOP;
						}
						else {
							m_ucBargraphValue = 0;
							m_iPhase = PHASE_GENERIC_BEFORE_INIT;
							m_iCurPhase = PHASE_GENERIC_CUR_NOP;
							m_iNextPhase = PHASE_GENERIC_CUR_NOP;
							//printf("received 0x03 - %lu\n",g_msec_tstamp);
						}
						break;

					case 0x04:
						if (m_cKeypress[0] == 0) {
							m_cKeypress[0] = (char)ucValue;
						}
						break;

					// Change m_iPhase without a prev
					case 0x05:
							m_iPhase = ucValue;
						break;

					case 0x06:
							if(CurrentPageStack() != AmuletHTMLIndex[NUCLIDE2_HTM]){
								m_ucSetNuclide = ucValue;
								SetAmuletHTML(AmuletHTMLIndex[NUCLIDE2_HTM]);
								PushPageStack(AmuletHTMLIndex[NUCLIDE2_HTM]);
								beep_amulet();
							}
						break;

					case 0x07: // Set Month One
						m_ucMonth = ucValue;
						break;

					case 0x08: // Set Day One
						m_ucDay = ucValue;
						break;

					case 0x09: // Set Day Ten
						m_ucDay = m_ucDay + (10 * ucValue);
						break;

					case 0x0A: // Set Hour One
						m_ucHour = ucValue;
						break;

					case 0x0B: // Set Hour Ten
						m_ucHour = m_ucHour + (10 * ucValue);
						break;

					case 0x0C: // Set Minute One
						m_ucMinute = ucValue;
						break;

					case 0x0D: // Set Minute Ten
						m_ucMinute = m_ucMinute + (10 * ucValue);
						break;

					case 0x0E: // Clear Menu Global Variables
						switch(ucValue){
							case 1:
								if(CurrentPageStack() != AmuletHTMLIndex[INVENTORYADD_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[INVENTORYADD_HTM]);
									PushPageStack(AmuletHTMLIndex[INVENTORYADD_HTM]);
									beep_amulet();
								}
								break;

							case 2:
								if(CurrentPageStack() != AmuletHTMLIndex[INVENTORYWITHDRAW_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[INVENTORYWITHDRAW_HTM]);
									PushPageStack(AmuletHTMLIndex[INVENTORYWITHDRAW_HTM]);
									beep_amulet();
								}
								break;

							case 3:
								if(CurrentPageStack() != AmuletHTMLIndex[INVENTORYKIT_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[INVENTORYKIT_HTM]);
									PushPageStack(AmuletHTMLIndex[INVENTORYKIT_HTM]);
									beep_amulet();
								}
								break;

							case 4:
								if(CurrentPageStack() != AmuletHTMLIndex[INFO_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[INFO_HTM]);
									PushPageStack(AmuletHTMLIndex[INFO_HTM]);
									beep_amulet();
								}
								break;

							case 5:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUP_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUP_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUP_HTM]);
									beep_amulet();
								}
								break;

							case 6:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPSOURCES_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPSOURCES_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPSOURCES_HTM]);
									beep_amulet();
								}
								break;

							case 7:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPMOLY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPMOLY_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPMOLY_HTM]);
									beep_amulet();
								}
								break;

							case 8:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPNUCLIDE_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPNUCLIDE_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPNUCLIDE_HTM]);
									beep_amulet();
								}
								break;

							case 9:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPLINEARITY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPLINEARITY_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPLINEARITY_HTM]);
									beep_amulet();
								}
								break;

							case 10:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPREMOTE_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPREMOTE_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPREMOTE_HTM]);
									beep_amulet();
								}
								break;

							case 11:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPCALNUM_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPCALNUM_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPCALNUM_HTM]);
									beep_amulet();
								}
								break;

							case 12:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPLINEARITYSTANDARD_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPLINEARITYSTANDARD_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPLINEARITYSTANDARD_HTM]);
									beep_amulet();
								}
								break;

							case 13:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPLINEARITYLINEATOR_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPLINEARITYLINEATOR_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPLINEARITYLINEATOR_HTM]);
									beep_amulet();
								}
								break;

							case 14:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPLINEARITYCALICHECK_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPLINEARITYCALICHECK_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPLINEARITYCALICHECK_HTM]);
									beep_amulet();
								}
								break;

							case 15:
								if(CurrentPageStack() != AmuletHTMLIndex[DOSETABLE_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[DOSETABLE_HTM]);
									PushPageStack(AmuletHTMLIndex[DOSETABLE_HTM]);
									beep_amulet();
								}
								break;

							case 16:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLMAINSCREEN_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLMAINSCREEN_HTM]);
									ClearPageStack();
									PushPageStack(AmuletHTMLIndex[WELLMAINSCREEN_HTM]);
									beep_amulet();
								}
								break;

							case 17:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLAUTOCALIBRATE_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLAUTOCALIBRATE_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLAUTOCALIBRATE_HTM]);
									beep_amulet();
								}
								break;

							case 18:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLMEASUREMENT_HTM]){
									m_ucClear = ucValue;
									AmuletWellMeasurementMenu_WipeBackgroundThreshold = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
									beep_amulet();
								}
								break;

							case 19:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPNUCLIDE_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPNUCLIDE_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPNUCLIDE_HTM]);
									beep_amulet();
								}
								break;

							case 20:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPSEALED_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPSEALED_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPSEALED_HTM]);
									beep_amulet();
								}
								break;

							case 21:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPSEALED2_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPSEALED2_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPSEALED2_HTM]);
									beep_amulet();
								}
								break;

							case 22:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPTESTSOURCE_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPTESTSOURCE_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPTESTSOURCE_HTM]);
									beep_amulet();
								}
								break;

							case 23:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPTRIGGERLEVEL_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPTRIGGERLEVEL_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPTRIGGERLEVEL_HTM]);
									beep_amulet();
								}
								break;

							case 24:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPEFFICIENCIES_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPEFFICIENCIES_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPEFFICIENCIES_HTM]);
									beep_amulet();
								}
								break;

							case 25:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLEDITEFFICIENCIES_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLEDITEFFICIENCIES_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLEDITEFFICIENCIES_HTM]);
									beep_amulet();
								}
								break;

							case 26:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLMEASUREEFFICIENCY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREEFFICIENCY_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLMEASUREEFFICIENCY_HTM]);
									beep_amulet();
								}
								break;

							case 27:
								if(CurrentPageStack() != AmuletHTMLIndex[DAILY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[DAILY_HTM]);
									PushPageStack(AmuletHTMLIndex[DAILY_HTM]);
									beep_amulet();
								}
								break;

							case 28:
								if(CurrentPageStack() != AmuletHTMLIndex[ACCURACY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[ACCURACY_HTM]);
									PushPageStack(AmuletHTMLIndex[ACCURACY_HTM]);
									beep_amulet();
								}
								break;

							case 29:
								if(CurrentPageStack() != AmuletHTMLIndex[AUTOCONSTANCY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[AUTOCONSTANCY_HTM]);
									PushPageStack(AmuletHTMLIndex[AUTOCONSTANCY_HTM]);
									beep_amulet();
								}
								break;

							case 30:
								if(CurrentPageStack() != AmuletHTMLIndex[LINEARITY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[LINEARITY_HTM]);
									PushPageStack(AmuletHTMLIndex[LINEARITY_HTM]);
									beep_amulet();
								}
								break;

							case 31:
								if(CurrentPageStack() != AmuletHTMLIndex[ONESTRIP_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[ONESTRIP_HTM]);
									PushPageStack(AmuletHTMLIndex[ONESTRIP_HTM]);
									beep_amulet();
								}
								break;

							case 32:
								if(CurrentPageStack() != AmuletHTMLIndex[TWOSTRIP_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[TWOSTRIP_HTM]);
									PushPageStack(AmuletHTMLIndex[TWOSTRIP_HTM]);
									beep_amulet();
								}
								break;

							case 33:
								if(CurrentPageStack() != AmuletHTMLIndex[HMPAO_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[HMPAO_HTM]);
									PushPageStack(AmuletHTMLIndex[HMPAO_HTM]);
									beep_amulet();
								}
								break;

							case 34:
								if(CurrentPageStack() != AmuletHTMLIndex[MAG3_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[MAG3_HTM]);
									PushPageStack(AmuletHTMLIndex[MAG3_HTM]);
									beep_amulet();
								}
								break;

							case 35:
								if(CurrentPageStack() != AmuletHTMLIndex[MOLY_HTM]){
									ch_num = current.main_chamber;

									if(chamber_type(ch_num) == P_CHAMB){
										//Amulet_DisplayNotification("PET Chamber", "The PET Chamber can not run a Moly Assay", TRUE);
										get_amulet_message(L_PET_CHAMBER,message);    // "PET Chamber"
										get_amulet_message(L_PET_CHAMBER_CANNOT_RUN_MOLY_ASSAY,acTempMessage);    // "The PET chamber can not run a Moly Assay"
										Amulet_DisplayNotification(message, acTempMessage, TRUE);
									}else if((chamber_type(ch_num) == C_CHAMB) || (chamber_type(ch_num) == K_CHAMB)){
										get_amulet_message(L_10_CHAMBER,message);    // "1 Atm Chamber"
										get_amulet_message(L_10_CHAMBER_CANNOT_RUN_MOLY_ASSAY,acTempMessage);    // "The 1 Atm chamber can not run a Moly Assay"
										Amulet_DisplayNotification(message, acTempMessage, TRUE);
									}else{
										m_ucClear = ucValue;
										SetAmuletHTML(AmuletHTMLIndex[MOLY_HTM]);
										PushPageStack(AmuletHTMLIndex[MOLY_HTM]);
										beep_amulet();
									}
								}
								break;

							case 36:
								if(CurrentPageStack() != AmuletHTMLIndex[FACTORY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[FACTORY_HTM]);
									PushPageStack(AmuletHTMLIndex[FACTORY_HTM]);
									beep_amulet();
								}
								break;

							case 37:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPCHAMBER_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPCHAMBER_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPCHAMBER_HTM]);
									beep_amulet();
								}
								break;

							case 38:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLMEASUREMENTS_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENTS_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLMEASUREMENTS_HTM]);
									beep_amulet();
								}
								break;

							case 39:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLEDITFULLEFFICIENCY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLEDITFULLEFFICIENCY_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLEDITFULLEFFICIENCY_HTM]);
									beep_amulet();
								}
								break;

							case 40:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPCALIBSERIALNUM_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPCALIBSERIALNUM_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPCALIBSERIALNUM_HTM]);
									beep_amulet();
								}
								break;

							case 41:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSCHILLING_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSCHILLING_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSCHILLING_HTM]);
									beep_amulet();
								}
								break;

							case 42:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLPLASMA_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLPLASMA_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLPLASMA_HTM]);
									beep_amulet();
								}
								break;

							case 43:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLRBC_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLRBC_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLRBC_HTM]);
									beep_amulet();
								}
								break;

							case 44:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLFACTORY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLFACTORY_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLFACTORY_HTM]);
									beep_amulet();
								}
								break;

							case 45:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLMDATEST_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLMDATEST_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLMDATEST_HTM]);
									beep_amulet();
								}
								break;

							case 46:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLCHITEST_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLCHITEST_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLCHITEST_HTM]);
									beep_amulet();
								}
								break;

							case 47:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPRHOTKEYS_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPRHOTKEYS_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPRHOTKEYS_HTM]);
									beep_amulet();
								}
								break;

							case 48:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPPHOTKEYS_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPPHOTKEYS_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPPHOTKEYS_HTM]);
									beep_amulet();
								}
								break;

							case 49:
								if(CurrentPageStack() != AmuletHTMLIndex[LOWLEVEL_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[LOWLEVEL_HTM]);
									PushPageStack(AmuletHTMLIndex[LOWLEVEL_HTM]);
									beep_amulet();
								}
								break;

							case 50:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSTABILITYTEST_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSTABILITYTEST_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSTABILITYTEST_HTM]);
									beep_amulet();
								}
								break;

							case 51:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPCOMMUNICATIONS_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPCOMMUNICATIONS_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPCOMMUNICATIONS_HTM]);
									beep_amulet();
								}
								break;

							case 52:
								if(CurrentPageStack() != AmuletHTMLIndex[ETHERNET_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[ETHERNET_HTM]);
									PushPageStack(AmuletHTMLIndex[ETHERNET_HTM]);
									beep_amulet();
								}
								break;

							case 53:
							case 54:
							case 55:
							case 56:
							case 57:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPTYPES_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPTYPES_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPTYPES_HTM]);
									beep_amulet();
								}
								break;

							case 58:
							case 59:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPADDEDITLOCATION_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPADDEDITLOCATION_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPADDEDITLOCATION_HTM]);
									beep_amulet();
								}
								break;

							case 60:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPDELETELOCATION_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPDELETELOCATION_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPDELETELOCATION_HTM]);
									beep_amulet();
								}
								break;

							case 61:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLWIPELIST_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLWIPELIST_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLWIPELIST_HTM]);
									beep_amulet();
								}
								break;

							case 62:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLWIPESEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLWIPESEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLWIPESEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 63:
								if(CurrentPageStack() != AmuletHTMLIndex[INACTIVATE_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[INACTIVATE_HTM]);
									PushPageStack(AmuletHTMLIndex[INACTIVATE_HTM]);
									beep_amulet();
								}
								break;

							case 64:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSYSTEMTESTSEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSYSTEMTESTSEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSYSTEMTESTSEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 65:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLMDASEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLMDASEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLMDASEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 66:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLCHISEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLCHISEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLCHISEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 67:
								if(CurrentPageStack() != AmuletHTMLIndex[TESTIDENT_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[TESTIDENT_HTM]);
									PushPageStack(AmuletHTMLIndex[TESTIDENT_HTM]);
									beep_amulet();
								}
								break;

							case 68:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSCHILLINGSEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSCHILLINGSEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSCHILLINGSEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 69:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLPLASMASEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLPLASMASEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLPLASMASEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 70:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLRBCSEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLRBCSEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLRBCSEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 71:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLAUTOCALSEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLAUTOCALSEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLAUTOCALSEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 72:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLFACTORYDETECTORS_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLFACTORYDETECTORS_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLFACTORYDETECTORS_HTM]);
									beep_amulet();
								}
								break;

							case 73:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPUSERNUCLIDES_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPUSERNUCLIDES_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPUSERNUCLIDES_HTM]);
									beep_amulet();
								}
								break;

							case 74:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLADDEDITUSERNUCLIDES_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLADDEDITUSERNUCLIDES_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLADDEDITUSERNUCLIDES_HTM]);
									beep_amulet();
								}
								break;

							case 75:
								if(CurrentPageStack() != AmuletHTMLIndex[LOGIN_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[LOGIN_HTM]);
									PushPageStack(AmuletHTMLIndex[LOGIN_HTM]);
									beep_amulet();
								}
								break;

							case 76:
								if(CurrentPageStack() != AmuletHTMLIndex[SECURITY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SECURITY_HTM]);
									PushPageStack(AmuletHTMLIndex[SECURITY_HTM]);
									beep_amulet();
								}
								break;

							case 77:
								if(CurrentPageStack() != AmuletHTMLIndex[ADDEDITUSER_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[ADDEDITUSER_HTM]);
									PushPageStack(AmuletHTMLIndex[ADDEDITUSER_HTM]);
									beep_amulet();
								}
								break;

							case 78:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLADVANCEDSETUP_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLADVANCEDSETUP_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLADVANCEDSETUP_HTM]);
									beep_amulet();
								}
								break;

							case 79:
								if(CurrentPageStack() != AmuletHTMLIndex[OTHER_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[OTHER_HTM]);
									PushPageStack(AmuletHTMLIndex[OTHER_HTM]);
									beep_amulet();
								}
								break;

							case 80:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPBIOASSAY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPBIOASSAY_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPBIOASSAY_HTM]);
									beep_amulet();
								}
								break;

							case 81:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPBIOASSAYENTEREFFICIENCY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPBIOASSAYENTEREFFICIENCY_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPBIOASSAYENTEREFFICIENCY_HTM]);
									beep_amulet();
								}
								break;

							case 82:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPBIOASSAYMEASUREEFFICIENCY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPBIOASSAYMEASUREEFFICIENCY_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPBIOASSAYMEASUREEFFICIENCY_HTM]);
									beep_amulet();
								}
								break;

							case 83:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLBIOASSAY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLBIOASSAY_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLBIOASSAY_HTM]);
									beep_amulet();
								}
								break;

							case 84:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM]);
									beep_amulet();
								}
								break;

							case 85:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLBIOASSAYSEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLBIOASSAYSEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLBIOASSAYSEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 86:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPBIOASSAYENTERROI_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPBIOASSAYENTERROI_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPBIOASSAYENTERROI_HTM]);
									beep_amulet();
								}
								break;

							case 87:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLSETUPTHYROIDUPTAKEPROTOCOL_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLSETUPTHYROIDUPTAKEPROTOCOL_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLSETUPTHYROIDUPTAKEPROTOCOL_HTM]);
									beep_amulet();
								}
								break;

							case 88:
								if(CurrentPageStack() != AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
									PushPageStack(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
									beep_amulet();
								}
								break;

							case 89:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLTHYROIDUPTAKE_HTM]){
									AmuletWellThyroidUptake_runMode = TRUE;
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKE_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKE_HTM]);
									beep_amulet();
								}
								break;

							case 90:
								if(CurrentPageStack() != AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM]);
									PushPageStack(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM]);
									beep_amulet();
								}
								break;

							case 91:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLTHYROIDUPTAKETEST_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKETEST_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKETEST_HTM]);
									beep_amulet();
								}
								break;

							case 92:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM]);
									beep_amulet();
								}
								break;

							case 93:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
									beep_amulet();
								}
								break;

							case 94:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
									beep_amulet();
								}
								break;

							case 95:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERNORMAL_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERNORMAL_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERNORMAL_HTM]);
									beep_amulet();
								}
								break;

							case 96:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLTHYROIDUPTAKE_HTM]){
									AmuletWellThyroidUptake_runMode = FALSE;
									m_ucClear = 89;
									SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKE_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKE_HTM]);
									beep_amulet();
								}
								break;

							case 99:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLRBCSURVIVAL_HTM]){
									AmuletWellRBCSurvival_runMode = TRUE;
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLRBCSURVIVAL_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLRBCSURVIVAL_HTM]);
									beep_amulet();
								}
								break;

							case 100:
								if(CurrentPageStack() != AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM]);
									PushPageStack(AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM]);
									beep_amulet();
								}
								break;

							case 101:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLRBCSURVIVALTEST_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLRBCSURVIVALTEST_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLRBCSURVIVALTEST_HTM]);
									beep_amulet();
								}
								break;

							case 102:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM]);
									beep_amulet();
								}
								break;

							case 103:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLRBCSURVIVAL_HTM]){
									AmuletWellRBCSurvival_runMode = FALSE;
									m_ucClear = 99;
									SetAmuletHTML(AmuletHTMLIndex[WELLRBCSURVIVAL_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLRBCSURVIVAL_HTM]);
									beep_amulet();
								}
								break;

							case 104:
								if(CurrentPageStack() != AmuletHTMLIndex[WELLRBCSURVIVALENTERNORMAL_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[WELLRBCSURVIVALENTERNORMAL_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLRBCSURVIVALENTERNORMAL_HTM]);
									beep_amulet();
								}
								break;

							case 105:
								if(CurrentPageStack() != AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]){
									AmuletAutoLinearityTest_mode = 0;
									m_ucClear = 105;
									SetAmuletHTML(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
									PushPageStack(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
									beep_amulet();
								}
								break;

							case 106:
								if(CurrentPageStack() != AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]){
									AmuletAutoLinearityTest_mode = 1;
									m_ucClear = 105;
									SetAmuletHTML(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
									PushPageStack(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
									beep_amulet();
								}
								break;

							case 107:
								if(CurrentPageStack() != AmuletHTMLIndex[AUTOLINEARITYSEARCH_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[AUTOLINEARITYSEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[AUTOLINEARITYSEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 108:
							case 109:
							case 110:
							case 111:
							case 112:
								if(CurrentPageStack() != AmuletHTMLIndex[CHAMBERSEARCH_HTM]){
									AmuletChamberSearch_mode = ucValue;
									m_ucClear = 108;
									SetAmuletHTML(AmuletHTMLIndex[CHAMBERSEARCH_HTM]);
									PushPageStack(AmuletHTMLIndex[CHAMBERSEARCH_HTM]);
									beep_amulet();
								}
								break;

							case 113:
								if(CurrentPageStack() != AmuletHTMLIndex[HALFLIFECALCULATOR_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[HALFLIFECALCULATOR_HTM]);
									PushPageStack(AmuletHTMLIndex[HALFLIFECALCULATOR_HTM]);
									beep_amulet();
								}
								break;

							case 114:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPKEY_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPKEY_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPKEY_HTM]);
									beep_amulet();
								}
								break;

							case 115:
								if(CurrentPageStack() != AmuletHTMLIndex[SETUPPASSWORD_HTM]){
									m_ucClear = ucValue;
									SetAmuletHTML(AmuletHTMLIndex[SETUPPASSWORD_HTM]);
									PushPageStack(AmuletHTMLIndex[SETUPPASSWORD_HTM]);
									beep_amulet();
								}
								break;
						}
						break;

					case 0x0F:
						switch(ucValue){
							case 1:
								if(CurrentPageStack() != AmuletHTMLIndex[KEYPAD2_HTM]){
									beep_amulet();
									m_fMinValue = 60.0;
									m_fMaxValue = 9960.0;
									if(current.system==CI){
									    strcpy((char *)m_acTitle, "Please Enter Background Threshold (cpm):");
										m_ucLenLimit = 4;
										m_ucLenFractionLimit = 2;
									}else{
										strcpy((char *)m_acTitle, "Please Enter Background Threshold (cps):");
										m_ucLenLimit = 3;
										m_ucLenFractionLimit = 3;
										m_fMinValue = Mca_convertDpmToBq(m_fMinValue);
										m_fMaxValue = Mca_convertDpmToBq(m_fMaxValue);
									}
									m_ucKeyPadConfig = 2;
									m_uiSetKeyPad = 69;
									m_flgShowPlusMinus = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
									PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
								}
								break;

							case 2:
								if(CurrentPageStack() != AmuletHTMLIndex[KEYPAD2_HTM]){
									beep_amulet();
									m_fMinValue = 60.0;
									m_fMaxValue = 25000.0;
									if(current.system==CI){
										strcpy((char *)m_acTitle, "Please Enter Work Area Threshold (dpm):");
										m_ucLenLimit = 5;
										m_ucLenFractionLimit = 2;
									}else{
										strcpy((char *)m_acTitle, "Please Enter Work Area Threshold (Bq):");
										m_ucLenLimit = 3;
										m_ucLenFractionLimit = 3;
										m_fMinValue = Mca_convertDpmToBq(m_fMinValue);
										m_fMaxValue = Mca_convertDpmToBq(m_fMaxValue);
									}
									m_ucKeyPadConfig = 2;
									m_uiSetKeyPad = 70;
									m_flgShowPlusMinus = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
									PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
								}
								break;

							case 3:
								if(CurrentPageStack() != AmuletHTMLIndex[KEYPAD2_HTM]){
									beep_amulet();
									m_fMinValue = 60.0;
									m_fMaxValue = 2500.0;
									if(current.system==CI){
										strcpy((char *)m_acTitle, "Please Enter Unrestricted Threshold (dpm):");
										m_ucLenLimit = 4;
										m_ucLenFractionLimit = 2;
									}else{
										strcpy((char *)m_acTitle, "Please Enter Unrestricted Threshold (Bq):");
										m_ucLenLimit = 2;
										m_ucLenFractionLimit = 3;
										m_fMinValue = Mca_convertDpmToBq(m_fMinValue);
										m_fMaxValue = Mca_convertDpmToBq(m_fMaxValue);
									}
									m_ucKeyPadConfig = 2;
									m_uiSetKeyPad = 71;
									m_flgShowPlusMinus = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
									PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
								}
								break;

							case 4:
								if(CurrentPageStack() != AmuletHTMLIndex[KEYPAD2_HTM]){
									beep_amulet();
									m_fMinValue = .1;
									m_fMaxValue = 270.0;
									if(current.system==CI){
										strcpy((char *)m_acTitle, "Please Enter Sealed Threshold (nCi):");
										m_ucLenLimit = 3;
										m_ucLenFractionLimit = 3;
									}else{
										strcpy((char *)m_acTitle, "Please Enter Sealed Threshold (Bq):");
										m_ucLenLimit = 5;
										m_ucLenFractionLimit = 3;
										m_fMinValue /= 1e+9;
										m_fMinValue = Mca_convertCiToDpm(m_fMinValue);
										m_fMinValue = Mca_convertDpmToBq(m_fMinValue);
										m_fMaxValue /= 1e+9;
										m_fMaxValue = Mca_convertCiToDpm(m_fMaxValue);
										m_fMaxValue = Mca_convertDpmToBq(m_fMaxValue);
									}
									m_ucKeyPadConfig = 2;
									m_uiSetKeyPad = 72;
									m_flgShowPlusMinus = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
									PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
								}
								break;

							case 5:
								if(CurrentPageStack() != AmuletHTMLIndex[KEYPAD2_HTM]){
									beep_amulet();
									m_fMinValue = 60.0;
									m_fMaxValue = 2500.0;
									if(current.system==CI){
										strcpy((char *)m_acTitle, "Please Enter Package Threshold (dpm):");
										m_ucLenLimit = 4;
										m_ucLenFractionLimit = 2;
									}else{
										strcpy((char *)m_acTitle, "Please Enter Package Threshold (Bq):");
										m_ucLenLimit = 2;
										m_ucLenFractionLimit = 3;
										m_fMinValue = Mca_convertDpmToBq(m_fMinValue);
										m_fMaxValue = Mca_convertDpmToBq(m_fMaxValue);
									}
									m_ucKeyPadConfig = 2;
									m_uiSetKeyPad = 100;
									m_flgShowPlusMinus = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
									PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
								}
								break;

							case 6:
								if(CurrentPageStack() != AmuletHTMLIndex[KEYPAD2_HTM]){
									beep_amulet();
									switch(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID){
										case 2: 	// Work Area
											m_fMinValue = 60.0;
											m_fMaxValue = 25000.0;
											if(current.system==CI){
												strcpy((char *)m_acTitle, "Please Enter Work Area Threshold (dpm):");
												m_ucLenLimit = 5;
												m_ucLenFractionLimit = 2;
											}else{
												strcpy((char *)m_acTitle, "Please Enter Work Area Threshold (Bq):");
												m_ucLenLimit = 3;
												m_ucLenFractionLimit = 3;
												m_fMinValue = Mca_convertDpmToBq(m_fMinValue);
												m_fMaxValue = Mca_convertDpmToBq(m_fMaxValue);
											}
											m_uiSetKeyPad = 102;
											break;

										case 3:		// Unrestricted
											m_fMinValue = 60.0;
											m_fMaxValue = 2500.0;
											if(current.system==CI){
												strcpy((char *)m_acTitle, "Please Enter Unrestricted Threshold (dpm):");
												m_ucLenLimit = 4;
												m_ucLenFractionLimit = 2;
											}else{
												strcpy((char *)m_acTitle, "Please Enter Unrestricted Threshold (Bq):");
												m_ucLenLimit = 2;
												m_ucLenFractionLimit = 3;
												m_fMinValue = Mca_convertDpmToBq(m_fMinValue);
												m_fMaxValue = Mca_convertDpmToBq(m_fMaxValue);
											}
											m_uiSetKeyPad = 103;
											break;

										case 4:		// Sealed
											m_fMinValue = .1;
											m_fMaxValue = 270.0;
											if(current.system==CI){
												strcpy((char *)m_acTitle, "Please Enter Sealed Threshold (nCi):");
												m_ucLenLimit = 3;
												m_ucLenFractionLimit = 3;
											}else{
												strcpy((char *)m_acTitle, "Please Enter Sealed Threshold (Bq):");
												m_ucLenLimit = 5;
												m_ucLenFractionLimit = 3;
												m_fMinValue /= 1e+9;
												m_fMinValue = Mca_convertCiToDpm(m_fMinValue);
												m_fMinValue = Mca_convertDpmToBq(m_fMinValue);
												m_fMaxValue /= 1e+9;
												m_fMaxValue = Mca_convertCiToDpm(m_fMaxValue);
												m_fMaxValue = Mca_convertDpmToBq(m_fMaxValue);
											}
											m_uiSetKeyPad = 104;
											break;

										case 5:		// Package
											m_fMinValue = 60.0;
											m_fMaxValue = 2500.0;
											if(current.system==CI){
												strcpy((char *)m_acTitle, "Please Enter Package Threshold (dpm):");
												m_ucLenLimit = 4;
												m_ucLenFractionLimit = 2;
											}else{
												strcpy((char *)m_acTitle, "Please Enter Package Threshold (Bq):");
												m_ucLenLimit = 2;
												m_ucLenFractionLimit = 3;
												m_fMinValue = Mca_convertDpmToBq(m_fMinValue);
												m_fMaxValue = Mca_convertDpmToBq(m_fMaxValue);
											}
											m_uiSetKeyPad = 105;
											break;
									}
									m_ucKeyPadConfig = 2;
									m_flgShowPlusMinus = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
									PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
								}
								break;
						}
						break;

					case 0x10:
						m_ucKeyPadConfig = ucValue;
						break;

					//case 0x11:
					//	m_ucLenLimit = ucValue;
					//	break;

					case 0x12:
						if(CurrentPageStack() != AmuletHTMLIndex[KEYBOARD2_HTM]){
							m_ucSetKeyBoard = ucValue;
							SetAmuletHTML(AmuletHTMLIndex[KEYBOARD2_HTM]);
							PushPageStack(AmuletHTMLIndex[KEYBOARD2_HTM]);
							beep_amulet();
						}
						break;

					case 0x13:
						if(CurrentPageStack() != AmuletHTMLIndex[SETACTIVITY_HTM]){
							m_ucSetActivity = ucValue;
							SetAmuletHTML(AmuletHTMLIndex[SETACTIVITY_HTM]);
							PushPageStack(AmuletHTMLIndex[SETACTIVITY_HTM]);
							beep_amulet();
						}
						break;

					case 0x14:
						if((ucValue == 12) && (!current.future_date_input)){
							if(CurrentPageStack() != AmuletHTMLIndex[KEYPAD2_HTM]){
								ch_num = current.main_chamber;
								strcpy(nucshrink, chamber[ch_num].nucdata.name);
								trim_and_shrink(nucshrink);
								if(nucshrink[0]!=0 && (!measurement[ch_num].over_flag)){
									if(current.language == ENGLISH) strcat((char *) m_acTitle, " hhmm[ss]/DD/MM/YY");
									else if(current.language == SPANISH) strcat((char *) m_acTitle, " hhmm[ss]/DD/MM/AA");
									else strcat((char *) m_acTitle, " hhmm[ss]/DD/MM/YY");
									m_ucLenLimit = 30;
									m_ucLenFractionLimit = 0;
									m_fMinValue = 0.0;
									m_fMaxValue = 0.0;
									m_ucKeyPadConfig = 3;
									m_uiSetKeyPad = 217;
									m_flgShowPlusMinus = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
									PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
									beep_amulet();
								}else{
									send_amulet_message(L_DOSE_DECAY, 6);    // "Dose Decay"
									send_to_amulet_string(7, "");
									measurement[ch_num].future.dosetime = NO_TIME;
									SetAmuletByte(101, 0xFF);
								}
							}
						}else{
							switch(current.time_format){
								case 0:
									if(CurrentPageStack() != AmuletHTMLIndex[SETTIME2_HTM]){
										if(ucValue == 12){
											ch_num = current.main_chamber;
											strcpy(nucshrink, chamber[ch_num].nucdata.name);
											trim_and_shrink(nucshrink);
											if(nucshrink[0]!=0 && (!measurement[ch_num].over_flag)){
												m_ucSetTime = ucValue;
												m_ucSecMode = 1;
												//m_ucTimeMode = 0;
												SetAmuletHTML(AmuletHTMLIndex[SETTIME2_HTM]);
												PushPageStack(AmuletHTMLIndex[SETTIME2_HTM]);
												beep_amulet();
											}else{
												send_amulet_message(L_DOSE_DECAY, 6);    // "Dose Decay"
												send_to_amulet_string(7, "");
												measurement[ch_num].future.dosetime = NO_TIME;
												SetAmuletByte(101, 0xFF);
											}
										}else{
											m_ucSetTime = ucValue;
											if(ucValue == 4 || ucValue == 5) m_ucSecMode = 1;
											else m_ucSecMode = 0;
											//m_ucTimeMode = 0;
											SetAmuletHTML(AmuletHTMLIndex[SETTIME2_HTM]);
											PushPageStack(AmuletHTMLIndex[SETTIME2_HTM]);
											beep_amulet();
										}
									}
									break;

								case 1:
									if(CurrentPageStack() != AmuletHTMLIndex[SETTIME2EUR_HTM]){
										if(ucValue == 12){
											ch_num = current.main_chamber;
											strcpy(nucshrink, chamber[ch_num].nucdata.name);
											trim_and_shrink(nucshrink);
											if(nucshrink[0]!=0 && (!measurement[ch_num].over_flag)){
												m_ucSetTime = ucValue;
												m_ucSecMode = 1;
												//m_ucTimeMode = 0;
												SetAmuletHTML(AmuletHTMLIndex[SETTIME2EUR_HTM]);
												PushPageStack(AmuletHTMLIndex[SETTIME2EUR_HTM]);
												beep_amulet();
											}else{
												send_amulet_message(L_DOSE_DECAY, 6);    // "Dose Decay"
												send_to_amulet_string(7, "");
												measurement[ch_num].future.dosetime = NO_TIME;
												SetAmuletByte(101, 0xFF);
											}
										}else{
											m_ucSetTime = ucValue;
											if(ucValue == 4 || ucValue == 5) m_ucSecMode = 1;
											else m_ucSecMode = 0;
											//m_ucTimeMode = 0;
											SetAmuletHTML(AmuletHTMLIndex[SETTIME2EUR_HTM]);
											PushPageStack(AmuletHTMLIndex[SETTIME2EUR_HTM]);
											beep_amulet();
										}
									}
									break;

								case 2:
									if(CurrentPageStack() != AmuletHTMLIndex[SETTIME2JAP_HTM]){
										if(ucValue == 12){
											ch_num = current.main_chamber;
											strcpy(nucshrink, chamber[ch_num].nucdata.name);
											trim_and_shrink(nucshrink);
											if(nucshrink[0]!=0 && (!measurement[ch_num].over_flag)){
												m_ucSetTime = ucValue;
												m_ucSecMode = 1;
												//m_ucTimeMode = 0;
												SetAmuletHTML(AmuletHTMLIndex[SETTIME2JAP_HTM]);
												PushPageStack(AmuletHTMLIndex[SETTIME2JAP_HTM]);
												beep_amulet();
											}else{
												send_amulet_message(L_DOSE_DECAY, 6);    // "Dose Decay"
												send_to_amulet_string(7, "");
												measurement[ch_num].future.dosetime = NO_TIME;
												SetAmuletByte(101, 0xFF);
											}
										}else{
											m_ucSetTime = ucValue;
											if(ucValue == 4 || ucValue == 5) m_ucSecMode = 1;
											else m_ucSecMode = 0;
											//m_ucTimeMode = 0;
											SetAmuletHTML(AmuletHTMLIndex[SETTIME2JAP_HTM]);
											PushPageStack(AmuletHTMLIndex[SETTIME2JAP_HTM]);
											beep_amulet();
										}
									}
									break;

								default:
									if(CurrentPageStack() != AmuletHTMLIndex[SETTIME2_HTM]){
										if(ucValue == 12){
											ch_num = current.main_chamber;
											strcpy(nucshrink, chamber[ch_num].nucdata.name);
											trim_and_shrink(nucshrink);
											if(nucshrink[0]!=0 && (!measurement[ch_num].over_flag)){
												m_ucSetTime = ucValue;
												m_ucSecMode = 1;
												//m_ucTimeMode = 0;
												SetAmuletHTML(AmuletHTMLIndex[SETTIME2_HTM]);
												PushPageStack(AmuletHTMLIndex[SETTIME2_HTM]);
												beep_amulet();
											}else{
												send_amulet_message(L_DOSE_DECAY, 6);    // "Dose Decay"
												send_to_amulet_string(7, "");
												measurement[ch_num].future.dosetime = NO_TIME;
												SetAmuletByte(101, 0xFF);
											}
										}else{
											m_ucSetTime = ucValue;
											if(ucValue == 4 || ucValue == 5) m_ucSecMode = 1;
											else m_ucSecMode = 0;
											//m_ucTimeMode = 0;
											SetAmuletHTML(AmuletHTMLIndex[SETTIME2_HTM]);
											PushPageStack(AmuletHTMLIndex[SETTIME2_HTM]);
											beep_amulet();
										}
									}
									break;
							}
						}
						break;

					case 0x15:
						if(CurrentPageStack() != AmuletHTMLIndex[MEASUREACTIVITY_HTM]){
							m_ucSetMeasure = ucValue;
							SetAmuletHTML(AmuletHTMLIndex[MEASUREACTIVITY_HTM]);
							PushPageStack(AmuletHTMLIndex[MEASUREACTIVITY_HTM]);
							beep_amulet();
						}
						break;

					//case 0x16:
					//	m_ucMeasureNuclide = ucValue;
					//	break;

					case 0x17:
						m_ucActivityConfig = ucValue;
						break;

					case 0x18:
						m_ucCheckboxState = ucValue;
						beep_amulet();
						break;

					case 0x19:
						beep_amulet();
						if(current.num_chambers>0){
							ch_num = current.main_chamber;
							if(chamber_type(ch_num) == R_CHAMB || chamber_type(ch_num) == P_CHAMB || chamber_type(ch_num) == C_CHAMB || chamber_type(ch_num) == K_CHAMB){
								if(NuclideData_getEffectiveResponse(m_ucHotKeyNuclideID[chamber_type(ch_num)][ucValue - 1], chamber_type(ch_num)) != 0.0){
									chamber[ch_num].nuc_index = m_ucHotKeyNuclideID[chamber_type(ch_num)][ucValue - 1];
									set_nuclide_data(chamber[ch_num].nuc_index, ch_num);
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
									EE_WRITE(nuclideID[ch_num], (uchar *) &(chamber[ch_num].nuc_index));
								}
							}
							/*if(m_ucLastHotKeyIndex == ucValue){
								if((g_msec_tstamp - m_ulLastHotKeyStamp) < 500) m_ucConsecHotKeyCount++;
								else m_ucConsecHotKeyCount = 1;
							}else{
								m_ucConsecHotKeyCount = 1;
							}

							if(m_ucConsecHotKeyCount >= 3){
								if(CurrentPageStack() != NUCLIDE2_HTM){
									PushPageStack(NUCLIDE2_HTM);
									m_ucNuclideConfig = 2;
									m_ucSetNuclide = 4;
									SetAmuletHTML(NUCLIDE2_HTM);
								}
								m_ucConsecHotKeyCount = 0;
							}

							m_ulLastHotKeyStamp = g_msec_tstamp;
							m_ucLastHotKeyIndex = ucValue; */
						}
						break;

					case 0x1A:
						m_ucNuclideConfig = ucValue;
						break;

					case 0x1B:
						SetVolume(ucValue);
						break;

					case 0x1C:
						keyboardCase = ucValue;
						break;

					case 0x1D:
						EepromRoutines_brightnessCurrent = ucValue;
						BrightnessMirrorUpdate();
						uiValue = EepromRoutines_brightnessCurrent;
						if(uiValue<10) uiValue = 10;
						uiValue *= 10;
						if(uiValue>999) uiValue = 999;
						if(INVERSION) uiValue = 1000 - uiValue;
						if (uiValue<10) uiValue = 10;
						SetAmuletWord(100, uiValue);
						uiValue2 = 900 - uiValue;
						double_value = uiValue2;
						double_value *= 235.0;
						double_value /= 890.0;
						double_value += 20.0;
						uiValue2 = double_value;
						if(uiValue2 > 255) uiValue2 = 255;
						if(uiValue2 < 20) uiValue2 = 20;
						ucValue2 = uiValue2;
						SetAmuletByte(55, ucValue2);
						SetAmuletByte(59, 0xFF);
						sprintf(message, "%u", EepromRoutines_brightnessCurrent);
						SetAmuletString(112, message);
						SetAmuletByte(112, 0xFF);
						// Min Brightness: Original Amulet(900) New Amulet(20), Max Brightness: Orignal Amulet(10) New Amulet(255)
						// new = (((900-original)/890)*235) + 20
						break;

					case 0x1E:
						if(CurrentPageStack() != AmuletHTMLIndex[WELLMEASUREMENT_HTM]){
							AmuletWellMeasurementMenu_WipeBackgroundThreshold = FALSE;
							m_ucSetWellMeasure = ucValue;
							m_ucClear = 18;
							SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
							PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
							beep_amulet();
						}
						break;

					case 0x1F:
						if(ucValue==1) Mca_setTimeType(COUNT_REAL);
						else if(ucValue==2) Mca_setTimeType(COUNT_LIVE);
						break;

					case 0x20:
						m_ucNuclideClearButton = ucValue;
						break;

					case 0x21:
						EepromRoutines_sleepTimeoutCurrent = ucValue;
						if(EepromRoutines_sleepTimeoutCurrent == 0)
						{	
							//strcpy(message, "OFF");
							get_amulet_message(L_TIMEOUT_OFF,message);    // "OFF"
						}	
						else sprintf(message, "%u", EepromRoutines_sleepTimeoutCurrent);
						SetAmuletString(110, message);
						SetAmuletByte(110, 0xFF);
						SleepTimeoutUpdate();
						break;

					case 0x22:
						EepromRoutines_sleepBrightnessCurrent = ucValue;
						sprintf(message, "%u", EepromRoutines_sleepBrightnessCurrent);
						SetAmuletString(111, message);
						SetAmuletByte(111, 0xFF);
						SleepBrightnessUpdate();
						break;

					case 0x23:
						AmuletGenericItems_config = ucValue;
						break;

					case 0x24:
						AmuletInactivate_config = ucValue;
						break;

					case 0x25:
						AmuletTestIdentMenu_config = ucValue;
						break;

					case 0x26:
						AmuletGenericYesNo_config = ucValue;
						break;

					case 0x27:
						AmuletHalflifeCalc_printSummary = ucValue;
						SetAmuletByte(105, AmuletHalflifeCalc_printSummary);
						beep_amulet();
						break;

					case 0x28:	// Set Second One
						m_ucSecond = ucValue;
						break;

					case 0x29:	// Set Second Ten
						m_ucSecond = m_ucSecond + (10 * ucValue);
						break;

					case 0xFF:
						CallHTML(ucValue);
						break;
				}
				break;

			// Set Word
			case 0xD6:
				SleepRefreshTimeout();
				ucVariable = ConvertByte(&(acCommand[1]));
				uiValue = ConvertWord(&(acCommand[3]));
				acCommand[0] = 0xE6;
				SendUart2Tx(acCommand);

				switch (ucVariable) {
					case 0x00:
						m_iCurPhase = uiValue;
						m_iCurPhase = m_iCurPhase >> 8;
						m_iNextPhase = uiValue & 0xFF;
						break;

					case 0x01:
						m_uiYear = uiValue;
						break;

					case 0x02:
						//m_uiOffsetX = uiValue;
						break;

					case 0x03:
						//m_uiOffsetY = uiValue;
						break;

					case 0x04:
						//Mca_setAcquireTime(uiValue);
						break;

					case 0x05:
						if(CurrentPageStack() != AmuletHTMLIndex[KEYPAD2_HTM]){
							m_uiSetKeyPad = uiValue;
							SetAmuletHTML(AmuletHTMLIndex[KEYPAD2_HTM]);
							PushPageStack(AmuletHTMLIndex[KEYPAD2_HTM]);
							beep_amulet();
						}
						break;

					case 0x06:
						m_usBackClear = uiValue;
						break;

					case 0xFE:
						amuletIndexPageNum = uiValue;
						if(amuletIndexPageNum == 0x20) LoadAmuletHTML(0);
						else if(amuletIndexPageNum == 0x21) LoadAmuletHTML(1);
						break;

					case 0xFF:
						amuletBuild = uiValue;
						break;
				}
				break;

			// Set String
			case 0xD7:
				ucVariable = ConvertByte(&(acCommand[1]));
				strcpy(acMessage, &(acCommand[3]));
				acCommand[0] = 0xE7;
				SendUart2Tx(acCommand);
				undo_string_coding(acTempMessage, acMessage);
				strcpy(acMessage, acTempMessage);		
				switch (ucVariable) {
					case 0x01:
						strcpy((char *)m_acTitle, acMessage);
						//undo_string_coding((char *)m_acTitle, acMessage);
						break;

					case 0x02:
						m_fMinValue = atof(acMessage);
						break;

					case 0x03:
						m_fMaxValue = atof(acMessage);
						break;
				}
				break;

			default:
				break;
		}
	}

	if (QueryPendingSlaveMessage() != 0) {
		flgByte = flgWord = flgString = 0;

		PopSlaveMessage(acMessage);
		strcpy(acCommand, acMessage);

		ucCommandByte = (unsigned char) acCommand[0];

		switch (ucCommandByte) {
			case 0xE0:
				acCommand[0] = 0xD0;
				acCommand[3] = 0;
				ucByte = ConvertByte(&(acMessage[3]));
				flgByte = -1;
				break;

			case 0xE1:
				acCommand[0] = 0xD1;
				acCommand[3] = 0;
				uiWord = ConvertWord(&(acMessage[3]));
				flgWord = -1;
				break;

			case 0xE2:
				acCommand[0] = 0xD2;
				acCommand[3] = 0;
				strcpy (acString, &(acMessage[3]));
				flgString = -1;
				break;

			case 0xE4:
				acCommand[0] = 0xD4;
				acCommand[3] = 0;
				strcpy (acString, &(acMessage[3]));
				flgString = -1;
				break;

			case 0xE5:
				acCommand[0] = 0xD5;
				acCommand[5] = 0;
				ucByte = 0xFF;
				flgByte = -1;
				break;

			case 0xE6:
				acCommand[0] = 0xD6;
				acCommand[7] = 0;
				ucByte = 0xFF;
				flgByte = -1;
				break;

			case 0xE7:
				acCommand[0] = 0xD7;
				ucByte = 0xFF;
				flgByte = -1;
				break;

			case 0xE9:
				acCommand[0] = 0xD9;
				ucByte = 0xFF;
				flgByte = -1;
				break;

			case 0xEA:
				acCommand[0] = 0xDA;
				ucByte = 0xFF;
				flgByte = -1;
				break;

			case 0xEB:
				acCommand[0] = 0xDB;
				ucByte = 0xFF;
				flgByte = -1;
				break;

			case 0xEC:
				acCommand[0] = 0xDC;
				ucByte = 0xFF;
				flgByte = -1;
				break;

			case 0xEF:
				acCommand[0] = 0xDF;
				ucByte = ConvertByte(&(acMessage[3]));
				flgByte = -1;
				break;

			case 0xF3:
				acCommand[0] = 0xF2;
				ucByte = ConvertByte(&(acMessage[3]));
				flgByte = -1;
				break;

			default:
				printf ("Unable to find matching Command Byte: %xh\n", ucCommandByte);
				break;

		}

		if (ScanCmdQueueCommand(acCommand, &CmdSent)) {
			if (!(CmdSent.ptrOutput == NULL)) {
				if (flgByte) {
					pucByte = (unsigned char *) CmdSent.ptrOutput;
					*pucByte = ucByte;
				}
				else if (flgWord) {
					puiWord = (short unsigned int *) CmdSent.ptrOutput;
					*puiWord = uiWord;
				}
				else if (flgString) {
					pcString = (char *) CmdSent.ptrOutput;
					strcpy (pcString, acString);
				}
			}

			if (!(CmdSent.ptrFunction == NULL)) {
				if (flgByte) {
					(*(CmdSent.ptrFunction))((void *)&ucByte);
				}
				else if (flgWord) {
					(*(CmdSent.ptrFunction))((void *)&uiWord);
				}
				else if (flgString) {
					(*(CmdSent.ptrFunction))((void *)acString);
				}
			}
		}
		else {
			printf ("Unable to find command: %s\n", acCommand);
		}
	}

	// Process Resend
	while (ScanCmdQueueResend (&CmdSent)) {
		SendUart2Command (CmdSent.cCommand, CmdSent.ptrOutput, CmdSent.ptrFunction, 0xFFFFFFFF, 100);
		printf ("Command Resent: %s\n", CmdSent.cCommand);
	}

	// Process Expired
	while (ScanCmdQueueExpired (&CmdSent)) {
		//printf ("Expired: %s\n", CmdSent.cCommand);
	}

	amulet_menu();
}
/**
 * \details This function is executed at the end of the service_amulet function.
 * \details It use m_iMenu variable to determine the current Amulet Touchscreen Handler (Amulet*_menu).
 * \returns None
 */
void amulet_menu(void) {
	if(EepromRoutines_sleepNow()) SetAmuletHTML(AmuletHTMLIndex[SLEEP_HTM]);

	if (m_iMenu != MENU_NONE) {
		if (m_iPhase == m_iCurPhase) {
			m_iPhase = m_iNextPhase;
			m_iCurPhase = PHASE_GENERIC_CUR_NOP;
			m_iNextPhase = PHASE_GENERIC_CUR_NOP;
		}

		switch (m_iMenu) {
			case MENU_DAILY:
				AmuletDaily_menu();
				break;

			case MENU_BKG:
				AmuletBackground_menu();
				break;

			case MENU_CHAMBERVOLTS:
				AmuletChamberVolts_menu();
				break;

			case MENU_ACCURACY:
				AmuletAccuracy_menu();
				break;

			case MENU_GEOMETRY:
				AmuletGeometry_menu();
				break;

			case MENU_LINEARITY:
				AmuletLinearity_menu();
				break;

			case MENU_ONESTRIP:
				AmuletOneStrip_menu();
				break;

			case MENU_TWOSTRIP:
				AmuletTwoStrip_menu();
				break;

			case MENU_HMPAO:
				AmuletHMPAO_menu();
				break;

			case MENU_MAG3:
				AmuletMAG3_menu();
				break;

			case MENU_MOLY:
				AmuletMoly_menu();
				break;

			case MENU_INVENTORY:
				AmuletInventory_menu();
				break;

			case MENU_NUCLIDE:
				//nuclide_menu();
				AmuletNuclide_menu();
				break;

			case MENU_TIME:
				AmuletTime_menu();
				break;

			case MENU_ADD_INVENTORY:
				AmuletAddInventory_menu();
				break;

			case MENU_KEYPAD:
				AmuletKeypad_menu();
				break;

			case MENU_KEYBOARD:
				AmuletKeyboard_menu();
				break;

			case MENU_STUDY:
				AmuletStudy_menu();
				break;

			case MENU_ACTIVITY:
				AmuletActivity_menu();
				break;

			case MENU_MEASURE:
				AmuletMeasure_menu();
				break;

			case MENU_WITHDRAW_INVENTORY:
				AmuletWithdrawInv_menu();
				break;

			case MENU_DELETE_INVENTORY:
				AmuletDeleteInv_menu();
				break;

			case MENU_KIT_INVENTORY:
				AmuletKitInv_menu();
				break;

			case MENU_STUDY2:
				AmuletStudy2_menu();
				break;

			case MENU_INFO:
				AmuletInfo_menu();
				break;

			case MENU_SETUP:
				AmuletSetup_menu();
				break;

			case MENU_SETUP_SOURCES:
				AmuletSetupSources_menu();
				break;

			case MENU_SETUP_MOLY:
				AmuletSetupMoly_menu();
				break;

			case MENU_SETUP_NUCLIDE:
				AmuletSetupNuclide_menu();
				break;

			case MENU_SETUP_LINEARITY:
				AmuletSetupLinearity_menu();
				break;

			case MENU_SETUP_REMOTE:
				AmuletSetupRemote_menu();
				break;

			case MENU_MAIN_SCREEN:
				AmuletMainScreen_menu();
				break;

			case MENU_SETUP_CALNUM:
				AmuletSetupCalNum_menu();
				break;

			case MENU_ERROR_MSG:
				AmuletErrorMsg_menu();
				break;

			case MENU_WARNING_MSG:
				AmuletWarningMsg_menu();
				break;

			case MENU_NOTIFICATION_MSG:
				AmuletNotificationMsg_menu();
				break;

			case MENU_SETUP_LINEARITY_STANDARD:
				AmuletSetupLinearityStandard_menu();
				break;

			case MENU_SETUP_LINEARITY_LINEATOR:
				AmuletSetupLinearityLineator_menu();
				break;

			case MENU_SETUP_LINEARITY_CALICHECK:
				AmuletSetupLinearityCalicheck_menu();
				break;

			case MENU_DOSE_TABLE:
				AmuletDoseTable_menu();
				break;

			case MENU_SELECT_CHAMBER:
				AmuletSelectChamber_menu();
				break;

			case MENU_INITIAL:
				AmuletInitial_menu();
				break;

			case MENU_OTHER:
				AmuletOther_menu();
				break;

			case MENU_WELL_MAIN_SCREEN:
				AmuletWellMainScreen_menu();
				break;

			case MENU_WELL_MEASUREMENT:
				AmuletWellMeasurement_menu();
				break;

			case MENU_WELL_AUTOCALIBRATE:
				AmuletWellAutoCalibrate_menu();
				break;

			case MENU_WELL_PEAKS:
				AmuletWellPeaks_menu();
				break;

			case MENU_WELL_ADVANCED_SETUP:
				AmuletWellAdvancedSetup_menu();
				break;

			case MENU_WELL_SETUP_NUCLIDE:
				AmuletWellSetupNuclide_menu();
				break;

			case MENU_WELL_SETUP_SEALED:
				AmuletWellSetupSealed_menu();
				break;

			case MENU_WELL_SETUP_SEALED2:
				AmuletWellSetupSealed2_menu();
				break;

			case MENU_WELL_SETUP_TEST_SOURCE:
				AmuletWellSetupTestNuclide_menu();
				break;

			case MENU_WELL_SETUP_TRIGGER_LEVEL:
				AmuletWellSetupTriggerLevel_menu();
				break;

			case MENU_WELL_SETUP_EFFICIENCIES:
				AmuletWellSetupEfficiencies_menu();
				break;

			case MENU_WELL_EDIT_EFFICIENCIES:
				AmuletWellEditEfficiencies_menu();
				break;

			case MENU_WELL_MANUAL:
				AmuletWellManual_menu();
				break;

			case MENU_WELL_MEASURE_EFFICIENCY:
				AmuletWellMeasureEfficiency_menu();
				break;

			case MENU_GEOMETRY_REPORT:
				AmuletGeometryReport_menu();
				break;

			case MENU_AUTOCONSTANCY:
				AmuletAutoConstancy_menu();
				break;

			case MENU_SLEEP:
				AmuletSleep_menu();
				break;

			case MENU_ENHANCED:
				AmuletEnhanced_menu();
				break;

			case MENU_FACTORY:
				AmuletFactory_menu();
				break;

			case MENU_SETUPCHAMBER:
				AmuletSetupChamber_menu();
				break;

			case MENU_DELETE_ALL_INVENTORY:
				AmuletDeleteAllInv_menu();
				break;

			case MENU_WELL_MDA_TEST:
				AmuletWellMDATest_menu();
				break;

			case MENU_WELL_MEASUREMENTS:
				AmuletWellMeasurements_menu();
				break;

			case MENU_WELL_EDITFULLEFFICIENCY:
				AmuletWellEditFullEfficiency_menu();
				break;

			case MENU_SETUPCALIBSERIAL:
				AmuletSetupCalibSerial_menu();
				break;

			case MENU_WELL_WIPEREPORT:
				AmuletWellWipeReport_menu();
				break;

			case MENU_WELL_SCHILLING:
				AmuletWellSchilling_menu();
				break;

			case MENU_WELL_PLASMA:
				AmuletWellPlasma_menu();
				break;

			case MENU_WELL_RBC:
				AmuletWellRBC_menu();
				break;

			case MENU_WELL_FACTORY:
				AmuletWellFactory_menu();
				break;

			case MENU_WELL_PLASMAANALYSIS:
				AmuletWellPlasmaAnalysis_menu();
				break;

			case MENU_WELL_RBCANALYSIS:
				AmuletWellRBCAnalysis_menu();
				break;

			case MENU_WELL_QATESTS:
				AmuletWellQATests_menu();
				break;

			case MENU_WELL_SYSTESTANALYSIS:
				AmuletWellSystemTestAnalysis_menu();
				break;

			case MENU_WELL_CHITEST:
				AmuletWellChiTest_menu();
				break;

			case MENU_WELL_SCHILLINGANALYSIS:
				AmuletWellSchillingAnalysis_menu();
				break;

			case MENU_WELL_CHANGE_ROI:
				AmuletWellChangeROI_menu();
				break;

			case MENU_WELL_GENERALANALYSIS:
				AmuletWellGeneralAnalysis_menu();
				break;

			case MENU_SETUP_RHOTKEYS:
				AmuletSetupRHotkeys_menu();
				break;

			case MENU_SETUP_PHOTKEYS:
				AmuletSetupPHotkeys_menu();
				break;

			case MENU_REMOTE:
				AmuletRemote_menu();
				break;

			case MENU_DELETE_LINEATOR:
				AmuletDeleteLineator_menu();
				break;

			case MENU_DELETE_CALICHECK:
				AmuletDeleteCalicheck_menu();
				break;

			case MENU_SELECT_LINEARITY:
				AmuletSelectLinearity_menu();
				break;

			case MENU_QC:
				AmuletQC_menu();
				break;

			case MENU_LOW_LEVEL:
				AmuletLowLevel_menu();
				break;

			case MENU_WELL_STABILITYTEST:
				AmuletWellStabilityTest_menu();
				break;

			case MENU_SETUP_COMMUNICATIONS:
				AmuletSetupCommunications_menu();
				break;

			case MENU_PCQC:
				AmuletPCQC_menu();
				break;
				
			case MENU_ETHERNET:
				AmuletEthernet_menu();
				break;

			case MENU_SETUP_TYPES:
				AmuletWellSetupTypes_menu();
				break;

			case MENU_WELL_SETUP_LOCATIONS:
				AmuletWellSetupLocations_menu();
				break;

			case MENU_WELL_SETUP_ADDEDITLOCATION:
				AmuletWellSetupAddEditLocation_menu();
				break;

			case MENU_WELL_SETUP_DELETELOCATION:
				AmuletWellSetupDeleteLocation_menu();
				break;

			case MENU_WELL_WIPE_LIST:
				AmuletWellWipeList_menu();
				break;

			case MENU_ERROR_MSG_LARGE:
				AmuletErrorMsgLarge_menu();
				break;

			case MENU_WELL_SPECTRUM:
				AmuletWellSpectrum_menu();
				break;

			case MENU_WELL_WIPE_SEARCH:
				AmuletWellWipeSearch_menu();
				break;

			case MENU_GENERIC_ITEMS:
				AmuletGenericItems_menu();
				break;

			case MENU_INACTIVATE:
				AmuletInactivate_menu();
				break;

			case MENU_WELL_SYSTEM_TEST_SEARCH:
				AmuletWellSystemTestSearch_menu();
				break;

			case MENU_WELL_MDA_SEARCH:
				AmuletWellMDASearch_menu();
				break;

			case MENU_WELL_MDA_ANALYSIS:
				AmuletWellMDAAnalysis_menu();
				break;

			case MENU_WELL_CHI_SEARCH:
				AmuletWellChiSearch_menu();
				break;

			case MENU_WELL_CHI_ANALYSIS:
				AmuletWellChiAnalysis_menu();
				break;

			case MENU_TEST_IDENT:
				AmuletTestIdent_menu();
				break;

			case MENU_WELL_SCHILLING_SEARCH:
				AmuletWellSchillingSearch_menu();
				break;

			case MENU_WELL_SCHILLING_REPORT:
				AmuletWellSchillingReport_menu();
				break;

			case MENU_WELL_PLASMA_SEARCH:
				AmuletWellPlasmaSearch_menu();
				break;

			case MENU_WELL_PLASMA_REPORT:
				AmuletWellPlasmaReport_menu();
				break;

			case MENU_WELL_RBC_SEARCH:
				AmuletWellRBCSearch_menu();
				break;

			case MENU_WELL_RBC_REPORT:
				AmuletWellRBCReport_menu();
				break;

			case MENU_WELL_AUTOCAL_SEARCH:
				AmuletWellAutoCalSearch_menu();
				break;

			case MENU_WELL_AUTOCAL_REPORT:
				AmuletWellAutoCalReport_menu();
				break;

			case MENU_WELL_FACTORY_DETECTORS:
				AmuletWellFactoryDetectors_menu();
				break;

			case MENU_WELL_SETUP_USER_NUCLIDES:
				AmuletWellSetupUserNuclides_menu();
				break;

			case MENU_WELL_ADD_EDIT_USER_NUCLIDES:
				AmuletWellAddEditUserNuclides_menu();
				break;

			case MENU_WELL_LAB_TESTS:
				AmuletWellLabTests_menu();
				break;

			case MENU_WELL_REPORTS:
				AmuletWellReports_menu();
				break;

			case MENU_LOGIN:
				AmuletLogin_menu();
				break;

			case MENU_SECURITY:
				AmuletSecurity_menu();
				break;

			case MENU_ADD_EDIT_USER:
				AmuletAddEditUser_menu();
				break;

			case MENU_WELL_SETUP_BIOASSAY:
				AmuletWellSetupBioAssay_menu();
				break;

			case MENU_WELL_SETUP_BIOASSAY_ENTER_EFF:
				AmuletWellSetupBioAssayEnterEfficiency_menu();
				break;

			case MENU_WELL_SETUP_BIOASSAY_MEASURE_EFF:
				AmuletWellSetupBioAssayMeasureEfficiency_menu();
				break;

			case MENU_WELL_BIOASSAY:
				AmuletWellBioAssay_menu();
				break;

			case MENU_WELL_BIOASSAYANALYSIS:
				AmuletWellBioAssayAnalysis_menu();
				break;

			case MENU_WELL_BIOASSAY_SEARCH:
				AmuletWellBioAssaySearch_menu();
				break;

			case MENU_WELL_SETUP_BIOASSAY_ENTER_ROI:
				AmuletWellSetupBioAssayEnterROI_menu();
				break;

			case MENU_WELL_SETUP_THYROID_UPTAKE_PROTOCOL:
				AmuletWellSetupThyroidUptakeProtocol_menu();
				break;

			case MENU_ADD_EDIT_THYROID_UPTAKE_PROTOCOL:
				AmuletAddEditThyroidUptakeProtocol_menu();
				break;

			case MENU_WELL_THYROID_UPTAKE:
				AmuletWellThyroidUptake_menu();
				break;

			case MENU_ADD_EDIT_THYROID_UPTAKE_TEST:
				AmuletAddEditThyroidUptakeTest_menu();
				break;

			case MENU_WELL_THYROID_UPTAKE_TEST:
				AmuletWellThyroidUptakeTest_menu();
				break;

			case MENU_WELL_THYROID_UPTAKE_ENTER_ADMIN:
				AmuletWellThyroidUptakeEnterAdministrationDate_menu();
				break;

			case MENU_WELL_THYROID_UPTAKE_MEAS_DOSE:
				AmuletWellThyroidUptakeMeasureDose_menu();
				break;

			case MENU_WELL_THYROID_UPTAKE_MEAS_PAT:
				AmuletWellThyroidUptakeMeasurePatient_menu();
				break;

			case MENU_WELL_THYROID_UPTAKE_ENTER_NORMAL:
				AmuletWellThyroidUptakeEnterNormal_menu();
				break;

			case MENU_WELL_RBC_SURVIVAL:
				AmuletWellRBCSurvival_menu();
				break;

			case MENU_ADD_EDIT_RBC_SURVIVAL_TEST:
				AmuletAddEditRBCSurvivalTest_menu();
				break;

			case MENU_WELL_RBC_SURVIVAL_TEST:
				AmuletWellRBCSurvivalTest_menu();
				break;

			case MENU_WELL_RBC_SURVIVAL_MEASUREMENT:
				AmuletWellRBCSurvivalMeasurement_menu();
				break;

			case MENU_WELL_RBC_SURVIVAL_ENTER_NORMAL:
				AmuletWellRBCSurvivalEnterNormal_menu();
				break;

			case MENU_AUTOLINEARITY:
				AmuletAutoLinearity_menu();
				break;

			case MENU_AUTOLINEARITY_TEST:
				AmuletAutoLinearityTest_menu();
				break;

			case MENU_GENERIC_YES_NO:
				AmuletGenericYesNo_menu();
				break;

			case MENU_AUTOLINEARITY_SEARCH:
				AmuletAutoLinearitySearch_menu();
				break;

			case MENU_CHAMBER_REPORTS:
				AmuletChamberReports_menu();
				break;

			case MENU_CHAMBER_SEARCH:
				AmuletChamberSearch_menu();
				break;

			case MENU_CHAMBER_DAILY_TEST_VIEW:
				AmuletChamberDailyTestView_menu();
				break;

			case MENU_CHAMBER_ZERO_TEST_VIEW:
				AmuletChamberZeroTestView_menu();
				break;

			case MENU_CHAMBER_BACKGROUND_TEST_VIEW:
				AmuletChamberBackgroundTestView_menu();
				break;

			case MENU_CHAMBER_VOLTAGE_TEST_VIEW:
				AmuletChamberVoltageTestView_menu();
				break;

			case MENU_CHAMBER_ACCURACY_TEST_VIEW:
				AmuletChamberAccuracyTestView_menu();
				break;

			case MENU_CHAMBER_AUTOCONSTANCY_TEST_VIEW:
				AmuletChamberAutoconstancyTestView_menu();
				break;

			case MENU_CHAMBER_HALFLIFE_CALC:
				AmuletChamberHalflifeCalc_menu();
				break;

			case MENU_SETUP_KEY:
				AmuletSetupKey_menu();
				break;

			case MENU_SETUP_PASSWORD:
				AmuletSetupPassword_menu();
				break;

			default:
				break;
		}
	}
}
/**
 * \details Test a string to verify a valid number as defined by m_ucLenFractionLimit and m_ucLenLimit
 * \param numstr1 Pointer to Null terminated string
 * \returns True = Valid Number, False = Exceeds m_ucLenFractionLimit and/or m_ucLenLimit
 */
bool testnumber(char *numstr1){
	bool returnValue, foundPeriod;
	short index, periodIndex;
	char test[20], fraction[20];
	char numstr[100];

	if(numstr1[0] == '-'){
		strcpy(numstr, &(numstr1[1]));
	}else{
		strcpy(numstr, numstr1);
	}

	if(strlen(numstr) > 0){
		strcpy(test, numstr);

		foundPeriod = FALSE;
		for(index=0; index<strlen(test); index++){
			if(test[index] == '.'){
				foundPeriod = TRUE;
				periodIndex = index;
				break;
			}
		}

		if(foundPeriod){
			strcpy(fraction, &(test[periodIndex + 1]));
			if(strlen(fraction) >= m_ucLenFractionLimit) returnValue = FALSE;
			else returnValue = TRUE;
		}else{
			if(strlen(test) >= m_ucLenLimit) returnValue = FALSE;
			else returnValue = TRUE;
		}

	}else{
		returnValue = TRUE;
	}

	return returnValue;
}
/**
 * \details Test the string to determine if the time/date string is full
 * \param datestr Pointer to Null terminated string
 * \param autoslash True = automatically append slash, if it not not possible to append addition digit, False = Do not append slash
 * \returns True = Is NOT full, False = Is Full
 */
bool testdate(char *datestr, bool autoslash){
	bool returnValue;
	char *ptr, firstTwo[3];
	int  i, stringLength, slashCount, chrsAfterSlash;

	returnValue = TRUE;
	stringLength = strlen(datestr);

	slashCount = 0;
	chrsAfterSlash = -1;
	if(stringLength > 0){
		ptr = datestr;
		for(i=0; i<stringLength; i++){
			if(*ptr == '/'){
				chrsAfterSlash = 0;
				slashCount++;
			}else{
				if(chrsAfterSlash >= 0) chrsAfterSlash++;
			}
			ptr++;
		}
	}

	if(autoslash){
		if(slashCount == 0){
			if(stringLength == 5){
				// Test if first two digits are larger the 23
				firstTwo[0] = *datestr;
				firstTwo[1] = *(datestr + 1);
				firstTwo[2] = 0;
				if(atoi(firstTwo) > 23) strcat(datestr, "/");
			}else if(stringLength == 6){
				strcat(datestr, "/");
			}
		}else if(slashCount == 1){
			if(chrsAfterSlash == 1){
				firstTwo[0] = datestr[strlen(datestr) - 1];
				firstTwo[1] = 0;
				if(atoi(firstTwo) > 3) strcat(datestr, "/");
			}else if(chrsAfterSlash == 2) strcat(datestr, "/");
		}else if(slashCount == 2){
			if(chrsAfterSlash == 1){
				firstTwo[0] = datestr[strlen(datestr) - 1];
				firstTwo[1] = 0;
				if(atoi(firstTwo) > 1) strcat(datestr, "/");
			}else if(chrsAfterSlash == 2) strcat(datestr, "/");
		}else if(slashCount == 3){
			if(chrsAfterSlash >= 2) returnValue = FALSE;
		}
	}else{
		if(slashCount >= 3) returnValue = FALSE;
	}

	if(strlen(datestr) >= 15) returnValue = FALSE;

	return returnValue;
}

/* static uchar numcount(char *numstr){
	uchar count;

	count = 0;

	if(*numstr != 0){
		while(*numstr != 0){
			if(*numstr == '0' ||
			   *numstr == '1' ||
			   *numstr == '2' ||
			   *numstr == '3' ||
			   *numstr == '4' ||
			   *numstr == '5' ||
			   *numstr == '6' ||
			   *numstr == '7' ||
			   *numstr == '8' ||
			   *numstr == '9' ||
			   *numstr == '/' ||
			   *numstr == '*'
			  ) count++;
			numstr++;
		}
	}

	return count;
}*/
/**
 * \details Test if significant digits in a number string are within the limit
 * \param str Pointer to Null terminated string
 * \param limit Number of significant digits
 * \returns True = significant digits are less then limit, False = Otherwise
 */
bool test_sig_digits(char *str, uchar limit){
	bool returnvalue;
	uchar sigdigit;

	sigdigit = 0;

	while(*str!=0){
		if(*str!='.'){
			sigdigit++;
		}
		str++;
	}

	if(sigdigit<limit) returnvalue = TRUE;
	else returnvalue = FALSE;

	return returnvalue;
}

// Routines to convert binary to hex
/**
 * \details Converts Nibble value to Hex character (0-9,A-F)
 * \param ucValue Nibble value, only lowest 4 bits are evaluated
 * \returns Hex character
 */
char ntoh(unsigned char ucValue) {
	char cReturn;

	cReturn = ucValue & 0x0F;

	if (cReturn < 10) cReturn =  cReturn + 48;
	else cReturn = cReturn + 55;

	return cReturn;
}

/**
 * \details Converts Byte value to Hex Value (00 - FF)
 * \param ucValue Byte value
 * \param pcOutput Pointer to array of at least two bytes. The output is written to this array. This is not Null terminated.
 * \returns None
 */
void ctoh(unsigned char ucValue, char *pcOutput) {
	char *pcTemp;

	pcTemp = pcOutput + 2;
	*pcTemp = 0;

	pcTemp = pcOutput + 1;
	*pcTemp = ntoh(ucValue);

	ucValue = ucValue >> 4;

	*pcOutput = ntoh(ucValue);
}

/**
 * \details Convert Word (16 bits) value to Hex Value (0000 - FFFF)
 * \param uiValue Word value
 * \param pcOutput Pointer to array of at least 4 bytes. The output is written to this array, This is not Null terminated.
 * \returns None
 */
void itoh(unsigned short int uiValue, char *pcOutput) {
	char pcLowByte[3];
	unsigned char ucByte;

	ucByte = uiValue & 0xFF;
	ctoh(ucByte, pcLowByte);

	ucByte = ((uiValue >> 8) & 0xFF);
	ctoh(ucByte, pcOutput);
	strcat(pcOutput, pcLowByte);
}
/**
 * \details Converts a single Hex Character (0 - F) into Byte Value
 * \param cInput Hex Character
 * \returns Byte Value
 */
unsigned char ConvertNibble(char cInput) {
	unsigned char ucOutput;

	switch (cInput) {
		case '0':
			ucOutput = 0;
			break;

		case '1':
			ucOutput = 1;
			break
			;
		case '2':
			ucOutput = 2;
			break;

		case '3':
			ucOutput = 3;
			break;

		case '4':
			ucOutput = 4;
			break;

		case '5':
			ucOutput = 5;
			break;

		case '6':
			ucOutput = 6;
			break;

		case '7':
			ucOutput = 7;
			break;

		case '8':
			ucOutput = 8;
			break;

		case '9':
			ucOutput = 9;
			break;

		case 'A':
		case 'a':
			ucOutput = 10;
			break;

		case 'B':
		case 'b':
			ucOutput = 11;
			break;

		case 'C':
		case 'c':
			ucOutput = 12;
			break;

		case 'D':
		case 'd':
			ucOutput = 13;
			break;

		case 'E':
		case 'e':
			ucOutput = 14;
			break;

		case 'F':
		case 'f':
			ucOutput = 15;
			break;
	}

	return ucOutput;
}

/**
 * \details Converts two Hex Characters (00 - FF) into Byte Value
 * \param pcInput Pointer to two Hex Characters. This is not a null terminated string
 * \returns Byte Value
 */
unsigned char ConvertByte(char *pcInput) {
	char cInput;
	unsigned char ucHighNibble;
	unsigned char ucLowNibble;
	unsigned char ucOutput;

	cInput = *pcInput;
	ucHighNibble = ConvertNibble(cInput);
	ucHighNibble = ucHighNibble << 4;

	pcInput++;
	cInput = *pcInput;
	ucLowNibble = ConvertNibble(cInput);

	ucOutput = ucHighNibble + ucLowNibble;

	return ucOutput;
}

/**
 * \details Converts four Hex Characters (0000 - FFFF) into Word (16 bits) Value
 * \param pcInput Pointer to four Hex Characters. This is not a null terminated string
 * \returns Word Value
 */
short unsigned int ConvertWord(char *pcInput) {
	short unsigned int uiOutput;
	short unsigned int uiHighByte;
	short unsigned int uiLowByte;

	uiHighByte = ConvertByte(pcInput);
	uiHighByte = uiHighByte << 8;
	pcInput = pcInput + 2;
	uiLowByte = ConvertByte(pcInput);
	uiOutput = uiHighByte + uiLowByte;

	return uiOutput;
}

/**
 * \details Test if byte is an Amulet Master Start Of Message character
 * \param ucValue Byte to test
 * \returns True = It is Master Start Of Message character, False = It is NOT Master Start Of Message character
 */
char IsMSOM(unsigned char ucValue) {
	if (((ucValue >= 0xD0) && (ucValue <= 0xD3)) || ((ucValue >= 0xD5) && (ucValue <=0xD8)) || (ucValue == 0xDD) || (ucValue == 0xDE) || (ucValue == 0xF2)) {
		return -1;
	}
	else {
		return 0;
	}
}

/**
 * \details Test is byte is an Amulet Slave Start Of Message character
 * \param ucValue Byte to test
 * \returns True = It is Slave Start Of Message characeter, False = It is NOT Slave Start of Message character
 *
 */
char IsSSOM(unsigned char ucValue) {
	if (((ucValue >= 0xE0) && (ucValue <= 0xE2)) || ((ucValue >= 0xE4) && (ucValue <= 0xE7)) || ((ucValue >= 0xE9) && (ucValue <= 0xEC)) || (ucValue >= 0xEF) || (ucValue == 0xF3)) {
		return -1;
	}
	else {
		return 0;
	}
}

/**
 * \details Clears the Amulet Master Message Queue by setting the Head and Tail to zero
 * \returns None
 */
void ClearMasterMessage(void) {
	uiMasterHead = uiMasterTail = 0;
}

/**
 * \details Returns the number of messages in the Amulet Master Message Queue
 * \returns Number of messages in the Amulet Message Queue
 */
short unsigned int QueryPendingMasterMessage(void) {
	short unsigned int uiSize;

	if (uiMasterHead >= uiMasterTail) {
		uiSize = uiMasterHead - uiMasterTail;
	}
	else {
		uiSize = MESSAGE_BUFFER_SIZE - uiMasterTail + uiMasterHead;
	}

	return uiSize;
}

/**
 * \details Adds a Master Message into the Amulet Master Message Queue
 * \returns 0 = Success, -1 = Failed because Queue is Full
 */
char PushMasterMessage(char *pcMessage) {
	char cError;
	short unsigned int uiNextHead;

	uiNextHead = uiMasterHead + 1;
	if (uiNextHead == MESSAGE_BUFFER_SIZE) uiNextHead = 0;

	if (uiMasterTail == uiNextHead) {
		cError = -1;
	}
	else {
		strcpy((char *) &(acMasterMessage[uiMasterHead][0]), pcMessage);
		uiMasterHead = uiNextHead;
		cError = 0;
	}

	return cError;
}

/**
 * \details Removes a Message from the Amulet Master Message Queue
 * \param pcOutput Pointer to allocated space for copying the removed Amulet Master Message
 * \returns 0 = Successful, -1 = Failed because queue is empty
 */
char PopMasterMessage(char *pcOutput) {
	char cError;

	if (uiMasterHead == uiMasterTail) {
		cError = -1;
	}
	else {
		strcpy(pcOutput, (const char *) &(acMasterMessage[uiMasterTail][0]));
		if ((uiMasterTail + 1) == MESSAGE_BUFFER_SIZE) uiMasterTail = 0;
		else uiMasterTail++;
		cError = 0;
	}

	return cError;
}

/**
 * \details Clears the Amulet Slave Message Queue by setting the Head and Tail to zero
 * \returns None
 */
void ClearSlaveMessage(void) {
	uiSlaveHead = uiSlaveTail = 0;
}

/**
 * \details Returns the number of messages in the Amulet Slave Message Queue
 * \returns Number of messages in the Amulet Message Queue
 */
short unsigned int QueryPendingSlaveMessage(void) {
	short unsigned int uiSize;

	if (uiSlaveHead >= uiSlaveTail) {
		uiSize = uiSlaveHead - uiSlaveTail;
	}
	else {
		uiSize = MESSAGE_BUFFER_SIZE - uiSlaveTail + uiSlaveHead;
	}

	return uiSize;
}

/**
 * \details Adds a Slave Message into the Amulet Slave Message Queue
 * \returns 0 = Success, -1 = Failed because Queue is Full
 */
char PushSlaveMessage(char *pcMessage) {
	char cError;
	short unsigned int uiNextHead;

	uiNextHead = uiSlaveHead + 1;
	if (uiNextHead == MESSAGE_BUFFER_SIZE) uiNextHead = 0;

	if (uiSlaveTail == uiNextHead) {
		cError = -1;
	}
	else {
		strcpy((char *) &(acSlaveMessage[uiSlaveHead][0]), pcMessage);
		uiSlaveHead = uiNextHead;
		cError = 0;
	}

	return cError;
}

/**
 * \details Removes a Message from the Amulet Slave Message Queue
 * \param pcOutput Pointer to allocated space for copying the removed Amulet Slave Message
 * \returns 0 = Successful, -1 = Failed because queue is empty
 */
char PopSlaveMessage(char *pcOutput) {
	char cError;

	if (uiSlaveHead == uiSlaveTail) {
		cError = -1;
	}
	else {
		strcpy(pcOutput, (const char *) &(acSlaveMessage[uiSlaveTail][0]));
		if ((uiSlaveTail + 1) == MESSAGE_BUFFER_SIZE) uiSlaveTail = 0;
		else uiSlaveTail++;
		cError = 0;
	}

	return cError;
}

/**
 * \details Clears the Amulet Command Queue by setting the Head and Tail to zero, and setting the command to -1 for the entire queue
 * \returns None
 */
void ClearCmdQueue(void) {
	int i;

	uiCmdQueueHead = uiCmdQueueTail = 0;
	for (i = 0; i < CMD_SENT_SIZE + 1; i++) iCmdQueue[i] = -1;
}

/**
 * \details Returns the number of commands in the Amulet Command Queue
 * \returns Number of commands in the Amulet Command Queue
 */
short unsigned int QueryPendingCmdQueue(void) {
	short unsigned int uiSize;

	if (uiCmdQueueHead >= uiCmdQueueTail) {
		uiSize = uiCmdQueueHead - uiCmdQueueTail;
	}
	else {
		uiSize = CMD_SENT_SIZE + 1 - uiCmdQueueTail + uiCmdQueueHead;
	}

	return uiSize;
}

/**
 * \details Test if a specific array index position is currently points to a command in the Amulet Command Queue
 * \param iPosition array index position
 * \returns 0 = Not pointing to a command, -1 = Pointing to a command
 */
char InCmdQueue(int iPosition) {
	char cReturn;
	int i, iHead, iTail;

	iHead = uiCmdQueueHead;
	iTail = uiCmdQueueTail;

	if (iHead == iTail) {
		cReturn = 0;
	}
	else {
		cReturn = 0;
		if (iHead > iTail) {
			for (i = iTail; i < iHead; i++) {
				if (iCmdQueue[i] == iPosition) {
					cReturn = -1;
					break;
				}
			}
		}
		else {
			for (i = iTail; i < CMD_SENT_SIZE + 1; i++) {
				if (iCmdQueue[i] == iPosition) {
					cReturn = -1;
					break;
				}
			}

			if (!cReturn) {
				if (iHead > 0) {
					for (i = 0; i < iHead; i++) {
						if (iCmdQueue[i] == iPosition) {
							cReturn = -1;
							break;
						}
					}
				}
			}
		}
	}

	return cReturn;
}

/**
 * \details Adds a command to the Amulet Command Queue
 * \param pcCommand Pointer to Null terminated command string to send to the Amulet Touchscreen
 * \param ptrOutput Pointer to the location, where the returned value from the command can be written
 * \param ptrFunction Pointer to a function, which is executed with the returned value from the command
 * \param ulResendTimeout Number of centi seconds before a command is resent
 * \param ulExpireTimeout Number of centi seconds before a command is expired and a reponse is not pending
 * \returns 0 = Success, -1 = Failed because Command Queue is full
 */
char PushCmdQueue(char *pcCommand, void *ptrOutput, void (*ptrFunction) (void *ptrInput), unsigned long int ulResendTimeout, unsigned long int ulExpireTimeout) {
	int iPosition;
	char cError;
	unsigned long int tstamp;
	short unsigned int uiNextHead;

	iPosition = 0;
	while ((InCmdQueue(iPosition)) && (iPosition < CMD_SENT_SIZE)) iPosition++;

	if (iPosition == CMD_SENT_SIZE) {
		cError = -1;
	}
	else {
		strcpy((CmdSent[iPosition]).cCommand, pcCommand);
		(CmdSent[iPosition]).ptrOutput = ptrOutput;
		(CmdSent[iPosition]).ptrFunction = ptrFunction;
		//if (uiCmdQueueHead == uiCmdQueueTail) g_msec_tstamp = 0;
		tstamp =  g_csec_tstamp;
		(CmdSent[iPosition]).ulCreateStamp = tstamp;

		if (ulResendTimeout == 0xFFFFFFFF) {
			(CmdSent[iPosition]).ulResendStamp = ulResendTimeout;
		}
		else {
			(CmdSent[iPosition]).ulResendStamp = tstamp + ulResendTimeout;
		}

		if (ulExpireTimeout == 0xFFFFFFFF) {
			(CmdSent[iPosition]).ulExpireStamp = ulExpireTimeout;
		}
		else {
			(CmdSent[iPosition]).ulExpireStamp = tstamp + ulExpireTimeout;
		}

		uiNextHead = uiCmdQueueHead + 1;
		if (uiNextHead == CMD_SENT_SIZE + 1) uiNextHead = 0;

		if (uiNextHead == uiCmdQueueTail) {
			cError = -1;
		}
		else {
			iCmdQueue[uiCmdQueueHead] = iPosition;
			uiCmdQueueHead = uiNextHead;
			cError = 0;
		}
	}
	return cError;
}
/**
 * \details Remove a command by setting the CmdArray index to -1 in the Amulet Command Queue
 * \param iPosition CmdArray index to be removed
 * \returns None
 */
void RemoveCmdQueue(int iPosition) {
	int i, iHead, iTail;

	iHead = uiCmdQueueHead;
	iTail = uiCmdQueueTail;

	if (!(iHead == iTail)) {
		if (iHead > iTail) {
			for (i = iTail; i < iHead; i++) {
				if (iCmdQueue[i] == iPosition) {
					iCmdQueue[i] = -1;
					break;
				}
			}
		}
		else {
			for (i = iTail; i < CMD_SENT_SIZE + 1; i++) {
				if (iCmdQueue[i] == iPosition) {
					iCmdQueue[i] = -1;
					break;
				}
			}

			if (iHead > 0) {
				for (i = 0; i < iHead; i++) {
					if (iCmdQueue[i] == iPosition) {
						iCmdQueue[i] = -1;
						break;
					}
				}
			}
		}
	}
}

/**
 * \details Remove spaces (-1) in the Amulet Command Queue left by removing CmdArray indexes. This routine is executed periodically to maintain the Amulet Command Queue.
 * \returns None
 */
void CompressCmdQueue(void) {
	int i, j, iHead, iTail;
	int jPrevious;

	iHead = uiCmdQueueHead;
	iTail = uiCmdQueueTail;

	if (!(iHead == iTail)) {
		i = iTail;
		while (i != iHead) {
			if (iCmdQueue[i] == -1) {
				j = i;

				while (j != iTail) {
					jPrevious = j - 1;
					if (jPrevious < 0) jPrevious = CMD_SENT_SIZE;

					iCmdQueue[j] = iCmdQueue[jPrevious];
					iCmdQueue[jPrevious] = -1;
					j = jPrevious;
				}

				iTail++;
				if (iTail == CMD_SENT_SIZE + 1) iTail = 0;
			}

			i++;
			if (i == CMD_SENT_SIZE + 1) i = 0;
		}
		uiCmdQueueTail = iTail;
	}
}

/**
 * \details Removes the CmdArray index from the Amulet Command Queue, if command string matches the command string in the CmdArray
 * \param pcCmd Pointer to null terminated command string
 * \param pCmdSent Pointer to CMDSENT structure, where a matched CmdArray data is written to.
 * \return 0 = No match found, -1 = Command was removed
 */
char ScanCmdQueueCommand(char *pcCmd, CMDSENT *pCmdSent) {
	int i, iHead, iTail;
	char cFound;

	iHead = uiCmdQueueHead;
	iTail = uiCmdQueueTail;

	cFound = 0;
	i = iTail;
	while (i != iHead) {
		if (iCmdQueue[i] != -1) {
			if (strcmp(CmdSent[iCmdQueue[i]].cCommand, pcCmd) == 0) {
				strcpy(pCmdSent->cCommand, CmdSent[iCmdQueue[i]].cCommand);
				pCmdSent->ptrOutput = CmdSent[iCmdQueue[i]].ptrOutput;
				pCmdSent->ptrFunction = CmdSent[iCmdQueue[i]].ptrFunction;
				pCmdSent->ulCreateStamp = CmdSent[iCmdQueue[i]].ulCreateStamp;
				pCmdSent->ulResendStamp = CmdSent[iCmdQueue[i]].ulResendStamp;
				pCmdSent->ulExpireStamp = CmdSent[iCmdQueue[i]].ulExpireStamp;
				iCmdQueue[i] = -1;
				cFound = -1;
				break;
			}
		}

		i++;
		if (i == CMD_SENT_SIZE + 1) i = 0;
	}

	if (cFound) CompressCmdQueue();

	return cFound;
}

/**
 * \details Removes a command from the Amulet Command Queue, when the current time exceeds the resend time
 * \param pCmdSent Pointer to CMDSENT structure, where the removed command data is written to
 * \returns 0 = No resend command were found, -1 = Resend command found
 */
char ScanCmdQueueResend(CMDSENT *pCmdSent) {
	unsigned long int ulNow;
	int i, iHead, iTail;
	char cFound;

	ulNow = g_csec_tstamp;

	iHead = uiCmdQueueHead;
	iTail = uiCmdQueueTail;

	cFound = 0;
	i = iTail;
	while (i != iHead) {
		if (iCmdQueue[i] != -1) {
			if (CmdSent[iCmdQueue[i]].ulResendStamp <= ulNow) {
				strcpy(pCmdSent->cCommand,CmdSent[iCmdQueue[i]].cCommand);
				pCmdSent->ptrOutput = CmdSent[iCmdQueue[i]].ptrOutput;
				pCmdSent->ptrFunction = CmdSent[iCmdQueue[i]].ptrFunction;
				pCmdSent->ulCreateStamp = CmdSent[iCmdQueue[i]].ulCreateStamp;
				pCmdSent->ulResendStamp = CmdSent[iCmdQueue[i]].ulResendStamp;
				pCmdSent->ulExpireStamp = CmdSent[iCmdQueue[i]].ulExpireStamp;
				iCmdQueue[i] = -1;
				cFound = -1;
				break;
			}
		}

		i++;
		if (i == CMD_SENT_SIZE + 1) i = 0;
	}

	if (cFound) CompressCmdQueue();

	return cFound;
}

/**
 * \details Removes a command from the Amulet Command Queue, when the current time exceeds the expired time
 * \param pCmdSent Pointer to CMDSENT structure, where the removed command data is written to
 * \returns 0 = No expired command were found, -1 = Expired command found
 */
char ScanCmdQueueExpired(CMDSENT *pCmdSent) {
	unsigned long int ulNow;
	int i, iHead, iTail;
	char cFound;

	ulNow = g_csec_tstamp;

	iHead = uiCmdQueueHead;
	iTail = uiCmdQueueTail;

	cFound = 0;
	i = iTail;
	while (i != iHead) {
		if (iCmdQueue[i] != -1) {
			if (CmdSent[iCmdQueue[i]].ulExpireStamp <= ulNow) {
				strcpy(pCmdSent->cCommand,CmdSent[iCmdQueue[i]].cCommand);
				pCmdSent->ptrOutput = CmdSent[iCmdQueue[i]].ptrOutput;
				pCmdSent->ptrFunction = CmdSent[iCmdQueue[i]].ptrFunction;
				pCmdSent->ulCreateStamp = CmdSent[iCmdQueue[i]].ulCreateStamp;
				pCmdSent->ulResendStamp = CmdSent[iCmdQueue[i]].ulResendStamp;
				pCmdSent->ulExpireStamp = CmdSent[iCmdQueue[i]].ulExpireStamp;
				iCmdQueue[i] = -1;
				cFound = -1;
				break;
			}
		}

		i++;
		if (i == CMD_SENT_SIZE + 1) i = 0;
	}

	if (cFound) CompressCmdQueue();

	return cFound;
}

/**
 * \details Pushes Amulet Message into either the Amulet Master Message Queue or the Amulet Slave Message Queue. The first byte is used to determine, which Amulet Queue to use
 * \param pcPacketArray Pointer to null terminated message
 * \returns None
 */
void PushPacket(char *pcPacketArray) {
	unsigned char ucStart;

	ucStart = (unsigned char) *pcPacketArray;

	if (((ucStart & 0xD0) == 0xD0) || (ucStart == 0xF2)) PushMasterMessage(pcPacketArray);
	else PushSlaveMessage(pcPacketArray);
}

/**
 * \details Check for a valid Amulet Packet by looking at the first byte and the length of the packet
 * \param pcPacketArray Pointer to null terminated Amulet Packet string
 * \returns 0 = Invalid Amulet Packet, -1 = Valid Amulet Packet
 */
char CheckPacket(char *pcPacketArray) {
	unsigned char ucStart;
	char cReturn;

	ucStart = (unsigned char) *pcPacketArray;

	switch(ucStart) {
		case 0xD0:
		case 0xD1:
		case 0xD2:
		case 0xD3:
		case 0xD4:
		case 0xD8:
		case 0xDD:
		case 0xDE:
		case 0xE8:
			if (strlen(pcPacketArray) == 3) cReturn = -1;
			else cReturn = 0;
			break;

		case 0xD5:
		case 0xE0:
		case 0xE5:
		case 0xEF:
		case 0xF3:
			if (strlen(pcPacketArray) == 5) cReturn = -1;
			else cReturn = 0;
			break;

		case 0xD6:
		case 0xE1:
		case 0xE6:
			if (strlen(pcPacketArray) == 7) cReturn = -1;
			else cReturn = 0;
			break;

		default:
			cReturn = -1;
			break;
	}

	return cReturn;
}
/**
 * \details Push an Amulet Command into the Amulet Command Queue and send the Amulet Command on UART_2
 * \param cOutput
 * \param ptrOutput
 * \param ptrFunction
 * \param ulResendTimeout
 * \param ulExpireTimeout
 * \returns 0 = Successful Sent, -1 = Not successfully sent because UART circular queue is full
 */
char SendUart2Command (char *cOutput, void *ptrOutput, void (*ptrFunction)(void *ptrInput), unsigned long int ulResendTimeout, unsigned long int ulExpireTimeout) {
	PushCmdQueue (cOutput, ptrOutput, ptrFunction, ulResendTimeout, ulExpireTimeout);
	return SendUart2Tx (cOutput);
}

/**
 * \details Send out Amulet Byte on UART_2
 * \param ucIndex Amulet Byte Index
 * \param ucValue Value of the Byte to send
 * \returns 0 = Successful Sent, -1 = Not successfully sent because UART circular queue is full
 */
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue) {
	char cValueArray[3];
	char cOutputArray[5];

	cOutputArray[0] = 0xD5;
	cOutputArray[1] = 0;

	ctoh(ucIndex, cValueArray);
	strcat(cOutputArray, cValueArray);
	ctoh(ucValue, cValueArray);
	strcat(cOutputArray, cValueArray);

	return SendUart2Tx(cOutputArray);
}

/**
 * \details Send out Amulet Word on UART_2
 * \param ucIndex Amulet Word Index
 * \param uiValue Value of the Word to send
 * \returns 0 = Successful Sent, -1 = Not successfully sent because UART circular queue is full
 */
char SetAmuletWord(unsigned char ucIndex, short unsigned int uiValue) {
	char cValueArray[5];
	char cOutputArray[8];

	cOutputArray[0] = 0xD6;
	cOutputArray[1] = 0;
	ctoh(ucIndex, cValueArray);
	strcat(cOutputArray, cValueArray);
	itoh(uiValue, cValueArray);
	strcat(cOutputArray, cValueArray);
	return SendUart2Tx(cOutputArray);
}

/**
 * \details Send out Amulet String on UART_2
 * \param ucIndex Amulet String Index
 * \param pcValue Pointer to null terminated string to send
 * \returns 0 = Successful Sent, -1 = Not successfully sent because UART circular queue is full
 */
char SetAmuletString(unsigned char ucIndex, char *pcValue) {
	char cIndexArray[3];
	char cOutputArray[250];
	int i;

	cOutputArray[0] = 0xD7;
	cOutputArray[1] = 0;
	ctoh(ucIndex, cIndexArray);
	strcat(cOutputArray, cIndexArray);

	i = 3;
	while (*pcValue != 0) {
		if(*pcValue == '$'){
			cOutputArray[i] = 0x75;
		}else if(*pcValue == 3){
			cOutputArray[i] = 0x02;
			i++;
			cOutputArray[i] = 0x80;
		}else if(*pcValue == 4){
			cOutputArray[i] = 0x02;
			i++;
			cOutputArray[i] = *pcValue;
		}else if(*pcValue == 5){
			cOutputArray[i] = 0x02;
			i++;
			cOutputArray[i] = 0x01;
		}else{
			cOutputArray[i] = *pcValue;
		}
		i++;
		pcValue++;
	}
	cOutputArray[i] = 0;
	return SendUart2Command (cOutputArray, NULL, NULL, 0xFFFFFFFF, 100);
}

/**
 * \details Send Amulet Display Screen Command
 * \param uiValue Amulet Screen index
 * \returns 0 = Successful Sent, -1 = Not successfully sent because UART circular queue is full
 */
char SetAmuletHTML(unsigned int uiValue){
	char cOutputArray[9];
	char cValueArray[5];
	unsigned int uiSum;
	unsigned char ucSum;
	short nuc_index;
	short ch_num = current.main_chamber;
	time_t savetime;
	short index;

	if((uiValue == AmuletHTMLIndex[MAINSCREEN_HTM]) || (uiValue == AmuletHTMLIndex[WELLMAINSCREEN_HTM]) || (uiValue == AmuletHTMLIndex[BETAMAINSCREEN_HTM])) m_usBackClear = 0;
	AmuletError_setNewPage(uiValue);
	AmuletWarning_setNewPage(uiValue);
	DisplayActivity_SendActivity = FALSE;
	DisplayActivity_SendActivity2 = FALSE;
	for(index=0; index<max_chambers; index++){
		if(remote[index].exists){
			if(index==ch_num){
				if(uiValue != AmuletHTMLIndex[REMOTE_HTM]) remote[index].keys_active = FALSE;
				else remote[index].keys_active = TRUE;
			}
			else remote[index].keys_active = TRUE;
		}else{
			remote[index].keys_active = FALSE;
		}
	}

	if(uiValue == AmuletHTMLIndex[MAINSCREEN_HTM]){
		savetime = measurement[ch_num].future.dosetime;
		meas_screen = TRUE;
		erase_screen();
		set_display_time();
		set_nokey();
		if(current.num_chambers>0){
			if(!chamber[ch_num].calkey){
				nuc_index = chamber[ch_num].nuc_index;
				set_nuclide_data(nuc_index, ch_num);
			}else{
				measurement[ch_num].mode = MEASMODE;
				measurement[ch_num].response = measurement[ch_num].resp0;
				respcor(ch_num, &measurement[ch_num].response);
			}
		}
		measurement[ch_num].future.dosetime = savetime;
	}else{
		measurement[ch_num].future.dosetime = NO_TIME;
	}

	m_iMenu = 0;
	m_iPhase = PHASE_GENERIC_NOP;
	m_iCurPhase = PHASE_GENERIC_CUR_NOP;
	m_iNextPhase = PHASE_GENERIC_CUR_NOP;

	cOutputArray[0] = 0xA0;
	cOutputArray[1] = 0x02;
	cOutputArray[2] = 0;

	itoh(uiValue, cValueArray);
	strcat(cOutputArray, cValueArray);

	uiSum = 0xA0 + 0x02;
	uiSum += (0xFF & (uiValue >> 8));
	uiSum += (0xFF & uiValue);
	uiSum = (0xFF & uiSum);
	uiSum = 0x100 - uiSum;
	ucSum = (0xFF & uiSum);
	ctoh(ucSum, cValueArray);
	strcat(cOutputArray, cValueArray);

	return SendUart2Command(cOutputArray, NULL, NULL, 0xFFFFFFFF, 100);
}

/**
 * \details Draw a line on the Amulet Screen
 * \param x1 X coordinate of Point 1
 * \param y1 Y coordinate of Point 1
 * \param x2 X coordinate of Point 2
 * \param y2 Y coordinate of Point 2
 * \param rgb Color of line as 24 bit RGB
 * \param weight Width of the line
 * \returns 0 = Successful Sent, -1 = Not successfully sent because UART circular queue is full
 */
char SetAmuletLine(ushort x1, ushort y1, ushort x2, ushort y2, ulong rgb, uchar weight){
	char cOutputArray[25];
	char cByteArray[3];
	char cWordArray[5];
	uchar red, blue, green;

	cOutputArray[0] = 0xD9;
	cOutputArray[1] = 0;

	itoh(x1, cWordArray);
	strcat(cOutputArray, cWordArray);

	itoh(y1, cWordArray);
	strcat(cOutputArray, cWordArray);

	itoh(x2, cWordArray);
	strcat(cOutputArray, cWordArray);

	itoh(y2, cWordArray);
	strcat(cOutputArray, cWordArray);

	blue = (uchar) (rgb & 0xFF);
	green = (uchar) ((rgb & 0xFF00) >> 8);
	red = (uchar) ((rgb & 0xFF0000) >> 16);

	ctoh(blue, cByteArray);
	strcat(cOutputArray, cByteArray);

	ctoh(green, cByteArray);
	strcat(cOutputArray, cByteArray);

	ctoh(red, cByteArray);
	strcat(cOutputArray, cByteArray);

	ctoh(weight, cByteArray);
	strcat(cOutputArray, &(cByteArray[1]));

	return SendUart2Tx(cOutputArray);
}

/**
 * \details Draw a fill rectangle on the Amulet Screen
 * \param x1 X coordinate of Point 1
 * \param y1 Y coordinate of Point 1
 * \param dx delta X
 * \param dy delta Y
 * \param rgb Color of line as 24 bit RGB
 * \param weight Width of the line
 * \returns 0 = Successful Sent, -1 = Not successfully sent because UART circular queue is full
 */
char SetAmuletFillRect(ushort x1, ushort y1, ushort dx, ushort dy, ulong rgb, uchar weight){
	char cOutputArray[25];
	char cByteArray[3];
	char cWordArray[5];
	uchar red, blue, green;

	cOutputArray[0] = 0xDB;
	cOutputArray[1] = 0;

	itoh(x1, cWordArray);
	strcat(cOutputArray, cWordArray);

	itoh(y1, cWordArray);
	strcat(cOutputArray, cWordArray);

	itoh(dx, cWordArray);
	strcat(cOutputArray, cWordArray);

	itoh(dy, cWordArray);
	strcat(cOutputArray, cWordArray);

	blue = (uchar) (rgb & 0xFF);
	green = (uchar) ((rgb & 0xFF00) >> 8);
	red = (uchar) ((rgb & 0xFF0000) >> 16);

	ctoh(blue, cByteArray);
	strcat(cOutputArray, cByteArray);

	ctoh(green, cByteArray);
	strcat(cOutputArray, cByteArray);

	ctoh(red, cByteArray);
	strcat(cOutputArray, cByteArray);

	ctoh(weight, cByteArray);
	strcat(cOutputArray, &(cByteArray[1]));

	return SendUart2Tx(cOutputArray);
}

//char SetAmuletByteArray() {
//}
//char SetAmuletWordArray() {
//}

// char GetAmuletByte()
// char GetAmuletWord()
// char GetAmuletString()
// char GetAmuletLabel()
// char GetAmuletRPC()

// debug routines
void print_uart(void) {
	int i;

	printf ("-------------------------------------------------------\n");
	printf ("uiMasterHead: %u\n", uiMasterHead);
	printf ("uiMasterTail: %u\n", uiMasterTail);

	printf ("uiSlaveHead: %u\n", uiSlaveHead);
	printf ("uiSlaveTail: %u\n", uiSlaveTail);

	printf ("uiCmdQueueHead: %u\n", uiCmdQueueHead);
	printf ("uiCmdQueueTail: %u\n", uiCmdQueueTail);

	if (uiCmdQueueHead != uiCmdQueueTail) {
		for (i=0; i < CMD_SENT_SIZE + 1; i++) {
			if (iCmdQueue[i] != -1) {
				printf("%d = %d\n", i, iCmdQueue[i]);

				printf("Command: %X", CmdSent[iCmdQueue[i]].cCommand[0]);
				printf("%s\n", &(CmdSent[iCmdQueue[i]].cCommand[1]));
				printf("Output Pointer: %lu\n", (long unsigned int) CmdSent[iCmdQueue[i]].ptrOutput);
				printf("Function Pointer: %lu\n", (long unsigned int) CmdSent[iCmdQueue[i]].ptrFunction);
				printf("Create Stamp: %lu\n", CmdSent[iCmdQueue[i]].ulCreateStamp);
				printf("Resend Stamp: %lu\n", CmdSent[iCmdQueue[i]].ulResendStamp);
				printf("Expire Stamp: %lu\n", CmdSent[iCmdQueue[i]].ulExpireStamp);
				printf("\n\n");
			}
		}
	}
	printf ("-------------------------------------------------------\n");
}

/**
 * \details Strip number string of leading and trailing zeros without changing value
 * \param numberString Pointer to null terminated string of a number
 * \returns None
 */
void Amulet_trimTrailingZero(char *numberString){
	char *workingPointer;
	char dotPresent;
	int positionsToShift;

	workingPointer = numberString;
	dotPresent = 0;
	while((*workingPointer)!=0){
		if((*workingPointer)=='.'){
			dotPresent=1;
		}
		workingPointer++;
	}

	if(dotPresent==1){
		workingPointer = numberString;
		while((*workingPointer)!=0) workingPointer++;
		if(workingPointer!=numberString){
			workingPointer--;
			while((workingPointer>numberString) && (((*workingPointer)=='0') || ((*workingPointer)=='.'))){
				if((*workingPointer)=='.'){
					*workingPointer = 0;
					break;
				}
				*workingPointer = 0;
				workingPointer--;
			}
		}
	}

	if(strlen(numberString)>1){
		workingPointer = numberString;
		positionsToShift = 0;
		while((*workingPointer)=='0'){
			positionsToShift++;
			workingPointer++;
		}
		if(positionsToShift>0){
			while((*workingPointer)!=0){
				*(workingPointer - positionsToShift) = *workingPointer;
				workingPointer++;
			}
			*(workingPointer - positionsToShift) = 0;
		}
	}
}

/**
 * \details Strip leading and trailing spaces from string
 * \param acByte Pointer to null terminated string
 * \returns None
 */
void trim(char *acByte) {
	char *ptr;
	int iMove;


	ptr = acByte;

	while((*ptr) != 0) ptr++;

	if (ptr != acByte) {
		ptr--;

		while ((ptr>=acByte) && (*ptr == ' ')) {
			*ptr = 0;
			ptr--;
		}
	}

	ptr = acByte;
	iMove = 0;
	while (*ptr == ' ') {
		iMove++;
		ptr++;
	}

	while (*ptr!=0) {
		*(ptr - iMove) = *ptr;
		ptr++;
	}
	*(ptr - iMove) = 0;
}

/**
 * \details Converts all lower case letters (a-z) to upper case letters (A-Z)
 * \param acByte Pointer to null terminated string
 * \returns None
 */
void toupper(char *acByte){
	char *ptr;

	ptr = acByte;
	while((*ptr) != 0){
		if(((*ptr) >= 97) && ((*ptr) <= 122)){
			(*ptr) = (*ptr) - 32;
		}
		ptr++;
	}
}

/**
 * \details Remove leading spaces, trailing spaces and any spaces between characters
 * \param acByte Pointer to null terminated string
 * \returns None
 */
void trim_and_shrink(char *acByte) {
	char *ptr;
	char *last;

	trim(acByte);

	last = acByte;
	ptr = last + 1;
	while (*ptr != 0) {
		if (*ptr != 32) {
			last++;
			*last = *ptr;
		}
		ptr++;
	}
	last++;
	*last = 0;
}

void wildcard(char *input, char *output){
	int i, length;
	char *ptr;

	length = strlen(input);
	if(length){
		ptr = output;
		for(i=0; i<length; i++){
			*ptr++ = '*';
		}
		*ptr=0;
	}else{
		*output = 0;
	}
}

void acc_meas_string(char *output, short iCurrentSource, short ch_num, short iOffset, bool *error) {
	char buffer[100];

	//sprintf(output, "%d) ", iCurrentSource + iOffset);
	//strcpy(buffer, acc_data[iCurrentSource].nucname);
	//trim(buffer);
	//strcat(output, buffer);
	//strcat(output, ", s/n:");
	//strcpy(buffer, acc_data[iCurrentSource].sn);
	//trim(buffer);
	//strcat(output, buffer);
	strcpy(output, "Calc: ");
	strcpy(buffer, acc_data[iCurrentSource].test_res[ch_num].pract);
	trim(buffer);
	strcat(output, buffer);
	buffer[3]=0;
	strncpy(buffer, unit_str[acc_data[iCurrentSource].test_res[ch_num].pr_kun - 1], 3);
	trim(buffer);
	strcat(output, buffer);
	strcat(output, "\nMeas: ");
	buffer[7] = 0;
	strncpy(buffer, acc_data[iCurrentSource].test_res[ch_num].msact, 7);
	trim(buffer);
	strcat(output, buffer);
	buffer[3] = 0;
	strncpy(buffer, unit_str[acc_data[iCurrentSource].test_res[ch_num].ms_kun - 1], 3);
	trim(buffer);
	strcat(output, buffer);
	if (acc_data[iCurrentSource].test_res[ch_num].nc < 0) {
		strcat(output, "\nERROR");
		*error = TRUE;
	}
	else {
		strncpy(buffer, acc_data[iCurrentSource].test_res[ch_num].var, acc_data[iCurrentSource].test_res[ch_num].nc);
		trim(buffer);
		strcat(output, "\nDev: ");
		strcat(output, buffer);
		strcat(output, "%");
		*error = FALSE;
	}
}

/**
 * \details Test for empty string or a '.' string
 * \returns 0 = Is empty or just a '.', 0xFF = Everything Else
 */
unsigned char IsNumber(char *ptr) {
	unsigned char ucReturn;

	if(*ptr == 0) {
		ucReturn = 0;
	}
	else if((*ptr == '.') && (*(ptr + 1) == 0)) {
		ucReturn = 0;
	}
	else {
		ucReturn = 0xFF;
	}

	return ucReturn;
}

/**
 * \details Create time strings for the Standard Linearity Test
 * \param index Measurement number
 * \param def Pointer to Linearity Setup Structure
 * \param start_time Time linearity tests was started
 * \param elapsed Pointer to space allocated for elapsed time string
 * \param time_due Pointer to space allocated for time due string
 * \returns None
 */
void LinearElapsedTime(short index, LINDEF *def, time_t start_time, char *elapsed, char *time_due) {
	time_t temp_date;
	long diffmin, nextmin;
	short elapsed_hours, elapsed_mins;
	char acMsg[100], formatstring[100];
	short ch_num = current.main_chamber;
	short ch_type, ch_type_mod;

	ch_type = chamber_type(ch_num);
	ch_type_mod = chamber_type_77t_remap(ch_type); // C_CHAMB and K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded
	read_clock(&temp_date);
	diffmin = (long) difftime(temp_date, start_time) / 60;
	nextmin = 60 * (def->hours_Std[ch_type_mod][index]);
	nextmin = nextmin - diffmin;
	elapsed_hours = nextmin / 60;
	elapsed_mins = nextmin % 60;
	get_amulet_message(L_IN_HRS_MINS, formatstring);    // "in %d hrs, %d mins"
	sprintf(elapsed, formatstring, elapsed_hours, elapsed_mins);

	nextmin = (def->hours_Std[ch_type_mod][index]) * 3600;
	temp_date = start_time + nextmin;
	dateout_language(time_due, &temp_date, 2);
	strcat(time_due, "; ");
	timeout(acMsg, &temp_date);
	strcat(time_due, acMsg);
}

void display_inventory(short *map, short *first_map_item_displayed, short *selected_map_item, short last_map_item) {
	short last_displayed;
	short item_count;
	short i, j;
	short index_end;
	char page_down_visible;
	char page_up_visible;
	short row;
	short *mapoffset;
	//float fActivity;
	float fVolume;
	//float fMoPerTc;
	short iNucNum;
	short iType;
	char  cId;
	//char  cSyst;
	char  acLot[16];
	//time_t  dtmDate;
	short iAmuletGridAddress;
	NUCDATA nuc;
	char acMsg[25];
	float curr_act;
	time_t nowtime;
	//short ndec;
	//short kun;
	//float act_disp;
	bool over_flag;
	float conc;
	//unsigned long int temp_msec;
	char f_str[20];

	if (last_map_item == -1) {
		*first_map_item_displayed = -1;
	}
	else {
		item_count = ITEMS_PER_PAGE;
		last_displayed = last_map_item / item_count;
		last_displayed *= item_count;

		if (*first_map_item_displayed == -1) {
			*first_map_item_displayed = 0;
		}
		else {
			if (*first_map_item_displayed > last_map_item) {
				*first_map_item_displayed = last_displayed;
			}
		}

		index_end = *first_map_item_displayed + (ITEMS_PER_PAGE - 1);
		if (index_end > last_map_item) index_end = last_map_item;

		row = 0;
		for (i=*first_map_item_displayed; i<=index_end; i++) {
			mapoffset = map + i;
			//fActivity = inventory[*mapoffset].act;
			fVolume = inventory[*mapoffset].vol;
			//fMoPerTc = inventory[*mapoffset].mopertc;
			iNucNum = inventory[*mapoffset].nucnum;
			iType = inventory[*mapoffset].type;
			cId = inventory[*mapoffset].id;
			//cSyst = inventory[*mapoffset].syst;
			acLot[15] = 0;
			strncpy(acLot, inventory[*mapoffset].lot, 15);
			//dtmDate = inventory[*mapoffset].date;

			iAmuletGridAddress = (7*row) + 107;

			// Display Nuclide
			NuclideData_getNuclide(iNucNum, &nuc);
			acMsg[6] = 0;
			strncpy(acMsg, nuc.name, 6);
			//SetAmuletString(iAmuletGridAddress + 0, acMsg);
			send_to_amulet_string(iAmuletGridAddress + 0, acMsg);

			// Display Study
			if (iType == -1) {
				acMsg[0] = 0;
			}
			else {
				//acMsg[5] = 0;
				//strncpy(acMsg, study_type[iType], 5);
				get_study_type(iType,f_str);
				strcpy(acMsg,f_str);
			}
			//SetAmuletString(iAmuletGridAddress + 1, acMsg);
			send_to_amulet_string(iAmuletGridAddress + 1, acMsg);

			// Display Lot
			//SetAmuletString(iAmuletGridAddress + 2, acLot);
			send_to_amulet_string(iAmuletGridAddress + 2, acLot);

			// Display ID
			if ((cId == -1) || (cId == 0)) {
				acMsg[0] = 0;
			}
			else {
				sprintf(acMsg, "%02d", cId);
			}
			//SetAmuletString(iAmuletGridAddress + 3, acMsg);
			send_to_amulet_string(iAmuletGridAddress + 3, acMsg);

			// Display Activity
			curr_act = get_current_act(&inventory[*mapoffset], &nuc, &nowtime);
			// Removed:
			//ndec = getdec(curr_act, inventory[i].syst, &kun);
			//ndec = getdec(curr_act, CI, &kun);
			//act_disp = curr_act * unitfact[kun - 1];
            over_flag = FALSE;
            // Removed:
			//if(inventory[*mapoffset].syst == CI && curr_act > 10. || inventory[*mapoffset].syst == BQ && curr_act > 1.0e+12)
            if(curr_act > 10.)
            {
				//strcpy(acMsg, "OVER");
            	get_amulet_message(L_OVER, acMsg);    // " OVER "
            	trim(acMsg);
                over_flag = TRUE;
            }
			else
			{
				// Removed:
				//act2str(act_disp, acMsg, ndec, FALSE, kun);
				//if(kun == UCI)
                //{
				//	acMsg[6] = 'u';
                //}
				format_activity_system(curr_act, acMsg);
			}
			//SetAmuletString(iAmuletGridAddress + 4, acMsg);
			send_to_amulet_string(iAmuletGridAddress + 4, acMsg);

			// Display Volume
			sprintf(acMsg,"%5.1f", fVolume);
			//SetAmuletString(iAmuletGridAddress + 5, acMsg);
			send_to_amulet_string(iAmuletGridAddress + 5, acMsg);

			// Display Conc
			if (over_flag) {
				acMsg[0] = 0;
			}
			else {
				// Removed:
				//conc = act_disp / fVolume;
				conc = curr_act / fVolume;

				// Removed:
				//act2str(conc, acMsg, ndec, FALSE, kun);
				//if(kun == UCI)
                //{
				//	acMsg[6] = 'u';
                //}
				format_activity_system(conc, acMsg);
			}
			//SetAmuletString(iAmuletGridAddress + 6, acMsg);
			send_to_amulet_string(iAmuletGridAddress + 6, acMsg);

			SetAmuletByte(row + 244, 0xFF);

			//delayloop(2);
			delayloop(20);
			row++;
		}

		if (row < ITEMS_PER_PAGE) {
			acMsg[0] = 0;
			for (i=row; i<ITEMS_PER_PAGE; i++) {
				iAmuletGridAddress = (7*i) + 107;
/*				SetAmuletString(iAmuletGridAddress, acMsg);
 - 				SetAmuletString(iAmuletGridAddress + 1, acMsg);
 - 				SetAmuletString(iAmuletGridAddress + 2, acMsg);
 - 				SetAmuletString(iAmuletGridAddress + 3, acMsg);
 - 				SetAmuletString(iAmuletGridAddress + 4, acMsg);
 - 				SetAmuletString(iAmuletGridAddress + 5, acMsg);
 - 				SetAmuletString(iAmuletGridAddress + 6, acMsg);*/

				send_to_amulet_string(iAmuletGridAddress, acMsg);
				send_to_amulet_string(iAmuletGridAddress + 1, acMsg);
				send_to_amulet_string(iAmuletGridAddress + 2, acMsg);
				send_to_amulet_string(iAmuletGridAddress + 3, acMsg);
				send_to_amulet_string(iAmuletGridAddress + 4, acMsg);
				send_to_amulet_string(iAmuletGridAddress + 5, acMsg);
				send_to_amulet_string(iAmuletGridAddress + 6, acMsg);

				SetAmuletByte(i + 244, 0);
				//delayloop(2);
				delayloop(20);
			}
		}
	}

	page_up_visible = 1;
	page_down_visible = 1;
	if (((*first_map_item_displayed) == -1) || ((*first_map_item_displayed) == 0)) page_up_visible = 0;
	if (((*first_map_item_displayed) == -1) || ((*first_map_item_displayed) == last_displayed)) page_down_visible = 0;

	SetAmuletByte(243, page_up_visible);
	SetAmuletByte(242, page_down_visible);

	if ((*first_map_item_displayed) == -1) {
		//strcpy(acMsg, "Page 1 of 1");
		i = j = 1;
		get_amulet_message(L_PAGE_OF,f_str);    // "Page %d of %d"
		sprintf(acMsg, f_str, i, j);
	}
	else {
		i = (*first_map_item_displayed) / ITEMS_PER_PAGE;
		i++;

		j = last_map_item / ITEMS_PER_PAGE;
		j++;

		//sprintf(acMsg, "Page %d of %d", i, j);
		get_amulet_message(L_PAGE_OF,f_str);    // "Page %d of %d"
		sprintf(acMsg, f_str, i, j);
	}
	//SetAmuletString(177, acMsg);
	send_to_amulet_string(177, acMsg);

	*selected_map_item = -1;
}

/**
 * \details Calculate crc for a 128 byte block
 * \param ptr Pointer to 128 byte block
 * \returns crc value
 */
short int calcrc(char *ptr){
	 short int crc, count;
	 char i;

	 crc = 0;
	 count = 128;

	 while(--count >= 0){
		 crc = crc ^ (short int) *ptr++ << 8;

		 i = 8;
		 do{
			 if(crc & 0x8000) crc = crc << 1 ^ 0x1021;
			 else crc = crc << 1;
		 } while(--i);
	 }
	 return crc;
 }

/**
 * \details Send command to display Amulet Screen and place the screen on top of the screen page stack
 * \param ucValue Screen index
 * \returns None
 */
void CallHTML(unsigned char ucValue){
	unsigned char i;

	switch(ucValue){
		case SET_HTML_ACCURACY:
			i = AmuletHTMLIndex[ACCURACY_HTM];
			break;

		case SET_HTML_BACKGROUND:
			i = AmuletHTMLIndex[BACKGROUND_HTM];
			break;

		case SET_HTML_CHAMBERVOLTS:
			i = AmuletHTMLIndex[CHAMBERVOLTS_HTM];
			break;

		case SET_HTML_DAILY:
			i = AmuletHTMLIndex[DAILY_HTM];
			break;

		case SET_HTML_GEOMETRY:
			i = AmuletHTMLIndex[GEOMETRY_HTM];
			break;

		case SET_HTML_GEOMETRYREPORT:
			i = AmuletHTMLIndex[GEOMETRYREPORT_HTM];
			break;

		case SET_HTML_HMPAO:
			i = AmuletHTMLIndex[HMPAO_HTM];
			break;

		case SET_HTML_INDEX:
			i = AmuletHTMLIndex[INDEX_HTM];
			break;

		case SET_HTML_INVENTORY:
			i = AmuletHTMLIndex[INVENTORY_HTM];
			break;

		case SET_HTML_INVENTORYADD:
			i = AmuletHTMLIndex[INVENTORYADD_HTM];
			break;

		case SET_HTML_INVENTORYWITHDRAW:
			i = AmuletHTMLIndex[INVENTORYWITHDRAW_HTM];
			break;

		case SET_HTML_KEYBOARD2:
			i = AmuletHTMLIndex[KEYBOARD2_HTM];
			break;

		case SET_HTML_KEYPAD2:
			i = AmuletHTMLIndex[KEYPAD2_HTM];
			break;

		case SET_HTML_LINEARITY:
			i = AmuletHTMLIndex[LINEARITY_HTM];
			break;

		case SET_HTML_MAG3:
			i = AmuletHTMLIndex[MAG3_HTM];
			break;

		case SET_HTML_MAINSCREEN:
			i = AmuletHTMLIndex[MAINSCREEN_HTM];
			break;

		case SET_HTML_MEASUREACTIVITY:
			i = AmuletHTMLIndex[MEASUREACTIVITY_HTM];
			break;

		case SET_HTML_MOLY:
			i = AmuletHTMLIndex[MOLY_HTM];
			break;

		case SET_HTML_NUCLIDE2:
			i = AmuletHTMLIndex[NUCLIDE2_HTM];
			break;

		case SET_HTML_ONESTRIP:
			i = AmuletHTMLIndex[ONESTRIP_HTM];
			break;

		case SET_HTML_QC:
			i = AmuletHTMLIndex[QC_HTM];
			break;

		case SET_HTML_SETACTIVITY:
			i = AmuletHTMLIndex[SETACTIVITY_HTM];
			break;

		case SET_HTML_SETSTUDY:
			i = AmuletHTMLIndex[SETSTUDY_HTM];
			break;

		case SET_HTML_SETTIME2:
			i = AmuletHTMLIndex[SETTIME2_HTM];
			break;

		case SET_HTML_SETUP:
			i = AmuletHTMLIndex[SETUP_HTM];
			break;

		case SET_HTML_TWOSTRIP:
			i = AmuletHTMLIndex[TWOSTRIP_HTM];
			break;

		case SET_HTML_INVENTORYDELETE:
			i = AmuletHTMLIndex[INVENTORYDELETE_HTM];
			break;

		case SET_HTML_INVENTORYKIT:
			i = AmuletHTMLIndex[INVENTORYKIT_HTM];
			break;

		case SET_HTML_SETSTUDY2:
			i = AmuletHTMLIndex[SETSTUDY2_HTM];
			break;

		case SET_HTML_ENHANCED:
			i = AmuletHTMLIndex[ENHANCED_HTM];
			break;

		case SET_HTML_INFO:
			i = AmuletHTMLIndex[INFO_HTM];
			break;

		case SET_HTML_PSETUP:
			i = AmuletHTMLIndex[OTHER_HTM];
			break;

		case SET_HTML_SETUPSOURCES:
			i = AmuletHTMLIndex[SETUPSOURCES_HTM];
			break;

		case SET_HTML_SETUPMOLY:
			i = AmuletHTMLIndex[SETUPMOLY_HTM];
			break;

		case SET_HTML_SETUPNUCLIDE:
			i = AmuletHTMLIndex[SETUPNUCLIDE_HTM];
			break;

		case SET_HTML_SETUPLINEARITY:
			i = AmuletHTMLIndex[SETUPLINEARITY_HTM];
			break;

		case SET_HTML_SETUPREMOTE:
			i = AmuletHTMLIndex[SETUPREMOTE_HTM];
			break;

		case SET_HTML_SETUPCALNUM:
			i = AmuletHTMLIndex[SETUPCALNUM_HTM];
			break;

		case SET_HTML_ERRORMSG:
			i = AmuletHTMLIndex[ERRORMSG_HTM];
			break;

		case SET_HTML_WARNINGMSG:
			i = AmuletHTMLIndex[WARNINGMSG_HTM];
			break;

		case SET_HTML_NOTIFICATIONMSG:
			i = AmuletHTMLIndex[NOTIFICATIONMSG_HTM];
			break;

		case SET_HTML_SETUPLINEARITYSTANDARD:
			i = AmuletHTMLIndex[SETUPLINEARITYSTANDARD_HTM];
			break;

		case SET_HTML_SETUPLINEARITYLINEATOR:
			i = AmuletHTMLIndex[SETUPLINEARITYLINEATOR_HTM];
			break;

		case SET_HTML_SETUPLINEARITYCALICHECK:
			i = AmuletHTMLIndex[SETUPLINEARITYCALICHECK_HTM];
			break;

		case SET_HTML_DOSETABLE:
			i = AmuletHTMLIndex[DOSETABLE_HTM];
			break;

		case SET_HTML_SELECT_CHAMBER:
			i = AmuletHTMLIndex[CHAMBER_HTM];
			break;

		case SET_HTML_GRADIENT:
			i = AmuletHTMLIndex[GRADIENT_HTM];
			break;

		case SET_HTML_WELLMAINSCREEN:
			i = AmuletHTMLIndex[WELLMAINSCREEN_HTM];
			break;

		case SET_HTML_BETAMAINSCREEN:
			i = AmuletHTMLIndex[BETAMAINSCREEN_HTM];
			break;

		case SET_HTML_WELLMEASUREMENT:
			i = AmuletHTMLIndex[WELLMEASUREMENT_HTM];
			break;

		case SET_HTML_WELLAUTOCALIBRATE:
			i = AmuletHTMLIndex[WELLAUTOCALIBRATE_HTM];
			break;

		case SET_HTML_WELLPEAKS:
			i = AmuletHTMLIndex[WELLPEAKS_HTM];
			break;

		case SET_HTML_WELLADVANCEDSETUP:
			i = AmuletHTMLIndex[WELLADVANCEDSETUP_HTM];
			break;

		case SET_HTML_WELLSETUPNUCLIDE:
			i = AmuletHTMLIndex[WELLSETUPNUCLIDE_HTM];
			break;

		case SET_HTML_WELLSETUPSEALED2:
			i = AmuletHTMLIndex[WELLSETUPSEALED2_HTM];
			break;

		case SET_HTML_WELLSETUPTESTSOURCE:
			i = AmuletHTMLIndex[WELLSETUPTESTSOURCE_HTM];
			break;

		case SET_HTML_WELLSETUPTRIGGERLEVEL:
			i = AmuletHTMLIndex[WELLSETUPTRIGGERLEVEL_HTM];
			break;

		case SET_HTML_WELLSETUPEFFICIENCIES:
			i = AmuletHTMLIndex[WELLSETUPEFFICIENCIES_HTM];
			break;

		case SET_HTML_WELLEDITEFFICIENCIES:
			i = AmuletHTMLIndex[WELLEDITEFFICIENCIES_HTM];
			break;

		case SET_HTML_WELLMANUAL:
			i = AmuletHTMLIndex[WELLMANUAL_HTM];
			break;

		case SET_HTML_WELLMEASUREEFFICIENCY:
			i = AmuletHTMLIndex[WELLMEASUREEFFICIENCY_HTM];
			break;

		case SET_HTML_AUTOCONSTANCY:
			i = AmuletHTMLIndex[AUTOCONSTANCY_HTM];
			break;

		case SET_HTML_SLEEP:
			i = AmuletHTMLIndex[SLEEP_HTM];
			break;

		case SET_HTML_FACTORY:
			i = AmuletHTMLIndex[FACTORY_HTM];
			break;

		case SET_HTML_SETUPCHAMBER:
			i = AmuletHTMLIndex[SETUPCHAMBER_HTM];
			break;

		case SET_HTML_INVENTORYDELETEALL:
			i = AmuletHTMLIndex[INVENTORYDELETEALL_HTM];
			break;

		case SET_HTML_WELLMDATEST:
			i = AmuletHTMLIndex[WELLMDATEST_HTM];
			break;

		case SET_HTML_WELLMEASUREMENTS:
			i = AmuletHTMLIndex[WELLMEASUREMENTS_HTM];
			break;

		case SET_HTML_WELLEDITFULLEFFICIENCY:
			i = AmuletHTMLIndex[WELLEDITFULLEFFICIENCY_HTM];
			break;

		case SET_HTML_SETUPCALIBSERIAL:
			i = AmuletHTMLIndex[SETUPCALIBSERIALNUM_HTM];
			break;

		case SET_HTML_WELLWIPEREPORT:
			i = AmuletHTMLIndex[WELLWIPEREPORT_HTM];
			break;

		case SET_HTML_WELLLABTESTS:
			i = AmuletHTMLIndex[WELLLABTESTS_HTM];
			break;

		case SET_HTML_WELLSCHILLING:
			i = AmuletHTMLIndex[WELLSCHILLING_HTM];
			break;

		case SET_HTML_WELLPLASMA:
			i = AmuletHTMLIndex[WELLPLASMA_HTM];
			break;

		case SET_HTML_WELLRBC:
			i = AmuletHTMLIndex[WELLRBC_HTM];
			break;

		case SET_HTML_WELLFACTORY:
			i = AmuletHTMLIndex[WELLFACTORY_HTM];
			break;

		case SET_HTML_WELLPLASMAANALYSIS:
			i = AmuletHTMLIndex[WELLPLASMAANALYSIS_HTM];
			break;

		case SET_HTML_WELLRBCANALYSIS:
			i = AmuletHTMLIndex[WELLRBCANALYSIS_HTM];
			break;

		case SET_HTML_WELLQATESTS:
			i = AmuletHTMLIndex[WELLQATESTS_HTM];
			break;

		case SET_HTML_WELLSYSTESTANALYSIS:
			i = AmuletHTMLIndex[WELLSYSTEMTESTANALYSIS_HTM];
			break;

		case SET_HTML_WELLCHITEST:
			i = AmuletHTMLIndex[WELLCHITEST_HTM];
			break;

		case SET_HTML_WELLSCHILLINGANALYSIS:
			i = AmuletHTMLIndex[WELLSCHILLINGANALYSIS_HTM];
			break;

		case SET_HTML_WELLCHANGEROI:
			i = AmuletHTMLIndex[WELLCHANGEROI_HTM];
			break;

		case SET_HTML_WELLGENERALANALYSIS:
			i = AmuletHTMLIndex[WELLGENERALANALYSIS_HTM];
			break;

		case SET_HTML_SETUPRHOTKEYS:
			i = AmuletHTMLIndex[SETUPRHOTKEYS_HTM];
			break;

		case SET_HTML_SETUPPHOTKEYS:
			i = AmuletHTMLIndex[SETUPPHOTKEYS_HTM];
			break;

		case SET_HTML_REMOTE:
			i = AmuletHTMLIndex[REMOTE_HTM];
			break;

		case SET_HTML_DELETELINEATOR:
			i = AmuletHTMLIndex[DELETELINEATORSETTINGS_HTM];
			break;

		case SET_HTML_DELETECALICHECK:
			i = AmuletHTMLIndex[DELETECALICHECKSETTINGS_HTM];
			break;

		case SET_HTML_SELECTLINEARITY:
			i = AmuletHTMLIndex[SELECTLINEARITY_HTM];
			break;

		case SET_HTML_LOWLEVEL:
			i = AmuletHTMLIndex[LOWLEVEL_HTM];
			break;

		case SET_HTML_WELLSTABILITYTEST:
			i = AmuletHTMLIndex[WELLSTABILITYTEST_HTM];
			break;

		case SET_HTML_SETUPCOMMUNICATIONS:
			i = AmuletHTMLIndex[SETUPCOMMUNICATIONS_HTM];
			break;

		case SET_HTML_PCQC:
			i = AmuletHTMLIndex[PCQC_HTM];
			break;
			
		case SET_HTML_ETHERNET:
			i = AmuletHTMLIndex[ETHERNET_HTM];
			break;

		case SET_HTML_WELLSETUPWIPES:
			i = AmuletHTMLIndex[WELLSETUPWIPES_HTM];
			break;

		case SET_HTML_WELLSETUPTYPES:
			i = AmuletHTMLIndex[WELLSETUPTYPES_HTM];
			break;

		case SET_HTML_WELLSETUPLOCATIONS:
			i = AmuletHTMLIndex[WELLSETUPLOCATIONS_HTM];
			break;

		case SET_HTML_WELLSETUPADDEDITLOCATION:
			i = AmuletHTMLIndex[WELLSETUPADDEDITLOCATION_HTM];
			break;

		case SET_HTML_WELLSETUPDELETELOCATION:
			i = AmuletHTMLIndex[WELLSETUPDELETELOCATION_HTM];
			break;

		case SET_HTML_WELLWIPELIST:
			i = AmuletHTMLIndex[WELLWIPELIST_HTM];
			break;

		case SET_HTML_ERRORMSGLARGE:
			i = AmuletHTMLIndex[ERRORMSGLARGE_HTM];
			break;

		case SET_HTML_WELLREPORTS:
			i = AmuletHTMLIndex[WELLREPORTS_HTM];
			break;

		case SET_HTML_WELLWIPESEARCH:
			i = AmuletHTMLIndex[WELLWIPESEARCH_HTM];
			break;

		case SET_HTML_GENERICITEMS:
			i = AmuletHTMLIndex[GENERICITEMS_HTM];
			break;

		case SET_HTML_INACTIVATE:
			i = AmuletHTMLIndex[INACTIVATE_HTM];
			break;

		case SET_HTML_WELLSYSTEMTESTSEARCH:
			i = AmuletHTMLIndex[WELLSYSTEMTESTSEARCH_HTM];
			break;

		case SET_HTML_WELLMDASEARCH:
			i = AmuletHTMLIndex[WELLMDASEARCH_HTM];
			break;

		case SET_HTML_WELLMDAANALYSIS:
			i = AmuletHTMLIndex[WELLMDAANALYSIS_HTM];
			break;

		case SET_HTML_WELLCHISEARCH:
			i = AmuletHTMLIndex[WELLCHISEARCH_HTM];
			break;

		case SET_HTML_WELLCHIANALYSIS:
			i = AmuletHTMLIndex[WELLCHIANALYSIS_HTM];
			break;

		case SET_HTML_TESTIDENT:
			i = AmuletHTMLIndex[TESTIDENT_HTM];
			break;

		case SET_HTML_WELLSCHILLINGSEARCH:
			i = AmuletHTMLIndex[WELLSCHILLINGSEARCH_HTM];
			break;

		case SET_HTML_WELLSCHILLINGREPORT:
			i = AmuletHTMLIndex[WELLSCHILLINGREPORT_HTM];
			break;

		case SET_HTML_WELLPLASMASEARCH:
			i = AmuletHTMLIndex[WELLPLASMASEARCH_HTM];
			break;

		case SET_HTML_WELLPLASMAREPORT:
			i = AmuletHTMLIndex[WELLPLASMAREPORT_HTM];
			break;

		case SET_HTML_WELLRBCSEARCH:
			i = AmuletHTMLIndex[WELLRBCSEARCH_HTM];
			break;

		case SET_HTML_WELLRBCREPORT:
			i = AmuletHTMLIndex[WELLRBCREPORT_HTM];
			break;

		case SET_HTML_WELLAUTOCALSEARCH:
			i = AmuletHTMLIndex[WELLAUTOCALSEARCH_HTM];
			break;

		case SET_HTML_WELLAUTOCALREPORT:
			i = AmuletHTMLIndex[WELLAUTOCALREPORT_HTM];
			break;

		case SET_HTML_WELLFACTORYDETECTORS:
			i = AmuletHTMLIndex[WELLFACTORYDETECTORS_HTM];
			break;

		case SET_HTML_WELLSETUPUSERNUCLIDES:
			i = AmuletHTMLIndex[WELLSETUPUSERNUCLIDES_HTM];
			break;

		case SET_HTML_WELLADDEDITUSERNUCLIDES:
			i = AmuletHTMLIndex[WELLADDEDITUSERNUCLIDES_HTM];
			break;

		case SET_HTML_LOGIN:
			i = AmuletHTMLIndex[LOGIN_HTM];
			break;

		case SET_HTML_SECURITY:
			i = AmuletHTMLIndex[SECURITY_HTM];
			break;

		case SET_HTML_ADDEDITUSER:
			i = AmuletHTMLIndex[ADDEDITUSER_HTM];
			break;

		case SET_HTML_WELLSETUPBIOASSAY:
			i = AmuletHTMLIndex[WELLSETUPBIOASSAY_HTM];
			break;

		case SET_HTML_WELLSETUPBIOASSAYENTEREFF:
			i = AmuletHTMLIndex[WELLSETUPBIOASSAYENTEREFFICIENCY_HTM];
			break;

		case SET_HTML_WELLSETUPBIOASSAYMEASUREEFF:
			i = AmuletHTMLIndex[WELLSETUPBIOASSAYMEASUREEFFICIENCY_HTM];
			break;

		case SET_HTML_WELLBIOASSAY:
			i = AmuletHTMLIndex[WELLBIOASSAY_HTM];
			break;

		case SET_HTML_WELLBIOASSAYANALYSIS:
			i = AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM];
			break;

		case SET_HTML_WELLBIOASSAYSEARCH:
			i = AmuletHTMLIndex[WELLBIOASSAYSEARCH_HTM];
			break;

		case SET_HTML_WELLSETUPBIOASSAYENTERROI:
			i = AmuletHTMLIndex[WELLSETUPBIOASSAYENTERROI_HTM];
			break;

		case SET_HTML_WELLSETUPTHYROIDUPTAKEPROTOCOL:
			i = AmuletHTMLIndex[WELLSETUPTHYROIDUPTAKEPROTOCOL_HTM];
			break;

		case SET_HTML_ADDEDITTHYROIDUPTAKEPROTOCOL:
			i = AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM];
			break;

		case SET_HTML_WELLTHYROIDUPTAKE:
			i = AmuletHTMLIndex[WELLTHYROIDUPTAKE_HTM];
			break;

		case SET_HTML_ADDEDITTHYROIDUPTAKETEST:
			i = AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM];
			break;

		case SET_HTML_WELLTHYROIDUPTAKETEST:
			i = AmuletHTMLIndex[WELLTHYROIDUPTAKETEST_HTM];
			break;

		case SET_HTML_WELLTHYROIDUPTAKEENTERADMIN:
			i = AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM];
			break;

		case SET_HTML_WELLTHYROIDUPTAKEMEASDOSE:
			i = AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM];
			break;

		case SET_HTML_WELLTHYROIDUPTAKEMEASPAT:
			i = AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM];
			break;

		case SET_HTML_WELLTHYROIDUPTAKEENTERNORMAL:
			i = AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERNORMAL_HTM];
			break;

		case SET_HTML_WELLRBCSURVIVAL:
			i = AmuletHTMLIndex[WELLRBCSURVIVAL_HTM];
			break;

		case SET_HTML_ADDEDITRBCSURVIVALTEST:
			i = AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM];
			break;

		case SET_HTML_WELLRBCSURVIVALTEST:
			i = AmuletHTMLIndex[WELLRBCSURVIVALTEST_HTM];
			break;

		case SET_HTML_WELLRBCSURVIVALMEASUREMENT:
			i = AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM];
			break;

		case SET_HTML_WELLRBCSURVIVALENTERNORMAL:
			i = AmuletHTMLIndex[WELLRBCSURVIVALENTERNORMAL_HTM];
			break;

		case SET_HTML_AUTOLINEARITY:
			i = AmuletHTMLIndex[AUTOLINEARITY_HTM];
			break;

		case SET_HTML_AUTOLINEARITYTEST:
			i = AmuletHTMLIndex[AUTOLINEARITYTEST_HTM];
			break;

		case SET_HTML_GENERIC_YES_NO:
			i = AmuletHTMLIndex[GENERICYESNO_HTM];
			break;

		case SET_HTML_AUTOLINEARITYSEARCH:
			i = AmuletHTMLIndex[AUTOLINEARITYSEARCH_HTM];
			break;

		case SET_HTML_REPORTS:
			i = AmuletHTMLIndex[CHAMBERREPORTS_HTM];
			break;

		case SET_HTML_CHAMBER_SEARCH:
			i = AmuletHTMLIndex[CHAMBERSEARCH_HTM];
			break;

		case SET_HTML_CHAMBERDAILYTESTVIEW:
			i = AmuletHTMLIndex[CHAMBERDAILYTESTVIEW_HTM];
			break;

		case SET_HTML_CHAMBERZEROTESTVIEW:
			i = AmuletHTMLIndex[CHAMBERZEROTESTVIEW_HTM];
			break;

		case SET_HTML_CHAMBERBACKGROUNDTESTVIEW:
			i = AmuletHTMLIndex[CHAMBERBACKGROUNDTESTVIEW_HTM];
			break;

		case SET_HTML_CHAMBERVOLTAGETESTVIEW:
			i = AmuletHTMLIndex[CHAMBERVOLTAGETESTVIEW_HTM];
			break;

		case SET_HTML_CHAMBERACCURACYTESTVIEW:
			i = AmuletHTMLIndex[CHAMBERACCURACYTESTVIEW_HTM];
			break;

		case SET_HTML_CHAMBERAUTOCONSTANCYTEST:
			i = AmuletHTMLIndex[CHAMBERAUTOCONSTANCYTESTVIEW_HTM];
			break;

		case SET_HTML_CHAMBERHALFLIFECALC:
			i = AmuletHTMLIndex[HALFLIFECALCULATOR_HTM];
			break;
	}

	if((i==AmuletHTMLIndex[MAINSCREEN_HTM]) || (i==AmuletHTMLIndex[WELLMAINSCREEN_HTM]) || (i==AmuletHTMLIndex[BETAMAINSCREEN_HTM]))
	{
		if((g_csec_tstamp - m_ulLastMainScreenStamp) > 200){
			SetAmuletHTML(i);
			ClearPageStack();
			PushPageStack(i);
			beep_amulet();
		}
		m_ulLastMainScreenStamp = g_csec_tstamp;
	}else{
		if(CurrentPageStack() != i){
			SetAmuletHTML(i);
			PushPageStack(i);
			beep_amulet();
		}
	}
}

/**
 * \details Push a Screen index into the Page Stack
 * \param ucPage Screen index
 */
void PushPageStack(unsigned char ucPage){
	if(m_ucPageStackIndex < PAGE_STACK_SIZE){
		m_ucPageStack[m_ucPageStackIndex] = ucPage;
		m_ucPageStackIndex++;
	}
}

/**
 * \details Pop a Screen index off the Page Stack
 * \returns Page Screen index pop off the Page Stack
 */
unsigned char PopPageStack(void){
	unsigned char ucReturn;

	if(m_ucPageStackIndex > 1){
		m_ucPageStackIndex--;
		ucReturn = m_ucPageStack[m_ucPageStackIndex];
	}else if(m_ucPageStackIndex == 1) ucReturn = m_ucPageStack[0];
	else ucReturn = AmuletHTMLIndex[MAINSCREEN_HTM];

	return ucReturn;
}

/**
 * \details Get the current Screen index on the top of the Page Stack
 * \returns Page Screen index at the top of the Page Stack
 */
unsigned char CurrentPageStack(void){
	unsigned char ucReturn;

	if(m_ucPageStackIndex>0) ucReturn = m_ucPageStack[m_ucPageStackIndex - 1];
	else ucReturn = AmuletHTMLIndex[MAINSCREEN_HTM];

	return ucReturn;
}

/**
 * \details Get the Screen index below the screen on the top of the Page Stack
 * \returns Page Screen index below the top of the Page Stack
 */
unsigned char PreviousPageStack(void){
	unsigned char ucReturn;

	if(m_ucPageStackIndex>1) ucReturn = m_ucPageStack[m_ucPageStackIndex - 2];
	else ucReturn = AmuletHTMLIndex[MAINSCREEN_HTM];

	return ucReturn;
}

/**
 * \details Get the Screen index at the bottom of the Page Stack
 * \returns Screen index at the bottom of the Page Stack
 */
unsigned char BasePageStack(void){
	return m_ucPageStack[0];
}
/**
 * \details Clear the Page Stack
 * \returns None
 */
void ClearPageStack(void){
	m_ucPageStackIndex = 0;
}
/**
 * \details Show the previous page on the Page Stack
 * \returns None
 */
void SetAmuletBackHTML(void){
	unsigned char backpage;

	PopPageStack();
	if(m_usBackClear){
		backpage = (m_usBackClear >> 8) & 0xFF;
		if(backpage == CurrentPageStack()){
			m_ucClear = m_usBackClear & 0xFF;
			m_usBackClear = 0;
		}
	}
	SetAmuletHTML(CurrentPageStack());
	beep_amulet();
}
/**
 * \details Show the Home Screen
 * \returns None
 */
void SetAmuletHomeHTML(void){
	unsigned char basepage;
	unsigned char currentpage;

	currentpage = CurrentPageStack();
	basepage = BasePageStack();

	if(currentpage == basepage){
		ClearPageStack();
		PushPageStack(basepage);
	}else{
		if(basepage == AmuletHTMLIndex[MAINSCREEN_HTM] || basepage == AmuletHTMLIndex[WELLMAINSCREEN_HTM] || basepage == AmuletHTMLIndex[BETAMAINSCREEN_HTM]){
			SetAmuletHTML(basepage);
			ClearPageStack();
			PushPageStack(basepage);
		}
	}
}
/**
 * \details Create a string with the Nuclide full name and halflife
 * \param ucNuclideID Nuclide index
 * \param acMsg Pointer to space allocated for Null terminated Nuclide string
 * \returns None
 */
void GetExtendedNuclideString(unsigned char ucNuclideID, char *acMsg){
	char acHalflife[30];
	char acFullName[15];
	NUCDATA output_nuc;
	char msg[8];

	NuclideData_getNuclide(ucNuclideID, &output_nuc);
	strncpy(acMsg, output_nuc.name, 7);
	trim_and_shrink(acMsg);

	//strcat(acMsg, " (");
	strcat(acMsg, "\n");

	NuclideData_getFullName(ucNuclideID, acFullName);
	strcat(acMsg, acFullName);

	//strcat(acMsg, ") ");
	strcat(acMsg, " ");

	sprintf(acHalflife, "%.2f", output_nuc.halflife);
	switch(output_nuc.hlunit){
		case SEC:
			//strcat(acHalflife, " sec");
			get_amulet_message(L_SEC2, msg);    // " sec"
			strcat(acHalflife, msg);
			break;

		case MIN:
			//strcat(acHalflife, " min");
			get_amulet_message(L_MIN2, msg);    // " min"
			strcat(acHalflife, msg);
			break;

		case HOUR:
			//strcat(acHalflife, " hr");
			get_amulet_message(L_HR2, msg);    // " hr"
			strcat(acHalflife, msg);
			break;

		case DAY:
			//strcat(acHalflife, " day");
			get_amulet_message(L_DAY2, msg);    // " day"
			strcat(acHalflife, msg);
			break;

		case YEAR:
			if(output_nuc.halflife > 1.0e+6){
				sprintf(acHalflife, "%.2f", output_nuc.halflife / 1.0e+6);
				//strcat(acHalflife, " Myr");
				get_amulet_message(L_MYR, msg);    // " Myr"
				strcat(acHalflife, msg);
			}else{
				//strcat(acHalflife, " yr");
				get_amulet_message(L_YR2, msg);    // " yr"
				strcat(acHalflife, msg);
			}
			break;
	}

	strcat(acMsg, acHalflife);
}
/**
 * \details Create a string with the Nuclide full name and halflife, and structure with Nuclide Information
 * \param ucNuclideID Nuclide index
 * \param output_nuc Pointer to structure with Nuclide Information
 * \param acMsg Pointer to space allocated for Null terminated Nuclide string
 * \returns None
 */
void GetExtendedNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg){
	char acHalflife[30];
	char acFullName[15];
	char msg[8];

	NuclideData_getNuclide(ucNuclideID, output_nuc);
	strncpy(acMsg, output_nuc->name, 7);
	trim_and_shrink(acMsg);
	strcat(acMsg, " (");

	NuclideData_getFullName(ucNuclideID, acFullName);
	strcat(acMsg, acFullName);

	strcat(acMsg, ") ");

	sprintf(acHalflife, "%.2f", output_nuc->halflife);
	switch(output_nuc->hlunit){
		case SEC:
			//strcat(acHalflife, " sec");
			get_amulet_message(L_SEC2, msg);    // " sec"
			strcat(acHalflife,msg);
			break;

		case MIN:
			//strcat(acHalflife, " min");
			get_amulet_message(L_MIN2,msg);    // " min"
			strcat(acHalflife,msg);
			break;

		case HOUR:
			//strcat(acHalflife, " hr");
			get_amulet_message(L_HR2,msg);    // " hr"
			strcat(acHalflife,msg);
			break;

		case DAY:
			//strcat(acHalflife, " day");
			get_amulet_message(L_DAY2,msg);    // " day"
			strcat(acHalflife,msg);
			break;

		case YEAR:
			if(output_nuc->halflife > 1.0e+6){
				sprintf(acHalflife, "%.2f", output_nuc->halflife / 1.0e+6);
				//strcat(acHalflife, " Myr");
				get_amulet_message(L_MYR,msg);    // " Myr"
				strcat(acHalflife,msg);
			}else{
				//strcat(acHalflife, " yr");
				get_amulet_message(L_YR2,msg);    // " yr"
				strcat(acHalflife,msg);
			}
			break;
	}
	strcat(acMsg, acHalflife);
}
/**
 * \details Create a string with short Nuclide string and structure with Nuclide Information
 * \param ucNuclideID Nuclide index
 * \param output_nuc Pointer to structure with Nuclide Information
 * \param acMsg Pointer to space allocated for Null terminated Nuclide string
 * \returns None
 */
void GetShortNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg){
	char acFullName[15];

	NuclideData_getNuclide(ucNuclideID, output_nuc);
	strncpy(acMsg, output_nuc->name, 7);
	trim_and_shrink(acMsg);
	strcat(acMsg, " ");

	NuclideData_getFullName(ucNuclideID, acFullName);
	strcat(acMsg, acFullName);
}
/**
 * \details Create a string with date and time
 * \param dtmDateTime Date and time variable
 * \param acMsg Pointer to space allocated for Null terminated date and time string
 * \returns None
 */
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg){
	char acMsg2[50];

	dateout(acMsg, dtmDateTime, 4);
	timeout(acMsg2, dtmDateTime);
	strcat(acMsg, " ");
	strcat(acMsg, acMsg2);
}
/**
 * \details Create a string with date and time
 * \param dtmDateTime Date and time variable
 * \param acMsg Pointer to space allocated for Null terminated date and time string
 * \returns None
 */
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg){
	char acMsg2[50];

	dateout(acMsg, dtmDateTime, 4);
	timeoutsec(acMsg2, dtmDateTime);
	strcat(acMsg, " ");
	strcat(acMsg, acMsg2);
}
/**
 * \details Create a string with date and time based on language setting
 * \param dtmDateTime Date and time variable
 * \param acMsg Pointer to space allocated for Null terminated date time string
 * \returns None
 */
void GetExtendedTimeInfoLanguage(time_t *dtmDateTime, char *acMsg){
	char acMsg2[50];

	dateout_language(acMsg, dtmDateTime, 4);
	timeout(acMsg2, dtmDateTime);
	strcat(acMsg, " ");
	strcat(acMsg, acMsg2);
}
/**
 * \details Create a string with date and time based on language setting
 * \param dtmDateTime Date and time variable
 * \param acMsg Pointer to space allocated for Null terminated date time string
 * \returns None
 */
void GetExtendedTimeInfoLanguageSec(time_t *dtmDateTime, char *acMsg){
	char acMsg2[50];

	dateout_language(acMsg, dtmDateTime, 4);
	timeoutsec(acMsg2, dtmDateTime);
	strcat(acMsg, " ");
	strcat(acMsg, acMsg2);
}
/**
 * \details Create string with activity in Curies
 * \param fActivity Activity
 * \param ucUnit Activity Unit
 * \param acMsg Pointer to space allocated for Null terminated activity string
 * \returns None
 */
void GetExtendedActivityInfo(float fActivity, unsigned char ucUnit, char *acMsg){
	char cSys;

	switch(ucUnit){
		case 0:
		case 1:
		case 2:
			cSys = CI;
			break;
		case 3:
		case 4:
		case 5:
			cSys = BQ;
			break;
	}
	format_activity(fActivity, cSys, acMsg);
}
/**
 * \details Create string with Activity in Bq
 * \param fActivity Activity
 * \param ucUnit Activity Unit
 * \param acMsg Pointer to space allocated for Null terminated activity string
 * \returns None
 */
void GetExtendedActivityInfoKBq(float fActivity, unsigned char ucUnit, char *acMsg){
	char cSys;

	switch(ucUnit){
		case 0:
		case 1:
		case 2:
			cSys = CI;
			break;
		case 3:
		case 4:
		case 5:
			cSys = BQ;
			break;
	}
	format_activity_kunit_kbq(fActivity, cSys, acMsg);
}

unsigned char CheckPassword(char *Passwd){
	unsigned char ucReturn;
	double double_value, sqrt_value;
	char key[20], fraction[20];

	if(!strcmp(current.password, Passwd)) ucReturn = 0xFF;
	else{
		if(current.bypass > 0){
			double_value = current.bypass;
			sqrt_value = sqrt(double_value);
			sprintf(key, "%.3f", sqrt_value);
			strcpy(fraction, &(key[strlen(key) - 3]));
			if(!strcmp(fraction, Passwd)){
				strcpy(current.password, fraction);
				DB_WritePassword(current.password);
				ucReturn = 0xFF;
			}else{
				ucReturn = 0;
			}
		}else{
			ucReturn = 0;
		}
	}

	current.bypass = -1;
	return ucReturn;
}
/**
 * \details Write a text file into the data folder
 * \param acFilename Pointer to Null terminated filename
 * \param acData Pointer to Null terminated text data
 * \returns None
 */
void write_string_to_Options_folder(char *acFilename, char *acData){
	//F_FILE *pFileHandle;
	FILINFO fileInfo;
	FIL fileObject;
	UINT bytesWritten;
	char longFileName[100];

	//mmc_change_dir("\\");
	f_chdrive(0);
	f_chdir("/");

	//mmc_change_dir("data");
	if(f_chdir("data") != FR_OK){
		f_mkdir("data");
		f_chdir("/");
		f_chdir("data");
	}

	//if(mmc_file_exists(acFilename)==0) f_delete(acFilename);
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat(acFilename, &fileInfo) == FR_OK) f_unlink(acFilename);

	//pFileHandle = f_open(acFilename, "w");
	f_open(&fileObject, acFilename, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

	//f_write(acData, strlen(acData), 1, pFileHandle);
	f_write(&fileObject, acData, strlen(acData), &bytesWritten);

	//f_close(pFileHandle);
	f_close(&fileObject);
}
/**
 * \details Write a binary file into the data folder
 * \param acFilename Pointer to Null terminated filename
 * \param acData Pointer to binary data
 * \param lFilelength Length of binary data
 * \returns None
 */
void write_binary_to_Options_folder(char *acFilename, char *acData, long int lFilelength){
	//F_FILE *pFileHandle;
	FILINFO fileInfo;
	FIL fileObject;
	UINT bytesWritten;
	char longFileName[100];

	//mmc_change_dir("\\");
	f_chdrive(0);
	f_chdir("/");

	//mmc_change_dir("data");
	if(f_chdir("data") != FR_OK){
		f_mkdir("data");
		f_chdir("/");
		f_chdir("data");
	}

	//if(mmc_file_exists(acFilename)==0) f_delete(acFilename);
	fileInfo.lfname = longFileName;
	fileInfo.lfsize = 100;
	if(f_stat(acFilename, &fileInfo) == FR_OK) f_unlink(acFilename);

	//pFileHandle = f_open(acFilename, "w");
	f_open(&fileObject, acFilename, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

	//f_write(acData, lFilelength, 1, pFileHandle);
	f_write(&fileObject, acData, lFilelength, &bytesWritten);

	//f_close(pFileHandle);
	f_close(&fileObject);
}

/*void write_HotKeys(void){
	short i, j;
	char acNucString[10];
	char acHotKeyData[300];

	acHotKeyData[0] = 0;
	for(j=0;j<2;j++){
		for(i=0; i<8; i++){
			sprintf(acNucString, "%u\n", m_ucHotKeyNuclideID[j][i]);
			strcat(acHotKeyData, acNucString);
		}
	}
	for(j=0;j<2;j++){
		for(i=0; i<20; i++){
			sprintf(acNucString, "%u\n", m_ucHotKeyNuclideID2[j][i]);
			strcat(acHotKeyData, acNucString);
		}
	}
	write_string_to_Options_folder("hotkeys.txt", acHotKeyData);
}*/
/**
 * \details Display Error message box
 * \param title Pointer to Null terminated title string
 * \param errorstring Pointer to Null terminated error message
 * \param showOK True = Show OK button, False = Do not show OK button
 * \returns None
 */
void Amulet_DisplayError(char *title, char *errorstring, bool showOK){
	strcpy(AmuletErrorMsgMenu_title, title);
#ifdef PRINTF_ERRORS
	printf("Amulet_DisplayError(Title) - %s\n", title);
#endif
	strcpy(AmuletErrorMsgMenu_message, errorstring);
#ifdef PRINTF_ERRORS
	printf("Amulet_DisplayError(Msg) - %s\n\n", errorstring);
#endif
	AmuletErrorMsgMenu_showOK = showOK;
	SetAmuletHTML(AmuletHTMLIndex[ERRORMSG_HTM]);
	PushPageStack(AmuletHTMLIndex[ERRORMSG_HTM]);
}
/**
 * \details Display Large Error message box
 * \param title Pointer to Null terminated title string
 * \param errorstring Pointer to Null terminated error message
 * \returns None
 */
void Amulet_DisplayErrorLarge(char *title, char *errorstring){
	strcpy(AmuletErrorMsgLargeMenu_title, title);
	strcpy(AmuletErrorMsgLargeMenu_message, errorstring);
	SetAmuletHTML(AmuletHTMLIndex[ERRORMSGLARGE_HTM]);
	PushPageStack(AmuletHTMLIndex[ERRORMSGLARGE_HTM]);
}
/**
 * \details Display Error message box (type 2)
 * \param title Pointer to Null terminated title string
 * \param errorstring Pointer to Null terminated error message
 * \param showOK True = Show OK button, False = Do not show OK button
 * \returns None
 */
void Amulet_DisplayError2(char *title, char *errorstring, bool showOK){
	strcpy(AmuletErrorMsgMenu_title, title);
	strcpy(AmuletErrorMsgMenu_message, errorstring);
	AmuletErrorMsgMenu_showOK = showOK;
	//SetAmuletHTML(ERRORMSG_HTM);
	PushPageStack(AmuletHTMLIndex[ERRORMSG_HTM]);
	PushPageStack(AmuletHTMLIndex[ERRORMSG_HTM]);
}
/**
 * \details Display Notification message box
 * \param title Pointer to Null terminated title string
 * \param notificationstring Pointer to Null terminated notification message
 * \param showOK True = Show OK button, False = Do not show OK button
 * \returns None
 */
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK){
	strcpy(AmuletNotificationMsgMenu_title, title);
	strcpy(AmuletNotificationMsgMenu_message, notificationstring);
	AmuletNotificationMsgMenu_showOK = showOK;
	SetAmuletHTML(AmuletHTMLIndex[NOTIFICATIONMSG_HTM]);
	PushPageStack(AmuletHTMLIndex[NOTIFICATIONMSG_HTM]);
}
/**
 * \details Display Warning message box
 * \param title Pointer to Null terminated title string
 * \param warningstring Pointer to Null terminated warning message
 * \param showOK True = Show OK button, False = Do not show OK button
 * \returns None
 */
void Amulet_DisplayWarning(char *title, char *warningstring, bool showOK){
	strcpy(AmuletWarningMsgMenu_title, title);
	strcpy(AmuletWarningMsgMenu_message, warningstring);
	AmuletWarningMsgMenu_showOK = showOK;
	SetAmuletHTML(AmuletHTMLIndex[WARNINGMSG_HTM]);
	PushPageStack(AmuletHTMLIndex[WARNINGMSG_HTM]);
}

void Amulet_DisplayErrorCode(short errcode){
	char message[101];

	message[0] = 0;
	switch(errcode){
		case ERR_MCA_NOT_INSTALLED:
			break;

		case ERR_MCA_EE_SIG_MISSING:
			strcpy(message, "MCA EEPROM was not programmed");
			break;

		case ERR_MCA_PROG_LOAD_FAILURE:
			strcpy(message, "MCA program failed to load");
			break;

		case ERR_MCA_PROG_NOT_RUNNING:
			strcpy(message, "MCA program failed to start");
			break;

		case ERR_MCA_DETECTOR_NOT_ATTACHED:
			strcpy(message, "Detector not attached to MCA");
			break;

		case ERR_MCA_UNKNOWN_DETECTOR:
			strcpy(message, "Unable to determine MCA detector");
			break;

		case ERR_MCA_COMMAND_TIMEOUT:
			strcpy(message, "MCA Command failed. (timeout)");
			break;
	}

	Amulet_DisplayError("System Error", message,TRUE);
}

void stringtofixed(char *output, const char *input, short length){
	short index;
	bool zerofound;

	zerofound = FALSE;
	for(index=0; index<length; index++){
		if(zerofound){
			*output = 32;
		}else{
			if((*input)==0){
				zerofound = TRUE;
				*output = 32;
			}else{
				*output = *input;
			}
			input++;
		}
		output++;
	}
}

double time_ttojd(time_t time){
	double returnvalue;
	struct tm tmt;
	DateTime datetime;

	gmtime_r(&time, &tmt);
	datetime.iJD = 0;
	datetime.Y = tmt.tm_year + 1900;
	datetime.M = tmt.tm_mon + 1;
	datetime.D = tmt.tm_mday;
	datetime.h = tmt.tm_hour;
	datetime.m = tmt.tm_min;
	datetime.s = tmt.tm_sec;
	datetime.validJD = 0;
	datetime.validYMD = 1;
	datetime.validHMS = 1;
	datetime.validTZ = 0;
	computeJD(&datetime);

	returnvalue = datetime.iJD;
	returnvalue /= 86400000.0;

	return returnvalue;
}

void strin(char *result, char *input){
	strncpy(result, input, strlen(input));
}

void get_amulet_strin(short msg_num, char *result){
	char message[100];

	get_amulet_message(msg_num, message);
	strncpy(result, message, strlen(message));
}
/*

void accuracy_menu(void) {
	short ch_num = current.main_chamber;
	char acMsg[100];
	char buffer[100];
	static short ciso;
	short i;
	static char acSourceFinished[5];
	static short iCurrentSource;
	short nuc_index;
	float diff;
	bool error;
	static bool printer_pressed;
	short iret;

	switch(m_iPhase) {
		case PHASE_ACCURACY_WAIT_FOR_AMULET_INIT:
			// We can begin after the Amulet memory is initialized
			if(current.num_chambers>1){
				sprintf(acMsg, "Ch: %d", ch_num + 1);
				SetAmuletString(9, acMsg);
				SetAmuletByte(21, 0xFF);
			}
			if(m_ucClear==28){
				AccuracyMenu_ucFinished = 0;
				printer_pressed = FALSE;
				m_iPhase = PHASE_ACCURACY_BEFORE_INIT;
				m_ucClear = 0;
			}else{
				if(printer_pressed){
					printer_pressed = FALSE;
					for(i=0; i<num_daily; i++){
						sprintf(acMsg, "%d) ", i+1);
						strcpy(buffer, acc_data[i].nucname);
						trim(buffer);
						strcat(acMsg, buffer);
						strcat(acMsg, "\ns/n:");
						strcpy(buffer, acc_data[i].sn);
						trim(buffer);
						strcat(acMsg, buffer);
						SetAmuletString((2*i)+100, acMsg);
					}

					for(i=0; i<num_daily; i++){
						SetAmuletByte(101+i, 0xFF);

						if(acSourceFinished[i] == 0){
							SetAmuletByte(121 + i, 0xFF);
						}else{
							acc_meas_string(acMsg, i, ch_num, 1, &error);
							if(error) SetAmuletByte(116+i, 0xFF);
							else SetAmuletByte(111+i, 0xFF);
							SetAmuletString((2 * i) + 110, acMsg);
							SetAmuletByte(106+i, 0xFF);
							//if(acc_data[i].const_source && !error && (current.printer!=NONE_PRINTER)) SetAmuletByte(141 + i, 0xFF);
							if(acc_data[i].const_source && !error) SetAmuletByte(141 + i, 0xFF);
						}
					}
					if(current.printer!= NONE_PRINTER) SetAmuletByte(146, 0xFF);

					m_iPhase = PHASE_ACCURACY_FINISHED;
				}else{
					m_iPhase = PHASE_ACCURACY_BEFORE_INIT;
				}
			}
			break;

		case PHASE_ACCURACY_BEFORE_INIT:
			if(current.num_chambers>0){
				num_daily = get_num_sources(AmuletAccuracy_daily);
				measurement[ch_num].future.dosetime = NO_TIME;
				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);

				if(num_daily == 0){
					SetAmuletString(100, "1) NO SOURCE DATA");
					SetAmuletByte(101, 0xFF);
					m_iPhase = PHASE_ACCURACY_FINISHED;
				}else{
					ciso = get_const_source2();
					get_acc_data(ciso, AmuletAccuracy_daily, ch_num);
					for (i=0; i<5; i++) acSourceFinished[i] = 0;

					for(i=0; i<num_daily; i++){
						sprintf(acMsg, "%d) ", i+1);
						strcpy(buffer, acc_data[i].nucname);
						trim(buffer);
						strcat(acMsg, buffer);
						strcat(acMsg, "\ns/n:");
						strcpy(buffer, acc_data[i].sn);
						trim(buffer);
						strcat(acMsg, buffer);
						SetAmuletString((2*i)+100, acMsg);
					}

					m_iPhase = PHASE_ACCURACY_AFTER_INIT;
				}
			}
			break;

		case PHASE_ACCURACY_AFTER_INIT:
			if(current.num_chambers>0){
				AccuracyMenu_ucFinished = 0xFF;
				for(i=0; i<num_daily; i++){
					SetAmuletByte(101+i, 0xFF);

					if(acSourceFinished[i] == 0){
						AccuracyMenu_ucFinished = 0;
						SetAmuletByte(121 + i, 0xFF);
					}else{
						acc_meas_string(acMsg, i, ch_num, 1, &error);
						if(error) SetAmuletByte(116+i, 0xFF);
						else SetAmuletByte(111+i, 0xFF);
						SetAmuletString((2 * i) + 110, acMsg);
						SetAmuletByte(106+i, 0xFF);
						//if(acc_data[i].const_source && !error && (current.printer != NONE_PRINTER)) SetAmuletByte(141 + i, 0xFF);
						if(acc_data[i].const_source && !error) SetAmuletByte(141 + i, 0xFF);
					}
				}

				if(AccuracyMenu_ucFinished){
					if(current.printer != NONE_PRINTER) SetAmuletByte(146, 0xFF);
					m_iPhase = PHASE_ACCURACY_FINISHED;
				}else{
					m_iPhase = PHASE_ACCURACY_DURING_ACCURACY;
				}
			}
			break;

		case PHASE_ACCURACY_DURING_ACCURACY:
			// Wait for User to press Measure button
			break;

		case PHASE_ACCURACY_MEASURE_SOURCE1:
			iCurrentSource = 0;
			m_iPhase = PHASE_ACCURACY_SET_NUCLIDE;
			break;

		case PHASE_ACCURACY_MEASURE_SOURCE2:
			iCurrentSource = 1;
			m_iPhase = PHASE_ACCURACY_SET_NUCLIDE;
			break;

		case PHASE_ACCURACY_MEASURE_SOURCE3:
			iCurrentSource = 2;
			m_iPhase = PHASE_ACCURACY_SET_NUCLIDE;
			break;

		case PHASE_ACCURACY_MEASURE_SOURCE4:
			iCurrentSource = 3;
			m_iPhase = PHASE_ACCURACY_SET_NUCLIDE;
			break;

		case PHASE_ACCURACY_MEASURE_SOURCE5:
			iCurrentSource = 4;
			m_iPhase = PHASE_ACCURACY_SET_NUCLIDE;
			break;

		case PHASE_ACCURACY_SET_NUCLIDE:
			if(current.num_chambers>0){
				beep_amulet();
				erase_screen();
				nuc_index = acc_data[iCurrentSource].nuc_index;
				set_nuclide_data(nuc_index, ch_num);
				disp_nuclide();
				disp_pract(iCurrentSource, ch_num);

				SetAmuletByte(106+iCurrentSource, 0xFF);
				SetAmuletByte(131+iCurrentSource, 0xFF);
				if(acc_data[iCurrentSource].const_source){
					memcpy(&(chamb_vals[8]), &(chamb_vals[ch_num]), sizeof(CHAMBERVALS));
					memcpy(&(chamber[8]), &(chamber[ch_num]), sizeof(CHAMBER));
				}

				m_iPhase = PHASE_ACCURACY_MEASURE_NUCLIDE;
			}
			break;

		case PHASE_ACCURACY_MEASURE_NUCLIDE:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					if(acc_data[iCurrentSource].const_source) memcpy(&(measurement[8]), &(measurement[ch_num]), sizeof(MEASUREMENT));
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					diff = calc_dev(iCurrentSource, ch_num);
					dispdev(diff);
					acMsg[0] = 0;
					acc_meas_string(acMsg, iCurrentSource, ch_num, 1, &error);
					if(error) SetAmuletByte(116+iCurrentSource, 0xFF);
					else SetAmuletByte(111+iCurrentSource, 0xFF);
					SetAmuletString((2 * iCurrentSource) + 110, acMsg);
				}
			}
			break;

		case PHASE_ACCURACY_ACCEPT_PRESSED:
			if(current.num_chambers>0){
				beep_amulet();
				acSourceFinished[iCurrentSource] = 0xFF;
				m_iPhase = PHASE_ACCURACY_AFTER_INIT;
			}
			break;

		case PHASE_ACCURACY_PRINT:
			m_iPhase = PHASE_ACCURACY_FINISHED;
			beep_amulet();
			printer_pressed = TRUE;

			if(AmuletAccuracy_daily){
				iret = prdaily(AmuletDailyMenu_okdata, 0);
			}else{
				iret = Amulet_printAccuracyAndConstancy(FALSE);
			}
			if(iret==1) dosig(current.printer);

			SetAmuletByte(146, 0xFF);
			break;

		case PHASE_ACCURACY_FINISHED:
			printer_pressed = FALSE;
			break;

		default:
			break;
	}
}

 */

//void linearity_menu(void) {
	/*short ch_num = current.main_chamber;
	short i;
	char acMsg[100];
	char acMsg2[100];
	char acMsg3[100];
	float slope;
	float meas0;
	float var;
	//float act;
    float act1;
    float corr_act;
    float mean;
    //short kun;
    //unsigned long int temp_msec;
	static LINDEF def;
	static short num_meas;
	static short num_test;
	static char nucname[8];
	static short nuc_index;
	static NUCDATA nucdata;
	static float factors[12];
	static time_t time0, time1;
	static LINMEAS linmeas[8];
	static CALMEAS calmeas[12];
	static unsigned long wait;
	float predicted_act[12];
	static char factor[8][25];
	static char ratio[8][25];
	static char caliresult[12][25];
	static char calivar[12][25];
	static bool printer_pressed;
	short ch_type;

	switch(m_iPhase) {
		case PHASE_LINEARITY_PRE_INIT:
			if(current.num_chambers>1){
				sprintf(acMsg, "Ch: %d", ch_num + 1);
				SetAmuletString(9, acMsg);
				SetAmuletByte(21, 0xFF);
			}
			EE_READ(lindef, (uchar *)&def);

			switch(def.type) {
				case -1:
					SetAmuletString(159, "Linearity");
					SetAmuletString(160, "Must Define");
					SetAmuletString(163, "Linearity Test");
					SetAmuletByte(160,0xFF);
					m_ucClear = 0;
					m_iPhase = PHASE_LINEARITY_FINISH;
					break;

				case STANDARD:
					ch_type = chamber_type(ch_num);
					if(NuclideData_getEffectiveResponse(def.nuc_index, ch_type) == 0.0){

						sprintf(acMsg, "Unable to find nuclide");
						SetAmuletString(160, acMsg);

						SetAmuletString(163, "calibration number");
						if(ch_type == R_CHAMB) SetAmuletString(166, "for R Chamber");
						else SetAmuletString(166, "for PET Chamber");
						SetAmuletByte(160, 0xFF);
						m_iPhase = PHASE_LINEARITY_FINISH;
					}else{
						m_iPhase = PHASE_LINEARITY_STANDARD_INIT;
					}
					SetAmuletString(159, "Linearity, Std");
					m_ucClear = 0;
					break;

				case LINEATOR:
					SetAmuletString(159, "Linearity, Lineator");
					m_iPhase = PHASE_LINEARITY_LINEATOR_INIT;
					break;

				case CALICHECK:
					SetAmuletString(159, "Linearity, Calicheck");
					m_iPhase = PHASE_LINEARITY_CALICHECK_INIT;
					break;

				default:
					m_ucClear = 0;
					break;
			}
			break;

		case PHASE_LINEARITY_STANDARD_INIT:
			if(current.num_chambers>0){
				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);
				nuc_index = def.nuc_index;

				set_nuclide_data(nuc_index, ch_num);
				NuclideData_getNuclide(nuc_index, &nucdata);
				strcpy(nucname, nucdata.name);

				num_meas = def.num;
				num_test = 1;
				for(i=0; i<12; i++){
					if(standlin[ch_num][i].meas_flag != 1) break;
					num_test++;
				}

				if(num_test > 1){
					SetAmuletByte(161, 0xFF);
					for(i=0;i<12;i++) predicted_act[i] = 0;
					predicted_standard_activity(predicted_act, num_test - 1, &slope, &nucdata);
				}

				SetAmuletString(160, "#");
				SetAmuletString(161, "Measured");
				SetAmuletString(162, "Var %");
				SetAmuletByte(182, 0xFF);
				for(i=0; i<num_test-1; i++){
					sprintf(acMsg, "%d)", i+1);
					dateout(acMsg2, &standlin[ch_num][i].date, 2);
					strcat(acMsg, acMsg2);
					strcat(acMsg, "; ");
					timeout(acMsg2, &standlin[ch_num][i].date);
					strcat(acMsg, acMsg2);
					SetAmuletString(3*i+163, acMsg);
					strcpy(acMsg, standlin[ch_num][i].actstr);
					trim(acMsg);
					SetAmuletString(3*i+164, acMsg);
					//Removed: Units Conversion is not needed
					//meas0 = convact(standlin[ch_num][i].meas, standlin[ch_num][i].syst);
					meas0 = standlin[ch_num][i].meas;
					var = 100. * (meas0 - predicted_act[i]) / predicted_act[i];
					sprintf(acMsg, "%6.2f", var);
					trim(acMsg);
					SetAmuletString(3*i+165, acMsg);
					SetAmuletByte(i+183, 0xFF);
				}

				SetAmuletByte(160, 0xFF);
				if (num_test <= num_meas) SetAmuletByte(num_test + 162, 0xFF);

				m_iPhase = PHASE_LINEARITY_STANDARD_WAIT_FULL_SCREEN;
			}
			break;

		case PHASE_LINEARITY_STANDARD_WAIT_FULL_SCREEN:
			break;

		case PHASE_LINEARITY_STANDARD_AFTER_FULL_SCREEN:
			if(current.num_chambers>0){
				SetAmuletWord(255, 532);
				SetAmuletWord(254, 39 * (num_test) + 41);
				SetAmuletWord(253, 532);
				SetAmuletWord(252, 39 * (num_test) + 41);

				if(num_test == 1){
					m_iPhase = PHASE_LINEARITY_STANDARD_MEASURE_ACCEPTED;
				}else if (num_test > num_meas){
					num_test--;
					if(current.printer!=NONE_PRINTER) SetAmuletByte(100, 0xFF);
					m_iPhase = PHASE_LINEARITY_FINISH;
				}else{
					for(i=num_test; i<=num_meas; i++){
						LinearElapsedTime(i - 1, &def, standlin[ch_num][0].date, acMsg, acMsg2);
						sprintf(acMsg3, "%d)", i);
						strcat(acMsg3, acMsg2);
						SetAmuletString(3*(i - 1)+163, acMsg3);
						SetAmuletString(3*(i - 1)+164, acMsg);
						SetAmuletByte(i + 162, 0xFF);
						SetAmuletByte(i + 182, 0xFF);
						delayloop(20);
					}
					SetAmuletByte(175, 0xFF);
					m_iPhase = PHASE_LINEARITY_STANDARD_WAIT_FOR_MEASURE;
				}
			}
			break;

		case PHASE_LINEARITY_STANDARD_WAIT_FOR_MEASURE:
			// Wait for user to press Measure button
			break;

		case PHASE_LINEARITY_STANDARD_MEASURE_ACCEPTED:
			if(current.num_chambers>0){
				if(num_test == 1){
					SetAmuletString(163, "First Measurement:");
				}else{
					for(i=0;i<12;i++) predicted_act[i] = 0;
					read_clock(&standlin[ch_num][num_test-1].date);
					standlin[ch_num][num_test-1].meas = nucdecay(standlin[ch_num][0].meas, standlin[ch_num][0].date, standlin[ch_num][num_test-1].date, nucdata.halflife, nucdata.hlunit);
					predicted_standard_activity(predicted_act, num_test, &slope, &nucdata);
					// Locked: CI
					//format_activity(predicted_act[num_test - 1], current.system, acMsg);
					//format_activity(predicted_act[num_test - 1], CI, acMsg);
					format_activity_system(predicted_act[num_test - 1], acMsg);
					trim(acMsg);
					strcpy(acMsg2, "Calc: ");
					strcat(acMsg2, acMsg);
					SetAmuletString(3*(num_test - 1) + 163, acMsg2);
				}
				SetAmuletByte(177, 0xFF);
				m_iPhase = PHASE_LINEARITY_STANDARD_WAIT_FOR_ACCEPT;
			}
			break;

		case PHASE_LINEARITY_STANDARD_WAIT_FOR_ACCEPT:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					// Changed:
					//act = measurement[ch_num].act;
					//kun = measurement[ch_num].kun;
					//act1 = act / unitfact[kun - 1];
					act1 = measurement[ch_num].act0;

					standlin[ch_num][num_test - 1].meas = act1;
					// Locked: CI
					//standlin[ch_num][num_test - 1].syst = current.system;
					// Removed:
					//standlin[ch_num][num_test - 1].syst = CI;
					// Locked: CI
					//format_activity(act1, current.system, standlin[ch_num][num_test - 1].actstr);
					//format_activity(act1, CI, standlin[ch_num][num_test - 1].actstr);
					format_activity_system(act1, standlin[ch_num][num_test - 1].actstr);
					read_clock(&standlin[ch_num][num_test - 1].date);
					strcpy(acMsg, standlin[ch_num][num_test - 1].actstr);
					trim(acMsg);
					strcpy(acMsg2, chamber[ch_num].nucdata.name);
					trim(acMsg2);
					strcat(acMsg2, " ");
					strcat(acMsg2, acMsg);
					SetAmuletString(3*(num_test - 1) + 164, acMsg2);
				}
			}
			break;

		case PHASE_LINEARITY_STANDARD_ACCEPTED:
			if(current.num_chambers>0){
				standlin[ch_num][num_test - 1].meas_flag = 1;
				sprintf(acMsg, "%d)", num_test);
				dateout(acMsg2, &standlin[ch_num][num_test-1].date, 2);
				strcat(acMsg, acMsg2);
				strcat(acMsg, "; ");
				timeout(acMsg2, &standlin[ch_num][num_test-1].date);
				strcat(acMsg, acMsg2);
				SetAmuletString(3 * (num_test - 1) + 163, acMsg);

				if(num_test == 1){
					strcpy(acMsg, "0.00");
					SetAmuletString(3*(num_test-1)+165, acMsg);
					SetAmuletByte(183, 0xFF);
					for(i=num_test+1; i<=num_meas; i++){
						LinearElapsedTime(i - 1, &def, standlin[ch_num][0].date, acMsg, acMsg2);
						sprintf(acMsg3, "%d)", i);
						strcat(acMsg3, acMsg2);
						SetAmuletString(3*(i - 1)+163, acMsg3);
						SetAmuletString(3*(i - 1)+164, acMsg);
						SetAmuletByte(i + 162, 0xFF);
						SetAmuletByte(i + 182, 0xFF);
						delayloop(20);
					}
					SetAmuletByte(161, 0xFF);
				}else{
					predicted_standard_activity(predicted_act, num_test, &slope, &nucdata);
					// Removed: Units conversion is not necessary
					//meas0 = convact(standlin[ch_num][num_test-1].meas, standlin[ch_num][num_test-1].syst);
					meas0 = standlin[ch_num][num_test-1].meas;
					var = 100. * (meas0 - predicted_act[num_test-1]) / predicted_act[num_test-1];
					sprintf(acMsg, "%6.2f", var);
					trim(acMsg);
					SetAmuletString(3*(num_test-1)+165, acMsg);
				}

				if(num_test==num_meas){
					//if(current.printer!=NONE_PRINTER) SetAmuletByte(100, 0xFF);
					m_iPhase = PHASE_LINEARITY_PRE_INIT;
				}else{
					m_iPhase = PHASE_LINEARITY_FINISH;
				}
			}
			break;

		case PHASE_LINEARITY_LINEATOR_INIT:
			if(current.num_chambers>0){
				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);
				for(i=0; i<8; i++){
					EE_READ(lindef.lin_cal_factors[i], (uchar *) &factors[i]);
				}

				num_meas = def.num;
				nuc_index = def.nuc_index;
				set_nuclide_data(nuc_index, ch_num);
				NuclideData_getNuclide(nuc_index, &nucdata);
				strcpy(nucname, nucdata.name);

				SetAmuletString(160, "#");
				SetAmuletString(161, "Factor");
				SetAmuletString(162, "% Ratio");
				SetAmuletByte(182, 0xFF);

				SetAmuletString(163, "1) Tube 1");
				SetAmuletByte(183, 0xFF);
				SetAmuletString(166, "2) Tube 1+2");
				SetAmuletByte(184, 0xFF);
				SetAmuletString(169, "3) Tube 1+3");
				SetAmuletByte(185, 0xFF);
				SetAmuletString(172, "4) Tube 1+2,3");
				SetAmuletByte(186, 0xFF);
				SetAmuletString(175, "5) Tube 1+4");
				SetAmuletByte(187, 0xFF);
				SetAmuletString(178, "6) Tube 1+2,4");
				SetAmuletByte(188, 0xFF);
				SetAmuletString(181, "7) Tube 1+3,4");
				SetAmuletByte(189, 0xFF);
				SetAmuletString(184, "8) Tube 1+2,3,4");
				SetAmuletByte(190, 0xFF);
				SetAmuletByte(159, 0xFF);
				num_test = 1;
				if(m_ucClear == 30){
					m_ucClear = 0;
					printer_pressed = FALSE;
					m_iPhase = PHASE_LINEARITY_LINEATOR_WAIT_FULL_SCREEN;
				}else{
					if(printer_pressed){
						printer_pressed = FALSE;
						wait = g_csec_tstamp + 20;
						m_iPhase = PHASE_LINEARITY_LINEATOR_DELAY_BEFORE_FINISH;
					}else{
						m_iPhase = PHASE_LINEARITY_LINEATOR_WAIT_FULL_SCREEN;
					}
				}
			}
			break;

		case PHASE_LINEARITY_LINEATOR_WAIT_FULL_SCREEN:
			break;

		case PHASE_LINEARITY_LINEATOR_AFTER_FULL_SCREEN:
			if(current.num_chambers>0){
				SetAmuletWord(255, 532);
				SetAmuletWord(254, 39 * (num_test) + 41);
				//SetAmuletByte(179, 0xFF);
				SetAmuletByte(num_test + 162, 0xFF);
				SetAmuletByte(179, 0xFF);
				m_iPhase = PHASE_LINEARITY_LINEATOR_WAIT_FOR_ACCEPT;
			}
			break;

		case PHASE_LINEARITY_LINEATOR_WAIT_FOR_ACCEPT:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					// Changed:
					//act = measurement[ch_num].act;
					//kun = measurement[ch_num].kun;
					//act1 = act / unitfact[kun - 1];
					act1 = measurement[ch_num].act0;

					if(num_test == 1){
						read_clock(&time0);
						corr_act = act1;
						linmeas[num_test - 1].meas_time = time0;
					}else{
						read_clock(&time1);
						linmeas[num_test - 1].meas_time = time1;
						corr_act = nucdecay(act1, time1, time0, nucdata.halflife, nucdata.hlunit);
					}

					linmeas[num_test - 1].meas_act = corr_act;
					linmeas[num_test - 1].factor = linmeas[0].meas_act / linmeas[num_test - 1].meas_act;
					linmeas[num_test - 1].pc_ratio = 100.0 * linmeas[num_test - 1].factor / factors[num_test - 1];
					// Locked: CI
					//format_activity(linmeas[num_test - 1].meas_act, current.system, linmeas[num_test - 1].act_str);
					//format_activity(linmeas[num_test - 1].meas_act, CI, linmeas[num_test - 1].act_str);
					format_activity_system(linmeas[num_test - 1].meas_act, linmeas[num_test - 1].act_str);
					strcpy(acMsg, linmeas[num_test - 1].act_str);
					trim(acMsg);
					strcpy(acMsg2, chamber[ch_num].nucdata.name);
					trim(acMsg2);
					strcat(acMsg2, " ");
					strcat(acMsg2, acMsg);
					SetAmuletString(3*(num_test - 1) + 164, acMsg2);
				}
			}
			break;

		case PHASE_LINEARITY_LINEATOR_ACCEPTED:
			if(current.num_chambers>0){
				sprintf(acMsg, "%6.2f", linmeas[num_test - 1].factor);
				//sprintf(acMsg, "factor %d", num_test);
				SetAmuletString(3*(num_test - 1) + 164, acMsg);
				strcpy(&(factor[num_test-1][0]),acMsg);

				sprintf(acMsg, "%6.2f", linmeas[num_test - 1].pc_ratio);
				//sprintf(acMsg, "ratio %d", num_test);
				SetAmuletString(3*(num_test - 1) + 165, acMsg);
				strcpy(&(ratio[num_test-1][0]), acMsg);

				num_test++;
				if(num_test < 9){
					wait = g_csec_tstamp + 5;
					m_iPhase = PHASE_LINEARITY_LINEATOR_DELAY_AFTER_ACCEPT;
				}else{
					if(current.printer!=NONE_PRINTER) SetAmuletByte(100, 0xFF);
					wait = g_csec_tstamp + 100;
					m_iPhase = PHASE_LINEARITY_STOP_UPDATING;
				}
			}
			break;

		case PHASE_LINEARITY_LINEATOR_DELAY_AFTER_ACCEPT:
			if(g_csec_tstamp > wait) m_iPhase = PHASE_LINEARITY_LINEATOR_AFTER_FULL_SCREEN;
			break;

		case PHASE_LINEARITY_STOP_UPDATING:
			if(g_csec_tstamp > wait){
				SetAmuletByte(101, 0xFF);
				m_iPhase = PHASE_LINEARITY_FINISH;
			}
			break;

		case PHASE_LINEARITY_LINEATOR_DELAY_BEFORE_FINISH:
			if(g_csec_tstamp > wait){
				for(i=0;i<8;i++){
					SetAmuletString(3*i+164, &(factor[i][0]));
					SetAmuletString(3*i+165, &(ratio[i][0]));
					SetAmuletByte(i+163, 0xFF);
				}
				if(current.printer!=NONE_PRINTER) SetAmuletByte(100, 0xFF);
				wait = g_csec_tstamp + 100;
				m_iPhase = PHASE_LINEARITY_STOP_UPDATING;
			}
			break;

		case PHASE_LINEARITY_CALICHECK_INIT:
			if(current.num_chambers>0){
				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);
				for(i=0; i<12; i++) EE_READ(lindef.lin_cal_factors[i],(uchar *)&factors[i]);

				num_meas = def.num;
				nuc_index = def.nuc_index;
				set_nuclide_data(nuc_index, ch_num);
				NuclideData_getNuclide(nuc_index, &nucdata);
				strcpy(nucname, nucdata.name);

				SetAmuletString(160, "#");
				SetAmuletString(161, "Result");
				SetAmuletString(162, "% Var");
				SetAmuletByte(182, 0xFF);
				switch(num_meas){
					case 12:
						SetAmuletString(196, "12)Blk/Purple/Blue");
						SetAmuletByte(194, 0xFF);
					case 11:
						SetAmuletString(193, "11)Blk/Purple/Green");
						SetAmuletByte(193, 0xFF);
					case 10:
						SetAmuletString(190, "10)Blk/Purpl/Yellow");
						SetAmuletByte(192, 0xFF);
					case 9:
						SetAmuletString(187, "9)Blk/Purple/Orange");
						SetAmuletByte(191, 0xFF);
					case 8:
						SetAmuletString(184, "8)Black/Purple/Red");
						SetAmuletByte(190, 0xFF);
					case 7:
						SetAmuletString(181, "7)Black/Purple");
						SetAmuletByte(189, 0xFF);
					case 6:
						SetAmuletString(178, "6)Black/Blue");
						SetAmuletByte(188, 0xFF);
					case 5:
						SetAmuletString(175, "5)Black/Green");
						SetAmuletByte(187, 0xFF);
					case 4:
						SetAmuletString(172, "4)Black/Yellow");
						SetAmuletByte(186, 0xFF);
					case 3:
						SetAmuletString(169, "3)Black/Orange");
						SetAmuletByte(185, 0xFF);
					case 2:
						SetAmuletString(166, "2)Black/Red");
						SetAmuletByte(184, 0xFF);
					case 1:
						SetAmuletString(163, "1)Black");
						SetAmuletByte(183, 0xFF);
						break;
				}

				SetAmuletByte(158, 0xFF);
				num_test = 1;
				if(m_ucClear == 30){
					m_ucClear = 0;
					printer_pressed = FALSE;
					m_iPhase = PHASE_LINEARITY_CALICHECK_WAIT_FULL_SCREEN;
				}else{
					if(printer_pressed){
						printer_pressed = FALSE;
						wait = g_csec_tstamp + 20;
						m_iPhase = PHASE_LINEARITY_CALICHECK_DELAY_BEFORE_FINISH;
					}else{
						m_iPhase = PHASE_LINEARITY_CALICHECK_WAIT_FULL_SCREEN;
					}
				}
			}
			break;

		case PHASE_LINEARITY_CALICHECK_WAIT_FULL_SCREEN:
			break;

		case PHASE_LINEARITY_CALICHECK_AFTER_FULL_SCREEN:
			if(current.num_chambers>0){
				SetAmuletWord(255, 532);
				SetAmuletWord(254, 39 * (num_test) + 41);
				SetAmuletByte(181, 0xFF);
				SetAmuletByte(num_test + 162, 0xFF);
				m_iPhase = PHASE_LINEARITY_CALICHECK_WAIT_FOR_ACCEPT;
			}
			break;

		case PHASE_LINEARITY_CALICHECK_WAIT_FOR_ACCEPT:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					// Changed:
					//act = measurement[ch_num].act;
					//kun = measurement[ch_num].kun;
					//act1 = act / unitfact[kun - 1];
					act1 = measurement[ch_num].act0;

					if(num_test == 1){
						read_clock(&time0);
						corr_act = act1;
						calmeas[num_test - 1].meas_time = time0;
					}else{
						read_clock(&time1);
						calmeas[num_test - 1].meas_time = time1;
						corr_act = nucdecay(act1, time1, time0, nucdata.halflife, nucdata.hlunit);
					}

					calmeas[num_test - 1].meas_act = corr_act;
					calmeas[num_test - 1].result = calmeas[num_test - 1].meas_act * factors[num_test - 1];

					// Locked: CI
					//format_activity(calmeas[num_test - 1].result, current.system, calmeas[num_test - 1 ].res_str);
					//format_activity(calmeas[num_test - 1].result, CI, calmeas[num_test - 1 ].res_str);
					format_activity_system(calmeas[num_test - 1].result, calmeas[num_test - 1 ].res_str);

					// Locked: CI
					//format_activity(calmeas[num_test - 1].meas_act, current.system, acMsg);
					//format_activity(calmeas[num_test - 1].meas_act, CI, acMsg);
					format_activity_system(calmeas[num_test - 1].meas_act, acMsg);

					trim(acMsg);
					strcpy(acMsg2, chamber[ch_num].nucdata.name);
					trim(acMsg2);
					strcat(acMsg2, " ");
					strcat(acMsg2, acMsg);
					SetAmuletString(3*(num_test - 1) + 164, acMsg2);
				}
			}
			break;

		case PHASE_LINEARITY_CALICHECK_ACCEPTED:
			if(current.num_chambers>0){
				mean = 0.0;
				for(i=0; i < num_test; i++){
					mean += calmeas[i].result;
				}
				mean /= (float) num_test;
				for(i=0; i < num_test; i++){
					calmeas[i].var = 100.0 * (calmeas[i].result - mean) / mean;
				}

				for(i=0; i < num_test; i++){
					strcpy(acMsg, calmeas[i].res_str);
					trim(acMsg);
					strcpy(&(caliresult[i][0]),acMsg);
					SetAmuletString(3*i + 164, acMsg);
					sprintf(acMsg, "%5.2f", calmeas[i].var);
					strcpy(&(calivar[i][0]), acMsg);
					SetAmuletString(3*i + 165, acMsg);
					delayloop(5);
				}

				num_test++;
				if(num_test < num_meas + 1){
					wait = g_csec_tstamp + 5;
					m_iPhase = PHASE_LINEARITY_CALICHECK_DELAY_AFTER_ACCEPT;
				}else{
					if(current.printer!=NONE_PRINTER) SetAmuletByte(100, 0xFF);
					wait = g_csec_tstamp + 100;
					m_iPhase = PHASE_LINEARITY_STOP_UPDATING;
				}
			}
			break;

		case PHASE_LINEARITY_CALICHECK_DELAY_AFTER_ACCEPT:
			if(g_csec_tstamp > wait) m_iPhase = PHASE_LINEARITY_CALICHECK_AFTER_FULL_SCREEN;
			break;

		case PHASE_LINEARITY_CALICHECK_DELAY_BEFORE_FINISH:
			if(g_csec_tstamp > wait){
				for(i=0; i<num_meas; i++){
					SetAmuletString(3*i + 164, &(caliresult[i][0]));
					SetAmuletString(3*i + 165, &(calivar[i][0]));
					SetAmuletByte(i+163, 0xFF);
					delayloop(20);
				}
				if(current.printer != NONE_PRINTER) SetAmuletByte(100, 0xFF);
				wait = g_csec_tstamp + 100;
				m_iPhase = PHASE_LINEARITY_STOP_UPDATING;
			}
			break;

		case PHASE_LINEARITY_CLEAR_MEASUREMENTS:
			if(current.num_chambers>0){
				for(i = 0; i < num_meas; i++)
					standlin[ch_num][i].meas_flag = 0;

				if(num_meas < 12){
					for(i = num_meas; i < 12; i++)
						standlin[ch_num][i].meas_flag = -1;
				}

				for(i=160; i<199; i++){
					SetAmuletString(i, "");
				}
				SetAmuletByte(176, 0xFF);
				SetAmuletByte(178, 0xFF);
				m_iPhase = PHASE_LINEARITY_PRE_INIT;
			}
			break;

		case PHASE_LINEARITY_PRINT:
			m_iPhase = PHASE_LINEARITY_FINISH;
			printer_pressed = TRUE;
			beep_amulet();
			switch(def.type){
				case STANDARD:
					predicted_standard_activity(predicted_act, num_test, &slope, &nucdata);
					Amulet_printLinearityStd(predicted_act, num_test, nucname);
					break;

				case LINEATOR:
					Amulet_printLinearityLin(linmeas, &def);
					break;

				case CALICHECK:
					Amulet_printLinearityCali(calmeas, &def);
					break;

			}
			SetAmuletByte(100, 0xFF);
			break;

		case PHASE_LINEARITY_FINISH:
			printer_pressed = FALSE;
			break;

		default:
			break;
	}*/
//}
