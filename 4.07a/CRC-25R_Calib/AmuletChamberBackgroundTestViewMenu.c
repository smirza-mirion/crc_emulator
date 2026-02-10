/**
 * \file
 * \details This file handles calls from the Amulet Background Test View Screen
 */
#define PHASE_CHAMBER_BACKGROUND_VIEW_PRE_INIT	0
#define PHASE_CHAMBER_BACKGROUND_VIEW_WAIT		1
#define PHASE_CHAMBER_BACKGROUND_VIEW_PRINT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "message.h"
#include "printer.h"
#include "daily.h"

extern int m_iPhase;
extern CURRENT current;
extern CHAMBERBACKGROUND AmuletChamberSearchMenu_Background;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void AmuletChamberSearch_SerialNumberString(char *serial_number, char *output);
void AmuletChamberSearch_DateString(time_t datetime, char *output);
void AmuletChamberSearch_DescString(int chamber_type, bool two_stage, char *output);
void AmuletChamberSearch_InactiveString(char *inactive_reason, char *output);
bool DB_CheckDailyTestForBackgroundID(long long int TestID);
void prbackground_db(void);

/**
 * \details Handles the Amulet Screen ChamberBackgroundTestView.htm ChamberBackgroundTestView.htm  shows a background test from the database.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 80 Language (STATE) English = 0, French = 1
 * \param 100 Print Button (STATE)
 * \param 101 Inactivate Button (STATE) FF = Inactivate Button, FE = Inactivate Reason
 * \param 102 Redisplay Buttons (TOGGLE)
 * \param 103 Display Background Red (STATE)
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102 Chamber Description
 * \param 103 Chamber Serial Number
 * \param 104 Date
 * \param 105 Background Label
 * \param 106_107 Background
 * \param 108 Inactivate
 * \param 109_110 Inactivate Text
 * \returns None
 */
void AmuletChamberBackgroundTestView_menu(void){
	char acMsg[100];

	switch(m_iPhase){
		case PHASE_CHAMBER_BACKGROUND_VIEW_PRE_INIT:
			SetAmuletByte(80, current.language);

			send_amulet_message(L_BACKGROUND2, 100);    // "Background"
			delayloop(2);

			AmuletChamberSearch_DescString(AmuletChamberSearchMenu_Background.ChamberType, AmuletChamberSearchMenu_Background.TwoStageChamber, acMsg);
			send_to_amulet_string(102, acMsg);
			delayloop(2);

			AmuletChamberSearch_SerialNumberString(AmuletChamberSearchMenu_Background.ChamberSerialNumber, acMsg);
			send_to_amulet_string(103, acMsg);
			delayloop(2);

			AmuletChamberSearch_DateString(AmuletChamberSearchMenu_Background.MeasuredOn, acMsg);
			send_to_amulet_string(104, acMsg);
			delayloop(2);

			send_amulet_message(L_BACKGROUND, 105);    // "BACKGROUND:"
			if(AmuletChamberSearchMenu_Background.CreatedOn == 0){
				send_to_amulet_string(106, "N/A");
				SetAmuletByte(103, 0xFF);
			}else{
				if(current.language == ENGLISH) send_to_amulet_string(106, AmuletChamberSearchMenu_Background.BackgroundTextEnglish);
				else if(current.language == FRENCH) send_to_amulet_string(106, AmuletChamberSearchMenu_Background.BackgroundTextFrench);
				if(AmuletChamberSearchMenu_Background.BackgroundStatus == BKG_HIGH || AmuletChamberSearchMenu_Background.BackgroundStatus == BKG_TOO_HIGH) SetAmuletByte(103, 0xFF);
			}
			delayloop(2);

			if(AmuletChamberSearchMenu_Background.Inactive){
				AmuletChamberSearch_InactiveString(AmuletChamberSearchMenu_Background.InactiveReason, acMsg);
				send_to_amulet_string(109, acMsg);
				SetAmuletByte(101, 0xFE);
			}else{
				if(!DB_CheckDailyTestForBackgroundID(AmuletChamberSearchMenu_Background.ChamberBackgroundID)){
					send_amulet_message(L_INACTIVATE, 108);    // "Inactivate"
					SetAmuletByte(101, 0xFF);
				}
			}
			delayloop(2);

			if(current.printer != NONE_PRINTER && current.printer != USB_EPS_LABEL_PRINTER) SetAmuletByte(100, 0xFF);

			SetAmuletByte(20, 0xFF);

			m_iPhase = PHASE_CHAMBER_BACKGROUND_VIEW_WAIT;
			break;

		case PHASE_CHAMBER_BACKGROUND_VIEW_WAIT:
			break;

		case PHASE_CHAMBER_BACKGROUND_VIEW_PRINT:
			beep_amulet();
			prbackground_db();
			SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_CHAMBER_BACKGROUND_VIEW_WAIT;
			break;
	}
}
