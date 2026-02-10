/**
 * \file
 * \details This file handles calls from the Amulet Zero Test View Screen
 */
#define PHASE_CHAMBER_ZERO_VIEW_PRE_INIT	0
#define PHASE_CHAMBER_ZERO_VIEW_WAIT		1
#define PHASE_CHAMBER_ZERO_VIEW_PRINT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "message.h"
#include "printer.h"
#include "daily.h"

extern int m_iPhase;
extern CURRENT current;
extern CHAMBERZERO AmuletChamberSearchMenu_Zero;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void AmuletChamberSearch_SerialNumberString(char *serial_number, char *output);
void AmuletChamberSearch_DateString(time_t datetime, char *output);
void AmuletChamberSearch_DescString(int chamber_type, bool two_stage, char *output);
void AmuletChamberSearch_InactiveString(char *inactive_reason, char *output);
bool DB_CheckDailyTestForZeroID(long long int TestID);
void przero_db(void);

/**
 * \details Handles the Amulet Screen ChamberZeroTestView.htm  ChamberZeroTestView.htm shows a zero test from the database.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 80 Language (STATE) English = 0, Spanish = 1
 * \param 100 Print Button (STATE)
 * \param 101 Inactivate Button (STATE) FF = Inactivate Button, FE = Inactivate Reason
 * \param 102 Redisplay Buttons (TOGGLE)
 * \param 103 Display Zero Red (STATE)
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102 Chamber Description
 * \param 103 Chamber Serial Number
 * \param 104 Date
 * \param 105 Zero Label
 * \param 106_107 Zero
 * \param 108 Inactivate
 * \param 109_110 Inactivate Text
 * \returns None
 */
void AmuletChamberZeroTestView_menu(void){
	char acMsg[100];

	switch(m_iPhase){
		case PHASE_CHAMBER_ZERO_VIEW_PRE_INIT:
			SetAmuletByte(80, current.language);

			send_amulet_message(L_ZERO_2, 100);    // "Zero"
			delayloop(2);

			AmuletChamberSearch_DescString(AmuletChamberSearchMenu_Zero.ChamberType, AmuletChamberSearchMenu_Zero.TwoStageChamber, acMsg);
			send_to_amulet_string(102, acMsg);
			delayloop(2);

			AmuletChamberSearch_SerialNumberString(AmuletChamberSearchMenu_Zero.ChamberSerialNumber, acMsg);
			send_to_amulet_string(103, acMsg);
			delayloop(2);

			AmuletChamberSearch_DateString(AmuletChamberSearchMenu_Zero.MeasuredOn, acMsg);
			send_to_amulet_string(104, acMsg);
			delayloop(2);

			send_amulet_message(L_ZERO, 105);    // "ZERO:"
			if(AmuletChamberSearchMenu_Zero.CreatedOn == 0){
				send_to_amulet_string(106, "N/A");
				SetAmuletByte(103, 0xFF);
			}else{
				if(current.language == ENGLISH) send_to_amulet_string(106, AmuletChamberSearchMenu_Zero.ZeroTextEnglish);
				else if(current.language == SPANISH) send_to_amulet_string(106, AmuletChamberSearchMenu_Zero.ZeroTextSpanish);
				if(AmuletChamberSearchMenu_Zero.ZeroStatus == ZERO_OUT_OF_RANGE || AmuletChamberSearchMenu_Zero.ZeroStatus == ZERO_DRIFT) SetAmuletByte(103, 0xFF);
			}
			delayloop(2);

			if(AmuletChamberSearchMenu_Zero.Inactive){
				AmuletChamberSearch_InactiveString(AmuletChamberSearchMenu_Zero.InactiveReason, acMsg);
				send_to_amulet_string(109, acMsg);
				SetAmuletByte(101, 0xFE);
			}else{
				if(!DB_CheckDailyTestForZeroID(AmuletChamberSearchMenu_Zero.ChamberZeroID)){
					send_amulet_message(L_INACTIVATE, 108);    // "Inactivate"
					SetAmuletByte(101, 0xFF);
				}
			}
			delayloop(2);

			if(current.printer != NONE_PRINTER && current.printer != USB_EPS_LABEL_PRINTER) SetAmuletByte(100, 0xFF);

			SetAmuletByte(20, 0xFF);

			m_iPhase = PHASE_CHAMBER_ZERO_VIEW_WAIT;
			break;

		case PHASE_CHAMBER_ZERO_VIEW_WAIT:
			break;

		case PHASE_CHAMBER_ZERO_VIEW_PRINT:
			beep_amulet();
			przero_db();
			SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_CHAMBER_ZERO_VIEW_WAIT;
			break;
	}
}
