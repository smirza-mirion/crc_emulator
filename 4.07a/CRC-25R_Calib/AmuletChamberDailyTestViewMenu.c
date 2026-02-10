/**
 * \file
 * \details This file handles calls from the Amulet Daily Test View Screen
 */
#define PHASE_CHAMBER_DAILY_VIEW_PRE_INIT	0
#define PHASE_CHAMBER_DAILY_VIEW_WAIT		1
#define PHASE_CHAMBER_DAILY_VIEW_PRINT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "message.h"
#include "printer.h"
#include "daily.h"

extern int m_iPhase;
extern CURRENT current;
extern CHAMBERDAILYTEST	AmuletChamberSearchMenu_DailyTest;
extern CHAMBERZERO AmuletChamberSearchMenu_Zero;
extern CHAMBERBACKGROUND AmuletChamberSearchMenu_Background;
extern CHAMBERVOLTAGE AmuletChamberSearchMenu_Voltage;
extern CHAMBERACCURACYTEST AmuletChamberSearchMenu_AccuracyTest;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void AmuletChamberSearch_SerialNumberString(char *serial_number, char *output);
void AmuletChamberSearch_DateString(time_t datetime, char *output);
void AmuletChamberSearch_DescString(int chamber_type, bool two_stage, char *output);
void AmuletChamberSearch_InactiveString(char *inactive_reason, char *output);
void prdaily_db(void);

/**
 * \details Handles the Amulet Screen ChamberDailyTestView.htm. ChamberDailyTestView.htm shows a daily test from the database.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 80 Language (STATE) English = 0, Spanish = 1
 * \param 100 Accuracy Button (STATE)
 * \param 101 Print Button (STATE)
 * \param 102 Inactivate Button (STATE) FF = Inactivate Button, FE = Inactivate Reason
 * \param 103 Redisplay Buttons (TOGGLE)
 * \param 104 Display Zero Red (STATE)
 * \param 105 Display Background Red (STATE)
 * \param 106 Display Voltage Red (STATE)
 * \param 107 Display Data Check Red (STATE)
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102 Chamber Description
 * \param 103 Chamber Serial Number
 * \param 104 Date
 * \param 105 Zero Label
 * \param 106_107 Zero
 * \param 108 Background Label
 * \param 109_110 Background
 * \param 111 Voltage Label
 * \param 112_113 Voltage
 * \param 114_115 Data Check Label
 * \param 116_117 Data Check
 * \param 118 Accuracy Test
 * \param 119 Inactivate
 * \param 120_121 Inactivate Text
 * \returns None
 */
