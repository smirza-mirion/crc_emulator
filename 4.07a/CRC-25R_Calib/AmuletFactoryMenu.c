/**
 * \file
 * \details This file handles calls from the Amulet Factory Setup Screen
 */
#define PHASE_FACTORY_PRE_INIT	0
#define PHASE_FACTORY_WAIT		1
#define PHASE_FACTORY_INITEE	2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "mca.h"
#include "database.h"
#include "ff.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void ee_init(void);
//void disable_all_interrupts(void);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);

/**
 * \details Handles the Amulet Screen Factory.htm. Factory.htm setups the factory settings.
 * \param Amulet_Byte_ID Description
 * \param 100 Shows chamber button
 * \param 103 Show screen
 * \param Amulet_String_ID Description
 * \param 100 Branding
 * \returns None
 */
void AmuletFactory_menu(void){

	switch(m_iPhase){
		case PHASE_FACTORY_PRE_INIT:
			if(m_ucClear==36){
				m_ucClear = 0;
			}

			if(current.num_chambers > 0) SetAmuletByte(100, 0xFF);

			if(current.default_keV == 0) SetAmuletString(101, "No");
			else if(current.default_keV == 1) SetAmuletString(101, "Yes");
			else SetAmuletString(101, "");

			if((Mca_installedDetector == DET_WELL) || (Mca_installedDetector == DET_BETA)) SetAmuletByte(101, 0xFF);
			else if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)) SetAmuletByte(102, 0xFF);

			if(current.branding == 0) SetAmuletString(100, "CRC-55t");
			else if(current.branding == 1) SetAmuletString(100, "CAPRAC-t");
			else if(current.branding == 2) SetAmuletString(100, "CAPTUS-700t");
			else if(current.branding == 3) SetAmuletString(100, "CRC-77t");
			else SetAmuletString(100, "");

			SetAmuletByte(103, 0xFF);
			m_iPhase = PHASE_FACTORY_WAIT;
			break;

		case PHASE_FACTORY_WAIT:
			break;

		case PHASE_FACTORY_INITEE:
			ee_init();
			//disable_all_interrupts();
			//for(;;);
			Amulet_DisplayNotification("EE Initialization", "Stored values have been initialized. Please restart.", FALSE);
			break;
	}
}
