/**
 * \file
 * \details This file handles calls from the Amulet Bias Voltage Test View Screen
 */
#define PHASE_CHAMBER_VOLTAGE_VIEW_PRE_INIT	0
#define PHASE_CHAMBER_VOLTAGE_VIEW_WAIT		1
#define PHASE_CHAMBER_VOLTAGE_VIEW_PRINT	2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "message.h"
#include "printer.h"
#include "daily.h"

extern int m_iPhase;
extern CURRENT current;
extern CHAMBERVOLTAGE AmuletChamberSearchMenu_Voltage;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void AmuletChamberSearch_SerialNumberString(char *serial_number, char *output);
void AmuletChamberSearch_DateString(time_t datetime, char *output);
void AmuletChamberSearch_DescString(int chamber_type, bool two_stage, char *output);
void AmuletChamberSearch_InactiveString(char *inactive_reason, char *output);
bool DB_CheckDailyTestForVoltageID(long long int TestID);
void prchambervoltage_db(void);

/**
 * \details Handles the Amulet Screen ChamberVoltageTestView.htm. ChamberVoltageTestView.htm shows a bias voltage test from the database.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 80 Language (STATE) English = 0, French = 1
 * \param 100 Print Button (STATE)
 * \param 101 Inactivate Button (STATE) FF = Inactivate Button, FE = Inactivate Reason
 * \param 102 Redisplay Buttons (TOGGLE)
 * \param 103 Display Voltage Red (STATE)
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102 Chamber Description
 * \param 103 Chamber Serial Number
 * \param 104 Date
 * \param 105 Voltage Label
 * \param 106_107 Voltage
 * \param 108 Nominal Label
 * \param 109_110 Nominal
 * \param 111 Min Label
 * \param 112_113 Min
 * \param 114 Max Label
 * \param 115_116 Max
 * \param 117 Inactivate
 * \param 118_119 Inactivate Text
 * \returns None
 */
void AmuletChamberVoltageTestView_menu(void){
	char acMsg[100];

	switch(m_iPhase){
		case PHASE_CHAMBER_VOLTAGE_VIEW_PRE_INIT:
			SetAmuletByte(80, current.language);

			send_amulet_message(L_CHAMBER_VOLTAGE_2, 100);    // "Chamber Voltage"
			delayloop(2);

			AmuletChamberSearch_DescString(AmuletChamberSearchMenu_Voltage.ChamberType, AmuletChamberSearchMenu_Voltage.TwoStageChamber, acMsg);
			send_to_amulet_string(102, acMsg);
			delayloop(2);

			AmuletChamberSearch_SerialNumberString(AmuletChamberSearchMenu_Voltage.ChamberSerialNumber, acMsg);
			send_to_amulet_string(103, acMsg);
			delayloop(2);

			AmuletChamberSearch_DateString(AmuletChamberSearchMenu_Voltage.MeasuredOn, acMsg);
			send_to_amulet_string(104, acMsg);
			delayloop(2);

			send_amulet_message(L_CHAMBER_VOLTAGE, 105);    // "CHAMBER VOLTAGE:"
			delayloop(2);
			send_amulet_message(L_NOMINAL_VOLTAGE, 108);    // "NOMINAL VOLTAGE:"
			delayloop(2);
			send_amulet_message(L_MIN_VOLTAGE, 111);    // "MIN VOLTAGE:"
			delayloop(2);
			send_amulet_message(L_MAX_VOLTAGE, 114);    // "MAX VOLTAGE:"
			delayloop(2);

			if(AmuletChamberSearchMenu_Voltage.CreatedOn == 0){
				send_to_amulet_string(106, "N/A");
				delayloop(2);
				send_to_amulet_string(109, "N/A");
				delayloop(2);
				send_to_amulet_string(112, "N/A");
				delayloop(2);
				send_to_amulet_string(115, "N/A");
				SetAmuletByte(103, 0xFF);
				delayloop(2);
			}else{
				if(current.language == ENGLISH) send_to_amulet_string(106, AmuletChamberSearchMenu_Voltage.VoltageTextEnglish);
				else if(current.language == FRENCH) send_to_amulet_string(106, AmuletChamberSearchMenu_Voltage.VoltageTextFrench);
				if(AmuletChamberSearchMenu_Voltage.VoltageStatus == BIAS_TEST_FAIL) SetAmuletByte(103, 0xFF);
				delayloop(2);

				sprintf(acMsg, "%.1f V", AmuletChamberSearchMenu_Voltage.NominalVoltage);
				send_to_amulet_string(109, acMsg);
				delayloop(2);

				sprintf(acMsg, "%.1f V", AmuletChamberSearchMenu_Voltage.MinVoltage);
				send_to_amulet_string(112, acMsg);
				delayloop(2);

				sprintf(acMsg, "%.1f V", AmuletChamberSearchMenu_Voltage.MaxVoltage);
				send_to_amulet_string(115, acMsg);
				delayloop(2);
			}

			if(AmuletChamberSearchMenu_Voltage.Inactive){
				AmuletChamberSearch_InactiveString(AmuletChamberSearchMenu_Voltage.InactiveReason, acMsg);
				send_to_amulet_string(118, acMsg);
				SetAmuletByte(101, 0xFE);
			}else{
				if(!DB_CheckDailyTestForVoltageID(AmuletChamberSearchMenu_Voltage.ChamberVoltageID)){
					send_amulet_message(L_INACTIVATE, 117);    // "Inactivate"
					SetAmuletByte(101, 0xFF);
				}
			}
			delayloop(2);

			if(current.printer != NONE_PRINTER && current.printer != USB_EPS_LABEL_PRINTER) SetAmuletByte(100, 0xFF);

			SetAmuletByte(20, 0xFF);

			m_iPhase = PHASE_CHAMBER_VOLTAGE_VIEW_WAIT;
			break;

		case PHASE_CHAMBER_VOLTAGE_VIEW_WAIT:
			break;

		case PHASE_CHAMBER_VOLTAGE_VIEW_PRINT:
			beep_amulet();
			prchambervoltage_db();
			SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_CHAMBER_VOLTAGE_VIEW_WAIT;
			break;
	}
}
