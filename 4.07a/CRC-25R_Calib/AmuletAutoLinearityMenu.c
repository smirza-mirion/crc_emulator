/**
 * \file
 * \details This file handles calls from the Amulet AutoLinearity Menu Screen
 */
#define PHASE_AUTOLINEARITY_PRE_INIT	0
#define PHASE_AUTOLINEARITY_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "chambfac.h"
#include "linearity.h"
#include "i2c.h"
#include "message.h"

extern int m_iPhase;
extern CURRENT current;
extern AUTOLINEARITYTEST AmuletAutoLinearityTest_test;

void send_to_amulet_string(uchar ucIndex, char message0[]);

/**
 * \details Handles the Amulet Screen AutoLinearity.htm. AutoLinearity.htm allows the user to start an autolinearity test or view a previous autolinearity test.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 21 Show Chamber Button (TOGGLE)
 * \param 80 Language (State)
 * \param 91 Show Start Test Button (STATE)
 * \param 92 Show Resume Test Button (STATE)
 * \param 93 Show Reports Button (STATE)
 * \param 100 Show Screen (TOGGLE)
 * \param Amulet_String_ID Description
 * \param 9 Chamber Label
 * \param 100_101 AutoLinearity
 * \param 102_103 Start Test
 * \param 104_105 Resume Test
 * \param 106_107 Reports
 * \returns None
 */
void AmuletAutoLinearity_menu(void){
	char message[100];

	switch(m_iPhase){
		case PHASE_AUTOLINEARITY_PRE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_AUTOLINEARITY, 100);    // "AutoLinearity"
			send_amulet_message(L_START_TEST, 102);    // "Start Test"
			send_amulet_message(L_RESUME_TEST, 104);    // "Resume Test"
			send_amulet_message(L_REPORTS, 106);    // "Reports"
			if(current.num_chambers>1) SetAmuletByte(21, 0xFF);

			if(current.num_chambers>0){
				sprintf(message, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(message, ", R");
						break;
					case P_CHAMB:
						strcat(message, ", PET");
						break;
					case B_CHAMB:
						strcat(message, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(message, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(message, ", HR");
						break;
					case K_CHAMB:
						strcat(message, ", 1K");
						break;
				}
				send_to_amulet_string(9, message);
				SetAmuletByte(20, 0xFF);
			}

			if(AmuletAutoLinearityTest_test.AutoLinearityTestID < -1) SetAmuletByte(92, 0xFF);	// Display Resume Button
			else SetAmuletByte(91, 0xFF);														// Display Start Button
			SetAmuletByte(93, 0xFF);

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_AUTOLINEARITY_WAIT;
			break;

		case PHASE_AUTOLINEARITY_WAIT:
			break;
	}
}
