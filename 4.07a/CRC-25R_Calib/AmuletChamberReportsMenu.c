/**
 * \file
 * \details This file handles calls from the Amulet Reports Menu Screen
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
 * \details Handles the Amulet Screen ChamberReports.htm ChamberReports.htm displays the reports available on the calibrator.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 21 Show Chamber Button (TOGGLE)
 * \param 80 Language (STATE) English = 0, French = 1
 * \param Amulet_String_ID Description
 * \param 100_101 Title
 * \param 102 Daily Test
 * \param 103 Zero
 * \param 104 Background
 * \param 105 Chamber Voltage
 * \param 106 Accuracy
 * \returns None
 */
void AmuletChamberReports_menu(void){
	switch(m_iPhase){
		case PHASE_ENHANCED_PRE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_REPORTS, 100);    // "Reports"
			delayloop(2);
			send_amulet_message(L_DAILY_TEST, 102);    // "Daily Test"
			delayloop(2);
			send_amulet_message(L_ZERO_2, 103);    // "Zero"
			delayloop(2);
			send_amulet_message(L_BACKGROUND2, 104);    // "Background"
			delayloop(2);
			send_amulet_message(L_CHAMBER_VOLTAGE_2, 105);    // "Chamber Voltage"
			delayloop(2);
			send_amulet_message(L_ACCURACYTEST_2, 106);    // "Accuracy Test"
			delayloop(2);
			SetAmuletByte(20, 0xFF);

			m_iPhase = PHASE_ENHANCED_WAIT;
			break;

		case PHASE_ENHANCED_WAIT:
			break;
	}
}
