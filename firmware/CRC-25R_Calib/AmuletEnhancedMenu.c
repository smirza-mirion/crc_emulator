/**
 * \file
 * \details This file handles calls from the Amulet Enhanced Test Menu Screen
 */
#define PHASE_ENHANCED_PRE_INIT	0
#define PHASE_ENHANCED_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "message.h"

extern int m_iPhase;
extern CURRENT current;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);

/**
 * \details Handles the Amulet Screen Enhanced.htm. Enhanced.htm displays the enhanced test available on the calibrator
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 21 Show Chamber Button (TOGGLE)
 * \param 80 Language (STATE) English = 0, French = 1
 * \param 100 Show QC Button (TOGGLE)
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102 Geometry
 * \param 103 Linearity
 * \param 104 QC
 */
void AmuletEnhanced_menu(void){
	short ch_num = current.main_chamber;
	char acMsg[100];

	switch(m_iPhase){
		case PHASE_ENHANCED_PRE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_ENHANCED_TESTS, 100);    // "Enhanced Tests"
			send_amulet_message(L_GEOMETRY, 102);    // "Geometry"
			send_amulet_message(L_LINEARITY, 103);    // "Linearity"
			send_amulet_message(L_QC, 104);    // "QC"
			send_amulet_message(L_HALFLIFE_CALC, 105);    // "Half-life Calculator"

			if(current.num_chambers>1) SetAmuletByte(21, 0xFF);
			if(current.num_chambers>0){
				sprintf(acMsg, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(acMsg, ", R");
						break;
					case P_CHAMB:
						strcat(acMsg, ", PET");
						break;
					case B_CHAMB:
						strcat(acMsg, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(acMsg, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(acMsg, ", HR");
						break;
					case K_CHAMB:
						strcat(acMsg, ", 1K");
						break;
				}
				send_to_amulet_string(9, acMsg);
				SetAmuletByte(20, 0xFF);
				if(chamber_type(ch_num) == R_CHAMB){
					SetAmuletByte(100, 0xFF);
				}
			}
			m_iPhase = PHASE_ENHANCED_WAIT;
			break;

		case PHASE_ENHANCED_WAIT:
			break;
	}
}
