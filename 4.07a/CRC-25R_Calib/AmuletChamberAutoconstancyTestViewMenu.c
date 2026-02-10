/**
 * \file
 * \details This file handles calls from the Amulet AutoConstancy Test View Screen
 */
#define PHASE_CHAMBER_AUTOCONSTANCY_VIEW_PRE_INIT	0
#define PHASE_CHAMBER_AUTOCONSTANCY_VIEW_WAIT		1
#define PHASE_CHAMBER_AUTOCONSTANCY_VIEW_PRINT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "message.h"
#include "printer.h"

extern int m_iPhase;
extern CURRENT current;
extern CHAMBERDAILYTEST			AmuletChamberSearchMenu_DailyTest;
extern CHAMBERACCURACYTEST	AmuletChamberSearchMenu_AccuracyTest;
extern CHAMBERAUTOCONSTANCY	AmuletChamberSearchMenu_AutoConstancy[12];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void AmuletChamberSearch_SerialNumberString(char *serial_number, char *output);
void AmuletChamberSearch_DateString(time_t datetime, char *output);
void AmuletChamberSearch_DescString(int chamber_type, bool two_stage, char *output);
void AmuletChamberSearch_InactiveString(char *inactive_reason, char *output);
void prdaily_db(void);
void praccuracy_db(void);

/**
 * \details Handles the Amulet Screen ChamberAutoconstancyTestView.htm. ChamberAutoconstancyTestView.htm shows an autoconstancy test from the database.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 80 Language (STATE) English = 0, Spanish = 1
 * \param 100 Print Button (STATE)
 * \param 101 Display Line 1 (STATE)
 * \param 102 Display Line 2 (STATE)
 * \param 103 Display Line 3 (STATE)
 * \param 104 Display Line 4 (STATE)
 * \param 105 Display Line 5 (STATE)
 * \param 106 Display Line 6 (STATE)
 * \param 107 Display Line 7 (STATE)
 * \param 108 Display Line 8 (STATE)
 * \param 109 Display Line 9 (STATE)
 * \param 110 Display Line 10 (STATE)
 * \param 111 Display Line 11 (STATE)
 * \param 112 Display Line 12 (STATE)
 * \param 113 Show Buttons (TOGGLE)
 * \param 114 Inactive Button (STATE) Inactivate Button, FE = Inactivate Reason=
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102 Chamber Description
 * \param 103 Chamber Serial Number
 * \param 104 Date
 * \param 105 Source Label
 * \param 106 Source
 * \param 107 S/N Label
 * \param 108 S/N
 * \param 109_110 Line 1
 * \param 111_112 Line 2
 * \param 113_114 Line 3
 * \param 115_116 Line 4
 * \param 117_118 Line 5
 * \param 119_120 Line 6
 * \param 121_122 Line 7
 * \param 123_124 Line 8
 * \param 125_126 Line 9
 * \param 127_128 Line 10
 * \param 129_130 Line 11
 * \param 131_132 Line 12
 * \param 133_134 Inactive Text
 * \returns None
 */
void AmuletChamberAutoconstancyTestView_menu(void){
	int i, j;
	char acMsg[100];

	switch(m_iPhase){
		case PHASE_CHAMBER_AUTOCONSTANCY_VIEW_PRE_INIT:
			SetAmuletByte(80, current.language);

			send_amulet_message(L_AUTOCONSTANCY, 100);    // "AutoConstancy"
			delayloop(2);

			AmuletChamberSearch_DescString(AmuletChamberSearchMenu_AccuracyTest.ChamberType, AmuletChamberSearchMenu_AccuracyTest.TwoStageChamber, acMsg);
			send_to_amulet_string(102, acMsg);
			delayloop(2);

			AmuletChamberSearch_SerialNumberString(AmuletChamberSearchMenu_AccuracyTest.ChamberSerialNumber, acMsg);
			send_to_amulet_string(103, acMsg);
			delayloop(2);

			AmuletChamberSearch_DateString(AmuletChamberSearchMenu_AccuracyTest.CreatedOn, acMsg);
			send_to_amulet_string(104, acMsg);
			delayloop(2);

			send_amulet_message(L_SOURCE, 105);    // "Source:"
			send_to_amulet_string(106, AmuletChamberSearchMenu_AccuracyTest.ConstancyNuclide);
			delayloop(2);

			send_amulet_message(L_SN2, 107);    // "S/N:"
			send_to_amulet_string(108, AmuletChamberSearchMenu_AccuracyTest.ConstancyNuclideSerialNumber);
			delayloop(2);

			if(current.printer != NONE_PRINTER && current.printer != USB_EPS_LABEL_PRINTER) SetAmuletByte(100, 0xFF);

			j = 0;
			for(i=0; i<12; i++){
				if(AmuletChamberSearchMenu_AutoConstancy[i].ChamberAutoConstancyID != 0){
					acMsg[0] = 0;
					if(current.language == ENGLISH) sprintf(acMsg, "%d) %s: %s", j+1, AmuletChamberSearchMenu_AutoConstancy[i].ConstancyNuclide, AmuletChamberSearchMenu_AutoConstancy[i].ConstancyActivityTextEnglish);
					else if(current.language == SPANISH) sprintf(acMsg, "%d) %s: %s", j+1, AmuletChamberSearchMenu_AutoConstancy[i].ConstancyNuclide, AmuletChamberSearchMenu_AutoConstancy[i].ConstancyActivityTextSpanish);
					send_to_amulet_string((2*j) + 109, acMsg);
					delayloop(2);
					SetAmuletByte(101 + j, 0xFF);
					j++;
				}
			}

			if(AmuletChamberSearchMenu_AccuracyTest.Inactive){
				AmuletChamberSearch_InactiveString(AmuletChamberSearchMenu_AccuracyTest.InactiveReason, acMsg);
				send_to_amulet_string(133, acMsg);
				SetAmuletByte(114, 0xFE);
			}
			delayloop(2);

			SetAmuletByte(20, 0xFF);

			m_iPhase = PHASE_CHAMBER_AUTOCONSTANCY_VIEW_WAIT;
			break;

		case PHASE_CHAMBER_AUTOCONSTANCY_VIEW_WAIT:
			break;

		case PHASE_CHAMBER_AUTOCONSTANCY_VIEW_PRINT:
			beep_amulet();
			if(AmuletChamberSearchMenu_DailyTest.ChamberDailyTestID == 0){
				praccuracy_db();
			}else{
				prdaily_db();
			}
			SetAmuletByte(113, 0xFF);
			m_iPhase = PHASE_CHAMBER_AUTOCONSTANCY_VIEW_WAIT;
			break;
	}
}
