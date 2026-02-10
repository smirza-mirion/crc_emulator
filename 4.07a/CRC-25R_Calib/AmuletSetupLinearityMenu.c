/**
 * \file
 * \details This file handles calls from the Amulet Linearity Setup Selection Screen
 */
#define PHASE_SETUP_LINEARITY_PRE_INIT	0
#define PHASE_SETUP_LINEARITY_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "chambfac.h"

extern int m_iPhase;
extern CURRENT current;

/**
 * \details Handles the Amulet Screen SetupLinearity.htm. SetupLinearity.htm shows the setup options for linearity.
 * \param Amulet_Byte_ID Description
 * \param 92 Language (STATE) English = 0, Spanish = 1
 * \param 93 CRC-77t (STATE) Not CRC-77t = 0, CRC-77t = 1
 * \param Amulet_String_ID Description
 * \param 110 Title
 * \param 111_112 Standard R Chamber
 * \param 113_114 Standard PET Chamber
 * \returns None
 */
void AmuletSetupLinearity_menu(void){


	switch(m_iPhase){
		case PHASE_SETUP_LINEARITY_PRE_INIT:

			SetAmuletByte(92,current.language);
			if(chamber_77t()) SetAmuletByte(93, 0x01);
			else SetAmuletByte(93, 0x00);

			send_amulet_message(L_SETUP_LINEARITY,110);    // "Setup Linearity"

			if(chamber_C()) send_amulet_message(L_STANDARD_C_CHAMBER,111);    // "Standard HR Chamber"
			else if(chamber_K()) send_amulet_message(L_STANDARD_K_CHAMBER,111);    // "Standard 1K Chamber"
			else send_amulet_message(L_STANDARD_R_CHAMBER,111);    // "Standard R Chamber"

			send_amulet_message(L_STANDARD_PET_CHAMBER,112);    // "Standard PET Chamber"


			SetAmuletByte(102, 0xFF);

			m_iPhase = PHASE_SETUP_LINEARITY_WAIT;
			
			break;
		case PHASE_SETUP_LINEARITY_WAIT:
			break;
	}
}
