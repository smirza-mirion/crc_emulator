#define PHASE_TIME_PRE_INIT 0
#define PHASE_TIME_WAIT 1
#define PHASE_TIME_SUBMIT 2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "mca.h"
#include "database.h"
#include "i2c.h"

extern CURRENT  current;
extern int m_iPhase;
extern time_t clock_time;
extern time_t low_clock_time;
extern unsigned char m_ucTimeMode;
extern unsigned char m_ucSecMode;
extern unsigned char m_ucSetTime;
extern volatile char m_acTitle[52];
extern unsigned char m_ucHour;
extern unsigned int m_uiYear;
extern unsigned char m_ucMonth;
extern unsigned char m_ucDay;
extern unsigned char m_ucMinute;
extern unsigned char m_ucSecond;
extern time_t m_dtmAddInventory_Date;
extern unsigned char m_ucAddInventory_MeasuredNuclide;
extern float m_fAddInventory_Activity;
extern time_t m_dtmWithdrawInventory_TimeOfUse;
extern time_t m_dtmInfo_FromTime;
extern time_t m_dtmInfo_ToTime;
extern float m_fInfo_ToActivity;
extern unsigned char m_ucInfo_ToUnit;
extern time_t m_dtmSetupSources_Co57Date;
extern time_t m_dtmSetupSources_Co60Date;
extern time_t m_dtmSetupSources_Ba133Date;
extern time_t m_dtmSetupSources_Cs137Date;
extern time_t m_dtmSetupSources_Na22Date;
extern time_t m_dtmSetupSources_Ge68Date;
extern time_t AmuletWellMeasureEfficiency_calibrationStamp;
extern MEASUREMENT measurement[];
extern time_t AmuletWellSetupTestSourceMenu_calDate;
extern time_t AmuletWellWipeSearch_from;
extern time_t AmuletWellWipeSearch_to;
extern time_t AmuletWellSystemTestSearch_from;
extern time_t AmuletWellSystemTestSearch_to;
extern time_t AmuletWellMDASearch_from;
extern time_t AmuletWellMDASearch_to;
extern time_t AmuletWellChiSearch_from;
extern time_t AmuletWellChiSearch_to;
extern time_t AmuletWellSchillingSearch_from;
extern time_t AmuletWellSchillingSearch_to;
extern time_t AmuletWellPlasmaSearch_from;
extern time_t AmuletWellPlasmaSearch_to;
extern time_t AmuletWellRBCSearch_from;
extern time_t AmuletWellRBCSearch_to;
extern time_t AmuletWellAutoCalSearch_from;
extern time_t AmuletWellAutoCalSearch_to;
extern TESTIDENT *AmuletTestIdentMenu_info;
extern PROBEBIOASSAYEFFICIENCY AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff;
extern time_t AmuletWellBioAssaySearch_from;
extern time_t AmuletWellBioAssaySearch_to;
extern PROBETHYROIDUPTAKETEST AmuletAddEditThyroidUptakeTest_test;
extern time_t AmuletWellThyroidUptakeEnterAdministrationDate;
extern WELLRBCSURVIVALTEST AmuletAddEditRBCSurvivalTest_test;
extern time_t AmuletAutoLinearitySearch_from;
extern time_t AmuletAutoLinearitySearch_to;
extern time_t AmuletAutoLinearitySearch_from;
extern time_t AmuletAutoLinearitySearch_to;
extern time_t AmuletChamberSearch_from;
extern time_t AmuletChamberSearch_to;

void send_to_amulet_string(uchar ucIndex, char message0[]);
bool test_date_julian(short mon, short day, short year);
bool test_date(short mon, short day, short year);
void SetAmuletBackHTML(void);