void AmuletChamberDailyTestView_menu(void){
	char acMsg[100];

	switch(m_iPhase){
		case PHASE_CHAMBER_DAILY_VIEW_PRE_INIT:
			SetAmuletByte(80, current.language);

			send_amulet_message(L_DAILY_TEST, 100);    // "Daily Test"
			delayloop(2);

			AmuletChamberSearch_DescString(AmuletChamberSearchMenu_DailyTest.ChamberType, AmuletChamberSearchMenu_DailyTest.TwoStageChamber, acMsg);
			send_to_amulet_string(102, acMsg);
			delayloop(2);

			AmuletChamberSearch_SerialNumberString(AmuletChamberSearchMenu_DailyTest.ChamberSerialNumber, acMsg);
			send_to_amulet_string(103, acMsg);
			delayloop(2);

			AmuletChamberSearch_DateString(AmuletChamberSearchMenu_DailyTest.CreatedOn, acMsg);
			send_to_amulet_string(104, acMsg);
			delayloop(2);

			send_amulet_message(L_ZERO, 105);    // "ZERO:"
			if(AmuletChamberSearchMenu_Zero.CreatedOn == 0){
				send_to_amulet_string(106, "N/A");
				SetAmuletByte(104, 0xFF);
			}else{
				if(current.language == ENGLISH) send_to_amulet_string(106, AmuletChamberSearchMenu_Zero.ZeroTextEnglish);
				else if(current.language == SPANISH) send_to_amulet_string(106, AmuletChamberSearchMenu_Zero.ZeroTextSpanish);
				if(AmuletChamberSearchMenu_Zero.ZeroStatus == ZERO_OUT_OF_RANGE || AmuletChamberSearchMenu_Zero.ZeroStatus == ZERO_DRIFT) SetAmuletByte(104, 0xFF);
			}
			delayloop(2);

			send_amulet_message(L_BACKGROUND, 108);    // "BACKGROUND:"
			if(AmuletChamberSearchMenu_Background.CreatedOn == 0){
				send_to_amulet_string(109, "N/A");
				SetAmuletByte(105, 0xFF);
			}else{
				if(current.language == ENGLISH) send_to_amulet_string(109, AmuletChamberSearchMenu_Background.BackgroundTextEnglish);
				else if(current.language == SPANISH) send_to_amulet_string(109, AmuletChamberSearchMenu_Background.BackgroundTextSpanish);
				if(AmuletChamberSearchMenu_Background.BackgroundStatus == BKG_HIGH || AmuletChamberSearchMenu_Background.BackgroundStatus == BKG_TOO_HIGH) SetAmuletByte(105, 0xFF);
			}
			delayloop(2);

			send_amulet_message(L_CHAMBER_VOLTAGE, 111);    // "CHAMBER VOLTAGE:"
			if(AmuletChamberSearchMenu_Voltage.CreatedOn == 0){
				send_to_amulet_string(112, "N/A");
				SetAmuletByte(106, 0xFF);
			}else{
				if(current.language == ENGLISH) send_to_amulet_string(112, AmuletChamberSearchMenu_Voltage.VoltageTextEnglish);
				else if(current.language == SPANISH) send_to_amulet_string(112, AmuletChamberSearchMenu_Voltage.VoltageTextSpanish);
				if(AmuletChamberSearchMenu_Voltage.VoltageStatus == BIAS_TEST_FAIL) SetAmuletByte(106, 0xFF);
			}
			delayloop(2);

			send_amulet_message(L_DATA_CHECK_PRINTED, 114);    // "DATA CHECK:"
			if(AmuletChamberSearchMenu_DailyTest.DataCheckTextEnglish[0] == 0){
				send_to_amulet_string(116, "N/A");
				SetAmuletByte(107, 0xFF);
			}else{
				if(current.language == ENGLISH) send_to_amulet_string(116, AmuletChamberSearchMenu_DailyTest.DataCheckTextEnglish);
				else if(current.language == SPANISH) send_to_amulet_string(116, AmuletChamberSearchMenu_DailyTest.DataCheckTextSpanish);
				if(AmuletChamberSearchMenu_DailyTest.DataCheckFailed) SetAmuletByte(107, 0xFF);
			}
			delayloop(2);

			if(AmuletChamberSearchMenu_AccuracyTest.CreatedOn != 0){
				send_amulet_message(L_ACCURACYTEST_2, 118);    // "Accuracy Test"
				SetAmuletByte(100, 0xFF);
			}
			delayloop(2);

			if(current.printer != NONE_PRINTER && current.printer != USB_EPS_LABEL_PRINTER) SetAmuletByte(101, 0xFF);

			if(AmuletChamberSearchMenu_DailyTest.Inactive){
				AmuletChamberSearch_InactiveString(AmuletChamberSearchMenu_DailyTest.InactiveReason, acMsg);
				send_to_amulet_string(120, acMsg);
				SetAmuletByte(102, 0xFE);
			}else{
				send_amulet_message(L_INACTIVATE, 119);    // "Inactivate"
				SetAmuletByte(102, 0xFF);
			}
			delayloop(2);

			SetAmuletByte(20, 0xFF);

			m_iPhase = PHASE_CHAMBER_DAILY_VIEW_WAIT;
			break;

		case PHASE_CHAMBER_DAILY_VIEW_WAIT:
			break;

		case PHASE_CHAMBER_DAILY_VIEW_PRINT:
			beep_amulet();
			prdaily_db();
			SetAmuletByte(103, 0xFF);
			m_iPhase = PHASE_CHAMBER_DAILY_VIEW_WAIT;
			break;
	}
}
