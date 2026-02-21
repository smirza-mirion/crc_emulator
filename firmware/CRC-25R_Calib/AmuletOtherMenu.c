/**
 * \file
 * \details This file handles calls from the Amulet Advanced Setup Screen
 */
#define PHASE_OTHER_PRE_INIT	0
#define PHASE_OTHER_WAIT		1
#define PHASE_OTHER_EEINIT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "message.h"
#include "chambfac.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);

extern int m_iPhase;
extern CURRENT current;

void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);
void ee_init(void);

/**
 * \details Handles the Amulet Screen Other.htm Other.htm shows the advanced setup options.
 * \param Amulet_Byte_ID Description
 * \param 92 Language (STATE) English = 0, French = 1
 * \param 93 CRC-77t (STATE) Not CRC-77t = 0, CRC-77t = 1
 * \param String
 * \param 109_110 Title
 * \param 111 Setup Sources
 * \param 112 Setup Moly
 * \param 113 Setup Nuclide
 * \param 114 Setup CalNum
 * \param 115 Setup Linearity
 * \param 116 Setup Remote
 * \param 117_118 Setup R Hotkeys
 * \param 119_120 Setup PET Hotkeys
 * \param 121_122 Setup 1 Atm Hotkeys
 * \param 130 Future Date Entry Mode
 * \param 131_132 Dose Decay Entry
 * \param 133 Setup Key
 * \param 134 Setup Password
 * \returns None
 */
void AmuletOther_menu(void){	
	switch(m_iPhase){
		case PHASE_OTHER_PRE_INIT:

			SetAmuletByte(92,current.language);
			
			if(chamber_77t()) SetAmuletByte(93, 1);
			else SetAmuletByte(93, 0);

			if(current.future_date_input) send_amulet_message(L_FULL,130);    // "Full"
				//SetAmuletString(130, "Full");
			else send_amulet_message(L_QUICK,130);  //SetAmuletString(130, "Quick");    // "Quick"

			send_amulet_message(L_DOSE_DECAY_ENTRY,131);    // "Dose Decay Entry:"

			send_amulet_message(L_ADVANCED_CHAMBER_SETUP_TITLE,109);    // "Advanced Chamber Setup"

			send_amulet_message(L_SETUP_SOURCES,111);    // "Setup Sources"
			
			send_amulet_message(L_SETUP_MOLY,112);    // "Setup Moly"
			
			send_amulet_message(L_SETUP_NUCLIDES,113);    // "Setup Nuclide"
			
			send_amulet_message(L_SETUP_CALNUM,114);    // "Setup CalNum"
			
			send_amulet_message(L_SETUP_LINEARITY,115);    // "Setup Linearity"
			
			send_amulet_message(L_SETUP_REMOTE,116);    // "Setup Remote"
			
			send_amulet_message(L_SETUP_R_HOTKEYS,117);    // "Setup R Chamber Hotkeys"
			
			send_amulet_message(L_SETUP_PET_HOTKEYS,119);    // "Setup PET Chamber Hotkeys"

			send_amulet_message(L_SETUP_HOTKEYS, 121);    // "Setup Hotkeys"

			send_amulet_message(L_SETUP_KEY, 133);    // "Setup Key"
			
			send_amulet_message(L_SETUP_PASSWORD, 134);    // "Setup Password"

			SetAmuletByte(102, 0xFF);
		
			m_iPhase = PHASE_OTHER_WAIT;
			break;

		case PHASE_OTHER_WAIT:
			break;

		case PHASE_OTHER_EEINIT:
			ee_init();
			Amulet_DisplayNotification("EE Initialization", "Stored values have been initialized. Please restart.", FALSE);
			m_iPhase = PHASE_OTHER_WAIT;
			break;
	}
}