void AmuletTime_menu(void) {
	short iYear;
	unsigned char ucMonth;
	unsigned char ucDayTen;
	unsigned char ucDayOne;
	unsigned char ucHourTen;
	unsigned char ucHourOne;
	unsigned char ucMinuteTen;
	unsigned char ucMinuteOne;
	unsigned char ucSecondTen;
	unsigned char ucSecondOne;
	struct tm tmt;
	struct tm dt;
	short ch = current.main_chamber;
	DateTime datetime;
	bool testresult;

	switch(m_iPhase) {
		case PHASE_TIME_PRE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_ACCEPT_BUTTON, 102);    // "Accept"
			send_amulet_message(L_CANCEL_BUTTON, 103);    // "Cancel"
			send_amulet_message(L_INVALID_DAY, 104);    // "Invalid Day"
			send_amulet_message(L_INVALID_HOUR, 105);    // "Invalid Hour"

			memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
			iYear = tmt.tm_year + 1900;
			ucMonth = tmt.tm_mon + 1;
			ucDayOne = tmt.tm_mday;
			ucDayTen = ucDayOne / 10;
			ucDayOne = ucDayOne % 10;

			if(m_ucTimeMode == 0){
				ucHourOne = tmt.tm_hour;
				ucHourTen = ucHourOne / 10;
				ucHourOne = ucHourOne % 10;
				ucMinuteOne = tmt.tm_min;
				ucMinuteTen = ucMinuteOne / 10;
				ucMinuteOne = ucMinuteOne % 10;
				if(m_ucSecMode == 0){
					ucSecondTen = 0;
					ucSecondOne = 0;
				}else if(m_ucSecMode == 1){
					ucSecondOne = tmt.tm_sec;
					ucSecondTen = ucSecondOne / 10;
					ucSecondOne = ucSecondOne % 10;
				}
			}else if(m_ucTimeMode == 1){
				ucHourTen = 0;
				ucHourOne = 0;
				ucMinuteTen = 0;
				ucMinuteOne = 0;
				ucSecondTen = 0;
				ucSecondOne = 0;
			}

			send_to_amulet_string(100, (char *)m_acTitle);
			SetAmuletWord(100, iYear);
			SetAmuletByte(100, ucMonth);
			SetAmuletByte(101, ucDayTen);
			SetAmuletByte(102, ucDayOne);
			SetAmuletByte(103, ucHourTen);
			SetAmuletByte(104, ucHourOne);
			SetAmuletByte(105, ucMinuteTen);
			SetAmuletByte(106, ucMinuteOne);
			SetAmuletByte(111, ucSecondTen);
			SetAmuletByte(112, ucSecondOne);

			if(m_ucSetTime == 22 || m_ucSetTime == 36 || m_ucSetTime == 43){
				SetAmuletWord(98, 1900);
				SetAmuletWord(99, 2100);
			}else{
				SetAmuletWord(98, 1970);
				SetAmuletWord(99, 2037);
			}
			delayloop(30);
			if(m_ucTimeMode == 0){
				if(m_ucSecMode == 0) SetAmuletByte(107, 0xFF);
				else if(m_ucSecMode == 1) SetAmuletByte(98, 0xFF);
			}else if(m_ucTimeMode == 1) SetAmuletByte(99, 0xFF);
			m_iPhase = PHASE_TIME_WAIT;
			break;

		case PHASE_TIME_WAIT:
			break;

		case PHASE_TIME_SUBMIT:
			if (m_ucHour < 24){
				if(m_ucSetTime == 22 || m_ucSetTime == 36 || m_ucSetTime == 43) testresult = test_date_julian(m_ucMonth, m_ucDay, m_uiYear);
				else testresult = test_date(m_ucMonth, m_ucDay, m_uiYear);

				if (testresult) {
					dt.tm_mon = m_ucMonth - 1;
					dt.tm_mday = m_ucDay;
					dt.tm_year = m_uiYear - 1900;
					dt.tm_hour = m_ucHour;
					dt.tm_min = m_ucMinute;
					dt.tm_sec = m_ucSecond;

					switch(m_ucSetTime){
						case 1:
							mk_time(&dt);
							set_clock(&dt);
							read_clock(&clock_time);
							low_clock_time = clock_time;
							reset_minute_counter();
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 2:
							m_dtmAddInventory_Date = mk_time(&dt);
							if(m_ucAddInventory_MeasuredNuclide!=0xFF) {
								m_fAddInventory_Activity = -999;
								m_ucAddInventory_MeasuredNuclide = 0xFF;
							}
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 3:
							m_dtmWithdrawInventory_TimeOfUse = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 4:
							m_dtmInfo_FromTime = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 5:
							m_dtmInfo_ToTime = mk_time(&dt);
							m_fInfo_ToActivity = -999;
							m_ucInfo_ToUnit = 0xFF;
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 6:
							m_dtmSetupSources_Co57Date = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 7:
							m_dtmSetupSources_Co60Date = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 8:
							m_dtmSetupSources_Ba133Date = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 9:
							m_dtmSetupSources_Cs137Date = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 10:
							m_dtmSetupSources_Na22Date = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 11:
							AmuletWellMeasureEfficiency_calibrationStamp = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 12:
							measurement[ch].future.dosetime = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 13:
							AmuletWellSetupTestSourceMenu_calDate = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 14:
							AmuletWellWipeSearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 15:
							AmuletWellWipeSearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 16:
							AmuletWellSystemTestSearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 17:
							AmuletWellSystemTestSearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 18:
							AmuletWellMDASearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 19:
							AmuletWellMDASearch_to =  mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 20:
							AmuletWellChiSearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 21:
							AmuletWellChiSearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 22:
							datetime.iJD = 0;
							datetime.Y = dt.tm_year + 1900;
							datetime.M = dt.tm_mon + 1;
							datetime.D = dt.tm_mday;
							datetime.h = dt.tm_hour;
							datetime.m = dt.tm_min;
							datetime.s = dt.tm_sec;
							datetime.validJD = 0;
							datetime.validYMD = 1;
							datetime.validHMS = 1;
							datetime.validTZ = 0;
							computeJD(&datetime);

							AmuletTestIdentMenu_info->DateOfBirth = datetime.iJD;
							AmuletTestIdentMenu_info->DateOfBirth /= 86400000.0;
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 23:
							AmuletWellSchillingSearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 24:
							AmuletWellSchillingSearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 25:
							AmuletWellPlasmaSearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 26:
							AmuletWellPlasmaSearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 27:
							AmuletWellRBCSearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 28:
							AmuletWellRBCSearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 29:
							AmuletWellAutoCalSearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 30:
							AmuletWellAutoCalSearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 31:
							AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Date = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 32:
							AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Date = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 33:
							AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Date = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 34:
							AmuletWellBioAssaySearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 35:
							AmuletWellBioAssaySearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 36:
							datetime.iJD = 0;
							datetime.Y = dt.tm_year + 1900;
							datetime.M = dt.tm_mon + 1;
							datetime.D = dt.tm_mday;
							datetime.h = dt.tm_hour;
							datetime.m = dt.tm_min;
							datetime.s = dt.tm_sec;
							datetime.validJD = 0;
							datetime.validYMD = 1;
							datetime.validHMS = 1;
							datetime.validTZ = 0;
							computeJD(&datetime);

							AmuletAddEditThyroidUptakeTest_test.DateOfBirth = datetime.iJD;
							AmuletAddEditThyroidUptakeTest_test.DateOfBirth /= 86400000.0;
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 37:
							AmuletAddEditThyroidUptakeTest_test.CalibrationDate = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 38:
							AmuletWellThyroidUptakeEnterAdministrationDate = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 43:
							datetime.iJD = 0;
							datetime.Y = dt.tm_year + 1900;
							datetime.M = dt.tm_mon + 1;
							datetime.D = dt.tm_mday;
							datetime.h = dt.tm_hour;
							datetime.m = dt.tm_min;
							datetime.s = dt.tm_sec;
							datetime.validJD = 0;
							datetime.validYMD = 1;
							datetime.validHMS = 1;
							datetime.validTZ = 0;
							computeJD(&datetime);

							AmuletAddEditRBCSurvivalTest_test.DateOfBirth = datetime.iJD;
							AmuletAddEditRBCSurvivalTest_test.DateOfBirth /= 86400000.0;
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 44:
							AmuletAddEditRBCSurvivalTest_test.CalibrationDate = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 45:
							AmuletAddEditRBCSurvivalTest_test.InjectionDate = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 46:
							AmuletAutoLinearitySearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 47:
							AmuletAutoLinearitySearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 48:
							AmuletChamberSearch_from = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 49:
							AmuletChamberSearch_to = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						case 50:
							m_dtmSetupSources_Ge68Date = mk_time(&dt);
							SetAmuletBackHTML();
							m_ucSetTime = 0;
							break;

						default:
							m_iPhase = PHASE_TIME_WAIT;
							break;
					}
				}
				else {
					SetAmuletByte(109, 0xFF);
					m_iPhase = PHASE_TIME_WAIT;
				}
			}
			else {
				SetAmuletByte(108, 0xFF);
				if(!test_date(m_ucMonth, m_ucDay, m_uiYear)) SetAmuletByte(109, 0xFF);
				m_iPhase = PHASE_TIME_WAIT;
			}

			break;
	}
}
